/*
 * usb_hid.c
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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <no2usb/usb.h>
#include <no2usb/usb_hw.h>
#include <no2usb/usb_priv.h>
#include <no2usb/usb_hid_proto.h>


extern const uint8_t app_hid_report_desc[63];

static struct {
	/* Attached interface / ep */
	uint8_t intf;
	uint8_t ep;

	/* State */
	bool boot_proto;
} g_hid;

static struct {
	uint8_t modifier;
	uint8_t _res;
	uint8_t keycodes[6];
} __attribute__ ((packed)) app_hid_report;

void
usb_hid_press_key(uint8_t keycode)
{
	app_hid_report.keycodes[0] = keycode;
}

static bool
_hid_get_report(struct usb_ctrl_req *req, struct usb_xfer *xfer)
{

	xfer->cb_data = (void *)&app_hid_report;
	xfer->len = sizeof(app_hid_report);
	return true;
}

static bool
_hid_get_descriptor(struct usb_ctrl_req *req, struct usb_xfer *xfer)
{
	int idx = req->wValue & 0xff;

	xfer->data = NULL;

	switch (req->wValue & 0xff00)
	{
	case (USB_HID_DT_REPORT << 8):
		if (idx == 0) {
			xfer->data = (void*)app_hid_report_desc;
			xfer->len  = sizeof(app_hid_report_desc);
		}
		break;
	}

	return xfer->data != NULL;
}

static enum usb_fnd_resp
_hid_ctrl_req(struct usb_ctrl_req *req, struct usb_xfer *xfer)
{
	bool rv = false;

	/* Handle all request for HID interface */
	if (USB_REQ_RCPT(req) != USB_REQ_RCPT_INTF)
		return USB_FND_CONTINUE;

	if (req->wIndex != g_hid.intf)
		return USB_FND_CONTINUE;

	/* Handle request */
	switch (req->wRequestAndType)
	{
	case USB_RT_HID_GET_REPORT:
		rv = _hid_get_report(req, xfer);
		break;

	case USB_RT_HID_SET_REPORT:
		/* Optional, Not handled */
		return USB_FND_ERROR;

	case USB_RT_HID_GET_IDLE:
		/* Optional, Not handled */
		return USB_FND_ERROR;

	case USB_RT_HID_SET_IDLE:
		/* Optional, Not handled */
		return USB_FND_ERROR;

	case USB_RT_HID_GET_PROTOCOL:
		/* Optional, Handled */
		return USB_FND_ERROR;

	case USB_RT_HID_SET_PROTOCOL:
		/* Optional, Handled */
		return USB_FND_ERROR;

	case USB_RT_HID_GET_DESCRIPTOR:
		rv = _hid_get_descriptor(req, xfer);
		break;

	case USB_RT_HID_SET_DESCRIPTOR:
		/* Not handled */
		return USB_FND_ERROR;

	default:
		return USB_FND_ERROR;
	}

	return rv ? USB_FND_SUCCESS : USB_FND_ERROR;
}

static enum usb_fnd_resp
_hid_set_conf(const struct usb_conf_desc *conf)
{
	const struct usb_intf_desc *intf;
	const struct usb_ep_desc *ep;
	const void *sod, *eod;

	/* Deconfig case */
	if (conf == NULL) {
		g_hid.intf = 0xff;
		g_hid.ep   = 0xff;
		return USB_FND_SUCCESS;
	}

	/* Find first HID interface */
        sod = conf;
        eod = sod + conf->wTotalLength;

	while (1) {
		sod = usb_desc_find(usb_desc_next(sod), eod, USB_DT_INTF);
		if (!sod)
			break;

		intf = (void*)sod;
		if ((intf->bInterfaceClass != USB_CLS_HID) ||
		    (intf->bAlternateSetting != 0))
			continue;

		/* Find EP */
		ep = (void*)usb_desc_find(sod, eod, USB_DT_EP);
		if (!ep || (ep->bEndpointAddress < 0x80) || (ep->bmAttributes != 0x03))
			continue;

		/* Save interface/ep number */
		g_hid.intf = intf->bInterfaceNumber;
		g_hid.ep   = ep->bEndpointAddress;

		/* Boot the endpoint */
		usb_ep_boot(intf, g_hid.ep, false);

		/* Done */
		return USB_FND_SUCCESS;
	}

	return USB_FND_ERROR;
}

static struct usb_fn_drv _hid_drv = {
	.ctrl_req	= _hid_ctrl_req,
	.set_conf	= _hid_set_conf,
};


void
usb_hid_poll(void)
{
	uint8_t buf[8] = { 0x00 };
	volatile struct usb_ep *ep = &usb_ep_regs[g_hid.ep & 0x1f].in;

	if (g_hid.ep == 0xff)
		return;

//	if ((ep->bd[0].csr & USB_BD_STATE_MSK) != USB_BD_STATE_RDY_DATA) {
//		usb_data_write(ep->bd[0].ptr, buf, 8);
//		ep->bd[0].csr = USB_BD_STATE_RDY_DATA | USB_BD_LEN(1);
//	}
}

void
usb_hid_init(void)
{
	app_hid_report.modifier = 0x00;
	app_hid_report._res = 0x00;
	app_hid_report.keycodes[0] = 0x00;
	app_hid_report.keycodes[1] = 0x00;
	app_hid_report.keycodes[2] = 0x00;
	app_hid_report.keycodes[3] = 0x00;
	app_hid_report.keycodes[4] = 0x00;
	app_hid_report.keycodes[5] = 0x00;
	usb_register_function_driver(&_hid_drv);
	g_hid.intf = 0xff;
	g_hid.ep   = 0xff;
}
