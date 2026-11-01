#include <l2c_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/l2cap/l2c_utils.c
 */

/* Includes changes by muff1n1634 */

/*******************************************************************************
 * headers
 */

#include <cstring>
#include <stddef.h>

#include <bt_target.h>
#include <bt_trace.h>
#include <bt_types.h>
#include <data_types.h>

#include <btm_api.h>
#include <btm_int.h>
#include <btu.h>
#include <gki.h>
#include <hcidefs.h>
#include <hcimsgs.h>
#include <l2c_api.h>
#include <l2cdefs.h>

/*******************************************************************************
 * macros
 */

// HYPOTHESIS: names for literals that headers don't have yet
#define L2CAP_MIN_MTU_VALUE				0x30
#define HCI_MAX_AUTO_FLUSH_TOUT_VALUE	0x4ff
#define L2CAP_ECHO_MAX_MTU				0x294

/*******************************************************************************
 * functions
 */

tL2C_LCB *l2cu_allocate_lcb(BD_ADDR p_bd_addr)
{
	int xx;
	tL2C_LCB *p_lcb = &l2cb.lcb_pool[0];

	for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_lcb++)
	{
		if (!p_lcb->in_use)
		{
			memset(p_lcb, 0, sizeof(tL2C_LCB));

			p_lcb->in_use = TRUE;
			memcpy(p_lcb->remote_bd_addr, p_bd_addr, BD_ADDR_LEN);

			p_lcb->link_state = LST_DISCONNECTED;
			p_lcb->handle = 0xffff;
			p_lcb->p_hcit_rcv_acl = NULL;
			p_lcb->link_flush_tout = 0xffff;
			p_lcb->timer_entry.param = (TIMER_PARAM_TYPE)p_lcb;
			p_lcb->sent_not_acked = 0;
			p_lcb->idle_timeout = l2cb.idle_timeout;
			p_lcb->id = 0;

			l2cb.num_links_active++;

			l2c_link_adjust_allocation();

			return p_lcb;
		}
	}

	return NULL;
}

void l2cu_release_lcb(tL2C_LCB *p_lcb)
{
	tL2C_CCB *p_ccb;

	p_lcb->in_use = FALSE;

	btu_stop_timer(&p_lcb->timer_entry);

	if (p_lcb->p_hcit_rcv_acl)
	{
		GKI_freebuf(p_lcb->p_hcit_rcv_acl);
		p_lcb->p_hcit_rcv_acl = NULL;
	}

	btm_remove_sco_links(p_lcb->remote_bd_addr);

	for (p_ccb = p_lcb->p_first_ccb; p_ccb; p_ccb = p_lcb->p_first_ccb)
		l2cu_release_ccb(p_ccb);

	if (p_lcb->link_state == LST_CONNECTED
	    || p_lcb->link_state == LST_DISCONNECTING)
	{
		btm_acl_removed(p_lcb->remote_bd_addr);
	}

	while (p_lcb->link_xmit_data_q.p_first)
		GKI_freebuf(GKI_dequeue(&p_lcb->link_xmit_data_q));

	if (l2cb.num_links_active >= 1)
		l2cb.num_links_active--;

	l2cb.controller_xmit_window += p_lcb->sent_not_acked;

	l2c_link_adjust_allocation();

	if (p_lcb->p_echo_rsp_cb)
	{
		tL2CA_ECHO_RSP_CB *p_cb = p_lcb->p_echo_rsp_cb;

		p_lcb->p_echo_rsp_cb = NULL;

		(*p_cb)(L2CAP_PING_RESULT_NO_LINK);
	}
}

tL2C_LCB *l2cu_find_lcb_by_bd_addr(BD_ADDR p_bd_addr)
{
	int xx;
	tL2C_LCB *p_lcb = &l2cb.lcb_pool[0];

	for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_lcb++)
	{
		if (p_lcb->in_use && !memcmp(p_lcb->remote_bd_addr, p_bd_addr, BD_ADDR_LEN))
			return p_lcb;
	}

	return NULL;
}

tL2C_LCB *l2cu_find_lcb_by_handle(UINT16 handle)
{
	int xx;
	tL2C_LCB *p_lcb = &l2cb.lcb_pool[0];

	for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_lcb++)
	{
		if (p_lcb->in_use && p_lcb->handle == handle)
			return p_lcb;
	}

	return NULL;
}

UINT8 l2cu_get_conn_role(tL2C_LCB *p_this_lcb)
{
	return l2cb.desire_role;
}

