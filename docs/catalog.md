# Object catalog (GD 2.2081, generated from the live game)

Source: the bridge `GET /catalog` (4092 object ids, 353 sprite families). Regenerate: `curl -s -H 'X-GD-Bridge: 1' 'http://127.0.0.1:7341/catalog?q=.png&limit=100000'`.

**What is verified and what is not.** Families and ids come from the game. The category of each family is guessed from the sprite name (`d_` = decoration, `edit_` = trigger icon, `spike`/`blade`/`saw` = hazard). Hitboxes, sizes and whether a sprite takes a color channel are **not** verified here. Test an id on an empty spot first. Verified facts live in `docs/object-ids.md` (for example: ids 8 and 39 are real spikes, `lighttriangle` 688 ignores the color channel, `ball` 50 is 30 wide).

Names ending in `_color` / `_c` are usually the layer that takes the main color channel. A family with many numbers (`block_light` 44) is a set of variants of one shape.


## Decoration sprites (`d_*`, light shapes, fake spikes, cogwheels, chains). Mostly no hitbox.

| Family | n | ids |
|---|---|---|
| `rod` | 3 | 15-17 |
| `d_spikes` | 4 | 18-21 |
| `chain` | 1 | 41 |
| `d_cloud` | 5 | 48-49, 129-131 |
| `d_ball` | 18 | 50-54, 60, 148-149, 405, 3621-3629 |
| `lightsquare` | 36 | 70, 76-78, 81, 91, 161, 207-210, 212-213, 247-250, 252-258, 260-261, 263-265, 267, 269-272, 274-275 |
| `lightsquare_sideLine` | 2 | 82, 268 |
| `d_cogwheel` | 4 | 85-87, 97 |
| `lightsquare_color` | 14 | 94, 211, 251, 259, 266, 273, 1820-1821, 1823-1828 |
| `d_chain` | 5 | 106-107, 110, 498-499 |
| `d_brick` | 3 | 113-115 |
| `d_thorn` | 12 | 123-128, 461-466 |
| `d_arrow` | 6 | 132, 460, 494, 3630-3632 |
| `d_exmark` | 2 | 133, 3633 |
| `d_art` | 2 | 134, 190 |
| `d_qmark` | 2 | 136, 3634 |
| `d_wheel` | 3 | 137-139 |
| `brick` | 1 | 143 |
| `d_cross` | 2 | 150, 3635 |
| `d_spikeart` | 3 | 151-153 |
| `d_spikewheel` | 3 | 154-156 |
| `d_wave` | 3 | 157-159 |
| `d_cartwheel` | 3 | 180-182 |
| `fakeSpike` | 8 | 191, 198-199, 393, 1889-1892 |
| `colorPlank` | 1 | 215 |
| `colorPlank_small` | 1 | 219 |
| `colorSquare_small` | 1 | 220 |
| `d_roundCloud` | 3 | 222-224 |
| `d_swirve` | 4 | 225-226, 500-501 |
| `d_bar` | 5 | 227-230, 242 |
| `d_smallbar` | 5 | 231-232, 283-285 |
| `d_square` | 7 | 233-235, 279-282 |
| `d_circle` | 4 | 236, 497, 3636-3637 |
| `d_link` | 5 | 237-241 |
| `lightsquare_brick` | 2 | 277-278 |
| `d_rotatingBall` | 4 | 375-378 |
| `d_geometric` | 3 | 394-396 |
| `d_grass` | 5 | 406-408, 414, 906 |
| `d_link_b` | 6 | 409-413, 1756 |
| `d_spikeWave` | 3 | 419-420, 767 |
| `d_pit06wave` | 2 | 448-449 |
| `d_pillar` | 3 | 450-452 |
| `d_link_c` | 5 | 453-457 |
| `d_largeSquare` | 4 | 495-496, 3638-3639 |
| `d_gradient` | 6 | 503-505, 1273-1274, 1291 |
| `d_block_piece` | 13 | 653-659, 722-724, 734-736 |
| `d_pixelArt` | 12 | 668-672, 738, 1737-1742 |
| `blackCogwheel` | 6 | 675-677, 1734-1736 |
| `lighttriangle_color` | 14 | 687-688, 693-702, 1747-1748 |
| `d_link_b_color` | 1 | 725 |
| `d_grassArt` | 4 | 907-910 |
| `d_whiteBlock` | 2 | 916-917 |
| `d_rainbow` | 2 | 925-926 |
| `d_artCloud` | 3 | 936-938 |
| `d_flower` | 1 | 939 |
| `d_grassDetail` | 3 | 940-942 |
| `d_ringSeg` | 4 | 997-1000 |
| `d_link_d` | 5 | 1001-1005 |
| `d_gradient_b` | 5 | 1009-1010, 1271-1272, 1292 |
| `d_gradient_c` | 6 | 1011-1013, 1269-1270, 1293 |
| `d_flashRing` | 3 | 1019-1021 |
| `d_animWave_base` | 3 | 1050-1052 |
| `d_animLoading` | 2 | 1053-1054 |
| `d_pickupCircle` | 3 | 1055-1057 |
| `d_spiral` | 4 | 1058-1061 |
| `d_waveBG` | 1 | 1228 |
| `d_key` | 1 | 1275 |
| `d_keyHole` | 1 | 1276 |
| `d_rotatingLine` | 4 | 1521-1524 |
| `d_rotatingSquare` | 4 | 1525-1528 |
| `d_heart` | 1 | 1587 |
| `d_heart_match` | 1 | 1588 |
| `d_potion` | 1 | 1589 |
| `d_potion_match` | 1 | 1590 |
| `d_animSquare` | 1 | 1592 |
| `d_skull` | 3 | 1596-1598 |
| `d_skull_match` | 1 | 1599 |
| `d_sign` | 1 | 1600 |
| `d_sign_pole` | 1 | 1601 |
| `d_sign_img` | 6 | 1602-1607 |
| `d_sign_paint` | 3 | 1608-1610 |
| `d_zag` | 3 | 1697-1699 |
| `d_ringSpiral` | 1 | 1752 |
| `d_small_ball` | 5 | 1764-1768 |
| `d_scaleFadeRing` | 12 | 1831-1842 |
| `d_sign_tile` | 1 | 1843 |
| `bush` | 3 | 1919-1921 |
| `d_animWave` | 3 | 3000-3002 |
| `d_time` | 1 | 3601 |

