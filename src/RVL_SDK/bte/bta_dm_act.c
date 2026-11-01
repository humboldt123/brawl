#include <bta_dm_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * bta/dm/bta_dm_act.c
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
#include "revolution/types.h"

#include <bd.h>
#include <bta_api.h>
#include <bta_dm_co.h>
#include <bta_sys.h>
#include <bta_sys_int.h>
#include <btm_api.h>
#include <btm_int.h>
#include <btu.h>
#include <gki.h>
#include <hcimsgs.h>
#include <l2c_api.h>
#include <sdp_api.h>
#include <sdpdefs.h>

/*******************************************************************************
 * macros
 */

// HYPOTHESIS: local names for literals the headers don't spell out
#define BTA_DM_SDP_DB_SIZE				250
#define BTA_DM_RNR_MAX_PAGE_TIMEOUT		30000
#define BTA_DM_RNR_TIMER_SLACK			100
#define BTA_DM_SEARCH_TIMER_MS			3000
#define BTA_DM_DISABLE_TIMER_MS			5000
#define BTA_DM_DISABLE_CONN_TIMER_MS	1000
#define BTA_DM_L2CAP_NO_IDLE_TIMEOUT	0xffff
#define BTA_DM_L2CAP_IDLE_TIMEOUT		2
#define BTA_DM_ATTR_ID_VENDOR			0x8001

/*******************************************************************************
 * local function declarations
 */

static void bta_dm_disable_timer_cback(UINT32 param);
static void bta_dm_search_timer_cback(UINT32 param);
static void bta_dm_discover_device(BD_ADDR remote_bd_addr);
static void bta_dm_discover_next_device(void);
static void bta_dm_sdp_callback(UINT16 sdp_status);
static void bta_dm_inq_results_cb(void *p_result);
static void bta_dm_inq_cmpl_cb(void *p_result);
static void bta_dm_remname_cback(BD_ADDR bd_addr, DEV_CLASS dc,
                                 BD_NAME bd_name);
static void bta_dm_inq_name_cback(void *p_result);
static void bta_dm_disc_name_cback(void *p_result);
static UINT8 bta_dm_authorize_cback(BD_ADDR bd_addr, DEV_CLASS dev_class,
                                    tBTM_BD_NAME bd_name, UINT8 *service_name,
                                    UINT8 service_id, BOOLEAN is_originator);
static void bta_dm_pinname_cback(void *p_result);
static UINT8 bta_dm_pin_cback(BD_ADDR bd_addr, DEV_CLASS dev_class,
                              tBTM_BD_NAME bd_name);
static UINT8 bta_dm_link_key_req_cback(BD_ADDR bd_addr, LINK_KEY key);
static UINT8 bta_dm_new_link_key_cback(BD_ADDR bd_addr, DEV_CLASS dev_class,
                                       tBTM_BD_NAME bd_name, UINT8 *key,
                                       UINT8 key_type);
static UINT8 bta_dm_authentication_complete_cback(BD_ADDR bd_addr,
                                                  DEV_CLASS dev_class,
                                                  tBTM_BD_NAME bd_name,
                                                  int result);
static void bta_dm_local_addr_cback(void *p1);
static void bta_dm_signal_strength_timer_cback(UINT32 param);
static void bta_dm_acl_change_cback(BD_ADDR p_bda, DEV_CLASS p_dc,
                                    BD_NAME p_bdn, BD_FEATURES features,
                                    BOOLEAN is_new);
static void bta_dm_disable_conn_down_timer_cback(UINT32 param);
static void bta_dm_rssi_cback(void *p_result);
static void bta_dm_link_quality_cback(void *p_result);
static BOOLEAN bta_dm_l2cap_server_compress_cback(
	BD_ADDR peer_addr, signed a, signed b, signed c, signed d, signed e,
	signed f, UINT8 **p_mem_pool, UINT32 *mem_pool_size);
static void bta_dm_compress_cback(tBTA_SYS_CONN_STATUS status, UINT8 id,
                                  UINT8 app_id, BD_ADDR peer_addr);
static void bta_dm_rm_cback(tBTA_SYS_CONN_STATUS status, UINT8 id,
                            UINT8 app_id, BD_ADDR peer_addr);

/*******************************************************************************
 * variables
 */

UINT16 const bta_service_id_to_uuid_lkup_tbl[BTA_MAX_SERVICE_ID + 1] =
{
	0x1200, 0x1101, 0x1103, 0x1111,
	0x1102, 0x1108, 0x111e, 0x1105,
	0x1106, 0x1109, 0x1110, 0x1104,
	0x1118, 0x111b, 0x1115, 0x1116,
	0x1117, 0x112d, 0x110b, 0x110e,
	0x1124, 0x1304, 0x112f, 0x0000,
};

UINT32 const bta_service_id_to_btm_srv_id_lkup_tbl[BTA_MAX_SERVICE_ID + 1] =
{
	0x00, 0x01, 0x03, 0x0b,
	0x02, 0x0c, 0x1d, 0x06,
	0x07, 0x09, 0x0a, 0x04,
	0x16, 0x23, 0x19, 0x1b,
	0x1a, 0x28, 0x25, 0x27,
	0x20, 0x25, 0x2c, 0x00,
};

static tBTM_APPL_INFO const bta_security =
{
	bta_dm_authorize_cback,
	bta_dm_pin_cback,
	bta_dm_new_link_key_cback,
	bta_dm_link_key_req_cback,
	bta_dm_authentication_complete_cback,
	NULL,
};

static tBTA_DM_COMPRESS_SRVCS bta_dm_compress_srvcs[BTA_DM_NUM_COMPRESS_SRVS];

/*******************************************************************************
 * functions
 */

void bta_dm_enable(tBTA_DM_MSG *p_data)
{
	DEV_CLASS dev_class;

	memset(&bta_dm_search_cb, 0, sizeof(bta_dm_search_cb));
	memset(&bta_dm_conn_srvcs, 0, sizeof(bta_dm_conn_srvcs));

	memcpy(dev_class, bta_dm_cfg.dev_class, sizeof(DEV_CLASS));
	BTM_SetDeviceClass(dev_class);

	bta_dm_cb.p_sec_cback = p_data->enable.p_sec_cback;

	BTM_SecRegister((tBTM_APPL_INFO *)&bta_security);

	BTM_SetDefaultLinkSuperTout(bta_dm_cfg.link_timeout);
	BTM_WritePageTimeout(bta_dm_cfg.page_timeout);
	BTM_SetDefaultLinkPolicy(bta_dm_cfg.policy_settings);

	BTM_AclRegisterForChanges(bta_dm_acl_change_cback);
	BTM_ReadLocalDeviceAddr(bta_dm_local_addr_cback);

	bta_sys_rm_register(bta_dm_rm_cback);
	bta_sys_compress_register(bta_dm_compress_cback);

	bta_dm_init_pm();

	memset(bta_dm_compress_srvcs, 0, sizeof(bta_dm_compress_srvcs));

	bta_sys_compress_register(bta_dm_compress_cback);

	L2CA_RegisterCompression(bta_dm_l2cap_server_compress_cback, 0);

	btu_set_local_di_record();
}

