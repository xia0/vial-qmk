// Copyright 2022 xia0 (@xia0)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "dynamic_keymap.h"

// define EEPROM vars
// TODO figure out how to store more than 32 bits into eeprom
typedef union {
  uint64_t raw;
  struct {
    bool simple_mode: 1;
    bool backspace_replaces_backslash :1;
    bool escape_replaces_tab :1;
    bool spinal_tap :1;
    uint8_t mods_for_pickup_pos_0 :4; // using 8 bit int but take up 4 bits. these need to be at the end
    uint8_t mods_for_pickup_pos_1 :4;
    uint8_t mods_for_pickup_pos_2 :4;
    uint8_t mods_for_pickup_pos_3 :4;
    uint8_t mods_for_pickup_pos_4 :4;
  };
} user_config_t;

user_config_t user_config;

// EEPROM reset - default settings
/*
  0    | 1          | 2      | 3            | 4
  neck | mid + neck | middle | mid + bridge | bridge
  GUI  | CTRL+ALT   | ALT    | CTRL         | none
*/
void eeconfig_init_user(void) {
  user_config.raw = 0;
  user_config.mods_for_pickup_pos_4 = 0;
  user_config.simple_mode = false;
  user_config.backspace_replaces_backslash = false;
  user_config.escape_replaces_tab = false;
  user_config.spinal_tap = false;
  user_config.mods_for_pickup_pos_0 = (MOD_BIT(KC_LEFT_GUI));
  user_config.mods_for_pickup_pos_1 = (MOD_BIT(KC_LEFT_CTRL) | MOD_BIT(KC_LEFT_ALT));
  user_config.mods_for_pickup_pos_2 = (MOD_BIT(KC_LEFT_ALT));
  user_config.mods_for_pickup_pos_3 = (MOD_BIT(KC_LEFT_CTRL));
  eeconfig_update_user(user_config.raw); // Write BASE value to EEPROM now
}

// define how many keys are in each row on the physical fretboard
// we do not check outside these bounds when running fretboard logic
const int num_frets[] = { 14, 13, 13 };

enum layer_names {
  BASE,
  NOSUS,
  NUM,
  NORMAL,
  CONFIG
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT(
        KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   , KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_LBRC, KC_RBRC, KC_BSLS,
        KC_CAPS, KC_A   , KC_S   , KC_D   , KC_F   , KC_G   , KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_QUOT, KC_ENT , MO(NOSUS),
        KC_LSFT, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   , KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_RSFT, KC_BSPC, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______,
        KC_MUTE,
        MO(NUM), _______, MO(NORMAL), MO(CONFIG)
    ),
    [NOSUS] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [NUM] = LAYOUT(
        _______, KC_1   , KC_2   , KC_3   , KC_4   , KC_5   , KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_MINS, KC_EQL , _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______,
        _______, _______,
        _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [NORMAL] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, MO(NUM),
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        KC_DOWN, KC_UP  ,
        KC_RGHT, KC_LEFT,
        MO(NUM), KC_SPC ,
        _______, _______, _______,
        _______,
        _______, _______, _______, _______
    ),
    [CONFIG] = LAYOUT(
        KC_NO, KC_NO, KC_NO, KC_NO, EE_CLR, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_LEFT_SHIFT, KC_LEFT_CTRL, KC_LEFT_GUI, KC_LEFT_ALT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO,
        KC_NO, KC_NO,
        KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO,
        QK_BOOTLOADER,
        _______, _______, _______, _______
    )
};

// gives the row number given strum bar col
/*
  \ 3,0 \______
   \ 3,1 \ 3,2 \
    \ 3,3 \ 3,4 \ row 0
           \ 3,5 \ row 1
                    row 2
*/
const int strumbar_row_from_col[] = { 0, 1, 0, 2, 1, 2 };

