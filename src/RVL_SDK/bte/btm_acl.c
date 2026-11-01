#include <btm_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/btm/btm_acl.c
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

/*******************************************************************************
 * macros
 */

// HYPOTHESIS: local names for things the headers don't spell out
#define BTM_ACL_REPLY_TIMEOUT	3

/*******************************************************************************
 * local function declarations
 */

static tBTM_STATUS btm_set_packet_types_inline(tACL_CONN *p, UINT16 pkt_types);

/*******************************************************************************
 * functions
 */

void btm_acl_init(void)
{
	btm_cb.btm_def_link_super_tout = HCI_DEFAULT_INACT_TOUT;
	btm_cb.acl_disc_reason = 0xff;
}

UINT8 btm_handle_to_acl_index(UINT16 hci_handle)
{
	tACL_CONN *p = &btm_cb.acl_db[0];
	UINT8 xx;

	for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p++)
	{
		if (p->in_use && p->hci_handle == hci_handle)
			break;
	}

	return xx;
}

tACL_CONN *btm_bda_to_acl(BD_ADDR bda)
{
	tACL_CONN *p = &btm_cb.acl_db[0];
	UINT8 xx;

	for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p++)
	{
		if (p->in_use && !memcmp(p->remote_addr, bda, BD_ADDR_LEN))
			return p;
	}

	return NULL;
}

void btm_acl_created(BD_ADDR bda, DEV_CLASS dc, BD_NAME bdn, UINT16 hci_handle,
                     UINT8 link_role)
{
	tBTM_SEC_DEV_REC *p_dev_rec = NULL;
	tACL_CONN *p;
	UINT8 xx;
	UINT8 yy;

	if ((p = btm_bda_to_acl(bda)) != NULL)
	{
		p->hci_handle = hci_handle;
		p->link_role = link_role;

		BTM_TRACE(EVENT, "Duplicate btm_acl_created: RemBdAddr: %02x%02x%02x%02x%02x%02x",
		          bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
		return;
	}

	for (xx = 0, p = &btm_cb.acl_db[0]; xx < MAX_L2CAP_LINKS; xx++, p++)
	{
		if (!p->in_use)
		{
			p->in_use = TRUE;
			p->hci_handle = hci_handle;
			p->link_role = link_role;
			p->restore_pkt_types = 0;

			btm_pm_sm_alloc(xx);

			memcpy(p->remote_addr, bda, BD_ADDR_LEN);

			if (dc)
				memcpy(p->remote_dc, dc, DEV_CLASS_LEN);

			if (bdn)
				memcpy(p->remote_name, bdn, sizeof(BD_NAME));

			btsnd_hcic_read_rmt_clk_offset(p->hci_handle);
			btsnd_hcic_rmt_ver_req(p->hci_handle);

			p_dev_rec = btm_find_dev_by_handle(hci_handle);

			if (p_dev_rec)
			{
				for (yy = 0; yy < BD_FEATURES_LEN; yy++)
				{
					if (p_dev_rec->features[yy] != 0)
					{
						memcpy(p->features, p_dev_rec->features,
						       BD_FEATURES_LEN);

						btm_set_packet_types_inline(
							p, btm_cb.btm_acl_pkt_types_supported);

						if (btm_cb.btm_def_link_policy)
						{
							BTM_SetLinkPolicy(p->remote_addr,
							                  &btm_cb.btm_def_link_policy);
						}

						BTM_SetLinkSuperTout(p->remote_addr,
						                     btm_cb.btm_def_link_super_tout);

						if (btm_cb.p_acl_changed_cb)
						{
							(*btm_cb.p_acl_changed_cb)(p->remote_addr,
							                           p->remote_dc,
							                           p->remote_name,
							                           p->features, TRUE);
						}

						return;
					}
				}
			}

			btsnd_hcic_rmt_features_req(p->hci_handle);
			return;
		}
	}
}

void btm_acl_removed(BD_ADDR bda)
{
	tACL_CONN *p;

	p = btm_bda_to_acl(bda);
	if (p != NULL)
	{
		p->in_use = FALSE;

		if (btm_cb.p_acl_changed_cb)
			(*btm_cb.p_acl_changed_cb)(bda, NULL, NULL, NULL, FALSE);
	}
}

void btm_acl_device_down(void)
{
	tACL_CONN *p = &btm_cb.acl_db[0];
	UINT16 xx;

	for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p++)
	{
		if (p->in_use)
			l2c_link_hci_disc_comp(p->hci_handle, HCI_ERR_HW_FAILURE);
	}
}

