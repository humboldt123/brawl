#include <port_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/rfcomm/port_utils.c
 */

/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
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

#include <bt_target.h>
#include <bt_trace.h>
#include <bt_types.h>
#include <data_types.h>

#include <btm_api.h>
#include <btm_int.h>
#include <gki.h>
#include <rfc_int.h>

/*******************************************************************************
 * macros
 */

// HYPOTHESIS: local name for a literal the headers don't spell out
#define PORT_L2CAP_PKT_OVERHEAD		4

/*******************************************************************************
 * functions
 */

void port_select_mtu(tPORT *p_port)
{
	UINT16 packet_size;

	if (p_port->mtu == 0)
	{
		packet_size = btm_get_max_packet_size(p_port->bd_addr);

		if (packet_size == 0)
		{
			RFCOMM_TRACE(WARNING, "port_select_mtu bad packet size");

			p_port->mtu = RFCOMM_DEFAULT_MTU;
		}
		else
		{
			if (packet_size <= L2CAP_MTU_SIZE + PORT_L2CAP_PKT_OVERHEAD)
			{
				p_port->mtu =
					(L2CAP_MTU_SIZE + PORT_L2CAP_PKT_OVERHEAD) / packet_size
						* packet_size
					- PORT_L2CAP_PKT_OVERHEAD - RFCOMM_DATA_OVERHEAD;

				RFCOMM_TRACE(DEBUG,
				             "port_select_mtu selected %d based on connection speed",
				             p_port->mtu);
			}
			else
			{
				p_port->mtu = L2CAP_MTU_SIZE - RFCOMM_DATA_OVERHEAD;

				RFCOMM_TRACE(DEBUG,
				             "port_select_mtu selected %d based on l2cap PDU size",
				             p_port->mtu);
			}
		}
	}
	else
	{
		RFCOMM_TRACE(DEBUG, "port_select_mtu application selected %d",
		             p_port->mtu);
	}

	p_port->credit_rx_max = PORT_RX_HIGH_WM / p_port->mtu;
	p_port->credit_rx_low = PORT_RX_LOW_WM / p_port->mtu;
	p_port->rx_buf_critical = PORT_RX_CRITICAL_WM / p_port->mtu;

	RFCOMM_TRACE(DEBUG,
	             "port_select_mtu credit_rx_max %d, credit_rx_low %d, "
	             "rx_buf_critical %d",
	             p_port->credit_rx_max, p_port->credit_rx_low,
	             p_port->rx_buf_critical);
}

void port_release_port(tPORT *p_port)
{
	BT_HDR *p_buf;

	while ((p_buf = (BT_HDR *)GKI_dequeue(&p_port->rx.queue)) != NULL)
		GKI_freebuf(p_buf);

	p_port->rx.queue_size = 0;

	while ((p_buf = (BT_HDR *)GKI_dequeue(&p_port->tx.queue)) != NULL)
		GKI_freebuf(p_buf);

	p_port->tx.queue_size = 0;

	p_port->state = PORT_STATE_CLOSED;

	if (p_port->rfc.state == RFC_STATE_CLOSED)
	{
		RFCOMM_TRACE(DEBUG, "rfc_port_closed DONE");

		if (p_port->rfc.p_mcb)
		{
			p_port->rfc.p_mcb->port_inx[p_port->dlci] = 0;

			rfc_check_mcb_active(p_port->rfc.p_mcb);
		}

		rfc_port_timer_stop(p_port);

		memset(p_port, 0, sizeof(tPORT));
	}
}

tPORT *port_find_mcb_dlci_port(tRFC_MCB *p_mcb, UINT8 dlci)
{
	UINT8 inx;

	if (!p_mcb)
		return NULL;

	if (dlci > RFCOMM_MAX_DLCI)
		return NULL;

	inx = p_mcb->port_inx[dlci];

	if (inx == 0)
		return NULL;
	else
		return &rfc_cb.port.port[inx - 1];
}

tPORT *port_find_dlci_port(UINT8 dlci)
{
	UINT16 i;
	tPORT *p_port;

	for (i = 0; i < MAX_RFC_PORTS; i++)
	{
		p_port = &rfc_cb.port.port[i];

		if (p_port->in_use && p_port->rfc.p_mcb == NULL)
		{
			if (p_port->dlci == dlci)
			{
				return p_port;
			}
			else if ((dlci & 0x01) && p_port->dlci == (dlci - 1))
			{
				p_port->dlci++;
				return p_port;
			}
		}
	}

	return NULL;
}

