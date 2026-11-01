#include <hci.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/hcis/hcisu_h2.c
 */

/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/*******************************************************************************
 * headers
 */

#include "revolution/types.h"
#include <cstring>

#include <bt_trace.h>
#include <btu.h>
#include <gki.h>
#include <gki_target.h>
#include <l2c_int.h>

// BYTES: none of the little static helpers in here ended up inlined in the original
#pragma auto_inline off

/*******************************************************************************
 * macros
 */

// HYPOTHESIS: names for the little numbers the transport juggles
#define H2_PORT_EVT				1
#define H2_PORT_ACL				2
#define H2_PORT_SCO				3
#define H2_NUM_PORTS			3

#define H2_STATE_ACTIVE			2	// hcisu_h2_cb.state once open() ran

// rcv_state values
#define H2_RX_MSGTYPE_ST		0
#define H2_RX_HDR_ST			1
#define H2_RX_HDR_IGNORE_ST		2
#define H2_RX_DATA_ST			3
#define H2_RX_DATA_IGNORE_ST	4

// message type codes (indexes into the sdata2 tables)
#define H2_TYPE_CMD				1
#define H2_TYPE_ACL				2
#define H2_TYPE_SCO				3
#define H2_TYPE_EVT				4

// usb side transport state
#define USB_STATE_CLOSED		0
#define USB_STATE_OPEN			2
#define USB_STATE_READY			4
#define USB_STATE_IDLE			5

#define USB_MAX_PENDING_WRITES	5

#define USB_ENDPOINT_BULK_OUT	0x02
#define USB_ENDPOINT_BULK_IN	0x82
#define USB_ENDPOINT_INTR_IN	0x81

#define HCISU_TRACE_MASK		0x00070000

/*******************************************************************************
 * types
 */

typedef void tUSB_CBACK(int result, void *ctx);
typedef void tHCISU_USB_STATE_CBACK(int type, INT8 arg);

typedef struct
{
	HC_BT_HDR	*p_rcv_msg[H2_NUM_PORTS];		// size 0x0c, offset 0x00
	UINT16		rcv_len[H2_NUM_PORTS];			// size 0x06, offset 0x0c
	UINT16		rx_event_base;					// size 0x02, offset 0x12
	UINT8		rcv_msg_type[H2_NUM_PORTS];		// size 0x03, offset 0x14
	UINT8		rcv_last_byte[H2_NUM_PORTS];	// size 0x03, offset 0x17
	UINT8		rcv_state[H2_NUM_PORTS];		// size 0x03, offset 0x1a
	UINT8		unk_1d;							// size 0x01, offset 0x1d
	UINT8		state;							// size 0x01, offset 0x1e
	UINT8		task_id;						// size 0x01, offset 0x1f
	UINT8		mbox;							// size 0x01, offset 0x20
	UINT8		reserved[0x17];					// size 0x17, offset 0x21
} tHCISU_H2_CB; // size 0x38

typedef struct
{
	int							fd;				// size 0x04, offset 0x00
	UINT8						unk_04[0x0c];	// size 0x0c, offset 0x04
	UINT8						ep_bulk_out;	// size 0x01, offset 0x10
	UINT8						ep_bulk_in;		// size 0x01, offset 0x11
	UINT8						ep_intr_in;		// size 0x01, offset 0x12
	UINT8						ep_unk;			// size 0x01, offset 0x13
	UINT32						vendor_id;		// size 0x04, offset 0x14
	UINT32						product_id;		// size 0x04, offset 0x18
	UINT8						pool_evt;		// size 0x01, offset 0x1c
	UINT8						pool_acl;		// size 0x01, offset 0x1d
	/* 2 bytes padding */
	tHCISU_USB_STATE_CBACK		*p_state_cback;	// size 0x04, offset 0x20
	void						*p_open_args;	// size 0x04, offset 0x24
	UINT8						init_done;		// size 0x01, offset 0x28
	UINT8						state;			// size 0x01, offset 0x29
	UINT8						unk_2a;			// size 0x01, offset 0x2a
	UINT8						reading_evt;	// size 0x01, offset 0x2b
	BUFFER_Q					acl_write_q;	// size 0x0c, offset 0x2c
	UINT8						acl_writing;	// size 0x01, offset 0x38
	/* 3 bytes padding */
	BUFFER_Q					cmd_write_q;	// size 0x0c, offset 0x3c
	UINT8						cmd_writing;	// size 0x01, offset 0x48
	/* 3 bytes padding */
	UINT8						unk_4c[0x14];	// size 0x14, offset 0x4c
} tHCISU_USB_CB; // size 0x60