static tBTM_STATUS btm_set_packet_types_inline(tACL_CONN *p, UINT16 pkt_types)
{
	UINT16 temp_pkt_types;

	UINT16 masked;

	masked = pkt_types & BTM_ACL_SUPPORTED_PKTS_MASK
	       & btm_cb.btm_acl_pkt_types_supported;

	if (btm_cb.devcb.local_version.hci_version >= HCI_PROTO_VERSION_2_0)
	{
		temp_pkt_types = masked
		               | ((pkt_types | btm_cb.btm_acl_pkt_types_supported)
		                  & BTM_ACL_EXCEPTION_PKTS_MASK);
	}
	else
	{
		temp_pkt_types = masked & ~BTM_ACL_EXCEPTION_PKTS_MASK;
	}

	BTM_TRACE(EVENT, "SetPacketType Mask -> 0x%04x", temp_pkt_types);

	if (btsnd_hcic_change_conn_type(p->hci_handle, temp_pkt_types))
	{
		p->pkt_types_mask = temp_pkt_types;
		return BTM_CMD_STARTED;
	}

	return BTM_NO_RESOURCES;
}

tBTM_STATUS BTM_SwitchRole(BD_ADDR remote_bd_addr, UINT8 new_role,
                           tBTM_CMPL_CB *p_cb)
{
	tACL_CONN *p;
	tBTM_SEC_DEV_REC *p_dev_rec;
	tBTM_PM_MODE pwr_mode;
	tBTM_PM_PWR_MD pm;
	tBTM_STATUS status;

	if (!(btm_cb.devcb.local_features[0] & 0x20))
		return BTM_MODE_UNSUPPORTED;

	p = btm_bda_to_acl(remote_bd_addr);
	if (p == NULL)
		return BTM_UNKNOWN_ADDR;

	if (p->link_role == new_role)
		return BTM_SUCCESS;

	if (btm_is_sco_active_by_bdaddr(remote_bd_addr) == TRUE)
		return BTM_NO_RESOURCES;

	if (p->switch_role_state != BTM_ACL_SWKEY_STATE_IDLE)
	{
		BTM_TRACE(DEBUG, "Role change request declined since the previous request for this device is not completed ");
		return BTM_BUSY;
	}

	memset(&btm_cb.devcb.switch_role_ref_data, 0,
	       sizeof(tBTM_ROLE_SWITCH_CMPL));

	if ((status = BTM_ReadPowerMode(p->remote_addr, &pwr_mode)) != BTM_SUCCESS)
		return status;

	if (pwr_mode == BTM_PM_MD_PARK || pwr_mode == BTM_PM_MD_SNIFF)
	{
		pm.mode = BTM_PM_MD_ACTIVE;

		if (BTM_SetPowerMode(BTM_PM_SET_ONLY_ID, p->remote_addr, &pm)
		    != BTM_CMD_STARTED)
		{
			return BTM_WRONG_MODE;
		}

		p->switch_role_state = BTM_ACL_SWKEY_STATE_MODE_CHANGE;
	}
	else if ((p_dev_rec = btm_find_dev(remote_bd_addr)) != NULL
	         && (p_dev_rec->sec_flags & BTM_SEC_ENCRYPTED))
	{
		if (!btsnd_hcic_set_conn_encrypt(p->hci_handle, FALSE))
			return BTM_NO_RESOURCES;

		p->switch_role_state = BTM_ACL_SWKEY_STATE_ENCRYPTION_OFF;
	}
	else
	{
		if (!btsnd_hcic_switch_role(remote_bd_addr, new_role))
			return BTM_NO_RESOURCES;

		p->switch_role_state = BTM_ACL_SWKEY_STATE_IN_PROGRESS;
	}

	memcpy(btm_cb.devcb.switch_role_ref_data.remote_bd_addr, remote_bd_addr,
	       BD_ADDR_LEN);
	btm_cb.devcb.switch_role_ref_data.role = new_role;
	btm_cb.devcb.p_switch_role_cb = p_cb;

	return BTM_CMD_STARTED;
}

