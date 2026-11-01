#include <btm_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/btm/btm_inq.c
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

/*******************************************************************************
 * macros
 */

// HYPOTHESIS: local names for literals the headers don't spell out
#define BTM_DEFAULT_SCAN_WINDOW		0x12
#define BTM_DEFAULT_SCAN_INTERVAL	0x800
#define BTM_MIN_SCAN_VALUE			0x12
#define BTM_MAX_SCAN_VALUE			0x1000
#define BTM_INQ_REM_NAME_TIMEOUT	40
#define BTM_INQ_BD_DB_BYTES			1800

/*******************************************************************************
 * variables
 */

// 3-byte lap values that live in .sdata2
static const LAP general_inq_lap = {0x9e, 0x8b, 0x33};
static const LAP limited_inq_lap = {0x9e, 0x8b, 0x00};

/*******************************************************************************
 * functions
 */

void btm_discovery_timeout(void)
{
}

tBTM_STATUS BTM_SetDiscoverability(UINT16 inq_mode, UINT16 window,
                                   UINT16 interval)
{
	UINT8 scan_mode = 0;
	void *p_buf;
	UINT8 *p_cod;
	UINT16 service;
	UINT8 major, minor;
	DEV_CLASS cod;
	BOOLEAN is_limited, cod_limited;
	LAP temp_lap[2];

	if (inq_mode != BTM_NON_DISCOVERABLE && inq_mode != BTM_LIMITED_DISCOVERABLE
	    && inq_mode != BTM_GENERAL_DISCOVERABLE)
	{
		return BTM_ILLEGAL_VALUE;
	}

	if (btm_cb.devcb.state < BTM_DEV_STATE_3)
		return BTM_DEV_RESET;

	if (!window)
		window = BTM_DEFAULT_SCAN_WINDOW;

	if (!interval)
		interval = BTM_DEFAULT_SCAN_INTERVAL;

	BTM_TRACE(API, "BTM_SetDiscoverability: mode %d [NonDisc-0, Lim-1, Gen-2], "
	          "window 0x%04x, interval 0x%04x", inq_mode, window, interval);

	if (inq_mode != BTM_NON_DISCOVERABLE
	    && (window < BTM_MIN_SCAN_VALUE || window > BTM_MAX_SCAN_VALUE
	        || interval < BTM_MIN_SCAN_VALUE || interval > BTM_MAX_SCAN_VALUE
	        || window > interval))
	{
		return BTM_ILLEGAL_VALUE;
	}

	if (inq_mode != BTM_NON_DISCOVERABLE)
	{
		if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
		{
			if (inq_mode & BTM_LIMITED_DISCOVERABLE)
			{
				memcpy(&temp_lap[0], limited_inq_lap, LAP_LEN);
				memcpy(&temp_lap[1], general_inq_lap, LAP_LEN);

				btsnd_hcic_write_cur_iac_lap(p_buf, 2, temp_lap);
			}
			else
			{
				btsnd_hcic_write_cur_iac_lap(p_buf, 1,
				                             (LAP *)&general_inq_lap);
			}

			scan_mode |= HCI_INQUIRY_SCAN_ENABLED;
		}
		else
		{
			return BTM_NO_RESOURCES;
		}
	}

	if (window != btm_cb.btm_inq_vars.inq_scan_window
	    || interval != btm_cb.btm_inq_vars.inq_scan_period)
	{
		if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
		{
			btm_cb.btm_inq_vars.inq_scan_window = window;
			btm_cb.btm_inq_vars.inq_scan_period = interval;

			btsnd_hcic_write_inqscan_cfg(p_buf, interval, window);
		}
		else
		{
			return BTM_NO_RESOURCES;
		}
	}

	if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
	{
		if (btm_cb.btm_inq_vars.connectable_mode != BTM_NON_CONNECTABLE)
			scan_mode |= HCI_PAGE_SCAN_ENABLED;

		btm_cb.btm_inq_vars.discoverable_mode = inq_mode;
		btsnd_hcic_write_scan_enable(p_buf, scan_mode);
	}
	else
	{
		return BTM_NO_RESOURCES;
	}

	p_cod = BTM_ReadDeviceClass();
	BTM_GET_COD_SERVICE_CLASS(p_cod, &service);

	is_limited = (inq_mode & BTM_LIMITED_DISCOVERABLE) ? TRUE : FALSE;
	cod_limited = (service & BTM_COD_SERVICE_LMTD_DISCOVER) ? TRUE : FALSE;

	if (is_limited ^ cod_limited)
	{
		BTM_GET_COD_MINOR_CLASS(p_cod, &minor);
		BTM_GET_COD_MAJOR_CLASS(p_cod, &major);

		if (is_limited)
			service |= BTM_COD_SERVICE_LMTD_DISCOVER;
		else
			service &= ~BTM_COD_SERVICE_LMTD_DISCOVER;

		MAKE_COD_FROM_FIELDS(cod, minor, major, service);
		BTM_SetDeviceClass(cod);
	}

	return BTM_SUCCESS;
}

