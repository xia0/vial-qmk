// Copyright 2022 xia0 (@xia0)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layer_names {
    DEFAULT,
    TONE0,
    TONE1,
    TONE2,
    TONE3,
    PU0,
    PU1,
    PU2,
    PU3,
    PU4,
    NORMAL,
    CONFIG
};

// "keymap" of the lowest layer
const int fretboard[3][14] = {
    { KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LEFT_BRACKET, KC_RIGHT_BRACKET, KC_BACKSLASH },
    { KC_CAPS_LOCK, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SEMICOLON, KC_QUOTE, KC_ENTER, KC_NO },
    { KC_LEFT_SHIFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMMA, KC_DOT, KC_SLASH, KC_RIGHT_SHIFT, KC_NO, KC_NO }
};

// define how many keys are in each row on the fretboard
const int num_frets[] = { 14, 13, 12 };

// returns the highest held fret for specified row
int get_highest_fret(int row) {

  // iterate through each row and return the right-most pressed key
  for (int c = num_frets[row]-1; c >= 0; c--) {
    if (matrix_is_on(row, c)) {
      //xprintf("highest fret for row %d is %d\n", row, c);
      return c;
    }
  }

  //xprintf("open fret for row %d\n", row);
  return -1;
}

// returns whether specified row should be considered held down or not
bool is_strum_held(int row) {
  switch(row) {
    case 0:
      if (matrix_is_on(1, 13) || matrix_is_on(3,11)) { return true; }
      break;
    case 1:
      if (matrix_is_on(2, 12) || matrix_is_on(3,12)) { return true; }
      break;
    case 2:
      if (matrix_is_on(2, 13) || matrix_is_on(3,13)) { return true; }
      break;
  }
  return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    xprintf("KL: row: %u, column: %u, pressed: %u\n", record->event.key.col, record->event.key.row, record->event.pressed);

    if (get_highest_layer(layer_state) != NORMAL &&
        get_highest_layer(layer_state) != CONFIG
      ) { // only run when not on normal typing layer

      // if a key is released, check if strum bar is held. if so, a lower fret shoud be pressed
      if (record->event.pressed) {
        for (int i = 0; i<= 2; i++) {
          if (is_strum_held(i)) {
            if (get_highest_fret(i) >= 0) {
              register_code(fretboard[i][get_highest_fret(i)]);
            } else {
              register_code(KC_SPACE);
            }
          }
        }

      } else { // key is released

        // only unregister spacebar if NONE of the strum bars are held
        if (!is_strum_held(0) && !is_strum_held(1) && !is_strum_held(2)) {
          unregister_code(KC_SPACE);
        }

        // iterate each row and check if a lower fret is still held
        for (int i = 0; i <= 2; i++) {
          if (is_strum_held(i)) { // only check if a key should be pressed if the strum bar is pressed
            if (get_highest_fret(i) >= 0) register_code(fretboard[i][get_highest_fret(i)]);
          } else { // if strum bar is not held, release all keys
            for (int f = 0; f < num_frets[i]; f++) {
              unregister_code(fretboard[i][f]);
            }
          }
        }

      }

    }

    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
  layer_debug();
  return state;
}


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [DEFAULT] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        MO(PU0), MO(PU2), MO(PU4), MO(PU3), MO(PU1),
        KC_MUTE,
        KC_LOCKING_CAPS_LOCK, _______, MO(NORMAL), MO(CONFIG)
    ),
    [TONE0] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [TONE1] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [TONE2] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [TONE3] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [PU2] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [PU0] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [PU4] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [PU1] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [PU3] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [NORMAL] = LAYOUT(
        KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   , KC_T   , KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_LBRC, KC_RBRC,
        KC_CAPS, KC_A   , KC_S   , KC_D   , KC_F   , KC_G   , KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_QUOT, KC_ENT ,
        KC_LSFT, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   , KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_RSFT, _______,
        KC_DOWN, KC_UP  ,
        KC_RGHT, KC_LEFT,
        KC_SPC , KC_SPC ,
        _______, _______, _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [CONFIG] = LAYOUT(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO,
        KC_NO, KC_NO,
        KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        QK_BOOTLOADER,
        _______, _______, _______, _______
    )
};


#ifdef ENCODER_ENABLE
bool encoder_update_kb(uint8_t index, bool clockwise) {
    if (!encoder_update_user(index, clockwise)) {
        return false;
    }

    if (clockwise) {
        tap_code_delay(KC_VOLU, 10);
    } else {
        tap_code_delay(KC_VOLD, 10);
    }

    return true;
}
#endif
