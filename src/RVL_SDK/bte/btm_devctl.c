#include <btm_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/btm/btm_devctl.c
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
#include <btu.h>
#include <gki.h>
#include <hcidefs.h>
#include <hcimsgs.h>
#include <l2c_int.h>
#include <sdp_api.h>

/*******************************************************************************
 * macros
 */

// HYPOTHESIS: local names for literals the headers don't spell out
#define BTM_DEV_RESET_RETRIES		5
#define BTM_DEV_STEP_RETRIES		4
#define BTM_DEV_TIMEOUT_RESET		4
#define BTM_DEV_TIMEOUT_STEP		1

#define BTM_DISCOVERY_DB_INIT()                                              \
	do                                                                       \
	{                                                                        \
		btm_cb.p_btm_ddb = (tBTM_DISCOVERY_DB *)btm_cb.btm_disc_vars.btm_db_area; \
                                                                             \
		memset(&btm_cb.btm_disc_vars, 0, sizeof(tBTM_DISCOVERY_VAR_ST));     \
                                                                             \
		btm_cb.btm_disc_vars.num_uuid_filters = 1;                           \
		btm_cb.btm_disc_vars.uuid_filters[0].len = LEN_UUID_16;              \
		btm_cb.btm_disc_vars.uuid_filters[0].uu.uuid16 = 0x1002;             \
                                                                             \
		SDP_InitDiscoveryDb(btm_cb.p_btm_ddb, BTM_DISC_DB_SIZE, 1,           \
		                    btm_cb.btm_disc_vars.uuid_filters, 0, NULL);     \
	} while (FALSE)

/*******************************************************************************
 * variables
 */

static DEV_CLASS btm_default_dev_class = {0x00, 0x1f, 0x00};

/*******************************************************************************
 * local function declarations
 */

static inline void btm_dev_send_reset(void);
static inline void btm_dev_step_buf_size(void);
static inline void btm_dev_step_local_ver(void);
static inline void btm_dev_step_features(void);

/*******************************************************************************
 * functions
 */

void btm_dev_init(void)
{
	memcpy(btm_cb.devcb.dev_class, btm_default_dev_class, DEV_CLASS_LEN);

	btm_cb.devcb.page_timeout = 0x1400;
	btm_cb.devcb.reset_timer.param = TT_DEV_RESET;
	btm_cb.devcb.rln_timer.param = TT_DEV_RLN;
	btm_cb.devcb.rlinkp_timer.param = TT_DEV_RLNKP;

	btm_cb.btm_acl_pkt_types_supported = 0xcc18;
	btm_cb.btm_sco_pkt_types_supported = 0x3f;

	btm_cb.first_disabled_channel = 0xff;
	btm_cb.last_disabled_channel = 0xff;

	btm_dev_send_reset();
}

void btm_db_reset(void)
{
	tBTM_CMPL_CB *p_cb;
	tBTM_STATUS status = BTM_DEV_RESET;

	btm_inq_db_reset();
	btm_discovery_db_reset();

	if (btm_cb.devcb.p_rln_cmpl_cb)
	{
		p_cb = btm_cb.devcb.p_rln_cmpl_cb;
		btm_cb.devcb.p_rln_cmpl_cb = NULL;

		if (p_cb)
			(*p_cb)(NULL);
	}

	if (btm_cb.devcb.p_rlinkp_cmpl_cb)
	{
		p_cb = btm_cb.devcb.p_rlinkp_cmpl_cb;
		btm_cb.devcb.p_rlinkp_cmpl_cb = NULL;

		if (p_cb)
			(*p_cb)(&status);
	}

	if (btm_cb.devcb.p_rssi_cmpl_cb)
	{
		p_cb = btm_cb.devcb.p_rssi_cmpl_cb;
		btm_cb.devcb.p_rssi_cmpl_cb = NULL;

		if (p_cb)
			(*p_cb)(&status);
	}
}

