#include <btm_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/btm/btm_sec.c
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

// HYPOTHESIS: the security_required bit that marks a dedicated bonding in
// progress.  not a name from the original headers
#define BTM_SEC_REQ_BONDING			0x40

/*******************************************************************************
 * local function declarations
 */

static void btm_restore_mode(void);
static BOOLEAN btm_sec_start_get_name(tBTM_SEC_DEV_REC *p_dev_rec);
static BOOLEAN btm_sec_start_encryption(tBTM_SEC_DEV_REC *p_dev_rec);
static tBTM_STATUS btm_sec_start_authorization(tBTM_SEC_DEV_REC *p_dev_rec);
static tBTM_STATUS btm_sec_execute_procedure(tBTM_SEC_DEV_REC *p_dev_rec);
static tBTM_SEC_SERV_REC *btm_sec_find_first_serv(BOOLEAN is_originator,
                                                  UINT16 psm);
static tBTM_SEC_SERV_REC *btm_sec_find_next_serv(tBTM_SEC_SERV_REC *p_cur);
static tBTM_SEC_SERV_REC *btm_sec_find_mx_serv(BOOLEAN is_originator,
                                               UINT16 psm, UINT32 mx_proto_id,
                                               UINT32 mx_chan_id);
static void btm_sec_pin_code_request_timeout(void);
static void btm_sec_collision_timeout(void);

/*******************************************************************************
 * functions
 */

BOOLEAN BTM_SecRegister(tBTM_APPL_INFO *p_cb_info)
{
	btm_cb.api = *p_cb_info;

	BTM_TRACE(EVENT, "BTM_Sec: application registered");

	return TRUE;
}

// HYPOTHESIS: BTM_SetSecurityMode is stripped from the final dol, so only its
// strings survive.  body is a guess.
void BTM_SetSecurityMode(UINT8 security_mode)
{
	UINT8 old_mode = btm_cb.security_mode;

	BTM_TRACE(EVENT, "BTM_SetSecurityMode: mode:%d", security_mode);

	if (security_mode == old_mode)
		return;

	btm_cb.security_mode = security_mode;

	if (old_mode == BTM_SEC_MODE_LINK)
	{
		BTM_TRACE(EVENT, "BTM_SetSecurityMode: Authen Enable -> FALSE");
		btsnd_hcic_write_auth_enable(FALSE);
	}
	else if (security_mode == BTM_SEC_MODE_LINK)
	{
		BTM_TRACE(EVENT, "BTM_SetSecurityMode: Authen Enable -> TRUE");
		btsnd_hcic_write_auth_enable(TRUE);
	}
}

BOOLEAN BTM_SecAddRmtNameNotifyCallback(tBTM_RMT_NAME_CALLBACK *p_callback)
{
	int i;

	for (i = 0; i < BTM_SEC_MAX_RMT_NAME_CALLBACKS; i++)
	{
		if (btm_cb.p_rmt_name_callback[i] == NULL)
		{
			btm_cb.p_rmt_name_callback[i] = p_callback;
			return TRUE;
		}
	}

	return FALSE;
}

BOOLEAN BTM_SecDeleteRmtNameNotifyCallback(tBTM_RMT_NAME_CALLBACK *p_callback)
{
	int i;

	for (i = 0; i < BTM_SEC_MAX_RMT_NAME_CALLBACKS; i++)
	{
		if (btm_cb.p_rmt_name_callback[i] == p_callback)
		{
			btm_cb.p_rmt_name_callback[i] = NULL;
			return TRUE;
		}
	}

	return FALSE;
}

void BTM_SetPinType(UINT8 pin_type, PIN_CODE pin_code, UINT8 pin_code_len)
{
	BTM_TRACE(API, "BTM_SetPinType: pin type %d [variable-0, fixed-1], code %s, length %d",
	          pin_type, pin_code, pin_code_len);

	if (btm_cb.devcb.state > BTM_DEV_STATE_WAIT_AFTER_RESET)
		btsnd_hcic_write_pin_type(pin_type);

	btm_cb.cfg.pin_type = pin_type;
	btm_cb.cfg.pin_code_len = pin_code_len;
	memcpy(btm_cb.cfg.pin_code, pin_code, pin_code_len);
}

BOOLEAN BTM_SetSecurityLevel(BOOLEAN is_originator, char *p_name,
                             UINT8 service_id, UINT8 sec_level, UINT16 psm,
                             UINT32 mx_proto_id, UINT32 mx_chan_id)
{
	tBTM_SEC_SERV_REC *p_srec;
	UINT16 index;
	UINT16 first_unused_record = 0xffff;
	BOOLEAN record_allocated = FALSE;

	p_srec = &btm_cb.sec_serv_rec[0];
	for (index = 0; index < BTM_SEC_MAX_SERVICE_RECORDS; index++, p_srec++)
	{
		if (p_srec->security_flags & BTM_SEC_IN_USE)
		{
			if (p_srec->psm == psm && p_srec->mx_proto_id == mx_proto_id
			    && service_id == p_srec->service_id
			    && (!strncmp(p_name, (char *)p_srec->orig_service_name,
			                 BTM_SEC_SERVICE_NAME_LEN)
			        || !strncmp(p_name, (char *)p_srec->term_service_name,
			                    BTM_SEC_SERVICE_NAME_LEN)))
			{
				record_allocated = TRUE;
				break;
			}
		}
		else if (!record_allocated)
		{
			record_allocated = TRUE;
			first_unused_record = index;
		}
	}

	if (record_allocated)
	{
		if (index >= BTM_SEC_MAX_SERVICE_RECORDS)
		{
			index = first_unused_record;
			p_srec = &btm_cb.sec_serv_rec[index];
		}

		p_srec->psm = psm;
		p_srec->service_id = service_id;
		p_srec->mx_proto_id = mx_proto_id;

		if (is_originator)
		{
			p_srec->orig_mx_chan_id = mx_chan_id;
			p_srec->security_flags &= ~(BTM_SEC_OUT_ENCRYPT
			                            | BTM_SEC_OUT_AUTHENTICATE
			                            | BTM_SEC_OUT_AUTHORIZE);
			strncpy((char *)p_srec->orig_service_name, p_name,
			        BTM_SEC_SERVICE_NAME_LEN);
		}
		else
		{
			p_srec->term_mx_chan_id = mx_chan_id;
			p_srec->security_flags &= ~(BTM_SEC_IN_ENCRYPT
			                            | BTM_SEC_IN_AUTHENTICATE
			                            | BTM_SEC_IN_AUTHORIZE);
			strncpy((char *)p_srec->term_service_name, p_name,
			        BTM_SEC_SERVICE_NAME_LEN);
		}

		p_srec->security_flags |= (UINT8)(sec_level | BTM_SEC_IN_USE);

		BTM_TRACE(API, "BTM_SEC_REG[%d]: id %d, is_orig %d, psm 0x%04x, proto_id %d, chan_id %d",
		          index, service_id, is_originator, psm, mx_proto_id,
		          mx_chan_id);

		BTM_TRACE(API, "               : sec: 0x%x, service name [%s] (up to %d chars saved)",
		          p_srec->security_flags, p_name, BTM_SEC_SERVICE_NAME_LEN);
	}
	else
	{
		BTM_TRACE(WARNING, "BTM_SEC_REG: Out of Service Records (%d)",
		          BTM_SEC_MAX_SERVICE_RECORDS);
	}

	return record_allocated;
}

void BTM_PINCodeReply(BD_ADDR bd_addr, UINT8 res, UINT8 pin_len, UINT8 *p_pin,
                      UINT32 trusted_mask[])
{
	tBTM_SEC_DEV_REC *p_dev_rec;

	if (btm_cb.sec_pin_code_req_tle.param)
	{
		btu_stop_timer(&btm_cb.sec_pin_code_req_tle);
		btm_cb.sec_pin_code_req_tle.param = 0;
		memset(btm_cb.sec_pin_code_req_bd_addr, 0xff, BD_ADDR_LEN);

		if (res != BTM_SUCCESS)
		{
			btm_cb.acl_disc_reason = HCI_ERR_HOST_REJECT_SECURITY;
			btsnd_hcic_pin_code_neg_reply(bd_addr);
		}
		else
		{
			btm_cb.acl_disc_reason = HCI_SUCCESS;
			btsnd_hcic_pin_code_req_reply(bd_addr, pin_len, p_pin);

			if ((p_dev_rec = btm_find_dev(bd_addr)) != NULL)
				BTM_SEC_COPY_TRUSTED_DEVICE(trusted_mask, p_dev_rec->trusted_mask);
		}
	}
}

