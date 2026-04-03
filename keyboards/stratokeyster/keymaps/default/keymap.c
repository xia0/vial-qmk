// Copyright 2022 xia0 (@xia0)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layer_names {
    DEFAULT,
    NOSUS,
    BASIC,
    NORMAL,
    CONFIG
};

// "keymap" of the lowest layer
const int fretboard[3][14] = {
    { KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LEFT_BRACKET, KC_RIGHT_BRACKET, KC_BACKSLASH },
    { KC_CAPS_LOCK, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SEMICOLON, KC_QUOTE, KC_ENTER, KC_NO },
    { KC_LEFT_SHIFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMMA, KC_DOT, KC_SLASH, KC_RIGHT_SHIFT, KC_BACKSPACE, KC_NO }
};

// define how many keys are in each row on the fretboard
const int num_frets[] = { 14, 13, 13 };

// gives the row number given strum bar col
/*
  \ 3,0 \______
   \ 3,1 \ 3,2 \
    \ 3,3 \ 3,4 \ row 0
           \ 3,5 \ row 1
                    row 2
*/
const int strumbar_row_from_col[] = { 0, 1, 0, 2, 1, 2 };

// returns the highest held fret for specified row
int get_highest_fret(int row) {
  // iterate through each row and return the right-most pressed key
  for (int c = num_frets[row]-1; c >= 0; c--) {
    if (matrix_is_on(row, c)) {
      //xprintf("highest fret for row %d is %d\n", row, c);
      return c;
    }
  }

  return -1;
}

// returns whether specified row should be considered held down or not
bool is_strum_held(int row) {
  switch(row) {
    case 0:
      if (matrix_is_on(3,0) || matrix_is_on(3,2)) { return true; }
      break;
    case 1:
      if (matrix_is_on(3,1) || matrix_is_on(3,4)) { return true; }
      break;
    case 2:
      if (matrix_is_on(3,3) || matrix_is_on(3,5)) { return true; }
      break;
  }
  return false;
}

// get what position the pickup selector is currently in
int get_pickup_selector_pos(void) {
  if (!matrix_is_on(3,7)) {
    if (matrix_is_on(3,6)) { return 0; }
    else if (matrix_is_on(3,8)) { return 4; }
  }
  else {
    if (matrix_is_on(3,6)) { return 1; }
    else if (matrix_is_on(3,8)) { return 3; }
    else { return 2; }
  }
  return -1;
}

// press the mods corresponding to pu pos
void set_pickup_selector_mods(int pos) {
  /*
    neck | mid + neck | middle | mid + bridge | bridge
    GUI  | CTRL+ALT   | ALT    | CTRL         | none
  */

  // TODO: figure out why set_mods doesn't work for pos 3 and 1

  //int mods = 0;
  //xprintf("setting mod state to PU pos: %u, bitmask \n", mods);

  switch(pos) {
    case 0:
      //mods = (MOD_BIT(KC_RIGHT_GUI));
      register_code(KC_RIGHT_GUI);
      unregister_code(KC_RIGHT_CTRL);
      unregister_code(KC_RIGHT_ALT);
      break;
    case 1:
      //mods = (MOD_BIT(KC_RIGHT_CTRL) | MOD_BIT(KC_RIGHT_ALT));
      unregister_code(KC_RIGHT_GUI);
      register_code(KC_RIGHT_CTRL);
      register_code(KC_RIGHT_ALT);
      break;
    case 2:
      //mods = (MOD_BIT(KC_RIGHT_ALT));
      unregister_code(KC_RIGHT_GUI);
      unregister_code(KC_RIGHT_CTRL);
      register_code(KC_RIGHT_ALT);
      break;
    case 3:
      //mods = (MOD_BIT(KC_RIGHT_CTRL));
      unregister_code(KC_RIGHT_GUI);
      register_code(KC_RIGHT_CTRL);
      unregister_code(KC_RIGHT_ALT);
      break;
    case 4:
      //clear_mods();
      unregister_code(KC_RIGHT_GUI);
      unregister_code(KC_RIGHT_CTRL);
      unregister_code(KC_RIGHT_ALT);
      break;
  }

  //set_mods(mods);
}

// unregister entire row
void unregister_row(int row) {
  for (int f = 0; f < num_frets[row]; f++) {
    unregister_code(fretboard[row][f]);
  }
}

