#include QMK_KEYBOARD_H

// to do:
// figure out how to move by half screen, like vim c-u, c-d
// refs:
// https://github.com/qmk/qmk_firmware/blob/master/keyboards/lily58/keymaps/bcat/keymap.c
// https://github.com/qmk/qmk_firmware/blob/master/keyboards/sofle/keymaps/default/keymap.c
// https://config.qmk.fm/#/test/
// use super alt tab: https://beta.docs.qmk.fm/using-qmk/advanced-keycodes/feature_macros#super-alt-tab

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
#define KY_CAR LGUI(KC_TAB)    // change app right
#define KY_CAL LSFT(KY_CAR)    // change app left
#define KY_CWR LGUI(KC_GRV)    // change window right
#define KY_CWL LSFT(KY_CWR)    // change window left
#define KY_CTR LCTL(KC_TAB)    // change tab right
#define KY_CTL LSFT(KY_CTR)    // change tab left
#define KY_CSL LCTL(LGUI(LOPT(KC_LEFT)))   // change space left
#define KY_CSR LCTL(LGUI(LOPT(KC_RIGHT)))  // change space right

// chunk for my defined magnet shortcuts for window management
#define MAG_L13 LCTL(LOPT(KC_LEFT))   // magnet left 1/3
#define MAG_L12 LGUI(LOPT(KC_LEFT))   // magnet left 1/2
#define MAG_L23 LCTL(LGUI(KC_LEFT))   // magnet left 2/3
#define MAG_R13 LCTL(LOPT(KC_RIGHT))  // magnet right 1/3
#define MAG_R12 LGUI(LOPT(KC_RIGHT))  // magnet right 1/2
#define MAG_R23 LCTL(LGUI(KC_RIGHT))  // magnet right 2/3
#define MAG_C13 LCTL(LOPT(KC_DOWN))   // magnet center 1/3
#define MAG_FW  LGUI(LOPT(KC_UP))     // magnet full width
#define MAG1 KC_F13  // alias obscure F-keys for magnet comboing
#define MAG2 KC_F14  // alias obscure F-keys for magnet comboing
#define MAG3 KC_F15  // alias obscure F-keys for magnet comboing
#define MAG4 KC_F16  // alias obscure F-keys for magnet comboing
#define MAG5 KC_F17  // alias obscure F-keys for magnet comboing

// use combos to compress (left|right|center)x(1/2|1/3|2/3) + full width to just 5 keys
// left, center, right thirds: 1, 3, 5
// left, right halves: (1,2), (4,5)
// left, right 2/3rds: (1,3), (3,5)
// full width: (3,4,5)
enum combo_events {
  ML13,
  ML12,
  ML23,
  MR13,
  MR12,
  MR23,
  MC13,
  MFW,
};

const uint16_t PROGMEM ml13[] = {MAG1, COMBO_END};
const uint16_t PROGMEM ml12[] = {MAG1, MAG2, COMBO_END};
const uint16_t PROGMEM ml23[] = {MAG1, MAG3, COMBO_END};
const uint16_t PROGMEM mr13[] = {MAG5, COMBO_END};
const uint16_t PROGMEM mr12[] = {MAG5, MAG4, COMBO_END};
const uint16_t PROGMEM mr23[] = {MAG5, MAG3, COMBO_END};
const uint16_t PROGMEM mc13[] = {MAG3, COMBO_END};
const uint16_t PROGMEM mfw[] = {MAG2, MAG3, MAG4, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
  [ML13] = COMBO_ACTION(ml13),
  [ML12] = COMBO_ACTION(ml12),
  [ML23] = COMBO_ACTION(ml23),
  [MR13] = COMBO_ACTION(mr13),
  [MR12] = COMBO_ACTION(mr12),
  [MR23] = COMBO_ACTION(mr23),
  [MC13] = COMBO_ACTION(mc13),
  [MFW] = COMBO_ACTION(mfw),
};