void BTM_DeviceAuthorized(BD_ADDR bd_addr, UINT8 res, UINT32 trusted_mask[])
{
	tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev(bd_addr);
	tBTM_STATUS rc;

	if (p_dev_rec == NULL)
	{
		BTM_TRACE(WARNING, "Security Manager: Attempting Authorization of Unknown Device Address [%02x%02x%02x%02x%02x%02x]",
		          bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4],
		          bd_addr[5]);
		return;
	}

	BTM_TRACE(EVENT, "Security Manager: authorized status:%d State:%d", res,
	          p_dev_rec ? p_dev_rec->sec_state : 0);

	if (res == BTM_SUCCESS)
	{
		p_dev_rec->sec_flags |= BTM_SEC_AUTHORIZED;
		BTM_SEC_COPY_TRUSTED_DEVICE(trusted_mask, p_dev_rec->trusted_mask);
	}

	if (p_dev_rec->sec_state == BTM_SEC_STATE_AUTHORIZING)
	{
		p_dev_rec->sec_state = BTM_SEC_STATE_IDLE;

		if (res != BTM_SUCCESS)
		{
			if (p_dev_rec->p_callback)
			{
				if (p_dev_rec->is_originator && p_dev_rec->p_cur_service
				    && p_dev_rec->p_cur_service->mx_proto_id
				           == BTM_SEC_PROTO_RFCOMM)
				{
					p_dev_rec->p_cur_service->orig_mx_chan_id = 0;
				}

				(*p_dev_rec->p_callback)(bd_addr, p_dev_rec->p_ref_data, res);

				p_dev_rec->p_callback = NULL;
			}
		}
		else
		{
			rc = btm_sec_execute_procedure(p_dev_rec);

			if (rc != BTM_CMD_STARTED && p_dev_rec->p_callback)
			{
				if (p_dev_rec->is_originator && p_dev_rec->p_cur_service
				    && p_dev_rec->p_cur_service->mx_proto_id
				           == BTM_SEC_PROTO_RFCOMM)
				{
					p_dev_rec->p_cur_service->orig_mx_chan_id = 0;
				}

				(*p_dev_rec->p_callback)(bd_addr, p_dev_rec->p_ref_data, rc);

				p_dev_rec->p_callback = NULL;
			}
		}
	}
}

tBTM_STATUS BTM_SecBond(BD_ADDR bd_addr, UINT8 pin_len, UINT8 *p_pin,
                        UINT32 trusted_mask[])
{
	tBTM_SEC_DEV_REC *p_dev_rec;
	UINT8 *p_features;
	UINT8 allow_switch;
	tBTM_INQ_INFO *p_inq_info;
	BOOLEAN ok;

	BTM_TRACE(API, "BTM_SecBond BDA: %02x:%02x:%02x:%02x:%02x:%02x",
	          bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4],
	          bd_addr[5]);

	if (pin_len > PIN_CODE_LEN)
	{
		BTM_TRACE(ERROR, "BTM_SecBond: Illegal Pin len:%d", pin_len);
		return BTM_ILLEGAL_VALUE;
	}

	if ((p_dev_rec = btm_find_dev(bd_addr)) == NULL
	    && (p_dev_rec = btm_sec_alloc_dev(bd_addr)) == NULL)
	{
		BTM_TRACE(ERROR, "BTM_SecBond: no device block");
		return BTM_NO_RESOURCES;
	}

	if (p_dev_rec->hci_handle != BTM_SEC_INVALID_HANDLE
	    && (p_dev_rec->sec_flags & BTM_SEC_AUTHENTICATED))
	{
		BTM_TRACE(WARNING, "BTM_SecBond -> Already Paired");
		return BTM_SUCCESS;
	}

	if (BTM_DeleteStoredLinkKey(bd_addr, NULL) != BTM_SUCCESS)
		return BTM_NO_RESOURCES;

	p_dev_rec->security_required = BTM_SEC_REQ_BONDING | BTM_SEC_OUT_AUTHENTICATE;
	p_dev_rec->is_originator = TRUE;

	BTM_SEC_COPY_TRUSTED_DEVICE(trusted_mask, p_dev_rec->trusted_mask);

	btm_cb.pin_code_len = pin_len;
	if (pin_len)
		memcpy(btm_cb.pin_code, p_pin, pin_len);

	p_dev_rec->sec_flags &= ~(BTM_SEC_LINK_KEY_KNOWN | BTM_SEC_AUTHENTICATED
	                          | BTM_SEC_ENCRYPTED);

	if ((p_dev_rec->dev_class[1] & 0x1f) == 5
	    && (p_dev_rec->dev_class[2] & 0x40)
	    && btm_cb.cfg.pin_type != 1)
	{
		btm_cb.pin_type_changed = TRUE;
		btsnd_hcic_write_pin_type(1);
	}

	btm_cb.disconnect_after_bond_reason = 0;

	if (p_dev_rec->hci_handle != BTM_SEC_INVALID_HANDLE)
	{
		p_dev_rec->sec_flags &= ~BTM_SEC_LINK_KEY_KNOWN;
		p_dev_rec->sec_state = BTM_SEC_STATE_AUTHENTICATING;

		if (!btsnd_hcic_auth_request(p_dev_rec->hci_handle))
			return BTM_NO_RESOURCES;

		btm_cb.bond_created_connection = FALSE;
		return BTM_CMD_STARTED;
	}

	if (btm_cb.security_mode != BTM_SEC_MODE_LINK)
	{
		BTM_TRACE(DEBUG, "BTM_SecBond: Authen Enable -> TRUE");

		if (!btsnd_hcic_write_auth_enable(TRUE))
		{
			BTM_TRACE(ERROR, "BTM_SecBond: no buffer");
			return BTM_NO_RESOURCES;
		}

		btm_cb.security_mode_changed = TRUE;
	}

	btm_cb.bond_created_connection = TRUE;

	p_features = BTM_ReadLocalFeatures();
	allow_switch = (p_features[0] & 0x20) ? 1 : 0;

	p_dev_rec->sec_acl_connected = FALSE;

	if ((p_inq_info = BTM_InqDbRead(bd_addr)) != NULL)
	{
		ok = btsnd_hcic_create_conn(
			bd_addr, 0x18, p_inq_info->results.page_scan_rep_mode,
			p_inq_info->results.page_scan_mode,
			p_inq_info->results.clock_offset | BTM_CLOCK_OFFSET_VALID,
			allow_switch);
	}
	else
	{
		ok = btsnd_hcic_create_conn(bd_addr, 0x18, 1, 0,
		                            p_dev_rec->clock_offset, allow_switch);
	}

	if (ok)
		return BTM_CMD_STARTED;

	BTM_TRACE(ERROR, "BTM_SecBond: no buffer");

	if (btm_cb.security_mode_changed)
	{
		btm_cb.security_mode_changed = FALSE;
		btm_restore_mode();
	}

	return BTM_NO_RESOURCES;
}

static void btm_restore_mode(void)
{
	BTM_TRACE(DEBUG, "btm_restore_mode: Authen Enable -> %d",
	          btm_cb.security_mode == BTM_SEC_MODE_LINK);

	btsnd_hcic_write_auth_enable(btm_cb.security_mode == BTM_SEC_MODE_LINK);
}