BT_HDR *l2cu_build_header(UINT16 hci_handle, UINT16 len, UINT8 cmd, UINT8 id)
{
	BT_HDR *p_buf = (BT_HDR *)GKI_getpoolbuf(L2CAP_CMD_POOL_ID);
	UINT8 *p;

	if (!p_buf)
		return NULL;

	p_buf->offset = L2CAP_SEND_CMD_OFFSET;
	p_buf->len = len + 4 + L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD;

	p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET;

	UINT16_TO_STREAM(p, hci_handle | (L2CAP_PKT_START << L2CAP_PKT_TYPE_SHIFT));
	UINT16_TO_STREAM(p, len + L2CAP_PKT_OVERHEAD + L2CAP_CMD_OVERHEAD);
	UINT16_TO_STREAM(p, len + L2CAP_CMD_OVERHEAD);
	UINT16_TO_STREAM(p, L2CAP_SIGNALLING_CID);

	UINT8_TO_STREAM(p, cmd);
	UINT8_TO_STREAM(p, id);
	UINT16_TO_STREAM(p, len);

	return p_buf;
}

void l2cu_send_peer_cmd_reject(tL2C_LCB *p_lcb, UINT16 reason, UINT8 rem_id,
                               UINT16 p1, UINT16 p2)
{
	UINT16 param_len;
	BT_HDR *p_buf;
	UINT8 *p;

	if (reason == L2CAP_CMD_REJ_MTU_EXCEEDED)
		param_len = 2;
	else if (reason == L2CAP_CMD_REJ_INVALID_CID)
		param_len = 4;
	else
		param_len = 0;

	if ((p_buf = l2cu_build_header(p_lcb->handle,
	                               (UINT16)(L2CAP_CMD_REJECT_LEN + param_len),
	                               L2CAP_CMD_REJECT, rem_id))
	    == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP - no buffer cmd_rej");
		return;
	}

	p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + 4 + L2CAP_PKT_OVERHEAD
	  + L2CAP_CMD_OVERHEAD;

	UINT16_TO_STREAM(p, reason);

	if (param_len >= 2)
		UINT16_TO_STREAM(p, p1);

	if (param_len >= 4)
		UINT16_TO_STREAM(p, p2);

	l2c_link_check_send_pkts(p_lcb, NULL, p_buf);
}

void l2cu_send_peer_connect_req(tL2C_CCB *p_ccb)
{
	BT_HDR *p_buf;
	UINT8 *p;

	p_ccb->local_id = ++p_ccb->p_lcb->id;

	if ((p_buf = l2cu_build_header(p_ccb->p_lcb->handle, L2CAP_CONN_REQ_LEN,
	                               L2CAP_CMD_CONN_REQ, p_ccb->local_id))
	    == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP - no buffer for conn_req");
		return;
	}

	p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + 4 + L2CAP_PKT_OVERHEAD
	  + L2CAP_CMD_OVERHEAD;

	UINT16_TO_STREAM(p, p_ccb->p_rcb->psm);
	UINT16_TO_STREAM(p, p_ccb->local_cid);

	l2c_link_check_send_pkts(p_ccb->p_lcb, NULL, p_buf);
}

void l2cu_send_peer_connect_rsp(tL2C_CCB *p_ccb, UINT16 result, UINT16 status)
{
	BT_HDR *p_buf;
	UINT8 *p;

	if ((p_buf = l2cu_build_header(p_ccb->p_lcb->handle, L2CAP_CONN_RSP_LEN,
	                               L2CAP_CMD_CONN_RSP, p_ccb->remote_id))
	    == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP - no buffer for conn_req");
		return;
	}

	p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + 4 + L2CAP_PKT_OVERHEAD
	  + L2CAP_CMD_OVERHEAD;

	UINT16_TO_STREAM(p, p_ccb->local_cid);
	UINT16_TO_STREAM(p, p_ccb->remote_cid);
	UINT16_TO_STREAM(p, result);
	UINT16_TO_STREAM(p, status);

	l2c_link_check_send_pkts(p_ccb->p_lcb, NULL, p_buf);
}

void l2cu_reject_connection(tL2C_LCB *p_lcb, UINT16 remote_cid, UINT8 rem_id,
                            UINT16 result)
{
	BT_HDR *p_buf;
	UINT8 *p;

	if ((p_buf = l2cu_build_header(p_lcb->handle, L2CAP_CONN_RSP_LEN,
	                               L2CAP_CMD_CONN_RSP, rem_id))
	    == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP - no buffer for conn_req");
		return;
	}

	p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + 4 + L2CAP_PKT_OVERHEAD
	  + L2CAP_CMD_OVERHEAD;

	UINT16_TO_STREAM(p, 0);
	UINT16_TO_STREAM(p, remote_cid);
	UINT16_TO_STREAM(p, result);
	UINT16_TO_STREAM(p, 0);

	l2c_link_check_send_pkts(p_lcb, NULL, p_buf);
}