## Blocks, slopes, planks, outlines, platform sets (`block*`, `square*`, `gdh*`, ...). Usually solid or block-styled.

| Family | n | ids |
|---|---|---|
| `square` | 9 | 1-7, 83, 502 |
| `pit` | 26 | 9, 61, 243-244, 421-422, 446-447, 667, 720, 768, 989, 991, 1715, 1719-1721, 1725-1733 |
| `plank` | 16 | 40, 369-370, 769-770, 967-968, 1079-1081, 1096-1098, 1903-1905 |
| `square_b` | 12 | 62-66, 68, 1893-1898 |
| `blockOutline` | 119 | 69, 71-72, 74-75, 90, 92-93, 95-96, 119, 160, 162-163, 165-169, 289, 291, 294-295, 299, 301, 305, 307, 309, 311, 315, 317, 321, 323, 331, 333, 337, 339, 343, 345, 349, 351, 353, 355, 371-372, 467-475, 483-484, 492-493, 651-652, 661-666, 709-712, 726-729, … (119 ids, first 69, last 2866) |
| `square_c` | 1 | 73 |
| `square_d` | 1 | 80 |
| `square_f` | 6 | 116-118, 120-122 |
| `pit_b` | 5 | 135, 1711-1714 |
| `invis_square` | 1 | 146 |
| `invis_plank` | 1 | 147 |
| `square_g` | 17 | 164, 193, 737, 1140-1153 |
| `square_h` | 10 | 170-176, 192, 194, 197 |
| `square_small` | 2 | 195, 1910 |
| `plank_small` | 2 | 196, 1911 |
| `invis_plank_small` | 1 | 204 |
| `invis_square_small` | 1 | 206 |
| `square_f_brick` | 2 | 245-246 |
| `triangle_b_square` | 4 | 296-297, 1901-1902 |
| `triangle_g_square` | 3 | 324-325, 358 |
| `triangle_h` | 2 | 326-327 |
| `triangle_h_square` | 2 | 328-329 |
| `pit_slope` | 8 | 363-364, 366-367, 1717-1718, 1723-1724 |
| `pit_low` | 4 | 365, 368, 1716, 1722 |
| `plank_square` | 4 | 373-374, 1908-1909 |
| `block` | 223 | 476-482, 485-491, 752-759, 807-825, 841-848, 850, 853-857, 859, 861-863, 867-872, 880-885, 890-891, 893-894, 903-905, 911, 927-930, 932-934, 952-959, 974-977, 1045-1048, 1062, 1082-1090, 1159-1186, 1239-1240, 1251-1252, 1266-1267, 1294-1297, 1299-1304, … (223 ids, first 476, last 1809) |
| `persp_outline` | 201 | 506-640, 902, 943-951, 980-988, 1024-1032, 1063-1071, 1529, 1531-1540, 1552-1569 |
| `block_part` | 17 | 641-650, 739, 873-874, 1075-1077, 1298 |
| `invis_triangle` | 2 | 673-674 |
| `triangle_a` | 4 | 681-682, 1743-1744 |
| `triangle_c` | 4 | 683-684, 1745-1746 |
| `triangle_d` | 2 | 685-686 |
| `triangle_f` | 4 | 689-690, 1749-1750 |
| `triangle_g` | 2 | 691-692 |
| `block_slope` | 51 | 703-708, 713-716, 730-733, 762-763, 826-829, 877-878, 888-889, 895-896, 960-961, 1014-1015, 1033-1034, 1037-1038, 1041-1044, 1091-1092, 1108, 1187-1188, 1256-1259, 1305-1306, 1316-1317 |
| `pit_shine` | 4 | 719, 721, 990, 992 |
| `block_slope_square` | 29 | 764-766, 830-833, 964-966, 1016-1018, 1035-1036, 1039-1040, 1093-1094, 1189-1190, 1307-1309, 1318-1320, 1325-1326 |
| `plank_slope` | 4 | 771-772, 969-970 |
| `plank_slope_square` | 6 | 773-775, 971-973 |
| `block_piece` | 20 | 931, 935, 1229-1238, 1387-1394 |
| `block_small` | 2 | 1078, 1095 |
| `block_bgcolor` | 19 | 1099-1107, 1109-1118 |
| `block_topcolor` | 21 | 1120, 1122-1127, 1132-1139, 1241-1246 |
| `smallOutline` | 5 | 1154-1158 |
| `block_color` | 7 | 1191-1197 |
| `block_slope_color` | 2 | 1198-1199 |
| `block_slope_square_color` | 2 | 1200-1201 |
| `blockOutlineThick` | 10 | 1202-1210, 1262 |
| `blockOutlineThickb` | 9 | 1220-1227, 1264 |
| `block_base` | 21 | 1247-1250, 1253-1255, 1277-1290 |
| `blockOutlineOuter` | 3 | 1261, 1263, 1265 |
| `invisibleOutline` | 1 | 1340 |
| `invisibleOutline_b` | 1 | 1343 |
| `block_edge` | 28 | 1356-1366, 1395, 1621-1636 |
| `block_light` | 44 | 1367-1386, 1453-1460, 1653-1668 |
| `block_detail` | 6 | 1471-1473, 1496, 1507, 1617 |
| `persp_block` | 1 | 1530 |
| `block_edge_c` | 16 | 1637-1652 |
| `block_light_c` | 16 | 1669-1684 |
| `puzzle_base` | 13 | 1685-1696, 1797 |
| `gjHand` | 5 | 1844-1848 |
| `blockDesign` | 25 | 1861-1885 |
| `edge` | 7 | 1922-1928 |
| `blockPiece` | 1 | 1964 |
| `gdh` | 94 | 2703-2704, 2708, 2711, 2714, 2717, 2720, 2723, 2726, 2729, 2732, 2735, 2738, 2741, 2744-2752, 2755, 2758, 2761, 2764-2770, 2773, 2776, 2779, 2782, 2785, 2788-2796, 2799, 2802, 2805, 2808-2851 |
| `gdh_o` | 54 | 2709, 2712, 2715, 2718, 2721, 2724, 2727, 2730, 2733, 2736, 2739, 2742, 2753, 2756, 2759, 2762, 2777, 2780, 2783, 2786, 2797, 2800, 2803, 2806, 2852-2863, 2927, 2929, 2931, 2933, 2935, 2937, 2939, 2941, 2973-2982 |
| `gdh_s` | 32 | 2710, 2713, 2716, 2719, 2722, 2725, 2728, 2731, 2734, 2737, 2740, 2743, 2754, 2757, 2760, 2763, 2778, 2781, 2784, 2787, 2798, 2801, 2804, 2807, 2928, 2930, 2932, 2934, 2936, 2938, 2940, 2942 |
| `gdh_b` | 9 | 2943-2951 |
| `gdh_b_s` | 9 | 2952-2960 |
| `gdh_c` | 7 | 2961-2967 |
| `gdh_c_s` | 5 | 2968-2972 |
| `gdh_o_s` | 4 | 2983-2986 |
| `gdh_g` | 12 | 2987-2998 |
| `gdh_chain` | 2 | 3050, 3052 |
| `gdh_chain_color` | 2 | 3051, 3053 |
| `gdh_platform` | 14 | 3054-3066, 3090 |
| `gdh_platform_color` | 14 | 3067-3079, 3091 |
| `gdh_platformArt` | 6 | 3080-3085 |
| `gdh_crystal` | 2 | 3086, 3088 |
| `gdh_crystal_color` | 2 | 3087, 3089 |