void btm_acl_encrypt_change(UINT16 handle, UINT8 status, UINT8 encr_enable)
{
	tACL_CONN *p;
	UINT8 xx;

	xx = btm_handle_to_acl_index(handle);

	if (xx < MAX_L2CAP_LINKS)
		p = &btm_cb.acl_db[xx];
	else
		return;

	{
		if (p->switch_role_state == BTM_ACL_SWKEY_STATE_ENCRYPTION_OFF)
		{
			if (encr_enable)
				p->switch_role_state = BTM_ACL_SWKEY_STATE_IDLE;
			else
				p->switch_role_state = BTM_ACL_SWKEY_STATE_SWITCHING;

			if (!btsnd_hcic_switch_role(p->remote_addr, (UINT8)(!p->link_role)))
			{
				p->switch_role_state = BTM_ACL_SWKEY_STATE_IDLE;

				if (btm_cb.devcb.p_switch_role_cb)
				{
					(*btm_cb.devcb.p_switch_role_cb)(
						&btm_cb.devcb.switch_role_ref_data);
					btm_cb.devcb.p_switch_role_cb = NULL;
				}
			}
		}
		else if (p->switch_role_state == BTM_ACL_SWKEY_STATE_ENCRYPTION_ON)
		{
			p->switch_role_state = BTM_ACL_SWKEY_STATE_IDLE;

			if (btm_cb.devcb.p_switch_role_cb)
			{
				(*btm_cb.devcb.p_switch_role_cb)(
					&btm_cb.devcb.switch_role_ref_data);
				btm_cb.devcb.p_switch_role_cb = NULL;
			}
		}
	}
}

tBTM_STATUS BTM_SetLinkPolicy(BD_ADDR remote_bda, UINT16 *settings)
{
	tACL_CONN *p;
	UINT8 *localFeatures = BTM_ReadLocalFeatures();

	if (*settings != HCI_DISABLE_ALL_LM_MODES)
	{
		if ((*settings & HCI_ENABLE_MASTER_SLAVE_SWITCH)
		    && !(localFeatures[0] & 0x20))
		{
			*settings &= ~HCI_ENABLE_MASTER_SLAVE_SWITCH;

			BTM_TRACE(API, "BTM_SetLinkPolicy switch not supported (settings: 0x%04x)",
			          *settings);
		}

		if ((*settings & HCI_ENABLE_HOLD_MODE) && !(localFeatures[0] & 0x40))
		{
			*settings &= ~HCI_ENABLE_HOLD_MODE;

			BTM_TRACE(API, "BTM_SetLinkPolicy hold not supported (settings: 0x%04x)",
			          *settings);
		}

		if ((*settings & HCI_ENABLE_SNIFF_MODE) && !(localFeatures[0] & 0x80))
		{
			*settings &= ~HCI_ENABLE_SNIFF_MODE;

			BTM_TRACE(API, "BTM_SetLinkPolicy sniff not supported (settings: 0x%04x)",
			          *settings);
		}

		if ((*settings & HCI_ENABLE_PARK_MODE) && !(localFeatures[1] & 0x01))
		{
			*settings &= ~HCI_ENABLE_PARK_MODE;

			BTM_TRACE(API, "BTM_SetLinkPolicy park not supported (settings: 0x%04x)",
			          *settings);
		}
	}

	if ((p = btm_bda_to_acl(remote_bda)) != NULL)
	{
		return btsnd_hcic_write_policy_set(p->hci_handle, *settings)
		           ? BTM_CMD_STARTED
		           : BTM_NO_RESOURCES;
	}

	return BTM_UNKNOWN_ADDR;
}

// HYPOTHESIS: BTM_ReadLinkPolicy is stripped from the final dol, so only its
// string survives.  body is a guess.
tBTM_STATUS BTM_ReadLinkPolicy(BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb)
{
	tACL_CONN *p;

	BTM_TRACE(API, "BTM_ReadLinkPolicy: RemBdAddr: %02x%02x%02x%02x%02x%02x",
	          remote_bda[0], remote_bda[1], remote_bda[2], remote_bda[3],
	          remote_bda[4], remote_bda[5]);

	if (btm_cb.devcb.p_rlinkp_cmpl_cb)
		return BTM_BUSY;

	if ((p = btm_bda_to_acl(remote_bda)) != NULL)
	{
		btu_start_timer(&btm_cb.devcb.rlinkp_timer, BTU_TTYPE_BTM_ACL,
		                BTM_ACL_REPLY_TIMEOUT);
		btm_cb.devcb.p_rlinkp_cmpl_cb = p_cb;

		return BTM_CMD_STARTED;
	}

	return BTM_UNKNOWN_ADDR;
}