void BTM_DeviceReset(tBTM_CMPL_CB *p_cb)
{
	if (btm_cb.devcb.p_reset_cmpl_cb && btm_cb.devcb.p_reset_cmpl_cb != p_cb)
		return;

	btm_acl_device_down();
	btm_db_reset();

	btm_cb.devcb.p_reset_cmpl_cb = p_cb;

	btm_dev_send_reset();

	if (btm_cb.devcb.p_dev_status_cb)
		(*btm_cb.devcb.p_dev_status_cb)(BTM_DEV_STATUS_DOWN);
}

void BTM_SendHciReset(tBTM_CMPL_CB *p_cb)
{
	if ((btm_cb.devcb.p_reset_cmpl_cb
	     && btm_cb.devcb.p_reset_cmpl_cb != p_cb)
	    && btm_cb.devcb.p_reset_only_cmpl_cb != p_cb)
	{
		return;
	}

	btm_acl_device_down();
	btm_db_reset();

	btm_cb.devcb.p_reset_only_cmpl_cb = p_cb;

	btm_dev_send_reset();

	if (btm_cb.devcb.p_dev_status_cb)
		(*btm_cb.devcb.p_dev_status_cb)(BTM_DEV_STATUS_DOWN);
}

BOOLEAN BTM_IsDeviceUp(void)
{
	return btm_cb.devcb.state == BTM_DEV_STATE_READY;
}

tBTM_STATUS BTM_SetAfhChannels(tBTM_AFH_CHANNEL first, tBTM_AFH_CHANNEL last)
{
	BTM_TRACE(API, "BTM_SetAfhChannels first: %d (%d) last: %d (%d)", first,
	          btm_cb.first_disabled_channel, last,
	          btm_cb.last_disabled_channel);

	if (!(btm_cb.devcb.local_features[5] & 0x08)
	    && !(btm_cb.devcb.local_features[4] & 0x10)
	    && !(btm_cb.devcb.local_features[5] & 0x10))
	{
		return BTM_MODE_UNSUPPORTED;
	}

	if (btm_cb.first_disabled_channel != first
	    || btm_cb.last_disabled_channel != last)
	{
		if (btsnd_hcic_set_afh_channels(first, last))
		{
			btm_cb.first_disabled_channel = first;
			btm_cb.last_disabled_channel = last;
		}
		else
		{
			return BTM_NO_RESOURCES;
		}
	}

	return BTM_SUCCESS;
}

void btm_dev_timeout(TIMER_LIST_ENT *p_tle)
{
	if (p_tle->param == TT_DEV_RESET)
	{
		switch (btm_cb.devcb.state)
		{
		case BTM_DEV_STATE_WAIT_RESET_CMPLT:
			btm_dev_send_reset();
			break;

		case BTM_DEV_STATE_WAIT_AFTER_RESET:
			btm_cb.devcb.state = BTM_DEV_STATE_2;
			btm_cb.devcb.retry_count = BTM_DEV_RESET_RETRIES;
			btm_dev_step_buf_size();

			BTM_SetDeviceClass(btm_cb.devcb.dev_class);
			BTM_SetLocalDeviceName(btm_cb.cfg.bd_name);
			BTM_SetPinType(btm_cb.cfg.pin_type, btm_cb.cfg.pin_code,
			               btm_cb.cfg.pin_code_len);
			break;

		case BTM_DEV_STATE_2:
			btm_dev_step_buf_size();
			break;

		case BTM_DEV_STATE_3:
			btm_dev_step_local_ver();
			break;
		}
	}
	else if (p_tle->param == TT_DEV_RLN)
	{
		tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_rln_cmpl_cb;

		btm_cb.devcb.p_rln_cmpl_cb = NULL;

		if (p_cb)
			(*p_cb)(NULL);
	}
}

static inline void btm_dev_send_reset(void)
{
	btm_cb.devcb.state = BTM_DEV_STATE_WAIT_RESET_CMPLT;
	btm_cb.devcb.retry_count = BTM_DEV_RESET_RETRIES;

	btu_start_timer(&btm_cb.devcb.reset_timer, BTU_TTYPE_BTM_DEV_CTL,
	                BTM_DEV_TIMEOUT_RESET);
	btsnd_hcic_reset();
}

