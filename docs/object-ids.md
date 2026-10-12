# Object reference (GD 2.2081)

**V** = verified in game through the bridge, **?** = not verified, test on one object first.
Full id list: `docs/catalog.md`. Look up names with `tools/gd.sh find <name>` (`d_` = decoration).

## Decoration concept (read first)

Do not copy a reference pixel for pixel. Take mood, palette, depth and the kinds of shapes.
Use the game's own sprites the way an editor decorator would, not squares and slices.

- Use real sprites from the catalog (chains, gears, crystals, thorns, arrows, orbs, gradients, rings). Repeat, rotate, scale.
- Depth in layers: dim background, the structure, then detail and light in front. One channel per layer.
- Color from channels, light from blending. Glow is a blended sprite, not stacked rectangles.
- Triggers first: background/ground color, pulses, gradients, alpha/move, shaders.
- Rhythm: repeat details with spacing, a few focal points, leave empty space.
- Keep the original block silhouette visible; decorate inside and around it.
- If a shape has no real sprite, say so and ask instead of faking it.
- Workflow: write mood + 3-5 colors + what repeats; test candidate sprites on an empty spot; confirm direction in one line; build.

What looks bad (V): triangles/slopes from thin squares (steps), glow from overlapping rectangles (blotches), outlines from many small pieces (seams), stacked translucent glow/gradient layers (gray haze where they cross).

## Object keys

| Key | Meaning |
|---|---|
| 1 | object ID |
| 2, 3 | x, y (one block = 30; cell centers at 15 + 30·n) |
| 4 / 5 | flipX / flipY (applied before rotation) |
| 6 | rotation (degrees) |
| 21 / 22 | main / detail color channel |
| 24 | Z layer: 11, 9 (top), 7, 5, 3, 1, -1, -3 |
| 25 | Z order within the layer |
| 32, 128, 129 | scale, scale X, scale Y (set all three) |
| 57 | groups (`.` separated) |
| 135 | **Hide**: invisible, hitbox stays. V |
| 134 | passable ? |
| 96 | disable glow ? |
| 64 / 67 | don't fade / don't enter ? |

To decorate something with a hitbox: copy its save string with `135,1`, delete the original, add the copy, draw decor on top. The bridge has no edit call. Keep a backup of the original strings.

## Colors

Channels: 1010 black, 1011 white, 1004 object, 1000 background, 1001 ground, 1002 ground line. Use 20+ for your own. An **undefined channel renders white**. V

Color trigger 899, placed left of the start (x = -60) so the editor applies it, one per channel:

```
1,899,2,-60,3,<y>,7,<R>,8,<G>,9,<B>,10,0,23,<channel>,35,1,17,<0|1>
```

23 target channel, 17 blending, 10 duration. Re-adding does not replace the old one: delete by uid. V

Blending (V): additive, dark vanishes, bright glows. Blending objects draw **under** normal objects on the same Z layer, so glow over other decor goes on a higher layer. A blended copy of a spike turns it white; keep glow on separate objects. Channel opacity (key 35) does not soften glow, make the color darker.

## Objects

| ID | What |
|---|---|
| 211 | `lightsquare_color`, plain square, no hitbox, takes any channel. V |
| 468 | one edge, centered on a cell boundary |
| 469 / 470 | corner / cap |
| 471 | at a cell center draws both side edges of a 1-wide column (rot 0 vertical, 90 horizontal) |
| 349 / 351 | triangle slopes 1×1 and 2×1; base 349: hypotenuse bottom-left to top-right, solid bottom-right. V |
| 1744 | 2×1 solid slope, right angle bottom right, grid pattern |
| 50 | filled ball, diameter 30 |
| 51 / 236 | ring small / big |
| 53 | diamond ~1 block |
| 52, 132, 150 | heart, arrow, X cross (~1.7 blocks) |
| 1596, 1597 | small skulls ~0.6 block |
| 1764 | small dot |
| 505 | `d_gradient`, 30×30, bright at bottom edge, fades to transparent at top; stretch with 128/129, blend with a dim channel. V |
| 503, 504, 1011, 1291, 1292 | other gradients |
| 1838 | `d_scaleFadeRing`, soft ring, glow/halo at scale 3-5.5 |
| 129–132 | `d_cloud`, soft fog when blended |
| 3086 / 3088 | crystals; 3088 needs `24,9` above fills |
| 85 | cogwheel, ~70 across |
| 1734 / 1735 / 1736 | saws ~78 / 52 / 38 across |
| 1754 | `gridLine`, scaled up gives tall light streaks |

## Hazards (real hitboxes, never as decoration)

| ID | What |
|---|---|
| 8, 39 | spikes. V. Copies (scaled too) kill the player |
| 1701–1703 | bladeTrap, look like stars |
| 1705–1709 | sawblades / dark blades |

Not verified, treat as hazards until tested: `pit_*` (1716–1733), `colorSpike` (216–218, 458).
`lighttriangle` 687–702 ignores the color channel and draws black (V for 688, ~60×30); decoration, hitbox not tested.

## Triggers

Triggers left of the level start (x < 0) are applied in the editor; elsewhere not tested.

Pulse 1006: `45` fade in, `46` hold, `47` fade out, `51` target channel, `52,0` channel mode, `7/8/9` color. The editor shows the last pulse color frozen, so pulses at or left of the start wash the view out. Keep pulse colors near the base palette, place them at x > 0. To judge the base look, export the pulses (`/objects?full=1`, id 1006), delete them, look, re-add. Never delete color triggers by a rectangle that covers x = 0, delete by uid.

## Structures

- Straight structures: build an edge grid from 468/471, flood-fill from outside, fill the inside with dark rectangles per row-run, inward 505 gradient along every exposed side.
- Slope walls: hide the slope tiles (`135,1`), merge touching tiles into runs along the hypotenuse, per run draw a dark band (211), a thin bright edge (211, blended) and an inner bevel (505). Per-tile glow reads as a dashed lattice or noisy snow.
- Light on layered depth: many thin layers (-3 to 9), each its own channel; gradients tiled per cell on several layers; far layers get big dim shapes (cogwheels, gradients).

## Bridge gotchas

- One `add` = one editor undo step; `POST /undo` n times removes n adds. Keep each build to one add.
- `gd.sh add` takes the body from stdin (`< file`). Chunk ~600 objects per call.
- Scan `/objects?full=1` in 1500-unit chunks; delete in batches of ~1500 uids from a file (`curl --data-binary @file /delete`).
- Pasted objects come in deselected (`/add?deselect=0` keeps them selected).
- Call `POST /view` first, then screenshot a crop that lies inside the visible area, otherwise the picture is stretched. Calibrate with a known size (block = 30). One small screenshot (500–800 px) per iteration.
- Generate with a script into the scratchpad, then one `delete` + one `add`.
- Speed portals: ids 200, 201, 202, 203, 1334. Units per second: 0.5x 251.16, 1x 311.58, 2x 387.42, 3x 468, 4x 576.