// HYPOTHESIS: BTM_SetEncryption is stripped from the final dol, so only its
// strings survive.  body is a guess.
tBTM_STATUS BTM_SetEncryption(BD_ADDR bd_addr, tBTM_SEC_CBACK *p_callback,
                              void *p_ref_data)
{
	tBTM_SEC_DEV_REC *p_dev_rec;
	tBTM_STATUS rc;

	p_dev_rec = btm_find_dev(bd_addr);

	if (!p_dev_rec || p_dev_rec->hci_handle == BTM_SEC_INVALID_HANDLE)
	{
		BTM_TRACE(WARNING, "Security Manager: BTM_SetEncryption not connected");
		return BTM_WRONG_MODE;
	}

	if (p_dev_rec->sec_flags & BTM_SEC_ENCRYPTED)
	{
		BTM_TRACE(EVENT, "Security Manager: BTM_SetEncryption already encrypted");
		return BTM_SUCCESS;
	}

	if (p_dev_rec->p_callback)
	{
		BTM_TRACE(WARNING, "Security Manager: BTM_SetEncryption busy");
		return BTM_BUSY;
	}

	p_dev_rec->p_callback = p_callback;
	p_dev_rec->p_ref_data = p_ref_data;
	p_dev_rec->security_required |= BTM_SEC_OUT_ENCRYPT;
	p_dev_rec->is_originator = TRUE;

	BTM_TRACE(EVENT, "Security Manager: BTM_SetEncryption Handle:%d State:%d Flags:0x%x Required:0x%x",
	          p_dev_rec->hci_handle, p_dev_rec->sec_state,
	          p_dev_rec->sec_flags, p_dev_rec->security_required);

	rc = btm_sec_execute_procedure(p_dev_rec);

	if (rc != BTM_CMD_STARTED && p_callback)
	{
		(*p_callback)(bd_addr, p_ref_data, rc);
		p_dev_rec->p_callback = NULL;
	}

	return rc;
}

tBTM_STATUS btm_sec_l2cap_access_req(
	BD_ADDR bd_addr, UINT16 psm, UINT16 handle, BOOLEAN is_originator,
	void (*p_callback)(unsigned char *, void *, unsigned char))
{
	tBTM_SEC_DEV_REC *p_dev_rec;
	tBTM_SEC_SERV_REC *p_serv_rec;
	UINT8 security_required;
	tBTM_STATUS rc;

	p_dev_rec = btm_find_dev(bd_addr);
	if (p_dev_rec == NULL && (p_dev_rec = btm_sec_alloc_dev(bd_addr)) == NULL)
	{
		BTM_TRACE(WARNING, "Security Manager: l2cap_access_req PSM:%d no resources",
		          psm);

		if (p_callback)
			(*p_callback)(bd_addr, NULL, BTM_NO_RESOURCES);

		return BTM_NO_RESOURCES;
	}

	p_dev_rec->hci_handle = handle;

	p_serv_rec = btm_sec_find_first_serv(is_originator, psm);
	if (p_serv_rec == NULL)
	{
		BTM_TRACE(WARNING, "Security Manager: l2cap_access_req PSM:%d no application registerd",
		          psm);

		if (p_callback)
			(*p_callback)(bd_addr, NULL, BTM_MODE_UNSUPPORTED);

		return BTM_MODE_UNSUPPORTED;
	}

	if (!is_originator || (is_originator && !p_dev_rec->p_cur_service))
		p_dev_rec->p_cur_service = p_serv_rec;

	security_required = p_serv_rec->security_flags;

	if (btm_sec_find_next_serv(p_serv_rec))
	{
		BTM_TRACE(EVENT, "Security Manager: l2cap_access_req PSM:%d postponed for multiplexer",
		          psm);

		if (p_callback)
			(*p_callback)(bd_addr, NULL, BTM_SUCCESS);

		return BTM_SUCCESS;
	}

	p_dev_rec->p_callback = p_callback;
	p_dev_rec->p_ref_data = NULL;
	p_dev_rec->security_required = security_required;
	p_dev_rec->is_originator = is_originator;

	BTM_TRACE(EVENT, "Security Manager: l2cap_access_req PSM:%d Handle:%d State:%d Flags:0x%x Required:0x%x",
	          psm, handle, p_dev_rec->sec_state, p_dev_rec->sec_flags,
	          security_required);

	if (!is_originator && (security_required & BTM_SEC_IN_AUTHORIZE))
	{
		p_dev_rec->sec_flags &= ~BTM_SEC_AUTHORIZED;

		BTM_TRACE(EVENT, "Security Manager: trusted:0x%04x%04x Flags:0x%x",
		          p_dev_rec->trusted_mask[1], p_dev_rec->trusted_mask[0],
		          p_dev_rec->sec_flags);
	}

	rc = btm_sec_execute_procedure(p_dev_rec);

	if (rc != BTM_CMD_STARTED && p_callback)
	{
		(*p_callback)(bd_addr, p_dev_rec->p_ref_data, rc);
		p_dev_rec->p_callback = NULL;
	}

	return rc;
}

void btm_sec_mx_access_request(BD_ADDR bd_addr, UINT16 psm,
                               BOOLEAN is_originator, UINT32 mx_proto_id,
                               UINT32 mx_chan_id, tBTM_SEC_CALLBACK *p_callback,
                               void *p_ref_data)
{
	tBTM_SEC_DEV_REC *p_dev_rec;
	tBTM_SEC_SERV_REC *p_serv_rec;
	tBTM_STATUS rc;

	p_dev_rec = btm_find_dev(bd_addr);
	if (p_dev_rec == NULL && (p_dev_rec = btm_sec_alloc_dev(bd_addr)) == NULL)
	{
		if (p_callback)
			(*p_callback)(bd_addr, p_ref_data, BTM_NO_RESOURCES);

		return;
	}

	p_serv_rec = btm_sec_find_mx_serv(is_originator, psm, mx_proto_id,
	                                  mx_chan_id);
	if (p_serv_rec == NULL)
	{
		if (p_callback)
			(*p_callback)(bd_addr, p_ref_data, BTM_MODE_UNSUPPORTED);

		BTM_TRACE(ERROR, "Security Manager: MX service not found PSM:%d Proto:%d SCN:%d",
		          psm, mx_proto_id, mx_chan_id);
		return;
	}

	p_dev_rec->p_cur_service = p_serv_rec;
	p_dev_rec->security_required = p_serv_rec->security_flags;
	p_dev_rec->is_originator = is_originator;
	p_dev_rec->p_callback = p_callback;
	p_dev_rec->p_ref_data = p_ref_data;
	p_dev_rec->sec_flags &= ~BTM_SEC_AUTHORIZED;

	rc = btm_sec_execute_procedure(p_dev_rec);

	if (rc != BTM_CMD_STARTED && p_callback)
	{
		if (p_dev_rec->is_originator && p_dev_rec->p_cur_service
		    && p_dev_rec->p_cur_service->mx_proto_id == BTM_SEC_PROTO_RFCOMM)
		{
			p_dev_rec->p_cur_service->orig_mx_chan_id = 0;
		}

		(*p_callback)(bd_addr, p_ref_data, rc);
		p_dev_rec->p_callback = NULL;
	}
}

void btm_sec_conn_req(UINT8 *bda, UINT8 *dc)
{
	tBTM_SEC_DEV_REC *p_dev_rec;
	void *p_buf;

	if (btm_cb.connect_only_paired
	    && ((p_dev_rec = btm_find_dev(bda)) == NULL
	        || !(p_dev_rec->sec_flags & BTM_SEC_LINK_KEY_KNOWN)))
	{
		BTM_TRACE(EVENT, "Security Manager: connect request from not paired device");

		if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
			btsnd_hcic_reject_conn(p_buf, bda, HCI_ERR_HOST_REJECT_DEVICE);
	}
	else if (btm_cb.p_conn_filter_cb && !(*btm_cb.p_conn_filter_cb)(bda, dc))
	{
		if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
			btsnd_hcic_reject_conn(p_buf, bda, HCI_ERR_HOST_REJECT_DEVICE);
	}
	else
	{
		memcpy(btm_cb.connecting_bda, bda, BD_ADDR_LEN);
		memcpy(btm_cb.connecting_dc, dc, DEV_CLASS_LEN);

		l2c_link_hci_conn_req(bda);
	}
}

void btm_sec_init(UINT8 sec_mode)
{
	btm_cb.security_mode = sec_mode;
	memset(btm_cb.sec_pin_code_req_bd_addr, 0xff, BD_ADDR_LEN);
}

