// Copyright 2022 xia0 (@xia0)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

// define EEPROM vars
typedef union {
  uint32_t raw;
  struct {
    bool backspace_replaces_backslash :1;
    bool escape_replaces_tab :1;
    uint8_t mods_for_pickup_pos_0 :8;
    uint8_t mods_for_pickup_pos_1 :8;
    uint8_t mods_for_pickup_pos_2 :8;
    uint8_t mods_for_pickup_pos_3 :8;
    uint8_t mods_for_pickup_pos_4 :8;
  };
} user_config_t;

user_config_t user_config;

// EEPROM reset - default settings
/* default mods
  0    | 1          | 2      | 3            | 4
  neck | mid + neck | middle | mid + bridge | bridge
  GUI  | CTRL+ALT   | ALT    | CTRL         | none
*/
void eeconfig_init_user(void) {
  user_config.raw = 0;
  user_config.backspace_replaces_backslash = false;
  user_config.escape_replaces_tab = false;
  user_config.mods_for_pickup_pos_0 = (MOD_BIT(KC_RIGHT_GUI));
  user_config.mods_for_pickup_pos_1 = (MOD_BIT(KC_RIGHT_CTRL) | MOD_BIT(KC_RIGHT_ALT));
  user_config.mods_for_pickup_pos_2 = (MOD_BIT(KC_RIGHT_ALT));
  user_config.mods_for_pickup_pos_3 = (MOD_BIT(KC_RIGHT_CTRL));
  user_config.mods_for_pickup_pos_4 = 0;
  eeconfig_update_user(user_config.raw); // Write default value to EEPROM now
}

enum layer_names {
    DEFAULT,
    NOSUS,
    BASIC,
    NORMAL,
    CONFIG
};

// default "keymap" of the fretboard
int fretboard[3][14] = {
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

// returns whether strumbar of specified row should be considered held down or not
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
  int mods = 0;

  switch(pos) {
    case 0:
      mods = user_config.mods_for_pickup_pos_0;
      break;
    case 1:
      mods = user_config.mods_for_pickup_pos_1;
      break;
    case 2:
      mods = user_config.mods_for_pickup_pos_2;
      break;
    case 3:
      mods = user_config.mods_for_pickup_pos_3;
      break;
    case 4:
      mods = user_config.mods_for_pickup_pos_4;
      break;
  }

  xprintf("PU pos: %u setting mod state to: %u\n", pos, mods);
  unregister_mods(get_mods());
  register_mods(mods);
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
    if (f != col || IS_LAYER_ON(NOSUS)) {
      unregister_code(fretboard[row][f]);
    }
  }
}

// update mods based on pickup selector pos
void update_mods(void) {
  set_pickup_selector_mods(get_pickup_selector_pos());
}

// update fretboard array from config
void update_fretboard(void) {
  if (user_config.backspace_replaces_backslash) { fretboard[0][13] = KC_BACKSPACE; }
  else { fretboard[0][13] = KC_BACKSLASH; }

  if (user_config.escape_replaces_tab) { fretboard[0][0] = KC_ESCAPE; }
  else { fretboard[0][0] = KC_TAB; }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    //xprintf("KL: col: %u, row: %u, pressed: %u\n", record->event.key.col, record->event.key.row, record->event.pressed);

    // check if user is configuring
    if (IS_LAYER_ON(CONFIG)) {
      // backslash as backspace
      if (record->event.pressed && record->event.key.row == 0 && record->event.key.col == 13) {
        user_config.backspace_replaces_backslash = !user_config.backspace_replaces_backslash;
        xprintf("CONFIG: backspace replaces backslash: %u\n", user_config.backspace_replaces_backslash);
        return false;
      }
      // escape as tab
      else if (record->event.pressed && record->event.key.row == 0 && record->event.key.col == 0) {
        user_config.escape_replaces_tab = !user_config.escape_replaces_tab;
        xprintf("CONFIG: escape replaces tab: %u\n", user_config.escape_replaces_tab);
        return false;
      }
      // mods (row 2 strumbar) -- save currently held mods to current pickup position
      else if (record->event.pressed && record->event.key.row == 3 && is_strum_held(2)) {
        xprintf("CONFIG: mods for pickup pos %u set to %u\n", get_pickup_selector_pos(), get_mods());
        switch(get_pickup_selector_pos()) {
          case 0:
            user_config.mods_for_pickup_pos_0 = get_mods();
            break;
          case 1:
            user_config.mods_for_pickup_pos_1 = get_mods();
            break;
          case 2:
            user_config.mods_for_pickup_pos_2 = get_mods();
            break;
          case 3:
            user_config.mods_for_pickup_pos_3 = get_mods();
            break;
          case 4:
            user_config.mods_for_pickup_pos_4 = get_mods();
            break;
        }
        return false;
      }
      return true;
    }

    // process pickup position
    if (record->event.key.row == 3 &&
        (
          record->event.key.col == 6 ||
          record->event.key.col == 7 ||
          record->event.key.col == 8
        )
    ) {
      update_mods();
      return true;
    }

    // type normally (without strumbars)
    if (IS_LAYER_ON(NORMAL)) {
      if (record->event.key.row >= 0 && record->event.key.row <= 2) { // backslash
        if (record->event.pressed) { register_code(fretboard[record->event.key.row][record->event.key.col]); }
        else { unregister_code(fretboard[record->event.key.row][record->event.key.col]); }
        return false;
      }
      return true;
    }

    // process fretboard

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
    if (r == 3 && record->event.key.col >= 0 && record->event.key.col <= 5) {
      r = strumbar_row_from_col[record->event.key.col];
      xprintf("SB: %u %u %u\n", is_strum_held(0), is_strum_held(1), is_strum_held(2));
    }
    if (r == 3) { return true; } // if r is still 3, likely not fret or strum bar

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


user_config_t user_config_previous; // save existing config to compare for changes

void keyboard_post_init_user(void) {
  // Read the user config from EEPROM
  user_config.raw = eeconfig_read_user();
  user_config_previous.raw = user_config.raw;
  update_fretboard();
}

layer_state_t layer_state_set_user(layer_state_t state) {
  static bool config_layer_on; // saves state of config layer

  // check if we are on config layer
  if (IS_LAYER_ON_STATE(state, CONFIG)) {
    xprintf("CONFIG layer activated\n");
    config_layer_on = true;
    unregister_mods(get_mods());
  }
  else { // save config to EEPROM when switching off config layer
    if (config_layer_on) {
      if (user_config.raw != user_config_previous.raw) {  // only save to EEPROM if config has changed
        eeconfig_update_user(user_config.raw);
        user_config_previous.raw = user_config.raw;
        xprintf("config has changed. saving to EEPROM...\n");
        update_fretboard();
      }
      else {
        xprintf("No changes to config detected...\n");
      }
      update_mods();
      config_layer_on = false;
    }
  }

  //layer_debug();
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
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        KC_DOWN, KC_UP  ,
        KC_RGHT, KC_LEFT,
        KC_SPC , KC_SPC ,
        _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [CONFIG] = LAYOUT(
        KC_NO, KC_NO, KC_NO, KC_NO, EE_CLR, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_LEFT_SHIFT, KC_LEFT_CTRL, KC_LEFT_GUI, KC_LEFT_ALT, KC_NO, KC_NO, KC_NO, KC_NO, KC_RIGHT_ALT, KC_RIGHT_GUI, KC_RIGHT_CTRL, KC_RIGHT_SHIFT, KC_NO,
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