void BTM_SetDefaultLinkPolicy(UINT16 settings)
{
	btm_cb.btm_def_link_policy = settings;
}

void btm_read_link_policy_complete(UINT8 *p)
{
	tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_rlinkp_cmpl_cb;
	tBTM_LINK_POLICY_RESULTS lnkpol;
	tACL_CONN *p_acl = &btm_cb.acl_db[0];
	UINT16 handle;
	UINT8 xx;

	btu_stop_timer(&btm_cb.devcb.rlinkp_timer);

	btm_cb.devcb.p_rlinkp_cmpl_cb = NULL;

	if (p_cb)
	{
		STREAM_TO_UINT8(p, &lnkpol.hci_status);

		if (lnkpol.hci_status == HCI_SUCCESS)
		{
			lnkpol.status = BTM_SUCCESS;

			STREAM_TO_UINT16(p, &handle);
			STREAM_TO_UINT16(p, &lnkpol.settings);

			for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_acl++)
			{
				if (p_acl->in_use && handle == p_acl->hci_handle)
				{
					memcpy(lnkpol.rem_bda, p_acl->remote_addr, BD_ADDR_LEN);
					break;
				}
			}
		}
		else
		{
			lnkpol.status = BTM_ERR_PROCESSING;
		}

		(*p_cb)(&lnkpol);
	}
}

void btm_read_remote_version_complete(UINT8 *p)
{
	tACL_CONN *p_acl_cb = &btm_cb.acl_db[0];
	UINT8 status;
	UINT16 handle;
	int xx;

	STREAM_TO_UINT8(p, &status);

	if (status == HCI_SUCCESS)
	{
		STREAM_TO_UINT16(p, &handle);

		for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_acl_cb++)
		{
			if (p_acl_cb->in_use && p_acl_cb->hci_handle == handle)
			{
				STREAM_TO_UINT8(p, &p_acl_cb->lmp_version);
				STREAM_TO_UINT16(p, &p_acl_cb->manufacturer);
				STREAM_TO_UINT16(p, &p_acl_cb->lmp_subversion);
				break;
			}
		}
	}
}

void btm_read_remote_features_complete(UINT8 *p)
{
	tACL_CONN *p_acl_cb = &btm_cb.acl_db[0];
	UINT8 status;
	UINT16 handle;
	int xx;
	int yy;
	tBTM_SEC_DEV_REC *p_dev_rec;

	STREAM_TO_UINT8(p, &status);

	if (status == HCI_SUCCESS)
	{
		STREAM_TO_UINT16(p, &handle);

		for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_acl_cb++)
		{
			if (p_acl_cb->in_use && p_acl_cb->hci_handle == handle)
			{
				for (yy = 0; yy < BD_FEATURES_LEN; yy++)
					p_acl_cb->features[yy] = *p++;

				if ((p_dev_rec = btm_find_dev_by_handle(handle)) != NULL)
				{
					memcpy(p_dev_rec->features, p_acl_cb->features,
					       BD_FEATURES_LEN);
				}

				btm_set_packet_types_inline(
					p_acl_cb, btm_cb.btm_acl_pkt_types_supported);

				if (btm_cb.btm_def_link_policy)
				{
					BTM_SetLinkPolicy(p_acl_cb->remote_addr,
					                  &btm_cb.btm_def_link_policy);
				}

				BTM_SetLinkSuperTout(p_acl_cb->remote_addr,
				                     btm_cb.btm_def_link_super_tout);

				if (btm_cb.p_acl_changed_cb)
				{
					(*btm_cb.p_acl_changed_cb)(
						p_acl_cb->remote_addr, p_acl_cb->remote_dc,
						p_acl_cb->remote_name, p_acl_cb->features, TRUE);
				}

				break;
			}
		}
	}
}

void BTM_SetDefaultLinkSuperTout(UINT16 timeout)
{
	btm_cb.btm_def_link_super_tout = timeout;
}