void l2cu_send_peer_config_req(tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
	BT_HDR *p_buf;
	UINT16 cfg_len = 0;
	UINT8 *p;

	p_ccb->local_id = ++p_ccb->p_lcb->id;

	if (p_cfg->mtu_present)
		cfg_len += L2CAP_CFG_MTU_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;

	if (p_cfg->flush_to_present)
		cfg_len += L2CAP_CFG_FLUSH_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;

	if (p_cfg->qos_present)
		cfg_len += L2CAP_CFG_QOS_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;

	if (p_cfg->fcr_present)
		cfg_len += L2CAP_CFG_FCR_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;

	if ((p_buf = l2cu_build_header(p_ccb->p_lcb->handle,
	                               (UINT16)(L2CAP_CONFIG_REQ_LEN + cfg_len),
	                               L2CAP_CMD_CONFIG_REQ, p_ccb->local_id))
	    == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP - no buffer for conn_req");
		return;
	}

	p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + 4 + L2CAP_PKT_OVERHEAD
	  + L2CAP_CMD_OVERHEAD;

	UINT16_TO_STREAM(p, p_ccb->remote_cid);
	UINT16_TO_STREAM(p, 0);

	if (p_cfg->mtu_present)
	{
		UINT8_TO_STREAM(p, L2CAP_CFG_TYPE_MTU);
		UINT8_TO_STREAM(p, L2CAP_CFG_MTU_OPTION_LEN);
		UINT16_TO_STREAM(p, p_cfg->mtu);
	}

	if (p_cfg->flush_to_present)
	{
		UINT8_TO_STREAM(p, L2CAP_CFG_TYPE_FLUSH_TOUT);
		UINT8_TO_STREAM(p, L2CAP_CFG_FLUSH_OPTION_LEN);
		UINT16_TO_STREAM(p, p_cfg->flush_to);
	}

	if (p_cfg->qos_present)
	{
		UINT8_TO_STREAM(p, L2CAP_CFG_TYPE_QOS);
		UINT8_TO_STREAM(p, L2CAP_CFG_QOS_OPTION_LEN);
		UINT8_TO_STREAM(p, p_cfg->qos.qos_flags);
		UINT8_TO_STREAM(p, p_cfg->qos.service_type);
		UINT32_TO_STREAM(p, p_cfg->qos.token_rate);
		UINT32_TO_STREAM(p, p_cfg->qos.token_bucket_size);
		UINT32_TO_STREAM(p, p_cfg->qos.peak_bandwidth);
		UINT32_TO_STREAM(p, p_cfg->qos.latency);
		UINT32_TO_STREAM(p, p_cfg->qos.delay_variation);
	}

	if (p_cfg->fcr_present)
	{
		UINT8_TO_STREAM(p, L2CAP_CFG_TYPE_FCR);
		UINT8_TO_STREAM(p, L2CAP_CFG_FCR_OPTION_LEN);
		UINT8_TO_STREAM(p, p_cfg->fcr.mode);
		UINT8_TO_STREAM(p, p_cfg->fcr.tx_win_sz);
		UINT8_TO_STREAM(p, p_cfg->fcr.max_transmit);
		UINT16_TO_STREAM(p, p_cfg->fcr.rtrans_tout);
		UINT16_TO_STREAM(p, p_cfg->fcr.mon_tout);
		UINT16_TO_STREAM(p, p_cfg->fcr.mps);
	}

	l2c_link_check_send_pkts(p_ccb->p_lcb, NULL, p_buf);
}

