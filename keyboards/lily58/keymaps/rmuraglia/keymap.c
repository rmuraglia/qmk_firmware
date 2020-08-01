#include QMK_KEYBOARD_H

// to do:
// finish numpad layer, maybe add a tapdance to toggle on
// finish vertical movement in MDS layer. may require macros for deletion (select then delete, or move then ctrl shift k)
// leader or layer for complex shortcuts (e.g. sublime cmd palette, origami, magnet)
  // leader key cannot be on a tap dance or mod tap. consider putting it on a combo like FJ?
  // currently existing leader combo (FJ) doesn't work... makes a T instead?
// figure out how to move by half screen, like vim c-u, c-d

// Potential changes:
// deprecate training wheel degenerate keys (tab, escCtrl, shifts, backspace)
// put equal on tap dance for minus, and plus on tap dance for quote (matches keycap legend)
  // problem here is that those keys are already used in combos. if I turn them into tapdances, then they won't be combo compatible basic keycodes anymore.
  // crazy quad function tapdance is possible:
  // minus: 1 tap minus, 2 tap dashdash, tap hold zoom out, 3 tap equal
  // quote: 1 tap quote, 2 tap  plus, tap hold zoom in, 3 tap triple quote (e.g. py docstring)
  // this is probably dumb though. just get better as using the sym layer quicky...

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

// want a zoom in (cmd+) on the right side of the layout near the native zoom out and zoom resets
// use the dedicated right control for the combo, because comboing with the mod tap cmd behaved strangely
// make combos for zoom out and zoom reset as well for consistency
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

// tap dance on inner column keys to have layer tap + custom key code
// most common use for keys is move by word, so put that as default, with option to go into layer if need other MDS action
enum tap_dances {
  LT_MWL,  // layer tap to MDS + move word left
  LT_MWR   // layer tap to MDS + move word right
};

typedef enum {
    SINGLE_TAP,
    SINGLE_HOLD
} td_state_t;

static td_state_t td_state;