static inline void btm_dev_step_buf_size(void)
{
	void *p_buf;

	if (--btm_cb.devcb.retry_count == 0)
	{
		btm_dev_send_reset();
	}
	else
	{
		btu_start_timer(&btm_cb.devcb.reset_timer, BTU_TTYPE_BTM_DEV_CTL,
		                BTM_DEV_TIMEOUT_STEP);

		if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
			btsnd_hcic_read_buffer_size(p_buf);
	}
}

static inline void btm_dev_step_local_ver(void)
{
	if (--btm_cb.devcb.retry_count == 0)
	{
		btm_dev_send_reset();
	}
	else
	{
		btu_start_timer(&btm_cb.devcb.reset_timer, BTU_TTYPE_BTM_DEV_CTL,
		                BTM_DEV_TIMEOUT_STEP);
		btsnd_hcic_read_local_ver();
		btsnd_hcic_read_bd_addr();
		btm_pm_reset();
	}
}

static inline void btm_dev_step_features(void)
{
	if (--btm_cb.devcb.retry_count == 0)
	{
		btm_dev_send_reset();
	}
	else
	{
		btu_start_timer(&btm_cb.devcb.reset_timer, BTU_TTYPE_BTM_DEV_CTL,
		                BTM_DEV_TIMEOUT_STEP);
		btsnd_hcic_read_local_features();
	}
}

void btm_reset_complete(void)
{
	BTM_TRACE(EVENT, "btm_reset_complete");

	if (btm_cb.devcb.p_reset_only_cmpl_cb)
	{
		tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_reset_only_cmpl_cb;

		btm_cb.devcb.p_reset_only_cmpl_cb = NULL;

		(*p_cb)(NULL);
	}
	else if (btm_cb.devcb.state == BTM_DEV_STATE_WAIT_RESET_CMPLT)
	{
		l2cu_device_reset();

		btm_cb.btm_inq_vars.inq_scan_window = HCI_DEF_INQUIRYSCAN_WINDOW;
		btm_cb.btm_inq_vars.inq_scan_period = HCI_DEF_INQUIRYSCAN_INTERVAL;
		btm_cb.btm_inq_vars.inq_scan_type = HCI_DEF_SCAN_TYPE;
		btm_cb.btm_inq_vars.page_scan_window = HCI_DEF_PAGESCAN_WINDOW;
		btm_cb.btm_inq_vars.page_scan_period = HCI_DEF_PAGESCAN_INTERVAL;
		btm_cb.btm_inq_vars.page_scan_type = HCI_DEF_SCAN_TYPE;

		btm_cb.devcb.state = BTM_DEV_STATE_2;
		btm_cb.devcb.retry_count = BTM_DEV_RESET_RETRIES;
		btm_dev_step_buf_size();

		BTM_SetDeviceClass(btm_cb.devcb.dev_class);
		BTM_SetLocalDeviceName(btm_cb.cfg.bd_name);
		BTM_SetPinType(btm_cb.cfg.pin_type, btm_cb.cfg.pin_code,
		               btm_cb.cfg.pin_code_len);
	}
}

void btm_read_hci_buf_size_complete(UINT8 *p, UINT16 evt_len)
{
	UINT8 status;

	STREAM_TO_UINT8(p, &status);

	if (status == HCI_SUCCESS)
	{
		UINT16 num_acl_bufs;
		UINT16 acl_size;

		STREAM_TO_UINT16(p, &acl_size);
		btu_cb.hcit_acl_data_size = acl_size;
		p += 1;
		STREAM_TO_UINT16(p, &num_acl_bufs);
		btu_cb.hcit_acl_pkt_size = acl_size + 4;

		l2c_link_processs_num_bufs(num_acl_bufs);

		btsnd_hcic_set_host_buf_size(L2CAP_MTU_SIZE, 30, 20, 10);
	}

	btm_cb.devcb.state = BTM_DEV_STATE_3;
	btm_cb.devcb.retry_count = BTM_DEV_RESET_RETRIES;
	btm_dev_step_local_ver();
}
void btm_read_local_version_complete(UINT8 *p, UINT16 evt_len)
{
	tBTM_VERSION_INFO *p_ver = &btm_cb.devcb.local_version;
	UINT8 status;

	STREAM_TO_UINT8(p, &status);

	if (status == HCI_SUCCESS)
	{
		STREAM_TO_UINT8(p, &p_ver->hci_version);
		STREAM_TO_UINT16(p, &p_ver->hci_revision);
		STREAM_TO_UINT8(p, &p_ver->lmp_version);
		STREAM_TO_UINT16(p, &p_ver->manufacturer);
		STREAM_TO_UINT16(p, &p_ver->lmp_subversion);
	}

	btm_cb.devcb.state = BTM_DEV_STATE_4;
	btm_cb.devcb.retry_count = BTM_DEV_RESET_RETRIES;
	btm_dev_step_features();
}