tBTM_STATUS BTM_SetInquiryScanType(UINT16 scan_type)
{
	void *p_buf;

	if (scan_type != BTM_SCAN_TYPE_STANDARD
	    && scan_type != BTM_SCAN_TYPE_INTERLACED)
	{
		return BTM_ILLEGAL_VALUE;
	}

	if (!HCI_LMP_INTERLACED_INQ_SCAN_SUPPORTED(btm_cb.devcb.local_features))
		return BTM_MODE_UNSUPPORTED;

	if (scan_type != btm_cb.btm_inq_vars.inq_scan_type)
	{
		if (BTM_IsDeviceUp())
		{
			if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
			{
				btsnd_hcic_write_inqscan_type(p_buf, scan_type);
				btm_cb.btm_inq_vars.inq_scan_type = scan_type;
			}
			else
			{
				return BTM_NO_RESOURCES;
			}
		}
		else
		{
			return BTM_WRONG_MODE;
		}
	}

	return BTM_SUCCESS;
}

tBTM_STATUS BTM_SetPageScanType(UINT16 scan_type)
{
	void *p_buf;

	if (scan_type != BTM_SCAN_TYPE_STANDARD
	    && scan_type != BTM_SCAN_TYPE_INTERLACED)
	{
		return BTM_ILLEGAL_VALUE;
	}

	if (!HCI_LMP_INTERLACED_PAGE_SCAN_SUPPORTED(btm_cb.devcb.local_features))
		return BTM_MODE_UNSUPPORTED;

	if (scan_type != btm_cb.btm_inq_vars.page_scan_type)
	{
		if (BTM_IsDeviceUp())
		{
			if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
			{
				btsnd_hcic_write_pagescan_type(p_buf, scan_type);
				btm_cb.btm_inq_vars.page_scan_type = scan_type;
			}
			else
			{
				return BTM_NO_RESOURCES;
			}
		}
		else
		{
			return BTM_WRONG_MODE;
		}
	}

	return BTM_SUCCESS;
}

tBTM_STATUS BTM_SetInquiryMode(UINT8 mode)
{
	void *p_buf;

	if (mode != BTM_INQ_RESULT_STANDARD && mode != BTM_INQ_RESULT_WITH_RSSI)
		return BTM_ILLEGAL_VALUE;

	if (!HCI_LMP_INQ_RSSI_SUPPORTED(btm_cb.devcb.local_features))
		return BTM_MODE_UNSUPPORTED;

	if (!BTM_IsDeviceUp())
		return BTM_WRONG_MODE;

	if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
		btsnd_hcic_write_inquiry_mode(p_buf, mode);
	else
		return BTM_NO_RESOURCES;

	return BTM_SUCCESS;
}

tBTM_STATUS BTM_SetConnectability(UINT16 page_mode, UINT16 window,
                                  UINT16 interval)
{
	tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
	UINT8 scan_mode = 0;
	void *p_buf;

	if (page_mode != BTM_NON_CONNECTABLE && page_mode != BTM_CONNECTABLE)
		return BTM_ILLEGAL_VALUE;

	if (btm_cb.devcb.state < BTM_DEV_STATE_3)
		return BTM_DEV_RESET;

	if (!window)
		window = BTM_DEFAULT_SCAN_WINDOW;

	if (!interval)
		interval = BTM_DEFAULT_SCAN_INTERVAL;

	BTM_TRACE(API, "BTM_SetConnectability: mode %d [NonConn-0, Conn-1], "
	          "window 0x%04x, interval 0x%04x", page_mode, window, interval);

	if (page_mode == BTM_CONNECTABLE)
	{
		if (window < BTM_MIN_SCAN_VALUE || window > BTM_MAX_SCAN_VALUE
		    || interval < BTM_MIN_SCAN_VALUE || interval > BTM_MAX_SCAN_VALUE
		    || window > interval)
		{
			return BTM_ILLEGAL_VALUE;
		}

		scan_mode |= HCI_PAGE_SCAN_ENABLED;
	}

	if (window != p_inq->page_scan_window || interval != p_inq->page_scan_period)
	{
		if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
		{
			p_inq->page_scan_window = window;
			p_inq->page_scan_period = interval;

			btsnd_hcic_write_pagescan_cfg(p_buf, interval, window);
		}
		else
		{
			return BTM_NO_RESOURCES;
		}
	}

	if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
	{
		if (p_inq->discoverable_mode != BTM_NON_DISCOVERABLE)
			scan_mode |= HCI_INQUIRY_SCAN_ENABLED;

		p_inq->connectable_mode = page_mode;
		btsnd_hcic_write_scan_enable(p_buf, scan_mode);
	}
	else
	{
		return BTM_NO_RESOURCES;
	}

	return BTM_SUCCESS;
}