BOOLEAN BTM_IsAclConnectionUp(BD_ADDR remote_bda)
{
	tACL_CONN *p;

	BTM_TRACE(API, "BTM_ReadClockOffset: RemBdAddr: %02x%02x%02x%02x%02x%02x",
	          remote_bda[0], remote_bda[1], remote_bda[2], remote_bda[3],
	          remote_bda[4], remote_bda[5]);

	p = btm_bda_to_acl(remote_bda);
	if (p != NULL)
		return TRUE;

	return FALSE;
}

UINT16 BTM_GetNumAclLinks(void)
{
	tACL_CONN *p = &btm_cb.acl_db[0];
	UINT16 num_acl = 0;
	UINT8 xx;

	for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p++)
	{
		if (p->in_use)
			num_acl++;
	}

	return num_acl;
}

UINT16 btm_get_acl_disc_reason_code(void)
{
	return btm_cb.acl_disc_reason;
}

UINT16 BTM_GetHCIConnHandle(BD_ADDR remote_bda)
{
	tACL_CONN *p;

	p = btm_bda_to_acl(remote_bda);
	if (p != NULL)
		return p->hci_handle;

	return 0xffff;
}

void btm_process_clk_off_comp_evt(UINT16 hci_handle, UINT16 clock_offset)
{
	UINT8 xx;

	if ((xx = btm_handle_to_acl_index(hci_handle)) < MAX_L2CAP_LINKS)
		btm_cb.acl_db[xx].clock_offset = clock_offset;
}

void btm_acl_role_changed(UINT8 hci_status, BD_ADDR bd_addr, UINT8 new_role)
{
	UINT8 *p_bda = (bd_addr) ? bd_addr : btm_cb.connecting_bda;
	tACL_CONN *p = btm_bda_to_acl(p_bda);
	tBTM_ROLE_SWITCH_CMPL *p_data = &btm_cb.devcb.switch_role_ref_data;

	if (p)
	{
		p_data->hci_status = hci_status;

		if (hci_status == HCI_SUCCESS)
		{
			p_data->role = new_role;
			memcpy(p_data->remote_bd_addr, p_bda, BD_ADDR_LEN);

			p->link_role = new_role;
		}

		if (bd_addr)
			BTM_SetLinkSuperTout(p->remote_addr, p->link_super_tout);

		if (p->switch_role_state == BTM_ACL_SWKEY_STATE_SWITCHING)
		{
			if (btsnd_hcic_set_conn_encrypt(p->hci_handle, TRUE))
			{
				p->switch_role_state = BTM_ACL_SWKEY_STATE_ENCRYPTION_ON;
				return;
			}
		}

		if (p && p->switch_role_state == BTM_ACL_SWKEY_STATE_IN_PROGRESS)
			p->switch_role_state = BTM_ACL_SWKEY_STATE_IDLE;

		if (btm_cb.devcb.p_switch_role_cb)
		{
			(*btm_cb.devcb.p_switch_role_cb)(p_data);
			btm_cb.devcb.p_switch_role_cb = NULL;
		}

		BTM_TRACE(EVENT, "Role Switch Event: new_role 0x%02x, HCI Status 0x%02x",
		          p_data->role, p_data->hci_status);
	}
}

// HYPOTHESIS: BTM_SetQoS is stripped from the final dol, so only its string
// survives.  body is a guess.
tBTM_STATUS BTM_SetQoS(BD_ADDR bd, FLOW_SPEC *p_flow, tBTM_CMPL_CB *p_cb)
{
	tACL_CONN *p;

	BTM_TRACE(API, "BTM_SetQoS: BdAddr: %02x%02x%02x%02x%02x%02x", bd[0], bd[1],
	          bd[2], bd[3], bd[4], bd[5]);

	if (btm_cb.devcb.p_qossu_cmpl_cb)
		return BTM_BUSY;

	if ((p = btm_bda_to_acl(bd)) != NULL)
	{
		btu_start_timer(&btm_cb.devcb.qossu_timer, BTU_TTYPE_BTM_ACL,
		                BTM_ACL_REPLY_TIMEOUT);
		btm_cb.devcb.p_qossu_cmpl_cb = p_cb;

		return BTM_CMD_STARTED;
	}

	return BTM_UNKNOWN_ADDR;
}

