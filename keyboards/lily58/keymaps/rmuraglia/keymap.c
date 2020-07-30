#include QMK_KEYBOARD_H

// to do:
// finish numpad layer, maybe add a tapdance to toggle on
// leader or layer for complex shortcuts (e.g. sublime cmd palette, origami, magnet)
// figure out how to move by half screen, like vim c-u, c-d

// Potential changes:
// deprecate training wheel top right backspace/del. Replace with +
// deprecate training wheel bottom right shift return
// deprecate training wheel top left tab
// monitor use level of left corner shift. currently basically only for cmd shift t (undo close tab) and shift tab (unindent)
// restore mod tap shift on space. adjust per key tapping term or set permissive hold.
// for finicky cmds (e.g. ctrl U del line left doesn't work in sublime) consider a macro that does selection then deletion
// add another space on the left side (on opt) for when right hand is on mouse

// refs:
// https://github.com/qmk/qmk_firmware/blob/master/keyboards/lily58/keymaps/bcat/keymap.c
// https://github.com/qmk/qmk_firmware/blob/master/keyboards/sofle/keymaps/default/keymap.c
// https://config.qmk.fm/#/test/

#ifdef PROTOCOL_LUFA
  #include "lufa.h"
  #include "split_util.h"
#endif

extern uint8_t is_master;

enum layers {
  _QWERTY,
  _SYM_NAV,
  _MDS,
  _FN_NUM,
};

#define KY_MWL LOPT(KC_LEFT)   // move   word left
#define KY_MWR LOPT(KC_RIGHT)  // move   word right
#define KY_MLL LCTL(KC_A)      // move   line left
#define KY_MLR LCTL(KC_E)      // move   line right
#define KY_DWL LOPT(KC_BSPC)   // delete word left
#define KY_DWR LOPT(KC_DEL)    // delete word right
#define KY_DLL LCTL(KC_U)      // delete line left
#define KY_DLR LCTL(KC_K)      // delete line right
#define KY_SCL LSFT(KC_LEFT)   // select char left
#define KY_SCR LSFT(KC_RIGHT)  // select char right
#define KY_SWL LSFT(KY_MWL)    // select word left
#define KY_SWR LSFT(KY_MWR)    // select word right
#define KY_SLL LSFT(KY_MLL)    // select line left
#define KY_SLR LSFT(KY_MLR)    // select line right

enum combo_events {
  ZOOM_PLUS,
  ZOOM_MINUS,
  ZOOM_RESET
};

const uint16_t PROGMEM zoom_plus_combo[] = {KC_RCTL, KC_QUOT, COMBO_END};
const uint16_t PROGMEM zoom_minus_combo[] = {KC_RCTL, KC_MINS, COMBO_END};
const uint16_t PROGMEM zoom_reset_combo[] = {KC_RCTL, KC_0, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
  [ZOOM_PLUS] = COMBO_ACTION(zoom_plus_combo),
  [ZOOM_MINUS] = COMBO_ACTION(zoom_minus_combo),
  [ZOOM_RESET] = COMBO_ACTION(zoom_reset_combo)
};

void process_combo_event(uint8_t combo_index, bool pressed) {
  switch(combo_index) {
    case ZOOM_PLUS:
      if (pressed) {
        tap_code16(RGUI(KC_EQUAL));
      }
      break;
    case ZOOM_MINUS:
      if (pressed) {
        tap_code16(RGUI(KC_MINS));
      }
      break;
    case ZOOM_RESET:
      if (pressed) {
        tap_code16(RGUI(KC_0));
      }
      break;
  }
};

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LSFT_T(KC_BSPC):
            return 150;
        case RSFT_T(KC_SPC):
            return 150;
        default:
            return TAPPING_TERM;
    }
}