UINT16 BTM_IsInquiryActive(void)
{
	return btm_cb.btm_inq_vars.inq_active;
}

static void btm_clr_inq_result_flt(void)
{
	tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;

	if (p_inq->p_bd_db)
	{
		GKI_freebuf(p_inq->p_bd_db);
		p_inq->p_bd_db = NULL;
	}

	p_inq->num_bd_entries = 0;
	p_inq->max_bd_entries = 0;
}

tBTM_STATUS BTM_CancelInquiry(void)
{
	tBTM_STATUS status = BTM_SUCCESS;
	tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;

	BTM_TRACE(API, "BTM_CancelInquiry called");

	if (!BTM_IsDeviceUp())
		return BTM_WRONG_MODE;

	if (p_inq->inq_active != BTM_INQUIRY_INACTIVE
	    && !(p_inq->inq_active & BTM_PERIODIC_INQUIRY_ACTIVE))
	{
		p_inq->inq_active = BTM_INQUIRY_INACTIVE;
		p_inq->state = BTM_INQ_INACTIVE_STATE;
		p_inq->p_inq_results_cb = NULL;
		p_inq->p_inq_cmpl_cb = NULL;

		if (p_inq->inqfilt_active)
		{
			p_inq->inqfilt_active = FALSE;
			p_inq->pending_filt_complete_event++;
		}
		else
		{
			if (!btsnd_hcic_inq_cancel())
				status = BTM_NO_RESOURCES;
		}

		p_inq->inq_counter++;
		btm_clr_inq_result_flt();
	}

	return status;
}

static UINT8 btm_set_inq_event_filter(UINT8 filter_cond_type,
                                      tBTM_INQ_FILT_COND *p_filt_cond);

tBTM_STATUS BTM_StartInquiry(tBTM_INQ_PARMS *p_inqparms,
                             tBTM_INQ_RESULTS_CB *p_results_cb,
                             tBTM_CMPL_CB *p_cmpl_cb)
{
	tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
	tBTM_STATUS status;

	BTM_TRACE(API, "BTM_StartInquiry: mode: %d, dur: %d, rsps: %d, flt: %d",
	          p_inqparms->mode, p_inqparms->duration, p_inqparms->max_resps,
	          p_inqparms->filter_cond_type);

	if (p_inq->inq_active || p_inq->inqfilt_active)
		return BTM_BUSY;

	if (p_inqparms->mode != BTM_GENERAL_INQUIRY
	    && p_inqparms->mode != BTM_LIMITED_INQUIRY)
	{
		return BTM_ILLEGAL_VALUE;
	}

	if (!BTM_IsDeviceUp())
		return BTM_WRONG_MODE;

	p_inq->inqparms = *p_inqparms;
	p_inq->state = BTM_INQ_ACTIVE_STATE;
	p_inq->p_inq_cmpl_cb = p_cmpl_cb;
	p_inq->p_inq_results_cb = p_results_cb;
	p_inq->inq_cmpl_info.num_resp = 0;

	p_inq->inq_active = (UINT8)((p_inqparms->mode == BTM_LIMITED_INQUIRY)
	                            ? BTM_LIMITED_INQUIRY_ACTIVE
	                            : BTM_GENERAL_INQUIRY_ACTIVE);

	switch (p_inqparms->filter_cond_type)
	{
	case BTM_CLR_INQUIRY_FILTER:
		p_inq->state = BTM_INQ_SET_FILT_STATE;
		break;

	case BTM_FILTER_COND_DEVICE_CLASS:
	case BTM_FILTER_COND_BD_ADDR:
		p_inq->state = BTM_INQ_CLR_FILT_STATE;
		p_inqparms->filter_cond_type = BTM_CLR_INQUIRY_FILTER;
		break;

	default:
		return BTM_ILLEGAL_VALUE;
	}

	status = btm_set_inq_event_filter(p_inqparms->filter_cond_type,
	                                  &p_inqparms->filter_cond);
	if (status != BTM_CMD_STARTED)
		p_inq->state = BTM_INQ_INACTIVE_STATE;

	return status;
}

