#!/usr/bin/env bash
# Short client for the Decor Bridge mod. Coordinates are level coordinates.
#   gd.sh state
#   gd.sh find star,glow                      object IDs by sprite name
#   gd.sh objs [x0 y0 x1 y1]                  'uid id x y' lines (all if omitted)
#   gd.sh full x0 y0 x1 y1                    JSON with save strings, keep the rect small
#   gd.sh sel                                 selected objects
#   gd.sh add 'objstr' | gd.sh add < file     one object per line or ';'
#   gd.sh del uid,uid,...
#   gd.sh view x y [zoom]
#   gd.sh area                                rect the user marked (hold Tab + drag in the editor)
#   gd.sh area x0 y0 x1 y1 | gd.sh area clear
#   gd.sh shot [x0 y0 x1 y1] [maxsize]        prints the PNG path (default 400px)
#   gd.sh shot area [maxsize]                 screenshot of the marked area
#   gd.sh fill x0 y0 x1 y1 [channel]          fill block cells with plain squares (1010 black)
#   gd.sh scatter theme x0 y0 x1 y1 [n] [margin]   random white deco (themes: spooky, space)
#   gd.sh undo | level | help
# GEN=1 prints the object string of fill/scatter instead of adding it.
set -euo pipefail
B="http://127.0.0.1:${GD_BRIDGE_PORT:-7341}"
c() { curl -s -m 20 -H "X-GD-Bridge: 1" "$@"; }
send() { if [ "${GEN:-0}" = 1 ]; then echo "$1"; else c --data-binary "$1" "$B/add" | cut -c1-60; fi; }

# White decoration on the top Z layer.
W="21,1011,24,9,25,3"
obj() { echo -n "1,$1,2,$2,3,$3,6,${5:-0},32,$4,128,$4,129,$4,$W;"; }
star() { echo -n "1,53,2,$1,3,$2,128,$3,129,$4,$W;1,53,2,$1,3,$2,6,90,128,$3,129,$4,$W;"; }
rnd() { awk -v a="$1" -v b="$2" -v r="$RANDOM" 'BEGIN { printf "%.1f", a + (b - a) * r / 32767 }'; }

# piece <theme> <x> <y>: one random decoration (size noted as rough radius).
piece() {
  local t=$1 x=$2 y=$3 k=$((RANDOM % 10)) rot=$((RANDOM % 41 - 20))
  case "$t" in
    spooky) case $k in
      0|1) obj 1596 "$x" "$y" 0.8 "$rot" ;;
      2) obj 1597 "$x" "$y" 0.7 "$rot" ;;
      3|4) star "$x" "$y" 0.1 0.33 ;;
      5) obj 150 "$x" "$y" 0.18 "$rot" ;;
      6) obj 236 "$x" "$y" 0.16 ;;
      *) obj 1764 "$x" "$y" 0.6 ;;
    esac ;;
    space) case $k in
      0|1|2) star "$x" "$y" 0.1 0.33 ;;
      3) star "$x" "$y" 0.14 0.5 ;;
      4) obj 236 "$x" "$y" 0.2 ;;
      5) obj 50 "$x" "$y" 0.3 ;;
      *) obj 1764 "$x" "$y" 0.6 ;;
    esac ;;
    *) echo "unknown theme $t" >&2; exit 1 ;;
  esac
}

cmd="${1:-help}"; shift || true
case "$cmd" in
  state) c "$B/state" ;;
  find)  c "$B/catalog?q=${1:-}&limit=${2:-300}" ;;
  objs)  if [ $# -ge 4 ]; then c "$B/objects?x0=$1&y0=$2&x1=$3&y1=$4"; else c "$B/objects"; fi ;;
  full)  c "$B/objects?x0=$1&y0=$2&x1=$3&y1=$4&full=1" ;;
  sel)   c "$B/objects?selected=1" ;;
  add)   if [ $# -ge 1 ]; then c --data-binary "$1" "$B/add"; else c --data-binary @- "$B/add"; fi ;;
  del)   c --data-binary "$1" "$B/delete" ;;
  view)  c -X POST "$B/view?x=$1&y=$2${3:+&zoom=$3}" ;;
  area)  if [ "${1:-}" = clear ]; then c -X POST "$B/area?clear=1"
         elif [ $# -ge 4 ]; then c -X POST "$B/area?x0=$1&y0=$2&x1=$3&y1=$4"
         else c "$B/area"; fi ;;
  shot)  if [ "${1:-}" = area ]; then
           r=$(c "$B/area" | sed -n 's/.*"x0":\([-0-9.e]*\),"y0":\([-0-9.e]*\),"x1":\([-0-9.e]*\),"y1":\([-0-9.e]*\).*/\1 \2 \3 \4/p')
           [ -n "$r" ] || { echo "no area marked" >&2; exit 1; }
           set -- $r "${2:-400}"
         fi
         if [ $# -ge 4 ]; then q="x0=$1&y0=$2&x1=$3&y1=$4&maxsize=${5:-400}"; else q="maxsize=${1:-400}"; fi
         c "$B/screenshot?$q" | sed -n 's/.*"path":"\([^"]*\)".*/\1/p' | sed 's/\\\\/\\/g' ;;
  fill)  S=""; ch=${5:-1010}
         for ((x = ${1%.*} + 15; x < ${3%.*}; x += 30)); do
           for ((y = ${2%.*} + 15; y < ${4%.*}; y += 30)); do S="${S}1,211,2,$x,3,$y,21,$ch,25,-5;"; done
         done
         send "$S" ;;
  scatter)
         t=$1 x0=$2 y0=$3 x1=$4 y1=$5 n=${6:-12} m=${7:-6}
         S=""; xs=(); ys=()
         for ((i = 0, tries = 0; i < n && tries < n * 30; tries++)); do
           x=$(rnd $((x0 + m)) $((x1 - m))); y=$(rnd $((y0 + m)) $((y1 - m)))
           ok=1
           for j in "${!xs[@]}"; do
             if awk -v a="$x" -v b="$y" -v c="${xs[$j]}" -v d="${ys[$j]}" 'BEGIN { exit !((a-c)^2 + (b-d)^2 < 100) }'; then ok=0; break; fi
           done
           [ $ok = 1 ] || continue
           xs+=("$x"); ys+=("$y"); S="${S}$(piece "$t" "$x" "$y")"; i=$((i + 1))
         done
         send "$S" ;;
  undo)  c -X POST "$B/undo" ;;
  level) c "$B/level" ;;
  *)     c "$B/help" ;;
esac
echo