// unregister space if all strumbars excluding fretted string is open
void unregister_space(void) {
  for (int i = 0; i <= 2; i++) {
    // if strumbar is held or row is not fretted
    if (!(!is_strum_held(i) || (is_strum_held(i) && get_highest_fret(i) >= 0))) {
      return;
    }
  }
  unregister_code(KC_SPACE);
}

// register space if any open string is held
void register_space(void) {
  for (int i = 0; i <= 2; i++) {
    if (is_strum_held(i) && get_highest_fret(i) < 0) { // if playing open string
      if (IS_LAYER_ON(NOSUS)) { tap_code(KC_SPACE); }
      else { register_code(KC_SPACE); }
      return;
    }
  }
}


// register keycode corresponding to fret
void register_fret(int row, int col) {
  register_code(fretboard[row][col]);

  // unregister any other frets that might be held or unregister all if no sustain
  for (int f = 0; f < num_frets[row]; f++) {
    if (f != col || IS_LAYER_ON(NOSUS)) { unregister_code(fretboard[row][f]); }
  }
}



bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    xprintf("KL: col: %u, row: %u, pressed: %u\n", record->event.key.col, record->event.key.row, record->event.pressed);
    xprintf("SB: %u %u %u\n", is_strum_held(0), is_strum_held(1), is_strum_held(2));

    // process pickup position
    if (record->event.key.row == 3 &&
        (
          record->event.key.col == 6 ||
          record->event.key.col == 7 ||
          record->event.key.col == 8
        )
    ) {
      xprintf("PU: %u\n", get_pickup_selector_pos());
      set_pickup_selector_mods(get_pickup_selector_pos());
    }

    // only run when not on "normal" typing layer
    if (IS_LAYER_ON(NORMAL) || IS_LAYER_ON(CONFIG)) { return true; }

    if (record->event.key.row >= 0 && record->event.key.row <= 2) {
      // ignore event if it's on a lower fret than what's currently held
      if (record->event.key.col < get_highest_fret(record->event.key.row)) { return false; }

      // ignore fret press if strumbar not pressed on that row
      if (!is_strum_held(record->event.key.row)) { return false; }
    }

    // figure out which row to interact with
    //   (strumbars are on row 3 but interact with rows 0-2)
    int r = record->event.key.row;
    // if row corresponds to strum bar, set to its corresponding row
    if (r == 3 && record->event.key.col >= 0 && record->event.key.col <= 5) { r = strumbar_row_from_col[record->event.key.col]; }

    if (record->event.pressed) {
      if (is_strum_held(r)) {
        if (get_highest_fret(r) >= 0) { // fret is held
          unregister_space();
          register_fret(r, get_highest_fret(r));
        } else { // no fret held - send space
          register_space();
        }
      }
    }
    else { // if a key is released, check if strum bar is held. if so, a lower fret shoud be pressed

      // only unregister spacebar if NONE of the strum bars are held
      unregister_space();

      // check if a lower fret is still held
      if (is_strum_held(r)) { // only check if a key should be pressed if the strum bar is pressed
        if (get_highest_fret(r) >= 0) {
          register_fret(r, get_highest_fret(r));
        } else { // no fret held -- unregister all frets
          unregister_row(r);
          register_space(); // revert back to space since a strum bar is held
        }

      } else { // if strum bar is not held, release all keys
        unregister_row(r);
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
        _______, _______, _______,
        KC_MUTE,
        MO(BASIC), MO(NOSUS), MO(NORMAL), MO(CONFIG)
    ),
    [BASIC] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [NOSUS] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [NORMAL] = LAYOUT(
        KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   , KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_LBRC, KC_RBRC, KC_BSLS,
        KC_CAPS, KC_A   , KC_S   , KC_D   , KC_F   , KC_G   , KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_QUOT, KC_ENT ,
        KC_LSFT, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   , KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_RSFT, KC_BSPC,
        KC_DOWN, KC_UP  ,
        KC_RGHT, KC_LEFT,
        KC_SPC , KC_SPC ,
        _______, _______, _______,
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
        KC_NO, KC_NO, KC_NO,
        QK_BOOTLOADER,
        _______, _______, _______, _______
    )
};


#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {

  if (clockwise) { tap_code_delay(KC_VOLU, 10); }
  else { tap_code_delay(KC_VOLD, 10); }

  return true;
}
#endif
