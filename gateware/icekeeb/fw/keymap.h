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

#pragma once

#include <stdint.h>

/* key matrix size */
#define MATRIX_ROWS 4
#define MATRIX_COLS 12

uint16_t keymap_get_layer_code(unsigned int layer, unsigned int col, unsigned int row);
uint16_t keymap_get_code(unsigned int col, unsigned int row);
void keymap_init(void);