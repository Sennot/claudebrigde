#include <Geode/Geode.hpp>
#include <Geode/modify/CCScheduler.hpp>

#include "http_server.hpp"
#include "png_writer.hpp"

#include <algorithm>
#include <deque>
#include <filesystem>
#include <future>
#include <mutex>

using namespace geode::prelude;

namespace bridge {
    namespace {
        using http::Request;
        using http::Response;

        // ---- main-thread job queue ------------------------------------------
        // The server thread only parses HTTP; everything touching the game runs
        // inside CCScheduler::update on the main thread.

        struct Job {
            Request request;
            std::shared_ptr<std::promise<Response>> result;
        };

        std::mutex s_jobsMutex;
        std::deque<Job> s_jobs;

        Response handle(Request const& req);

        Response enqueue(Request const& req) {
            auto result = std::make_shared<std::promise<Response>>();
            auto future = result->get_future();
            {
                std::lock_guard lock(s_jobsMutex);
                s_jobs.push_back({req, result});
            }
            if (future.wait_for(std::chrono::seconds(20)) != std::future_status::ready) {
                return {504, "application/json", R"({"ok":false,"error":"game did not answer, it may be loading"})"};
            }
            return future.get();
        }

        void drainJobs() {
            std::deque<Job> jobs;
            {
                std::lock_guard lock(s_jobsMutex);
                jobs.swap(s_jobs);
            }
            for (auto& job : jobs) {
                Response res;
                try {
                    res = handle(job.request);
                } catch (std::exception const& e) {
                    res = {500, "application/json", fmt::format(R"({{"ok":false,"error":"exception: {}"}})", e.what())};
                }
                job.result->set_value(std::move(res));
            }
        }

        // ---- JSON helpers -----------------------------------------------------

        std::string escape(std::string_view s) {
            std::string out;
            out.reserve(s.size() + 2);
            for (char c : s) {
                switch (c) {
                    case '"': out += "\\\""; break;
                    case '\\': out += "\\\\"; break;
                    case '\n': out += "\\n"; break;
                    case '\r': out += "\\r"; break;
                    case '\t': out += "\\t"; break;
                    default:
                        if (static_cast<unsigned char>(c) < 0x20) {
                            out += fmt::format("\\u{:04x}", static_cast<int>(c));
                        } else {
                            out += c;
                        }
                }
            }
            return out;
        }

        std::string quote(std::string_view s) {
            return "\"" + escape(s) + "\"";
        }

        Response ok(std::string fields = {}) {
            return {200, "application/json", fields.empty() ? R"({"ok":true})" : "{\"ok\":true," + fields + "}"};
        }

        Response fail(int status, std::string_view message) {
            return {status, "application/json", "{\"ok\":false,\"error\":" + quote(message) + "}"};
        }

        std::optional<double> number(Request const& req, char const* key) {
            auto it = req.query.find(key);
            if (it == req.query.end() || it->second.empty()) return std::nullopt;
            char* end = nullptr;
            double value = std::strtod(it->second.c_str(), &end);
            if (end == it->second.c_str()) return std::nullopt;
            return value;
        }

        bool flag(Request const& req, char const* key, bool fallback) {
            auto it = req.query.find(key);
            if (it == req.query.end()) return fallback;
            return it->second != "0" && it->second != "false";
        }

        std::vector<int> integers(std::string_view text) {
            std::vector<int> out;
            int current = 0;
            bool inNumber = false;
            for (char c : text) {
                if (c >= '0' && c <= '9') {
                    current = current * 10 + (c - '0');
                    inNumber = true;
                } else if (inNumber) {
                    out.push_back(current);
                    current = 0;
                    inNumber = false;
                }
            }
            if (inNumber) out.push_back(current);
            return out;
        }

        // ---- editor access ----------------------------------------------------

        struct Editor {
            LevelEditorLayer* layer;
            EditorUI* ui;
        };

        std::optional<Editor> editor() {
            auto layer = LevelEditorLayer::get();
            if (!layer || !layer->m_editorUI) return std::nullopt;
            return Editor{layer, layer->m_editorUI};
        }

        bool playtesting(Editor const& ed) {
            return ed.layer->m_playbackMode != PlaybackMode::Not;
        }