typedef struct
{
	UINT32		unk_00;			// size 0x04, offset 0x00
	UINT16		vid;			// size 0x02, offset 0x04
	UINT16		pid;			// size 0x02, offset 0x06
	UINT16		task_id;		// size 0x02, offset 0x08
	UINT8		unk_0a;			// size 0x01, offset 0x0a
	UINT8		unk_0b;			// size 0x01, offset 0x0b
} tHCISU_USB_ARGS;

/*******************************************************************************
 * external functions
 */

// bounces onto a stack of its own, see os
extern void fn_801D81C4(void *a, UINT32 b, UINT32 c, UINT32 d, void *func, void *stack_top);

extern int IPCCltInit(void);

// usb.c, no names for any of these yet
extern int IUSB_OpenLib(void);
extern void IUSB_CloseLib(void);
extern int IUSB_OpenDeviceIds(char *name, UINT16 vid, UINT16 pid);
extern int IUSB_CloseDeviceAsync(int fd, void (*p_cback)(int), void *ctx);
extern int fn_802291D4(int fd, int ep, int len, void *p_data, tUSB_CBACK *p_cback, void *ctx);
extern int fn_80229248(int fd, int ep, int len, void *p_data, tUSB_CBACK *p_cback, void *ctx);
extern int fn_802292BC(int fd, int ep, int len, void *p_data, tUSB_CBACK *p_cback, void *ctx);
extern int fn_802298D4(int fd, int req_type, int req, int value, int index, int len, void *p_data, tUSB_CBACK *p_cback, void *ctx);

extern void bta_ci_hci_msg_handler(void *p_data);

// gki_glue_unk.c
extern void bte_usb_state_cback(int type, INT8 arg);

// owned by the wud code, hcisu flips it to say whether the transport is still busy
extern int bte_hcisu_wait;

/*******************************************************************************
 * local function declarations
 */

static UINT16 hcisu_h2_receive_msg(UINT16 idx, tHCISU_H2_CB *p_cb);
static void hcisu_h2_send_msg(tHCISU_H2_CB *p_cb, HC_BT_HDR *p_msg);

static void hcisu_usb_closed_cback(int result);
static void hcisu_usb_evt_read_cback(int result, void *ctx);
static void hcisu_usb_acl_read_cback(int result, void *ctx);
static void hcisu_usb_post_evt_read(void);
static void hcisu_usb_cmd_write_cback(int result, void *ctx);
static void hcisu_usb_acl_write_cback(int result, void *ctx);
static void hcisu_usb_init(tHCISU_USB_ARGS *p_args);
static void hcisu_usb_open(tHCISU_USB_ARGS *p_args, tHCISU_USB_STATE_CBACK *p_cback);
static UINT16 hcisu_usb_read(UINT8 port, UINT8 *p_data, UINT16 len);
static UINT16 hcisu_usb_write(int type, UINT8 *p_data, UINT16 len, HC_BT_HDR *p_msg);
static void hcisu_usb_close(void);
static void hcisu_usb_reset(void);

/*******************************************************************************
 * variables
 */

// the sdata2 lookup tables live in this unit, indexed by message type - 1
static UINT8 const hcisu_h2_hdr_len[4] = {3, 4, 3, 2};
static UINT16 const hcisu_h2_rcv_event[4] = {
	MSG_HC_TO_STACK_HCI_ERR,
	MSG_HC_TO_STACK_HCI_ACL,
	MSG_HC_TO_STACK_HCI_SCO,
	MSG_HC_TO_STACK_HCI_EVT,
};

static tHCISU_H2_CB hcisu_h2_cb;
static tHCISU_USB_CB hcisu_usb_cb;
static UINT8 hcisu_evt_stack[0x1000];
static UINT8 hcisu_acl_stack[0x1000];

static int hcisu_usb_dev = -1;

// none of these are ever written in this build, they look like leftover configuration knobs
static UINT8 hcisu_usb_opened;
static UINT8 hcisu_usb_init_done;
static UINT32 hcisu_usb_which;
static UINT32 hcisu_usb_use_which;
static UINT8 hcisu_usb_ids_overridden;
static UINT32 hcisu_usb_override_vid;
static UINT32 hcisu_usb_override_pid;

/*******************************************************************************
 * functions
 */