static inline tINQ_DB_ENT *btm_inq_db_find(BD_ADDR p_bda)
{
	UINT16 xx;
	tINQ_DB_ENT *p_ent = btm_cb.btm_inq_vars.inq_db;

	for (xx = 0; xx < BTM_INQ_DB_SIZE; xx++, p_ent++)
	{
		if (p_ent->in_use
		    && !memcmp(p_ent->inq_info.results.remote_bd_addr, p_bda,
		               BD_ADDR_LEN))
		{
			return p_ent;
		}
	}

	return NULL;
}

tBTM_STATUS BTM_ReadRemoteDeviceName(BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb)
{
	tBTM_INQ_INFO *p_cur = NULL;
	tINQ_DB_ENT *p_i;

	BTM_TRACE(API, "BTM_ReadRemoteDeviceName: bd addr [%02x%02x%02x%02x%02x%02x]",
	          remote_bda[0], remote_bda[1], remote_bda[2], remote_bda[3],
	          remote_bda[4], remote_bda[5]);

	p_i = btm_inq_db_find(remote_bda);
	if (p_i)
		p_cur = &p_i->inq_info;

	return btm_initiate_rem_name(remote_bda, p_cur, BTM_RMT_NAME_EXT,
	                             BTM_INQ_REM_NAME_TIMEOUT, p_cb);
}

tBTM_STATUS BTM_CancelRemoteDeviceName(void)
{
	tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;

	BTM_TRACE(API, "BTM_CancelRemoteDeviceName()");

	if (p_inq->remname_active & BTM_RMT_NAME_EXT)
	{
		if (btsnd_hcic_rmt_name_req_cancel(p_inq->remname_bda))
			return BTM_CMD_STARTED;
		else
			return BTM_NO_RESOURCES;
	}
	else
	{
		return BTM_WRONG_MODE;
	}
}

tBTM_INQ_INFO *BTM_InqDbRead(BD_ADDR p_bda)
{
	UINT16 xx;
	tINQ_DB_ENT *p_ent = btm_cb.btm_inq_vars.inq_db;

	BTM_TRACE(API, "BTM_InqDbRead: bd addr [%02x%02x%02x%02x%02x%02x]",
	          p_bda[0], p_bda[1], p_bda[2], p_bda[3], p_bda[4], p_bda[5]);

	for (xx = 0; xx < BTM_INQ_DB_SIZE; xx++, p_ent++)
	{
		if (p_ent->in_use
		    && !memcmp(p_ent->inq_info.results.remote_bd_addr, p_bda,
		               BD_ADDR_LEN))
		{
			return &p_ent->inq_info;
		}
	}

	return NULL;
}

tBTM_INQ_INFO *btm_inq_db_first(void)
{
	tINQ_DB_ENT *p_ent = btm_cb.btm_inq_vars.inq_db;
	UINT16 xx;

	for (xx = 0; xx < BTM_INQ_DB_SIZE; xx++, p_ent++)
	{
		if (p_ent->in_use)
			return &p_ent->inq_info;
	}

	return NULL;
}

tBTM_INQ_INFO *btm_inq_db_next(tBTM_INQ_INFO *p_cur)
{
	tINQ_DB_ENT *p_ent;
	UINT16 inx;

	if (p_cur)
	{
		p_ent = (tINQ_DB_ENT *)((UINT8 *)p_cur
		                        - offsetof(tINQ_DB_ENT, inq_info));
		inx = (p_ent - btm_cb.btm_inq_vars.inq_db) + 1;

		for (p_ent = &btm_cb.btm_inq_vars.inq_db[inx]; inx < BTM_INQ_DB_SIZE;
		     inx++, p_ent++)
		{
			if (p_ent->in_use)
				return &p_ent->inq_info;
		}

		return NULL;
	}
	else
	{
		return btm_inq_db_first();
	}
}

tBTM_STATUS BTM_ClearInqDb(BD_ADDR p_bda)
{
	UINT16 xx;
	tINQ_DB_ENT *p_ent;

	if (btm_cb.btm_inq_vars.inq_active != BTM_INQUIRY_INACTIVE
	    || btm_cb.btm_inq_vars.remname_active != BTM_RMT_NAME_INACTIVE
	    || btm_cb.btm_inq_vars.inqfilt_active)
	{
		return BTM_BUSY;
	}

	for (xx = 0, p_ent = btm_cb.btm_inq_vars.inq_db; xx < BTM_INQ_DB_SIZE; xx++, p_ent++)
	{
		if (p_ent->in_use)
		{
			if (!p_bda
			    || !memcmp(p_ent->inq_info.results.remote_bd_addr, p_bda,
			               BD_ADDR_LEN))
			{
				p_ent->in_use = FALSE;

				if (btm_cb.btm_inq_vars.p_inq_change_cb)
					(*btm_cb.btm_inq_vars.p_inq_change_cb)(&p_ent->inq_info, FALSE);
			}
		}
	}

	return BTM_SUCCESS;
}