        // Object nodes sit this far above the coordinates GD saves (keys 2/3).
        // Everything the bridge exposes uses the saved, "level" coordinates.
        constexpr float kGroundY = 90.f;

        CCPoint levelPos(GameObject* obj) {
            auto p = obj->getPosition();
            return {p.x, p.y - kGroundY};
        }

        std::string objectJson(Editor const& ed, GameObject* obj) {
            auto pos = levelPos(obj);
            return fmt::format(
                R"({{"uid":{},"id":{},"x":{},"y":{},"str":{}}})",
                obj->m_uniqueID, obj->m_objectID, pos.x, pos.y,
                quote(std::string(obj->getSaveString(ed.layer)))
            );
        }

        std::string objectsJson(Editor const& ed, std::vector<GameObject*> const& objects) {
            std::string out = "[";
            for (size_t i = 0; i < objects.size(); i++) {
                if (i) out += ",";
                out += objectJson(ed, objects[i]);
            }
            return out + "]";
        }

        CCArray* findByUids(Editor const& ed, std::vector<int> const& uids) {
            std::unordered_set<int> wanted(uids.begin(), uids.end());
            auto found = CCArray::create();
            for (auto obj : CCArrayExt<GameObject*>(ed.layer->m_objects)) {
                if (wanted.contains(obj->m_uniqueID)) found->addObject(obj);
            }
            return found;
        }

        // Level-space rectangle currently visible on screen.
        CCRect visibleRect(Editor const& ed) {
            auto win = CCDirector::sharedDirector()->getWinSize();
            auto layer = ed.layer->m_objectLayer;
            auto a = layer->convertToNodeSpace({0.f, 0.f});
            auto b = layer->convertToNodeSpace({win.width, win.height});
            return {a.x, a.y - kGroundY, b.x - a.x, b.y - a.y};
        }

        // Screen position (points) of a level-space point.
        CCPoint levelToScreen(Editor const& ed, CCPoint level) {
            return ed.layer->m_objectLayer->convertToWorldSpace({level.x, level.y + kGroundY});
        }

        std::string songPath(GJGameLevel* level) {
            if (level->m_songID > 0) {
                return std::string(MusicDownloadManager::sharedState()->pathForSong(level->m_songID));
            }
            return std::string(level->getAudioFileName());
        }

        // ---- endpoints --------------------------------------------------------

        Response help() {
            return {200, "text/plain",
                "Decor Bridge. Every request needs the header 'X-GD-Bridge: 1'.\n"
                "All coordinates are level coordinates, as saved in object keys 2/3.\n"
                "GET  /state                       editor, level, song, view rect, counts\n"
                "GET  /objects?x0=&y0=&x1=&y1=     objects in a rect (all if omitted)\n"
                "GET  /objects?selected=1          selected objects\n"
                "     &limit=N                     default 20000\n"
                "GET  /level                       full level string (text/plain)\n"
                "GET  /catalog?q=star,glow         'id frame' lines matching any term, &limit=300\n"
                "POST /add        body: object string 'k,v,k,v;k,v,...;'  pasted in place, one undo step\n"
                "     ?deselect=0                  keep the pasted objects selected\n"
                "POST /delete     body: uids       deleted as one undo step\n"
                "POST /select     body: uids       empty body deselects\n"
                "POST /undo                        editor undo\n"
                "POST /view?x=&y=&zoom=            center the editor camera on a level point\n"
                "GET  /screenshot                  PNG of the screen, returns its path\n"
                "     ?x0=&y0=&x1=&y1=             crop to an on-screen rect\n"
                "     &maxsize=768&hideui=1        longest side in pixels, hide editor UI\n"};
        }

        Response state() {
            auto ed = editor();
            if (!ed) return ok(R"("editor":false)");

            auto level = ed->layer->m_level;
            auto view = visibleRect(*ed);
            return ok(fmt::format(
                R"("editor":true,"playtesting":{},"level":{},"songID":{},"audioTrack":{},"songPath":{},)"
                R"("objectCount":{},"selectedCount":{},"zoom":{},"view":{{"x0":{},"y0":{},"x1":{},"y1":{}}})",
                playtesting(*ed), quote(std::string(level->m_levelName)), level->m_songID, level->m_audioTrack,
                quote(songPath(level)), ed->layer->m_objects->count(), ed->ui->getSelectedObjects()->count(),
                ed->layer->m_objectLayer->getScale(), view.getMinX(), view.getMinY(), view.getMaxX(), view.getMaxY()
            ));
        }