static UINT16 hcisu_h2_receive_msg(UINT16 idx, tHCISU_H2_CB *p_cb)
{
	UINT16 count = 0;
	UINT8 byte;
	UINT32 len;
	BOOLEAN msg_received;
	UINT16 port = idx + 1;

	if (p_cb->state != H2_STATE_ACTIVE)
		return 0;

	for (;;)
	{
		if (hcisu_usb_read(port, &byte, 1) == 0)
			break;

		msg_received = FALSE;
		count++;

		switch (p_cb->rcv_state[idx])
		{
		case H2_RX_MSGTYPE_ST:
			if (port == H2_PORT_EVT)
			{
				p_cb->p_rcv_msg[idx] = GKI_getpoolbuf(GKI_POOL_ID_2);
				p_cb->rcv_msg_type[idx] = H2_TYPE_EVT;
			}
			else if (port == H2_PORT_ACL)
			{
				p_cb->p_rcv_msg[idx] = GKI_getpoolbuf(GKI_POOL_ID_3);
				p_cb->rcv_msg_type[idx] = H2_TYPE_ACL;
			}
			else if (port == H2_PORT_SCO)
			{
				p_cb->p_rcv_msg[idx] = GKI_getpoolbuf(GKI_POOL_ID_1);
				p_cb->rcv_msg_type[idx] = H2_TYPE_SCO;
			}
			else
			{
				break;
			}

			if (p_cb->p_rcv_msg[idx])
			{
				p_cb->p_rcv_msg[idx]->len = 0;
				p_cb->p_rcv_msg[idx]->event = hcisu_h2_rcv_event[p_cb->rcv_msg_type[idx] - 1];
				p_cb->p_rcv_msg[idx]->offset = 0;
				p_cb->rcv_state[idx] = H2_RX_HDR_ST;
			}
			else
			{
				LogMsg_0(HCISU_TRACE_MASK, "HCIS: Unable to allocate buffer for incoming HCI message.");
				p_cb->rcv_state[idx] = H2_RX_HDR_IGNORE_ST;
			}

			p_cb->rcv_len[idx] = hcisu_h2_hdr_len[p_cb->rcv_msg_type[idx] - 1];
			// fall through, this byte is the first header byte

		case H2_RX_HDR_ST:
			*((UINT8 *)(p_cb->p_rcv_msg[idx] + 1) + p_cb->p_rcv_msg[idx]->len++) = byte;

			if (--p_cb->rcv_len[idx] == 0)
			{
				len = byte;

				if (p_cb->rcv_msg_type[idx] == H2_TYPE_ACL)
				{
					len = (UINT16)((byte << 8) + p_cb->rcv_last_byte[idx]);

					p_cb->p_rcv_msg[idx] = (HC_BT_HDR *)l2cap_link_chk_pkt_start((BT_HDR *)p_cb->p_rcv_msg[idx]);

					if (p_cb->p_rcv_msg[idx] == NULL)
					{
						p_cb->rcv_len[idx] = len;

						if (len == 0)
							p_cb->rcv_state[idx] = H2_RX_MSGTYPE_ST;
						else
							p_cb->rcv_state[idx] = H2_RX_DATA_IGNORE_ST;

						break;
					}
				}

				p_cb->rcv_len[idx] = len;

				if (len + hcisu_h2_hdr_len[p_cb->rcv_msg_type[idx] - 1] + sizeof(HC_BT_HDR)
				    > GKI_get_buf_size(p_cb->p_rcv_msg[idx]))
				{
					GKI_freebuf(p_cb->p_rcv_msg[idx]);
					p_cb->p_rcv_msg[idx] = NULL;

					LogMsg_0(HCISU_TRACE_MASK, "HCIS: Invalid length for incoming HCI message.");

					p_cb->rcv_state[idx] = H2_RX_DATA_IGNORE_ST;
				}
				else if (len != 0)
				{
					p_cb->rcv_state[idx] = H2_RX_DATA_ST;
				}
				else
				{
					p_cb->rcv_state[idx] = H2_RX_MSGTYPE_ST;
					msg_received = TRUE;
				}
			}
			else
			{
				p_cb->rcv_last_byte[idx] = byte;
			}

			break;

		case H2_RX_HDR_IGNORE_ST:
			if (--p_cb->rcv_len[idx] == 0)
			{
				len = byte;

				if (p_cb->rcv_msg_type[idx] == H2_TYPE_ACL)
					len = (UINT16)((byte << 8) + p_cb->rcv_last_byte[idx]);

				p_cb->rcv_len[idx] = len;
				p_cb->rcv_state[idx] = H2_RX_DATA_IGNORE_ST;
			}
			else
			{
				p_cb->rcv_last_byte[idx] = byte;
			}

			break;

		case H2_RX_DATA_ST:
		{
			UINT16 n;

			*((UINT8 *)(p_cb->p_rcv_msg[idx] + 1) + p_cb->p_rcv_msg[idx]->len++) = byte;
			p_cb->rcv_len[idx]--;

			n = hcisu_usb_read(idx + 1, (UINT8 *)(p_cb->p_rcv_msg[idx] + 1) + p_cb->p_rcv_msg[idx]->len, p_cb->rcv_len[idx]);

			count += n;
			p_cb->p_rcv_msg[idx]->len += n;
			p_cb->rcv_len[idx] -= n;

			if (p_cb->rcv_len[idx] == 0)
			{
				if (p_cb->rcv_msg_type[idx] == H2_TYPE_ACL && !l2cap_link_chk_pkt_end())
				{
					p_cb->rcv_state[idx] = H2_RX_MSGTYPE_ST;
				}
				else
				{
					p_cb->rcv_state[idx] = H2_RX_MSGTYPE_ST;
					msg_received = TRUE;
				}
			}
		}
			break;

		case H2_RX_DATA_IGNORE_ST:
			if (--p_cb->rcv_len[idx] == 0)
				p_cb->rcv_state[idx] = H2_RX_MSGTYPE_ST;

			break;
		}

		if (msg_received)
		{
			GKI_send_msg(hcisu_h2_cb.task_id, BTU_HCI_RCV_MBOX, p_cb->p_rcv_msg[idx]);
			p_cb->p_rcv_msg[idx] = NULL;
		}
	}

	return count;
}