void btm_inq_db_reset(void)
{
	tBTM_REMOTE_DEV_NAME rem_name;
	tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
	UINT16 xx;
	tINQ_DB_ENT *p_ent;
	UINT8 inq_status;
	UINT8 filt_status;
	UINT8 active;

	btu_stop_timer(&p_inq->inq_timer_ent);

	if (p_inq->inq_active != BTM_INQUIRY_INACTIVE)
	{
		active = p_inq->inq_active;
		p_inq->inq_active = BTM_INQUIRY_INACTIVE;

		if (active == BTM_LIMITED_INQUIRY_ACTIVE
		    || active == BTM_GENERAL_INQUIRY_ACTIVE)
		{
			if (p_inq->p_inq_cmpl_cb)
			{
				inq_status = BTM_SUCCESS;
				(*p_inq->p_inq_cmpl_cb)(&inq_status);
			}
		}
	}

	if (p_inq->remname_active & BTM_RMT_NAME_EXT)
	{
		btu_stop_timer(&p_inq->rmt_name_timer_ent);
		p_inq->remname_active = BTM_RMT_NAME_INACTIVE;
		memset(p_inq->remname_bda, 0, BD_ADDR_LEN);

		if (p_inq->p_remname_cmpl_cb)
		{
			rem_name.status = BTM_DEV_RESET;
			(*p_inq->p_remname_cmpl_cb)(&rem_name);
			p_inq->p_remname_cmpl_cb = NULL;
		}
	}

	if (p_inq->inqfilt_active)
	{
		p_inq->inqfilt_active = FALSE;

		if (p_inq->p_inqfilter_cmpl_cb)
		{
			filt_status = BTM_DEV_RESET;
			(*p_inq->p_inqfilter_cmpl_cb)(&filt_status);
		}
	}

	p_inq->state = BTM_INQ_INACTIVE_STATE;
	p_inq->pending_filt_complete_event = 0;
	p_inq->p_inq_results_cb = NULL;

	p_ent = btm_cb.btm_inq_vars.inq_db;
	for (xx = 0; xx < BTM_INQ_DB_SIZE; xx++, p_ent++)
	{
		if (p_ent->in_use)
		{
			p_ent->in_use = FALSE;

			if (btm_cb.btm_inq_vars.p_inq_change_cb)
				(*btm_cb.btm_inq_vars.p_inq_change_cb)(&p_ent->inq_info, FALSE);
		}
	}

	btm_clr_inq_result_flt();

	p_inq->discoverable_mode = BTM_NON_DISCOVERABLE;
	p_inq->connectable_mode = BTM_NON_CONNECTABLE;
	p_inq->page_scan_type = BTM_SCAN_TYPE_STANDARD;
	p_inq->inq_scan_type = BTM_SCAN_TYPE_STANDARD;
}

void btm_inq_db_init(void)
{
}

static BOOLEAN btm_inq_find_bdaddr(BD_ADDR p_bda)
{
	tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
	tINQ_BDADDR *p_db = p_inq->p_bd_db;
	UINT16 xx;

	if ((p_inq->inq_active & BTM_PERIODIC_INQUIRY_ACTIVE) || !p_db)
		return FALSE;

	for (xx = 0; xx < p_inq->num_bd_entries; xx++, p_db++)
	{
		if (!memcmp(p_db->bd_addr, p_bda, BD_ADDR_LEN)
		    && p_db->inq_count == p_inq->inq_counter)
		{
			return TRUE;
		}
	}

	if (xx < p_inq->max_bd_entries)
	{
		p_db->inq_count = p_inq->inq_counter;
		memcpy(p_db->bd_addr, p_bda, BD_ADDR_LEN);
		p_inq->num_bd_entries++;
	}

	return FALSE;
}