void l2cu_send_peer_config_rsp(tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
	BT_HDR *p_buf;
	UINT16 cfg_len = 0;
	UINT8 *p;

	if (p_cfg->mtu_present)
		cfg_len += L2CAP_CFG_MTU_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;

	if (p_cfg->flush_to_present)
		cfg_len += L2CAP_CFG_FLUSH_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;

	if (p_cfg->qos_present)
		cfg_len += L2CAP_CFG_QOS_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;

	if (p_cfg->fcr_present)
		cfg_len += L2CAP_CFG_FCR_OPTION_LEN + L2CAP_CFG_OPTION_OVERHEAD;

	if ((p_buf = l2cu_build_header(p_ccb->p_lcb->handle,
	                               (UINT16)(L2CAP_CONFIG_RSP_LEN + cfg_len),
	                               L2CAP_CMD_CONFIG_RSP, p_ccb->remote_id))
	    == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP - no buffer for conn_req");
		return;
	}

	p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + 4 + L2CAP_PKT_OVERHEAD
	  + L2CAP_CMD_OVERHEAD;

	UINT16_TO_STREAM(p, p_ccb->remote_cid);
	UINT16_TO_STREAM(p, 0);
	UINT16_TO_STREAM(p, p_cfg->result);

	if (p_cfg->mtu_present)
	{
		UINT8_TO_STREAM(p, L2CAP_CFG_TYPE_MTU);
		UINT8_TO_STREAM(p, L2CAP_CFG_MTU_OPTION_LEN);
		UINT16_TO_STREAM(p, p_cfg->mtu);
	}

	if (p_cfg->flush_to_present)
	{
		UINT8_TO_STREAM(p, L2CAP_CFG_TYPE_FLUSH_TOUT);
		UINT8_TO_STREAM(p, L2CAP_CFG_FLUSH_OPTION_LEN);
		UINT16_TO_STREAM(p, p_cfg->flush_to);
	}

	if (p_cfg->qos_present)
	{
		UINT8_TO_STREAM(p, L2CAP_CFG_TYPE_QOS);
		UINT8_TO_STREAM(p, L2CAP_CFG_QOS_OPTION_LEN);
		UINT8_TO_STREAM(p, p_cfg->qos.qos_flags);
		UINT8_TO_STREAM(p, p_cfg->qos.service_type);
		UINT32_TO_STREAM(p, p_cfg->qos.token_rate);
		UINT32_TO_STREAM(p, p_cfg->qos.token_bucket_size);
		UINT32_TO_STREAM(p, p_cfg->qos.peak_bandwidth);
		UINT32_TO_STREAM(p, p_cfg->qos.latency);
		UINT32_TO_STREAM(p, p_cfg->qos.delay_variation);
	}

	if (p_cfg->fcr_present)
	{
		UINT8_TO_STREAM(p, L2CAP_CFG_TYPE_FCR);
		UINT8_TO_STREAM(p, L2CAP_CFG_FCR_OPTION_LEN);
		UINT8_TO_STREAM(p, p_cfg->fcr.mode);
		UINT8_TO_STREAM(p, p_cfg->fcr.tx_win_sz);
		UINT8_TO_STREAM(p, p_cfg->fcr.max_transmit);
		UINT16_TO_STREAM(p, p_cfg->fcr.rtrans_tout);
		UINT16_TO_STREAM(p, p_cfg->fcr.mon_tout);
		UINT16_TO_STREAM(p, p_cfg->fcr.mps);
	}

	l2c_link_check_send_pkts(p_ccb->p_lcb, NULL, p_buf);
}

void l2cu_send_peer_config_rej(tL2C_CCB *p_ccb, UINT8 *p_data, UINT16 data_len,
                               UINT16 rej_len)
{
	BT_HDR *p_buf;
	UINT16 len;
	UINT8 cfg_len;
	UINT8 *p, *p_hci_len, *p_data_end;
	UINT8 cfg_code;

	if ((p_buf = (BT_HDR *)GKI_getpoolbuf(L2CAP_CMD_POOL_ID)) == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP - no buffer for cfg_rej");
		return;
	}

	p_buf->offset = L2CAP_SEND_CMD_OFFSET;
	p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET;

	UINT16_TO_STREAM(p, p_ccb->p_lcb->handle | (L2CAP_PKT_START << L2CAP_PKT_TYPE_SHIFT));

	p_hci_len = p;
	p += 2;

	UINT16_TO_STREAM(p, rej_len + L2CAP_CMD_OVERHEAD + L2CAP_CONFIG_RSP_LEN);
	UINT16_TO_STREAM(p, L2CAP_SIGNALLING_CID);

	UINT8_TO_STREAM(p, L2CAP_CMD_CONFIG_RSP);
	UINT8_TO_STREAM(p, p_ccb->remote_id);
	UINT16_TO_STREAM(p, rej_len + L2CAP_CONFIG_RSP_LEN);

	UINT16_TO_STREAM(p, p_ccb->remote_cid);
	UINT16_TO_STREAM(p, 0);
	UINT16_TO_STREAM(p, L2CAP_CFG_UNKNOWN_OPTIONS);

	p_data_end = p_data + data_len;

	while (p_data < p_data_end)
	{
		cfg_code = *p_data;
		cfg_len = p_data[1];

		switch (cfg_code & 0x7f)
		{
		case L2CAP_CFG_TYPE_MTU:
		case L2CAP_CFG_TYPE_FLUSH_TOUT:
		case L2CAP_CFG_TYPE_QOS:
			p_data += cfg_len + L2CAP_CFG_OPTION_OVERHEAD;
			break;

		default:
			if (cfg_len + L2CAP_CFG_OPTION_OVERHEAD <= data_len)
			{
				if (!(cfg_code & 0x80))
				{
					memcpy(p, p_data, cfg_len + L2CAP_CFG_OPTION_OVERHEAD);
					p += cfg_len + L2CAP_CFG_OPTION_OVERHEAD;
				}

				p_data += cfg_len + L2CAP_CFG_OPTION_OVERHEAD;
			}
			else
			{
				p_data = p_data_end;
			}

			break;
		}
	}

	len = (UINT16)(p - p_hci_len - 2);
	UINT16_TO_STREAM(p_hci_len, len);

	p_buf->len = len + 4;

	l2c_link_check_send_pkts(p_ccb->p_lcb, p_ccb, p_buf);
}

