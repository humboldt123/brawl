#include <hidd_int.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/hid/hidd_conn.c, hidd_pm.c
 */

/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
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

#include <btm_api.h>
#include <btu.h>
#include <hidd_api.h>
#include <l2c_api.h>

/*******************************************************************************
 * macros
 */

// HYPOTHESIS: the original names for these live in a header i don't have, values are what the asm uses
#define HIDD_MTU_SIZE			0x40
#define HIDD_MAX_CONN_RETRY		15
#define HIDD_REPAGE_WIN			1
#define HIDD_PM_ERR_BASE		0x37
#define HIDD_PM_INVALID_MODE	0xff
#define HIDD_STATE_NO_CONN		0

// callback event codes as passed to hd_cb.callback
#define HIDD_EVT_CLOSE			1
#define HIDD_EVT_RETRYING		2
#define HIDD_EVT_PM_MODE_CHG	3
#define HIDD_EVT_PM_ERR_RSP		4

/*******************************************************************************
 * local function declarations
 */

static void hidd_pm_timer_cback(TIMER_LIST_ENT *p_tle);

/*******************************************************************************
 * variables
 */

// .bss
tHID_DEV_CTB hd_cb;

/*******************************************************************************
 * functions
 */

void HID_DevInit(void)
{
	memset(&hd_cb, 0, sizeof hd_cb);

	hd_cb.l2cap_ctrl_cfg.mtu_present = TRUE;
	hd_cb.l2cap_ctrl_cfg.mtu = HIDD_MTU_SIZE;

	hd_cb.l2cap_int_cfg.mtu_present = TRUE;
	hd_cb.l2cap_int_cfg.mtu = HIDD_MTU_SIZE;

	hd_cb.conn.timer_entry.param = (TIMER_PARAM_TYPE)hidd_proc_repage_timeout;

	hd_cb.trace_level = BT_TRACE_LEVEL_NONE;
}

// BYTES: the original is not inlined into the repage timer callback, no idea why
#pragma dont_inline on
// pages the host: opens the control channel and waits for l2cap to report back
tHID_STATUS hidd_conn_initiate(void)
{
	tHID_CONN *p_conn = &hd_cb.conn;

	HIDD_TRACE(EVENT, "HID - Originate started");

	p_conn->ctrl_cid = 0;
	p_conn->intr_cid = 0;
	p_conn->conn_flags = HID_CONN_FLAGS_IS_ORIG;

	if ((p_conn->ctrl_cid = L2CA_ConnectReq(HID_PSM_CONTROL, hd_cb.host_addr)) == 0)
	{
		HIDD_TRACE(WARNING, "HID - Originate failed");

		return HID_ERR_L2CAP_FAILED;
	}

	p_conn->conn_state = HID_CONN_STATE_CONNECTING_CTRL;

	return HID_SUCCESS;
}
#pragma dont_inline reset

// repage timer expired: try again, give up (and tell the app) after too many tries
void hidd_proc_repage_timeout(TIMER_LIST_ENT *p_tle)
{
	HIDD_TRACE(DEBUG, "hidd_proc_repage_timeout");

	hd_cb.conn_tries++;

	if (hidd_conn_initiate() != HID_SUCCESS)
	{
		if (hd_cb.conn_tries > HIDD_MAX_CONN_RETRY)
		{
			hd_cb.dev_state = HIDD_STATE_NO_CONN;

			(*hd_cb.callback)(HIDD_EVT_CLOSE, 0, NULL);
		}
		else
		{
			btu_start_timer(&hd_cb.conn.timer_entry, BTU_TTYPE_USER_FUNC, HIDD_REPAGE_WIN);
		}
	}
	else
	{
		(*hd_cb.callback)(HIDD_EVT_RETRYING, hd_cb.conn_tries, NULL);
	}
}

