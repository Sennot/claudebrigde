#!/usr/bin/env bash
# Short client for the Decor Bridge mod. Coordinates are level coordinates.
#   gd.sh state
#   gd.sh find star,glow            object IDs by sprite name
#   gd.sh objs [x0 y0 x1 y1]        objects in a rect (all if omitted)
#   gd.sh sel                       selected objects
#   gd.sh add 'objstr' | gd.sh add < file   (one object per line or ';')
#   gd.sh del uid,uid,...
#   gd.sh view x y [zoom]
#   gd.sh shot [x0 y0 x1 y1] [maxsize]       prints the PNG path
#   gd.sh undo | level | help
set -euo pipefail
B="http://127.0.0.1:${GD_BRIDGE_PORT:-7341}"
c() { curl -s -m 20 -H "X-GD-Bridge: 1" "$@"; }

cmd="${1:-help}"; shift || true
case "$cmd" in
  state) c "$B/state" ;;
  find)  c "$B/catalog?q=${1:-}&limit=${2:-300}" ;;
  objs)  if [ $# -ge 4 ]; then c "$B/objects?x0=$1&y0=$2&x1=$3&y1=$4"; else c "$B/objects"; fi ;;
  sel)   c "$B/objects?selected=1" ;;
  add)   if [ $# -ge 1 ]; then c --data-binary "$1" "$B/add"; else c --data-binary @- "$B/add"; fi ;;
  del)   c --data-binary "$1" "$B/delete" ;;
  view)  c -X POST "$B/view?x=$1&y=$2${3:+&zoom=$3}" ;;
  shot)  if [ $# -ge 4 ]; then q="x0=$1&y0=$2&x1=$3&y1=$4&maxsize=${5:-768}"; else q="maxsize=${1:-768}"; fi
         c "$B/screenshot?$q" | sed -n 's/.*"path":"\([^"]*\)".*/\1/p' | sed 's/\\\\/\\/g' ;;
  undo)  c -X POST "$B/undo" ;;
  level) c "$B/level" ;;
  *)     c "$B/help" ;;
esac
echo