void btm_read_local_features_complete(UINT8 *p, UINT16 evt_len)
{
	UINT8 status;
	UINT8 last;
	tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_reset_cmpl_cb;
	UINT8 first;
	UINT8 *lf = btm_cb.devcb.local_features;

	btm_cb.devcb.p_reset_cmpl_cb = NULL;

	STREAM_TO_UINT8(p, &status);

	if (status == HCI_SUCCESS)
	{
		btm_cb.devcb.state = BTM_DEV_STATE_READY;

		lf[0] = p[0];
		lf[1] = p[1];
		lf[2] = p[2];
		lf[3] = p[3];
		lf[4] = p[4];
		lf[5] = p[5];
		lf[6] = p[6];
		lf[7] = p[7];

		btm_cb.btm_acl_pkt_types_supported = HCI_PKT_TYPES_MASK_DM1
		                                   | HCI_PKT_TYPES_MASK_DH1;

		if (lf[0] & 0x01)
		{
			btm_cb.btm_acl_pkt_types_supported |= HCI_PKT_TYPES_MASK_DM3
			                                    | HCI_PKT_TYPES_MASK_DH3;
		}

		if (lf[0] & 0x02)
		{
			btm_cb.btm_acl_pkt_types_supported |= HCI_PKT_TYPES_MASK_DM5
			                                    | HCI_PKT_TYPES_MASK_DH5;
		}

		if (btm_cb.devcb.local_version.hci_version >= HCI_PROTO_VERSION_2_0)
		{
			if (!(lf[3] & 0x02))
			{
				btm_cb.btm_acl_pkt_types_supported |= 0x1102;
			}

			if (!(lf[3] & 0x04))
			{
				btm_cb.btm_acl_pkt_types_supported |= 0x2204;
			}

			if ((lf[3] & 0x02)
			    || (lf[3] & 0x04))
			{
				if (!(lf[4] & 0x80))
					btm_cb.btm_acl_pkt_types_supported |= 0x0300;

				if (!(lf[5] & 0x01))
					btm_cb.btm_acl_pkt_types_supported |= 0x3000;
			}
		}

		BTM_TRACE(DEBUG, "Local supported ACL packet types: 0x%04x",
		          btm_cb.btm_acl_pkt_types_supported);

		btm_cb.btm_sco_pkt_types_supported = 0;
		btm_cb.sco_cb.esco_supported = FALSE;

		if (lf[1] & 0x08)
		{
			btm_cb.btm_sco_pkt_types_supported = HCI_ESCO_PKT_TYPES_MASK_HV1;

			if (lf[1] & 0x10)
			{
				btm_cb.btm_sco_pkt_types_supported |=
					HCI_ESCO_PKT_TYPES_MASK_HV2;
			}

			if (lf[1] & 0x20)
			{
				btm_cb.btm_sco_pkt_types_supported |=
					HCI_ESCO_PKT_TYPES_MASK_HV3;
			}
		}

		if (lf[3] & 0x80)
			btm_cb.btm_sco_pkt_types_supported |= HCI_ESCO_PKT_TYPES_MASK_EV3;

		if (lf[4] & 0x01)
			btm_cb.btm_sco_pkt_types_supported |= HCI_ESCO_PKT_TYPES_MASK_EV4;

		if (lf[4] & 0x02)
			btm_cb.btm_sco_pkt_types_supported |= HCI_ESCO_PKT_TYPES_MASK_EV5;

		if (btm_cb.btm_sco_pkt_types_supported & BTM_ESCO_LINK_ONLY_MASK)
		{
			btm_cb.sco_cb.esco_supported = TRUE;

			if (lf[5] & 0x20)
			{
				if (!(lf[5] & 0x80))
				{
					btm_cb.btm_sco_pkt_types_supported |=
						HCI_ESCO_PKT_TYPES_MASK_NO_2_EV5;
				}
			}
			else
			{
				btm_cb.btm_sco_pkt_types_supported |=
					HCI_ESCO_PKT_TYPES_MASK_NO_2_EV3
					| HCI_ESCO_PKT_TYPES_MASK_NO_2_EV5;
			}

			if (lf[5] & 0x40)
			{
				if (!(lf[5] & 0x80))
				{
					btm_cb.btm_sco_pkt_types_supported |=
						HCI_ESCO_PKT_TYPES_MASK_NO_3_EV5;
				}
			}
			else
			{
				btm_cb.btm_sco_pkt_types_supported |=
					HCI_ESCO_PKT_TYPES_MASK_NO_3_EV3
					| HCI_ESCO_PKT_TYPES_MASK_NO_3_EV5;
			}
		}

		BTM_TRACE(DEBUG, "Local supported SCO packet types: 0x%04x",
		          btm_cb.btm_sco_pkt_types_supported);

		if (lf[0] & 0x20)
			btm_cb.btm_def_link_policy |= HCI_ENABLE_MASTER_SLAVE_SWITCH;
		else
			btm_cb.btm_def_link_policy &= ~HCI_ENABLE_MASTER_SLAVE_SWITCH;

		if (lf[0] & 0x40)
			btm_cb.btm_def_link_policy |= HCI_ENABLE_HOLD_MODE;
		else
			btm_cb.btm_def_link_policy &= ~HCI_ENABLE_HOLD_MODE;

		if (lf[0] & 0x80)
			btm_cb.btm_def_link_policy |= HCI_ENABLE_SNIFF_MODE;
		else
			btm_cb.btm_def_link_policy &= ~HCI_ENABLE_SNIFF_MODE;

		if (lf[1] & 0x01)
			btm_cb.btm_def_link_policy |= HCI_ENABLE_PARK_MODE;
		else
			btm_cb.btm_def_link_policy &= ~HCI_ENABLE_PARK_MODE;

		btm_sec_dev_reset();

		last = btm_cb.last_disabled_channel;
		if (last != 0xff)
		{
			first = btm_cb.first_disabled_channel;
			btm_cb.last_disabled_channel = 0xff;
			btm_cb.first_disabled_channel = 0xff;

			BTM_SetAfhChannels(first, last);
		}

		if (lf[3] & 0x40)
			BTM_SetInquiryMode(BTM_INQ_RESULT_WITH_RSSI);

		BTM_SetPageScanType(BTM_SCAN_TYPE_INTERLACED);
		BTM_SetInquiryScanType(BTM_SCAN_TYPE_INTERLACED);

		btm_report_device_status(BTM_DEV_STATUS_UP);

		if (p_cb)
			(*p_cb)(NULL);
	}
}