void btm_sec_dev_reset(void)
{
	if (btm_cb.security_mode == BTM_SEC_MODE_LINK)
	{
		btsnd_hcic_write_auth_enable(TRUE);
		btsnd_hcic_write_encr_mode(TRUE);
	}
}

void btm_sec_abort_access_req(BD_ADDR bd_addr)
{
	tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev(bd_addr);

	if (p_dev_rec)
	{
		if (btm_cb.api.p_abort_callback)
		{
			(*btm_cb.api.p_abort_callback)(bd_addr, p_dev_rec->dev_class,
			                               p_dev_rec->sec_bd_name);
		}

		if (p_dev_rec->sec_state == BTM_SEC_STATE_AUTHORIZING
		    || p_dev_rec->sec_state == BTM_SEC_STATE_AUTHENTICATING)
		{
			p_dev_rec->sec_state = BTM_SEC_STATE_IDLE;

			if (p_dev_rec->is_originator && p_dev_rec->p_cur_service
			    && p_dev_rec->p_cur_service->mx_proto_id == BTM_SEC_PROTO_RFCOMM)
			{
				p_dev_rec->p_cur_service->orig_mx_chan_id = 0;
			}

			p_dev_rec->p_callback = NULL;
		}
	}
}

void btm_sec_rmt_name_request_complete(UINT8 *p_bd_addr, UINT8 *p_bd_name,
                                       UINT8 status)
{
	tBTM_SEC_DEV_REC *p_dev_rec;
	int i;
	tBTM_STATUS rc;

	if (p_bd_addr)
	{
		p_dev_rec = btm_find_dev(p_bd_addr);
	}
	else
	{
		p_dev_rec = &btm_cb.sec_dev_rec[0];
		for (i = 0; i < BTM_SEC_MAX_DEVICE_RECORDS; i++, p_dev_rec++)
		{
			if ((p_dev_rec->sec_flags & BTM_SEC_IN_USE)
			    && p_dev_rec->sec_state == BTM_SEC_STATE_GETTING_NAME)
			{
				break;
			}
		}

		if (i == BTM_SEC_MAX_DEVICE_RECORDS)
			p_dev_rec = NULL;
	}

	BTM_TRACE(EVENT, "Security Manager: rmt_name_complete status:%d State:%d",
	          status, p_dev_rec ? p_dev_rec->sec_state : 0);

	if (p_dev_rec == NULL)
		return;

	if (status == HCI_SUCCESS)
	{
		memcpy(p_dev_rec->sec_bd_name, p_bd_name, BTM_MAX_REM_BD_NAME_LEN);
		p_dev_rec->sec_flags |= BTM_SEC_NAME_KNOWN;

		for (i = 0; i < BTM_SEC_MAX_RMT_NAME_CALLBACKS; i++)
		{
			if (btm_cb.p_rmt_name_callback[i])
			{
				(*btm_cb.p_rmt_name_callback[i])(p_bd_addr,
				                                 p_dev_rec->dev_class,
				                                 p_dev_rec->sec_bd_name);
			}
		}
	}

	if (p_dev_rec->link_key_not_sent)
	{
		if ((p_dev_rec->security_required & BTM_SEC_REQ_BONDING)
		    && !p_dev_rec->sec_acl_connected)
		{
			return;
		}

		p_dev_rec->link_key_not_sent = FALSE;

		if (btm_cb.api.p_link_key_callback)
		{
			(*btm_cb.api.p_link_key_callback)(
				p_dev_rec->bd_addr, p_dev_rec->dev_class,
				p_dev_rec->sec_bd_name, p_dev_rec->link_key,
				p_dev_rec->link_key_type);
		}

		for (i = 0; i < BTM_SEC_MAX_LINK_KEY_CALLBACKS; i++)
		{
			if (btm_cb.p_link_key_callback[i])
			{
				(*btm_cb.p_link_key_callback[i])(
					p_dev_rec->bd_addr, p_dev_rec->dev_class,
					p_dev_rec->sec_bd_name, p_dev_rec->link_key,
					p_dev_rec->link_key_type);
			}
		}
	}

	if ((p_dev_rec->security_required & BTM_SEC_REQ_BONDING)
	    && (p_dev_rec->sec_flags & BTM_SEC_AUTHENTICATED))
	{
		p_dev_rec->security_required &= ~(BTM_SEC_REQ_BONDING
		                                  | BTM_SEC_OUT_AUTHENTICATE);
		btsnd_hcic_disconnect(p_dev_rec->hci_handle, HCI_ERR_PEER_USER);
		return;
	}

	if (p_dev_rec->sec_state == BTM_SEC_STATE_GETTING_NAME)
	{
		p_dev_rec->sec_state = BTM_SEC_STATE_IDLE;

		if (status != HCI_SUCCESS)
		{
			if (p_dev_rec->p_callback)
			{
				(*p_dev_rec->p_callback)(p_dev_rec->bd_addr,
				                         p_dev_rec->p_ref_data,
				                         BTM_ERR_PROCESSING);
				p_dev_rec->p_callback = NULL;
			}
		}
		else
		{
			rc = btm_sec_execute_procedure(p_dev_rec);

			if (rc != BTM_CMD_STARTED && p_dev_rec->p_callback)
			{
				(*p_dev_rec->p_callback)(p_dev_rec->bd_addr,
				                         p_dev_rec->p_ref_data, rc);
				p_dev_rec->p_callback = NULL;
			}
		}
	}
}

void btm_sec_auth_complete(UINT16 handle, UINT8 status)
{
	tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev_by_handle(handle);
	tBTM_STATUS rc;
	UINT8 reason;

	BTM_TRACE(EVENT, "Security Manager: auth_complete status:%d State:%d",
	          status, p_dev_rec ? p_dev_rec->sec_state : 0);

	if (btm_cb.security_mode_changed)
	{
		btm_cb.security_mode_changed = FALSE;
		btm_restore_mode();
	}

	if (btm_cb.pin_type_changed)
	{
		btm_cb.pin_type_changed = FALSE;
		btsnd_hcic_write_pin_type(btm_cb.cfg.pin_type);
	}

	if (p_dev_rec && (p_dev_rec->security_required & BTM_SEC_REQ_BONDING)
	    && !btm_cb.bond_created_connection)
	{
		p_dev_rec->security_required &= ~(BTM_SEC_REQ_BONDING
		                                  | BTM_SEC_OUT_AUTHENTICATE);

		if ((reason = btm_cb.disconnect_after_bond_reason) != 0)
			btm_sec_disconnect(handle, reason);
	}

	if (p_dev_rec == NULL)
		return;

	if (p_dev_rec->sec_state != BTM_SEC_STATE_AUTHENTICATING)
	{
		if (btm_cb.api.p_auth_complete_callback && status != HCI_SUCCESS)
		{
			(*btm_cb.api.p_auth_complete_callback)(
				p_dev_rec->bd_addr, p_dev_rec->dev_class,
				p_dev_rec->sec_bd_name, status);
		}

		return;
	}

	if (status == 0x0c
	    && (p_dev_rec->sec_flags
	        & (BTM_SEC_AUTHENTICATED | BTM_SEC_ENCRYPTED))
	           == (BTM_SEC_AUTHENTICATED | BTM_SEC_ENCRYPTED))
	{
		status = HCI_SUCCESS;
	}

	if (btm_cb.api.p_auth_complete_callback)
	{
		(*btm_cb.api.p_auth_complete_callback)(
			p_dev_rec->bd_addr, p_dev_rec->dev_class, p_dev_rec->sec_bd_name,
			status);
	}

	p_dev_rec->sec_state = BTM_SEC_STATE_IDLE;

	if (p_dev_rec->security_required & BTM_SEC_REQ_BONDING)
	{
		p_dev_rec->security_required &= ~(BTM_SEC_REQ_BONDING
		                                  | BTM_SEC_OUT_AUTHENTICATE);
		btsnd_hcic_disconnect(p_dev_rec->hci_handle, HCI_ERR_PEER_USER);
		return;
	}

	if (status != HCI_SUCCESS)
	{
		if (p_dev_rec->p_callback)
		{
			if (p_dev_rec->is_originator && p_dev_rec->p_cur_service
			    && p_dev_rec->p_cur_service->mx_proto_id == BTM_SEC_PROTO_RFCOMM)
			{
				p_dev_rec->p_cur_service->orig_mx_chan_id = 0;
			}

			(*p_dev_rec->p_callback)(p_dev_rec->bd_addr,
			                         p_dev_rec->p_ref_data, BTM_ERR_PROCESSING);
			p_dev_rec->p_callback = NULL;
		}

		btsnd_hcic_disconnect(p_dev_rec->hci_handle, HCI_ERR_AUTH_FAILURE);
		return;
	}

	p_dev_rec->sec_flags |= BTM_SEC_AUTHENTICATED;

	rc = btm_sec_execute_procedure(p_dev_rec);

	if (rc != BTM_CMD_STARTED && p_dev_rec->p_callback)
	{
		if (p_dev_rec->is_originator && p_dev_rec->p_cur_service
		    && p_dev_rec->p_cur_service->mx_proto_id == BTM_SEC_PROTO_RFCOMM)
		{
			p_dev_rec->p_cur_service->orig_mx_chan_id = 0;
		}

		(*p_dev_rec->p_callback)(p_dev_rec->bd_addr, p_dev_rec->p_ref_data,
		                         rc);
		p_dev_rec->p_callback = NULL;
	}
}