void l2cu_send_peer_disc_req(tL2C_CCB *p_ccb)
{
	BT_HDR *p_buf;
	UINT8 *p;

	p_ccb->local_id = ++p_ccb->p_lcb->id;

	if ((p_buf = l2cu_build_header(p_ccb->p_lcb->handle, L2CAP_DISC_REQ_LEN,
	                               L2CAP_CMD_DISC_REQ, p_ccb->local_id))
	    == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP - no buffer for conn_req");
		return;
	}

	p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + 4 + L2CAP_PKT_OVERHEAD
	  + L2CAP_CMD_OVERHEAD;

	UINT16_TO_STREAM(p, p_ccb->remote_cid);
	UINT16_TO_STREAM(p, p_ccb->local_cid);

	p_buf->layer_specific = 0;
	GKI_enqueue(&p_ccb->p_lcb->link_xmit_data_q, p_buf);

	l2c_link_check_send_pkts(p_ccb->p_lcb, NULL, NULL);
}

void l2cu_send_peer_disc_rsp(tL2C_LCB *p_lcb, UINT8 remote_id, UINT16 local_cid,
                             UINT16 remote_cid)
{
	BT_HDR *p_buf;
	UINT8 *p;

	if ((p_buf = l2cu_build_header(p_lcb->handle, L2CAP_DISC_RSP_LEN,
	                               L2CAP_CMD_DISC_RSP, remote_id))
	    == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP - no buffer for conn_req");
		return;
	}

	p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + 4 + L2CAP_PKT_OVERHEAD
	  + L2CAP_CMD_OVERHEAD;

	UINT16_TO_STREAM(p, local_cid);
	UINT16_TO_STREAM(p, remote_cid);

	p_buf->layer_specific = 0;
	GKI_enqueue(&p_lcb->link_xmit_data_q, p_buf);

	l2c_link_check_send_pkts(p_lcb, NULL, NULL);
}

void l2cu_send_peer_echo_req(tL2C_LCB *p_lcb, UINT8 *p_data, UINT16 data_len)
{
	BT_HDR *p_buf;
	UINT8 *p;

	if ((p_buf = l2cu_build_header(p_lcb->handle, data_len, L2CAP_CMD_ECHO_REQ,
	                               ++p_lcb->id))
	    == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP - no buffer for echo_req");
		return;
	}

	p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + 4 + L2CAP_PKT_OVERHEAD
	  + L2CAP_CMD_OVERHEAD;

	if (data_len)
		ARRAY_TO_STREAM(p, p_data, data_len);

	l2c_link_check_send_pkts(p_lcb, NULL, p_buf);
}

void l2cu_send_peer_echo_rsp(tL2C_LCB *p_lcb, UINT8 id, UINT8 *p_data,
                             UINT16 data_len)
{
	BT_HDR *p_buf;
	UINT8 *p;
	UINT16 maxlen;

	maxlen = btu_cb.hcit_acl_pkt_size < L2CAP_ECHO_MAX_MTU
	             ? btu_cb.hcit_acl_data_size
	             : L2CAP_ECHO_MAX_MTU;

	if (data_len > (UINT16)(maxlen - 12))
		data_len = 0;

	if ((p_buf = l2cu_build_header(p_lcb->handle, data_len, L2CAP_CMD_ECHO_RSP,
	                               id))
	    == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP - no buffer for conn_req");
		return;
	}

	p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + 4 + L2CAP_PKT_OVERHEAD
	  + L2CAP_CMD_OVERHEAD;

	if (data_len)
		ARRAY_TO_STREAM(p, p_data, data_len);

	l2c_link_check_send_pkts(p_lcb, NULL, p_buf);
}

void l2cu_send_peer_info_rsp(tL2C_LCB *p_lcb, UINT8 remote_id,
                             UINT16 info_type)
{
	BT_HDR *p_buf;
	UINT8 *p;

	if ((p_buf = l2cu_build_header(p_lcb->handle, L2CAP_INFO_RSP_LEN,
	                               L2CAP_CMD_INFO_RSP, remote_id))
	    == NULL)
	{
		L2CAP_TRACE(WARNING, "L2CAP - no buffer for conn_req");
		return;
	}

	p = (UINT8 *)(p_buf + 1) + L2CAP_SEND_CMD_OFFSET + 4 + L2CAP_PKT_OVERHEAD
	  + L2CAP_CMD_OVERHEAD;

	UINT16_TO_STREAM(p, info_type);
	UINT16_TO_STREAM(p, L2CAP_INFO_RESP_RESULT_NOT_SUPPORTED);

	l2c_link_check_send_pkts(p_lcb, NULL, p_buf);
}