static void hcisu_h2_send_msg(tHCISU_H2_CB *p_cb, HC_BT_HDR *p_msg)
{
	UINT16 event = p_msg->event;
	UINT8 *p = (UINT8 *)(p_msg + 1) + p_msg->offset;
	int type;
	UINT16 handle;

	if (event == MSG_STACK_TO_HC_HCI_ACL)
		type = 2;
	else if (event == MSG_STACK_TO_HC_HCI_SCO)
		type = 3;
	else if (event == MSG_STACK_TO_HC_HCI_CMD)
		type = 0;

	if (event == MSG_STACK_TO_HC_HCI_ACL && p_msg->len > btu_cb.hcit_acl_pkt_size)
	{
		// too big for one controller buffer, cut it into continuation fragments
		handle = (UINT16)(p[0] + (UINT16)((p[1] << 8) & 0xff00));
		p += 2;

		handle = (handle & 0xcfff) | 0x1000;

		while (p_msg->len > btu_cb.hcit_acl_pkt_size)
		{
			hcisu_usb_write(type, p, btu_cb.hcit_acl_pkt_size, p_msg);

			p_msg->offset += btu_cb.hcit_acl_data_size;
			p = (UINT8 *)(p_msg + 1) + p_msg->offset;
			p_msg->len -= btu_cb.hcit_acl_data_size;

			p[0] = (UINT8)handle;
			p[1] = (UINT8)(handle >> 8);
			p += 2;

			if (p_msg->len > btu_cb.hcit_acl_pkt_size)
			{
				*p++ = (UINT8)btu_cb.hcit_acl_data_size;
				*p++ = (UINT8)(btu_cb.hcit_acl_data_size >> 8);
			}
			else
			{
				*p++ = (UINT8)(p_msg->len - 4);
				*p++ = (UINT8)((p_msg->len - 4) >> 8);
			}

			if (p_msg->layer_specific != 0)
			{
				p_msg->layer_specific--;

				if (p_msg->layer_specific == 0)
				{
					p_msg->event = MSG_HC_TO_STACK_L2C_SEG_XMIT;
					GKI_send_msg(hcisu_h2_cb.task_id, BTU_HCI_RCV_MBOX, p_msg);
					return;
				}
			}
		}
	}

	hcisu_usb_write(type, p, p_msg->len, p_msg);
	GKI_freebuf(p_msg);
}

void hcisu_h2_init(UINT8 task_id, UINT8 mbox, UINT16 rx_event_base)
{
	hcisu_h2_cb.state = 0;
	hcisu_h2_cb.rcv_state[0] = 0;
	hcisu_h2_cb.rcv_state[1] = 0;
	hcisu_h2_cb.rcv_state[2] = 0;

	hcisu_h2_cb.task_id = task_id;
	hcisu_h2_cb.mbox = mbox;
	hcisu_h2_cb.rx_event_base = rx_event_base;
}