void btm_sec_mkey_comp_event(UINT16 handle, UINT8 status, UINT8 key_flg)
{
	tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev_by_handle(handle);
	BD_ADDR null_bda = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	BTM_TRACE(EVENT, "Security Manager: mkey comp status:%d State:%d", status,
	          p_dev_rec ? p_dev_rec->sec_state : 0);

	if (btm_cb.mkey_cback)
	{
		if (p_dev_rec == NULL)
			(*btm_cb.mkey_cback)(null_bda, status, key_flg);
		else
			(*btm_cb.mkey_cback)(p_dev_rec->bd_addr, status, key_flg);
	}
}

void btm_sec_encrypt_change(UINT16 handle, UINT8 status, UINT8 encr_enable)
{
	tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev_by_handle(handle);
	tBTM_STATUS rc;

	BTM_TRACE(EVENT, "Security Manager: encrypt_change status:%d State:%d",
	          status, p_dev_rec ? p_dev_rec->sec_state : 0);

	if (p_dev_rec)
	{
		if (status == HCI_ERR_LMP_ERR_TRANS_COLLISION)
		{
			btm_cb.p_collided_dev_rec = p_dev_rec;
			btm_cb.sec_collision_tle.param
				= (TIMER_PARAM_TYPE)btm_sec_collision_timeout;

			btu_start_timer(&btm_cb.sec_collision_tle, BTU_TTYPE_USER_FUNC, 1);
		}
		else if (status != HCI_SUCCESS)
		{
			if (p_dev_rec->p_callback)
			{
				(*p_dev_rec->p_callback)(p_dev_rec->bd_addr,
				                         p_dev_rec->p_ref_data,
				                         BTM_ERR_PROCESSING);
				p_dev_rec->p_callback = NULL;
			}
		}
		else
		{
			p_dev_rec->sec_flags |= BTM_SEC_AUTHENTICATED | BTM_SEC_ENCRYPTED;

			if (p_dev_rec->sec_state == BTM_SEC_STATE_ENCRYPTING)
			{
				p_dev_rec->sec_state = BTM_SEC_STATE_IDLE;

				rc = btm_sec_execute_procedure(p_dev_rec);

				if (rc != BTM_CMD_STARTED && p_dev_rec->p_callback)
				{
					(*p_dev_rec->p_callback)(p_dev_rec->bd_addr,
					                         p_dev_rec->p_ref_data, rc);
					p_dev_rec->p_callback = NULL;
				}
			}
		}
	}
}

BOOLEAN btm_sec_is_bonding(UINT16 handle)
{
	tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev_by_handle(handle);

	if (p_dev_rec && (p_dev_rec->security_required & BTM_SEC_REQ_BONDING))
		return TRUE;

	return FALSE;
}

void btm_sec_connected(UINT8 *bda, UINT16 handle, UINT8 status, UINT8 enc_mode)
{
	tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev(bda);
	tBTM_STATUS res;
	int i;

	BTM_TRACE(EVENT, "Security Manager: btm_sec_connected handle:%d status:%d enc_mode:%d",
	          handle, status, enc_mode);

	if (!p_dev_rec)
	{
		if (status == HCI_SUCCESS)
		{
			if ((p_dev_rec = btm_sec_alloc_dev(bda)) == NULL)
				return;
		}
		else
		{
			return;
		}
	}
	else
	{
		p_dev_rec->timestamp = btm_cb.dev_rec_count++;
		p_dev_rec->sec_acl_connected = TRUE;
	}

	if (p_dev_rec->security_required & BTM_SEC_REQ_BONDING)
	{
		if (btm_cb.security_mode_changed)
		{
			btm_cb.security_mode_changed = FALSE;
			btm_restore_mode();
		}

		if (btm_cb.pin_type_changed)
		{
			btm_cb.pin_type_changed = FALSE;
			btsnd_hcic_write_pin_type(btm_cb.cfg.pin_type);
		}
	}

	if (status != HCI_SUCCESS)
	{
		if (status == HCI_ERR_AUTH_FAILURE || status == HCI_ERR_KEY_MISSING
		    || status == HCI_ERR_HOST_REJECT_SECURITY
		    || status == HCI_ERR_PAIRING_NOT_ALLOWED
		    || status == HCI_ERR_UNIT_KEY_USED
		    || status == HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED
		    || status == HCI_ERR_ENCRY_MODE_NOT_ACCEPTABLE
		    || status == HCI_ERR_REPEATED_ATTEMPTS
		    || status == 0x22 /* HCI_ERR_LMP_RESPONSE_TIMEOUT */
		    || (p_dev_rec->security_required & BTM_SEC_REQ_BONDING))
		{
			p_dev_rec->security_required &= ~(BTM_SEC_REQ_BONDING
			                                  | BTM_SEC_OUT_AUTHENTICATE);
			p_dev_rec->sec_flags &= ~BTM_SEC_LINK_KEY_KNOWN;

			if (btm_cb.sec_pin_code_req_tle.param)
			{
				btu_stop_timer(&btm_cb.sec_pin_code_req_tle);
				btm_cb.sec_pin_code_req_tle.param = 0;
				status = HCI_ERR_HOST_REJECT_SECURITY;
			}

			if (btm_cb.acl_disc_reason == HCI_ERR_HOST_REJECT_SECURITY)
				status = HCI_ERR_HOST_REJECT_SECURITY;

			if (btm_cb.api.p_auth_complete_callback)
			{
				(*btm_cb.api.p_auth_complete_callback)(
					p_dev_rec->bd_addr, p_dev_rec->dev_class,
					p_dev_rec->sec_bd_name, status);
			}
		}
		else if (p_dev_rec->p_callback)
		{
			if (p_dev_rec->is_originator && p_dev_rec->p_cur_service
			    && p_dev_rec->p_cur_service->mx_proto_id == BTM_SEC_PROTO_RFCOMM)
			{
				p_dev_rec->p_cur_service->orig_mx_chan_id = 0;
			}

			if (status == 8 || status == HCI_ERR_UNSPECIFIED || status == 4)
			{
				(*p_dev_rec->p_callback)(p_dev_rec->bd_addr,
				                         p_dev_rec->p_ref_data,
				                         BTM_DEVICE_TIMEOUT);
			}
			else
			{
				(*p_dev_rec->p_callback)(p_dev_rec->bd_addr,
				                         p_dev_rec->p_ref_data,
				                         BTM_ERR_PROCESSING);
			}
		}

		return;
	}

	if ((p_dev_rec->security_required & BTM_SEC_REQ_BONDING)
	    && p_dev_rec->link_key_not_sent)
	{
		p_dev_rec->link_key_not_sent = FALSE;

		if (btm_cb.api.p_link_key_callback)
		{
			(*btm_cb.api.p_link_key_callback)(
				p_dev_rec->bd_addr, p_dev_rec->dev_class,
				p_dev_rec->sec_bd_name, p_dev_rec->link_key,
				p_dev_rec->link_key_type);
		}

		for (i = 0; i < BTM_SEC_MAX_LINK_KEY_CALLBACKS; i++)
		{
			if (btm_cb.p_link_key_callback[i])
			{
				(*btm_cb.p_link_key_callback[i])(
					p_dev_rec->bd_addr, p_dev_rec->dev_class,
					p_dev_rec->sec_bd_name, p_dev_rec->link_key,
					p_dev_rec->link_key_type);
			}
		}

		if (btm_cb.api.p_auth_complete_callback)
		{
			(*btm_cb.api.p_auth_complete_callback)(
				p_dev_rec->bd_addr, p_dev_rec->dev_class,
				p_dev_rec->sec_bd_name, HCI_SUCCESS);
		}

		p_dev_rec->security_required &= ~(BTM_SEC_REQ_BONDING
		                                  | BTM_SEC_OUT_AUTHENTICATE);

		btsnd_hcic_disconnect(handle, HCI_ERR_PEER_USER);
		return;
	}

	p_dev_rec->hci_handle = handle;

	p_dev_rec->sec_flags &= 0xf8;

	if (enc_mode)
		p_dev_rec->sec_flags |= BTM_SEC_AUTHENTICATED | BTM_SEC_ENCRYPTED;

	if (btm_cb.security_mode == BTM_SEC_MODE_LINK)
		p_dev_rec->sec_flags |= BTM_SEC_AUTHENTICATED;

	p_dev_rec->link_key_changed = FALSE;

	if (!(p_dev_rec->sec_flags & BTM_SEC_NAME_KNOWN)
	    || p_dev_rec->is_originator)
	{
		res = btm_sec_execute_procedure(p_dev_rec);

		if (res != BTM_CMD_STARTED && p_dev_rec->p_callback)
		{
			if (p_dev_rec->is_originator && p_dev_rec->p_cur_service
			    && p_dev_rec->p_cur_service->mx_proto_id == BTM_SEC_PROTO_RFCOMM)
			{
				p_dev_rec->p_cur_service->orig_mx_chan_id = 0;
			}

			(*p_dev_rec->p_callback)(p_dev_rec->bd_addr,
			                         p_dev_rec->p_ref_data, res);
			p_dev_rec->p_callback = NULL;
		}
	}
}