void bta_dm_disable(tBTA_DM_MSG *p_data)
{
	L2CA_SetIdleTimeoutByBdAddr((UINT8 *)BT_BD_ANY, 0);

	bta_sys_disable();

	BTM_SetDiscoverability(BTM_NON_DISCOVERABLE, 0, 0);
	BTM_SetConnectability(BTM_NON_CONNECTABLE, 0, 0);

	bta_dm_disable_pm();

	if (BTM_GetNumAclLinks() == 0)
	{
		bta_dm_cb.disable_timer.p_cback =
			(TIMER_CBACK *)bta_dm_disable_conn_down_timer_cback;
		bta_sys_start_timer(&bta_dm_cb.disable_timer, 0,
		                    BTA_DM_DISABLE_CONN_TIMER_MS);
	}
	else
	{
		bta_dm_cb.disabling = TRUE;

		bta_dm_cb.disable_timer.p_cback =
			(TIMER_CBACK *)bta_dm_disable_timer_cback;
		bta_sys_start_timer(&bta_dm_cb.disable_timer, 0,
		                    BTA_DM_DISABLE_TIMER_MS);
	}
}

static void bta_dm_disable_timer_cback(UINT32 param)
{
	UINT8 i;

	APPL_TRACE(EVENT, " bta_dm_disable_timer_cback  ");

	if (BTM_GetNumAclLinks() && bta_dm_cb.keep_acl_on_shut_down == FALSE)
	{
		for (i = 0; i < bta_dm_cb.device_list.count; i++)
			btm_remove_acl(bta_dm_cb.device_list.peer_device[i].peer_bdaddr);
	}
	else
	{
		bta_dm_cb.disabling = FALSE;

		(*bta_dm_cb.p_sec_cback)(BTA_DM_DISABLE_EVT, NULL);
	}
}

void bta_dm_set_dev_name(tBTA_DM_MSG *p_data)
{
	BTM_SetLocalDeviceName(p_data->set_name.name);
}

void bta_dm_set_visibility(tBTA_DM_MSG *p_data)
{
	BTM_SetDiscoverability(p_data->set_visibility.disc_mode, 0, 0);
	BTM_SetConnectability(p_data->set_visibility.conn_mode, 0, 0);
}

void bta_dm_bond(tBTA_DM_MSG *p_data)
{
	UINT32 trusted_mask[BTM_SEC_SERVICE_ARRAY_SIZE];
	UINT32 *current_trusted_mask;

	current_trusted_mask = btm_read_trusted_mask(p_data->bond.bd_addr);

	if (current_trusted_mask)
		memcpy(trusted_mask, current_trusted_mask, sizeof(trusted_mask));
	else
		memset(trusted_mask, 0, sizeof(trusted_mask));

	BTM_SecBond(p_data->bond.bd_addr, p_data->bond.pin_len, p_data->bond.p_pin,
	            trusted_mask);
}

void bta_dm_pin_reply(tBTA_DM_MSG *p_data)
{
	UINT32 trusted_mask[BTM_SEC_SERVICE_ARRAY_SIZE];
	UINT32 *current_trusted_mask;

	current_trusted_mask = btm_read_trusted_mask(p_data->pin_reply.bd_addr);

	if (current_trusted_mask)
		memcpy(trusted_mask, current_trusted_mask, sizeof(trusted_mask));
	else
		memset(trusted_mask, 0, sizeof(trusted_mask));

	if (p_data->pin_reply.accept)
	{
		BTM_PINCodeReply(p_data->pin_reply.bd_addr, BTM_SUCCESS,
		                 p_data->pin_reply.pin_len, p_data->pin_reply.p_pin,
		                 trusted_mask);
	}
	else
	{
		BTM_PINCodeReply(p_data->pin_reply.bd_addr, BTM_NOT_AUTHORIZED, 0, NULL,
		                 trusted_mask);
	}
}

void bta_dm_auth_reply(tBTA_DM_MSG *p_data)
{
	UINT32 trusted_mask[BTM_SEC_SERVICE_ARRAY_SIZE];
	UINT32 *current_trusted_mask;

	current_trusted_mask = btm_read_trusted_mask(p_data->auth_reply.bd_addr);

	if (current_trusted_mask)
		memcpy(trusted_mask, current_trusted_mask, sizeof(trusted_mask));
	else
		memset(trusted_mask, 0, sizeof(trusted_mask));

	if (p_data->auth_reply.response != BTA_DM_NOT_AUTH)
	{
		if (p_data->auth_reply.response == BTA_DM_AUTH_PERM)
		{
			if (p_data->auth_reply.service <= BTA_MAX_SERVICE_ID)
			{
				UINT32 srv_id =
					bta_service_id_to_btm_srv_id_lkup_tbl[p_data->auth_reply.service];
				UINT8 q = srv_id / BTM_SEC_ARRAY_BITS;

				trusted_mask[q] |= ((UINT32)1 << (srv_id - q * BTM_SEC_ARRAY_BITS));
			}
		}

		BTM_DeviceAuthorized(p_data->auth_reply.bd_addr, BTM_SUCCESS,
		                     trusted_mask);
	}
	else
	{
		BTM_DeviceAuthorized(p_data->auth_reply.bd_addr, BTM_NOT_AUTHORIZED,
		                     trusted_mask);
	}
}

void bta_dm_search_start(tBTA_DM_MSG *p_data)
{
	BTM_ClearInqDb(NULL);

	bta_dm_search_cb.p_search_cback = p_data->search.p_cback;
	bta_dm_search_cb.services = p_data->search.services;

	BTM_StartInquiry((tBTM_INQ_PARMS *)&p_data->search.inq_params,
	                 bta_dm_inq_results_cb, bta_dm_inq_cmpl_cb);
}

void bta_dm_search_cancel(tBTA_DM_MSG *p_data)
{
	tBTA_DM_MSG *p_msg;

	BTM_ClearInqDb(NULL);

	if (BTM_IsInquiryActive())
	{
		BTM_CancelInquiry();

		(*bta_dm_search_cb.p_search_cback)(BTA_DM_SEARCH_CANCEL_CMPL_EVT, NULL);

		if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
		{
			p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;

			bta_sys_sendmsg(p_msg);
		}
	}
	else
	{
		if (bta_dm_search_cb.services == 0)
			BTM_CancelRemoteDeviceName();
	}
}

