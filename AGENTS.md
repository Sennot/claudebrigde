# Working with the Decor Bridge

The user has Geometry Dash open with this mod. It serves the level editor on
`http://127.0.0.1:7341`; every request needs the header `X-GD-Bridge: 1`.
`curl -s -H "X-GD-Bridge: 1" http://127.0.0.1:7341/help` lists the endpoints,
`README.md` documents them, and `tools/gd.sh` (bash) wraps them.

## "The area", "here", "this part"

The user marks a region by holding Tab in the editor and dragging. When they
refer to an area without giving coordinates, read it first:

```sh
curl -s -H "X-GD-Bridge: 1" http://127.0.0.1:7341/area
# {"ok":true,"seq":3,"area":{"x0":..,"y0":..,"x1":..,"y1":..,"width":..,"height":..,"objectCount":..}}
```

`"area":null` means nothing is marked: ask the user to mark one. `seq` grows on
every change, so a new number means the user picked a new area.

Typical loop:

1. `GET /area`, then `GET /objects?x0=&y0=&x1=&y1=` for what is already there.
2. `POST /view?x=&y=` on the area center if it is off screen, then
   `GET /screenshot?x0=&y0=&x1=&y1=` and look at the PNG.
3. Build object strings and `POST /add`; keep the returned uids so you can
   `POST /delete` them or `POST /undo` if the result looks wrong.
4. Screenshot again to check.

## Before you build

Read `docs/object-ids.md` in full before placing anything. It holds what
earlier sessions learned the hard way; `docs/catalog.md` is the full id list.
The mistakes that already happened, in short:

- Hazards are not decoration: spikes 8/39 (even scaled copies), bladeTraps
  1701–1703 (look like stars), saws 1705–1709 kill the player. Treat untested
  `pit_*` and `colorSpike` ids as hazards too.
- An undefined color channel renders white. Define it with a color trigger
  (899) left of the start first, and delete old triggers by uid, never by a
  rect that covers x = 0.
- No fake shapes: triangles from thin squares, glow from stacked rectangles or
  outlines from many small pieces look bad. Use real sprites, and ask if none fits.
- Stacked translucent glow layers turn into gray haze; blended objects draw
  under normal ones on the same Z layer.
- One `POST /add` is one undo step. Generate a build with a script, add it in
  one call, screenshot, and undo if it looks wrong.
- Call `POST /view` before a cropped screenshot, or the picture is stretched.

When you learn something new about an object or the bridge, add it to
`docs/object-ids.md` (mark it V only if you verified it in game) so the next
agent doesn't repeat it.

## Rules

- All coordinates are level coordinates (object keys 2/3), one block is 30 units.
- Don't edit while `GET /state` reports `"playtesting":true`.
- Keep changes inside the marked area unless the user says otherwise.
