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

## Rules

- All coordinates are level coordinates (object keys 2/3), one block is 30 units.
- Don't edit while `GET /state` reports `"playtesting":true`.
- Keep changes inside the marked area unless the user says otherwise.
- `docs/object-ids.md` and `docs/catalog.md` list verified object IDs.