static tINQ_DB_ENT *btm_inq_db_new(BD_ADDR p_bda)
{
	tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
	tINQ_DB_ENT *p_ent = p_inq->inq_db;
	tINQ_DB_ENT *p_old = p_inq->inq_db;
	UINT16 xx;
	UINT32 ot = 0xffffffff;

	for (xx = 0; xx < BTM_INQ_DB_SIZE; xx++, p_ent++)
	{
		if (!p_ent->in_use)
		{
			memset(p_ent, 0, sizeof(tINQ_DB_ENT));
			memcpy(p_ent->inq_info.results.remote_bd_addr, p_bda, BD_ADDR_LEN);
			p_ent->in_use = TRUE;

			return p_ent;
		}

		if (p_ent->time_of_resp < ot)
		{
			p_old = p_ent;
			ot = p_ent->time_of_resp;
		}
	}

	if (btm_cb.btm_inq_vars.p_inq_change_cb)
		(*btm_cb.btm_inq_vars.p_inq_change_cb)(&p_old->inq_info, FALSE);

	memset(p_old, 0, sizeof(tINQ_DB_ENT));
	memcpy(p_old->inq_info.results.remote_bd_addr, p_bda, BD_ADDR_LEN);
	p_old->in_use = TRUE;

	return p_old;
}

static UINT8 btm_set_inq_event_filter(UINT8 filter_cond_type,
                                      tBTM_INQ_FILT_COND *p_filt_cond)
{
	void *p_buf;
	UINT8 condition_length = 6;
	UINT8 condition_buf[6];
	UINT8 *p_cond = condition_buf;

	if ((p_buf = GKI_getpoolbuf(HCI_CMD_POOL_ID)) != NULL)
	{
		switch (filter_cond_type)
		{
		case BTM_FILTER_COND_DEVICE_CLASS:
			memcpy(p_cond, p_filt_cond->cod_cond.dev_class, 3);
			memcpy(&condition_buf[3], p_filt_cond->cod_cond.dev_class_mask, 3);
			break;

		case BTM_FILTER_COND_BD_ADDR:
			p_cond = p_filt_cond->bdaddr_cond;
			break;

		case BTM_CLR_INQUIRY_FILTER:
			condition_length = 0;
			break;

		default:
			return BTM_ILLEGAL_VALUE;
		}

		btm_cb.btm_inq_vars.inqfilt_active = TRUE;

		btsnd_hcic_set_event_filter(p_buf, HCI_FILTER_INQUIRY_RESULT,
		                            filter_cond_type, p_cond, condition_length);

		return BTM_CMD_STARTED;
	}
	else
	{
		return BTM_NO_RESOURCES;
	}
}

void btm_event_filter_complete(UINT8 *p)
{
	tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
	tBTM_STATUS status;
	UINT8 hci_status;
	const UINT8 *lap;
	tBTM_CMPL_CB *p_cb;

	p_cb = p_inq->p_inqfilter_cmpl_cb;

	if (p_inq->pending_filt_complete_event)
	{
		p_inq->pending_filt_complete_event--;
		return;
	}

	if (p_inq->inqfilt_active == TRUE)
	{
		hci_status = *p;

		if (hci_status != HCI_SUCCESS)
		{
			BTM_TRACE(WARNING,
			          "BTM Warning: Set Event Filter Failed (HCI returned 0x%x)",
			          hci_status);

			status = BTM_ERR_PROCESSING;
		}
		else
		{
			status = BTM_SUCCESS;
		}

		if (p_inq->state == BTM_INQ_INACTIVE_STATE)
		{
			p_inq->inqfilt_active = FALSE;

			if (p_cb)
				(*p_cb)(&status);
		}
		else if (status != BTM_SUCCESS)
		{
			btm_process_inq_complete(BTM_ERR_PROCESSING);

			p_inq->inqfilt_active = FALSE;
			p_inq->inq_active = BTM_INQUIRY_INACTIVE;
			p_inq->state = BTM_INQ_INACTIVE_STATE;
		}
		else if (p_inq->state == BTM_INQ_CLR_FILT_STATE)
		{
			status = btm_set_inq_event_filter(p_inq->inqparms.filter_cond_type,
			                                  &p_inq->inqparms.filter_cond);

			if (status == BTM_CMD_STARTED)
			{
				p_inq->state = BTM_INQ_SET_FILT_STATE;
			}
			else
			{
				p_inq->inqfilt_active = FALSE;
				btm_process_inq_complete(BTM_ERR_PROCESSING);
			}
		}
		else
		{
			p_inq->state = BTM_INQ_ACTIVE_STATE;
			p_inq->inqfilt_active = FALSE;

			p_inq->inqparms.max_resps =
				(p_inq->inqparms.max_resps > BTM_INQ_DB_SIZE)
					? BTM_INQ_DB_SIZE
					: p_inq->inqparms.max_resps;

			lap = general_inq_lap;
			if (p_inq->inq_active & BTM_LIMITED_INQUIRY_ACTIVE)
				lap = limited_inq_lap;

			if (p_inq->inq_active & BTM_PERIODIC_INQUIRY_ACTIVE)
			{
				if (!btsnd_hcic_per_inq_mode(p_inq->per_max_delay,
				                             p_inq->per_min_delay, lap,
				                             p_inq->inqparms.duration,
				                             p_inq->inqparms.max_resps))
				{
					btm_process_inq_complete(BTM_NO_RESOURCES);
				}
			}
			else
			{
				btm_clr_inq_result_flt();

				if ((p_inq->p_bd_db = (tINQ_BDADDR *)GKI_getbuf(
						 BTM_INQ_BD_DB_BYTES)) != NULL)
				{
					p_inq->max_bd_entries = (UINT16)(BTM_INQ_BD_DB_BYTES
					                                 / sizeof(tINQ_BDADDR));
					memset(p_inq->p_bd_db, 0, BTM_INQ_BD_DB_BYTES);
				}

				if (!btsnd_hcic_inquiry(lap, p_inq->inqparms.duration, 0))
					btm_process_inq_complete(BTM_NO_RESOURCES);
			}
		}
	}
}