void process_combo_event(uint8_t combo_index, bool pressed) {
  switch(combo_index) {
    case ML13:
      if (pressed) {
        tap_code16(MAG_L13);
      }
      break;
    case ML12:
      if (pressed) {
        tap_code16(MAG_L12);
      }
      break;
    case ML23:
      if (pressed) {
        tap_code16(MAG_L23);
      }
      break;
    case MR13:
      if (pressed) {
        tap_code16(MAG_R13);
      }
      break;
    case MR12:
      if (pressed) {
        tap_code16(MAG_R12);
      }
      break;
    case MR23:
      if (pressed) {
        tap_code16(MAG_R23);
      }
      break;
    case MC13:
      if (pressed) {
        tap_code16(MAG_C13);
      }
      break;
    case MFW:
      if (pressed) {
        tap_code16(MAG_FW);
      }
      break;
  }
};


uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(_SYM_NAV, KC_ENT):
            return 100;
        case LT(_FN_NUM, KC_TAB):
            return 100;
        case RSFT_T(KC_SPC):
          return 120;
        default:
            return TAPPING_TERM;
    }
};

// for space/shift, I tend to not key up the space before inputting the first letter of the next word
// normally this forces the mod to register, so I get a camelCaseSentence
// with imti, the space must be held for the tapping term to register as a shift, making it better for sloppy fast typing with overlapping keypresses
// we don't want imti on any of the other mods, because then we would need to slow down for quick shortcuts like cmd + C to register
bool get_ignore_mod_tap_interrupt(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LSFT_T(KC_BSPC):
            return true;
        case RSFT_T(KC_SPC):
            return true;
        default:
            return false;
    }
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* QWERTY
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |  `~  |   1! |   2@ |   3# |   4$ |   5% |                    |   6^ |   7& |   8* |   9( |   0) | BSPC |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |LEADER|   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |  -_  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |EscCtl|   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;: |  '"  |
 * |------+------+------+------+------+------|  MDS  |    |  MDS  |------+------+------+------+------+------|
 * |LShift|   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,< |   .> |   /? |RShift|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | LAlt | Esc  | Tab  | /BackSP /       \Space \  | Enter |  Del | Ctrl |
 *                   |      | LGUI |FN_NUM|/ Shift /         \ Shift\ |SYM_NAV| RGUI |      |
 *                   `----------------------------'           '------''---------------------'
 */

 [_QWERTY] = LAYOUT( \
  KC_GRV,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC, \
  KC_LEAD,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS, \
  LCTL_T(KC_ESC), KC_A, KC_S, KC_D,   KC_F,    KC_G,                     KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, \
  KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B, MO(_MDS), MO(_MDS), KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSHIFT, \
  KC_LALT, LGUI_T(KC_ESC), LT(_FN_NUM, KC_TAB), LSFT_T(KC_BSPC), RSFT_T(KC_SPC), LT(_SYM_NAV, KC_ENT), RGUI_T(KC_DEL), KC_RCTL  \
),
/* SYM_NAV: symbols and navigation
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |   |  |      |   [  |   ]  |   +  |                    | Home | PgDn | PgUp | End  |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |   (  |   )  |   =  |-------.    ,-------| Left | Down |  Up  | Right|      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |      |   {  |   }  |   \  |-------|    |-------|      |      |      |      |      | Enter|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /BackSP /       \Space \  |      |  Del |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_SYM_NAV] = LAYOUT( \
  _______, MAG1, MAG2, MAG3, MAG4, MAG5,                   _______, _______, _______,_______, _______, _______, \
  _______, KC_PIPE, _______, KC_LBRC, KC_RBRC,  KC_PLUS,                  KC_HOME, KC_PGDN, KC_PGUP, KC_END, _______, _______, \
  _______, _______, _______, KC_LPRN, KC_RPRN, KC_EQUAL,                  KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, _______, _______, \
  _______, _______, _______, KC_LCBR, KC_RCBR, KC_BSLS, _______,   _______, _______, _______, _______, _______, _______, KC_ENT, \
                    _______, _______, _______, KC_BSPC,                   KC_SPC,  _______, KC_DEL, _______ \
),
/* MDS: Movement, deletion and selection
 * note: vertical movement optimized for sublime text 3, as in iterm I don't really care, and in vim I'll just dd the line or yank the section
 *
 * |         UP         |        LEFT        |                    |        RIGHT       |        DOWN        |
 * |  DOC | PAGE | LINE | LINE | WORD | CHAR |                    | CHAR | WORD | LINE | LINE | PAGE | DOC  |
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      | SPCL | TABL | WINL | APPL |                    | APPR | WINR | TABR | SPCR |      |      |   FOCUS SWITCHING (application, window, tab, space)
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
  _______, _______,  KY_CSL,  KY_CTL,  KY_CWL,  KY_CAL,                    KY_CAR,  KY_CWR,  KY_CTR,  KY_CSR, _______, _______, \
  _______, _______, _______,  KY_DLL,  KY_DWL, KC_BSPC,                    KC_DEL,  KY_DWR,  KY_DLR, _______, _______, _______, \
  _______, _______, _______,  KY_MLL,  KY_MWL, KC_LEFT,                  KC_RIGHT,  KY_MWR,  KY_MLR, _______, _______, _______, \
  _______, _______, _______,  KY_SLL,  KY_SWL,  KY_SCL, _______, _______,  KY_SCR,  KY_SWR,  KY_SLR, _______, _______, KC_ENT, \
                    _______, _______, _______, KC_BSPC,                   KC_SPC,  _______, KC_DEL, _______ \
),
/* FN_NUM: Functions and numpad (right hand numpad not yet implemented)
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |   ^  |  tab |  *   |   -  |   )  | BSPC |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |  Up |Bright+| Next | Vol+ |                    |   7  |   8  |  9   |   +  |   (  |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      | Left | Down | Right| Play | Mute |-------.    ,-------|   4  |   5  |  6   |   =  |      |      |
 * |------+------+------+------+------+------|       |    |   0   |------+------+------+------+------+------|
 * |      |      |     |Bright-| Prev | Vol- |-------|    |-------|   1  |   2  |  3   |   .  |   /  | Enter|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /BackSP /       \Space \  |  Ent |  DEL |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
  [_FN_NUM] = LAYOUT( \
  _______, _______, _______, _______, _______, _______,                   KC_CIRC,  KC_TAB, KC_ASTR, KC_MINS, KC_RPRN, KC_BSPC, \
  _______, _______,   KC_UP,   KC_F2,   KC_F9,  KC_F12,                      KC_7,    KC_8,    KC_9, KC_PLUS, KC_LPRN, _______, \
  _______, KC_LEFT, KC_DOWN, KC_RIGHT,  KC_F8,  KC_F10,                      KC_4,    KC_5,    KC_6, KC_EQUAL, _______, _______, \
  _______, _______, _______,   KC_F1,   KC_F7,  KC_F11, _______,    KC_0,    KC_1,    KC_2,    KC_3, KC_DOT, KC_SLSH, KC_ENT, \
                    _______, _______, _______, KC_BSPC,                    KC_SPC,  KC_ENT, KC_DEL, _______ \
)
};

// Leader key for application specific shortcuts that are annoying to hit on a split board
LEADER_EXTERNS();

void matrix_scan_user(void) {
  LEADER_DICTIONARY() {
    leading = false;
    leader_end();

    SEQ_ONE_KEY(KC_Q) {  // OSX lock screen (quit)
      tap_code16(RGUI(RCTL(KC_Q)));
    }
    SEQ_ONE_KEY(KC_F) {  // spotlight (find)
      SEND_STRING(SS_LGUI(" "));
    }
    SEQ_ONE_KEY(KC_S) {  // screenshot
      tap_code16(LGUI(LSFT(KC_4)));
    }
    SEQ_TWO_KEYS(KC_S, KC_L) {  // spotlight -> go to slack
      SEND_STRING(SS_LGUI(" ") SS_DELAY(50) "slack" SS_TAP(X_ENT));
    }
    // to do: Sublime Repl Line, Sublime Repl File, Sublime Command Palette, Sublime # Col, Sublime # Row, Sublime 4 Grid
    // Origami Carry HJKL, Origami Destroy HJKL, Sublime New View (into file)
    // tmux splits and focus (in meantime, iterm split horizontal, iterm split vertical)
  }
}
