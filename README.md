# Decor Bridge

Geode mod (GD 2.2081, Windows) that exposes the level editor over a local HTTP
server so an AI assistant can read the level, place decoration and look at the
result while you watch.

The server listens on `127.0.0.1:7341` only. Every request must send the header
`X-GD-Bridge: 1`; requests with an `Origin` header (web pages) are refused.

```sh
curl -s -H "X-GD-Bridge: 1" http://127.0.0.1:7341/state
```

| Method | Path | Description |
|---|---|---|
| GET | `/state` | Editor open, playtesting, level name, song, object counts, zoom, visible rect |
| GET | `/objects?x0=&y0=&x1=&y1=` | Objects in a level-space rect (all if omitted), `&limit=N` |
| GET | `/objects?selected=1` | Selected objects |
| GET | `/level` | Full level string (plain text) |
| GET | `/catalog?q=star,glow` | `id frame` lines whose sprite name matches any term |
| POST | `/add` | Body: GD object string (`1,1,2,15,3,15;...`, `;` or newline separated). Pasted in place as one undo step and deselected (`?deselect=0` keeps them selected), returns new uids |
| POST | `/delete` | Body: uids. Deleted as one undo step |
| POST | `/select` | Body: uids; empty body deselects |
| POST | `/undo` | Editor undo |
| POST | `/view?x=&y=&zoom=` | Centers the camera on a level point |
| GET | `/screenshot?x0=&y0=&x1=&y1=&maxsize=768` | PNG of the screen, optionally cropped to an on-screen rect and downscaled; returns its path |

All coordinates are level coordinates, as saved in object keys 2/3.
`tools/gd.sh` wraps these calls; `docs/object-ids.md` lists verified object IDs.

Objects are returned as `{"uid","id","x","y","str"}` where `str` is the
object's full save string (groups, colors, trigger settings, ...).

Builds run on GitHub Actions (`.github/workflows/build.yml`).
