/*
 * fw_app.c
 *
 * Copyright (C) 2019 Sylvain Munaut
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
#include <string.h>

#include "config.h"
#include "console.h"
#include "led.h"
#include "mini-printf.h"
#include "spi.h"
#include "utils.h"

#include "usb_hid.h"
#include "keyboard.h"

#include <no2usb/usb.h>
#include <no2usb/usb_dfu_rt.h>

extern const struct usb_stack_descriptors app_stack_desc;

static void
serial_no_init()
{
	uint8_t buf[8];
	char *id, *desc;
	int i;

	flash_manuf_id(buf);
	printf("Flash Manufacturer : %s\n", hexstr(buf, 3, true));

	flash_unique_id(buf);
	printf("Flash Unique ID    : %s\n", hexstr(buf, 8, true));

	/* Overwrite descriptor string */
		/* In theory in rodata ... but nothing is ro here */
	id = hexstr(buf, 8, false);
	desc = (char*)app_stack_desc.str[1];
	for (i=0; i<16; i++)
		desc[2 + (i << 1)] = id[i];
}

static void
boot_dfu(void)
{
	/* Force re-enumeration */
	usb_disconnect();

	/* Boot firmware */
	volatile uint32_t *boot = (void*)0x80000000;
	*boot = (1 << 2) | (1 << 0);
}

void
usb_dfu_rt_cb_reboot(void)
{
        boot_dfu();
}

void
help(void)
{
	puts(
		"Available commands:\n"
		"  ?: This help\n"
		"  p: Print USB debug information.\n"
		"  b: Boot into DFU mode.\n"
		"  c: Connect USB\n"
		"  d: Disconnect USB\n"
		"  r: Read row values\n"
		"  h: Print hid internal state\n"
	);
}

void main()
{
	int cmd = 0;
	bool key_print = false;
	bool hid_print = false;

	/* Init console IO */
	console_init();
	puts("Booting App image..\n");

	/* LED */
	led_init();
	led_color(48, 96, 5);
	led_blink(true, 200, 1000);
	led_breathe(true, 100, 200);
	led_state(true);

	/* SPI */
	spi_init();

	/* Enable USB directly */
	serial_no_init();
	usb_init(&app_stack_desc);
	usb_dfu_rt_init();
	usb_hid_init();
	usb_connect();
	keyboard_init();

	/* Main loop */
	while (1)
	{
		/* Prompt ? */
		if (cmd >= 0)
			printf("Command> ");

		/* Poll for command */
		cmd = getchar_nowait();

		if (cmd >= 0) {
			if (cmd > 32 && cmd < 127) {
				putchar(cmd);
				putchar('\r');
				putchar('\n');
			}

			switch (cmd)
			{
			case '?':
				help();
				break;
			case 'p':
				usb_debug_print();
				break;
			case 'b':
				boot_dfu();
				break;
			case 'c':
				usb_connect();
				break;
			case 'd':
				usb_disconnect();
				break;
			case 'r':
				key_print = !key_print;
				break;
			case 'h':
				hid_print = !hid_print;
				break;
			default:
				printf("Unknown command '%c'\r\n", cmd);
				help();
				break;
			}
		}

		if (key_print && (usb_get_tick() % 100 == 0)) {
			keyboard_print_state();
		}

		/* Key poll */
		keyboard_poll();

		if (hid_print) {
			usb_hid_debug_print();
		}

		/* USB poll */
		usb_poll();
		usb_hid_poll();
	}
}