uint8_t cur_dance(qk_tap_dance_state_t *state);
void ltmwl_finished(qk_tap_dance_state_t *state, void *user_data);
void ltmwl_reset(qk_tap_dance_state_t *state, void *user_data);
void ltmwr_finished(qk_tap_dance_state_t *state, void *user_data);
void ltmwr_reset(qk_tap_dance_state_t *state, void *user_data);

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LSFT_T(KC_BSPC):
            return 150;
        case RSFT_T(KC_SPC):
            return 150;
        case TD(LT_MWL):
            return 1;  // make tapping term for tap dance really short to minimize time before entering layer or for key repress
        case TD(LT_MWR):
            return 1;
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
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |  -_  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |EscCtl|   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;: |  '"  |
 * |------+------+------+------+------+------|MWR/MDS|    |MWL/MDS|------+------+------+------+------+------|
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
  KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B, TD(LT_MWR), TD(LT_MWL), KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSHIFT, \
  KC_LALT, LGUI_T(KC_ESC), LT(_FN_NUM, KC_TAB), LSFT_T(KC_BSPC), RSFT_T(KC_SPC), LT(_SYM_NAV, KC_ENT), RGUI_T(KC_DEL), KC_RCTL  \
),
/* SYM_NAV: symbols and navigation
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |   |  |   }  |   ]  |   )  |   +  |                    | Home | PgDn | PgUp | End  |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |   \  |   {  |   [  |   (  |   =  |-------.    ,-------| Left | Down |  Up  | Right|      |      |
 * |------+------+------+------+------+------|  MWR  |    |  MWL  |------+------+------+------+------+------|
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
  _______, _______, _______, _______, _______, _______, KY_MWR,   KY_MWL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_ENT, \
                    _______, _______, _______, KC_BSPC,                   KC_SPC,  _______, KC_DEL, _______ \
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
 * |------+------+------+------+------+------|  MWR  |    |  MWL  |------+------+------+------+------+------|
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
  XXXXXXX, XXXXXXX, XXXXXXX,   KC_F1,   KC_F7,  KC_F11, KY_MWR,   KY_MWL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_ENT, \
                    _______, _______, _______, KC_BSPC,                   KC_SPC,  _______, KC_DEL, _______ \
)
};

// Determine the tapdance state to return
uint8_t cur_dance(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return SINGLE_TAP;
        else return SINGLE_HOLD;
    }
    else return 2; // Any number higher than the maximum state value you return above
}

// Handle the possible states for each tapdance keycode you define:
void ltmwl_finished(qk_tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state);
    switch (td_state) {
        case SINGLE_TAP: register_code16(KY_MWL); break;
        case SINGLE_HOLD: layer_on(_MDS); break;
    }
}

void ltmwl_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (td_state) {
        case SINGLE_TAP: unregister_code16(KY_MWL); break;
        case SINGLE_HOLD: layer_off(_MDS); break;
    }
}

void ltmwr_finished(qk_tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state);
    switch (td_state) {
        case SINGLE_TAP: register_code16(KY_MWR); break;
        case SINGLE_HOLD: layer_on(_MDS); break;
    }
}

void ltmwr_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (td_state) {
        case SINGLE_TAP: unregister_code16(KY_MWR); break;
        case SINGLE_HOLD: layer_off(_MDS); break;
    }
}

// Define `ACTION_TAP_DANCE_FN_ADVANCED()` for each tapdance keycode, passing in `finished` and `reset` functions
qk_tap_dance_action_t tap_dance_actions[] = {
    [LT_MWL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, ltmwl_finished, ltmwl_reset),
    [LT_MWR] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, ltmwr_finished, ltmwr_reset)
};

// Leader key for application specific shortcuts that are annoying to hit on a split board
LEADER_EXTERNS();

void matrix_scan_user(void) {
  LEADER_DICTIONARY() {
    leading = false;
    leader_end();

    // SEQ_ONE_KEY(KC_Q) {  // OSX lock screen
    //   register_code16(RGUI(RCTL(KC_Q)));  // this one causes problems with mod keyups not being registered before locking
    // }
    SEQ_ONE_KEY(KC_F) {  // spotlight (find)
      SEND_STRING(SS_LGUI(" "));
    }
    SEQ_ONE_KEY(KC_S) {  // screenshot
      register_code16(LGUI(LSFT(KC_4)));
    }
    SEQ_TWO_KEYS(KC_S, KC_L) {  // spotlight -> go to slack
      SEND_STRING(SS_LGUI(" ") SS_DELAY(50) "slack" SS_TAP(X_ENT));
    }
    SEQ_THREE_KEYS(KC_M, KC_F, KC_S) {  // magnet, full screen
      register_code16(LGUI(LOPT(KC_UP)));
    }
    SEQ_FOUR_KEYS(KC_M, KC_H, KC_1, KC_3) {  // magnet, left, 1/3
      register_code16(LCTL(LOPT(KC_LEFT)));
    }
    SEQ_FOUR_KEYS(KC_M, KC_H, KC_1, KC_2) {  // magnet, left, 1/2
      register_code16(LGUI(LOPT(KC_LEFT)));
    }
    SEQ_FOUR_KEYS(KC_M, KC_H, KC_2, KC_3) {  // magnet, left, 2/3
      register_code16(LCTL(LGUI(KC_LEFT)));
    }
    SEQ_FOUR_KEYS(KC_M, KC_L, KC_1, KC_3) {  // magnet, right, 1/3
      register_code16(LCTL(LOPT(KC_RIGHT)));
    }
    SEQ_FOUR_KEYS(KC_M, KC_L, KC_1, KC_2) {  // magnet, right, 1/2
      register_code16(LGUI(LOPT(KC_RIGHT)));
    }
    SEQ_FOUR_KEYS(KC_M, KC_L, KC_2, KC_3) {  // magnet, right, 2/3
      register_code16(LCTL(LGUI(KC_RIGHT)));
    }
    SEQ_FOUR_KEYS(KC_M, KC_J, KC_1, KC_3) {  // magnet, middle, 1/3
      register_code16(LCTL(LOPT(KC_DOWN)));
    }
  }
}
