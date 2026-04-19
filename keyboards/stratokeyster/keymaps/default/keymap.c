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
    bool ctrl_replaces_caps :1;
    bool encoder_reverse_direction :1;
    bool drop_d :1;
    bool enable_audio :1;
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
  user_config.encoder_reverse_direction = false;
  user_config.drop_d = false;
  user_config.enable_audio = true;
  user_config.spinal_tap = false;
  user_config.mods_for_pickup_pos_0 = (MOD_BIT(KC_LEFT_GUI));
  user_config.mods_for_pickup_pos_1 = (MOD_BIT(KC_LEFT_CTRL) | MOD_BIT(KC_LEFT_ALT));
  user_config.mods_for_pickup_pos_2 = (MOD_BIT(KC_LEFT_ALT));
  user_config.mods_for_pickup_pos_3 = (MOD_BIT(KC_LEFT_CTRL));
  eeconfig_update_user(user_config.raw); // Write BASE value to EEPROM now
}

// fretboard lookup table for notes
const float fretboard_notes[][15] = {
  { NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_BF5, NOTE_B5, NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_EF6, NOTE_E6, NOTE_F6, NOTE_FS6 },
  { NOTE_B4, NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_EF5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_BF5, NOTE_B5, NOTE_C6, NOTE_CS6 },
  { NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_BF4, NOTE_B4, NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_EF5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5 },
  { NOTE_D4, NOTE_EF4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_BF4, NOTE_B4, NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_EF5, NOTE_E5 },
  { NOTE_A3, NOTE_BF3, NOTE_B3, NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_EF4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_BF4, NOTE_A4 },
  { NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_BF3, NOTE_B3, NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_EF4, NOTE_E4, NOTE_F4, NOTE_FS4 },
  { NOTE_D3, NOTE_EF3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_BF3, NOTE_B3, NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_EF4, NOTE_E4 } // drop D
};

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
  else if (keycode == KC_CAPS && user_config.ctrl_replaces_caps && row == 1 && col == 0) { keycode = KC_LEFT_CTRL; }

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

int get_pickup_selector_mods(int pos) {
  switch(pos) {
    case 0: return user_config.mods_for_pickup_pos_0;
    case 1: return user_config.mods_for_pickup_pos_1;
    case 2: return user_config.mods_for_pickup_pos_2;
    case 3: return user_config.mods_for_pickup_pos_3;
    case 4: return user_config.mods_for_pickup_pos_4;
  }
  return 0;
}

// update mods based on pickup selector pos
void update_mods(void) {
  int pos = get_pickup_selector_pos();
  int mods = get_pickup_selector_mods(pos);
  unregister_mods(get_mods());

  xprintf("PU pos: %u setting mod state to: %u\n", pos, mods);
  register_mods(mods);
}

// get the fretboard note depending on layer and drop d tuning
int matrix_row_to_fretboard_row(int row) {
  row = row + 3 - 3*IS_LAYER_ON(NUM); // upper three strings when set to NUM layer
  if (user_config.drop_d && row == 5) { // tuned to drop D
    row++;
  }
  return row;
}

// play audio
void audio_process_notes(void) {
  audio_stop_all();

  float tone;
  for (int r = 0; r < 3; r++) {
    tone = -1;

    if (is_strum_held(r)) {
      if (get_highest_fret(r) >= 0) {
        tone = fretboard_notes[matrix_row_to_fretboard_row(r)][get_highest_fret(r)+1];
      }
      else {
        tone = fretboard_notes[matrix_row_to_fretboard_row(r)][0];
      }
    }

    if (tone >= 0) { audio_play_note(tone, 64*IS_LAYER_ON(NOSUS)); }
  }
}


