# Object reference (GD 2.2081)

Verified in game through the bridge. Look up anything else with
`tools/gd.sh find <name>` before guessing.

## Object keys

| Key | Meaning |
|---|---|
| 1 | object ID |
| 2, 3 | x, y (level coordinates, one block = 30) |
| 6 | rotation (degrees) |
| 21 / 22 | main / detail color channel |
| 24 | Z layer |
| 25 | Z order |
| 32 | scale |
| 57 | groups (`.` separated) |

## Color channels

| Channel | Color |
|---|---|
| 1010 | black |
| 1011 | white |
| 1004 | object (OBJ) |

## Objects

| ID | What |
|---|---|
| 211 | plain square, fills a block cell fully (black fill with `21,1010`) |
| 468 | thin edge line (rotate 6,-90 / -270 for walls) |
| 469 | edge corner line |
| 1519 | tiny 4-point sparkle |
| 1701 | 4-point star, white, ~1.3 blocks at scale 1 |
| 1702 | 8-point star / sun |

## Gotchas

- Grid cell centers are at 15 + 30·n.
- Pasted objects come in deselected by default (`/add?deselect=0` keeps them selected).
- Crop screenshots to the area that changed; a 768px crop is far cheaper than a full screen.