tBTM_STATUS btm_sec_disconnect(UINT16 handle, UINT8 reason)
{
	tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev_by_handle(handle);
	UINT8 old_state;

	if (!p_dev_rec)
		return BTM_ILLEGAL_VALUE;

	if (p_dev_rec->security_required & BTM_SEC_REQ_BONDING)
	{
		btm_cb.disconnect_after_bond_reason = reason;
		return BTM_SUCCESS;
	}

	old_state = p_dev_rec->sec_state;
	if (old_state == BTM_SEC_STATE_DISCONNECTING)
		return BTM_CMD_STARTED;

	p_dev_rec->sec_state = BTM_SEC_STATE_DISCONNECTING;

	if (btsnd_hcic_disconnect(handle, reason))
		return BTM_CMD_STARTED;

	p_dev_rec->sec_state = old_state;
	return BTM_NO_RESOURCES;
}

void btm_sec_disconnected(UINT16 handle, UINT8 reason)
{
	tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev_by_handle(handle);

	if (!p_dev_rec)
		return;

	if ((p_dev_rec->security_required & BTM_SEC_REQ_BONDING)
	    && btm_cb.api.p_auth_complete_callback)
	{
		if (reason == HCI_ERR_REPEATED_ATTEMPTS)
		{
			(*btm_cb.api.p_auth_complete_callback)(
				p_dev_rec->bd_addr, p_dev_rec->dev_class,
				p_dev_rec->sec_bd_name, HCI_ERR_REPEATED_ATTEMPTS);
		}
		else
		{
			(*btm_cb.api.p_auth_complete_callback)(
				p_dev_rec->bd_addr, p_dev_rec->dev_class,
				p_dev_rec->sec_bd_name, HCI_ERR_AUTH_FAILURE);
		}
	}

	if (btm_cb.sec_pin_code_req_tle.param
	    && !memcmp(btm_cb.sec_pin_code_req_bd_addr, p_dev_rec->bd_addr,
	               BD_ADDR_LEN))
	{
		btu_stop_timer(&btm_cb.sec_pin_code_req_tle);
		btm_cb.sec_pin_code_req_tle.param = 0;
		memset(btm_cb.sec_pin_code_req_bd_addr, 0xff, BD_ADDR_LEN);

		if (btm_cb.api.p_auth_complete_callback)
		{
			(*btm_cb.api.p_auth_complete_callback)(
				p_dev_rec->bd_addr, p_dev_rec->dev_class,
				p_dev_rec->sec_bd_name, HCI_ERR_HOST_REJECT_SECURITY);
		}
	}

	p_dev_rec->hci_handle = BTM_SEC_INVALID_HANDLE;
	p_dev_rec->sec_state = BTM_SEC_STATE_IDLE;
	p_dev_rec->sec_flags &= 0xf8;
	p_dev_rec->security_required &= 0x80;
	p_dev_rec->p_callback = NULL;
}

void btm_sec_link_key_notification(UINT8 *p_bda, UINT8 *p_link_key,
                                   UINT8 key_type)
{
	tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev(p_bda);
	int i;

	if (!p_dev_rec && !(p_dev_rec = btm_sec_alloc_dev(p_bda)))
		return;

	BTM_TRACE(EVENT, "btm_sec_link_key_notification()  BDA: %02x:%02x:%02x:%02x:%02x:%02x",
	          p_bda[0], p_bda[1], p_bda[2], p_bda[3], p_bda[4], p_bda[5]);

	BTM_TRACE(EVENT, "                                TYPE: %d", key_type);

	if (btm_cb.devcb.local_version.hci_version < 1)
		p_dev_rec->link_key_type = 0xff;
	else
		p_dev_rec->link_key_type = key_type;

	p_dev_rec->sec_flags |= BTM_SEC_LINK_KEY_KNOWN;
	memcpy(p_dev_rec->link_key, p_link_key, LINK_KEY_LEN);

	if (!(p_dev_rec->sec_flags & BTM_SEC_NAME_KNOWN)
	    || ((p_dev_rec->security_required & BTM_SEC_REQ_BONDING)
	        && !p_dev_rec->sec_acl_connected))
	{
		p_dev_rec->link_key_not_sent = TRUE;
		return;
	}

	if (btm_cb.api.p_link_key_callback)
	{
		(*btm_cb.api.p_link_key_callback)(p_bda, p_dev_rec->dev_class,
		                                  p_dev_rec->sec_bd_name, p_link_key,
		                                  p_dev_rec->link_key_type);
	}

	for (i = 0; i < BTM_SEC_MAX_LINK_KEY_CALLBACKS; i++)
	{
		if (btm_cb.p_link_key_callback[i])
		{
			(*btm_cb.p_link_key_callback[i])(p_bda, p_dev_rec->dev_class,
			                                 p_dev_rec->sec_bd_name,
			                                 p_link_key,
			                                 p_dev_rec->link_key_type);
		}
	}
}

void btm_sec_link_key_request(UINT8 *p_bda)
{
	tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev(p_bda);
	LINK_KEY link_key;

	BTM_TRACE(EVENT, "btm_sec_link_key_request()  BDA: %02x:%02x:%02x:%02x:%02x:%02x",
	          p_bda[0], p_bda[1], p_bda[2], p_bda[3], p_bda[4], p_bda[5]);

	if (p_dev_rec && (p_dev_rec->sec_flags & BTM_SEC_LINK_KEY_KNOWN))
	{
		btsnd_hcic_link_key_req_reply(p_bda, p_dev_rec->link_key);
		return;
	}

	if (p_dev_rec == NULL)
		p_dev_rec = btm_sec_alloc_dev(p_bda);

	if (!(p_dev_rec->security_required & BTM_SEC_REQ_BONDING)
	    && btm_cb.api.p_link_key_req_callback)
	{
		if (p_dev_rec)
		{
			if ((*btm_cb.api.p_link_key_req_callback)(p_bda,
			                                          p_dev_rec->link_key)
			    == BTM_SUCCESS)
			{
				btsnd_hcic_link_key_req_reply(p_bda, p_dev_rec->link_key);
				return;
			}
		}
		else
		{
			if ((*btm_cb.api.p_link_key_req_callback)(p_bda, link_key)
			    == BTM_SUCCESS)
			{
				btsnd_hcic_link_key_req_reply(p_bda, link_key);
				return;
			}
		}
	}

	btsnd_hcic_link_key_neg_reply(p_bda);
}