void bta_dm_discover(tBTA_DM_MSG *p_data)
{
	tBTA_DM_MSG *p_msg;

	bta_dm_search_cb.p_search_cback = p_data->discover.p_cback;
	bta_dm_search_cb.services_to_search = bta_dm_search_cb.services =
		p_data->discover.services;
	bta_dm_search_cb.service_index = 0;
	bta_dm_search_cb.services_found = 0;
	bta_dm_search_cb.peer_name[0] = 0;

	BTM_SecAddRmtNameNotifyCallback(bta_dm_remname_cback);

	bdcpy(bta_dm_search_cb.peer_bdaddr, p_data->discover.bd_addr);

	if (bta_dm_search_cb.services != 0)
	{
		bta_dm_discover_device(bta_dm_search_cb.peer_bdaddr);
	}
	else
	{
		if (btm_cb.devcb.page_timeout < BTA_DM_RNR_MAX_PAGE_TIMEOUT)
		{
			bta_sys_start_timer(&bta_dm_search_cb.rnr_timer,
			                    BTA_DM_REMT_NAME_RESP_EVT,
			                    btm_cb.devcb.page_timeout
			                    + BTA_DM_RNR_TIMER_SLACK);
		}

		if (BTM_ReadRemoteDeviceName(bta_dm_search_cb.peer_bdaddr,
		                             bta_dm_disc_name_cback) != BTM_CMD_STARTED)
		{
			bta_sys_stop_timer(&bta_dm_search_cb.rnr_timer);

			if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
			{
				bdcpy(p_msg->rem_name.result.disc_res.bd_addr,
				      bta_dm_search_cb.peer_bdaddr);
				p_msg->rem_name.result.disc_res.bd_name[0] = 0;

				p_msg->hdr.event = BTA_DM_REMT_NAME_EVT;

				bta_sys_sendmsg(p_msg);
			}
		}
	}
}

void bta_dm_inq_cmpl(tBTA_DM_MSG *p_data)
{
	tBTA_DM_SEARCH data;
	tBTA_DM_SEARCH result;
	UINT8 *p_bd;
	tBTA_DM_MSG *p_msg;
	BOOLEAN search_cmpl = TRUE;

	bta_dm_search_cb.p_btm_inq_info = btm_inq_db_first();

	if (bta_dm_search_cb.p_btm_inq_info != NULL)
	{
		search_cmpl = FALSE;

		if (bta_dm_search_cb.services != 0)
		{
			if (BTM_IsAclConnectionUp(
					bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr))
			{
				bta_dm_search_cb.wait_disc = FALSE;
			}
			else
			{
				bta_dm_search_cb.wait_disc = TRUE;
			}

			bta_dm_search_cb.sdp_results = FALSE;

			BTM_SecAddRmtNameNotifyCallback(bta_dm_remname_cback);

			bta_dm_search_cb.service_index = 0;
			bta_dm_search_cb.services_found = 0;
			bta_dm_search_cb.services_to_search = bta_dm_search_cb.services;
			bta_dm_search_cb.peer_name[0] = 0;

			bdcpy(bta_dm_search_cb.peer_bdaddr,
			      bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);

			bta_dm_discover_device(
				bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);
		}
		else
		{
			search_cmpl = TRUE;

			while (bta_dm_search_cb.p_btm_inq_info != NULL)
			{
				if (bta_dm_search_cb.p_btm_inq_info->appl_knows_rem_name)
				{
					bdcpy(result.disc_res.bd_addr,
					      bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);
					result.disc_res.bd_name[0] = 0;
					result.disc_res.services = 0;

					(*bta_dm_search_cb.p_search_cback)(BTA_DM_DISC_RES_EVT,
					                                   &result);
				}
				else
				{
					if (btm_cb.devcb.page_timeout < BTA_DM_RNR_MAX_PAGE_TIMEOUT)
					{
						bta_sys_start_timer(&bta_dm_search_cb.rnr_timer,
						                    BTA_DM_REMT_NAME_RESP_EVT,
						                    btm_cb.devcb.page_timeout
						                    + BTA_DM_RNR_TIMER_SLACK);
					}

					if (BTM_ReadRemoteDeviceName(
							bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr,
							bta_dm_inq_name_cback) != BTM_CMD_STARTED)
					{
						bta_sys_stop_timer(&bta_dm_search_cb.rnr_timer);

						p_bd = bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr;

						if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(
								 sizeof(tBTA_DM_MSG))) != NULL)
						{
							bdcpy(p_msg->rem_name.result.disc_res.bd_addr, p_bd);
							p_msg->rem_name.result.disc_res.bd_name[0] = 0;

							p_msg->hdr.event = BTA_DM_REMT_NAME_EVT;

							bta_sys_sendmsg(p_msg);
						}
					}

					search_cmpl = FALSE;
					break;
				}

				bta_dm_search_cb.p_btm_inq_info =
					btm_inq_db_next(bta_dm_search_cb.p_btm_inq_info);
			}
		}
	}

	if (search_cmpl)
	{
		bta_dm_search_cb.services = 0;

		if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
		{
			p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;

			bta_sys_sendmsg(p_msg);
		}
	}

	data.inq_cmpl.num_resps = p_data->inq_cmpl.num;

	(*bta_dm_search_cb.p_search_cback)(BTA_DM_INQ_CMPL_EVT, &data);
}

void bta_dm_rmt_name(tBTA_DM_MSG *p_data)
{
	tBTA_DM_SEARCH result;
	UINT8 *p_bd;
	tBTA_DM_MSG *p_msg;
	BOOLEAN search_cmpl = TRUE;

	while ((bta_dm_search_cb.p_btm_inq_info =
	            btm_inq_db_next(bta_dm_search_cb.p_btm_inq_info)) != NULL)
	{
		if (bta_dm_search_cb.p_btm_inq_info->appl_knows_rem_name)
		{
			bdcpy(result.disc_res.bd_addr,
			      bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);
			result.disc_res.bd_name[0] = 0;
			result.disc_res.services = 0;

			(*bta_dm_search_cb.p_search_cback)(BTA_DM_DISC_RES_EVT, &result);
		}
		else
		{
			if (btm_cb.devcb.page_timeout < BTA_DM_RNR_MAX_PAGE_TIMEOUT)
			{
				bta_sys_start_timer(&bta_dm_search_cb.rnr_timer,
				                    BTA_DM_REMT_NAME_RESP_EVT,
				                    btm_cb.devcb.page_timeout
				                    + BTA_DM_RNR_TIMER_SLACK);
			}

			if (BTM_ReadRemoteDeviceName(
					bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr,
					bta_dm_inq_name_cback) != BTM_CMD_STARTED)
			{
				bta_sys_stop_timer(&bta_dm_search_cb.rnr_timer);

				p_bd = bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr;

				if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG)))
				    != NULL)
				{
					bdcpy(p_msg->rem_name.result.disc_res.bd_addr, p_bd);
					p_msg->rem_name.result.disc_res.bd_name[0] = 0;

					p_msg->hdr.event = BTA_DM_REMT_NAME_EVT;

					bta_sys_sendmsg(p_msg);
				}
			}

			search_cmpl = FALSE;
			break;
		}
	}

	if (search_cmpl)
	{
		if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
		{
			p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;

			bta_sys_sendmsg(p_msg);
		}
	}

	(*bta_dm_search_cb.p_search_cback)(BTA_DM_DISC_RES_EVT,
	                                   &p_data->rem_name.result);
}

void bta_dm_disc_rmt_name(tBTA_DM_MSG *p_data)
{
	tBTA_DM_MSG *p_msg;

	if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
	{
		p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;

		bta_sys_sendmsg(p_msg);
	}

	p_data->rem_name.result.disc_res.services = 0;

	(*bta_dm_search_cb.p_search_cback)(BTA_DM_DISC_RES_EVT,
	                                   &p_data->rem_name.result);
}

