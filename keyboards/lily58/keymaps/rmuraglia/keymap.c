#include QMK_KEYBOARD_H

// to do:
// figure out how to move by half screen, like vim c-u, c-d
// rename the layers since the original layer names aren't relevant at all anymore

// from MDS I really only move by word or line, and move/select are kinda redundant (could be done just with select -- frankly del could too)
// consider condensing that 3x3x2 to just 2x2x2 which could fit above/below vim arrows
  // raise yuio: delete LL, LW, RW, RL
  // raise nm,.: select LL, LW, RW, RL
  // this means we can deprecate MDS layer
  // lower op l; for square and curly brackets
  // this frees up inner col for macros (e.g. alt tab)
  // this frees up bottom corners for ???

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
  _FN_NUM,
  _MDS,
};

// chunk for super alt tab
// https://beta.docs.qmk.fm/using-qmk/advanced-keycodes/feature_macros#super-alt-tab
bool is_alt_tab_active = false;
uint16_t alt_tab_timer = 0;

enum custom_keycodes {
  KY_CAR = SAFE_RANGE, // change app right (alt tab)
  KY_OFL,              // origami focus   left
  KY_OFD,              // origami focus   down
  KY_OFU,              // origami focus   up
  KY_OFR,              // origami focus   right
  KY_OCL,              // origami carry   left
  KY_OCD,              // origami carry   down
  KY_OCU,              // origami carry   up
  KY_OCR,              // origami carry   right
  KY_ODL,              // origami destroy left
  KY_ODD,              // origami destroy down
  KY_ODU,              // origami destroy up
  KY_ODR,              // origami destroy right
  KY_ODS,              // origami destroy self
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case KY_CAR:
      if (record->event.pressed) {  // do something on key press
        if (!is_alt_tab_active) {
          is_alt_tab_active = true;
          register_code(KC_LGUI);
        }
        alt_tab_timer = timer_read();
        register_code(KC_TAB);
      } else {  // do something on key release
        unregister_code(KC_TAB);
      }
      break;

    case KY_OFL:
      if (record->event.pressed) {
        SEND_STRING(SS_LGUI("k") SS_DELAY(10) SS_TAP(X_LEFT));
      }
      break;

    case KY_OFD:
      if (record->event.pressed) {
        SEND_STRING(SS_LGUI("k") SS_DELAY(10) SS_TAP(X_DOWN));
      }
      break;

    case KY_OFU:
      if (record->event.pressed) {
        SEND_STRING(SS_LGUI("k") SS_DELAY(10) SS_TAP(X_UP));
      }
      break;

    case KY_OFR:
      if (record->event.pressed) {
        SEND_STRING(SS_LGUI("k") SS_DELAY(10) SS_TAP(X_RIGHT));
      }
      break;

    case KY_OCL:
      if (record->event.pressed) {
        tap_code16(LGUI(KC_K));
        tap_code16(LSFT(KC_LEFT));
      }
      break;

    case KY_OCD:
      if (record->event.pressed) {
        tap_code16(LGUI(KC_K));
        tap_code16(LSFT(KC_DOWN));
      }
      break;

    case KY_OCU:
      if (record->event.pressed) {
        tap_code16(LGUI(KC_K));
        tap_code16(LSFT(KC_UP));
      }
      break;

    case KY_OCR:
      if (record->event.pressed) {
        tap_code16(LGUI(KC_K));
        tap_code16(LSFT(KC_RIGHT));
      }
      break;

    case KY_ODL:
      if (record->event.pressed) {
        tap_code16(LGUI(KC_K));
        tap_code16(LGUI(LSFT(KC_LEFT)));
      }
      break;

    case KY_ODD:
      if (record->event.pressed) {
        tap_code16(LGUI(KC_K));
        tap_code16(LGUI(LSFT(KC_DOWN)));
      }
      break;

    case KY_ODU:
      if (record->event.pressed) {
        tap_code16(LGUI(KC_K));
        tap_code16(LGUI(LSFT(KC_UP)));
      }
      break;

    case KY_ODR:
      if (record->event.pressed) {
        tap_code16(LGUI(KC_K));
        tap_code16(LGUI(LSFT(KC_RIGHT)));
      }
      break;

    case KY_ODS:
      if (record->event.pressed) {
        tap_code16(LGUI(KC_K));
        tap_code16(LGUI(LSFT(KC_0)));
      }
      break;
  }
  return true;  // process all other keycodes normally
}

// custom keycodes for MDS layer
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

// custom keycodes for window management
#define KY_CWR LGUI(KC_GRV)                 // change window right
#define KY_CWL LSFT(KY_CWR)                 // change window left
#define KY_CTR LCTL(KC_TAB)                 // change tab    right, note: ctrl(tab) and cmd(curly) are equivalent
#define KY_CTL LSFT(KY_CTR)                 // change tab    left
#define KY_CSR LCTL(LGUI(LOPT(KC_RIGHT)))   // change space  right
#define KY_CSL LSFT(KY_CSR)                 // change space  left

