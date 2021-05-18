/*
 * usb_desc_app.c
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

#include <no2usb/usb_proto.h>
#include <no2usb/usb_cdc_proto.h>
#include <no2usb/usb_dfu_proto.h>
#include <no2usb/usb_hid_proto.h>
#include <no2usb/usb.h>


usb_cdc_union_desc_def(1);

const uint8_t app_hid_report_desc[63] = {
	0x05, 0x01,	/* Usage Page (Generic Desktop) */
	0x09, 0x06,	/* Usage (Keyboard) */
	0xa1, 0x01,	/* Collection (Application) */
	0x05, 0x07,		/* Usage Page (KeyCodes) */
	0x19, 0xe0,		/* Usage Minimum (224) */
	0x29, 0xe7,		/* Usage Maximum (231) */
	0x15, 0x00,		/* Logical Minimum (0) */
	0x25, 0x01,		/* Logical Maximum (1) */
	0x75, 0x01,		/* Report Size (1) */
	0x95, 0x08,		/* Report Count (8) */
	0x81, 0x02,		/* Input (Data, Variable, Absolute)	Modifier byte */
	0x95, 0x01,		/* Report Count (1) */
	0x75, 0x08,		/* Report Size (8) */
	0x81, 0x01,		/* Input (Constant)			Reserved byte */
	0x95, 0x05,		/* Report Count (5) */
	0x75, 0x01,		/* Report Size (1) */
	0x05, 0x08,		/* Usage Page (Page# for LEDs) */
	0x19, 0x01,		/* Usage Minimum (1) */
	0x29, 0x05,		/* Usage Maximum (5) */
	0x91, 0x02,		/* Output (Data, Variable, Absolute)	LED report */
	0x95, 0x01,		/* Report Count (1) */
	0x75, 0x03,		/* Report Size (3) */
	0x91, 0x01,		/* Output (Constant)			LED report padding */
	0x95, 0x06,		/* Report Count (6) */
	0x75, 0x08,		/* Report Size (8) */
	0x15, 0x00,		/* Logical Minimum (0) */
	0x25, 0x65,		/* Logical Maximum(101) */
	0x05, 0x07,		/* Usage Page (Key Codes) */
	0x19, 0x00,		/* Usage Minimum (0) */
	0x29, 0x65,		/* Usage Maximum (101) */
	0x81, 0x00,		/* Input (Data, Array)			Key arrays (6 bytes) */
	0xc0,		/* End Collection */
};


