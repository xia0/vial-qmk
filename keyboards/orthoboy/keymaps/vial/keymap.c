#include QMK_KEYBOARD_H

enum rg_keycode {
  RG_MENU = QK_KB_0,
  RG_OPTION,
  RG_START,
  RG_SELECT,
  RG_UP,
  RG_DOWN,
  RG_LEFT,
  RG_RIGHT,
  RG_A,
  RG_B,
  RG_X,
  RG_Y,
  RG_L,
  RG_R
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [0] = LAYOUT(
      KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
      KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_DEL,
      KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
      KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT,
      KC_LCTL, MO(1),   KC_LALT, KC_LGUI, MO(1),   KC_SPC,  KC_SPC,  MO(1),   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
  ),
  [1] = LAYOUT(
      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, TG(2),
      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
      KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS,
      _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_NUHS, KC_NUBS, KC_PGUP, KC_PGDN, _______,
      _______, _______, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY
  ),
  [2] = LAYOUT(
      RG_MENU, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, RG_SELECT, RG_START, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, TG(2),
      XXXXXXX, XXXXXXX, RG_UP, XXXXXXX, XXXXXXX, XXXXXXX, RG_Y, RG_X, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
      XXXXXXX, RG_LEFT, RG_DOWN, RG_RIGHT, XXXXXXX, XXXXXXX, RG_B, RG_A, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
      RG_OPTION, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, RG_L, RG_R, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
  )

};

void keyboard_post_init_user(void) {
  gpio_set_pin_output(GP27);  // menu
  gpio_set_pin_output(GP28);  // option
  gpio_set_pin_output(GP29);  // start
  gpio_set_pin_output(GP0);   // select
  gpio_set_pin_output(GP1);   // up
  gpio_set_pin_output(GP2);   // down
  gpio_set_pin_output(GP3);   // left
  gpio_set_pin_output(GP4);   // right
  gpio_set_pin_output(GP5);   // A
  gpio_set_pin_output(GP6);   // B
  gpio_set_pin_output(GP7);   // X
  gpio_set_pin_output(GP8);   // Y
  gpio_set_pin_output(GP9);   // L
  gpio_set_pin_output(GP10);  // R

  gpio_write_pin_high(GP27);
  gpio_write_pin_high(GP28);
  gpio_write_pin_high(GP29);
  gpio_write_pin_high(GP0);
  gpio_write_pin_high(GP1);
  gpio_write_pin_high(GP2);
  gpio_write_pin_high(GP3);
  gpio_write_pin_high(GP4);
  gpio_write_pin_high(GP5);
  gpio_write_pin_high(GP6);
  gpio_write_pin_high(GP7);
  gpio_write_pin_high(GP8);
  gpio_write_pin_high(GP9);
  gpio_write_pin_high(GP10);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

	switch (keycode) {
		case RG_MENU:
      gpio_write_pin(GP27, !record->event.pressed);
      return 0;
    case RG_OPTION:
      gpio_write_pin(GP28, !record->event.pressed);
      return 0;
    case RG_START:
      gpio_write_pin(GP29, !record->event.pressed);
      return 0;
    case RG_SELECT:
      gpio_write_pin(GP0, !record->event.pressed);
      return 0;
    case RG_UP:
      gpio_write_pin(GP1, !record->event.pressed);
      return 0;
    case RG_DOWN:
      gpio_write_pin(GP2, !record->event.pressed);
      return 0;
    case RG_LEFT:
      gpio_write_pin(GP3, !record->event.pressed);
      return 0;
    case RG_RIGHT:
      gpio_write_pin(GP4, !record->event.pressed);
      return 0;
    case RG_A:
      gpio_write_pin(GP5, !record->event.pressed);
      return 0;
    case RG_B:
      gpio_write_pin(GP6, !record->event.pressed);
      return 0;
    case RG_X:
      gpio_write_pin(GP7, !record->event.pressed);
      return 0;
    case RG_Y:
      gpio_write_pin(GP8, !record->event.pressed);
      return 0;
    case RG_L:
      gpio_write_pin(GP9, !record->event.pressed);
      return 0;
    case RG_R:
      gpio_write_pin(GP10, !record->event.pressed);
      return 0;
	}

  return true;
}