tL2C_CCB *l2cu_allocate_ccb(tL2C_LCB *p_lcb)
{
	tL2C_CCB *p_ccb;

	if (!l2cb.p_free_ccb_first)
		return NULL;

	p_ccb = l2cb.p_free_ccb_first;
	l2cb.p_free_ccb_first = p_ccb->p_next_ccb;

	p_ccb->in_use = TRUE;

	p_ccb->local_cid = L2CAP_BASE_APPL_CID + (UINT16)(p_ccb - l2cb.ccb_pool);

	p_ccb->p_lcb = p_lcb;

	if (!p_lcb->p_first_ccb)
	{
		p_lcb->p_first_ccb = p_lcb->p_last_ccb = p_ccb;
		p_ccb->p_prev_ccb = p_ccb->p_next_ccb = NULL;
	}
	else
	{
		p_ccb->p_next_ccb = NULL;
		p_ccb->p_prev_ccb = p_lcb->p_last_ccb;
		p_lcb->p_last_ccb->p_next_ccb = p_ccb;
		p_lcb->p_last_ccb = p_ccb;
	}

	p_ccb->our_flush_to = 0xffff;
	p_ccb->out_mtu = L2CAP_DEFAULT_MTU;
	p_ccb->in_mtu = L2CAP_DEFAULT_MTU;

	p_ccb->out_flow.service_type = BEST_EFFORT;
	p_ccb->in_flow.service_type = BEST_EFFORT;
	p_ccb->out_flow.token_rate = 0;
	p_ccb->in_flow.token_rate = 0;
	p_ccb->out_flow.token_bucket_size = 0;
	p_ccb->in_flow.token_bucket_size = 0;
	p_ccb->out_flow.peak_bandwidth = 0;
	p_ccb->in_flow.peak_bandwidth = 0;
	p_ccb->out_flow.latency = 0xffffffff;
	p_ccb->in_flow.latency = 0xffffffff;
	p_ccb->out_flow.delay_variation = 0xffffffff;
	p_ccb->in_flow.delay_variation = 0xffffffff;

	p_ccb->config_done = 0;
	p_ccb->chnl_state = CST_CLOSED;
	p_ccb->rtry_flag = TRUE;

	p_ccb->timer_entry.param = (TIMER_PARAM_TYPE)p_ccb;

	return p_ccb;
}

void l2cu_release_ccb(tL2C_CCB *p_ccb)
{
	tL2C_LCB *p_lcb = p_ccb->p_lcb;

	if (!p_ccb->in_use)
		return;

	p_ccb->in_use = FALSE;

	btu_stop_timer(&p_ccb->timer_entry);

	while (p_ccb->xmit_hold_q.p_first)
		GKI_freebuf(GKI_dequeue(&p_ccb->xmit_hold_q));

	p_ccb->p_lcb = NULL;

	if (p_lcb->p_first_ccb == p_ccb)
	{
		p_lcb->p_first_ccb = p_ccb->p_next_ccb;

		if (p_lcb->p_first_ccb)
			p_lcb->p_first_ccb->p_prev_ccb = NULL;
	}
	else if (p_lcb->p_last_ccb == p_ccb)
	{
		p_lcb->p_last_ccb = p_ccb->p_prev_ccb;
		p_lcb->p_last_ccb->p_next_ccb = NULL;
	}
	else
	{
		p_ccb->p_prev_ccb->p_next_ccb = p_ccb->p_next_ccb;
		p_ccb->p_next_ccb->p_prev_ccb = p_ccb->p_prev_ccb;
	}

	if (!l2cb.p_free_ccb_first)
	{
		l2cb.p_free_ccb_first = p_ccb;
		l2cb.p_free_ccb_last = p_ccb;
		p_ccb->p_next_ccb = NULL;
		p_ccb->p_prev_ccb = NULL;
	}
	else
	{
		p_ccb->p_next_ccb = NULL;
		p_ccb->p_prev_ccb = l2cb.p_free_ccb_last;
		l2cb.p_free_ccb_last->p_next_ccb = p_ccb;
		l2cb.p_free_ccb_last = p_ccb;
	}

	if (p_lcb->in_use && p_lcb->link_state == LST_CONNECTED
	    && !p_lcb->p_first_ccb)
	{
		UINT16 timeout;

		if (p_lcb->idle_timeout == 0)
		{
			tBTM_STATUS rc = btm_sec_disconnect(p_lcb->handle, HCI_ERR_PEER_USER);

			if (rc == BTM_CMD_STARTED)
			{
				p_lcb->link_state = LST_DISCONNECTING;
				timeout = L2CAP_LINK_DISCONNECT_TOUT;
			}
			else if (rc == BTM_SUCCESS)
			{
				p_lcb->link_state = LST_DISCONNECTING;
				timeout = 0xffff;
			}
			else
			{
				timeout = BT_1SEC_TIMEOUT;
			}
		}
		else
		{
			timeout = p_lcb->idle_timeout;
		}

		if (timeout != 0xffff)
		{
			btu_start_timer(&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK,
			                timeout);
		}
	}
}