BOOLEAN hcisu_h2_open(tHCI_CFG *p_cfg)
{
	tHCISU_USB_ARGS args;

	args.unk_0b = 0;
	args.vid = p_cfg->at_0x00;
	args.pid = p_cfg->at_0x02;
	args.task_id = hcisu_h2_cb.task_id;

	hcisu_h2_cb.state = H2_STATE_ACTIVE;

	hcisu_usb_init(&args);
	hcisu_usb_open(&args, (tHCISU_USB_STATE_CBACK *)bte_usb_state_cback);

	return TRUE;
}

void hcisu_h2_close(void)
{
	hcisu_h2_cb.state = 0;

	hcisu_usb_close();
	hcisu_usb_reset();
}

BOOLEAN hcisu_h2_send(HC_BT_HDR *p_msg)
{
	hcisu_h2_send_msg(&hcisu_h2_cb, p_msg);

	return TRUE;
}

unk_t hcisu_h2_handle_event(UINT16 event)
{
	hcisu_h2_receive_msg(event - 8 - hcisu_h2_cb.rx_event_base, &hcisu_h2_cb);

	return 0;
}

// HYPOTHESIS: this is what WUDGetAllocatedMemSize hands back, so it should be the bte heap size; 0x14a78 is all I know
int BTE_GetMemSize(void)
{
	return 0x14a78;
}

static void hcisu_usb_closed_cback(int result)
{
	hcisu_usb_cb.fd = 0;
	hcisu_usb_cb.ep_bulk_out = 0;
	hcisu_usb_cb.ep_bulk_in = 0;
	hcisu_usb_cb.ep_intr_in = 0;
	hcisu_usb_cb.ep_unk = 0;
	hcisu_usb_opened = 0;

	IUSB_CloseLib();

	GKI_disable();
	hcisu_usb_cb.init_done = 0;
	hcisu_usb_cb.state = USB_STATE_IDLE;
	hcisu_usb_init_done = 0;
	GKI_enable();

	if (hcisu_usb_cb.p_state_cback)
		(*hcisu_usb_cb.p_state_cback)(4, (INT8)result);
}

static void hcisu_usb_evt_read_cback(int result, void *ctx)
{
	HC_BT_HDR *p_buf = ctx;
	HC_BT_HDR *p_msg;

	hcisu_usb_cb.reading_evt = 0;

	if (hcisu_usb_cb.state != USB_STATE_OPEN)
	{
		GKI_freebuf(p_buf);
		GKI_delete_pool(hcisu_usb_cb.pool_evt);
		hcisu_usb_cb.pool_evt = GKI_INVALID_POOL;
		return;
	}

	if ((UINT32)p_buf != 0)
	{
		if (result <= 0)
		{
			GKI_freebuf(p_buf);
		}
		else
		{
			p_msg = GKI_getpoolbuf(GKI_POOL_ID_2);

			if (p_msg == NULL)
			{
				GKI_freebuf(p_buf);
			}
			else
			{
				p_buf->len = result;
				p_buf->event = MSG_HC_TO_STACK_HCI_EVT;

				memcpy(p_msg, p_buf, ((p_buf->len + p_buf->offset + 8) & ~3) + 4);

				fn_801D81C4(p_msg, 0, 0, 0, bta_ci_hci_msg_handler,
				            hcisu_evt_stack + sizeof hcisu_evt_stack);

				GKI_freebuf(p_buf);
			}
		}
	}

	hcisu_usb_post_evt_read();
}

