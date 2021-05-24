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

struct keyscan {
	uint32_t csr;
	uint32_t _res[3];
	uint32_t rows[4];
} __attribute__((packed,aligned(4)));

static volatile struct keyscan * const keyscan_regs = (void*)(KEYSCAN_BASE);

struct keyboard_state {
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
keyboard_poll(void)
{
    static uint32_t old = 0;
	if(keyscan_regs->rows[1] != old) {
		old = keyscan_regs->rows[1];
		if((old & 0x0001) != 0) {
			usb_hid_press_key(0x04); // A
		} else {
			usb_hid_press_key(0x00); // No Key
		}
	}
}

void
keyboard_init(void)
{
    for (int i = 0; i < 4; i++) {
        keyboard_state.prev_rows[i] = 0x00000000;
    }
}