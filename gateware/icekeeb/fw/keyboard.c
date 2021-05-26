/*
 * keyboard.c
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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "keyboard.h"
#include "usb_hid.h"

#include "config.h"

#include "keycode.h"
#include "keymap.h"
#include "quantum_keycodes.h"

struct keyscan {
	uint32_t csr;
	uint32_t _res[3];
	uint32_t rows[4];
} __attribute__((packed,aligned(4)));

static volatile struct keyscan * const keyscan_regs = (void*)(KEYSCAN_BASE);

static struct {
    uint32_t prev_rows[4];
} keyboard_state;

static char *tobits(uint32_t v)
{
        static char buf[13];

        for (int i=0; i<12; i++)
                buf[i] = (v >> i) & 1 ? '#' : '.';
        buf[13] = 0;

        return buf;
}

void
keyboard_print_state(void)
{
    for (int i = 0; i<4; i++) {
		printf("r%d %s\n", i, tobits(keyscan_regs->rows[i]));
	}
	puts("\n");
}

void
keyboard_do_key(unsigned int col, unsigned int row, bool down)
{
    uint16_t keycode = keymap_get_code(col, row);
    //printf("do c%d r%d %c kc%02X\n", col, row, down?'v':'^', keycode);

    // Handle regular keycodes
    if (IS_KEY(keycode)) {
        if (down) {
            //printf("v %04X\n", keycode);
            usb_hid_press_key(col, row, keycode);
        } else {
            //printf("^ %04X\n", keycode);
            usb_hid_release_key(col, row);
        }
    }

    if (IS_MOD(keycode)) {
        if (down) {
            usb_hid_set_mod(MOD_BIT(keycode));
        } else {
            usb_hid_reset_mod(MOD_BIT(keycode));
        }
    }

    switch (keycode) {
        case QK_MODS...QK_MODS_MAX:
            if (down) {
                usb_hid_set_weak_mod(keycode >> 8);
                usb_hid_press_key(col, row, keycode & 0xFF);
            } else {
                usb_hid_reset_weak_mod(keycode >> 8);
                usb_hid_release_key(col, row);
            }
            break;
        case QK_TO...QK_TO_MAX:
            // The keycode contains a param at bit 4 to be active at press
            if (down && (keycode & 0x10)) {
                keymap_set_layer(keycode & 0x0F);
            }
            if (!down && (keycode & 0x20)) {
                keymap_set_layer(keycode & 0x0F);
            }
            break;

        case QK_MOMENTARY...QK_MOMENTARY_MAX:
            if (down) {
                keymap_set_layer(keycode & 0x0F);
            } else {
                keymap_set_layer(0);
            }
            break;

        case QK_TOGGLE_LAYER...QK_TOGGLE_LAYER_MAX:
            if (down) {
                keymap_toggle_layer(keycode & 0x0F);
            }
    }
}

void
keyboard_poll(void)
{
    // get keyboard state
    for (int i = 0; i < MATRIX_ROWS; i++) {
        uint32_t row = keyscan_regs->rows[i];
        uint32_t mask = keyboard_state.prev_rows[i] ^ row;
        if (mask) {
            uint32_t window = 1;
            for (int j = 0; j < MATRIX_COLS; j++, window <<= 1) {
                if (mask & window) {
                    keyboard_do_key(j, i, (row & window) != 0);
                }
            }
        }
        keyboard_state.prev_rows[i] = row;
    }
}

void
keyboard_init(void)
{
    keymap_init();

    for (int i = 0; i < 4; i++) {
        keyboard_state.prev_rows[i] = 0x00000000;
    }
}