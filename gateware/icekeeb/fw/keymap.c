/*
 * keymap.h
 *
 * Copyright (C) 2021 Piotr Esden-Tempski
 * All rights reserved.
 *
 * LGPL v3+, see LICENSE.lgpl3
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/* This file defines the mapping of the keyboard */

#include <stdint.h>
#include <stdio.h>

#include "keymap.h"
#include "keycode.h"
#include "quantum_keycodes.h"
#include "action_code.h"

#define XXX KC_NO

#define LAYOUT(                                                  \
  k00, k01, k02, k03, k04,           k05, k06, k07, k08, k09,    \
  k10, k11, k12, k13, k14,           k15, k16, k17, k18, k19,    \
  k20, k21, k22, k23, k24, k25, k26, k27, k28, k29, k2a, k2b,    \
  k30, k31, k32, k33, k34, k35, k36, k37, k38, k39, k3a, k3b     \
)                                                                \
{                                                                \
 { k00, k01, k02, k03, k04, XXX, XXX, k05, k06, k07, k08, k09 }, \
 { k10, k11, k12, k13, k14, XXX, XXX, k15, k16, k17, k18, k19 }, \
 { k20, k21, k22, k23, k24, k25, k26, k27, k28, k29, k2a, k2b }, \
 { k30, k31, k32, k33, k34, k35, k36, k37, k38, k39, k3a, k3b }  \
}

/* This layout is a slightly modified dvorak layout for the keyboardio Atreus keyboard.
 * This layout is closer to the keyboardio Model 01 keyboard default layout than the default Atreus layout is.
 */
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0] = LAYOUT(KC_QUOT, KC_COMM, KC_DOT,  KC_P,    KC_Y,                      KC_F,    KC_G,    KC_C,    KC_R,    KC_L,
                 KC_A,    KC_O,    KC_E,    KC_U,    KC_I,                      KC_D,    KC_H,    KC_T,    KC_N,    KC_S,
                 KC_SCLN, KC_Q,    KC_J,    KC_K,    KC_X,    KC_TAB,  KC_ENT,  KC_B,    KC_M,    KC_W,    KC_V,    KC_Z,
                 KC_ESC,  KC_GRV,  KC_LGUI, KC_LSFT, KC_BSPC, KC_LCTL, KC_LALT, KC_SPC,  MO(1),   KC_MINS, KC_SLSH, KC_BSLS),
	[1] = LAYOUT(KC_EXLM, KC_AT,   KC_UP,   KC_DLR,  KC_PERC,                   KC_PGUP, KC_7,    KC_8,    KC_9,    KC_BSPC,
                 KC_LPRN, KC_LEFT, KC_DOWN, KC_RGHT, KC_RPRN,                   KC_PGDN, KC_4,    KC_5,    KC_6,    KC_BSLS,
                 KC_LBRC, KC_RBRC, KC_HASH, KC_LCBR, KC_RCBR, KC_INS,  KC_AMPR, KC_ASTR, KC_1,    KC_2,    KC_3,    KC_PLUS,
                 TG(2),   KC_CIRC, KC_LGUI, KC_LSFT, KC_BSPC, KC_LCTL, KC_LALT, KC_SPC,  KC_TRNS, KC_DOT,  KC_0,    KC_EQL),
	[2] = LAYOUT(KC_INS,  KC_HOME, KC_UP,   KC_END,  KC_PGUP,                   KC_UP,   KC_F7,   KC_F8,   KC_F9,   KC_F10,
                 KC_DEL,  KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN,                   KC_DOWN, KC_F4,   KC_F5,   KC_F6,   KC_F11,
                 KC_NO,   KC_VOLU, KC_NO,   KC_NO,   RESET,   KC_TRNS, KC_TRNS, KC_NO,   KC_F1,   KC_F2,   KC_F3,   KC_F12,
                 KC_TRNS, KC_VOLD, KC_LGUI, KC_LSFT, KC_BSPC, KC_LCTL, KC_LALT, KC_SPC,  TO(0),   KC_PSCR, KC_SLCK, KC_PAUS)
};

static struct {
    int prev_layer;
    int active_layer;
} keymap_state;

uint16_t
keymap_get_code(unsigned int col, unsigned int row)
{
    return keymap_get_layer_code(keymap_state.active_layer, col, row);
}

uint16_t
keymap_get_layer_code(int layer, unsigned int col, unsigned int row)
{
    uint16_t code;
    do {
        code = keymaps[layer][row][col];
        layer--;
    } while ((code == KC_TRNS) && (layer >= 0));

    return code;
}

void
keymap_set_layer(int layer)
{
    keymap_state.active_layer = layer;
}

void
keymap_toggle_layer(int layer)
{
    if (keymap_state.active_layer != layer) {
        keymap_state.prev_layer = keymap_state.active_layer;
        keymap_state.active_layer = layer;
    } else {
        keymap_state.active_layer = keymap_state.prev_layer;
    }
}

void
keymap_print_state(void)
{
    printf("prev layer %d current layer %d\n", keymap_state.prev_layer, keymap_state.active_layer);
}

void
keymap_init(void)
{
    keymap_state.active_layer = 0;
}