static void hcisu_usb_acl_read_cback(int result, void *ctx)
{
	HC_BT_HDR *p_buf = ctx;
	UINT8 *p_data;
	HC_BT_HDR *p_msg;
	HC_BT_HDR *p_new;

	if (hcisu_usb_cb.state != USB_STATE_OPEN)
	{
		GKI_freebuf(p_buf);
		GKI_delete_pool(hcisu_usb_cb.pool_acl);
		hcisu_usb_cb.pool_acl = GKI_INVALID_POOL;
		return;
	}

	if (result <= 0)
	{
		GKI_freebuf(p_buf);
	}
	else
	{
		p_buf->len = result;

		p_msg = GKI_getpoolbuf(GKI_POOL_ID_3);

		if (p_msg == NULL)
		{
			GKI_freebuf(p_buf);
		}
		else
		{
			memcpy(p_msg, p_buf, ((p_buf->len + p_buf->offset + 8) & ~3) + 4);

			p_new = (HC_BT_HDR *)l2cap_link_chk_pkt_start((BT_HDR *)p_msg);

			if (p_new != NULL && l2cap_link_chk_pkt_end())
			{
				fn_801D81C4(p_new, 0, 0, 0, bta_ci_hci_msg_handler,
				            hcisu_acl_stack + sizeof hcisu_acl_stack);
			}

			GKI_freebuf(p_buf);
		}
	}

	// keep a read posted for the next acl packet
	do
	{
		p_buf = GKI_getpoolbuf(hcisu_usb_cb.pool_acl);
	} while (p_buf == NULL);

	p_buf->event = MSG_HC_TO_STACK_HCI_ACL;
	p_buf->len = 0;

	p_data = (UINT8 *)(((UINT32)p_buf + 0x27) & ~0x1f);
	p_buf->offset = p_data - (UINT8 *)(p_buf + 1);

	if (fn_80229248(hcisu_usb_cb.fd, hcisu_usb_cb.ep_bulk_in,
	                GKI_get_buf_size(p_buf) - 0x28 - p_buf->offset, p_data,
	                hcisu_usb_acl_read_cback, p_buf) != 0)
	{
		GKI_freebuf(p_buf);
	}
}

static void hcisu_usb_post_evt_read(void)
{
	HC_BT_HDR *p_buf;
	UINT8 *p_data;

	do
	{
		p_buf = GKI_getpoolbuf(hcisu_usb_cb.pool_evt);
	} while (p_buf == NULL);

	p_buf->event = MSG_HC_TO_STACK_HCI_EVT;
	p_buf->len = 0;

	p_data = (UINT8 *)(((UINT32)p_buf + 0x27) & ~0x1f);
	p_buf->offset = p_data - (UINT8 *)(p_buf + 1);

	if (fn_802291D4(hcisu_usb_cb.fd, hcisu_usb_cb.ep_intr_in,
	                GKI_get_buf_size(p_buf) - 0x28 - p_buf->offset, p_data,
	                hcisu_usb_evt_read_cback, p_buf) != 0)
	{
		GKI_freebuf(p_buf);
	}

	hcisu_usb_cb.reading_evt = TRUE;
}

static void hcisu_usb_cmd_write_cback(int result, void *ctx)
{
	HC_BT_HDR *p_buf = ctx;
	HC_BT_HDR *p_next;

	if (p_buf != NULL)
	{
		GKI_freebuf(p_buf);

		GKI_disable();
		hcisu_usb_cb.cmd_writing--;
		GKI_enable();
	}

	if (hcisu_usb_cb.cmd_writing < USB_MAX_PENDING_WRITES && hcisu_usb_cb.cmd_write_q.count != 0)
	{
		p_next = GKI_dequeue(&hcisu_usb_cb.cmd_write_q);

		if (fn_802298D4(hcisu_usb_cb.fd, 0x20, 0, 0, 0, p_next->len,
		                (UINT8 *)(p_next + 1) + p_next->offset,
		                hcisu_usb_cmd_write_cback, p_next) < 0)
		{
			GKI_enqueue_head(&hcisu_usb_cb.cmd_write_q, p_next);
		}
		else
		{
			GKI_disable();
			hcisu_usb_cb.cmd_writing++;
			GKI_enable();
		}
	}
}

static void hcisu_usb_acl_write_cback(int result, void *ctx)
{
	HC_BT_HDR *p_buf = ctx;
	HC_BT_HDR *p_next;

	if (p_buf != NULL)
	{
		GKI_freebuf(p_buf);

		GKI_disable();
		hcisu_usb_cb.acl_writing--;
		GKI_enable();
	}

	if (hcisu_usb_cb.acl_writing < USB_MAX_PENDING_WRITES && hcisu_usb_cb.acl_write_q.count != 0)
	{
		p_next = GKI_dequeue(&hcisu_usb_cb.acl_write_q);

		if (fn_802292BC(hcisu_usb_cb.fd, hcisu_usb_cb.ep_bulk_out, p_next->len,
		                (UINT8 *)(p_next + 1) + p_next->offset,
		                hcisu_usb_acl_write_cback, p_next) < 0)
		{
			GKI_enqueue_head(&hcisu_usb_cb.acl_write_q, p_next);
		}
		else
		{
			GKI_disable();
			hcisu_usb_cb.acl_writing++;
			GKI_enable();
		}
	}
}