tBTM_STATUS BTM_SetLocalDeviceName(char *p_name)
{
	void *p_buf;
	UINT8 *p_dst;

	if (!p_name)
		return BTM_ILLEGAL_VALUE;

	if (btm_cb.devcb.state == BTM_DEV_STATE_WAIT_RESET_CMPLT
	    || btm_cb.devcb.state == BTM_DEV_STATE_WAIT_AFTER_RESET)
		return BTM_DEV_RESET;

	p_dst = (UINT8 *)btm_cb.cfg.bd_name;

	if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
	{
		if (p_dst != (UINT8 *)p_name)
		{
			memset(p_dst, 0, sizeof(tBTM_LOC_BD_NAME));
			strncpy((char *)p_dst, p_name, BTM_MAX_LOC_BD_NAME_LEN);
		}

		btsnd_hcic_change_name(p_buf, p_dst);

		return BTM_CMD_STARTED;
	}

	return BTM_NO_RESOURCES;
}

void btm_read_local_name_complete(UINT8 *p, UINT16 evt_len)
{
	tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_rln_cmpl_cb;
	UINT8 status;

	btu_stop_timer(&btm_cb.devcb.rln_timer);

	btm_cb.devcb.p_rln_cmpl_cb = NULL;

	if (p_cb)
	{
		STREAM_TO_UINT8(p, &status);

		if (status == HCI_SUCCESS)
			(*p_cb)(p);
		else
			(*p_cb)(NULL);
	}
}