void btm_acl_timeout(TIMER_LIST_ENT *p_tle)
{
	tBTM_CMPL_CB *p_cb;
	tBTM_LINK_POLICY_RESULTS lnkpol;

	if (p_tle->param == TT_DEV_RLNKP)
	{
		p_cb = btm_cb.devcb.p_rlinkp_cmpl_cb;
		lnkpol.status = BTM_ERR_PROCESSING;
		lnkpol.settings = 0;
		btm_cb.devcb.p_rlinkp_cmpl_cb = NULL;

		if (p_cb)
			(*p_cb)(&lnkpol);
	}
}

UINT16 btm_get_max_packet_size(BD_ADDR addr)
{
	tACL_CONN *p = btm_bda_to_acl(addr);
	UINT16 pkt_types = 0;
	UINT16 pkt_size = 0;

	if (p)
	{
		pkt_types = p->pkt_types_mask;
	}
	else
	{
		if (!memcmp(btm_cb.devcb.local_addr, addr, BD_ADDR_LEN))
			pkt_types = btm_cb.btm_acl_pkt_types_supported;
	}

	if (pkt_types)
	{
		if (!(pkt_types & BTM_ACL_PKT_TYPES_MASK_NO_3_DH5))
			pkt_size = HCI_EDR3_DH5_PACKET_SIZE;
		else if (!(pkt_types & BTM_ACL_PKT_TYPES_MASK_NO_2_DH5))
			pkt_size = HCI_EDR2_DH5_PACKET_SIZE;
		else if (!(pkt_types & BTM_ACL_PKT_TYPES_MASK_NO_3_DH3))
			pkt_size = HCI_EDR3_DH3_PACKET_SIZE;
		else if (pkt_types & BTM_ACL_PKT_TYPES_MASK_DH5)
			pkt_size = HCI_DH5_PACKET_SIZE;
		else if (!(pkt_types & BTM_ACL_PKT_TYPES_MASK_NO_2_DH3))
			pkt_size = HCI_EDR2_DH3_PACKET_SIZE;
		else if (pkt_types & BTM_ACL_PKT_TYPES_MASK_DM5)
			pkt_size = HCI_DM5_PACKET_SIZE;
		else if (pkt_types & BTM_ACL_PKT_TYPES_MASK_DH3)
			pkt_size = HCI_DH3_PACKET_SIZE;
		else if (pkt_types & BTM_ACL_PKT_TYPES_MASK_DM3)
			pkt_size = HCI_DM3_PACKET_SIZE;
		else if (!(pkt_types & BTM_ACL_PKT_TYPES_MASK_NO_3_DH1))
			pkt_size = HCI_EDR3_DH1_PACKET_SIZE;
		else if (!(pkt_types & BTM_ACL_PKT_TYPES_MASK_NO_2_DH1))
			pkt_size = HCI_EDR2_DH1_PACKET_SIZE;
		else if (pkt_types & BTM_ACL_PKT_TYPES_MASK_DH1)
			pkt_size = HCI_DH1_PACKET_SIZE;
		else if (pkt_types & BTM_ACL_PKT_TYPES_MASK_DM1)
			pkt_size = HCI_DM1_PACKET_SIZE;
	}

	if (pkt_size == HCI_EDR3_DH5_PACKET_SIZE
	    && btu_cb.hcit_acl_data_size == 1017)
	{
		pkt_size = 1017;
	}

	return pkt_size;
}

tBTM_STATUS BTM_AclRegisterForChanges(tBTM_ACL_DB_CHANGE_CB *p_cb)
{
	if (!p_cb)
		btm_cb.p_acl_changed_cb = NULL;
	else if (btm_cb.p_acl_changed_cb)
		return BTM_BUSY;
	else
		btm_cb.p_acl_changed_cb = p_cb;

	return BTM_SUCCESS;
}

void btm_qos_setup_complete(UINT8 status, UINT16 handle, FLOW_SPEC *p_flow)
{
	tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_qossu_cmpl_cb;
	tBTM_QOS_SETUP_CMPL qossu;

	btu_stop_timer(&btm_cb.devcb.qossu_timer);

	btm_cb.devcb.p_qossu_cmpl_cb = NULL;

	if (p_cb)
	{
		qossu.status = status;
		qossu.handle = handle;

		if (p_flow != NULL)
		{
			qossu.flow.qos_flags = p_flow->qos_flags;
			qossu.flow.service_type = p_flow->service_type;
			qossu.flow.token_rate = p_flow->token_rate;
			qossu.flow.peak_bandwidth = p_flow->peak_bandwidth;
			qossu.flow.latency = p_flow->latency;
			qossu.flow.delay_variation = p_flow->delay_variation;
		}

		BTM_TRACE(DEBUG, "BTM: p_flow->delay_variation: 0x%02x",
		          p_flow->delay_variation);

		(*p_cb)(&qossu);
	}
}