        Response objects(Request const& req) {
            auto ed = editor();
            if (!ed) return fail(409, "editor is not open");

            size_t limit = static_cast<size_t>(number(req, "limit").value_or(20000));
            std::vector<GameObject*> result;
            bool truncated = false;

            auto collect = [&](CCArray* source, auto&& keep) {
                for (auto obj : CCArrayExt<GameObject*>(source)) {
                    if (!keep(obj)) continue;
                    if (result.size() >= limit) {
                        truncated = true;
                        break;
                    }
                    result.push_back(obj);
                }
            };

            if (flag(req, "selected", false)) {
                collect(ed->ui->getSelectedObjects(), [](GameObject*) { return true; });
            } else {
                double x0 = number(req, "x0").value_or(-1e9), y0 = number(req, "y0").value_or(-1e9);
                double x1 = number(req, "x1").value_or(1e9), y1 = number(req, "y1").value_or(1e9);
                collect(ed->layer->m_objects, [&](GameObject* obj) {
                    auto p = levelPos(obj);
                    return p.x >= x0 && p.x <= x1 && p.y >= y0 && p.y <= y1;
                });
            }

            return ok(fmt::format(
                R"("count":{},"truncated":{},"objects":{})", result.size(), truncated, objectsJson(*ed, result)
            ));
        }