## Animations, particles, fire, smoke, lightning, water.

| Family | n | ids |
|---|---|---|
| `dA_blackSludge` | 1 | 919 |
| `Fire_looped` | 4 | 920-921, 923-924 |
| `waterfallAnim` | 2 | 1516-1517 |
| `waterSplash` | 1 | 1518 |
| `starAnim` | 1 | 1519 |
| `fireball` | 2 | 1582-1583 |
| `lava_top` | 2 | 1591, 1593 |
| `gj_smoke` | 2 | 1849-1850 |
| `gj_drops` | 6 | 1851-1855, 1858 |
| `gj_bubble` | 1 | 1856 |
| `gj_lightning` | 2 | 1857, 1860 |
| `fire_b` | 4 | 1936-1939 |
| `gj_anim` | 66 | 2020-2055, 2864-2865, 2867-2894 |
| `particle` | 213 | 3646-3654, 3656-3659, 3800-3999 |

## Hazards (spikes, blades, saws). Real hitboxes. Never as decoration.

| Family | n | ids |
|---|---|---|
| `spike` | 4 | 8, 39, 103, 392 |
| `sawblade` | 6 | 88-89, 98, 1705-1707 |
| `invis_spike` | 4 | 144-145, 205, 459 |
| `iceSpike` | 3 | 177-179 |
| `blade_b` | 3 | 183-185 |
| `blade` | 6 | 186-188, 740-742 |
| `colorSpike` | 4 | 216-218, 458 |
| `darkblade` | 6 | 397-399, 1708-1710 |
| `lightBlade` | 3 | 678-680 |
| `GJBeast` | 5 | 918, 1327-1328, 1584, 2012 |
| `spinBlade` | 2 | 1619-1620 |
| `bladeTrap` | 3 | 1701-1703 |
| `gdh_spike` | 4 | 3034-3037 |
| `gdh_spike_color` | 4 | 3038-3041 |
| `gdh_spike_g` | 8 | 3042-3049 |