tBTM_STATUS BTM_ReadRSSI(BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb)
{
	tACL_CONN *p;

	BTM_TRACE(API, "BTM_ReadRSSI: RemBdAddr: %02x%02x%02x%02x%02x%02x",
	          remote_bda[0], remote_bda[1], remote_bda[2], remote_bda[3],
	          remote_bda[4], remote_bda[5]);

	if (btm_cb.devcb.p_rssi_cmpl_cb)
		return BTM_BUSY;

	p = btm_bda_to_acl(remote_bda);
	if (p)
	{
		btu_start_timer(&btm_cb.devcb.rssi_timer, BTU_TTYPE_BTM_ACL,
		                BTM_ACL_REPLY_TIMEOUT);
		btm_cb.devcb.p_rssi_cmpl_cb = p_cb;

		if (!btsnd_hcic_read_rssi(p->hci_handle))
		{
			btm_cb.devcb.p_rssi_cmpl_cb = NULL;
			btu_stop_timer(&btm_cb.devcb.rssi_timer);
			return BTM_NO_RESOURCES;
		}

		return BTM_CMD_STARTED;
	}

	return BTM_UNKNOWN_ADDR;
}

tBTM_STATUS BTM_ReadLinkQuality(BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb)
{
	tACL_CONN *p;

	BTM_TRACE(API, "BTM_ReadLinkQuality: RemBdAddr: %02x%02x%02x%02x%02x%02x",
	          remote_bda[0], remote_bda[1], remote_bda[2], remote_bda[3],
	          remote_bda[4], remote_bda[5]);

	if (btm_cb.devcb.p_lnk_qual_cmpl_cb)
		return BTM_BUSY;

	p = btm_bda_to_acl(remote_bda);
	if (p)
	{
		btu_start_timer(&btm_cb.devcb.lnk_quality_timer, BTU_TTYPE_BTM_ACL,
		                BTM_ACL_REPLY_TIMEOUT);
		btm_cb.devcb.p_lnk_qual_cmpl_cb = p_cb;

		if (!btsnd_hcic_get_link_quality(p->hci_handle))
		{
			btu_stop_timer(&btm_cb.devcb.lnk_quality_timer);
			btm_cb.devcb.p_lnk_qual_cmpl_cb = NULL;
			return BTM_NO_RESOURCES;
		}

		return BTM_CMD_STARTED;
	}

	return BTM_UNKNOWN_ADDR;
}

void btm_read_rssi_complete(UINT8 *p)
{
	tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_rssi_cmpl_cb;
	tBTM_RSSI_RESULTS results;
	tACL_CONN *p_acl_cb = &btm_cb.acl_db[0];
	UINT16 handle;
	UINT8 xx;

	btu_stop_timer(&btm_cb.devcb.rssi_timer);

	btm_cb.devcb.p_rssi_cmpl_cb = NULL;

	if (p_cb)
	{
		STREAM_TO_UINT8(p, &results.hci_status);

		if (results.hci_status == HCI_SUCCESS)
		{
			results.status = BTM_SUCCESS;

			STREAM_TO_UINT16(p, &handle);
			STREAM_TO_UINT8(p, &results.rssi);

			BTM_TRACE(EVENT, "BTM RSSI Complete: rssi %d, hci status 0x%02x",
			          results.rssi, results.hci_status);

			for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_acl_cb++)
			{
				if (p_acl_cb->in_use && handle == p_acl_cb->hci_handle)
				{
					memcpy(results.rem_bda, p_acl_cb->remote_addr, BD_ADDR_LEN);
					break;
				}
			}
		}
		else
		{
			results.status = BTM_ERR_PROCESSING;
		}

		(*p_cb)(&results);
	}
}

