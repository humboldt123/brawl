#include <bta_hh_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * bta/hh/bta_hh_utils.c
 */

/******************************************************************************
 *
 *  Copyright (C) 2005-2012 Broadcom Corporation
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

#include <bd.h>
#include <bt_trace.h>
#include <gki.h>
#include <utl.h>

/*******************************************************************************
 * macros
 */

#define BTA_HH_IDX_INVALID	BTA_HH_MAX_KNOWN

/*******************************************************************************
 * functions
 */

UINT8 bta_hh_find_cb(BD_ADDR bda)
{
	UINT8 xx;

	for (xx = 0; xx < BTA_HH_MAX_KNOWN; xx++)
	{
		if (bdcmp(bda, bta_hh_cb.kdev[xx].addr) == 0 && bdcmp(bda, bd_addr_null) != 0)
		{
			APPL_TRACE(DEBUG, "found kdev_cb[%d] hid_handle = %d ", xx, bta_hh_cb.kdev[xx].hid_handle);

			return xx;
		}

		APPL_TRACE(DEBUG, "in_use ? [%d] kdev[%d].hid_handle = %d state = [%d]",
		           bta_hh_cb.kdev[xx].in_use, xx, bta_hh_cb.kdev[xx].hid_handle,
		           bta_hh_cb.kdev[xx].state);
	}

	// not known yet, hand out the first free slot
	for (xx = 0; xx < BTA_HH_MAX_KNOWN; xx++)
	{
		if (!bta_hh_cb.kdev[xx].in_use)
		{
			bdcpy(bta_hh_cb.kdev[xx].addr, bda);
			break;
		}
	}

	APPL_TRACE(DEBUG, "bta_hh_find_cb:: index = %d while max = %d", xx, BTA_HH_MAX_KNOWN);

	return xx;
}

void bta_hh_clean_up_kdev(tBTA_HH_DEV_CB *p_cb)
{
	UINT8 index;

	if (p_cb->hid_handle != BTA_HH_INVALID_HANDLE)
		bta_hh_cb.cb_index[p_cb->hid_handle] = BTA_HH_IDX_INVALID;

	index = p_cb->index;

	utl_freebuf((void **)&p_cb->dscp_info.dsc_list);

	memset(p_cb, 0, sizeof(tBTA_HH_DEV_CB));

	p_cb->index = index;
	p_cb->state = BTA_HH_IDLE_ST;
	p_cb->hid_handle = BTA_HH_INVALID_HANDLE;
}

void bta_hh_add_device_to_list(tBTA_HH_DEV_CB *p_cb, UINT8 handle,
                               UINT16 attr_mask,
                               tHID_DEV_DSCP_INFO *p_dscp_info, UINT8 sub_class,
                               UINT8 app_id)
{
	APPL_TRACE(DEBUG, "subclass = 0x%2x", sub_class);

	p_cb->hid_handle = handle;
	p_cb->in_use = TRUE;
	p_cb->attr_mask = (UINT16)attr_mask;
	p_cb->sub_class = sub_class;
	p_cb->app_id = app_id;

	if (p_dscp_info)
	{
		utl_freebuf((void **)&p_cb->dscp_info.dsc_list);

		if (p_dscp_info->dl_len != 0)
		{
			p_cb->dscp_info.dsc_list = (UINT8 *)GKI_getbuf(p_dscp_info->dl_len);

			if (p_cb->dscp_info.dsc_list != NULL)
			{
				p_cb->dscp_info.dl_len = p_dscp_info->dl_len;
				memcpy(p_cb->dscp_info.dsc_list, p_dscp_info->dsc_list, p_dscp_info->dl_len);
			}
		}
	}
}

BOOLEAN bta_hh_tod_spt(tBTA_HH_DEV_CB *p_cb, UINT8 sub_class)
{
	UINT8 xx;
	UINT8 cod = (sub_class >> 2);

	for (xx = 0; xx < p_bta_hh_cfg->max_devt_spt; xx++)
	{
		if (cod == p_bta_hh_cfg->p_devt_list[xx].tod)
		{
			p_cb->app_id = p_bta_hh_cfg->p_devt_list[xx].app_id;
			return TRUE;
		}
	}

	return FALSE;
}

// HYPOTHESIS: nothing in the wii build calls the two boot report parsers, they only
// survive as strings. the code below is my own guess at what they did (only its strings are checked)
void bta_hh_parse_keybd_rpt(tBTA_HH_BOOT_RPT *p_kb_data, UINT8 *p_report,
                            UINT16 report_len)
{
	tBTA_HH_KEYBD_RPT *p_data = &p_kb_data->data_rpt.keybd_rpt;
	UINT8 this_char, ctl_shift;
	UINT16 xx;

	APPL_TRACE(DEBUG, "bta_hh_parse_keybd_rpt:  (report=%p, report_len=%d) called", p_report, report_len);

	if (report_len < 2)
		return;

	ctl_shift = *p_report++;
	report_len--;

	p_data->upper = (ctl_shift & 0x22) ? TRUE : FALSE;
	p_data->ctrl = (ctl_shift & 0x11) ? TRUE : FALSE;

	if (ctl_shift & 0x44)
	{
		p_data->alt = TRUE;
		APPL_TRACE(DEBUG, "Alt key pressed");
	}
	else
	{
		p_data->alt = FALSE;
		APPL_TRACE(DEBUG, "Alt key not pressed");
	}

	for (xx = 0; xx < report_len; xx++)
	{
		this_char = *p_report++;

		if (this_char == 0)
			continue;

		APPL_TRACE(DEBUG, "this_char = %02x", this_char);

		if (this_char > 0x73)
		{
			APPL_TRACE(ERROR, "BTA_HhParseKeybdRpt:  Cannot interpret scan code                 0x%02x", this_char);
			continue;
		}

		p_data->this_char = this_char;
	}
}

void bta_hh_parse_mice_rpt(tBTA_HH_BOOT_RPT *p_mice_data, UINT8 *p_report,
                           UINT16 report_len)
{
	tBTA_HH_MICE_RPT *p_data = &p_mice_data->data_rpt.mice_rpt;

	APPL_TRACE(DEBUG, "bta_hh_parse_mice_rpt:  bta_keybd_rpt_rcvd(report=%p,                 report_len=%d) called", p_report, report_len);

	if (report_len < 3)
		return;

	p_data->mouse_button = p_report[0];
	APPL_TRACE(DEBUG, "mice button: 0x%2x", p_data->mouse_button);

	p_data->delta_x = p_report[1];
	p_data->delta_y = p_report[2];
	APPL_TRACE(DEBUG, "mice move: x = %d y = %d", p_data->delta_x, p_data->delta_y);
}

void bta_hh_trace_dev_db(void)
{
	UINT8 xx;

	APPL_TRACE(DEBUG, "bta_hh_trace_dev_db:: Device DB list********************");

	for (xx = 0; xx < BTA_HH_MAX_KNOWN; xx++)
	{
		APPL_TRACE(DEBUG, "kdev[%d] in_use[%d]  handle[%d] ", xx,
		           bta_hh_cb.kdev[xx].in_use, bta_hh_cb.kdev[xx].hid_handle);

		APPL_TRACE(DEBUG, "\t\t\t attr_mask[%04x] state [%d] sub_class[%02x] index = %d",
		           bta_hh_cb.kdev[xx].attr_mask, bta_hh_cb.kdev[xx].state,
		           bta_hh_cb.kdev[xx].sub_class, bta_hh_cb.kdev[xx].index);
	}

	APPL_TRACE(DEBUG, "*********************************************************");
}