// get keycode with provided fret
// allows substitution from user config
uint8_t get_fret_keycode(int row, int col) {

  int keycode;

  // go down in layers until a non-transparent key on an active layer is found
  for (int i = get_highest_layer(layer_state); i >= 0; i--) {
    if (IS_LAYER_ON(i) || i == 0) { // base layer always accepted
      keycode = pgm_read_word(&keymaps[i][row][col]);
      if (keycode != KC_TRANSPARENT) { break; }
    }
  }

  // check config if substitution required
  // row and col should be specified in case keymap has these keycodes at other positions
  if (keycode == KC_TAB && user_config.escape_replaces_tab && row == 0 && col == 0) { keycode = KC_ESCAPE; }
  else if (keycode == KC_BACKSLASH && user_config.backspace_replaces_backslash && row == 0 && col == 13) { keycode = KC_BACKSPACE; }

  return keycode;
}

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
void unregister_row_except(int row, int ignore_col) {
  for (int f = 0; f < num_frets[row]; f++) {
    if (f != ignore_col || IS_LAYER_ON(NOSUS)) {

      // do not unregister shift mods if they happen to already be held
      if (row == 2 && get_mods() & MOD_MASK_SHIFT &&
         ((get_fret_keycode(row, f) == KC_LEFT_SHIFT && get_mods() & MOD_BIT(KC_LEFT_SHIFT) && !matrix_is_on(2,0)) ||
          (get_fret_keycode(row, f) == KC_RIGHT_SHIFT && get_mods() & MOD_BIT(KC_RIGHT_SHIFT) && !matrix_is_on(2,11)))
         ) {
        continue;
      }

      unregister_code(get_fret_keycode(row, f));
    }
  }
}
void unregister_row(int row) {
  unregister_row_except(row, -1);
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

  // spinal tap mode
  if (user_config.spinal_tap && IS_LAYER_ON(NUM) && row == 0) {
    switch(col) {
      case 11:
        SEND_STRING("11");
        return;
      case 10:
        SEND_STRING("10");
        return;
    }
  }

  register_code(get_fret_keycode(row, col));

  // unregister any other frets that might be held or unregister all if no sustain
  unregister_row_except(row, col);
}