UINT32 port_flow_control_user(tPORT *p_port)
{
	BOOLEAN fc;

	// the user gets flow controlled when the peer says so, the link isn't ready,
	// or the tx queue backed up too far
	fc = p_port->tx.peer_fc
	     || !p_port->rfc.p_mcb
	     || !p_port->rfc.p_mcb->peer_ready
	     || p_port->tx.queue_size > PORT_TX_HIGH_WM
	     || p_port->tx.queue.count > PORT_TX_BUF_HIGH_WM;

	if (p_port->tx.user_fc == fc)
		return 0;

	p_port->tx.user_fc = fc;

	if (fc)
		return PORT_EV_FC;
	else
		return PORT_EV_FC | PORT_EV_FCS;
}

UINT32 port_get_signal_changes(tPORT *p_port, UINT8 old_signals, UINT8 signal)
{
	UINT8 changed_signals;
	UINT32 events = 0;

	changed_signals = (signal ^ old_signals);

	if (changed_signals & PORT_DTRDSR_ON)
	{
		events |= PORT_EV_DSR;

		if (signal & PORT_DTRDSR_ON)
			events |= PORT_EV_DSRS;
	}

	if (changed_signals & PORT_CTSRTS_ON)
	{
		events |= PORT_EV_CTS;

		if (signal & PORT_CTSRTS_ON)
			events |= PORT_EV_CTSS;
	}

	if (changed_signals & PORT_RING_ON)
		events |= PORT_EV_RING;

	if (changed_signals & PORT_DCD_ON)
	{
		events |= PORT_EV_RLSD;

		if (signal & PORT_DCD_ON)
			events |= PORT_EV_RLSDS;
	}

	return p_port->ev_mask & events;
}

void port_flow_control_peer(tPORT *p_port, BOOLEAN enable, UINT16 count)
{
	if (!p_port->rfc.p_mcb)
		return;

	if (p_port->rfc.p_mcb->flow == PORT_FC_CREDIT)
	{
		if (enable)
		{
			if (count > p_port->credit_rx)
				p_port->credit_rx = 0;
			else
				p_port->credit_rx -= count;

			if (p_port->credit_rx <= p_port->credit_rx_low
			    && !p_port->rx.user_fc
			    && p_port->credit_rx_max > p_port->credit_rx)
			{
				rfc_send_credit(p_port->rfc.p_mcb, p_port->dlci,
				                (UINT8)(p_port->credit_rx_max - p_port->credit_rx));

				p_port->credit_rx = p_port->credit_rx_max;
				p_port->rx.peer_fc = FALSE;
			}
		}
		else
		{
			if (p_port->p_data_callback)
				p_port->rx.peer_fc = TRUE;
			else if (p_port->rx.queue.count >= p_port->credit_rx_max)
				p_port->rx.peer_fc = TRUE;
		}
	}
	else
	{
		if (enable)
		{
			if (p_port->rx.peer_fc
			    && p_port->rx.queue_size < PORT_RX_LOW_WM
			    && p_port->rx.queue.count < PORT_RX_BUF_LOW_WM)
			{
				p_port->rx.peer_fc = FALSE;

				if (!p_port->rx.user_fc)
					RFCOMM_FlowReq(p_port->rfc.p_mcb, p_port->dlci, TRUE);
			}
		}
		else
		{
			if (p_port->p_data_callback)
			{
				p_port->rx.peer_fc = TRUE;
				RFCOMM_FlowReq(p_port->rfc.p_mcb, p_port->dlci, FALSE);
			}
			else if (p_port->rx.queue_size > PORT_RX_HIGH_WM
			         || p_port->rx.queue.count > PORT_RX_BUF_HIGH_WM)
			{
				if (!p_port->rx.peer_fc)
				{
					RFCOMM_TRACE(EVENT, "PORT_DataInd Data reached HW. Sending FC set.");

					p_port->rx.peer_fc = TRUE;
					RFCOMM_FlowReq(p_port->rfc.p_mcb, p_port->dlci, FALSE);
				}
			}
		}
	}
}