void btm_process_inq_results(UINT8 *p, BOOLEAN with_rssi)
{
	UINT8 page_scan_per_mode;
	UINT8 page_scan_rep_mode;
	UINT8 num_resp;
	tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
	UINT8 xx;
	tINQ_DB_ENT *p_i;
	tBTM_INQ_RESULTS *p_res;
	BOOLEAN is_new = TRUE;
	tBTM_INQ_RESULTS_CB *p_inq_results_cb = p_inq->p_inq_results_cb;
	UINT8 page_scan_mode = 0;
	UINT8 rssi = 0;
	UINT16 clock_offset;
	BD_ADDR bda;
	DEV_CLASS dc;

	if (p_inq->inq_active == BTM_INQUIRY_INACTIVE)
		return;

	STREAM_TO_UINT8(p, &num_resp);

	for (xx = 0; xx < num_resp; xx++)
	{
		STREAM_TO_BDADDR(p, bda);
		STREAM_TO_UINT8(p, &page_scan_rep_mode);
		STREAM_TO_UINT8(p, &page_scan_per_mode);

		if (!with_rssi)
			STREAM_TO_UINT8(p, &page_scan_mode);

		STREAM_TO_DEVCLASS(p, dc);
		STREAM_TO_UINT16(p, &clock_offset);

		if (with_rssi)
			STREAM_TO_UINT8(p, &rssi);

		if (btm_inq_find_bdaddr(bda))
			continue;

		if (btm_cb.p_inq_filter_cb)
		{
			if (!(*btm_cb.p_inq_filter_cb)(bda, dc))
				continue;
		}

		p_i = btm_inq_db_find(bda);

		if (p_i == NULL)
		{
			p_i = btm_inq_db_new(bda);
			is_new = TRUE;
		}
		else if (p_i->inq_count == p_inq->inq_counter)
		{
			is_new = FALSE;
		}

		if (is_new == TRUE)
		{
			p_res = &p_i->inq_info.results;

			p_res->page_scan_rep_mode = page_scan_rep_mode;
			p_res->page_scan_per_mode = page_scan_per_mode;
			p_res->page_scan_mode = page_scan_mode;
			p_res->dev_class[0] = dc[0];
			p_res->dev_class[1] = dc[1];
			p_res->dev_class[2] = dc[2];
			p_res->clock_offset = clock_offset | BTM_CLOCK_OFFSET_VALID;

			if (with_rssi)
				p_res->rssi = rssi;
			else
				p_res->rssi = BTM_INQ_RES_IGNORE_RSSI;

			p_i->time_of_resp = GKI_get_tick_count();
			p_i->inq_count = p_inq->inq_counter;

			p_inq->inq_cmpl_info.num_resp++;

			if (!(p_inq->inq_active & BTM_PERIODIC_INQUIRY_ACTIVE))
			{
				if (p_inq->inqparms.max_resps
				    && p_inq->inq_cmpl_info.num_resp == p_inq->inqparms.max_resps)
				{
					btsnd_hcic_inq_cancel();
				}
			}

			p_i->inq_info.appl_knows_rem_name = FALSE;

			if (p_inq_results_cb)
				(*p_inq_results_cb)(p_res);

			if (p_inq->p_inq_change_cb)
				(*p_inq->p_inq_change_cb)(&p_i->inq_info, TRUE);
		}
	}
}