// update mods based on pickup selector pos
void update_mods(void) {
  set_pickup_selector_mods(get_pickup_selector_pos());
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  //xprintf("KL: col: %u, row: %u, pressed: %u\n", record->event.key.col, record->event.key.row, record->event.pressed);

  // check if user is configuring
  if (IS_LAYER_ON(CONFIG)) {
    // toggle backslash as backspace
    if (record->event.pressed && record->event.key.row == 0 && record->event.key.col == 13) {
      user_config.backspace_replaces_backslash = !user_config.backspace_replaces_backslash;
      xprintf("CONFIG: backspace replaces backslash set to %u\n", user_config.backspace_replaces_backslash);
      return false;
    }
    // toggle escape as tab
    else if (record->event.pressed && record->event.key.row == 0 && record->event.key.col == 0) {
      user_config.escape_replaces_tab = !user_config.escape_replaces_tab;
      xprintf("CONFIG: escape replaces tab set to %u\n", user_config.escape_replaces_tab);
      return false;
    }
    // these keyboards go to 11
    else if (record->event.pressed && record->event.key.row == 0 && record->event.key.col == 11) {
      user_config.spinal_tap = !user_config.spinal_tap;
      xprintf("CONFIG: these keyboards go to 1%u\n", user_config.spinal_tap);
      return false;
    }
    // toggle simple mode
    else if (record->event.pressed && record->event.key.row == 3 && (record->event.key.col == 1 || record->event.key.col == 4)) {
      user_config.simple_mode = !user_config.simple_mode;
      xprintf("CONFIG: simple mode set to %u\n", user_config.simple_mode);
      return false;
    }
    // enter - save currently held mods to current pickup position
    else if (record->event.pressed && record->event.key.row == 1 && record->event.key.col == 12) {
      xprintf("CONFIG: mods for pickup pos %u set to %u\n", get_pickup_selector_pos(), get_mods());
      switch(get_pickup_selector_pos()) {
        case 0: user_config.mods_for_pickup_pos_0 = get_mods(); break;
        case 1: user_config.mods_for_pickup_pos_1 = get_mods(); break;
        case 2: user_config.mods_for_pickup_pos_2 = get_mods(); break;
        case 3: user_config.mods_for_pickup_pos_3 = get_mods(); break;
        case 4: user_config.mods_for_pickup_pos_4 = get_mods(); break;
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
    // check if keycode substitution is required
    //if (keycode != get_fret_keycode(record->event.key.row, record->event.key.col)) {
    if (record->event.key.row >= 0 && record->event.key.row <= 2 && record->event.key.col < num_frets[record->event.key.row]) {
      if (record->event.pressed) { register_code(get_fret_keycode(record->event.key.row, record->event.key.col)); }
      else { unregister_code(get_fret_keycode(record->event.key.row, record->event.key.col)); }
      return false;
    }

    return true;
  }

  // anything below NORMAL layer is triggered using strum bar
  // process fretboard

  // first check if press should be ignored
  if (record->event.key.row >= 0 && record->event.key.row <= 2) {
    // ignore event if it's on a lower fret than what's currently held
    if (record->event.key.col < get_highest_fret(record->event.key.row)) { return false; }

    // ignore fret press if strumbar not pressed on that row
    if (!is_strum_held(record->event.key.row) && !user_config.simple_mode) { return false; }

    // check if pressed key is out of bounds of fretboard (e.g. 1,13 and 2,13)
    if (record->event.key.col >= num_frets[record->event.key.row]) { return true; }
  }

  // figure out which row to interact with
  //   (strumbars are on row 3 but interact with rows 0-2)
  int r = record->event.key.row;
  // if row corresponds to strum bar, set to its corresponding row
  if (r == 3 && record->event.key.col >= 0 && record->event.key.col <= 5) {

    // ignore strum bars on row 0 and 2 if simple mode
    if (user_config.simple_mode && !(record->event.key.col == 1 || record->event.key.col == 4)) { return false; }

    r = strumbar_row_from_col[record->event.key.col];
    //xprintf("SB: %u %u %u\n", is_strum_held(0), is_strum_held(1), is_strum_held(2));
  }
  if (r == 3) { return true; } // if r is still 3, likely not fret or strum bar


  // process keypresses for simple mode
  // TODO refactor this code into non-simple typing
  if (user_config.simple_mode) {
    bool is_fretted = false;
    if (record->event.pressed) {
      if (is_strum_held(1)) {
        for (int i = 0; i <= 2; i++) {
          if (get_highest_fret(i) >= 0) {
            unregister_code(KC_SPACE);
            register_fret(i, get_highest_fret(i));
            is_fretted = true;
          }
        }
        if (!is_fretted) {
          register_space();
        }
      }
    }
    else {
      unregister_space();
      for (int i = 0; i <= 2; i++) {
        // check if a lower fret is still held
        if (is_strum_held(1)) { // only check if a key should be pressed if the strum bar is pressed
          if (get_highest_fret(i) >= 0) {
            register_fret(i, get_highest_fret(i));
            is_fretted = true;
          } else { // no fret held -- unregister all frets
            unregister_row(i);
          }
        } else { // if strum bar is not held, release all keys
          unregister_row(i);
        }
      }
      if (!is_fretted) {
        register_space();
      }
    }
    return false;
  }


  // process keypresses for normal mode
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

  return false;
}




void keyboard_post_init_user(void) {
  // Read the user config from EEPROM
  user_config.raw = eeconfig_read_user();
}

layer_state_t layer_state_set_user(layer_state_t state) {
  static bool config_layer_on; // saves state of config layer so we know when we leave it
  static user_config_t user_config_previous;

  // check if we are on config layer
  if (IS_LAYER_ON_STATE(state, CONFIG)) {
    user_config_previous.raw = user_config.raw; // save existing config to compare for changes
    unregister_row(0);
    unregister_row(1);
    unregister_row(2);
    unregister_space();
    xprintf("CONFIG layer activated\n");

    xprintf("current settings:\n");
    xprintf("- simple mode: %u\n", user_config_previous.simple_mode);
    xprintf("- backspace_replaces_backslash: %u\n", user_config_previous.backspace_replaces_backslash);
    xprintf("- escape_replaces_tab: %u\n", user_config_previous.escape_replaces_tab);
    xprintf("- these keyboards go to 1%u\n", user_config_previous.spinal_tap);
    xprintf("- mods_for_pickup_pos_0: %u\n", user_config_previous.mods_for_pickup_pos_0);
    xprintf("- mods_for_pickup_pos_1: %u\n", user_config_previous.mods_for_pickup_pos_1);
    xprintf("- mods_for_pickup_pos_2: %u\n", user_config_previous.mods_for_pickup_pos_2);
    xprintf("- mods_for_pickup_pos_3: %u\n", user_config_previous.mods_for_pickup_pos_3);
    xprintf("- mods_for_pickup_pos_4: %u\n", user_config_previous.mods_for_pickup_pos_4);

    config_layer_on = true;
    unregister_mods(get_mods());
  }
  else { // save config to EEPROM when switching off config layer
    if (config_layer_on) {
      if (user_config.raw != user_config_previous.raw) {  // only save to EEPROM if config has changed
        eeconfig_update_user(user_config.raw);
        xprintf("config has changed. saving to EEPROM...\n");
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

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {

  if (clockwise) { tap_code_delay(KC_VOLU, 10); }
  else { tap_code_delay(KC_VOLD, 10); }

  return true;
}
#endif