static const struct {
	/* Configuration */
	struct usb_conf_desc conf;

	/* HID */
	struct {
		struct usb_intf_desc intf;
		struct usb_hid_hid_desc hid;
		struct usb_ep_desc ep_data_in;
	} __attribute__ ((packed)) hid;

	/* CDC */
	struct {
		struct usb_intf_desc intf_ctl;
		struct usb_cdc_hdr_desc cdc_hdr;
		struct usb_cdc_acm_desc cdc_acm;
		struct usb_cdc_union_desc__1 cdc_union;
		struct usb_ep_desc ep_ctl;
		struct usb_intf_desc intf_data;
		struct usb_ep_desc ep_data_out;
		struct usb_ep_desc ep_data_in;
	} __attribute__ ((packed)) cdc;

	/* DFU Runtime */
	struct {
		struct usb_intf_desc intf;
		struct usb_dfu_func_desc func;
	} __attribute__ ((packed)) dfu;
} __attribute__ ((packed)) _app_conf_desc = {
	.conf = {
		.bLength                = sizeof(struct usb_conf_desc),
		.bDescriptorType        = USB_DT_CONF,
		.wTotalLength           = sizeof(_app_conf_desc),
		.bNumInterfaces         = 4,
		.bConfigurationValue    = 1,
		.iConfiguration         = 4,
		.bmAttributes           = 0x80,
		.bMaxPower              = 0x32, /* 100 mA */
	},
	.hid = {
		.intf = {
			.bLength		= sizeof(struct usb_intf_desc),
			.bDescriptorType	= USB_DT_INTF,
			.bInterfaceNumber	= 0,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 1,
			.bInterfaceClass	= USB_CLS_HID,
			.bInterfaceSubClass	= USB_HID_SCLS_BOOT,
			.bInterfaceProtocol	= USB_HID_PROTO_KEYBOARD,
			.iInterface		= 6,
		},
		.hid = {
			.bLength		= sizeof(struct usb_hid_hid_desc),
			.bDescriptorType	= USB_HID_DT_HID,
			.bcdHID			= 0x0101,
			.bCountryCode		= 0x00,
			.bNumDescriptors	= 1,
			.desc[0]		= {
				.bDescriptorType	= USB_HID_DT_REPORT,
				.wDescriptorLength	= sizeof(app_hid_report_desc),
			},
		},
		.ep_data_in = {
			.bLength		= sizeof(struct usb_ep_desc),
			.bDescriptorType	= USB_DT_EP,
			.bEndpointAddress	= 0x81,
			.bmAttributes		= 0x03,
			.wMaxPacketSize		= 8,
			.bInterval		= 0x0a,
		},
	},
	.cdc = {
		.intf_ctl = {
			.bLength		= sizeof(struct usb_intf_desc),
			.bDescriptorType	= USB_DT_INTF,
			.bInterfaceNumber	= 1,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 1,
			.bInterfaceClass	= USB_CLS_CDC_CONTROL,
			.bInterfaceSubClass	= USB_CDC_SCLS_ACM,
			.bInterfaceProtocol	= 0x00,
			.iInterface		= 6,
		},
		.cdc_hdr = {
			.bLength		= sizeof(struct usb_cdc_hdr_desc),
			.bDescriptorType	= USB_CS_DT_INTF,
			.bDescriptorsubtype	= USB_CDC_DST_HEADER,
			.bcdCDC			= 0x0110,
		},
		.cdc_acm = {
			.bLength		= sizeof(struct usb_cdc_acm_desc),
			.bDescriptorType	= USB_CS_DT_INTF,
			.bDescriptorsubtype	= USB_CDC_DST_ACM,
			.bmCapabilities		= 0x02,
		},
		.cdc_union = {
			.bLength		= sizeof(struct usb_cdc_union_desc) + 1,
			.bDescriptorType	= USB_CS_DT_INTF,
			.bDescriptorsubtype	= USB_CDC_DST_UNION,
			.bMasterInterface	= 0,
			.bSlaveInterface	= { 2 },
		},
		.ep_ctl = {
			.bLength		= sizeof(struct usb_ep_desc),
			.bDescriptorType	= USB_DT_EP,
			.bEndpointAddress	= 0x84,
			.bmAttributes		= 0x03,
			.wMaxPacketSize		= 64,
			.bInterval		= 0x40,
		},
		.intf_data = {
			.bLength		= sizeof(struct usb_intf_desc),
			.bDescriptorType	= USB_DT_INTF,
			.bInterfaceNumber	= 2,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 2,
			.bInterfaceClass	= USB_CLS_CDC_DATA,
			.bInterfaceSubClass	= 0x00,
			.bInterfaceProtocol	= 0x00,
			.iInterface		= 7,
		},
		.ep_data_out = {
			.bLength		= sizeof(struct usb_ep_desc),
			.bDescriptorType	= USB_DT_EP,
			.bEndpointAddress	= 0x05,
			.bmAttributes		= 0x02,
			.wMaxPacketSize		= 64,
			.bInterval		= 0x00,
		},
		.ep_data_in = {
			.bLength		= sizeof(struct usb_ep_desc),
			.bDescriptorType	= USB_DT_EP,
			.bEndpointAddress	= 0x85,
			.bmAttributes		= 0x02,
			.wMaxPacketSize		= 64,
			.bInterval		= 0x00,
		},
	},
	.dfu = {
		.intf = {
			.bLength		= sizeof(struct usb_intf_desc),
			.bDescriptorType	= USB_DT_INTF,
			.bInterfaceNumber	= 3,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 0,
			.bInterfaceClass	= 0xfe,
			.bInterfaceSubClass	= 0x01,
			.bInterfaceProtocol	= 0x01,
			.iInterface		= 8,
		},
		.func = {
			.bLength		= sizeof(struct usb_dfu_func_desc),
			.bDescriptorType	= USB_DFU_DT_FUNC,
			.bmAttributes		= 0x0d,
			.wDetachTimeOut		= 1000,
			.wTransferSize		= 4096,
			.bcdDFUVersion		= 0x0101,
		},
	},
};

static const struct usb_conf_desc * const _conf_desc_array[] = {
	&_app_conf_desc.conf,
};

static const struct usb_dev_desc _dev_desc = {
	.bLength		= sizeof(struct usb_dev_desc),
	.bDescriptorType	= USB_DT_DEV,
	.bcdUSB			= 0x0200,
	.bDeviceClass		= 0,
	.bDeviceSubClass	= 0,
	.bDeviceProtocol	= 0,
	.bMaxPacketSize0	= 64,
	.idVendor		= 0x1d50,
	.idProduct		= 0x6147,
	.bcdDevice		= 0x0001,	/* v0.1 */
	.iManufacturer		= 2,
	.iProduct		= 3,
	.iSerialNumber		= 1,
	.bNumConfigurations	= num_elem(_conf_desc_array),
};

#include "usb_str_app.gen.h"

const struct usb_stack_descriptors app_stack_desc = {
	.dev = &_dev_desc,
	.conf = _conf_desc_array,
	.n_conf = num_elem(_conf_desc_array),
	.str = _str_desc_array,
	.n_str = num_elem(_str_desc_array),
};