void bta_dm_sdp_result(tBTA_DM_MSG *p_data)
{
	tBTA_DM_MSG *p_msg;
	tSDP_DISC_REC *p_sdp_rec = NULL;
	BOOLEAN found = FALSE;
	UINT16 uuid;
	char *p_name;

	if (p_data->sdp_event.sdp_result == SDP_SUCCESS
	    || p_data->sdp_event.sdp_result == SDP_NO_RECS_MATCH
	    || p_data->sdp_event.sdp_result == SDP_DB_FULL)
	{
		uuid = bta_service_id_to_uuid_lkup_tbl[bta_dm_search_cb.service_index - 1];

		if (p_data->sdp_event.sdp_result == SDP_DB_FULL
		    || (p_sdp_rec = SDP_FindServiceInDb(bta_dm_search_cb.p_sdp_db, uuid,
		                                        NULL)) != NULL)
		{
			if (uuid == UUID_SERVCLASS_PNP_INFORMATION)
			{
				if (p_sdp_rec != NULL
				    && SDP_FindAttributeInRec(p_sdp_rec, BTA_DM_ATTR_ID_VENDOR))
				{
					found = TRUE;
				}
			}
			else
			{
				found = TRUE;
			}

			if (found)
			{
				bta_dm_search_cb.services_found |= BTA_SERVICE_ID_TO_SERVICE_MASK(
					bta_dm_search_cb.service_index - 1);
			}
		}

		GKI_freebuf(bta_dm_search_cb.p_sdp_db);
		bta_dm_search_cb.p_sdp_db = NULL;

		if (bta_dm_search_cb.services_to_search != 0)
		{
			bta_dm_discover_device(bta_dm_search_cb.peer_bdaddr);
		}
		else
		{
			BTM_SecDeleteRmtNameNotifyCallback(bta_dm_remname_cback);

			if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
			{
				p_msg->hdr.event = BTA_DM_DISCOVERY_RESULT_EVT;
				p_msg->disc_result.result.disc_res.result = BTA_SUCCESS;
				p_msg->disc_result.result.disc_res.services =
					bta_dm_search_cb.services_found;

				bdcpy(p_msg->disc_result.result.disc_res.bd_addr,
				      bta_dm_search_cb.peer_bdaddr);

				p_name = bta_dm_search_cb.peer_name;

				if (bta_dm_search_cb.peer_name[0] == 0)
				{
					char *p_btm_name = BTM_SecReadDevName(
						bta_dm_search_cb.peer_bdaddr);

					if (p_btm_name != NULL)
						p_name = p_btm_name;
				}

				strncpy((char *)p_msg->disc_result.result.disc_res.bd_name,
				        p_name, BTA_DM_REMOTE_DEVICE_NAME_LENGTH);

				bta_sys_sendmsg(p_msg);
			}
		}
	}
	else
	{
		if (p_data->sdp_event.sdp_result == SDP_CONN_FAILED
		    || p_data->sdp_event.sdp_result == SDP_SECURITY_ERR
		    || p_data->sdp_event.sdp_result == SDP_CONN_REJECTED)
		{
			bta_dm_search_cb.wait_disc = FALSE;
		}

		GKI_freebuf(bta_dm_search_cb.p_sdp_db);
		bta_dm_search_cb.p_sdp_db = NULL;

		BTM_SecDeleteRmtNameNotifyCallback(bta_dm_remname_cback);

		{
			tBTA_DM_MSG *p_msg;
			char *p_name;

			if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
			{
				p_msg->hdr.event = BTA_DM_DISCOVERY_RESULT_EVT;
				p_msg->disc_result.result.disc_res.result = BTA_FAILURE;
				p_msg->disc_result.result.disc_res.services =
					bta_dm_search_cb.services_found;

				bdcpy(p_msg->disc_result.result.disc_res.bd_addr,
				      bta_dm_search_cb.peer_bdaddr);

				p_name = bta_dm_search_cb.peer_name;

				if (bta_dm_search_cb.peer_name[0] == 0)
				{
					char *p_btm_name = BTM_SecReadDevName(bta_dm_search_cb.peer_bdaddr);

					if (p_btm_name != NULL)
						p_name = p_btm_name;
				}

				strncpy((char *)p_msg->disc_result.result.disc_res.bd_name, p_name,
				        BTA_DM_REMOTE_DEVICE_NAME_LENGTH);

				bta_sys_sendmsg(p_msg);
			}
		}
	}
}

void bta_dm_search_cmpl(tBTA_DM_MSG *p_data)
{
	(*bta_dm_search_cb.p_search_cback)(BTA_DM_DISC_CMPL_EVT, NULL);
}

void bta_dm_disc_result(tBTA_DM_MSG *p_data)
{
	tBTA_DM_MSG *p_msg;

	(*bta_dm_search_cb.p_search_cback)(BTA_DM_DISC_RES_EVT,
	                                   &p_data->disc_result.result);

	if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
	{
		p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;

		bta_sys_sendmsg(p_msg);
	}
}

void bta_dm_search_result(tBTA_DM_MSG *p_data)
{
	if (p_data->disc_result.result.disc_res.services != 0)
	{
		(*bta_dm_search_cb.p_search_cback)(BTA_DM_DISC_RES_EVT,
		                                   &p_data->disc_result.result);
	}

	if (bta_dm_search_cb.wait_disc == FALSE)
	{
		bta_dm_search_cb.wait_disc = FALSE;

		bta_dm_discover_next_device();
	}
	else
	{
		bta_dm_search_cb.sdp_results = TRUE;

		bta_dm_search_cb.search_timer.p_cback =
			(TIMER_CBACK *)bta_dm_search_timer_cback;
		bta_sys_start_timer(&bta_dm_search_cb.search_timer, 0,
		                    BTA_DM_SEARCH_TIMER_MS);
	}
}

static void bta_dm_search_timer_cback(UINT32 param)
{
	APPL_TRACE(EVENT, " bta_dm_search_timer_cback  ");

	bta_dm_search_cb.wait_disc = FALSE;

	bta_dm_discover_next_device();
}

void bta_dm_free_sdp_db(tBTA_DM_MSG *p_data)
{
	if (bta_dm_search_cb.p_sdp_db)
	{
		GKI_freebuf(bta_dm_search_cb.p_sdp_db);
		bta_dm_search_cb.p_sdp_db = NULL;
	}
}

void bta_dm_queue_search(tBTA_DM_MSG *p_data)
{
	bta_dm_search_cb.p_search_queue =
		(tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_API_SEARCH));

	memcpy(bta_dm_search_cb.p_search_queue, p_data, sizeof(tBTA_DM_API_SEARCH));
}

void bta_dm_queue_disc(tBTA_DM_MSG *p_data)
{
	bta_dm_search_cb.p_search_queue =
		(tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_API_DISCOVER));

	memcpy(bta_dm_search_cb.p_search_queue, p_data,
	       sizeof(tBTA_DM_API_DISCOVER));
}

void bta_dm_search_clear_queue(tBTA_DM_MSG *p_data)
{
	if (bta_dm_search_cb.p_search_queue)
	{
		GKI_freebuf(bta_dm_search_cb.p_search_queue);
		bta_dm_search_cb.p_search_queue = NULL;
	}
}

