# stratokeyster
guitar hero mechanical keyboard

## bom
|item|qty|
|---|---|
|M3 x 10~12 mm bolt*|14|
|M3 x 30 mm bolt*|4|
|M3 x 80 mm bolt|3|
|M3 washer|6+|
|M3 nyloc nut|3|
|M3 x 5 mm OD x 5 mm L heat set insert|18|
|ec11 rotary encoder 15 mm half shaft|1|
|rs26 3 pole 4 throw rotary switch 15 mm half shaft|1|
|5 way pickup selector|1|
|raspberry pi pico|1|
|fretboard pcb|1|
|fretboard plate|1|
|strumbar pcb|1|
|18p 2.54 mm pitch angled connector male|1|
|18p 2.54 mm pitch dupont cable female end ~5 cm|1|
|usb breakout board male (to match your pico)|1|
|usb breakout board female (to match your lead)|1|
|mx switch for fretboard|39-40|
|mx switch for strumbars|6|
|2u stabs|2-3|
|3d printed parts||

_\* phillips head silver colour bolts recommended for aesthetic reasons_

## pick guard wiring
#### pickup selector
- com -> row 3
- 1 or 6 -> col 8
- 2 or 7 -> col 7
- 3 or 8 -> col 6

#### EC11 rotary encoder
- push button -> row 3 and col 9
- encoder -> encA, gnd, encB

#### RS26 rotary switch
- com -> row 3
- 1 -> col 13
- 2 -> col 12
- 3 -> col 11
- 4 -> col 10

## usage
#### pickup selector (mods)
| neck | neck + mid | middle | bridge + mid | bridge |
|---|---|---|---|---|
| gui | ctrl + alt | alt | ctrl | none |

#### rotary encoder
- volume control
- mute/unmute

#### rotary switch
| pos 1 (ccw) | pos 2 | pos 3 | pos 4 (cw) |
|---|---|---|---|
| config | normal typing | guitar typing | caps lock |

#### bootloader
accesss bootloader by switching the keyboard into the "config" layer and push the rotary encoder
