/*
 * usb_hid.h
 *
 * Copyright (C) 2021 Sylvain Munaut
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

#pragma once

void usb_hid_poll(void);
void usb_hid_init(void);
void usb_hid_press_key(int col, int row, uint8_t keycode);
void usb_hid_release_key(int col, int row);
void usb_hid_set_mod(uint8_t keycode);
void usb_hid_reset_mod(uint8_t keycode);
void usb_hid_set_weak_mod(uint8_t keycode);
void usb_hid_reset_weak_mod(uint8_t keycode);
void usb_hid_clear_weak_mod(void);
void usb_hid_debug_print(void);