bool get_ignore_mod_tap_interrupt(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LSFT_T(KC_BSPC):
            return true;
        case RSFT_T(KC_SPC):
            return true;
        default:
            return false;
    }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* QWERTY
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |  `~  |   1! |   2@ |   3# |   4$ |   5% |                    |   6^ |   7& |   8* |   9( |   0) | BSPC |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |  -_  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |EscCtl|   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;: |  '"  |
 * |------+------+------+------+------+------|   MDS |    |  MDS  |------+------+------+------+------+------|
 * |LShift|   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,< |   .> |   /? |RShift|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | LAlt | Esc  | Tab  | /BackSP /       \Space \  | Enter |  Del | Ctrl |
 *                   |      | LGUI |FN_NUM|/ Shift /         \ Shift\ |SYM_NAV| RGUI |      |
 *                   `----------------------------'           '------''---------------------'
 */

 [_QWERTY] = LAYOUT( \
  KC_GRV,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC, \
  KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS, \
  LCTL_T(KC_ESC), KC_A, KC_S, KC_D,   KC_F,    KC_G,                     KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, \
  KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B, MO(_MDS), MO(_MDS), KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSHIFT, \
  KC_LALT, LGUI_T(KC_ESC), LT(_FN_NUM, KC_TAB), LSFT_T(KC_BSPC), RSFT_T(KC_SPC), LT(_SYM_NAV, KC_ENT), RGUI_T(KC_DEL), KC_RCTL  \
),
/* SYM_NAV: symbols and navigation
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |   |  |   }  |   ]  |   )  |   +  |                    | Home | PgDn | PgUp | End  |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |   \  |   {  |   [  |   (  |   =  |-------.    ,-------| Left | Down |  Up  | Right|      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      |      |      |      |      | Enter|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /BackSP /       \Space \  |      |  Del |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_SYM_NAV] = LAYOUT( \
  _______, _______, _______, _______, _______, _______,                   XXXXXXX, XXXXXXX, XXXXXXX,XXXXXXX, XXXXXXX, _______, \
  _______, KC_PIPE, KC_RCBR, KC_RBRC, KC_RPRN,  KC_PLUS,                  KC_HOME, KC_PGDN, KC_PGUP, KC_END, XXXXXXX, _______, \
  _______, KC_BSLS, KC_LCBR, KC_LBRC, KC_LPRN, KC_EQUAL,                  KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, XXXXXXX, _______, \
  _______, _______, _______, _______, _______, _______, _______, _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_ENT, \
                    _______, _______, _______, KC_BSPC,                   KC_SPC,  _______, KC_DEL, _______ \
),
/* MDS: Movement, deletion and selection
 *                      |        LEFT        |                    |        RIGHT       |
 *                      | LINE | WORD | CHAR |                    | CHAR | WORD | LINE |
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |  DLL |  DWL | BSPC |                    |  Del |  DWR |  DLR |      |      |      |   DELETE
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |  MLL |  MWL | Left |-------.    ,-------| Right|  MWR |  MLR |      |      |      |   MOVE
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |      |  SLL |  SWL |  SCL |-------|    |-------|  SCR |  SWR |  SLR |      |      | Enter|   SELECT
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /BackSP /       \Space \  |      |  Del |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */

[_MDS] = LAYOUT( \
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
  XXXXXXX, XXXXXXX, XXXXXXX,  KY_DLL,  KY_DWL, KC_BSPC,                    KC_DEL,  KY_DWR,  KY_DLR, XXXXXXX, XXXXXXX, XXXXXXX, \
  XXXXXXX, XXXXXXX, XXXXXXX,  KY_MLL,  KY_MWL, KC_LEFT,                  KC_RIGHT,  KY_MWR,  KY_MLR, XXXXXXX, XXXXXXX, XXXXXXX, \
  XXXXXXX, XXXXXXX, XXXXXXX,  KY_SLL,  KY_SWL,  KY_SCL, _______, _______,  KY_SCR,  KY_SWR,  KY_SLR, XXXXXXX, XXXXXXX, KC_ENT, \
                    _______, _______, _______, KC_BSPC,                   KC_SPC,  _______, KC_DEL, _______ \
),
/* FN_NUM: Functions and numpad (right hand numpad not yet implemented)
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |  Del |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |  Up |Bright+| Next | Vol+ |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      | Left | Down | Right| Play | Mute |-------.    ,-------|      |      |      |      |      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |     |Bright-| Prev | Vol- |-------|    |-------|      |      |      |      |      | Enter|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /BackSP /       \Space \  |      |  Del |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
  [_FN_NUM] = LAYOUT( \
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_DEL, \
  XXXXXXX, XXXXXXX,   KC_UP,   KC_F2,   KC_F9,  KC_F12,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
  XXXXXXX, KC_LEFT, KC_DOWN, KC_RIGHT,  KC_F8,  KC_F10,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
  XXXXXXX, XXXXXXX, XXXXXXX,   KC_F1,   KC_F7,  KC_F11, _______, _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_ENT, \
                    _______, _______, _______, KC_BSPC,                   KC_SPC,  _______, KC_DEL, _______ \
)
};