// hammerspoon aliases
#define HS_DIS  LGUI(LCTL(LOPT(KC_Q)))  // dismiss alerts
#define HS_INFO LGUI(LCTL(LOPT(KC_I)))  // date, battery, bluetooth info
#define HS_CAL  LGUI(LCTL(LOPT(KC_C)))  // calendar
#define HS_EXP  LGUI(LCTL(LOPT(KC_E)))  // expose
#define HS_FC1  LGUI(LCTL(LOPT(KC_1)))  // focus display 1
#define HS_FC2  LGUI(LCTL(LOPT(KC_2)))  // focus display 2
#define HS_FC3  LGUI(LCTL(LOPT(KC_3)))  // focus display 3
#define HS_TH1  LGUI(LCTL(LOPT(KC_6)))  // throw window to display 1
#define HS_TH2  LGUI(LCTL(LOPT(KC_7)))  // throw window to display 2
#define HS_TH3  LGUI(LCTL(LOPT(KC_8)))  // throw window to display 3
// todo: replace magnet window management with hs resizing

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

//  system shortcuts worth nothing
// cmd ctrl F: fullscreen
// cmd []    : history navigation
// cmd {}    : tab navigation (roughly equiv to ctrl tab -- some apps like iterm have weird overrides)
// cmd `     : within app window switcher

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

