# Object reference (GD 2.2081)

Verified in game through the bridge. Look up anything else with
`tools/gd.sh find <name>` (sprite names: `d_` = decoration) before guessing.

## Object keys

| Key | Meaning |
|---|---|
| 1 | object ID |
| 2, 3 | x, y (level coordinates, one block = 30) |
| 6 | rotation (degrees) |
| 21 / 22 | main / detail color channel |
| 24 | Z layer (9 = top layer, above block fills) |
| 25 | Z order |
| 32, 128, 129 | scale, scale X, scale Y (set all three) |
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
| 211 | plain square, fills a block cell fully (black fill: `21,1010,25,-5`) |
| 468 / 469 | edge line / edge corner line |
| 470 / 471 | outline cap / outline side (1-wide column) |
| 53 | diamond, ~1 block. Star = two diamonds 128,0.12,129,0.4 crossed (second with 6,90) |
| 1764 | small dot, star dust |
| 1596, 1597 | small skulls, ~0.6 block |
| 150 | big X cross, ~1.7 blocks |
| 236 / 51 | ring big / ring small |
| 50 | filled ball |
| 52 | heart |
| 3088 | crystal cluster, ~1 block — needs `24,9` to show above fills |
| 3086 | two crystals |
| 132 | arrow |

## Hazards — never use as decoration

| ID | What |
|---|---|
| 1701–1703 | bladeTrap (look like stars, kill the player) |
| 1705–1709 | sawblades / dark blades |

## Gotchas

- Grid cell centers are at 15 + 30·n.
- Pasted objects come in deselected by default (`/add?deselect=0` keeps them selected).
- Crop screenshots to the area that changed; the crop must be on screen (POST /view first).