tL2C_CCB *l2cu_find_ccb_by_cid(tL2C_LCB *p_lcb, UINT16 local_cid)
{
	tL2C_CCB *p_ccb = NULL;

	if (local_cid >= L2CAP_BASE_APPL_CID)
	{
		p_ccb = l2cb.ccb_pool + (UINT16)(local_cid - L2CAP_BASE_APPL_CID);

		if (!p_ccb->in_use)
			p_ccb = NULL;
		else if (p_lcb && p_lcb != p_ccb->p_lcb)
			p_ccb = NULL;
	}

	return p_ccb;
}

tL2C_RCB *l2cu_allocate_rcb(UINT16 psm)
{
	tL2C_RCB *p_rcb = &l2cb.rcb_pool[0];
	UINT16 xx;

	for (xx = 0; xx < MAX_L2CAP_CLIENTS; xx++, p_rcb++)
	{
		if (!p_rcb->in_use)
		{
			p_rcb->in_use = TRUE;
			p_rcb->psm = psm;
			return p_rcb;
		}
	}

	return NULL;
}

void l2cu_release_rcb(tL2C_RCB *p_rcb)
{
	p_rcb->in_use = FALSE;
	p_rcb->psm = 0;
}

tL2C_RCB *l2cu_find_rcb_by_psm(UINT16 psm)
{
	tL2C_RCB *p_rcb = &l2cb.rcb_pool[0];
	UINT16 xx;

	for (xx = 0; xx < MAX_L2CAP_CLIENTS; xx++, p_rcb++)
	{
		if (p_rcb->in_use && p_rcb->psm == psm)
			return p_rcb;
	}

	return NULL;
}

BOOLEAN l2cu_process_peer_cfg_req(tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
	BOOLEAN mtu_ok = TRUE;
	BOOLEAN qos_ok = TRUE;
	BOOLEAN flush_ok = TRUE;
	BOOLEAN rc = FALSE;

	if (p_cfg->mtu_present)
	{
		if (p_cfg->mtu >= L2CAP_MIN_MTU_VALUE)
		{
			p_ccb->out_mtu = p_cfg->mtu;

			if (p_ccb->out_mtu > L2CAP_MTU_SIZE)
			{
				p_cfg->mtu = L2CAP_MTU_SIZE;
				p_ccb->out_mtu = L2CAP_MTU_SIZE;
			}
		}
		else
		{
			p_cfg->mtu = L2CAP_MIN_MTU_VALUE;
			mtu_ok = FALSE;
		}
	}

	if (p_cfg->flush_to_present && p_cfg->flush_to == 0)
	{
		p_cfg->flush_to = 0xffff;
		flush_ok = FALSE;
	}

	if (p_cfg->qos_present)
	{
		if (p_cfg->qos.service_type <= GUARANTEED)
		{
			p_ccb->in_flow = p_cfg->qos;
		}
		else
		{
			p_cfg->qos.service_type = BEST_EFFORT;
			qos_ok = FALSE;
		}
	}

	rc = (mtu_ok && flush_ok && qos_ok) ? TRUE : FALSE;

	if (rc)
		return rc;

	p_cfg->result = L2CAP_CFG_UNACCEPTABLE_PARAMS;

	if (mtu_ok)
		p_cfg->mtu_present = FALSE;

	if (flush_ok)
		p_cfg->flush_to_present = FALSE;

	if (qos_ok)
		p_cfg->qos_present = FALSE;

	p_cfg->fcr_present = FALSE;

	return rc;
}

void l2cu_process_peer_cfg_rsp(tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
	if (p_cfg->qos_present)
		p_ccb->out_flow = p_cfg->qos;
}

void l2cu_process_our_cfg_req(tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
	tL2C_LCB *p_lcb;
	UINT16 hci_flush_to;
	void *p_buf;

	if (p_cfg->mtu_present)
	{
		p_ccb->in_mtu = p_cfg->mtu;

		if (p_ccb->in_mtu > L2CAP_MTU_SIZE)
		{
			p_cfg->mtu = L2CAP_MTU_SIZE;
			p_ccb->in_mtu = L2CAP_MTU_SIZE;
		}
	}

	if (p_cfg->qos_present)
		p_ccb->out_flow = p_cfg->qos;

	if (p_cfg->flush_to_present && p_cfg->flush_to != 0)
	{
		p_ccb->our_flush_to = p_cfg->flush_to;

		p_lcb = p_ccb->p_lcb;

		if (p_cfg->flush_to < p_lcb->link_flush_tout)
		{
			p_lcb->link_flush_tout = p_cfg->flush_to;

			if (p_cfg->flush_to <= HCI_MAX_AUTO_FLUSH_TOUT_VALUE)
			{
				hci_flush_to = (UINT16)(((p_cfg->flush_to << 3) + 3) / 5);

				if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
				{
					btsnd_hcic_write_auto_flush_tout(p_buf, p_lcb->handle,
					                                 hci_flush_to);
				}
			}
		}
	}
}

void l2cu_process_our_cfg_rsp(tL2C_CCB *p_ccb, tL2CAP_CFG_INFO *p_cfg)
{
	if (p_cfg->qos_present)
		p_ccb->in_flow = p_cfg->qos;
}