static void hcisu_usb_init(tHCISU_USB_ARGS *p_args)
{
	int fd;
	UINT32 vid, pid;

	memset(&hcisu_usb_cb, 0, 0x4c);

	GKI_disable();
	hcisu_usb_cb.state = USB_STATE_IDLE;
	GKI_enable();

	if (hcisu_usb_init_done == 0)
	{
		hcisu_usb_cb.init_done = 0;
		hcisu_usb_init_done = 1;
	}

	if (IPCCltInit() != 0)
		return;

	if (IUSB_OpenLib() != 0)
		return;

	hcisu_usb_cb.p_open_args = p_args;

	if (hcisu_usb_ids_overridden == 1)
	{
		hcisu_usb_cb.vendor_id = hcisu_usb_override_vid;
		hcisu_usb_cb.product_id = hcisu_usb_override_pid;
	}
	else
	{
		// nintendo's own bluetooth dongle ids
		hcisu_usb_cb.vendor_id = 0x57e;
		hcisu_usb_cb.product_id = 0x305;
	}

	pid = hcisu_usb_cb.product_id;
	vid = hcisu_usb_cb.vendor_id;

	hcisu_usb_cb.ep_bulk_out = 0;
	hcisu_usb_cb.ep_bulk_in = 0;
	hcisu_usb_cb.ep_intr_in = 0;
	hcisu_usb_cb.ep_unk = 0;

	if (hcisu_usb_use_which == 1)
	{
		if (hcisu_usb_which == 0)
		{
			fd = IUSB_OpenDeviceIds("oh0", (UINT16)vid, (UINT16)pid);
		}
		else if (hcisu_usb_which == 1)
		{
			fd = IUSB_OpenDeviceIds("oh1", (UINT16)vid, (UINT16)pid);
		}
	}
	else
	{
		fd = IUSB_OpenDeviceIds("oh1", (UINT16)vid, (UINT16)pid);
	}

	if (fd >= 0)
		hcisu_usb_dev = fd;

	if (fd < 0)
		return;

	hcisu_usb_cb.ep_bulk_out = USB_ENDPOINT_BULK_OUT;
	hcisu_usb_cb.ep_bulk_in = USB_ENDPOINT_BULK_IN;
	hcisu_usb_cb.ep_intr_in = USB_ENDPOINT_INTR_IN;
	hcisu_usb_cb.ep_unk = 0;

	GKI_init_q(&hcisu_usb_cb.acl_write_q);
	hcisu_usb_cb.acl_writing = 0;

	GKI_init_q(&hcisu_usb_cb.cmd_write_q);
	hcisu_usb_cb.cmd_writing = 0;

	hcisu_usb_cb.pool_evt = GKI_create_pool(GKI_BUF2_SIZE, GKI_BUF2_MAX, GKI_RESTRICTED_POOL, NULL);
	hcisu_usb_cb.pool_acl = GKI_create_pool(GKI_BUF3_SIZE, GKI_BUF3_MAX, GKI_RESTRICTED_POOL, NULL);

	if (hcisu_usb_cb.pool_evt == GKI_INVALID_POOL || hcisu_usb_cb.pool_acl == GKI_INVALID_POOL)
		return;

	GKI_disable();
	hcisu_usb_cb.state = USB_STATE_READY;
	GKI_enable();

	bte_hcisu_wait = 1;
}

static void hcisu_usb_open(tHCISU_USB_ARGS *p_args,
                           tHCISU_USB_STATE_CBACK *p_cback)
{
	UINT8 *p_data;
	HC_BT_HDR *p_buf;

	if (hcisu_usb_cb.state == USB_STATE_READY
	    && hcisu_usb_cb.pool_evt != GKI_INVALID_POOL
	    && hcisu_usb_cb.pool_acl != GKI_INVALID_POOL)
	{
		GKI_disable();
		hcisu_usb_cb.p_state_cback = p_cback;
		hcisu_usb_cb.state = USB_STATE_OPEN;
		GKI_enable();

		hcisu_usb_post_evt_read();

		do
		{
			p_buf = GKI_getpoolbuf(hcisu_usb_cb.pool_acl);
		} while (p_buf == NULL);

		p_buf->event = MSG_HC_TO_STACK_HCI_ACL;
		p_buf->len = 0;

		p_data = (UINT8 *)(((UINT32)p_buf + 0x27) & ~0x1f);
		p_buf->offset = p_data - (UINT8 *)(p_buf + 1);

		if (fn_80229248(hcisu_usb_cb.fd, hcisu_usb_cb.ep_bulk_in,
		                GKI_get_buf_size(p_buf) - 0x28 - p_buf->offset, p_data,
		                hcisu_usb_acl_read_cback, p_buf) != 0)
		{
			GKI_freebuf(p_buf);
		}

		bte_hcisu_wait = 0;
	}
}