void process_combo_event(uint16_t combo_index, bool pressed) {
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

// make tapping term for layers very quick, and shorten for space (continue tweaking this)
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
 * | LEAD |   1! |   2@ |   3# |   4$ |   5% |                    |   6^ |   7& |   8* |   9( |   0) |  =+  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  `~  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |  -_  |
 * |------+------+------+------+------+------|-------.    ,-------|------+------+------+------+------+------|
 * | LCtrl|   A  |   S  |   D  |   F  |   G  |  [{   |    |   ]}  |   H  |   J  |   K  |   L  |   ;: |  '"  |
 * |------+------+------+------+------+------|  MDS  |    |  MDS  |------+------+------+------+------+------|
 * | LAlt |   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,< |   .> |   /? |  \|  |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      | Esc  | Tab  | /BackSP /       \Space \  | Enter |  Del |      |
 *                   |      | LGUI |FN_NUM|/ Shift /         \ Shift\ |SYM_NAV| RGUI |      |
 *                   `----------------------------'           '------''---------------------'
 */

 [_QWERTY] = LAYOUT( \
  KC_LEAD,  KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_EQUAL, \
  KC_GRV,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS, \
  KC_LCTRL, KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                     KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, \
  KC_LALT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B, LT(_MDS, KC_LBRC), LT(_MDS, KC_RBRC), KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_BSLASH, \
  XXXXXXX, LGUI_T(KC_ESC), LT(_FN_NUM, KC_TAB), LSFT_T(KC_BSPC), RSFT_T(KC_SPC), LT(_SYM_NAV, KC_ENT), RGUI_T(KC_DEL), XXXXXXX  \
),
/* SYM_NAV: symbols and navigation
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      | mag1 | mag2 | mag3 | mag4 | mag5 |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |  ODS |  ODL |  ODD |  ODU |  ODR |                    | Vol- | Mute | Vol+ | Bri+ |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |  OCL |  OCD |  OCU |  OCR |-------.    ,-------| Left | Down |  Up  | Right|      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |  OFL |  OFD |  OFU |  OFR |-------|    |-------| Prev | Play | Next | Bri- |      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /BackSP /       \Space \  |      |  Del |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_SYM_NAV] = LAYOUT( \
  _______,    MAG1,    MAG2,    MAG3,    MAG4,    MAG5,                   _______, _______, _______, _______, _______, _______, \
  _______,  KY_ODS,  KY_ODL,  KY_ODD,  KY_ODU,  KY_ODR,                   KC_F11,   KC_F10, KC_F12,    KC_F2, _______, _______, \
  _______, _______,  KY_OCL,  KY_OCD,  KY_OCU,  KY_OCR,                  KC_LEFT,  KC_DOWN,  KC_UP, KC_RIGHT, _______, _______, \
  _______, _______,  KY_OFL,  KY_OFD,  KY_OFU,  KY_OFR, _______, _______,  KC_F7,    KC_F8,  KC_F9,    KC_F1, _______, _______, \
                    _______, _______, _______, KC_BSPC,                   KC_SPC,  _______, KC_DEL, _______ \
),
/* FN_NUM: Functions and numpad
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |     |Dismiss| Info |Focus1|Focus2|Focus3|                    |   7  |   8  |  9   |      |      |      |
 * |------+------+------|------+------+------+                    |------+------+------+------+------+------|
 * |      |      |Expose| WkspL|MsnCtl| WkspR|-------.    ,-------|   4  |   5  |  6   |      |      |      |
 * |------+------+------|------+------+------+       |    |  tab  |------+------+------+------+------+------|
 * |      |      |  Cal |Throw1|Throw2|Throw3|-------|    |-------|   1  |   2  |  3   |      |      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /BackSP /       \Enter \  |   0  |   .  |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
  [_FN_NUM] = LAYOUT( \
  _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, \
  _______,  HS_DIS, HS_INFO,  HS_FC1,  HS_FC2,  HS_FC3,                      KC_7,    KC_8,    KC_9, _______, _______, _______, \
  _______, _______,  HS_EXP,  KY_CSL,   KC_F3,  KY_CSR,                      KC_4,    KC_5,    KC_6, _______, _______, _______, \
  _______, _______,  HS_CAL,  HS_TH1,  HS_TH2,  HS_TH3, _______,    KC_TAB,  KC_1,    KC_2,    KC_3, _______, _______, _______, \
                    _______, _______, _______, KC_BSPC,                    KC_ENT,    KC_0,  KC_DOT, _______ \
),
/* MDS: Movement, deletion and selection
 * note: vertical movement optimized for sublime text 3, as in iterm I don't really care, and in vim I'll just dd the line or yank the section
 *
 * |         UP         |        LEFT        |                    |        RIGHT       |        DOWN        |
 * |  DOC | PAGE | LINE | LINE | WORD | CHAR |                    | CHAR | WORD | LINE | LINE | PAGE | DOC  |
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |  DLL |  DWL | BSPC |                    |  Del |  DWR |  DLR |      |      |      |   DELETE
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |  MLL |  MWL | Left |-------.    ,-------| Right|  MWR |  MLR |      |      |      |   MOVE
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |      |      |      |  SLL |  SWL |  SCL |-------|    |-------|  SCR |  SWR |  SLR |      |      |      |   SELECT
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /BackSP /       \Space \  |      |  Del |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */

[_MDS] = LAYOUT( \
  _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, \
  _______, _______, _______,  KY_DLL,  KY_DWL, KC_BSPC,                    KC_DEL,  KY_DWR,  KY_DLR, _______, _______, _______, \
  _______, _______, _______,  KY_MLL,  KY_MWL, KC_LEFT,                  KC_RIGHT,  KY_MWR,  KY_MLR, _______, _______, _______, \
  _______, _______, _______,  KY_SLL,  KY_SWL,  KY_SCL, _______, _______,  KY_SCR,  KY_SWR,  KY_SLR, _______, _______, _______, \
                    _______, _______, _______, KC_BSPC,                    KC_SPC,  _______, KC_DEL, _______ \
),
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
    SEQ_ONE_KEY(KC_GRV) {  // generic code block
      SEND_STRING("```" SS_LSFT("\n\n") "```" SS_LSFT("\n") SS_TAP(X_UP) SS_TAP(X_UP));
    }
    SEQ_TWO_KEYS(KC_S, KC_L) {  // spotlight -> go to slack
      SEND_STRING(SS_LGUI(" ") SS_DELAY(50) "slack" SS_TAP(X_ENT));
    }
    SEQ_TWO_KEYS(KC_GRV, KC_S) {  // sql code block
      SEND_STRING("```sql" SS_LSFT("\n\n") "```" SS_LSFT("\n") SS_TAP(X_UP) SS_TAP(X_UP));
    }
    SEQ_TWO_KEYS(KC_GRV, KC_P) {  // python code block
      SEND_STRING("```python" SS_LSFT("\n\n") "```" SS_LSFT("\n") SS_TAP(X_UP) SS_TAP(X_UP));
    }
    SEQ_TWO_KEYS(KC_Y, KC_O) {  // yo link markdown format (with sublime multi cursor)
      SEND_STRING("[yo/\n](http://yo/\n)" SS_TAP(X_LEFT));
      tap_code16(LCTL(LSFT(KC_UP)));
      tap_code(KC_BSPC);
    }
    SEQ_TWO_KEYS(KC_V, KC_T) { // toggle vim number and list
      SEND_STRING(SS_TAP(X_ESC) ":set number! list!" SS_TAP(X_ENT));
    }
    SEQ_TWO_KEYS(KC_C, KC_C) { // copy command (from iterm)
      // enter copy code, go to beginning of line, right, right, start character copy, go to end of line, yank
      tap_code16(LGUI(LSFT(KC_C)));
      SEND_STRING("0llv$y");
    }
  }
  if (is_alt_tab_active) {
    if (timer_elapsed(alt_tab_timer) > 750) {
      unregister_code(KC_LGUI);
      is_alt_tab_active = false;
    }
  }
}