void l2cu_device_reset(void)
{
	int xx;
	tL2C_LCB *p_lcb = &l2cb.lcb_pool[0];

	for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_lcb++)
	{
		if (p_lcb->in_use && p_lcb->handle != 0xffff)
			l2c_link_hci_disc_comp(p_lcb->handle, 0xff);
	}
}

BOOLEAN l2cu_create_conn(tL2C_LCB *p_lcb)
{
	int xx;
	tL2C_LCB *p_lcb_cur;
	BOOLEAN is_sco_active;

	p_lcb->link_state = LST_CONNECTING;

	p_lcb_cur = &l2cb.lcb_pool[0];

	for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_lcb_cur++)
	{
		if (p_lcb_cur != p_lcb && p_lcb_cur->in_use
		    && p_lcb_cur->link_role == HCI_ROLE_SLAVE)
		{
			is_sco_active = btm_is_sco_active_by_bdaddr(p_lcb_cur->remote_bd_addr);

			L2CAP_TRACE(API, "l2cu_create_conn - btm_is_sco_active_by_bdaddr() is_sco_active = %s",
			            (is_sco_active == TRUE) ? "TRUE" : "FALSE");

			if (is_sco_active == TRUE)
				continue;

			if (btm_cb.devcb.local_features[0] & 0x20)
			{
				p_lcb->link_state = LST_CONNECTING_WAIT_SWITCH;
				p_lcb->link_role = HCI_ROLE_MASTER;

				BTM_SwitchRole(p_lcb_cur->remote_bd_addr, HCI_ROLE_MASTER,
				               NULL);

				btu_start_timer(&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK,
				                L2CAP_LINK_ROLE_SWITCH_TOUT);

				return TRUE;
			}
		}
	}

	return l2cu_create_conn_after_switch(p_lcb);
}

BOOLEAN l2cu_create_conn_after_switch(tL2C_LCB *p_lcb)
{
	UINT8 *p_features;
	UINT8 allow_switch;
	tBTM_INQ_INFO *p_inq_info;
	UINT8 page_scan_rep_mode, page_scan_mode;
	UINT16 clock_offset;

	p_features = BTM_ReadLocalFeatures();
	allow_switch = (p_features[0] & 0x20) ? 1 : 0;

	p_lcb->link_state = LST_CONNECTING;

	if ((p_inq_info = BTM_InqDbRead(p_lcb->remote_bd_addr)) != NULL)
	{
		clock_offset = p_inq_info->results.clock_offset | BTM_CLOCK_OFFSET_VALID;
		page_scan_rep_mode = p_inq_info->results.page_scan_rep_mode;
		page_scan_mode = p_inq_info->results.page_scan_mode;
	}
	else
	{
		page_scan_rep_mode = 1;
		page_scan_mode = 0;
		clock_offset = 0;
	}

	if (!btsnd_hcic_create_conn(p_lcb->remote_bd_addr,
	                            HCI_PKT_TYPES_MASK_DM1 | HCI_PKT_TYPES_MASK_DH1,
	                            page_scan_rep_mode, page_scan_mode,
	                            clock_offset, allow_switch))
	{
		L2CAP_TRACE(ERROR, "L2CAP - no buffer for l2cu_create_conn");

		l2cu_release_lcb(p_lcb);
		return FALSE;
	}

	btu_start_timer(&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK,
	                L2CAP_LINK_CONNECT_TOUT);

	return TRUE;
}

tL2C_LCB *l2cu_find_lcb_by_state(tL2C_LINK_STATE state)
{
	int i;
	tL2C_LCB *p_lcb = &l2cb.lcb_pool[0];

	for (i = 0; i < MAX_L2CAP_LINKS; i++, p_lcb++)
	{
		if (p_lcb->in_use && p_lcb->link_state == state)
			return p_lcb;
	}

	return NULL;
}

BOOLEAN l2cu_lcb_disconnecting(void)
{
	tL2C_LCB *p_lcb;
	UINT16 i;
	BOOLEAN status = FALSE;

	p_lcb = &l2cb.lcb_pool[0];

	for (i = 0; i < MAX_L2CAP_LINKS; i++, p_lcb++)
	{
		if (p_lcb->in_use)
		{
			if (!p_lcb->p_first_ccb || p_lcb->link_state == LST_DISCONNECTING)
			{
				status = TRUE;
				break;
			}
			else if (p_lcb->p_first_ccb == p_lcb->p_last_ccb)
			{
				if (p_lcb->p_first_ccb->in_use
				    && (p_lcb->p_first_ccb->chnl_state
				            == CST_W4_L2CAP_DISCONNECT_RSP
				        || p_lcb->p_first_ccb->chnl_state
				               == CST_W4_L2CA_DISCONNECT_RSP))
				{
					status = TRUE;
					break;
				}
			}
		}
	}

	return status;
}
