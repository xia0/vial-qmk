# stratokeyster
![stratokeyster guitar hero mechanical keyboard](https://github.com/xia0/vial-qmk/blob/stratokeyster/keyboards/stratokeyster/pics/IMG_5900%5B1%5D.jpg?raw=true)

## bom
|item|qty|
|---|---|
|M3 x 10~12 mm bolt*|16|
|M3 x 30 mm bolt*|4|
|M3 x 80 mm bolt|3|
|M3 nyloc nut|3|
|M3 heat set insert 5 mm OD x 4~5 mm L|20|
|M3 washer#|0~8|
|ec11 rotary encoder 15 mm half shaft|1|
|rs26 3 pole 4 throw rotary switch 15 mm half shaft|1|
|5 way pickup selector|1|
|passive buzzer 6.5 mm pin spacing|1|
|18 pin 2.54 mm pitch angled connector male^|1|
|18 pin 2.54 mm pitch dupont cable female end ~5 cm^|1|
|usb breakout board male|1|
|usb breakout board female|1|
|mx switch for fretboard|39-40|
|mx switch for bridge†|1|
|mx switch for strumbars†|6|
|2u pcb mount stab|2-3|
|2u plate mount stab|1|
|raspberry pi pico|1|
|fretboard pcb|1|
|fretboard plate|1|
|strumbar pcb|1|
|3d printed parts||

_\* phillips head silver colour bolts recommended for aesthetic reasons_\
_^ alternatively, a combination of pins that add up to 18_\
_# for tuning strum bar clearance. you could also use a 3d printed washer, bits of paper, etc..._\
_† heavy and clicky recommended e.g. kailh box navy or kailh box black_

## pick guard wiring
#### pickup selector
- com (4 or 5) -> row 3
- 1 or 6 -> col 8
- 2 or 7 -> col 7
- 3 or 8 -> col 6

#### EC11 rotary encoder
- push button -> row 3, col 9
- encoder -> encA, gnd, encB

encoder direction can be changed later on config layer

#### RS26 rotary switch
- A -> row 3
- 1 -> col 13
- 2 -> col 12
- 3 -> col 11
- 4 -> col 10

#### bridge key
row 1, col 13

#### buzzer
- +ve -> GP0
- -ve -> ground

#### free pin
row 2, col 13

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

#### bridge switch (palm mute)
does not hold down the key and does not register space if the key is released before the strum bar.

### config layer
the hid console in qmk toolbox can be useful to view which options are set but is not necessary.
|control|function|
|---|---|
|middle strum bar|toggle between simple mode and normal mode*|
|backslash|toggle between backslash or backspace|
|tab|toggle between tab or escape|
|bridge key|NUM layer replaces palm mute (access higher strings easier)|
|encoder push button|bootloader|
|encoder rotate|set direction of rotation as clockwise|
|a|toggle audio|
|d|toggle drop D tuning|
|r|reset eeprom|
|[|???|

_* in simple mode, only the middle strum bar is active._

#### edit mods
mods can be changed by selecting the pickup position, hold the desired mods and press enter to set.
|left shift|z|x|c|
|---|---|---|---|
|left shift|left ctrl|left gui|left alt|

switch off the config layer to save to eeprom.