// nothing to read from here, the usb reads are all async
static UINT16 hcisu_usb_read(UINT8 port, UINT8 *p_data, UINT16 len)
{
	return 0;
}

// callers hand over the message as a 4th argument that this never looks at
static UINT16 hcisu_usb_write(int type, UINT8 *p_data, UINT16 len, HC_BT_HDR *p_msg)
{
	int ret = 0;

	if (hcisu_usb_cb.state != USB_STATE_OPEN)
		return 0;

	switch (type)
	{
	case 0:
	{
		HC_BT_HDR *p_buf;
		UINT8 *p_dst;

		p_buf = GKI_getpoolbuf(hcisu_usb_cb.pool_evt);

		if (p_buf == NULL)
			return 0;

		p_buf->len = len;

		p_dst = (UINT8 *)(((UINT32)p_buf + 0x27) & ~0x1f);
		p_buf->offset = p_dst - (UINT8 *)(p_buf + 1);

		memcpy(p_dst, p_data, len);

		if (hcisu_usb_cb.cmd_writing < USB_MAX_PENDING_WRITES
		    && hcisu_usb_cb.cmd_write_q.count == 0)
		{
			ret = fn_802298D4(hcisu_usb_cb.fd, 0x20, 0, 0, 0, len, p_dst,
			                  hcisu_usb_cmd_write_cback, p_buf);
		}
		else
		{
			GKI_enqueue(&hcisu_usb_cb.cmd_write_q, p_buf);
			return 0;
		}

		if (ret != 0)
		{
			GKI_freebuf(p_buf);
		}
		else
		{
			GKI_disable();
			hcisu_usb_cb.cmd_writing++;
			GKI_enable();
		}
	}
		break;

	case 2:
	{
		HC_BT_HDR *p_buf;
		UINT8 *p_dst;

		p_buf = GKI_getpoolbuf(hcisu_usb_cb.pool_acl);

		if (p_buf == NULL)
			return 0;

		p_buf->len = len;

		p_dst = (UINT8 *)(((UINT32)p_buf + 0x27) & ~0x1f);
		p_buf->offset = p_dst - (UINT8 *)(p_buf + 1);

		memcpy(p_dst, p_data, len);

		if (hcisu_usb_cb.acl_writing < USB_MAX_PENDING_WRITES
		    && hcisu_usb_cb.acl_write_q.count == 0)
		{
			ret = fn_802292BC(hcisu_usb_cb.fd, hcisu_usb_cb.ep_bulk_out, len, p_dst,
			                  hcisu_usb_acl_write_cback, p_buf);
		}
		else
		{
			GKI_enqueue(&hcisu_usb_cb.acl_write_q, p_buf);
			return 0;
		}

		if (ret != 0)
		{
			GKI_freebuf(p_buf);
		}
		else
		{
			GKI_disable();
			hcisu_usb_cb.acl_writing++;
			GKI_enable();
		}
	}
		break;

	case 3:
		// sco isn't carried over this transport
		break;
	}

	return ret;
}

static void hcisu_usb_close(void)
{
	void *p_buf;

	GKI_disable();
	hcisu_usb_cb.state = USB_STATE_CLOSED;
	GKI_enable();

	// throw away whatever never got written
	if (hcisu_usb_cb.acl_write_q.count != 0)
	{
		while (hcisu_usb_cb.acl_write_q.count != 0)
		{
			p_buf = GKI_dequeue(&hcisu_usb_cb.acl_write_q);
			GKI_freebuf(p_buf);
		}
	}

	if (hcisu_usb_cb.cmd_write_q.count != 0)
	{
		while (hcisu_usb_cb.cmd_write_q.count != 0)
		{
			p_buf = GKI_dequeue(&hcisu_usb_cb.cmd_write_q);
			GKI_freebuf(p_buf);
		}
	}

	IUSB_CloseDeviceAsync(hcisu_usb_cb.fd, hcisu_usb_closed_cback, NULL);
}

static void hcisu_usb_reset(void)
{
	IUSB_CloseLib();

	GKI_disable();
	hcisu_usb_cb.state = USB_STATE_IDLE;
	hcisu_usb_cb.init_done = 0;
	hcisu_usb_init_done = 0;
	GKI_enable();
}

tHCI_IF const hcisu_h2 = {
	hcisu_h2_init,
	hcisu_h2_open,
	hcisu_h2_close,
	hcisu_h2_send,
	hcisu_h2_handle_event,
};