static void btm_sec_pin_code_request_timeout(void)
{
	tBTM_CB *p_cb = &btm_cb;

	BTM_TRACE(EVENT, "btm_sec_pin_code_request_timeout()");

	p_cb->sec_pin_code_req_tle.param = 0;
	memset(btm_cb.sec_pin_code_req_bd_addr, 0xff, BD_ADDR_LEN);

	btsnd_hcic_pin_code_neg_reply(p_cb->connecting_bda);
}

void btm_sec_pin_code_request(UINT8 *p_bda)
{
	tBTM_SEC_DEV_REC *p_dev_rec;
	tBTM_CB *p_cb = &btm_cb;
	BOOLEAN reject = FALSE;
	int i = 0;

	BTM_TRACE(EVENT, "btm_sec_pin_code_request()  BDA: %02x:%02x:%02x:%02x:%02x:%02x",
	          p_bda[0], p_bda[1], p_bda[2], p_bda[3], p_bda[4], p_bda[5]);

	if (!p_cb->pairing_disabled && p_cb->cfg.pin_type == 1)
	{
		if (btm_find_dev(p_bda) == NULL)
		{
			tBTM_SEC_DEV_REC *p = &btm_cb.sec_dev_rec[0];

			for (; i < BTM_SEC_MAX_DEVICE_RECORDS; i++, p++)
			{
				if (!(p->sec_flags & BTM_SEC_IN_USE))
					break;
			}
		}

		if (i >= BTM_SEC_MAX_DEVICE_RECORDS)
			btsnd_hcic_pin_code_neg_reply(p_bda);
		else
			btsnd_hcic_pin_code_req_reply(p_bda, p_cb->cfg.pin_code_len,
			                              p_cb->cfg.pin_code);

		return;
	}

	p_dev_rec = btm_find_dev(p_bda);
	if (p_dev_rec == NULL)
		p_dev_rec = btm_sec_alloc_dev(p_bda);

	memcpy(btm_cb.sec_pin_code_req_bd_addr, p_bda, BD_ADDR_LEN);

	if (p_dev_rec)
	{
		if (!memcmp(p_bda, p_cb->connecting_bda, BD_ADDR_LEN))
			memcpy(p_dev_rec->dev_class, p_cb->connecting_dc, DEV_CLASS_LEN);

		if (p_dev_rec->security_required & BTM_SEC_REQ_BONDING)
		{
			BTM_TRACE(EVENT, "btm_sec_pin_code_request bonding sending reply");

			btsnd_hcic_pin_code_req_reply(p_bda, p_cb->pin_code_len,
			                              p_cb->pin_code);

			if (btm_cb.security_mode_changed)
			{
				btm_cb.security_mode_changed = FALSE;

				BTM_TRACE(DEBUG, "btm_sec_pin_code_request: Authen Enable -> %d",
				          btm_cb.security_mode == BTM_SEC_MODE_LINK);

				btsnd_hcic_write_auth_enable(btm_cb.security_mode
				                             == BTM_SEC_MODE_LINK);
			}
		}
		else if (p_cb->pairing_disabled || !p_cb->api.p_pin_callback
		         || (!p_dev_rec->is_originator
		             && (p_dev_rec->dev_class[1] & 0x1f) == 5
		             && (p_dev_rec->dev_class[2] & 0x40)))
		{
			reject = TRUE;
		}
		else
		{
			memcpy(p_cb->connecting_bda, p_bda, BD_ADDR_LEN);
			memcpy(p_cb->connecting_dc, p_dev_rec->dev_class, DEV_CLASS_LEN);

			p_cb->sec_pin_code_req_tle.param
				= (TIMER_PARAM_TYPE)btm_sec_pin_code_request_timeout;
			btu_start_timer(&p_cb->sec_pin_code_req_tle, BTU_TTYPE_USER_FUNC,
			                60);

			(*p_cb->api.p_pin_callback)(p_bda, p_dev_rec->dev_class,
			                            p_dev_rec->sec_bd_name);
		}
	}
	else
	{
		reject = TRUE;
	}

	if (reject)
	{
		BTM_TRACE(WARNING, "btm_sec_pin_code_request(): Pairing disabled:%d; PIN callback:%x, Dev Rec:%x!",
		          p_cb->pairing_disabled, p_cb->api.p_pin_callback,
		          p_dev_rec);

		btsnd_hcic_pin_code_neg_reply(p_bda);
	}
}

void btm_sec_clk_offset(UINT16 handle, UINT16 clock_offset)
{
	tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev_by_handle(handle);
	tBTM_INQ_INFO *p_inq_info;

	if (p_dev_rec)
	{
		clock_offset |= BTM_CLOCK_OFFSET_VALID;
		p_dev_rec->clock_offset = clock_offset;

		p_inq_info = BTM_InqDbRead(p_dev_rec->bd_addr);
		if (p_inq_info)
			p_inq_info->results.clock_offset = clock_offset;
	}
}

static tBTM_STATUS btm_sec_execute_procedure(tBTM_SEC_DEV_REC *p_dev_rec)
{
	BTM_TRACE(EVENT, "btm_sec_execute_procedure: Required:0x%x Flags:0x%x State:%d",
	          p_dev_rec->security_required, p_dev_rec->sec_flags,
	          p_dev_rec->sec_state);

	if (p_dev_rec->sec_state != BTM_SEC_STATE_IDLE)
		return BTM_CMD_STARTED;

	if (p_dev_rec->hci_handle == BTM_SEC_INVALID_HANDLE)
	{
		tL2C_LCB *p_lcb = l2cu_allocate_lcb(p_dev_rec->bd_addr);

		if (!p_lcb)
		{
			L2CAP_TRACE(WARNING, "L2CAP - no LCB for L2CA_conn_req");
			return BTM_NO_RESOURCES;
		}

		if (!l2cu_create_conn(p_lcb))
			return BTM_NO_RESOURCES;

		p_dev_rec->is_originator = TRUE;
		return BTM_CMD_STARTED;
	}

	if (!(p_dev_rec->sec_flags & BTM_SEC_NAME_KNOWN)
	    && p_dev_rec->hci_handle != BTM_SEC_INVALID_HANDLE)
	{
		BTM_TRACE(EVENT, "Security Manager: Start get name");

		if (btm_sec_start_get_name(p_dev_rec))
			return BTM_CMD_STARTED;

		p_dev_rec->sec_flags |= BTM_SEC_NAME_KNOWN;
	}

	if (!(p_dev_rec->sec_flags & BTM_SEC_AUTHENTICATED)
	    && ((p_dev_rec->is_originator
	         && (p_dev_rec->security_required & BTM_SEC_OUT_AUTHENTICATE))
	        || (!p_dev_rec->is_originator
	            && (p_dev_rec->security_required & BTM_SEC_IN_AUTHENTICATE)))
	    && p_dev_rec->hci_handle != BTM_SEC_INVALID_HANDLE)
	{
		BTM_TRACE(EVENT, "Security Manager: Start authentication");

		p_dev_rec->sec_state = BTM_SEC_STATE_AUTHENTICATING;

		if (!btsnd_hcic_auth_request(p_dev_rec->hci_handle))
			return BTM_NO_RESOURCES;

		return BTM_CMD_STARTED;
	}

	if (!(p_dev_rec->sec_flags & BTM_SEC_ENCRYPTED)
	    && ((p_dev_rec->is_originator
	         && (p_dev_rec->security_required & BTM_SEC_OUT_ENCRYPT))
	        || (!p_dev_rec->is_originator
	            && (p_dev_rec->security_required & BTM_SEC_IN_ENCRYPT)))
	    && p_dev_rec->hci_handle != BTM_SEC_INVALID_HANDLE)
	{
		BTM_TRACE(EVENT, "Security Manager: Start encryption");

		if (!btm_sec_start_encryption(p_dev_rec))
			return BTM_NO_RESOURCES;

		return BTM_CMD_STARTED;
	}

	if (!(p_dev_rec->sec_flags & BTM_SEC_AUTHORIZED)
	    && ((p_dev_rec->is_originator
	         && (p_dev_rec->security_required & BTM_SEC_OUT_AUTHORIZE))
	        || (!p_dev_rec->is_originator
	            && (p_dev_rec->security_required & BTM_SEC_IN_AUTHORIZE)))
	    && !(p_dev_rec->trusted_mask[0] == BTM_SEC_TRUST_ALL
	         && p_dev_rec->trusted_mask[1] == BTM_SEC_TRUST_ALL)
	    && p_dev_rec->p_cur_service->service_id < BTM_SEC_MAX_SERVICES
	    && !BTM_SEC_IS_SERVICE_TRUSTED(p_dev_rec->trusted_mask,
	                                   p_dev_rec->p_cur_service->service_id))
	{
		BTM_TRACE(EVENT, "Security Manager: Start authorization");

		return btm_sec_start_authorization(p_dev_rec);
	}

	p_dev_rec->security_required &= ~(BTM_SEC_OUT_AUTHORIZE
	                                  | BTM_SEC_OUT_ENCRYPT
	                                  | BTM_SEC_OUT_AUTHENTICATE
	                                  | BTM_SEC_IN_AUTHORIZE
	                                  | BTM_SEC_IN_ENCRYPT
	                                  | BTM_SEC_IN_AUTHENTICATE);

	BTM_TRACE(EVENT, "Security Manager: trusted:0x%04x%04x",
	          p_dev_rec->trusted_mask[1], p_dev_rec->trusted_mask[0]);

	BTM_TRACE(EVENT, "Security Manager: access granted");

	return BTM_SUCCESS;
}