## Interactive (portals, orbs, pads, coins, rings). They do something, not decor.

| Family | n | ids |
|---|---|---|
| `portal_front` | 21 | 10-13, 45-47, 99, 101, 111, 286-287, 660, 745, 747, 749, 1331, 1933, 2064, 2902, 2926 |
| `bump` | 3 | 35, 140, 1332 |
| `ring` | 3 | 36, 141, 1333 |
| `gravbump` | 1 | 67 |
| `gravring` | 1 | 84 |
| `secretCoin` | 2 | 142, 1329 |
| `boost` | 5 | 200-203, 1334 |
| `gravJumpRing` | 1 | 1022 |
| `dropRing` | 1 | 1330 |
| `ring_custom` | 1 | 1594 |
| `smallCoin` | 1 | 1614 |
| `dashRing` | 2 | 1704, 1751 |
| `checkpoint_d` | 1 | 2063 |
| `smartBlock` | 3 | 2895-2897 |
| `spiderRing` | 1 | 3004 |
| `spiderBump` | 1 | 3005 |
| `teleportRing` | 1 | 3027 |

## Pixel art sprites (`pixelart`, `pixelitem`, `pixelb`). Huge, one block each. Pixel look.

| Family | n | ids |
|---|---|---|
| `pixelb` | 30 | 2070-2097, 2120-2121 |
| `pixelart` | 1600 | 2098-2119, 2122-2700, 3101-3599, 3700-3799, 4000-4399 |
| `pixelart_base` | 6 | 3092-3097 |
| `pixelitem` | 139 | 4401-4539 |

## Trigger / editor icons (`edit_*`). Placed as triggers, not visual decoration.