tBTM_STATUS BTM_ReadLocalDeviceAddr(tBTM_CMPL_CB *p_cb)
{
	if (p_cb)
		(*p_cb)(btm_cb.devcb.local_addr);

	return BTM_SUCCESS;
}

void btm_read_local_addr_complete(UINT8 *p, UINT16 evt_len)
{
	UINT8 status;

	STREAM_TO_UINT8(p, &status);

	if (status == HCI_SUCCESS)
		STREAM_TO_BDADDR(p, btm_cb.devcb.local_addr);
}

tBTM_STATUS BTM_ReadLocalVersion(tBTM_VERSION_INFO *p_vers)
{
	if (btm_cb.devcb.state < BTM_DEV_STATE_4)
		return BTM_DEV_RESET;

	*p_vers = btm_cb.devcb.local_version;

	return BTM_SUCCESS;
}

tBTM_STATUS BTM_SetDeviceClass(DEV_CLASS dev_class)
{
	void *p_buf;

	memcpy(btm_cb.devcb.dev_class, dev_class, DEV_CLASS_LEN);

	if (btm_cb.devcb.state == BTM_DEV_STATE_WAIT_RESET_CMPLT
	    || btm_cb.devcb.state == BTM_DEV_STATE_WAIT_AFTER_RESET)
		return BTM_DEV_RESET;

	if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
	{
		btsnd_hcic_write_dev_class(p_buf, dev_class);
		return BTM_SUCCESS;
	}

	return BTM_NO_RESOURCES;
}

UINT8 *BTM_ReadDeviceClass(void)
{
	return btm_cb.devcb.dev_class;
}

UINT8 *BTM_ReadLocalFeatures(void)
{
	return btm_cb.devcb.local_features;
}

tBTM_DEV_STATUS_CB *BTM_RegisterForDeviceStatusNotif(tBTM_DEV_STATUS_CB *p_cb)
{
	tBTM_DEV_STATUS_CB *p_prev = btm_cb.devcb.p_dev_status_cb;

	btm_cb.devcb.p_dev_status_cb = p_cb;

	return p_prev;
}

tBTM_STATUS BTM_VendorSpecificCommand(UINT16 opcode, UINT8 param_len,
                                      UINT8 *p_param_buf, tBTM_CMPL_CB *p_cb)
{
	void *p_buf;

	BTM_TRACE(EVENT, "BTM: BTM_VendorSpecificCommand: Opcode: 0x%04X, ParamLen: %i.",
	          opcode, param_len);

	if (btm_cb.devcb.vsc_busy)
	{
		BTM_TRACE(EVENT, "BTM: Unable to send vendor specific command (controller is busy).");
		return BTM_BUSY;
	}

	if ((p_buf = GKI_getbuf((UINT16)(sizeof(BT_HDR) + 3 + param_len))) != NULL)
	{
		btm_cb.devcb.p_vsc_cmpl_cb = p_cb;

		if (p_cb)
			btm_cb.devcb.vsc_busy = TRUE;

		btsnd_hcic_vendor_spec_cmd(p_buf, opcode, param_len, p_param_buf);

		if (p_cb)
			return BTM_CMD_STARTED;
		else
			return BTM_SUCCESS;
	}

	return BTM_NO_RESOURCES;
}

void btm_vsc_complete(UINT8 *p, UINT16 opcode, UINT16 evt_len)
{
	tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_vsc_cmpl_cb;
	tBTM_VSC_CMPL vcs_cplt_params;
	UINT8 *p_src = p;
	UINT8 *p_dst = vcs_cplt_params.param_buf;

	btm_cb.devcb.vsc_busy = FALSE;
	btm_cb.devcb.p_vsc_cmpl_cb = NULL;

	if (p_cb)
	{
		vcs_cplt_params.opcode = opcode;
		vcs_cplt_params.param_len = evt_len;
		memcpy(p_dst, p_src, evt_len);

		(*p_cb)(&vcs_cplt_params);
	}
}