void bta_dm_search_cancel_cmpl(tBTA_DM_MSG *p_data)
{
	if (bta_dm_search_cb.p_search_queue)
	{
		bta_sys_sendmsg(bta_dm_search_cb.p_search_queue);
		bta_dm_search_cb.p_search_queue = NULL;
	}
}

void bta_dm_search_cancel_transac_cmpl(tBTA_DM_MSG *p_data)
{
	if (bta_dm_search_cb.p_sdp_db)
	{
		GKI_freebuf(bta_dm_search_cb.p_sdp_db);
		bta_dm_search_cb.p_sdp_db = NULL;
	}

	(*bta_dm_search_cb.p_search_cback)(BTA_DM_SEARCH_CANCEL_CMPL_EVT, NULL);
}

void bta_dm_search_cancel_notify(tBTA_DM_MSG *p_data)
{
	(*bta_dm_search_cb.p_search_cback)(BTA_DM_SEARCH_CANCEL_CMPL_EVT, NULL);
}

static void bta_dm_discover_device(BD_ADDR remote_bd_addr)
{
	tSDP_UUID uuid;
	UINT16 attr_list[2] = {ATTR_ID_SERVICE_CLASS_ID_LIST, BTA_DM_ATTR_ID_VENDOR};
	UINT16 num_attrs = 1;
	char *p_name;
	tBTA_DM_MSG *p_msg;

	while (bta_dm_search_cb.service_index < BTA_MAX_SERVICE_ID)
	{
		if (bta_dm_search_cb.services_to_search
		    & (UINT32)(1 << bta_dm_search_cb.service_index))
		{
			if ((bta_dm_search_cb.p_sdp_db =
			         (tSDP_DISCOVERY_DB *)GKI_getbuf(BTA_DM_SDP_DB_SIZE))
			    != NULL)
			{
				bta_dm_search_cb.services_to_search &=
					(UINT32)(~(1 << bta_dm_search_cb.service_index));

				uuid.len = LEN_UUID_16;
				uuid.uu.uuid16 = bta_service_id_to_uuid_lkup_tbl
					[bta_dm_search_cb.service_index];

				if (uuid.uu.uuid16 == UUID_SERVCLASS_PNP_INFORMATION)
					num_attrs = 2;

				SDP_InitDiscoveryDb(bta_dm_search_cb.p_sdp_db, BTA_DM_SDP_DB_SIZE,
				                    1, &uuid, num_attrs, attr_list);

				if (!SDP_ServiceSearchAttributeRequest(
						remote_bd_addr, bta_dm_search_cb.p_sdp_db,
						bta_dm_sdp_callback))
				{
					GKI_freebuf(bta_dm_search_cb.p_sdp_db);
					bta_dm_search_cb.p_sdp_db = NULL;

					bta_dm_search_cb.service_index = BTA_MAX_SERVICE_ID;
				}
				else
				{
					bta_dm_search_cb.service_index++;
					return;
				}
			}
		}

		bta_dm_search_cb.service_index++;
	}

	if (bta_dm_search_cb.service_index >= BTA_MAX_SERVICE_ID)
	{
		if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
		{
			p_msg->hdr.event = BTA_DM_DISCOVERY_RESULT_EVT;
			p_msg->disc_result.result.disc_res.services =
				bta_dm_search_cb.services_found;

			bdcpy(p_msg->disc_result.result.disc_res.bd_addr,
			      bta_dm_search_cb.peer_bdaddr);

			p_name = bta_dm_search_cb.peer_name;

			if (bta_dm_search_cb.peer_name[0] == 0)
			{
				char *p_btm_name = BTM_SecReadDevName(bta_dm_search_cb.peer_bdaddr);

				if (p_btm_name != NULL)
					p_name = p_btm_name;
			}

			strncpy((char *)p_msg->disc_result.result.disc_res.bd_name, p_name,
			        BTA_DM_REMOTE_DEVICE_NAME_LENGTH);

			bta_sys_sendmsg(p_msg);
		}
	}
}

static void bta_dm_discover_next_device(void)
{
	tBTA_DM_MSG *p_msg;

	if ((bta_dm_search_cb.p_btm_inq_info =
	         btm_inq_db_next(bta_dm_search_cb.p_btm_inq_info)) != NULL)
	{
		if (BTM_IsAclConnectionUp(
				bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr))
		{
			bta_dm_search_cb.wait_disc = FALSE;
		}
		else
		{
			bta_dm_search_cb.wait_disc = TRUE;
		}

		bta_dm_search_cb.sdp_results = FALSE;

		BTM_SecAddRmtNameNotifyCallback(bta_dm_remname_cback);

		bta_dm_search_cb.service_index = 0;
		bta_dm_search_cb.services_found = 0;
		bta_dm_search_cb.peer_name[0] = 0;
		bta_dm_search_cb.services_to_search = bta_dm_search_cb.services;

		bdcpy(bta_dm_search_cb.peer_bdaddr,
		      bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);

		bta_dm_discover_device(
			bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);
	}
	else
	{
		bta_dm_search_cb.services = 0;

		if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
		{
			p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;

			bta_sys_sendmsg(p_msg);
		}
	}
}

static void bta_dm_sdp_callback(UINT16 sdp_status)
{
	tBTA_DM_SDP_RESULT *p_msg;

	if ((p_msg = (tBTA_DM_SDP_RESULT *)GKI_getbuf(sizeof(tBTA_DM_SDP_RESULT)))
	    != NULL)
	{
		p_msg->hdr.event = BTA_DM_SDP_RESULT_EVT;
		p_msg->sdp_result = sdp_status;

		bta_sys_sendmsg(p_msg);
	}
}

static void bta_dm_inq_results_cb(void *p_result)
{
	tBTM_INQ_RESULTS *p_inq = (tBTM_INQ_RESULTS *)p_result;
	tBTA_DM_SEARCH result;
	tBTM_INQ_INFO *p_inq_info;

	bdcpy(result.inq_res.bd_addr, p_inq->remote_bd_addr);
	memcpy(result.inq_res.dev_class, p_inq->dev_class, DEV_CLASS_LEN);
	result.inq_res.rssi = p_inq->rssi;

	if ((p_inq_info = BTM_InqDbRead(p_inq->remote_bd_addr)) != NULL)
		result.inq_res.remt_name_not_required = FALSE;

	(*bta_dm_search_cb.p_search_cback)(BTA_DM_INQ_RES_EVT, &result);

	if (p_inq_info)
		p_inq_info->appl_knows_rem_name = result.inq_res.remt_name_not_required;
}

static void bta_dm_inq_cmpl_cb(void *p_result)
{
	tBTA_DM_MSG *p_msg;

	if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
	{
		p_msg->hdr.event = BTA_DM_INQUIRY_CMPL_EVT;
		p_msg->inq_cmpl.num = ((tBTM_INQUIRY_CMPL *)p_result)->num_resp;

		bta_sys_sendmsg(p_msg);
	}
}

static void bta_dm_remname_cback(BD_ADDR bd_addr, DEV_CLASS dc,
                                 BD_NAME bd_name)
{
	strncpy(bta_dm_search_cb.peer_name, (char *)bd_name,
	        BTA_DM_REMOTE_DEVICE_NAME_LENGTH - 1);

	BTM_SecDeleteRmtNameNotifyCallback(bta_dm_remname_cback);
}