| Family | n | ids |
|---|---|---|
| `edit_eeNoneBtn` | 1 | 22 |
| `edit_eeFBBtn` | 1 | 23 |
| `edit_eeFTBtn` | 1 | 24 |
| `edit_eeFLBtn` | 1 | 25 |
| `edit_eeFRBtn` | 1 | 26 |
| `edit_eeSUBtn` | 1 | 27 |
| `edit_eeSDBtn` | 1 | 28 |
| `edit_eTintCol01Btn` | 7 | 29-30, 105, 744, 899-900, 915 |
| `edit_eStartPosBtn` | 3 | 31, 34, 914 |
| `edit_eGhostEBtn` | 1 | 32 |
| `edit_eGhostDBtn` | 1 | 33 |
| `edit_eeFABtn` | 1 | 55 |
| `edit_eeFALBtn` | 1 | 56 |
| `edit_eeFARBtn` | 1 | 57 |
| `edit_eeFRHBtn` | 1 | 58 |
| `edit_eeFRHInvBtn` | 1 | 59 |
| `edit_eMoveComBtn` | 1 | 901 |
| `edit_ePulseBtn` | 1 | 1006 |
| `edit_eAlphaBtn` | 1 | 1007 |
| `edit_eToggleBtn` | 1 | 1049 |
| `edit_eSpawnBtn` | 1 | 1268 |
| `edit_eRotateComBtn` | 1 | 1346 |
| `edit_eFollowComBtn` | 1 | 1347 |
| `edit_eShakeBtn` | 1 | 1520 |
| `edit_eAnimateBtn` | 1 | 1585 |
| `edit_eParticleBtn` | 2 | 1586, 1700 |
| `edit_eTouchBtn` | 1 | 1595 |
| `edit_eCountBtn` | 1 | 1611 |
| `edit_ePHideBtn` | 1 | 1612 |
| `edit_ePShowBtn` | 1 | 1613 |
| `edit_eCounterBtn` | 1 | 1615 |
| `edit_eStopMoverBtn` | 1 | 1616 |
| `edit_eInstantCountBtn` | 1 | 1811 |
| `edit_eOnDeathBtn` | 1 | 1812 |
| `edit_eFollowPComBtn` | 1 | 1814 |
| `edit_eCollisionBtn` | 1 | 1815 |
| `edit_eCollisionBlock` | 1 | 1816 |
| `edit_ePickupBtn` | 1 | 1817 |
| `edit_eBGEOn` | 1 | 1818 |
| `edit_eBGEOff` | 1 | 1819 |
| `edit_eRandomBtn` | 1 | 1912 |
| `edit_eZoomBtn` | 1 | 1913 |
| `edit_eStaticBtn` | 1 | 1914 |
| `edit_eeNone2Btn` | 1 | 1915 |
| `edit_eOffsetBtn` | 1 | 1916 |
| `edit_eReverseBtn` | 1 | 1917 |
| `edit_eEndBtn` | 2 | 1931, 3600 |
| `edit_ePlayerControlBtn` | 1 | 1932 |
| `edit_eSongBtn` | 1 | 1934 |
| `edit_eTimeWarpBtn` | 1 | 1935 |
| `edit_eCamRotBtn` | 1 | 2015 |
| `edit_eCamGuideBtn` | 1 | 2016 |
| `edit_eEdgeBtn` | 1 | 2062 |
| `edit_eCParticleBtn` | 1 | 2065 |
| `edit_eGravity` | 1 | 2066 |
| `edit_eScaleComBtn` | 1 | 2067 |
| `edit_eAdvRandomBtn` | 1 | 2068 |
| `edit_eForceBlock` | 2 | 2069, 3645 |
| `edit_eOptionsBtn` | 1 | 2899 |
| `edit_eGameRotBtn` | 1 | 2900 |
| `edit_eGPOffsetBtn` | 1 | 2901 |
| `edit_eGradientBtn` | 1 | 2903 |
| `edit_eShaderBtn` | 1 | 2904 |
| `edit_eSh_ShockWaveBtn` | 1 | 2905 |
| `edit_eSh_ShockLineBtn` | 1 | 2907 |
| `edit_eSh_GlitchBtn` | 1 | 2909 |
| `edit_eSh_ChromaticBtn` | 1 | 2910 |
| `edit_eSh_ChromaticGlitchBtn` | 1 | 2911 |
| `edit_eSh_PixelateBtn` | 1 | 2912 |
| `edit_eSh_LensCircleBtn` | 1 | 2913 |
| `edit_eSh_RadialBlurBtn` | 1 | 2914 |
| `edit_eSh_MotionBlurBtn` | 1 | 2915 |
| `edit_eSh_BulgeBtn` | 1 | 2916 |
| `edit_eSh_PinchBtn` | 1 | 2917 |
| `edit_eSh_GrayscaleBtn` | 1 | 2919 |
| `edit_eSh_SepiaBtn` | 1 | 2920 |
| `edit_eSh_InvertColorBtn` | 1 | 2921 |
| `edit_eSh_HueBtn` | 1 | 2922 |
| `edit_eSh_EditColorBtn` | 1 | 2923 |
| `edit_eSh_SplitScreenBtn` | 1 | 2924 |
| `edit_eCamModeBtn` | 1 | 2925 |
| `edit_eSetupMGBtn` | 1 | 2999 |
| `edit_eAreaMoveBtn` | 1 | 3006 |
| `edit_eAreaRotateBtn` | 1 | 3007 |
| `edit_eAreaScaleBtn` | 1 | 3008 |
| `edit_eAreaFadeBtn` | 1 | 3009 |
| `edit_eAreaTintBtn` | 1 | 3010 |
| `edit_eEAreaMoveBtn` | 1 | 3011 |
| `edit_eEAreaRotateBtn` | 1 | 3012 |
| `edit_eEAreaScaleBtn` | 1 | 3013 |
| `edit_eEAreaFadeBtn` | 1 | 3014 |
| `edit_eEAreaTintBtn` | 1 | 3015 |
| `edit_eAdvFollowBtn` | 1 | 3016 |
| `edit_eEnterMoveBtn` | 1 | 3017 |
| `edit_eEnterRotateBtn` | 1 | 3018 |
| `edit_eEnterScaleBtn` | 1 | 3019 |
| `edit_eEnterFadeBtn` | 1 | 3020 |
| `edit_eEnterTintBtn` | 1 | 3021 |
| `edit_eTeleportBtn` | 1 | 3022 |
| `edit_eEnterStopBtn` | 1 | 3023 |
| `edit_eAreaStopBtn` | 1 | 3024 |
| `edit_eChangeBG` | 1 | 3029 |
| `edit_eChangeG` | 1 | 3030 |
| `edit_eChangeMG` | 1 | 3031 |
| `edit_eKeyframeBtn` | 1 | 3033 |
| `edit_eSFXBtn` | 1 | 3602 |
| `edit_eEditSFXBtn` | 1 | 3603 |
| `edit_eEventLinkBtn` | 1 | 3604 |
| `edit_eEditSongBtn` | 1 | 3605 |
| `edit_eBGSpeedBtn` | 1 | 3606 |
| `edit_eSequenceBtn` | 1 | 3607 |
| `edit_eSpawnParticleBtn` | 1 | 3608 |
| `edit_eInstantCollisionBtn` | 1 | 3609 |
| `edit_eDamageSquare` | 1 | 3610 |
| `edit_eDamageCircle` | 1 | 3611 |
| `edit_eMGSpeedBtn` | 1 | 3612 |
| `edit_eUISettingsBtn` | 1 | 3613 |
| `edit_eTimeBtn` | 1 | 3614 |
| `edit_eTimeEventBtn` | 1 | 3615 |
| `edit_eTimeControlBtn` | 1 | 3617 |
| `edit_eResetBtn` | 1 | 3618 |
| `edit_eItemEditBtn` | 1 | 3619 |
| `edit_eItemCompBtn` | 1 | 3620 |
| `edit_eCollisionStateBtn` | 2 | 3640, 3655 |
| `edit_eItemPersBtn` | 1 | 3641 |
| `edit_eBPMBtn` | 1 | 3642 |
| `edit_ePlayerTouchBtn` | 1 | 3643 |
| `edit_eEditAdvFollowBtn` | 1 | 3660 |
| `edit_eReAdvFollowBtn` | 1 | 3661 |
| `edit_eLinkVisibleBtn` | 1 | 3662 |

## Utility / editor helpers.

| Family | n | ids |
|---|---|---|
| `explosion` | 1 | 1618 |
| `gridLine` | 4 | 1753-1754, 1757, 1830 |
| `emptyFrame` | 3 | 1886-1888 |
| `keyframeIcon` | 1 | 3032 |