tBTM_STATUS BTM_RegisterForVSEvents(tBTM_VS_EVT_CB *p_cb)
{
	if (btm_cb.devcb.p_vend_spec_cb && p_cb)
		return BTM_BUSY;

	btm_cb.devcb.p_vend_spec_cb = p_cb;

	return BTM_SUCCESS;
}

void btm_vendor_specific_evt(UINT8 *p, UINT8 evt_len)
{
	tBTM_CB *p_cb = &btm_cb;

	if (p_cb->devcb.local_version.manufacturer != 0x12)
	{
		BTM_TRACE(EVENT, "BTM Event: Received a vendor specific event from controller");
	}

	p_cb->devcb.p_vsc_cmpl_cb = NULL;
	p_cb->devcb.vsc_busy = FALSE;

	if (p_cb->devcb.p_vend_spec_cb)
		(*p_cb->devcb.p_vend_spec_cb)(evt_len, p);
}

tBTM_STATUS BTM_WritePageTimeout(UINT16 timeout)
{
	void *p_buf;

	BTM_TRACE(EVENT, "BTM: BTM_WritePageTimeout: Timeout: %d.", timeout);

	btm_cb.devcb.page_timeout = (timeout * 625) / 1000;

	if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
	{
		btsnd_hcic_write_page_tout(p_buf, timeout);
		return BTM_SUCCESS;
	}

	return BTM_NO_RESOURCES;
}

// HYPOTHESIS: the next two are stripped, only their strings survive.  bodies
// are guesses.
tBTM_STATUS BTM_WriteVoiceSettings(UINT16 settings)
{
	BTM_TRACE(EVENT, "BTM: BTM_WriteVoiceSettings: Settings: 0x%04x.", settings);

	return BTM_SUCCESS;
}

tBTM_STATUS BTM_EnableTestMode(void)
{
	BTM_TRACE(EVENT, "BTM: BTM_EnableTestMode");

	return BTM_SUCCESS;
}

tBTM_STATUS BTM_ReadStoredLinkKey(BD_ADDR bd_addr, tBTM_CMPL_CB *p_cb)
{
	void *p_buf;
	BOOLEAN read_all_flag = FALSE;
	BD_ADDR local_bd_addr;

	if (btm_cb.devcb.p_stored_link_key_cmpl_cb)
		return BTM_BUSY;

	if (!bd_addr)
	{
		read_all_flag = TRUE;
		bd_addr = local_bd_addr;
	}

	BTM_TRACE(EVENT, "BTM: BTM_ReadStoredLinkKey: Read_All: %s",
	          read_all_flag ? "TRUE" : "FALSE");

	if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
	{
		btm_cb.devcb.p_stored_link_key_cmpl_cb = p_cb;

		btsnd_hcic_read_stored_key(p_buf, bd_addr, read_all_flag);

		return BTM_SUCCESS;
	}

	return BTM_NO_RESOURCES;
}

tBTM_STATUS BTM_WriteStoredLinkKey(UINT8 num_keys, BD_ADDR *bd_addr,
                                   LINK_KEY *link_key, tBTM_CMPL_CB *p_cb)
{
	void *p_buf;

	if (btm_cb.devcb.p_stored_link_key_cmpl_cb)
		return BTM_BUSY;

	BTM_TRACE(EVENT, "BTM: BTM_WriteStoredLinkKey: num_keys: %d", num_keys);

	if (num_keys > HCI_MAX_NUM_OF_LINK_KEYS_PER_CMD)
		num_keys = HCI_MAX_NUM_OF_LINK_KEYS_PER_CMD;

	if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
	{
		btm_cb.devcb.p_stored_link_key_cmpl_cb = p_cb;

		btsnd_hcic_write_stored_key(p_buf, num_keys, bd_addr, link_key);

		return BTM_SUCCESS;
	}

	return BTM_NO_RESOURCES;
}