static void bta_dm_inq_name_cback(void *p_result)
{
	tBTM_REMOTE_DEV_NAME *p_remote_name = (tBTM_REMOTE_DEV_NAME *)p_result;
	tBTA_DM_MSG *p_msg;

	bta_sys_stop_timer(&bta_dm_search_cb.rnr_timer);

	if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
	{
		bdcpy(p_msg->rem_name.result.disc_res.bd_addr,
		      bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);
		strncpy((char *)p_msg->rem_name.result.disc_res.bd_name,
		        (char *)p_remote_name->remote_bd_name,
		        BTA_DM_REMOTE_DEVICE_NAME_LENGTH);

		p_msg->hdr.event = BTA_DM_REMT_NAME_EVT;

		bta_sys_sendmsg(p_msg);
	}
}

static void bta_dm_disc_name_cback(void *p_result)
{
	tBTM_REMOTE_DEV_NAME *p_remote_name = (tBTM_REMOTE_DEV_NAME *)p_result;
	tBTA_DM_MSG *p_msg;

	bta_sys_stop_timer(&bta_dm_search_cb.rnr_timer);

	if ((p_msg = (tBTA_DM_MSG *)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL)
	{
		bdcpy(p_msg->rem_name.result.disc_res.bd_addr,
		      bta_dm_search_cb.peer_bdaddr);
		strncpy((char *)p_msg->rem_name.result.disc_res.bd_name,
		        (char *)p_remote_name->remote_bd_name,
		        BTA_DM_REMOTE_DEVICE_NAME_LENGTH);

		p_msg->hdr.event = BTA_DM_REMT_NAME_EVT;

		bta_sys_sendmsg(p_msg);
	}
}

void bta_dm_cancel_rmt_name(tBTA_DM_MSG *p_data)
{
	BTM_CancelRemoteDeviceName();
}

static UINT8 bta_dm_authorize_cback(BD_ADDR bd_addr, DEV_CLASS dev_class,
                                    tBTM_BD_NAME bd_name, UINT8 *service_name,
                                    UINT8 service_id, BOOLEAN is_originator)
{
	tBTA_DM_SEC sec_event;
	tBTA_SERVICE_ID bta_service_id = 1;

	bdcpy(sec_event.authorize.bd_addr, bd_addr);
	strncpy((char *)sec_event.authorize.bd_name, (char *)bd_name,
	        BTA_DM_REMOTE_DEVICE_NAME_LENGTH);

	for (; bta_service_id <= BTA_MAX_SERVICE_ID;
	     bta_service_id++)
	{
		if (service_id == bta_service_id_to_btm_srv_id_lkup_tbl[bta_service_id])
		{
			sec_event.authorize.service = bta_service_id;
			break;
		}
	}

	if (bta_dm_cb.p_sec_cback && bta_service_id <= BTA_MAX_SERVICE_ID)
	{
		(*bta_dm_cb.p_sec_cback)(BTA_DM_AUTHORIZE_EVT, &sec_event);

		return BTM_CMD_STARTED;
	}
	else
	{
		return BTM_NOT_AUTHORIZED;
	}
}

static void bta_dm_pinname_cback(void *p_result)
{
	tBTM_REMOTE_DEV_NAME *p_data = (tBTM_REMOTE_DEV_NAME *)p_result;
	tBTA_DM_SEC sec_event;

	bdcpy(sec_event.pin_req.bd_addr, bta_dm_cb.pin_bd_addr);
	BTA_COPY_DEVICE_CLASS(sec_event.pin_req.dev_class, bta_dm_cb.pin_dev_class);

	if (p_data && p_data->status == BTM_SUCCESS)
	{
		memcpy(sec_event.pin_req.bd_name, p_data->remote_bd_name,
		       (p_data->length < BTA_DM_REMOTE_DEVICE_NAME_LENGTH)
		           ? p_data->length
		           : BTA_DM_REMOTE_DEVICE_NAME_LENGTH);
		sec_event.pin_req.bd_name[BTA_DM_REMOTE_DEVICE_NAME_LENGTH] = 0;
	}
	else
	{
		sec_event.pin_req.bd_name[0] = 0;
	}

	(*bta_dm_cb.p_sec_cback)(BTA_DM_PIN_REQ_EVT, &sec_event);
}

static UINT8 bta_dm_pin_cback(BD_ADDR bd_addr, DEV_CLASS dev_class,
                              tBTM_BD_NAME bd_name)
{
	tBTA_DM_SEC sec_event;

	if (!bta_dm_cb.p_sec_cback)
		return BTM_NOT_AUTHORIZED;

	if (bd_name[0] == 0)
	{
		bdcpy(bta_dm_cb.pin_bd_addr, bd_addr);
		BTA_COPY_DEVICE_CLASS(bta_dm_cb.pin_dev_class, dev_class);

		if (BTM_ReadRemoteDeviceName(bd_addr, bta_dm_pinname_cback)
		    == BTM_CMD_STARTED)
		{
			return BTM_CMD_STARTED;
		}

		APPL_TRACE(WARNING,
		           " bta_dm_pin_cback() -> Failed to start Remote Name Request  ");
	}

	bdcpy(sec_event.pin_req.bd_addr, bd_addr);
	BTA_COPY_DEVICE_CLASS(sec_event.pin_req.dev_class, dev_class);
	strncpy((char *)sec_event.pin_req.bd_name, (char *)bd_name,
	        BTA_DM_REMOTE_DEVICE_NAME_LENGTH);
	sec_event.pin_req.bd_name[BTA_DM_REMOTE_DEVICE_NAME_LENGTH] = 0;

	(*bta_dm_cb.p_sec_cback)(BTA_DM_PIN_REQ_EVT, &sec_event);

	return BTM_CMD_STARTED;
}

static UINT8 bta_dm_link_key_req_cback(BD_ADDR bd_addr, LINK_KEY key)
{
	return BTM_NOT_AUTHORIZED;
}

static UINT8 bta_dm_new_link_key_cback(BD_ADDR bd_addr, DEV_CLASS dev_class,
                                       tBTM_BD_NAME bd_name, UINT8 *key,
                                       UINT8 key_type)
{
	tBTA_DM_SEC sec_event;

	bdcpy(sec_event.auth_cmpl.bd_addr, bd_addr);
	memcpy(sec_event.auth_cmpl.bd_name, bd_name,
	       BTA_DM_REMOTE_DEVICE_NAME_LENGTH - 1);
	sec_event.auth_cmpl.bd_name[BTA_DM_REMOTE_DEVICE_NAME_LENGTH - 1] = 0;
	memcpy(sec_event.auth_cmpl.key, key, LINK_KEY_LEN);
	sec_event.auth_cmpl.key_present = TRUE;
	sec_event.auth_cmpl.success = TRUE;

	if (bta_dm_cb.p_sec_cback)
		(*bta_dm_cb.p_sec_cback)(BTA_DM_AUTH_CMPL_EVT, &sec_event);

	return BTM_CMD_STARTED;
}

static UINT8 bta_dm_authentication_complete_cback(BD_ADDR bd_addr,
                                                  DEV_CLASS dev_class,
                                                  tBTM_BD_NAME bd_name,
                                                  int result)
{
	tBTA_DM_SEC sec_event;

	if (result != BTM_SUCCESS)
	{
		bdcpy(sec_event.auth_cmpl.bd_addr, bd_addr);
		memcpy(sec_event.auth_cmpl.bd_name, bd_name,
		       BTA_DM_REMOTE_DEVICE_NAME_LENGTH - 1);
		sec_event.auth_cmpl.bd_name[BTA_DM_REMOTE_DEVICE_NAME_LENGTH - 1] = 0;
		sec_event.auth_cmpl.key_present = sec_event.auth_cmpl.success = FALSE;

		if (bta_dm_cb.p_sec_cback)
			(*bta_dm_cb.p_sec_cback)(BTA_DM_AUTH_CMPL_EVT, &sec_event);
	}

	return BTM_SUCCESS;
}

static void bta_dm_local_addr_cback(void *p1)
{
	if (bta_dm_cb.p_sec_cback)
		(*bta_dm_cb.p_sec_cback)(BTA_DM_ENABLE_EVT, (tBTA_DM_SEC *)p1);
}

void bta_dm_signal_strength(tBTA_DM_MSG *p_data)
{
	if (p_data->sig_strength.start)
	{
		bta_dm_cb.signal_strength_mask = p_data->sig_strength.mask;
		bta_dm_cb.signal_strength_period = p_data->sig_strength.period;

		bta_dm_signal_strength_timer_cback(0);
	}
	else
	{
		bta_sys_stop_timer(&bta_dm_cb.signal_strength_timer);
	}
}

static void bta_dm_signal_strength_timer_cback(UINT32 param)
{
	UINT8 i;

	if (bta_dm_cb.signal_strength_mask & BTA_SIG_STRENGTH_RSSI_MASK)
	{
		for (i = 0; i < bta_dm_cb.device_list.count; i++)
		{
			BTM_ReadRSSI(bta_dm_cb.device_list.peer_device[i].peer_bdaddr,
			             bta_dm_rssi_cback);
		}
	}

	if (bta_dm_cb.signal_strength_mask & BTA_SIG_STRENGTH_LINK_QUALITY_MASK)
	{
		for (i = 0; i < bta_dm_cb.device_list.count; i++)
		{
			BTM_ReadLinkQuality(bta_dm_cb.device_list.peer_device[i].peer_bdaddr,
			                    bta_dm_link_quality_cback);
		}
	}

	if (bta_dm_cb.signal_strength_period)
	{
		bta_dm_cb.signal_strength_timer.p_cback =
			(TIMER_CBACK *)bta_dm_signal_strength_timer_cback;
		bta_sys_start_timer(&bta_dm_cb.signal_strength_timer, 0,
		                    bta_dm_cb.signal_strength_period * 1000);
	}
}

static void bta_dm_acl_change_cback(BD_ADDR p_bda, DEV_CLASS p_dc,
                                    BD_NAME p_bdn, BD_FEATURES features,
                                    BOOLEAN is_new)
{
	tBTA_DM_ACL_CHANGE *p_msg;

	if ((p_msg = (tBTA_DM_ACL_CHANGE *)GKI_getbuf(sizeof(tBTA_DM_ACL_CHANGE)))
	    != NULL)
	{
		bdcpy(p_msg->bd_addr, p_bda);
		p_msg->is_new = is_new;

		p_msg->hdr.event = BTA_DM_ACL_CHANGE_EVT;

		bta_sys_sendmsg(p_msg);
	}
}

void bta_dm_acl_change(tBTA_DM_MSG *p_data)
{
	UINT8 i;
	tBTA_DM_SEC conn;
	BOOLEAN is_new = p_data->acl_change.is_new;
	UINT8 *p_bda = p_data->acl_change.bd_addr;

	if (is_new)
	{
		for (i = 0; i < bta_dm_cb.device_list.count; i++)
		{
			if (!bdcmp(bta_dm_cb.device_list.peer_device[i].peer_bdaddr, p_bda))
				break;
		}

		if (i == bta_dm_cb.device_list.count)
		{
			bdcpy(bta_dm_cb.device_list.peer_device[bta_dm_cb.device_list.count]
			          .peer_bdaddr,
			      p_bda);

			bta_dm_cb.device_list.count++;
		}

		bta_dm_cb.device_list.peer_device[i].conn_state = BTA_DM_CONNECTED;
		bta_dm_cb.device_list.peer_device[i].pref_role = BTA_ANY_ROLE;

		bdcpy(conn.link_up.bd_addr, p_bda);

		(*bta_dm_cb.p_sec_cback)(BTA_DM_LINK_UP_EVT, &conn);
	}
	else
	{
		for (i = 0; i < bta_dm_cb.device_list.count; i++)
		{
			if (!bdcmp(bta_dm_cb.device_list.peer_device[i].peer_bdaddr, p_bda))
			{
				for (; i < bta_dm_cb.device_list.count; i++)
				{
					memcpy(&bta_dm_cb.device_list.peer_device[i],
					       &bta_dm_cb.device_list.peer_device[i + 1],
					       sizeof(tBTA_DM_PEER_DEVICE));
				}

				break;
			}
		}

		bta_dm_cb.device_list.count--;

		if (bta_dm_search_cb.wait_disc
		    && !bdcmp(bta_dm_search_cb.peer_bdaddr, p_bda))
		{
			bta_dm_search_cb.wait_disc = FALSE;

			if (bta_dm_search_cb.sdp_results)
			{
				APPL_TRACE(EVENT, " timer stopped  ");

				bta_sys_stop_timer(&bta_dm_search_cb.search_timer);

				bta_dm_discover_next_device();
			}
		}

		if (bta_dm_cb.disabling)
		{
			if (!BTM_GetNumAclLinks())
			{
				bta_dm_cb.disabling = FALSE;

				bta_sys_stop_timer(&bta_dm_cb.disable_timer);

				bta_dm_cb.disable_timer.p_cback =
					(TIMER_CBACK *)bta_dm_disable_conn_down_timer_cback;
				bta_sys_start_timer(&bta_dm_cb.disable_timer, 0,
				                    BTA_DM_DISABLE_CONN_TIMER_MS);
			}
		}

		bdcpy(conn.link_down.bd_addr, p_bda);
		conn.link_down.status = (UINT8)btm_get_acl_disc_reason_code();

		(*bta_dm_cb.p_sec_cback)(BTA_DM_LINK_DOWN_EVT, &conn);
	}
}

static void bta_dm_disable_conn_down_timer_cback(UINT32 param)
{
	(*bta_dm_cb.p_sec_cback)(BTA_DM_DISABLE_EVT, NULL);
}

static void bta_dm_rssi_cback(void *p_result)
{
	tBTM_RSSI_RESULTS *p_data = (tBTM_RSSI_RESULTS *)p_result;
	tBTA_DM_SIG_STRENGTH sig;

	if (p_data->status == BTM_SUCCESS)
	{
		bdcpy(sig.bd_addr, p_data->rem_bda);
		sig.mask = BTA_SIG_STRENGTH_RSSI_MASK;
		sig.rssi_value = p_data->rssi;

		(*bta_dm_cb.p_sec_cback)(BTA_DM_SIG_STRENGTH_EVT, (tBTA_DM_SEC *)&sig);
	}
}

static void bta_dm_link_quality_cback(void *p_result)
{
	tBTM_LINK_QUALITY_RESULTS *p_data = (tBTM_LINK_QUALITY_RESULTS *)p_result;
	tBTA_DM_SIG_STRENGTH sig;

	if (p_data->status == BTM_SUCCESS)
	{
		bdcpy(sig.bd_addr, p_data->rem_bda);
		sig.mask = BTA_SIG_STRENGTH_LINK_QUALITY_MASK;
		sig.link_quality_value = p_data->link_quality;

		(*bta_dm_cb.p_sec_cback)(BTA_DM_SIG_STRENGTH_EVT, (tBTA_DM_SEC *)&sig);
	}
}

static BOOLEAN bta_dm_l2cap_server_compress_cback(
	BD_ADDR peer_addr, signed a, signed b, signed c, signed d, signed e,
	signed f, UINT8 **p_mem_pool, UINT32 *mem_pool_size)
{
	BOOLEAN ret = FALSE;
	int i;

	for (i = 0; i < BTA_DM_NUM_COMPRESS_SRVS; i++)
	{
		if (bta_dm_compress_srvcs[i].state == BTA_COMPRESS_STATE_1
		    && !bdcmp(bta_dm_compress_srvcs[i].peer_bdaddr, peer_addr))
		{
			APPL_TRACE(EVENT, "bta_dm_l2cap_server_compress_cback, BTA ID %d",
			           bta_dm_compress_srvcs[i].id);

			ret = bta_dm_co_get_compress_memory(bta_dm_compress_srvcs[i].id,
			                                    p_mem_pool, mem_pool_size);
			break;
		}
	}

	return ret;
}

static void bta_dm_compress_cback(tBTA_SYS_CONN_STATUS status, UINT8 id,
                                  UINT8 app_id, BD_ADDR peer_addr)
{
	UINT8 i;
	UINT8 k;
	UINT8 j;
	BOOLEAN found;

	if (status == BTA_SYS_CONN_OPEN)
	{
		for (i = 1; i <= p_bta_dm_compress_cfg[0].app_id; i++)
		{
			if ((p_bta_dm_compress_cfg[i].app_id == app_id
			     || p_bta_dm_compress_cfg[i].app_id == BTA_ALL_APP_ID)
			    && p_bta_dm_compress_cfg[i].id == id
			    && p_bta_dm_compress_cfg[i].mask == 1)
			{
				found = FALSE;

				for (k = 1; k <= p_bta_dm_compress_cfg[0].app_id; k++)
				{
					if (p_bta_dm_compress_cfg[k].mask == 2)
					{
						for (j = 0; j < bta_dm_conn_srvcs.count; j++)
						{
							if (bta_dm_conn_srvcs.conn_srvc[j].id
							    == p_bta_dm_compress_cfg[k].id)
							{
								found = TRUE;
							}
						}
					}
				}

				if (!found)
				{
					bta_dm_compress_srvcs[i - 1].app_id = app_id;
					bta_dm_compress_srvcs[i - 1].id = id;
					bdcpy(bta_dm_compress_srvcs[i - 1].peer_bdaddr, peer_addr);
					bta_dm_compress_srvcs[i - 1].state = BTA_COMPRESS_STATE_1;

					APPL_TRACE(EVENT,
					           "bta_dm_compress_cback open app_id %d, BTA id %d, "
					           "state %d",
					           bta_dm_compress_srvcs[i - 1].app_id,
					           bta_dm_compress_srvcs[i - 1].id,
					           bta_dm_compress_srvcs[i - 1].state);
				}

				break;
			}
		}
	}
	else if (status == BTA_SYS_CONN_CLOSE)
	{
		for (i = 1; i <= p_bta_dm_compress_cfg[0].app_id; i++)
		{
			if ((p_bta_dm_compress_cfg[i].app_id == app_id
			     || p_bta_dm_compress_cfg[i].app_id == BTA_ALL_APP_ID)
			    && p_bta_dm_compress_cfg[i].id == id)
			{
				bta_dm_compress_srvcs[i - 1].state = BTA_COMPRESS_STATE_0;

				APPL_TRACE(EVENT,
				           "bta_dm_compress_cback close app_id %d, BTA id %d, "
				           "state %d",
				           bta_dm_compress_srvcs[i - 1].app_id,
				           bta_dm_compress_srvcs[i - 1].id,
				           bta_dm_compress_srvcs[i - 1].state);
				break;
			}
		}
	}
}

static void bta_dm_rm_cback(tBTA_SYS_CONN_STATUS status, UINT8 id,
                            UINT8 app_id, BD_ADDR peer_addr)
{
	UINT8 j;
	UINT8 i;

	if (status == BTA_SYS_CONN_OPEN)
	{
		for (j = 0; j < bta_dm_cb.device_list.count; j++)
		{
			if (!bdcmp(bta_dm_cb.device_list.peer_device[j].peer_bdaddr,
			           peer_addr))
			{
				bta_dm_cb.device_list.peer_device[j].conn_state = BTA_DM_CONNECTED;

				for (i = 1; i <= p_bta_dm_rm_cfg[0].app_id; i++)
				{
					if ((p_bta_dm_rm_cfg[i].app_id == app_id
					     || p_bta_dm_rm_cfg[i].app_id == BTA_ALL_APP_ID)
					    && p_bta_dm_rm_cfg[i].id == id)
					{
						{
							UINT8 cfg = p_bta_dm_rm_cfg[i].cfg;

							if (cfg > bta_dm_cb.device_list.peer_device[j].pref_role)
								bta_dm_cb.device_list.peer_device[j].pref_role = cfg;
						}

						break;
					}
				}

				break;
			}
		}
	}
}

void bta_dm_keep_acl(tBTA_DM_MSG *p_data)
{
	bta_dm_cb.keep_acl_on_shut_down = (UINT8)p_data->hdr.layer_specific;

	if (bta_dm_cb.keep_acl_on_shut_down)
	{
		L2CA_SetIdleTimeoutByBdAddr((UINT8 *)BT_BD_ANY, BTA_DM_L2CAP_NO_IDLE_TIMEOUT);
		L2CA_SetIdleTimeout(0, BTA_DM_L2CAP_NO_IDLE_TIMEOUT, TRUE);
	}
	else
	{
		L2CA_SetIdleTimeoutByBdAddr((UINT8 *)BT_BD_ANY, BTA_DM_L2CAP_IDLE_TIMEOUT);
		L2CA_SetIdleTimeout(0, BTA_DM_L2CAP_IDLE_TIMEOUT, TRUE);
	}
}

void bta_dm_immediate_disable(void)
{
	(*bta_dm_cb.p_sec_cback)(BTA_DM_DISABLE_EVT, NULL);
}

void bta_dm_reset_complete(void *p1)
{
}

void bta_dm_send_hci_reset(tBTA_DM_MSG *p_data)
{
	void *p_buf;

	bta_sys_cb.events_disabled = TRUE;

	if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
		btsnd_hcic_write_scan_enable(p_buf, 0);

	BTM_SendHciReset(bta_dm_reset_complete);
}