void btm_process_inq_complete(UINT8 status)
{
	tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
	tBTM_CMPL_CB *p_inq_cb = p_inq->p_inq_cmpl_cb;

	if (p_inq->inq_active != BTM_INQUIRY_INACTIVE)
	{
		p_inq->inq_counter++;

		p_inq->inq_cmpl_info.status =
			(status == HCI_SUCCESS) ? BTM_SUCCESS : BTM_ERR_PROCESSING;

		if (!(p_inq->inq_active & BTM_PERIODIC_INQUIRY_ACTIVE))
		{
			p_inq->p_inq_results_cb = NULL;
			p_inq->inq_active = BTM_INQUIRY_INACTIVE;
			p_inq->state = BTM_INQ_INACTIVE_STATE;
			p_inq->p_inq_cmpl_cb = NULL;

			btm_clr_inq_result_flt();

			BTM_TRACE(DEBUG, "BTM Inq Compl Callback: status 0x%02x, num results %d",
			          p_inq->inq_cmpl_info.status,
			          p_inq->inq_cmpl_info.num_resp);

			if (p_inq_cb)
				(*p_inq_cb)(&p_inq->inq_cmpl_info);
		}
	}
}

tBTM_STATUS btm_initiate_rem_name(BD_ADDR remote_bda, tBTM_INQ_INFO *p_cur,
                                  UINT8 origin, UINT32 timeout,
                                  tBTM_CMPL_CB *p_cb)
{
	tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
	BOOLEAN start = TRUE;
	BOOLEAN cmd_ok = TRUE;
	tBTM_STATUS status = BTM_CMD_STARTED;

	if (!BTM_IsDeviceUp())
		return BTM_WRONG_MODE;

	if (!(p_inq->remname_active & origin))
	{
		if (origin == BTM_RMT_NAME_EXT)
		{
			if (p_inq->remname_active)
			{
				if (!memcmp(remote_bda, p_inq->remname_bda, BD_ADDR_LEN))
					start = FALSE;
				else
					return BTM_BUSY;
			}

			p_inq->p_remname_cmpl_cb = p_cb;
		}
		else
		{
			if (p_inq->remname_active)
				return BTM_BUSY;
		}

		if (start)
		{
			memcpy(p_inq->remname_bda, remote_bda, BD_ADDR_LEN);
			btu_start_timer(&p_inq->rmt_name_timer_ent,
			                BTU_TTYPE_BTM_RMT_NAME, timeout);

			if (p_cur)
			{
				cmd_ok = btsnd_hcic_rmt_name_req(
					remote_bda, p_cur->results.page_scan_rep_mode,
					p_cur->results.page_scan_mode,
					(UINT16)(p_cur->results.clock_offset
					         | BTM_CLOCK_OFFSET_VALID));
			}
			else
			{
				cmd_ok = btsnd_hcic_rmt_name_req(
					remote_bda, HCI_PAGE_SCAN_REP_MODE_R1, 0, 0);
			}
		}

		if (cmd_ok)
			p_inq->remname_active |= origin;
		else
			status = BTM_NO_RESOURCES;
	}
	else
	{
		status = BTM_BUSY;
	}

	return status;
}

void btm_process_remote_name(BD_ADDR bda, BD_NAME bdn, UINT16 evt_len,
                             UINT8 hci_status)
{
	tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
	tBTM_REMOTE_DEV_NAME rem_name;
	tBTM_CMPL_CB *p_cb = p_inq->p_remname_cmpl_cb;
	UINT8 active_flags = p_inq->remname_active;
	UINT16 len;
	UINT16 xx;

	btu_stop_timer(&p_inq->rmt_name_timer_ent);

	p_inq->remname_active = BTM_RMT_NAME_INACTIVE;

	if (hci_status == HCI_SUCCESS)
	{
		UINT8 *p_dst = rem_name.remote_bd_name;
		UINT16 n;

		len = (evt_len < BD_NAME_LEN) ? evt_len : BD_NAME_LEN;
		rem_name.length = len;
		rem_name.status = BTM_SUCCESS;

		for (n = len; n > 0; n--)
			*p_dst++ = *bdn++;
	}
	else
	{
		rem_name.length = 0;
		rem_name.status = BTM_BAD_VALUE_RET;
		rem_name.remote_bd_name[0] = 0;
	}

	memset(p_inq->remname_bda, 0, BD_ADDR_LEN);

	if (active_flags & BTM_RMT_NAME_EXT)
	{
		p_inq->p_remname_cmpl_cb = NULL;

		if (p_cb)
			(*p_cb)(&rem_name);
	}
}

void btm_inq_rmt_name_failed(void)
{
	btm_process_remote_name(NULL, NULL, 0, HCI_ERR_UNSPECIFIED);
	btm_sec_rmt_name_request_complete(NULL, NULL, HCI_ERR_UNSPECIFIED);
}