void btm_read_link_quality_complete(UINT8 *p)
{
	tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_lnk_qual_cmpl_cb;
	tBTM_LINK_QUALITY_RESULTS results;
	tACL_CONN *p_acl_cb = &btm_cb.acl_db[0];
	UINT16 handle;
	UINT8 xx;

	btu_stop_timer(&btm_cb.devcb.rssi_timer);

	btm_cb.devcb.p_lnk_qual_cmpl_cb = NULL;

	if (p_cb)
	{
		STREAM_TO_UINT8(p, &results.hci_status);

		if (results.hci_status == HCI_SUCCESS)
		{
			results.status = BTM_SUCCESS;

			STREAM_TO_UINT16(p, &handle);
			STREAM_TO_UINT8(p, &results.link_quality);

			BTM_TRACE(EVENT, "BTM Link Quality Complete: Link Quality %d, hci status 0x%02x",
			          results.link_quality, results.hci_status);

			for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_acl_cb++)
			{
				if (p_acl_cb->in_use && handle == p_acl_cb->hci_handle)
				{
					memcpy(results.rem_bda, p_acl_cb->remote_addr, BD_ADDR_LEN);
					break;
				}
			}
		}
		else
		{
			results.status = BTM_ERR_PROCESSING;
		}

		(*p_cb)(&results);
	}
}

tBTM_STATUS btm_remove_acl(BD_ADDR bd_addr)
{
	UINT16 hci_handle = BTM_GetHCIConnHandle(bd_addr);

	if (!btsnd_hcic_disconnect(hci_handle, HCI_ERR_PEER_USER))
		return BTM_NO_RESOURCES;

	return BTM_SUCCESS;
}

void btm_chg_all_acl_pkt_types(BOOLEAN entering_sco)
{
	tACL_CONN *p = &btm_cb.acl_db[0];
	UINT16 xx;
	UINT16 temp_pkt_types;
	tBTM_PM_MODE mode;
	tBTM_PM_PWR_MD pm;

	if (entering_sco)
	{
		for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p++)
		{
			if (p->in_use)
			{
				BTM_TRACE(DEBUG, "btm BEFORE SCO setting to 1 slot; hci hdl 0x%x",
				          p->hci_handle);

				p->restore_pkt_types = p->pkt_types_mask;

				temp_pkt_types = BTM_ACL_PKT_TYPES_MASK_DM1
				               | BTM_ACL_PKT_TYPES_MASK_DH1;

				if (btm_cb.devcb.local_version.hci_version
				    >= HCI_PROTO_VERSION_2_0)
				{
					temp_pkt_types |= BTM_ACL_PKT_TYPES_MASK_NO_2_DH3
					                | BTM_ACL_PKT_TYPES_MASK_NO_3_DH3
					                | BTM_ACL_PKT_TYPES_MASK_NO_2_DH5
					                | BTM_ACL_PKT_TYPES_MASK_NO_3_DH5;
				}

				btm_set_packet_types_inline(p, temp_pkt_types);
			}
		}
	}
	else
	{
		for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p++)
		{
			if (p->in_use && p->restore_pkt_types)
			{
				if (BTM_ReadPowerMode(p->remote_addr, &mode) == BTM_SUCCESS
				    && mode == BTM_PM_STS_SNIFF)
				{
					BTM_TRACE(DEBUG, "btm last SCO removed; unsniffing hci hdl 0x%x",
					          p->hci_handle);

					pm.mode = BTM_PM_MD_ACTIVE;

					BTM_SetPowerMode(BTM_PM_SET_ONLY_ID, p->remote_addr, &pm);
				}
				else
				{
					BTM_TRACE(DEBUG, "btm last SCO removed; hci hdl 0x%x, types 0x%02x",
					          p->hci_handle, p->pkt_types_mask);

					btm_set_packet_types_inline(p, p->restore_pkt_types);

					p->restore_pkt_types = 0;
				}
			}
		}
	}
}

tBTM_STATUS BTM_SetLinkSuperTout(BD_ADDR remote_bda, UINT16 timeout)
{
	tACL_CONN *p = btm_bda_to_acl(remote_bda);

	if (p != NULL)
	{
		p->link_super_tout = timeout;

		if (p->link_role == BTM_ROLE_MASTER)
		{
			if (btsnd_hcic_write_link_super_tout(p->hci_handle, timeout))
				return BTM_CMD_STARTED;

			return BTM_NO_RESOURCES;
		}

		return BTM_SUCCESS;
	}

	return BTM_UNKNOWN_ADDR;
}