        std::string lowercase(std::string s) {
            std::ranges::transform(s, s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return s;
        }

        // Object ID -> sprite frame name, filtered by comma-separated substrings.
        Response catalog(Request const& req) {
            std::vector<std::string> terms;
            if (auto it = req.query.find("q"); it != req.query.end()) {
                std::string_view rest = it->second;
                while (!rest.empty()) {
                    auto comma = rest.find(',');
                    if (auto term = lowercase(std::string(rest.substr(0, comma))); !term.empty()) terms.push_back(term);
                    if (comma == std::string_view::npos) break;
                    rest.remove_prefix(comma + 1);
                }
            }
            size_t limit = static_cast<size_t>(number(req, "limit").value_or(300));

            std::string out;
            size_t count = 0;
            for (auto const& [id, frame] : ObjectToolbox::sharedState()->m_allKeys) {
                std::string name(frame);
                if (!terms.empty()) {
                    auto lower = lowercase(name);
                    if (std::ranges::none_of(terms, [&](auto const& t) { return lower.find(t) != std::string::npos; })) continue;
                }
                if (count++ >= limit) break;
                out += std::to_string(id) + " " + name + "\n";
            }
            return {200, "text/plain", out};
        }

        Response levelString() {
            auto ed = editor();
            if (!ed) return fail(409, "editor is not open");
            return {200, "text/plain", std::string(ed->layer->getLevelString())};
        }

        Response add(Request const& req) {
            auto ed = editor();
            if (!ed) return fail(409, "editor is not open");
            if (playtesting(*ed)) return fail(409, "stop the playtest first");

            // Accept one object per line as well as the native ';' separator.
            std::string str;
            for (char c : req.body) {
                if (c == '\r') continue;
                str += c == '\n' ? ';' : c;
            }
            if (str.find_first_not_of("; \t") == std::string::npos) return fail(400, "empty object string");

            auto pasted = ed->ui->pasteObjects(gd::string(str), true, false);
            // Pasted objects come in selected; the highlight hides their colors.
            if (flag(req, "deselect", true)) ed->ui->deselectAll();
            ed->ui->updateButtons();
            ed->ui->updateObjectInfoLabel();

            std::string uids;
            int count = 0;
            if (pasted) {
                for (auto obj : CCArrayExt<GameObject*>(pasted)) {
                    if (count++) uids += ",";
                    uids += std::to_string(obj->m_uniqueID);
                }
            }
            log::info("added {} objects", count);
            return ok(fmt::format(R"("added":{},"uids":[{}])", count, uids));
        }

        Response deleteObjects(Request const& req) {
            auto ed = editor();
            if (!ed) return fail(409, "editor is not open");
            if (playtesting(*ed)) return fail(409, "stop the playtest first");

            auto found = findByUids(*ed, integers(req.body));
            if (found->count() == 0) return ok(R"("deleted":0)");

            ed->ui->deselectAll();
            ed->ui->selectObjects(found, true);
            ed->ui->onDeleteSelected(nullptr);
            ed->ui->updateButtons();
            ed->ui->updateObjectInfoLabel();
            return ok(fmt::format(R"("deleted":{})", found->count()));
        }

        Response selectObjects(Request const& req) {
            auto ed = editor();
            if (!ed) return fail(409, "editor is not open");

            auto found = findByUids(*ed, integers(req.body));
            ed->ui->deselectAll();
            if (found->count()) ed->ui->selectObjects(found, true);
            ed->ui->updateButtons();
            ed->ui->updateObjectInfoLabel();
            return ok(fmt::format(R"("selected":{})", found->count()));
        }

        Response undo() {
            auto ed = editor();
            if (!ed) return fail(409, "editor is not open");
            if (playtesting(*ed)) return fail(409, "stop the playtest first");
            ed->ui->undoLastAction(nullptr);
            return ok();
        }

        Response view(Request const& req) {
            auto ed = editor();
            if (!ed) return fail(409, "editor is not open");

            if (auto zoom = number(req, "zoom")) ed->ui->updateZoom(static_cast<float>(*zoom));

            auto x = number(req, "x"), y = number(req, "y");
            if (x || y) {
                auto layer = ed->layer->m_objectLayer;
                auto current = visibleRect(*ed);
                CCPoint target{
                    static_cast<float>(x.value_or(current.getMidX())),
                    static_cast<float>(y.value_or(current.getMidY()))
                };
                auto win = CCDirector::sharedDirector()->getWinSize();
                auto onScreen = levelToScreen(*ed, target);
                layer->setPosition(layer->getPosition() + (ccp(win.width / 2, win.height / 2) - onScreen));
            }

            auto rect = visibleRect(*ed);
            return ok(fmt::format(
                R"("zoom":{},"view":{{"x0":{},"y0":{},"x1":{},"y1":{}}})", ed->layer->m_objectLayer->getScale(),
                rect.getMinX(), rect.getMinY(), rect.getMaxX(), rect.getMaxY()
            ));
        }

        Response screenshot(Request const& req) {
            auto director = CCDirector::sharedDirector();
            auto scene = director->getRunningScene();
            if (!scene) return fail(409, "no running scene");

            auto dir = Mod::get()->getSaveDir() / "screenshots";
            std::error_code ec;
            std::filesystem::create_directories(dir, ec);
            static int s_index = 0;
            auto path = dir / fmt::format("shot_{}.png", s_index++ % 10);

            auto win = director->getWinSize();
            auto texture = CCRenderTexture::create(static_cast<int>(win.width), static_cast<int>(win.height));
            if (!texture) return fail(500, "could not create render texture");

            auto ed = editor();
            bool hideUI = ed && flag(req, "hideui", true);
            bool uiWasVisible = ed && ed->ui->isVisible();
            if (hideUI) ed->ui->setVisible(false);

            texture->beginWithClear(0.f, 0.f, 0.f, 1.f);
            scene->visit();
            texture->end();

            if (hideUI) ed->ui->setVisible(uiWasVisible);

            auto image = texture->newCCImage(true);
            if (!image) return fail(500, "could not read render texture");
            int imgW = image->getWidth(), imgH = image->getHeight();
            int bpp = image->hasAlpha() ? 4 : 3;
            auto data = image->getData();

            // Crop to a level-space rect if one is given (it must be on screen).
            int cx0 = 0, cy0 = 0, cx1 = imgW, cy1 = imgH;
            bool clipped = false;
            auto x0 = number(req, "x0"), y0 = number(req, "y0"), x1 = number(req, "x1"), y1 = number(req, "y1");
            if (ed && x0 && y0 && x1 && y1) {
                auto a = levelToScreen(*ed, {static_cast<float>(std::min(*x0, *x1)), static_cast<float>(std::min(*y0, *y1))});
                auto b = levelToScreen(*ed, {static_cast<float>(std::max(*x0, *x1)), static_cast<float>(std::max(*y0, *y1))});
                float sx = imgW / win.width, sy = imgH / win.height;
                int rx0 = static_cast<int>(a.x * sx), rx1 = static_cast<int>(b.x * sx);
                int ry0 = imgH - static_cast<int>(b.y * sy), ry1 = imgH - static_cast<int>(a.y * sy);
                cx0 = std::clamp(rx0, 0, imgW);
                cx1 = std::clamp(rx1, 0, imgW);
                cy0 = std::clamp(ry0, 0, imgH);
                cy1 = std::clamp(ry1, 0, imgH);
                clipped = cx0 != rx0 || cx1 != rx1 || cy0 != ry0 || cy1 != ry1;
                if (cx1 - cx0 < 2 || cy1 - cy0 < 2) {
                    image->release();
                    return fail(400, "region is not on screen, POST /view first");
                }
            }

            // Area-average downscale so the longest side is at most maxsize.
            int cropW = cx1 - cx0, cropH = cy1 - cy0;
            int maxSize = static_cast<int>(number(req, "maxsize").value_or(768));
            double scale = std::min(1.0, static_cast<double>(std::max(maxSize, 16)) / std::max(cropW, cropH));
            int outW = std::max(1, static_cast<int>(cropW * scale)), outH = std::max(1, static_cast<int>(cropH * scale));

            std::vector<uint8_t> rgb(static_cast<size_t>(outW) * outH * 3);
            for (int oy = 0; oy < outH; oy++) {
                int sy0 = cy0 + oy * cropH / outH, sy1 = std::max(sy0 + 1, cy0 + (oy + 1) * cropH / outH);
                for (int ox = 0; ox < outW; ox++) {
                    int sx0 = cx0 + ox * cropW / outW, sx1 = std::max(sx0 + 1, cx0 + (ox + 1) * cropW / outW);
                    uint32_t sum[3] = {0, 0, 0}, n = 0;
                    for (int y = sy0; y < sy1; y++) {
                        auto row = data + (static_cast<size_t>(y) * imgW + sx0) * bpp;
                        for (int x = sx0; x < sx1; x++, row += bpp, n++) {
                            sum[0] += row[0];
                            sum[1] += row[1];
                            sum[2] += row[2];
                        }
                    }
                    auto out = &rgb[(static_cast<size_t>(oy) * outW + ox) * 3];
                    for (int c = 0; c < 3; c++) out[c] = static_cast<uint8_t>(sum[c] / n);
                }
            }
            image->release();

            if (!png::writeRGB(path.string(), rgb, outW, outH)) return fail(500, "could not save png");

            std::string fields = fmt::format(
                R"("path":{},"width":{},"height":{},"clipped":{})", quote(path.string()), outW, outH, clipped
            );
            if (ed) {
                auto rect = visibleRect(*ed);
                fields += fmt::format(
                    R"(,"view":{{"x0":{},"y0":{},"x1":{},"y1":{}}})",
                    rect.getMinX(), rect.getMinY(), rect.getMaxX(), rect.getMaxY()
                );
            }
            return ok(fields);
        }

        Response handle(Request const& req) {
            auto const& p = req.path;
            bool get = req.method == "GET", post = req.method == "POST";

            if (get && (p == "/" || p == "/help")) return help();
            if (get && p == "/state") return state();
            if (get && p == "/objects") return objects(req);
            if (get && p == "/level") return levelString();
            if (get && p == "/catalog") return catalog(req);
            if (get && p == "/screenshot") return screenshot(req);
            if (post && p == "/add") return add(req);
            if (post && p == "/delete") return deleteObjects(req);
            if (post && p == "/select") return selectObjects(req);
            if (post && p == "/undo") return undo();
            if (post && p == "/view") return view(req);
            return fail(404, "unknown endpoint, see GET /help");
        }

        void startServer() {
            auto mod = Mod::get();
            if (!mod->getSettingValue<bool>("enabled")) return;
            auto port = static_cast<unsigned short>(mod->getSettingValue<int64_t>("port"));
            auto error = http::start(port, &enqueue);
            if (error.empty()) {
                log::info("listening on http://127.0.0.1:{}", port);
            } else {
                log::error("could not start: {}", error);
            }
        }
    }
}

class $modify(BridgeScheduler, CCScheduler) {
    void update(float dt) {
        CCScheduler::update(dt);

        static bool s_started = false;
        if (!s_started) {
            s_started = true;
            bridge::startServer();
        }
        bridge::drainJobs();
    }
};
