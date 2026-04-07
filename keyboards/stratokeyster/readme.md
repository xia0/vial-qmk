# stratokeyster
guitar hero mechanical keyboard

## bom
|item|qty|
|---|---|
|M3 x 10~12 mm bolt*|14|
|M3 x 30 mm bolt*|4|
|M3 x 80 mm bolt|3|
|M3 nyloc nut|3|
|M3 heat set insert 5 mm OD x 5 mm L|18|
|M3 washer<sup>#</sup>|0~8|
|ec11 rotary encoder 15 mm half shaft|1|
|rs26 3 pole 4 throw rotary switch 15 mm half shaft|1|
|5 way pickup selector|1|
|18 pin 2.54 mm pitch angled connector male<sup>^</sup>|1|
|18 pin 2.54 mm pitch dupont cable female end ~5 cm<sup>^</sup>|1|
|usb breakout board male (to match your pico)|1|
|usb breakout board female (to match your lead)|1|
|mx switch for fretboard|39-40|
|mx switch for bridge|1|
|mx switch for strumbars†|6|
|2u pcb mount stab|2-3|
|2u plate mount stab|1|
|raspberry pi pico|1|
|fretboard pcb|1|
|fretboard plate|1|
|strumbar pcb|1|
|3d printed parts||

_\* phillips head silver colour bolts recommended for aesthetic reasons._\
_^ alternatively, use a combination of pins that add up to 18 pins._\
_# for tuning strum bar clearance. you could also use a 3d printed washer, bits of paper, etc..._\
_† heavy and clicky recommended for strum bars._

## pick guard wiring
#### pickup selector
- com (4 or 5) -> row 3
- 1 or 6 -> col 8
- 2 or 7 -> col 7
- 3 or 8 -> col 6

#### EC11 rotary encoder
- push button -> row 3, col 9
- encoder -> encA, gnd, encB

#### RS26 rotary switch
- com -> row 3
- 1 -> col 13
- 2 -> col 12
- 3 -> col 11
- 4 -> col 10

#### bridge switch
row 1, col 13

## usage
#### pickup selector (mods) defaults
| neck | neck + mid | middle | bridge + mid | bridge |
|---|---|---|---|---|
| gui | ctrl + alt | alt | ctrl | none |

#### rotary encoder
- volume control
- mute/unmute

#### rotary switch
| pos 1 (ccw) | pos 2 | pos 3 | pos 4 (cw) |
|---|---|---|---|
| config | normal typing | strum typing | numbers |

### config layer
|control|function|
|---|---|
|middle strum bar|toggle between simple mode and normal mode*|
|backslash|toggle between backslash or backspace|
|tab|toggle between tab or escape|
|minus|???|
|encoder push button|bootloader|
|r|reset eeprom|

_* in simple mode, only the middle strum bar is active._

#### edit mods
mods can be changed by selecting the pickup position, hold the desired mods and press enter to set.
|left shift|z|x|c|
|---|---|---|---|
|left shift|left ctrl|left gui|left alt|

switch off the config layer to save to eeprom.