// unregister entire row
void unregister_row_except_col(int row, int ignore_col) {

  for (int f = 0; f < num_frets[row]; f++) {
    if (f != ignore_col || IS_LAYER_ON(NOSUS)) {

      // do not unregister shift mods if they happen to already be held
      // this is in case user has set pickup selector mod to include shift
      if (row == 2
           && get_pickup_selector_mods(get_pickup_selector_pos()) & MOD_BIT(KC_LEFT_SHIFT)
           && get_fret_keycode(row, f) == KC_LEFT_SHIFT
         ) {
        continue;
      }
      else if (row == 1
           && get_pickup_selector_mods(get_pickup_selector_pos()) & MOD_BIT(KC_LEFT_CTRL)
           && get_fret_keycode(row, f) == KC_LEFT_CTRL
         ) {
        continue;
      }

      unregister_code(get_fret_keycode(row, f));
    }
  }
}
void unregister_row(int row) {
  unregister_row_except_col(row, -1);
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
      if (IS_LAYER_ON(NOSUS)) {
        tap_code(KC_SPACE);
      }
      else {
        register_code(KC_SPACE);
      }
      //return;
    }
  }
}



// register keycode corresponding to fret
void register_fret(int row, int col) {

  // unregister any other frets that might be held or unregister all if no sustain
  unregister_row_except_col(row, col);

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
}



bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  //xprintf("KL: col: %u, row: %u, pressed: %u\n", record->event.key.col, record->event.key.row, record->event.pressed);

  // check if user is configuring
  if (IS_LAYER_ON(CONFIG)) {
    // BACKSLASH - toggle backslash as backspace
    if (record->event.pressed && record->event.key.row == 0 && record->event.key.col == 13) {
      user_config.backspace_replaces_backslash = !user_config.backspace_replaces_backslash;
      audio_play_note(fretboard_notes[!user_config.backspace_replaces_backslash][0], 64);
      xprintf("CONFIG: backspace replaces backslash set to %u\n", user_config.backspace_replaces_backslash);
      return false;
    }
    // TAB - toggle escape as tab
    else if (record->event.pressed && record->event.key.row == 0 && record->event.key.col == 0) {
      user_config.escape_replaces_tab = !user_config.escape_replaces_tab;
      audio_play_note(fretboard_notes[!user_config.escape_replaces_tab][0], 64);
      xprintf("CONFIG: escape replaces tab set to %u\n", user_config.escape_replaces_tab);
      return false;
    }
    // CAPS - toggle ctrl as caps
    else if (record->event.pressed && record->event.key.row == 1 && record->event.key.col == 0) {
      user_config.ctrl_replaces_caps = !user_config.ctrl_replaces_caps;
      audio_play_note(fretboard_notes[!user_config.ctrl_replaces_caps][0], 64);
      xprintf("CONFIG: ctrl replaces caps set to %u\n", user_config.ctrl_replaces_caps);
      return false;
    }
    // MINUS - these keyboards go to 11
    else if (record->event.pressed && record->event.key.row == 0 && record->event.key.col == 11) {
      user_config.spinal_tap = !user_config.spinal_tap;
      audio_play_note(fretboard_notes[!user_config.spinal_tap][0], 64);
      xprintf("CONFIG: these keyboards go to 1%u\n", user_config.spinal_tap);
      return false;
    }
    // D - toggle drop d tuning
    else if (record->event.pressed && record->event.key.row == 1 && record->event.key.col == 3) {
      user_config.drop_d = !user_config.drop_d;
      audio_play_note(fretboard_notes[!user_config.drop_d][0], 64);
      xprintf("CONFIG: drop d tuning set to %u\n", user_config.drop_d);
      return false;
    }
    // A - toggle audio
    else if (record->event.pressed && record->event.key.row == 1 && record->event.key.col == 1) {
      user_config.enable_audio = !user_config.enable_audio;
      audio_play_note(fretboard_notes[!user_config.enable_audio][0], 64);
      xprintf("CONFIG: enable audio set to %u\n", user_config.enable_audio);
      return false;
    }
    // MIDDLE STRUM BAR - toggle simple mode
    else if (record->event.pressed && record->event.key.row == 3 && (record->event.key.col == 1 || record->event.key.col == 4)) {
      user_config.simple_mode = !user_config.simple_mode;
      audio_play_note(fretboard_notes[!user_config.simple_mode][0], 64);
      xprintf("CONFIG: simple mode set to %u\n", user_config.simple_mode);
      return false;
    }
    // ENTER - save currently held mods to current pickup position
    else if (record->event.pressed && record->event.key.row == 1 && record->event.key.col == 12) {
      audio_play_note(fretboard_notes[0][0], 64);
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

    // only check for keys on the fretboard
    if (IS_QK_BASIC(keycode) && record->event.key.row >= 0 && record->event.key.row <= 2 && record->event.key.col < num_frets[record->event.key.row]) {
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

    // check if pressed key is out of bounds of fretboard (e.g. 1,13 and 2,13)
    if (record->event.key.col > num_frets[record->event.key.row]-1) { return true; }

    // ignore event if it's on a lower fret than what's currently held
    if (record->event.key.col < get_highest_fret(record->event.key.row)) { return false; }

    // ignore fret press if strumbar not pressed on that row
    if (!is_strum_held(record->event.key.row) && !user_config.simple_mode) { return false; }
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
      if (!is_fretted && !IS_LAYER_ON(NOSUS)) {
        register_space();
      }
    }
    return false;
  }


  // process keypresses for non-simple mode
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
        if (!IS_LAYER_ON(NOSUS)) { register_space(); } // revert back to space since a strum bar is held
      }

    } else { // if strum bar is not held, release all keys
      unregister_row(r);
    }
  }

  // play buzzer
  if (user_config.enable_audio
      && !(!record->event.pressed && IS_LAYER_ON(NOSUS)) // if NOSUS is on, do not do anything on release
     ) { audio_process_notes(); }

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
    unregister_mods(get_mods());
    xprintf("CONFIG layer activated\n");

    xprintf("current settings:\n");
    xprintf("- simple mode: %u\n", user_config_previous.simple_mode);
    xprintf("- backspace_replaces_backslash: %u\n", user_config_previous.backspace_replaces_backslash);
    xprintf("- escape_replaces_tab: %u\n", user_config_previous.escape_replaces_tab);
    xprintf("- ctrl_replaces_caps: %u\n", user_config_previous.ctrl_replaces_caps);
    xprintf("- encoder_reverse_direction: %u\n", user_config_previous.encoder_reverse_direction);
    xprintf("- drop_d: %u\n", user_config_previous.drop_d);
    xprintf("- enable_audio: %u\n", user_config_previous.enable_audio);
    xprintf("- these keyboards go to 1%u\n", user_config_previous.spinal_tap);
    xprintf("- mods_for_pickup_pos_0: %u\n", user_config_previous.mods_for_pickup_pos_0);
    xprintf("- mods_for_pickup_pos_1: %u\n", user_config_previous.mods_for_pickup_pos_1);
    xprintf("- mods_for_pickup_pos_2: %u\n", user_config_previous.mods_for_pickup_pos_2);
    xprintf("- mods_for_pickup_pos_3: %u\n", user_config_previous.mods_for_pickup_pos_3);
    xprintf("- mods_for_pickup_pos_4: %u\n", user_config_previous.mods_for_pickup_pos_4);

    config_layer_on = true;
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

bool encoder_update_user(uint8_t index, bool clockwise) {
  // check if user is configuring
  if (IS_LAYER_ON(CONFIG)) {
    // user to turn encoder clockwise
    // if we get clockwise = 0 when they do this, the encoder direction should be reversed
    user_config.encoder_reverse_direction = !clockwise;
    audio_play_note(fretboard_notes[user_config.encoder_reverse_direction][0], 64);
    xprintf("CONFIG: reverse encoder direction set to %u\n", !clockwise);
    return false;
  }

  // check if user has config reverse direction
  if (user_config.encoder_reverse_direction) { clockwise = !clockwise; }

  if (clockwise) { tap_code_delay(KC_VOLU, 10); }
  else { tap_code_delay(KC_VOLD, 10); }

  return false; // default behaviour is vol control which we already do above
}