// asks btm to move the link to the requested power mode. returns TRUE if nothing went wrong
// (or there was nothing to do), FALSE after reporting a pm error to the app
UINT8 hidd_pm_set_power_mode(tHID_DEV_PWR_MD *p_req_mode)
{
	tHID_DEV_PWR_MD act_mode = {0};
	UINT8 pm_err = 0;

	if (hd_cb.pm_ctrl_busy ||
	    (p_req_mode->mode == hd_cb.curr_pm.mode &&
	     (p_req_mode->mode == HCI_MODE_ACTIVE ||
	      (hd_cb.curr_pm.interval >= p_req_mode->min && hd_cb.curr_pm.interval <= p_req_mode->max))))
	{
		hd_cb.final_pm.mode = HIDD_PM_INVALID_MODE;

		return TRUE;
	}

	switch (p_req_mode->mode)
	{
	case HCI_MODE_ACTIVE:
		if (hd_cb.curr_pm.mode == HCI_MODE_SNIFF)
		{
			pm_err = BTM_SetPowerMode(BTM_PM_SET_ONLY_ID, hd_cb.host_addr, (tBTM_PM_PWR_MD *)&act_mode);
			hd_cb.pm_ctrl_busy = TRUE;
		}
		else if (hd_cb.curr_pm.mode == HCI_MODE_PARK)
		{
			pm_err = BTM_SetPowerMode(BTM_PM_SET_ONLY_ID, hd_cb.host_addr, (tBTM_PM_PWR_MD *)&act_mode);
			hd_cb.pm_ctrl_busy = TRUE;
		}
		break;

	case HCI_MODE_SNIFF:
		if (hd_cb.curr_pm.mode != HCI_MODE_ACTIVE)
		{
			hidd_pm_set_power_mode(&act_mode);
		}
		else
		{
			pm_err = BTM_SetPowerMode(BTM_PM_SET_ONLY_ID, hd_cb.host_addr, (tBTM_PM_PWR_MD *)p_req_mode);
			hd_cb.pm_ctrl_busy = TRUE;
		}
		break;

	case HCI_MODE_PARK:
		if (hd_cb.curr_pm.mode != HCI_MODE_ACTIVE)
		{
			hidd_pm_set_power_mode(&act_mode);
		}
		else
		{
			pm_err = BTM_SetPowerMode(BTM_PM_SET_ONLY_ID, hd_cb.host_addr, (tBTM_PM_PWR_MD *)p_req_mode);
			hd_cb.pm_ctrl_busy = TRUE;
		}
		break;
	}

	if (pm_err <= BTM_CMD_STARTED)
		return TRUE;

	pm_err += HIDD_PM_ERR_BASE;

	if (hd_cb.callback)
		(*hd_cb.callback)(HIDD_EVT_PM_ERR_RSP, hd_cb.conn_substate, (tHID_DEV_REG_INFO_CBACK_DATA *)&pm_err);

	return FALSE;
}

// btm told us the link changed mode
void hidd_pm_proc_mode_change(UINT8 hci_status, UINT8 mode, UINT16 interval)
{
	if (!hd_cb.reg_flag)
		return;

	hd_cb.pm_ctrl_busy = FALSE;

	if (hci_status != 0)
	{
		if (hd_cb.callback)
			(*hd_cb.callback)(HIDD_EVT_PM_ERR_RSP, hd_cb.conn_substate, (tHID_DEV_REG_INFO_CBACK_DATA *)&hci_status);

		return;
	}

	hd_cb.curr_pm.mode = mode;
	hd_cb.curr_pm.interval = interval;

	if (hd_cb.final_pm.mode != HIDD_PM_INVALID_MODE)
	{
		if (hd_cb.final_pm.mode != mode ||
		    (hd_cb.final_pm.mode != HCI_MODE_ACTIVE && (interval < hd_cb.final_pm.min || interval > hd_cb.final_pm.max)))
		{
			hidd_pm_set_power_mode(&hd_cb.final_pm);
		}
		else
		{
			hd_cb.final_pm.mode = HIDD_PM_INVALID_MODE;
		}
	}
	else if (mode == HCI_MODE_ACTIVE)
	{
		memcpy(&hd_cb.final_pm, &hd_cb.pm_params[0], sizeof hd_cb.final_pm);
		hidd_pm_set_power_mode(&hd_cb.pm_params[0]);

		hd_cb.conn_substate = 0;

		hd_cb.idle_tle.param = (TIMER_PARAM_TYPE)hidd_pm_timer_cback;
		btu_start_timer(&hd_cb.idle_tle, BTU_TTYPE_USER_FUNC, 60);
	}

	if (hd_cb.callback)
		(*hd_cb.callback)(HIDD_EVT_PM_MODE_CHG, mode, (tHID_DEV_REG_INFO_CBACK_DATA *)&interval);
}

// idle timer fired: go to the second configured power mode
static void hidd_pm_timer_cback(TIMER_LIST_ENT *p_tle)
{
	memcpy(&hd_cb.final_pm, &hd_cb.pm_params[1], sizeof hd_cb.final_pm);
	hidd_pm_set_power_mode(&hd_cb.pm_params[1]);

	hd_cb.conn_substate = 1;
}