tBTM_STATUS BTM_DeleteStoredLinkKey(BD_ADDR bd_addr, tBTM_CMPL_CB *p_cb)
{
	BD_ADDR local_bd_addr;
	BOOLEAN delete_all_flag = FALSE;

	if (btm_cb.devcb.p_stored_link_key_cmpl_cb)
		return BTM_BUSY;

	if (!bd_addr)
	{
		delete_all_flag = TRUE;
		bd_addr = local_bd_addr;
	}

	BTM_TRACE(EVENT, "BTM: BTM_DeleteStoredLinkKey: delete_all_flag: %s",
	          delete_all_flag ? "TRUE" : "FALSE");

	btm_cb.devcb.p_stored_link_key_cmpl_cb = p_cb;

	if (!btsnd_hcic_delete_stored_key(bd_addr, delete_all_flag))
		return BTM_NO_RESOURCES;

	return BTM_SUCCESS;
}

void btm_read_stored_link_key_complete(UINT8 *p)
{
	tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_stored_link_key_cmpl_cb;
	tBTM_READ_STORED_LINK_KEY_COMPLETE result;

	btm_cb.devcb.p_stored_link_key_cmpl_cb = NULL;

	if (p_cb)
	{
		result.event = BTM_CB_EVT_READ_STORED_LINK_KEYS;

		STREAM_TO_UINT8(p, &result.status);
		STREAM_TO_UINT16(p, &result.max_keys);
		STREAM_TO_UINT16(p, &result.read_keys);

		(*p_cb)(&result);
	}
}

void btm_write_stored_link_key_complete(UINT8 *p)
{
	tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_stored_link_key_cmpl_cb;
	tBTM_WRITE_STORED_LINK_KEY_COMPLETE result;

	btm_cb.devcb.p_stored_link_key_cmpl_cb = NULL;

	if (p_cb)
	{
		result.event = BTM_CB_EVT_WRITE_STORED_LINK_KEYS;

		STREAM_TO_UINT8(p, &result.status);
		STREAM_TO_UINT8(p, &result.num_keys);

		(*p_cb)(&result);
	}
}

void btm_delete_stored_link_key_complete(UINT8 *p)
{
	tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_stored_link_key_cmpl_cb;
	tBTM_DELETE_STORED_LINK_KEY_COMPLETE result;

	btm_cb.devcb.p_stored_link_key_cmpl_cb = NULL;

	if (p_cb)
	{
		result.event = BTM_CB_EVT_DELETE_STORED_LINK_KEYS;

		STREAM_TO_UINT8(p, &result.status);
		STREAM_TO_UINT16(p, &result.num_keys);

		(*p_cb)(&result);
	}
}

void btm_return_link_keys_evt(tBTM_RETURN_LINK_KEYS_EVT *result)
{
	UINT8 i;
	UINT8 *p;
	BD_ADDR bda;
	LINK_KEY key;
	tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_stored_link_key_cmpl_cb;

	if (p_cb)
	{
		p = (UINT8 *)(result + 1);

		for (i = 0; i < result->num_keys; i++)
		{
			STREAM_TO_BDADDR(p, bda);
			STREAM_TO_ARRAY16(p, key);

			p -= BD_ADDR_LEN + 16;
			ARRAY_TO_STREAM(p, bda, BD_ADDR_LEN);
			ARRAY_TO_STREAM(p, key, 16);
		}

		(*p_cb)(result);
	}
}

void btm_report_device_status(tBTM_DEV_STATUS status)
{
	if (btm_cb.devcb.p_dev_status_cb)
		(*btm_cb.devcb.p_dev_status_cb)(status);
}

void btm_sdp_db_init(void)
{
	BTM_DISCOVERY_DB_INIT();
}

#pragma dont_inline on
void btm_discovery_db_reset(void)
{
	btu_stop_timer(&btm_cb.btm_disc_vars.disc_timer_ent);

	if (btm_cb.btm_disc_vars.disc_active)
	{
		tBTM_CMPL_CB *p_cb = btm_cb.btm_disc_vars.p_disc_cmpl_cb;

		btm_cb.btm_disc_vars.disc_active = FALSE;
		btm_cb.btm_disc_vars.p_disc_cmpl_cb = NULL;

		if (p_cb)
			(*p_cb)(NULL);
	}

	BTM_DISCOVERY_DB_INIT();
}
#pragma dont_inline reset