static BOOLEAN btm_sec_start_get_name(tBTM_SEC_DEV_REC *p_dev_rec)
{
	UINT8 tempstate = p_dev_rec->sec_state;
	BOOLEAN started;

	p_dev_rec->sec_state = BTM_SEC_STATE_GETTING_NAME;

	if (btm_initiate_rem_name(p_dev_rec->bd_addr, NULL, BTM_RMT_NAME_SEC,
	                          BTM_EXT_RMT_NAME_TIMEOUT, NULL)
	    != BTM_CMD_STARTED)
	{
		p_dev_rec->sec_state = tempstate;
		started = FALSE;
	}
	else
	{
		started = TRUE;
	}

	return started;
}

static BOOLEAN btm_sec_start_encryption(tBTM_SEC_DEV_REC *p_dev_rec)
{
	if (!btsnd_hcic_set_conn_encrypt(p_dev_rec->hci_handle, TRUE))
		return FALSE;

	p_dev_rec->sec_state = BTM_SEC_STATE_ENCRYPTING;
	return TRUE;
}

static tBTM_STATUS btm_sec_start_authorization(tBTM_SEC_DEV_REC *p_dev_rec)
{
	UINT8 *p_service_name = NULL;
	UINT8 service_id;
	tBTM_STATUS rc;

	if ((p_dev_rec->sec_flags & BTM_SEC_NAME_KNOWN)
	    || p_dev_rec->hci_handle == BTM_SEC_INVALID_HANDLE)
	{
		if (!btm_cb.api.p_authorize_callback)
			return BTM_MODE_UNSUPPORTED;

		if (p_dev_rec->p_cur_service)
		{
			if (p_dev_rec->is_originator)
				p_service_name = p_dev_rec->p_cur_service->orig_service_name;
			else
				p_service_name = p_dev_rec->p_cur_service->term_service_name;

			service_id = p_dev_rec->p_cur_service->service_id;
		}
		else
		{
			service_id = 0;
		}

		p_dev_rec->sec_state = BTM_SEC_STATE_AUTHORIZING;

		rc = (*btm_cb.api.p_authorize_callback)(
			p_dev_rec->bd_addr, p_dev_rec->dev_class, p_dev_rec->sec_bd_name,
			p_service_name, service_id, p_dev_rec->is_originator);

		if (rc == BTM_SUCCESS)
		{
			p_dev_rec->sec_flags |= BTM_SEC_AUTHORIZED;
			p_dev_rec->sec_state = BTM_SEC_STATE_IDLE;
		}

		return rc;
	}

	if (btm_sec_start_get_name(p_dev_rec))
		;

	return BTM_CMD_STARTED;
}

static void btm_sec_collision_timeout(void)
{
	tBTM_STATUS status;

	BTM_TRACE(EVENT, "btm_sec_collision_timeout()");

	btm_cb.sec_collision_tle.param = 0;

	status = btm_sec_execute_procedure(btm_cb.p_collided_dev_rec);

	if (status != BTM_CMD_STARTED && btm_cb.p_collided_dev_rec->p_callback)
	{
		if (btm_cb.p_collided_dev_rec->is_originator
		    && btm_cb.p_collided_dev_rec->p_cur_service
		    && btm_cb.p_collided_dev_rec->p_cur_service->mx_proto_id
		           == BTM_SEC_PROTO_RFCOMM)
		{
			btm_cb.p_collided_dev_rec->p_cur_service->orig_mx_chan_id = 0;
		}

		(*btm_cb.p_collided_dev_rec->p_callback)(
			btm_cb.p_collided_dev_rec->bd_addr,
			btm_cb.p_collided_dev_rec->p_ref_data, status);

		btm_cb.p_collided_dev_rec->p_callback = NULL;
	}
}

UINT32 *btm_read_trusted_mask(BD_ADDR bd_addr)
{
	tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev(bd_addr);

	if (p_dev_rec)
		return p_dev_rec->trusted_mask;

	return NULL;
}

static tBTM_SEC_SERV_REC *btm_sec_find_first_serv(BOOLEAN is_originator,
                                                  UINT16 psm)
{
	tBTM_SEC_SERV_REC *p_serv_rec = &btm_cb.sec_serv_rec[0];
	int i;

	for (i = 0; i < BTM_SEC_MAX_SERVICE_RECORDS; i++, p_serv_rec++)
	{
		if ((p_serv_rec->security_flags & BTM_SEC_IN_USE)
		    && p_serv_rec->psm == psm)
		{
			return p_serv_rec;
		}
	}

	return NULL;
}

static tBTM_SEC_SERV_REC *btm_sec_find_next_serv(tBTM_SEC_SERV_REC *p_cur)
{
	tBTM_SEC_SERV_REC *p_serv_rec = &btm_cb.sec_serv_rec[0];
	BOOLEAN found = FALSE;
	int i;

	for (i = 0; i < BTM_SEC_MAX_SERVICE_RECORDS; i++, p_serv_rec++)
	{
		if ((p_serv_rec->security_flags & BTM_SEC_IN_USE)
		    && p_serv_rec->psm == p_cur->psm)
		{
			if (!found)
			{
				if (p_cur == p_serv_rec)
					found = TRUE;
			}
			else
			{
				return p_serv_rec;
			}
		}
	}

	return NULL;
}

static tBTM_SEC_SERV_REC *btm_sec_find_mx_serv(BOOLEAN is_originator,
                                               UINT16 psm, UINT32 mx_proto_id,
                                               UINT32 mx_chan_id)
{
	tBTM_SEC_SERV_REC *p_out_serv = &btm_cb.sec_serv_rec[0];
	int i;

	for (i = 0; i < BTM_SEC_MAX_SERVICE_RECORDS; i++, p_out_serv++)
	{
		if ((p_out_serv->security_flags & BTM_SEC_IN_USE)
		    && p_out_serv->psm == psm && p_out_serv->mx_proto_id == mx_proto_id
		    && ((is_originator && p_out_serv->orig_mx_chan_id == mx_chan_id)
		        || (!is_originator
		            && p_out_serv->term_mx_chan_id == mx_chan_id)))
		{
			return p_out_serv;
		}
	}

	return NULL;
}
