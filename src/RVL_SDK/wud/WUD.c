#include "revolution.h"
#include "revolution/bte.h"
#include "revolution/os.h"
#include "revolution/sc.h"
#include "revolution/wud.h"
#include <cstring>

#define DEBUGPrint WUD_DEBUGPrint

// HYPOTHESIS: names for the state numbers the headers have wrong or don't spell out
#define WUD_INIT_STATE_SC_FLUSHED    4
#define WUD_INIT_STATE_RUNNING       5
#define WUD_LIB_STATUS_READY         3
#define WUD_PM_SNIFF_MAX_DELAY       8

// MATCH-ONLY: bte, sc and os pieces that have no header yet
extern u32 BTE_GetMemSize(void);
extern u8 lbl_805A0C71;                                     // an sc dirty flag, I think
extern u32 lbl_805A0674;                                    // low word of the boot time

/*******************************************************************************
 * variables
 */

// the hid report descriptor handed to the HH layer for every wiimote
u8 _hidDescriptor[0xDC] = {
    0x05, 0x01, 0x09, 0x05, 0xA1, 0x01, 0x85, 0x10, 0x15, 0x00, 0x26, 0xFF,
    0x00, 0x75, 0x08, 0x95, 0x01, 0x06, 0x00, 0xFF, 0x09, 0x01, 0x91, 0x00,
    0x85, 0x11, 0x95, 0x01, 0x09, 0x01, 0x91, 0x00, 0x85, 0x12, 0x95, 0x02,
    0x09, 0x01, 0x91, 0x00, 0x85, 0x13, 0x95, 0x01, 0x09, 0x01, 0x91, 0x00,
    0x85, 0x14, 0x95, 0x01, 0x09, 0x01, 0x91, 0x00, 0x85, 0x15, 0x95, 0x01,
    0x09, 0x01, 0x91, 0x00, 0x85, 0x16, 0x95, 0x15, 0x09, 0x01, 0x91, 0x00,
    0x85, 0x17, 0x95, 0x06, 0x09, 0x01, 0x91, 0x00, 0x85, 0x18, 0x95, 0x15,
    0x09, 0x01, 0x91, 0x00, 0x85, 0x19, 0x95, 0x01, 0x09, 0x01, 0x91, 0x00,
    0x85, 0x1A, 0x95, 0x01, 0x09, 0x01, 0x91, 0x00, 0x85, 0x20, 0x95, 0x06,
    0x09, 0x01, 0x81, 0x00, 0x85, 0x21, 0x95, 0x15, 0x09, 0x01, 0x81, 0x00,
    0x85, 0x22, 0x95, 0x04, 0x09, 0x01, 0x81, 0x00, 0x85, 0x30, 0x95, 0x02,
    0x09, 0x01, 0x81, 0x00, 0x85, 0x31, 0x95, 0x05, 0x09, 0x01, 0x81, 0x00,
    0x85, 0x32, 0x95, 0x0A, 0x09, 0x01, 0x81, 0x00, 0x85, 0x33, 0x95, 0x11,
    0x09, 0x01, 0x81, 0x00, 0x85, 0x34, 0x95, 0x15, 0x09, 0x01, 0x81, 0x00,
    0x85, 0x35, 0x95, 0x15, 0x09, 0x01, 0x81, 0x00, 0x85, 0x36, 0x95, 0x15,
    0x09, 0x01, 0x81, 0x00, 0x85, 0x37, 0x95, 0x15, 0x09, 0x01, 0x81, 0x00,
    0x85, 0x3D, 0x95, 0x15, 0x09, 0x01, 0x81, 0x00, 0x85, 0x3E, 0x95, 0x15,
    0x09, 0x01, 0x81, 0x00, 0x85, 0x3F, 0x95, 0x15, 0x09, 0x01, 0x81, 0x00,
    0xC0, 0x00, 0x00, 0x00,
};
WUDCB _wcb;
WUDDevInfo _work;

static WUDDiscResp _discResp;

SCBtDeviceInfoArray _scArray;

BD_ADDR_PTR _dev_handle_to_bda[WUD_MAX_DEV_ENTRY];

static u8 _fiberStack[0x1000];
static SCBtCmpDevInfoArray _scCmpArray;

u16 _dev_handle_queue_size[WUD_MAX_DEV_ENTRY];
u16 _dev_handle_notack_num[WUD_MAX_DEV_ENTRY];

// the compiler lays these out in reverse order of definition
static u8 sFoundCount;
static s8 sFoundRssi;
static u8 sCheckName;
static u32 sPatchLen;
static u32 sPatchOffset;
static u32 sPatchAddr;
static u8 sPatchCount;
static u8 sPatchIndex;
// MATCH-ONLY: the three pad bytes in front of this one have no owner I could find
static u8 sVscParam __attribute__((aligned(4)));
static u8 sTraceLevel;
static int sInitialized;

/*******************************************************************************
 * local function declarations
 */

static void WUDiInitCore(void);
extern u8 _vscCancelParam[0x1C];
extern u8 _patchFirst[0xBC];
extern u8 _patchList[0x5C];

static void WUDiPatchFirstCallback(tBTM_VSC_CMPL* p);
static void WUDiPatchStepCallback(tBTM_VSC_CMPL* p);
static void WUDiPatchListCallback(tBTM_VSC_CMPL* p);
static void WUDiPatchDone(void* p);
static void WUDSecurityCallback(tBTA_DM_SEC_EVT event, tBTA_DM_SEC* pData);
static void WUDSearchCallback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH* pData);
static void WUDVendorSpecificCallback(UINT8 len, UINT8* p);
static void WUDDeviceStatusCallback(tBTM_DEV_STATUS status);
static void WUDLinkKeyCallback(void* p);
static void WUDPowerManagerCallback(BD_ADDR addr, tBTM_PM_STATUS status, UINT16 value, UINT8 hciStatus);
static void WUDiUnloadCallback(int result);
static void WUDiInitAlarmHandler(OSAlarm* alarm, OSContext* context);
static void WUDiStackAlarmHandler(OSAlarm* alarm, OSContext* context);
static void WUDiSyncAlarmHandler(OSAlarm* alarm, OSContext* context);
static void WUDiDeleteAlarmHandler(OSAlarm* alarm, OSContext* context);
static void WUDiShutdownAlarmHandler(OSAlarm* alarm, OSContext* context);
static BOOL WUDiStartSync(int type, s8 loopNum, u8 checkName, u8 skipChecks);

/*******************************************************************************
 * functions
 */

static BOOL WUDiIsBusy(void) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    BOOL busy;

    enabled = OSDisableInterrupts();

    if (p->syncState == 0 && p->deleteState == 0 && p->stackState == 4 && p->initState == WUD_INIT_STATE_RUNNING) {
        OSRestoreInterrupts(enabled);
        busy = FALSE;
    } else {
        OSRestoreInterrupts(enabled);
        busy = TRUE;
    }

    return busy;
}

static u32 WUDiGetLibStatus(void) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    u32 status;

    enabled = OSDisableInterrupts();
    status = p->libStatus;
    OSRestoreInterrupts(enabled);

    return status;
}

void* WUDiMemAlloc(u32 size) {
    DEBUGPrint("App_MEMalloc\n");

    return _wcb.allocFunc(size);
}

u8 WUDiMemFree(void* pBlock) {
    DEBUGPrint("App_MEMfree\n");

    return _wcb.freeFunc(pBlock);
}

static void WUDiSyncFlushCallback(s32 result) {
    WUDCB* p = &_wcb;

    DEBUGPrint("SyncFlushCallback() : %d, Sync: %d\n", result, p->syncState);

    if (p->syncState != 0) {
        if (result == 0) {
            p->syncState = WUD_STATE_SYNC_COMPLETE;
        } else {
            p->syncState = WUD_STATE_SYNC_ERROR;
        }
    }
}

static void WUDiDeleteFlushCallback(s32 result) {
    WUDCB* p = &_wcb;

    DEBUGPrint("DeleteFlushCallback() : %d, Delete: %d\n", result, p->deleteState);

    if (p->deleteState != 0) {
        p->deleteState = WUD_STATE_DELETE_DONE;
    }
}

static void WUDiShutFlushCallback(s32 result) {
    WUDCB* p = &_wcb;

    DEBUGPrint("ShutFlushCallback() : %d, Shutdown: %d\n", result, p->shutdownState);

    p->shutdownState = WUD_STATE_SHUTDOWN_DONE;
}

static void WUDiInitFlushCallback(s32 result) {
    WUDCB* p = &_wcb;

    DEBUGPrint("InitFlushCallback() : %d, Init: %d\n", result, p->initState);

    p->initState = WUD_INIT_STATE_SC_FLUSHED;
}

u8 WUDiGetDevNumber(void) {
    BOOL enabled;
    u8 num;

    enabled = OSDisableInterrupts();
    num = _wcb.devNums;
    OSRestoreInterrupts(enabled);

    return num;
}

u8 WUDiGetDevSmpNumber(void) {
    BOOL enabled;
    u8 num;

    enabled = OSDisableInterrupts();
    num = _wcb.devSmpNums;
    OSRestoreInterrupts(enabled);

    return num;
}

u8 WUDiGetConnNumber(void) {
    BOOL enabled;
    u8 num;

    enabled = OSDisableInterrupts();
    num = _wcb.connectedNum;
    OSRestoreInterrupts(enabled);

    return num;
}

u8 WUDiGetLinkNum(void) {
    BOOL enabled;
    u8 num;

    enabled = OSDisableInterrupts();
    num = _wcb.linkedNum;
    OSRestoreInterrupts(enabled);

    return num;
}

WUDDevInfo* WUDiGetDevInfoIndex(int idx) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    if (idx >= 0 && idx < WUD_MAX_DEV_ENTRY_FOR_STD) {
        pInfo = &p->stdDevs[idx];
    } else {
        pInfo = &p->smpDevs[idx - WUD_MAX_DEV_ENTRY_FOR_STD];
    }

    OSRestoreInterrupts(enabled);

    return pInfo;
}

// state 1: look for another wiimote to sync, unless we're done or full
static u8 WUDiSyncPrepareSearch(void) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    WUDDevInfo* pInfo;
    tBTM_PM_PWR_MD mode;
    int i;

    enabled = OSDisableInterrupts();
    p->discoverable = FALSE;
    p->connectable = FALSE;
    OSRestoreInterrupts(enabled);

    BTA_DmSetVisibility(FALSE, FALSE);

    if (p->syncLoopNum == 0) {
        return WUD_STATE_SYNC_DONE;
    }

    if (WUDiGetLinkNum() == 4 && WUDiGetConnNumber() == 4) {
        return WUD_STATE_SYNC_DONE;
    }

    if (p->syncSkipChecks) {
        for (i = 0; i < WUD_MAX_DEV_ENTRY; i++) {
            pInfo = WUDiGetDevInfoIndex(i);

            if (pInfo->status == 9) {
                mode.mode = BTM_PM_MD_ACTIVE;
                mode.max = 0;
                mode.min = 0;
                mode.attempt = 1;
                mode.timeout = 0;

                BTM_SetPowerMode(_wcb.pmID, pInfo->devAddr, &mode);
                return WUD_STATE_SYNC_PREPARE_SEARCH;
            }
        }
    }

    if (p->syncLoopNum > 0) {
        p->syncLoopNum--;
    }

    p->delayCnt = 50;
    return WUD_STATE_SYNC_WAIT_FOR_START_SEARCH;
}

// state 17: a wiimote we've never seen, make room for it
#pragma opt_propagation off
static u8 WUDiSyncPrepareUnknownDevice(void) {
    WUDDevInfo* pWork;

    if (WUDiGetDevNumber() == WUD_MAX_DEV_ENTRY_FOR_STD && WUDiGetDevSmpNumber() == WUD_MAX_DEV_ENTRY_FOR_SMP) {
        DEBUGPrint("Device Entry is full.\n");

        if (_wcb.syncType == WUD_SYNC_TYPE_STANDARD) {
            DEBUGPrint("Remove the oldest device in std.\n");
            pWork = _wcb.stdListTail->devInfo;
        } else {
            DEBUGPrint("Remove the oldest device in smp.\n");
            pWork = _wcb.smpListTail->devInfo;
        }

        WUDiRemoveDevice(pWork->devAddr);
    }

    pWork = &_work;
    pWork->status = 1;
    pWork->sync_type = _wcb.syncType != WUD_SYNC_TYPE_STANDARD;

    memcpy(pWork->devAddr, _discResp.devAddr, WUD_ADDR_LEN);
    memcpy(&pWork->conf, _discResp.devName, sizeof(pWork->conf));
    memset(pWork->linkKey, 0, sizeof(pWork->linkKey));

    return WUD_STATE_SYNC_TRY_CONNECT;
}
#pragma opt_propagation reset

// state 22: copy the std list into the sc array and write it out
static u8 WUDiSyncStoreDevInfo(void) {
    u8 i;
    WUDDevInfoList* pList;
    BOOL ready = SCCheckStatus() != SC_STATUS_BUSY;

    if (!ready) {
        return WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND;
    }

    memset(_scArray.info, 0, WUD_MAX_DEV_ENTRY_FOR_STD * sizeof(SCBtDeviceInfoSingle));
    _scArray.num = WUDiGetDevNumber();

    i = 0;
    for (pList = _wcb.stdListHead; pList != NULL; pList = pList->next) {
        memcpy(_scArray.info[i].bd_addr, pList->devInfo->devAddr, WUD_ADDR_LEN);
        memcpy(_scArray.info[i].bd_name, pList->devInfo, sizeof(_scArray.info[i].bd_name));
        i++;
    }

    DEBUGPrint("%d devices is stored into SC.\n", i);

    if (SCSetBtDeviceInfoArray(&_scArray)) {
        return WUD_STATE_SYNC_SC_FLUSH;
    }

    return WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND;
}

// state 14: the sync run is finished, back to being connectable
static u8 WUDiSyncFinish(void) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    tBTM_PM_PWR_MD mode;
    WUDSyncDeviceCallback cb;
    BOOL enabled;
    int i;

    if (p->syncSkipChecks) {
        for (i = 0; i < WUD_MAX_DEV_ENTRY; i++) {
            pInfo = WUDiGetDevInfoIndex(i);

            if (pInfo->status == 8) {
                mode.mode = BTM_PM_MD_SNIFF;
                mode.max = WUD_PM_SNIFF_MAX_DELAY;
                mode.min = WUD_PM_SNIFF_MAX_DELAY;
                mode.attempt = 1;
                mode.timeout = 0;

                BTM_SetPowerMode(_wcb.pmID, pInfo->devAddr, &mode);
                return WUD_STATE_SYNC_DONE;
            }
        }
    }

    OSCancelAlarm(&p->alarm);

    enabled = OSDisableInterrupts();
    _wcb.discoverable = FALSE;
    _wcb.connectable = TRUE;
    OSRestoreInterrupts(enabled);

    BTA_DmSetVisibility(FALSE, TRUE);

    if (p->syncType == WUD_SYNC_TYPE_STANDARD) {
        cb = p->syncStdCB;
    } else {
        cb = p->syncSmpCB;
    }

    if (cb != NULL) {
        cb(WUD_RESULT_SYNC_DONE, p->syncedNum);
    }

    DEBUGPrint("Pairing Done\n");
    return 0;
}

// the whole sync procedure, one step per timer tick
static void WUDiSyncStateMachine(void) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    WUDDevInfo* pWork;
    tBTA_DM_INQ inq;
    u8 next;
    u8 state;
    BOOL enabled;

    switch (p->syncState) {
    case WUD_STATE_SYNC_PREPARE_SEARCH: {
        p->syncState = WUDiSyncPrepareSearch();
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_START_SEARCH: {
        if (p->syncLoopNum == 0) {
            next = WUD_STATE_SYNC_DONE;
        } else if (WUDiGetLinkNum() == 4 && WUDiGetConnNumber() == 4) {
            next = WUD_STATE_SYNC_DONE;
        } else {
            next = WUD_STATE_SYNC_WAIT_FOR_START_SEARCH;

            if (--p->delayCnt < 0) {
                next = WUD_STATE_SYNC_START_SEARCH;
            }
        }

        p->syncState = next;
        break;
    }

    case WUD_STATE_SYNC_START_SEARCH: {
        inq.mode = 1;
        inq.max_resps = 1;
        inq.report_dup = FALSE;

        if (p->syncSkipChecks) {
            inq.duration = 3;
        } else if (p->syncType == WUD_SYNC_TYPE_STANDARD) {
            inq.duration = WUDiGetLinkNum() == 3 ? 10 : 5;
            inq.duration -= p->syncLoopNum;
        } else {
            inq.duration = WUDiGetLinkNum() == 3 ? 8 : 3;
        }

        sFoundCount = 0;
        memset(&_discResp, 0, sizeof(_discResp));

        BTA_DmSearch(&inq, 0, WUDSearchCallback);

        p->syncState = WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT;
        break;
    }

    case WUD_STATE_SYNC_CHECK_SEARCH_RESULT: {
        next = WUD_STATE_SYNC_PREPARE_SEARCH;

        if (sFoundCount != 0) {
            if (!sCheckName && memcmp(_discResp.devName, "Nintendo RVL-CNT", 16) == 0) {
                next = WUD_STATE_SYNC_IS_EXISTED_DEVICE;
            }

            if (sFoundRssi < _wcb.syncRssi) {
                next = WUD_STATE_SYNC_PREPARE_SEARCH;
            }
        }

        if (_wcb.syncType == WUD_SYNC_TYPE_STANDARD && next == WUD_STATE_SYNC_PREPARE_SEARCH) {
            next = WUD_STATE_SYNC_WAIT_FOR_INCOMING;
            _wcb.incomeCnt = WUDiGetLinkNum() == 3 ? 200 : 100;

            enabled = OSDisableInterrupts();
            _wcb.discoverable = FALSE;
            _wcb.connectable = TRUE;
            OSRestoreInterrupts(enabled);

            BTA_DmSetVisibility(FALSE, TRUE);
        }

        p->syncState = next;
        break;
    }

    case WUD_STATE_SYNC_IS_EXISTED_DEVICE: {
        next = WUD_STATE_SYNC_PREPARE_FOR_UNKNOWN_DEVICE;

        pInfo = WUDiGetDevInfo(_discResp.devAddr);
        if (pInfo != NULL) {
            memcpy(&_work, pInfo, sizeof(_work));
            next = WUD_STATE_SYNC_PREPARE_FOR_EXISTED_DEVICE;
        }

        p->syncState = next;
        break;
    }

    case WUD_STATE_SYNC_PREPARE_FOR_EXISTED_DEVICE: {
        pWork = &_work;
        pWork->status = 1;

        switch (pWork->sync_type) {
        case 0:
        case 2:
        case 4:
        case 5: {
            if (_wcb.syncType == WUD_SYNC_TYPE_STANDARD) {
                WUDiMoveBottomStdDevInfoPtr(pWork);
                WUDiRemoveDevice(pWork->devAddr);
                next = WUD_STATE_SYNC_PREPARE_FOR_UNKNOWN_DEVICE;
                break;
            }

            pWork->sync_type = 4;
            goto connect;
        }

        case 1:
        case 3: {
            WUDiMoveBottomSmpDevInfoPtr(pWork);
            WUDiRemoveDevice(_wcb.smpListTail->devInfo->devAddr);
            next = WUD_STATE_SYNC_PREPARE_FOR_UNKNOWN_DEVICE;
            break;
        }

        default: {
        connect:
            next = WUD_STATE_SYNC_TRY_CONNECT;
            break;
        }
        }

        p->syncState = next;
        break;
    }

    case WUD_STATE_SYNC_PREPARE_FOR_UNKNOWN_DEVICE: {
        p->syncState = WUDiSyncPrepareUnknownDevice();
        break;
    }

    case WUD_STATE_SYNC_TRY_CONNECT: {
        next = WUD_STATE_SYNC_ERROR;

        if (memcmp(_discResp.devName, "Nintendo RVL-CNT", 16) == 0) {
            _work.status = 2;
            BTA_HhOpen(_work.devAddr, 0, 0x12);
            next = WUD_STATE_SYNC_6;
        }

        p->syncState = next;
        break;
    }

    case WUD_STATE_SYNC_REGISTER_DEVICE: {
        state = _work.sync_type;

        switch (state) {
        case 1:
            state = WUD_STATE_SYNC_VIRGIN_SIMPLE;
            break;
        case 0:
            state = WUD_STATE_SYNC_VIRGIN_STANDARD;
            break;
        case 5:
            state = WUD_STATE_SYNC_CHANGE_SIMPLE_TO_STANDARD;
            break;
        case 3:
            state = WUD_STATE_SYNC_COMPLETE;
            break;
        case 4:
            state = WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM;
            break;
        case 2:
            state = WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM;
            break;
        }

        p->syncState = state;
        break;
    }

    case WUD_STATE_SYNC_VIRGIN_SIMPLE: {
        if (WUDiGetDevSmpNumber() == WUD_MAX_DEV_ENTRY_FOR_SMP) {
            WUDiRemoveDevice(p->smpListTail->devInfo->devAddr);
        }

        pInfo = WUDiGetNewDevInfo();
        if (pInfo == NULL) {
            state = WUD_STATE_SYNC_ERROR;
        } else if (pInfo->status != 0) {
            state = WUD_STATE_SYNC_ERROR;
        } else {
            memcpy(pInfo, &_work, sizeof(*pInfo));
            WUDiRegisterDevice(pInfo->devAddr);
            WUDiMoveTopSmpDevInfoPtr(pInfo);
            _dev_handle_to_bda[pInfo->devHandle] = pInfo->devAddr;
            state = WUD_STATE_SYNC_COMPLETE;
        }

        p->syncState = state;
        break;
    }

    case WUD_STATE_SYNC_VIRGIN_STANDARD: {
        if (WUDiGetDevNumber() == WUD_MAX_DEV_ENTRY_FOR_STD) {
            WUDiRemoveDevice(p->stdListTail->devInfo->devAddr);
        }

        pInfo = WUDiGetNewDevInfo();
        if (pInfo == NULL) {
            state = WUD_STATE_SYNC_ERROR;
        } else if (pInfo->status != 0) {
            state = WUD_STATE_SYNC_ERROR;
        } else {
            memcpy(pInfo, &_work, sizeof(*pInfo));
            WUDiRegisterDevice(pInfo->devAddr);
            WUDiMoveTopStdDevInfoPtr(pInfo);
            _dev_handle_to_bda[pInfo->devHandle] = pInfo->devAddr;
            state = WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM;
        }

        p->syncState = state;
        break;
    }

    case WUD_STATE_SYNC_CHANGE_SIMPLE_TO_STANDARD: {
        WUDiMoveBottomSmpDevInfoPtr(&_work);
        WUDiRemoveDevice(p->smpListTail->devInfo->devAddr);

        if (WUDiGetDevNumber() == WUD_MAX_DEV_ENTRY_FOR_STD) {
            WUDiRemoveDevice(p->stdListTail->devInfo->devAddr);
        }

        pInfo = WUDiGetNewDevInfo();
        if (pInfo == NULL) {
            state = WUD_STATE_SYNC_ERROR;
        } else if (pInfo->status != 0) {
            state = WUD_STATE_SYNC_ERROR;
        } else {
            memcpy(pInfo, &_work, sizeof(*pInfo));
            WUDiRegisterDevice(pInfo->devAddr);
            WUDiMoveTopStdDevInfoPtr(pInfo);
            _dev_handle_to_bda[pInfo->devHandle] = pInfo->devAddr;
            state = WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM;
        }

        p->syncState = state;
        break;
    }

    case WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM: {
        if (p->linkKeyState != 0) {
            next = WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM;
        } else {
            pInfo = WUDiGetDevInfo(_work.devAddr);

            DEBUGPrint("write stored link key\n");
            DEBUGPrint("addr : %02x:%02x:%02x:%02x:%02x:%02x\n", pInfo->devAddr[0], pInfo->devAddr[1], pInfo->devAddr[2], pInfo->devAddr[3],
                       pInfo->devAddr[4], pInfo->devAddr[5]);
            DEBUGPrint("key  : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", pInfo->linkKey[0],
                       pInfo->linkKey[1], pInfo->linkKey[2], pInfo->linkKey[3], pInfo->linkKey[4], pInfo->linkKey[5], pInfo->linkKey[6],
                       pInfo->linkKey[7], pInfo->linkKey[8], pInfo->linkKey[9], pInfo->linkKey[10], pInfo->linkKey[11], pInfo->linkKey[12],
                       pInfo->linkKey[13], pInfo->linkKey[14], pInfo->linkKey[15]);

            p->linkKeyState = 2;
            BTM_WriteStoredLinkKey(1, &pInfo->devAddr, &pInfo->linkKey, WUDLinkKeyCallback);

            next = WUD_STATE_SYNC_WAIT_FOR_STORING;
        }

        p->syncState = next;
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_STORING: {
        if (p->linkKeyState == 0) {
            next = WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND;
        } else {
            next = WUD_STATE_SYNC_WAIT_FOR_STORING;
        }

        p->syncState = next;
        break;
    }

    case WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND: {
        p->syncState = WUDiSyncStoreDevInfo();
        break;
    }

    case WUD_STATE_SYNC_SC_FLUSH: {
        enabled = OSDisableInterrupts();
        p->syncState = WUD_STATE_SYNC_13;
        OSRestoreInterrupts(enabled);

        SCFlushAsync((SCFlushCallback)WUDiSyncFlushCallback);
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_READING: {
        next = WUD_STATE_SYNC_WAIT_FOR_READING;

        if (p->linkKeyState == 0) {
            next = WUD_STATE_SYNC_VIRGIN_STANDARD;
            WUDiRemoveDevice(p->pairAddr);
        }

        p->syncState = next;
        break;
    }

    case WUD_STATE_SYNC_COMPLETE: {
        p->syncedNum++;
        memset(&_work, 0, sizeof(_work));

        if (p->syncType == WUD_SYNC_TYPE_STANDARD) {
            next = WUD_STATE_SYNC_DONE;
        } else {
            next = WUD_STATE_SYNC_PREPARE_SEARCH;
        }

        p->syncState = next;
        break;
    }

    case WUD_STATE_SYNC_ERROR: {
        memset(&_work, 0, sizeof(_work));

        if (p->syncType == WUD_SYNC_TYPE_STANDARD) {
            next = WUD_STATE_SYNC_DONE;
        } else {
            next = WUD_STATE_SYNC_PREPARE_SEARCH;
        }

        p->syncState = next;
        break;
    }

    case WUD_STATE_SYNC_DONE: {
        p->syncState = WUDiSyncFinish();
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT: {
        if (WUDiGetLinkNum() == 4 && WUDiGetConnNumber() == 4) {
            DEBUGPrint("Cancel searching because 4 connections exist.\n");
            BTA_DmSearchCancel();
            next = WUD_STATE_SYNC_CANCEL_SEARCH;
        } else {
            next = WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT;
        }

        p->syncState = next;
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_INCOMING: {
        next = WUD_STATE_SYNC_WAIT_FOR_INCOMING;

        if (WUDiGetLinkNum() == 4 && WUDiGetConnNumber() == 4) {
            next = WUD_STATE_SYNC_DONE;
        }

        if (--_wcb.incomeCnt < 0) {
            next = WUD_STATE_SYNC_PREPARE_SEARCH;
        }

        p->syncState = next;
        break;
    }

    case WUD_STATE_SYNC_6:
    case 13:
    case WUD_STATE_SYNC_CANCEL_SEARCH: {
        break;
    }

    default: {
        DEBUGPrint("WARNING: Illigal status\n");
        break;
    }
    }
}

static void WUDiSyncAlarmHandler(OSAlarm* alarm, OSContext* context) {
    OSSwitchFiberEx((u32)alarm, (u32)context, 0, 0, (u32)WUDiSyncStateMachine, (u32)_fiberStack + sizeof(_fiberStack));
}

// delete state 2: drop every connection
static u8 WUDiDeleteDisconnectAll(void) {
    int i;
    WUDDevInfo* pInfo;
    WUDCB* p = &_wcb;

    pInfo = p->stdDevs;
    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++, pInfo++) {
        if (pInfo->status > 1) {
            btm_remove_acl(pInfo->devAddr);
        }
    }

    pInfo = p->smpDevs;
    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++, pInfo++) {
        if (pInfo->status > 1) {
            btm_remove_acl(pInfo->devAddr);
        }
    }

    return WUD_STATE_DELETE_CLEANUP_DATABASE;
}

// delete state 3: forget every device
static u8 WUDiDeleteCleanupDatabase(void) {
    int i;
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;

    if (WUDiGetLinkNum() != 0) {
        return WUD_STATE_DELETE_CLEANUP_DATABASE;
    }

    pInfo = p->stdDevs;
    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++, pInfo++) {
        if (pInfo->status == 1) {
            WUDiRemoveDevice(pInfo->devAddr);
        }
    }

    pInfo = p->smpDevs;
    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++, pInfo++) {
        if (pInfo->status == 1) {
            WUDiRemoveDevice(pInfo->devAddr);
        }
    }

    return WUD_STATE_DELETE_CLEANUP_SETTING;
}

static void WUDiDeleteStateMachine(void) {
    WUDCB* p = &_wcb;
    BOOL ret1 = FALSE;
    BOOL ret2;
    BOOL enabled;
    u8 connectable;
    WUDClearDeviceCallback cb;

    p->syncState = 0;

    switch (p->deleteState) {
    case WUD_STATE_DELETE_DISALLOW_INCOMING: {
        BTA_DmSetVisibility(FALSE, FALSE);
        p->deleteState = WUD_STATE_DELETE_DISCONNECT_ALL;
        break;
    }

    case WUD_STATE_DELETE_DISCONNECT_ALL: {
        p->deleteState = WUDiDeleteDisconnectAll();
        break;
    }

    case WUD_STATE_DELETE_CLEANUP_DATABASE: {
        p->deleteState = WUDiDeleteCleanupDatabase();
        break;
    }

    case WUD_STATE_DELETE_CLEANUP_SETTING: {
        if (SCCheckStatus() != SC_STATUS_BUSY) {
            memset(&_scArray, 0, 0x461);
            memset(&_scCmpArray, 0, 0x205);

            ret1 = SCSetBtDeviceInfoArray(&_scArray);
            ret2 = SCSetBtCmpDevInfoArray(&_scCmpArray);

            if (ret1 | ret2) {
                p->deleteState = WUD_STATE_DELETE_6;
                SCFlushAsync((SCFlushCallback)WUDiDeleteFlushCallback);
            } else {
                p->deleteState = WUD_STATE_DELETE_DONE;
            }
        }
        break;
    }

    case WUD_STATE_DELETE_DONE: {
        enabled = OSDisableInterrupts();
        connectable = p->connectable;
        OSRestoreInterrupts(enabled);

        enabled = OSDisableInterrupts();
        p->discoverable = ret1;
        p->connectable = connectable;
        OSRestoreInterrupts(enabled);

        BTA_DmSetVisibility(FALSE, connectable);

        OSCancelAlarm(&p->alarm);

        cb = p->clearDevCB;
        p->deleteState = ret1;
        if (cb != NULL) {
            cb(WUD_RESULT_DELETE_COMPLETE);
        }

        p->deleteState = 0;
        break;
    }
    }
}

static void WUDiDeleteAlarmHandler(OSAlarm* alarm, OSContext* context) {
    OSSwitchFiberEx((u32)alarm, (u32)context, 0, 0, (u32)WUDiDeleteStateMachine, (u32)_fiberStack + sizeof(_fiberStack));
}

// stack state 2: sort out the devices the controller doesn't know about
static void WUDiClearDevInfo(WUDDevInfo* pInfo) {
    WUDDevInfo* pFound;
    BOOL enabled;

    pFound = WUDiGetDevInfo(pInfo->devAddr);

    if (pFound != NULL) {
        enabled = OSDisableInterrupts();
        memset(pFound, 0, sizeof(*pFound));
        OSRestoreInterrupts(enabled);
    }
}

static u8 WUDiStackCheckDeviceInfo(void) {
    WUDDevInfo* pInfo;
    WUDCB* p = &_wcb;
    int i;

    if (p->linkKeyState == 0) {
        for (i = 0; i < WUD_MAX_DEV_ENTRY; i++) {
            pInfo = WUDiGetDevInfoIndex(i);

            if (pInfo->status != 0) {
                if (pInfo->UNK_0x5C == 1) {
                    p->linkKeyState = WUD_STATE_LINK_KEY_DELETING;
                    BTM_DeleteStoredLinkKey(pInfo->devAddr, WUDLinkKeyCallback);
                    pInfo->UNK_0x5C = 0;
                    return WUD_STATE_STACK_CHECK_DEVICE_INFO;
                } else if (pInfo->UNK_0x5C != 3) {
                    WUDiClearDevInfo(pInfo);
                }
            }
        }

        return WUD_STATE_STACK_DONE;
    }

    return WUD_STATE_STACK_CHECK_DEVICE_INFO;
}

static void WUDiStackStateMachine(void) {
    WUDCB* p = &_wcb;
    tBTM_VERSION_INFO version;
    u8 next;

    switch (p->stackState) {
    case WUD_STATE_STACK_GET_STORED_LINK_KEY: {
        if (p->hhFlags == 1) {
            p->linkKeyState = WUD_STATE_LINK_KEY_READING;
            BTM_ReadStoredLinkKey(NULL, WUDLinkKeyCallback);
        }

        p->stackState = WUD_STATE_STACK_CHECK_DEVICE_INFO;
        break;
    }

    case WUD_STATE_STACK_CHECK_DEVICE_INFO: {
        p->stackState = WUDiStackCheckDeviceInfo();
        break;
    }

    case WUD_STATE_STACK_DONE: {
        if (p->linkKeyState == 0) {
            OSCancelAlarm(&p->alarm);

            BTM_ReadLocalVersion(&version);

            DEBUGPrint("hci_version   : %02x\n", version.hci_version);
            DEBUGPrint("hci_revision  : %04x\n", version.hci_revision);
            DEBUGPrint("lmp_version   : %02x\n", version.lmp_version);
            DEBUGPrint("lmp_subversion: %04x\n", version.lmp_subversion);
            DEBUGPrint("manufacturer  : %04x\n", version.manufacturer);
            DEBUGPrint(" ==> 2045 firmware ver.002.003.014.%d\n", version.hci_revision & 0xFFF);

            if ((version.hci_revision & 0xFFF) == 0xA7) {
                DEBUGPrint("WUDiAppendRuntimePatch()\n");

                sPatchAddr = (((((_patchFirst[3] << 8) + _patchFirst[2]) << 8) + _patchFirst[1]) << 8) + _patchFirst[0];
                sPatchLen = (((((_patchFirst[7] << 8) + _patchFirst[6]) << 8) + _patchFirst[5]) << 8) + _patchFirst[4];

                BTM_VendorSpecificCommand(0xFC4F, 1, &sVscParam, WUDiPatchFirstCallback);
            } else {
                WUDiInitSub();
            }

            next = 4;
        } else {
            next = WUD_STATE_STACK_CHECK_DEVICE_INFO;
        }

        p->stackState = next;
        break;
    }
    }
}

static void WUDiStackAlarmHandler(OSAlarm* alarm, OSContext* context) {
    OSSwitchFiberEx((u32)alarm, (u32)context, 0, 0, (u32)WUDiStackStateMachine, (u32)_fiberStack + sizeof(_fiberStack));
}

// init state 2: pull the paired wiimotes out of the sc settings
static BOOL WUDiIsStoredStd(u8* addr) {
    int j;

    for (j = 0; j < _scArray.num; j++) {
        if (memcmp(_scArray.info[j].bd_addr, addr, WUD_ADDR_LEN) == 0) {
            return TRUE;
        }
    }

    return FALSE;
}

static u8 WUDiLoadDevInfoFromSC(void) {
    BD_ADDR zero;
    int i;
    int num;
    int num2;
    WUDDevInfo* pInfo;

    memset(&_scArray, 0, 0x461);
    memset(&_scCmpArray, 0, 0x205);
    memset(zero, 0, sizeof(zero));

    SCGetBtDeviceInfoArray(&_scArray);
    SCGetBtCmpDevInfoArray(&_scCmpArray);

    num = _scArray.num;
    _wcb.syncType = WUD_SYNC_TYPE_STANDARD;

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++) {
        if (num == 0) {
            break;
        }

        if (memcmp(_scArray.info[i].bd_addr, zero, WUD_ADDR_LEN) != 0) {
            pInfo = WUDiGetNewDevInfo();

            if (pInfo != NULL) {
                memcpy(pInfo->devAddr, _scArray.info[i].bd_addr, WUD_ADDR_LEN);
                memcpy(pInfo, _scArray.info[i].bd_name, sizeof(_scArray.info[i].bd_name));

                pInfo->status = 1;
                pInfo->sync_type = 0;
                pInfo->UNK_0x5C = 2;

                if (memcmp(pInfo, "Nintendo RVL-CNT-01", 19) == 0) {
                    pInfo->subclass = 2;
                    pInfo->hhAttrMask = 0x8074;
                    pInfo->appID = 3;
                }

                DEBUGPrint("addr : %02x:%02x:%02x:%02x:%02x:%02x\n", pInfo->devAddr[0], pInfo->devAddr[1], pInfo->devAddr[2], pInfo->devAddr[3],
                           pInfo->devAddr[4], pInfo->devAddr[5]);
                DEBUGPrint("name : %s\n", pInfo);

                num--;
            }
        }
    }

    _wcb.syncType = WUD_SYNC_TYPE_SIMPLE;
    num2 = _scCmpArray.num;

    for (i = 5; i >= 0; i--) {
        if (num2 == 0) {
            break;
        }

        if (memcmp(_scCmpArray.info[i].bd_addr, zero, WUD_ADDR_LEN) != 0) {
            if (!WUDiIsStoredStd(_scCmpArray.info[i].bd_addr)) {
                pInfo = WUDiGetNewDevInfo();

                if (pInfo != NULL) {
                    memcpy(pInfo->devAddr, _scCmpArray.info[i].bd_addr, WUD_ADDR_LEN);
                    memcpy(pInfo, _scCmpArray.info[i].bd_name, sizeof(_scCmpArray.info[i].bd_name));
                    memcpy(pInfo->linkKey, _scCmpArray.info[i].link_key, sizeof(_scCmpArray.info[i].link_key));

                    pInfo->status = 1;
                    pInfo->sync_type = 1;
                    pInfo->UNK_0x5C = 3;

                    if (memcmp(pInfo, "Nintendo RVL-CNT-01", 19) == 0) {
                        pInfo->subclass = 2;
                        pInfo->hhAttrMask = 0x8074;
                        pInfo->appID = 3;
                    }

                    DEBUGPrint("addr : %02x:%02x:%02x:%02x:%02x:%02x\n", pInfo->devAddr[0], pInfo->devAddr[1], pInfo->devAddr[2],
                               pInfo->devAddr[3], pInfo->devAddr[4], pInfo->devAddr[5]);
                    DEBUGPrint("name : %s\n", pInfo);

                    WUDiMoveTopSmpDevInfoPtr(pInfo);

                    num2--;
                }
            }
        }
    }

    _wcb.syncType = WUD_SYNC_TYPE_STANDARD;
    _wcb.initState = 3;

    memset(&_scCmpArray, 0, 0x205);
    SCSetBtCmpDevInfoArray(&_scCmpArray);

    SCFlushAsync((SCFlushCallback)WUDiInitFlushCallback);

    return 4;
}

static void WUDiInitStateMachine(void) {
    u8 next;
    WUDCB* p = &_wcb;
    u32 elapsed;

    switch (p->initState) {
    case 1: {
        next = 1;

        elapsed = ((u32)__OSGetSystemTime() - lbl_805A0674) / (OS_TIME_SPEED / 1000);

        if ((s32)(500 - elapsed) < 0) {
            if (SCCheckStatus() != SC_STATUS_BUSY) {
                WUDiInitCore();
                next = 2;
            }
        }

        p->initState = next;
        break;
    }

    case 2: {
        WUDiLoadDevInfoFromSC();
        break;
    }

    case 4: {
        OSCancelAlarm(&p->alarm);

        p->libStatus = 1;
        BTA_EnableBluetooth(WUDSecurityCallback);

        p->initState = WUD_INIT_STATE_RUNNING;
        break;
    }
    }
}

static void WUDiInitAlarmHandler(OSAlarm* alarm, OSContext* context) {
    OSSwitchFiberEx((u32)alarm, (u32)context, 0, 0, (u32)WUDiInitStateMachine, (u32)_fiberStack + sizeof(_fiberStack));
}

static void WUDiShutdownStateMachine(void) {
    WUDCB* p = &_wcb;
    BOOL ok;

    switch (p->shutdownState) {
    case WUD_STATE_SHUTDOWN_STORE_SETTINGS: {
        if (SCCheckStatus() != SC_STATUS_BUSY) {
            ok = SCSetBtDeviceInfoArray(&_scArray) & 1;
            ok &= SCSetBtCmpDevInfoArray(&_scCmpArray);

            if (ok) {
                _wcb.shutdownState = WUD_STATE_SHUTDOWN_FLUSH_SETTINGS;
                SCFlushAsync((SCFlushCallback)WUDiShutFlushCallback);
            } else {
                _wcb.shutdownState = WUD_STATE_SHUTDOWN_DONE;
            }
        }
        break;
    }

    case WUD_STATE_SHUTDOWN_DONE: {
        OSCancelAlarm(&_wcb.alarm);
        BTA_DisableBluetooth();
        break;
    }
    }
}

static void WUDiShutdownAlarmHandler(OSAlarm* alarm, OSContext* context) {
    OSSwitchFiberEx((u32)alarm, (u32)context, 0, 0, (u32)WUDiShutdownStateMachine, (u32)_fiberStack + sizeof(_fiberStack));
}

static void WUDiClearDevices(void) {
    WUDCB* p = &_wcb;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    memset(p->stdDevs, 0, sizeof(p->stdDevs));
    memset(p->smpDevs, 0, sizeof(p->smpDevs));

    p->devNums = 0;
    p->devSmpNums = 0;
    p->connectedNum = 0;
    p->linkedNum = 0;
    p->syncedNum = 0;

    OSRestoreInterrupts(enabled);
}

static void WUDiInitCore(void) {
    WUDCB* p = &_wcb;
    int i;

    DEBUGPrint("InitCore\n");

    for (i = 0; i < WUD_MAX_DEV_ENTRY; i++) {
        _dev_handle_to_bda[i] = NULL;
        _dev_handle_queue_size[i] = 0;
        _dev_handle_notack_num[i] = 0;
    }

    p->smpListTail = &p->smpList[WUD_MAX_DEV_ENTRY_FOR_SMP - 1];
    p->smpListHead = &p->smpList[0];

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++) {
        p->smpList[i].devInfo = &p->smpDevs[WUD_MAX_DEV_ENTRY_FOR_SMP - 1 - i];
        p->smpList[i].prev = i == 0 ? NULL : &p->smpList[i - 1];
        p->smpList[i].next = i == WUD_MAX_DEV_ENTRY_FOR_SMP - 1 ? NULL : &p->smpList[i + 1];
    }

    p->stdListTail = &p->stdList[WUD_MAX_DEV_ENTRY_FOR_STD - 1];
    p->stdListHead = &p->stdList[0];

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++) {
        p->stdList[i].devInfo = &p->stdDevs[i];
        p->stdList[i].prev = i == 0 ? NULL : &p->stdList[i - 1];
        p->stdList[i].next = i == WUD_MAX_DEV_ENTRY_FOR_STD - 1 ? NULL : &p->stdList[i + 1];
    }

    p->syncState = 0;
    p->linkKeyState = 0;
    p->deleteState = 0;
    p->stackState = 0;
    p->initState = 0;
    p->shutdownState = 0;
    p->syncSkipChecks = 0;
    p->syncType = WUD_SYNC_TYPE_STANDARD;
    p->syncLoopNum = 1;
    p->connectable = FALSE;
    p->discoverable = FALSE;
    p->libStatus = 0;
    p->pmID = 0;
    p->syncRssi = -65;

    memset(p->hostAddr, 0, WUD_ADDR_LEN);
    memset(p->pairAddr, 0, WUD_ADDR_LEN);

    p->bufferStatus0 = 0;
    p->bufferStatus1 = 10;

    WUDiClearDevices();
}

BOOL WUDInit(void) {
    WUDCB* p = &_wcb;

    if (sInitialized) {
        return FALSE;
    }

    DEBUGPrint("BTA_Init() is started\n");

    BTE_LoadStack();

    bta_sys_set_trace_level(sTraceLevel);
    L2CA_SetTraceLevel(sTraceLevel);
    SDP_SetTraceLevel(sTraceLevel);

    DEBUGPrint("BTA_Init() is done\n");

    p->syncStdCB = NULL;
    p->syncSmpCB = NULL;
    p->clearDevCB = NULL;
    p->hidConnCB = NULL;
    p->hidRecvCB = NULL;

    p->initState = 1;

    SCInit();

    OSCreateAlarm(&p->alarm);
    OSSetPeriodicAlarm(&p->alarm, OSGetTime(), OS_MSEC_TO_TICKS(10), WUDiInitAlarmHandler);

    sInitialized = TRUE;
    return TRUE;
}

void WUDRegisterAllocator(WUDAllocFunc pAllocFunc, WUDFreeFunc pFreeFunc) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    _wcb.allocFunc = pAllocFunc;
    _wcb.freeFunc = pFreeFunc;
    OSRestoreInterrupts(enabled);
}

u32 WUDGetAllocatedMemSize(void) {
    return BTE_GetMemSize();
}

void WUDShutdown(BOOL saveSimpleDevs) {
    WUDCB* p = &_wcb;
    WUDDevInfoList* pList;
    BOOL enabled;
    int i;

    DEBUGPrint("WUDShutdown()\n");

    enabled = OSDisableInterrupts();

    if (WUDiIsBusy()) {
        OSCancelAlarm(&p->alarm);
    }

    memset(_scArray.info, 0, WUD_MAX_DEV_ENTRY_FOR_STD * sizeof(_scArray.info[0]));

    i = 0;
    for (pList = _wcb.stdListHead; pList != NULL; pList = pList->next) {
        memcpy(_scArray.info[i].bd_addr, pList->devInfo->devAddr, WUD_ADDR_LEN);
        memcpy(_scArray.info[i].bd_name, pList->devInfo, sizeof(_scArray.info[i].bd_name));
        i++;
    }

    _scArray.num = WUDiGetDevNumber();

    memset(_scCmpArray.info, 0, sizeof(_scCmpArray.info));

    if (saveSimpleDevs) {
        i = 0;
        for (pList = _wcb.smpListHead; pList != NULL; pList = pList->next) {
            memcpy(_scCmpArray.info[i].bd_addr, pList->devInfo->devAddr, WUD_ADDR_LEN);
            memcpy(_scCmpArray.info[i].bd_name, pList->devInfo, sizeof(_scCmpArray.info[i].bd_name));
            memcpy(_scCmpArray.info[i].link_key, pList->devInfo->linkKey, sizeof(_scCmpArray.info[i].link_key));
            i++;
        }

        _scCmpArray.num = WUDiGetDevSmpNumber();
    } else {
        _scCmpArray.num = 0;
    }

    p->shutdownState = 1;

    OSCreateAlarm(&p->alarm);
    OSSetPeriodicAlarm(&p->alarm, OSGetTime(), OS_MSEC_TO_TICKS(10), WUDiShutdownAlarmHandler);

    p->libStatus = 4;

    OSRestoreInterrupts(enabled);
}

WUDLibStatus WUDGetStatus(void) {
    BOOL enabled;
    s8 status;

    enabled = OSDisableInterrupts();
    status = _wcb.libStatus;
    OSRestoreInterrupts(enabled);

    return status;
}

u8 WUDGetBufferStatus(void) {
    BOOL enabled;
    u8 status;

    enabled = OSDisableInterrupts();
    status = _wcb.bufferStatus1 - _wcb.bufferStatus0;
    OSRestoreInterrupts(enabled);

    return status;
}

void WUDSetSniffMode(BD_ADDR addr, s32 interval) {
    WUDCB* p = &_wcb;
    tBTM_PM_PWR_MD mode;

    if (interval > 0) {
        mode.mode = BTM_PM_MD_SNIFF;
    } else {
        mode.mode = BTM_PM_MD_ACTIVE;
    }

    mode.max = interval;
    mode.min = interval;
    mode.attempt = 1;
    mode.timeout = 0;

    BTM_SetPowerMode(p->pmID, addr, &mode);
}

WUDSyncDeviceCallback WUDSetSyncSimpleCallback(WUDSyncDeviceCallback pCallback) {
    WUDSyncDeviceCallback old;
    BOOL enabled;

    DEBUGPrint("WUDSetSyncDeviceCallback\n");

    enabled = OSDisableInterrupts();
    old = _wcb.syncSmpCB;
    _wcb.syncSmpCB = pCallback;
    OSRestoreInterrupts(enabled);

    return old;
}

// HYPOTHESIS: no live copy of this one, so the body is a guess from its string
WUDClearDeviceCallback WUDSetClearDeviceCallback(WUDClearDeviceCallback pCallback) {
    WUDClearDeviceCallback old;
    BOOL enabled;

    DEBUGPrint("WUDSetClearDeviceCallback\n");

    enabled = OSDisableInterrupts();
    old = _wcb.clearDevCB;
    _wcb.clearDevCB = pCallback;
    OSRestoreInterrupts(enabled);

    return old;
}

// this one only shows up inlined into the vendor event handler
BOOL WUDStartSyncDevice(void) {
    BOOL ret;
    BOOL enabled;
    WUDSyncDeviceCallback cb;

    DEBUGPrint("WUDStartSyncDevice()\n");

    ret = WUDiStartSync(WUD_SYNC_TYPE_STANDARD, 3, 0, 0);

    enabled = OSDisableInterrupts();
    cb = _wcb.syncStdCB;
    OSRestoreInterrupts(enabled);

    if (!ret && cb != NULL) {
        cb(WUD_RESULT_SYNC_BUSY, 0);
    }

    return ret;
}

static BOOL WUDiStartSync(int type, s8 loopNum, u8 checkName, u8 skipChecks) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    BOOL ret = FALSE;
    u32 status;

    enabled = OSDisableInterrupts();
    status = p->libStatus;
    OSRestoreInterrupts(enabled);

    if (status == WUD_LIB_STATUS_READY && !WUDiIsBusy()) {
        enabled = OSDisableInterrupts();

        sCheckName = checkName;

        p->syncState = 1;
        p->syncLoopNum = loopNum;
        p->syncType = type;
        p->syncSkipChecks = skipChecks != 0;
        p->syncedNum = 0;
        p->delayCnt = 50;
        p->incomeCnt = 200;

        OSCreateAlarm(&p->alarm);
        OSSetPeriodicAlarm(&p->alarm, OSGetTime(), OS_MSEC_TO_TICKS(20), WUDiSyncAlarmHandler);

        OSRestoreInterrupts(enabled);

        ret = TRUE;
    }

    return ret;
}

BOOL WUDStartSyncSimple(void) {
    BOOL ret;
    BOOL enabled;
    WUDSyncDeviceCallback cb;

    DEBUGPrint("WUDStartSyncSimple()\n");

    ret = WUDiStartSync(WUD_SYNC_TYPE_SIMPLE, -1, 0, 1);

    enabled = OSDisableInterrupts();
    cb = _wcb.syncSmpCB;
    OSRestoreInterrupts(enabled);

    if (!ret && cb != NULL) {
        cb(WUD_RESULT_SYNC_BUSY, 0);
    }

    return ret;
}

// HYPOTHESIS: no live copy of this one, so the body is a guess from its string
BOOL WUDStartFastSyncSimple(void) {
    BOOL ret;
    BOOL enabled;
    WUDSyncDeviceCallback cb;

    DEBUGPrint("WUDStartSyncSpDevice()\n");

    ret = WUDiStartSync(WUD_SYNC_TYPE_SIMPLE, 1, 1, 1);

    enabled = OSDisableInterrupts();
    cb = _wcb.syncSmpCB;
    OSRestoreInterrupts(enabled);

    if (!ret && cb != NULL) {
        cb(WUD_RESULT_SYNC_BUSY, 0);
    }

    return ret;
}

// HYPOTHESIS: no live copy of this one, so the body is a guess from its string
BOOL WUDCancelSyncDevice(void) {
    BOOL ret = FALSE;
    BOOL enabled;

    DEBUGPrint("WUDCancelSyncDevice()\n");

    enabled = OSDisableInterrupts();

    if (_wcb.libStatus == WUD_LIB_STATUS_READY) {
        if (_wcb.syncLoopNum != 0) {
            if (_wcb.syncState == WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT) {
                BTA_DmSearchCancel();
            }

            _wcb.syncLoopNum = 0;
        }

        ret = TRUE;
    }

    OSRestoreInterrupts(enabled);

    return ret;
}

BOOL WUDStopSyncSimple(void) {
    BOOL enabled;
    BOOL ret;
    WUDCB* p;

    DEBUGPrint("WUDStopSyncSimple()\n");

    p = &_wcb;
    ret = FALSE;

    enabled = OSDisableInterrupts();

    if (p->libStatus == WUD_LIB_STATUS_READY) {
        if (p->syncLoopNum != 0) {
            if (p->syncState == WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT) {
                BTA_DmSearchCancel();
            }

            p->syncLoopNum = 0;
        }

        ret = TRUE;
    }

    OSRestoreInterrupts(enabled);

    return ret;
}

// this one only shows up inlined into the vendor event handler
BOOL WUDStartClearDevice(void) {
    WUDCB* p = &_wcb;
    BOOL ret = FALSE;
    BOOL enabled;

    DEBUGPrint("WUDStartClearDevice()\n");

    if (WUDiGetLibStatus() == WUD_LIB_STATUS_READY) {
        if (!WUDiIsBusy()) {
            enabled = OSDisableInterrupts();

            p->deleteState = 1;

            OSCreateAlarm(&p->alarm);
            OSSetPeriodicAlarm(&p->alarm, OSGetTime(), OS_MSEC_TO_TICKS(20), WUDiDeleteAlarmHandler);

            OSRestoreInterrupts(enabled);

            ret = TRUE;
        }
    }

    return ret;
}

BOOL WUDSetDisableChannel(s8 afhChannel) {
    BOOL ret = FALSE;
    WUDCB* p = &_wcb;
    BOOL enabled;
    u32 status;
    int first;
    int last;
    int low;
    int high;
    int tmp;
    tBTM_STATUS result;

    DEBUGPrint("WUDSetDisableChannel()\n");

    if (afhChannel < 0 || afhChannel > 13) {
        return FALSE;
    }

    enabled = OSDisableInterrupts();
    status = p->libStatus;
    OSRestoreInterrupts(enabled);

    if (status == WUD_LIB_STATUS_READY) {
        if (afhChannel == 0) {
            first = 0xFF;
            last = 0xFF;
        } else {
            tmp = (afhChannel + 1) * 5;

            low = tmp - 14;
            high = tmp + 14;

            if (low < 0) {
                low = 0;
            }

            if (high > 78) {
                high = 78;
            }

            first = low;
            last = high;
        }

        result = BTM_SetAfhChannels(first, last);
        DEBUGPrint("BTM_SetAfhChannels() : %d\n", result);

        ret = TRUE;
    }

    return ret;
}

WUDHidRecvCallback WUDSetHidRecvCallback(WUDHidRecvCallback pCallback) {
    WUDHidRecvCallback old;
    BOOL enabled;

    DEBUGPrint("WUDSetHidRecvCallback()\n");

    enabled = OSDisableInterrupts();
    old = _wcb.hidRecvCB;
    _wcb.hidRecvCB = pCallback;
    OSRestoreInterrupts(enabled);

    return old;
}

WUDHidConnCallback WUDSetHidConnCallback(WUDHidConnCallback pCallback) {
    WUDHidConnCallback old;
    BOOL enabled;

    DEBUGPrint("WUDSetHidConnCallback()\n");

    enabled = OSDisableInterrupts();
    old = _wcb.hidConnCB;
    _wcb.hidConnCB = pCallback;
    OSRestoreInterrupts(enabled);

    return old;
}

// the bcm2045 firmware patch, plus a list of single-shot patches to apply after it
u8 _patchFirst[0xBC] = {
    0x70, 0x99, 0x08, 0x00, 0xB4, 0x00, 0x00, 0x00, 0x88, 0x43, 0xD1, 0x07,
    0x09, 0x0C, 0x08, 0x43, 0xA0, 0x62, 0x19, 0x23, 0xDB, 0x01, 0x33, 0x80,
    0x7C, 0xF7, 0x88, 0xF8, 0x28, 0x76, 0x80, 0xF7, 0x17, 0xFF, 0x43, 0x78,
    0xEB, 0x70, 0x19, 0x23, 0xDB, 0x01, 0x33, 0x87, 0x7C, 0xF7, 0xBC, 0xFB,
    0x0B, 0x60, 0xA3, 0x7B, 0x01, 0x49, 0x0B, 0x60, 0x90, 0xF7, 0x96, 0xFB,
    0xD8, 0x1D, 0x08, 0x00, 0x00, 0xF0, 0x04, 0xF8, 0x00, 0x23, 0x79, 0xF7,
    0xE3, 0xFA, 0x00, 0x00, 0x00, 0xB5, 0x00, 0x23, 0x11, 0x49, 0x0B, 0x60,
    0x1D, 0x21, 0xC9, 0x03, 0x0B, 0x60, 0x7D, 0x20, 0x80, 0x01, 0x01, 0x38,
    0xFD, 0xD1, 0x0E, 0x4B, 0x0E, 0x4A, 0x13, 0x60, 0x47, 0x20, 0x00, 0x21,
    0x96, 0xF7, 0x96, 0xFF, 0x46, 0x20, 0x00, 0x21, 0x96, 0xF7, 0x92, 0xFF,
    0x0A, 0x4A, 0x13, 0x68, 0x0A, 0x48, 0x03, 0x40, 0x13, 0x60, 0x0A, 0x4A,
    0x13, 0x68, 0x0A, 0x48, 0x03, 0x40, 0x13, 0x60, 0x09, 0x4A, 0x13, 0x68,
    0x09, 0x48, 0x03, 0x40, 0x13, 0x60, 0x00, 0xBD, 0x24, 0x80, 0x0E, 0x00,
    0x81, 0x03, 0x0F, 0xFE, 0x5C, 0x00, 0x0F, 0x00, 0x60, 0xFC, 0x0E, 0x00,
    0xFE, 0xFF, 0x00, 0x00, 0xFC, 0xFC, 0x0E, 0x00, 0xFF, 0x9F, 0x00, 0x00,
    0x30, 0xFC, 0x0E, 0x00, 0x7F, 0xFF, 0x00, 0x00,
};

u8 _patchList[0x5C] = {
    0x07, 0x20, 0xBC, 0x65, 0x01, 0x00, 0x84, 0x42, 0x09, 0xD2, 0x84, 0x42,
    0x09, 0xD1, 0x21, 0x84, 0x5A, 0x00, 0x00, 0x83, 0xF0, 0x74, 0xFF, 0x09,
    0x0C, 0x08, 0x43, 0x22, 0x00, 0x61, 0x00, 0x00, 0x83, 0xF0, 0x40, 0xFC,
    0x00, 0x00, 0x00, 0x00, 0x23, 0xCC, 0x9F, 0x01, 0x00, 0x6F, 0xF0, 0xE4,
    0xFC, 0x03, 0x28, 0x7D, 0xD1, 0x24, 0x3C, 0x62, 0x01, 0x00, 0x28, 0x20,
    0x00, 0xE0, 0x60, 0x8D, 0x23, 0x68, 0x25, 0x04, 0x12, 0x01, 0x00, 0x20,
    0x1C, 0x20, 0x1C, 0x24, 0xE0, 0xB0, 0x21, 0x26, 0x74, 0x2F, 0x00, 0x00,
    0x86, 0xF0, 0x18, 0xFD, 0x21, 0x4F, 0x3B, 0x60,
};

void WUDSetVisibility(u8 disc, u8 conn) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    _wcb.discoverable = disc;
    _wcb.connectable = conn;
    OSRestoreInterrupts(enabled);

    BTA_DmSetVisibility(disc, conn);
}

static void WUDiPatchDone(void* p) {
    WUDiInitSub();
}

static void WUDiSendPatchData(u8 len) {
    u8 buf[0x100];
    u32 addr;
    int i;

    addr = sPatchAddr + sPatchOffset;

    buf[0] = addr;
    buf[1] = addr >> 8;
    buf[2] = addr >> 16;
    buf[3] = addr >> 24;

    for (i = 0; i < len; i++) {
        buf[4 + i] = _patchFirst[8 + sPatchOffset + i];
    }

    sPatchOffset += len;

    BTM_VendorSpecificCommand(0xFC4C, len + 4, buf, WUDiPatchStepCallback);
}

static void WUDiPatchFirstCallback(tBTM_VSC_CMPL* p) {
    if (p == NULL) {
        BTM_DeviceReset(WUDiPatchDone);
        return;
    }

    sPatchOffset = 0;

    WUDiSendPatchData(sPatchLen < 0xFB ? sPatchLen : 0xFB);
}

static void WUDiPatchStepCallback(tBTM_VSC_CMPL* p) {
    u8 listBuf[0x100];
    u8 len;
    int count;
    u8 n;
    u32 size;

    if (p != NULL) {
        if (sPatchLen == sPatchOffset) {
            sPatchIndex = 0;
            count = ((WUDPatchList*)_patchList)->num;
            sPatchCount = count;

            n = count < WUD_MAX_PATCHES ? count : WUD_MAX_PATCHES;
            listBuf[0] = n;

            size = n * sizeof(WUDPatchCmd);
            memcpy(&listBuf[1], ((WUDPatchList*)_patchList)->cmds, size);

            sPatchIndex += n;

            BTM_VendorSpecificCommand(0xFC4F, size + 1, listBuf, WUDiPatchListCallback);
        } else {
            len = sPatchLen - sPatchOffset < 0xFB ? sPatchLen - sPatchOffset : 0xFB;

            WUDiSendPatchData(len);
        }
    } else {
        BTM_DeviceReset(WUDiPatchDone);
    }
}

static void WUDiPatchListCallback(tBTM_VSC_CMPL* p) {
    u8 buf[0x100];
    u8 n;
    u32 size;

    if (p != NULL) {
        if (sPatchCount == sPatchIndex) {
            BTM_DeviceReset(WUDiPatchDone);
        } else {
            n = sPatchCount - sPatchIndex < WUD_MAX_PATCHES ? sPatchCount - sPatchIndex : WUD_MAX_PATCHES;
            buf[0] = n;

            size = n * sizeof(WUDPatchCmd);
            memcpy(&buf[1], &((WUDPatchList*)_patchList)->cmds[sPatchIndex], size);

            sPatchIndex += n;

            BTM_VendorSpecificCommand(0xFC4F, size + 1, buf, WUDiPatchListCallback);
        }
    } else {
        BTM_DeviceReset(WUDiPatchDone);
    }
}

void WUDiInitSub(void) {
    WUDCB* p = &_wcb;
    char name[4] = "Wii";
    DEV_CLASS devClass = {0x00, 0x04, 0x48};
    BOOL enabled;
    int i;

    DEBUGPrint("start WUDiInitSub()\n");

    BTA_DmSetDeviceName(name);
    BTM_SetDeviceClass(devClass);

    BTM_RegisterForVSEvents(WUDVendorSpecificCallback);
    BTM_RegisterForDeviceStatusNotif(WUDDeviceStatusCallback);
    BTM_PmRegister(BTM_PM_REG_SET | BTM_PM_REG_NOTIF, &p->pmID, WUDPowerManagerCallback);

    BTM_WritePageTimeout(0x8000);
    BTM_SetDefaultLinkPolicy(5);
    BTM_SetDefaultLinkSuperTout(0xC80);

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++) {
        if (p->stdDevs[i].status == 1) {
            WUDiRegisterDevice(p->stdDevs[i].devAddr);
        }
    }

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++) {
        if (p->smpDevs[i].status == 1) {
            WUDiRegisterDevice(p->smpDevs[i].devAddr);
        }
    }

    enabled = OSDisableInterrupts();
    p->libStatus = 3;
    OSRestoreInterrupts(enabled);

    enabled = OSDisableInterrupts();
    _wcb.discoverable = FALSE;
    _wcb.connectable = TRUE;
    OSRestoreInterrupts(enabled);

    BTA_DmSetVisibility(FALSE, TRUE);
}

// this one only shows up inlined into the vendor event handler
void WUDiAutoSync(void) {
    WUDSyncDeviceCallback cb;
    BOOL enabled;
    s32 result;

    DEBUGPrint("WUDiAutoSync()\n");

    enabled = OSDisableInterrupts();
    cb = _wcb.syncStdCB;
    result = WUDiIsBusy() ? -1 : 0;
    OSRestoreInterrupts(enabled);

    if (cb != NULL) {
        cb(result, 0);
    } else {
        WUDStartSyncDevice();
    }
}

// HYPOTHESIS: no live copy of this one, so the body is a guess from its string
void WUDiCancelSync(void) {
    DEBUGPrint("WUDiCancelSync()\n");

    WUDCancelSyncDevice();
}

// this one only shows up inlined into the vendor event handler
void WUDiDeleteAllLinkKeys(void) {
    WUDClearDeviceCallback cb;
    BOOL enabled;
    s32 result;

    DEBUGPrint("WUDiDeleteAllLinkKeys()\n");

    enabled = OSDisableInterrupts();
    cb = _wcb.clearDevCB;
    result = WUDiIsBusy() ? -1 : 0;
    OSRestoreInterrupts(enabled);

    if (cb != NULL) {
        cb(result);
    } else {
        WUDStartClearDevice();
    }
}

void WUDiRegisterDevice(BD_ADDR addr) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    tBTA_HH_DEV_DESCR descr;
    BOOL enabled;
    tBTA_STATUS status;

    enabled = OSDisableInterrupts();

    pInfo = WUDiGetDevInfo(addr);

    status = BTA_DmAddDevice(pInfo->devAddr, pInfo->linkKey, 0, FALSE);
    DEBUGPrint("BTA_DmAddDevice(): %d\n", status);

    if (memcmp(pInfo, "Nintendo RVL-CNT", 16) == 0) {
        descr.dl_len = 0xD9;
        descr.dsc_list = _hidDescriptor;

        DEBUGPrint("BTA_HhAddDev()\n");
        BTA_HhAddDev(pInfo->devAddr, pInfo->hhAttrMask, pInfo->subclass, pInfo->appID, descr);
    }

    if (pInfo->sync_type == 0 || pInfo->sync_type == 4 || pInfo->sync_type == 2 || pInfo->sync_type == 5) {
        p->devNums++;
    } else {
        p->devSmpNums++;
    }

    OSRestoreInterrupts(enabled);
}

void WUDiRemoveDevice(BD_ADDR addr) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    BOOL enabled;
    tBTA_STATUS status;

    enabled = OSDisableInterrupts();

    DEBUGPrint("WUDiRemoveDevice : \n");

    pInfo = WUDiGetDevInfo(addr);
    if (pInfo != NULL) {
        DEBUGPrint(" handle : %d,  addr : %02x:%02x:%02x:%02x:%02x:%02x\n", pInfo->devHandle, pInfo->devAddr[0], pInfo->devAddr[1],
                   pInfo->devAddr[2], pInfo->devAddr[3], pInfo->devAddr[4], pInfo->devAddr[5]);
        DEBUGPrint("remove device info from database.\n");

        if (memcmp(pInfo, "Nintendo RVL-CNT", 16) == 0) {
            DEBUGPrint("BTA_HhRemoveDev()\n");
            DEBUGPrint(" handle : %d\n", pInfo->devHandle);

            BTA_HhRemoveDev(pInfo->devHandle);
        }

        status = BTA_DmRemoveDevice(pInfo->devAddr);
        DEBUGPrint("BTA_DmRemoveDevice(): %d\n", status);

        if (pInfo->sync_type == 0 || pInfo->sync_type == 2 || pInfo->sync_type == 4 || pInfo->sync_type == 5) {
            p->devNums--;
        } else {
            p->devSmpNums--;
        }

        memset(pInfo, 0, sizeof(*pInfo));
    }

    OSRestoreInterrupts(enabled);
}

WUDDevInfo* WUDiGetDevInfo(BD_ADDR addr) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo = NULL;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++) {
        if (memcmp(p->stdDevs[i].devAddr, addr, WUD_ADDR_LEN) == 0) {
            pInfo = &p->stdDevs[i];
            break;
        }
    }

    if (pInfo == NULL) {
        for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++) {
            if (memcmp(p->smpDevs[i].devAddr, addr, WUD_ADDR_LEN) == 0) {
                pInfo = &p->smpDevs[i];
                break;
            }
        }
    }

    OSRestoreInterrupts(enabled);

    return pInfo;
}

WUDDevInfo* WUDiGetNewDevInfo(void) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo = NULL;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    if (p->syncType == WUD_SYNC_TYPE_STANDARD) {
        for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++) {
            if (p->stdDevs[i].status == 0) {
                DEBUGPrint("new entry index is %d\n", i);

                pInfo = &p->stdDevs[i];
                break;
            }
        }
    } else {
        pInfo = p->smpListTail->devInfo;
    }

    OSRestoreInterrupts(enabled);

    return pInfo;
}

void WUDiMoveTopSmpDevInfoPtr(WUDDevInfo* pInfo) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++) {
        if (memcmp(p->smpList[i].devInfo->devAddr, pInfo->devAddr, WUD_ADDR_LEN) == 0) {
            if (memcmp(p->smpListHead->devInfo->devAddr, p->smpList[i].devInfo->devAddr, WUD_ADDR_LEN) != 0) {
                p->smpList[i].prev->next = p->smpList[i].next;

                if (memcmp(p->smpListTail->devInfo->devAddr, p->smpList[i].devInfo->devAddr, WUD_ADDR_LEN) == 0) {
                    p->smpListTail = p->smpList[i].prev;
                } else {
                    p->smpList[i].next->prev = p->smpList[i].prev;
                }

                p->smpList[i].next = p->smpListHead;
                p->smpListHead->prev = &p->smpList[i];
                p->smpListHead = &p->smpList[i];
                p->smpList[i].prev = NULL;
            }

            break;
        }
    }

    OSRestoreInterrupts(enabled);
}

void WUDiMoveBottomSmpDevInfoPtr(WUDDevInfo* pInfo) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++) {
        if (memcmp(p->smpList[i].devInfo->devAddr, pInfo->devAddr, WUD_ADDR_LEN) == 0) {
            if (memcmp(p->smpListTail->devInfo->devAddr, p->smpList[i].devInfo->devAddr, WUD_ADDR_LEN) != 0) {
                p->smpList[i].next->prev = p->smpList[i].prev;

                if (memcmp(p->smpListHead->devInfo->devAddr, p->smpList[i].devInfo->devAddr, WUD_ADDR_LEN) == 0) {
                    p->smpListHead = p->smpList[i].next;
                } else {
                    p->smpList[i].prev->next = p->smpList[i].next;
                }

                p->smpList[i].prev = p->smpListTail;
                p->smpListTail->next = &p->smpList[i];
                p->smpListTail = &p->smpList[i];
                p->smpList[i].next = NULL;
            }

            break;
        }
    }

    OSRestoreInterrupts(enabled);
}

void WUDiMoveTopOfDisconnectedSmpDevice(WUDDevInfo* pInfo) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pCur;
    int i;
    u8* pAddr;
    WUDDevInfoList* pHead;
    BOOL enabled;
    WUDDevInfoList* pSearch;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++) {
        pAddr = p->smpList[i].devInfo->devAddr;

        if (memcmp(pAddr, pInfo->devAddr, WUD_ADDR_LEN) == 0) {
            pHead = p->smpListHead;

            for (pSearch = pHead; pSearch != NULL; pSearch = pSearch->next) {
                pCur = pSearch->devInfo;

                if (memcmp(pCur->devAddr, pInfo->devAddr, WUD_ADDR_LEN) != 0 && pCur->status <= 1) {
                    if (memcmp(pHead->devInfo->devAddr, pAddr, WUD_ADDR_LEN) == 0) {
                        if (pSearch == pHead->next) {
                            break;
                        }

                        p->smpListHead = p->smpList[i].next;
                    } else {
                        p->smpList[i].prev->next = p->smpList[i].next;
                    }

                    p->smpList[i].next->prev = p->smpList[i].prev;

                    if (pSearch != p->smpListHead) {
                        p->smpList[i].prev = pSearch->prev;
                        p->smpList[i].next = pSearch;
                        pSearch->prev->next = &p->smpList[i];
                        pSearch->prev = &p->smpList[i];
                    } else {
                        p->smpList[i].prev = pSearch;
                        p->smpList[i].next = pSearch->next;
                        pSearch->next->prev = &p->smpList[i];
                        pSearch->next = &p->smpList[i];
                    }

                    break;
                }
            }
        }
    }

    OSRestoreInterrupts(enabled);
}

void WUDiMoveTopStdDevInfoPtr(WUDDevInfo* pInfo) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++) {
        if (memcmp(p->stdList[i].devInfo->devAddr, pInfo->devAddr, WUD_ADDR_LEN) == 0) {
            if (memcmp(p->stdListHead->devInfo->devAddr, p->stdList[i].devInfo->devAddr, WUD_ADDR_LEN) != 0) {
                p->stdList[i].prev->next = p->stdList[i].next;

                if (memcmp(p->stdListTail->devInfo->devAddr, p->stdList[i].devInfo->devAddr, WUD_ADDR_LEN) == 0) {
                    p->stdListTail = p->stdList[i].prev;
                } else {
                    p->stdList[i].next->prev = p->stdList[i].prev;
                }

                p->stdList[i].next = p->stdListHead;
                p->stdListHead->prev = &p->stdList[i];
                p->stdListHead = &p->stdList[i];
                p->stdList[i].prev = NULL;
            }

            break;
        }
    }

    OSRestoreInterrupts(enabled);
}

void WUDiMoveBottomStdDevInfoPtr(WUDDevInfo* pInfo) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++) {
        if (memcmp(p->stdList[i].devInfo->devAddr, pInfo->devAddr, WUD_ADDR_LEN) == 0) {
            if (memcmp(p->stdListTail->devInfo->devAddr, p->stdList[i].devInfo->devAddr, WUD_ADDR_LEN) != 0) {
                p->stdList[i].next->prev = p->stdList[i].prev;

                if (memcmp(p->stdListHead->devInfo->devAddr, p->stdList[i].devInfo->devAddr, WUD_ADDR_LEN) == 0) {
                    p->stdListHead = p->stdList[i].next;
                } else {
                    p->stdList[i].prev->next = p->stdList[i].next;
                }

                p->stdList[i].prev = p->stdListTail;
                p->stdListTail->next = &p->stdList[i];
                p->stdListTail = &p->stdList[i];
                p->stdList[i].next = NULL;
            }

            break;
        }
    }

    OSRestoreInterrupts(enabled);
}

void WUDiMoveTopOfDisconnectedStdDevice(WUDDevInfo* pInfo) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pCur;
    int i;
    u8* pAddr;
    WUDDevInfoList* pHead;
    BOOL enabled;
    WUDDevInfoList* pSearch;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++) {
        pAddr = p->stdList[i].devInfo->devAddr;

        if (memcmp(pAddr, pInfo->devAddr, WUD_ADDR_LEN) == 0) {
            pHead = p->stdListHead;

            for (pSearch = pHead; pSearch != NULL; pSearch = pSearch->next) {
                pCur = pSearch->devInfo;

                if (memcmp(pCur->devAddr, pInfo->devAddr, WUD_ADDR_LEN) != 0 && pCur->status <= 1) {
                    if (memcmp(pHead->devInfo->devAddr, pAddr, WUD_ADDR_LEN) == 0) {
                        if (pSearch == pHead->next) {
                            break;
                        }

                        p->stdListHead = p->stdList[i].next;
                    } else {
                        p->stdList[i].prev->next = p->stdList[i].next;
                    }

                    p->stdList[i].next->prev = p->stdList[i].prev;

                    if (pSearch != p->stdListHead) {
                        p->stdList[i].prev = pSearch->prev;
                        p->stdList[i].next = pSearch;
                        pSearch->prev->next = &p->stdList[i];
                        pSearch->prev = &p->stdList[i];
                    } else {
                        p->stdList[i].prev = pSearch;
                        p->stdList[i].next = pSearch->next;
                        pSearch->next->prev = &p->stdList[i];
                        pSearch->next = &p->stdList[i];
                    }

                    break;
                }
            }
        }
    }

    OSRestoreInterrupts(enabled);
}

BOOL WUDIsBusy(void) {
    return WUDiIsBusy();
}

static void WUDiUnloadCallback(int result) {
    WUDCB* p = &_wcb;

    if (result == 0) {
        sInitialized = FALSE;
        p->libStatus = 0;
    } else {
        DEBUGPrint("WARNING: USB_CLOSE_FAILURE!\n");
    }
}

static void WUDSecurityCallback(tBTA_DM_SEC_EVT event, tBTA_DM_SEC* pData) {
    WUDCB* p = &_wcb;
    u8* d = (u8*)pData;
    WUDDevInfo* pInfo;
    BOOL enabled;
    u8 pin[6];
    u8* pAddr;
    u8 i;

    DEBUGPrint("WUDSecurityCallback: ");

    switch (event) {
    case BTA_DM_ENABLE_EVT:
        memcpy(p->hostAddr, d, WUD_ADDR_LEN);

        DEBUGPrint("BTA_ENABLE_EVT\n");
        DEBUGPrint("host : %02x:%02x:%02x:%02x:%02x:%02x\n", p->hostAddr[0], p->hostAddr[1], p->hostAddr[2], p->hostAddr[3],
                   p->hostAddr[4], p->hostAddr[5]);

        BTA_HhEnable(0x12, WUDHidHostCallback);

        p->stackState = 1;

        OSCreateAlarm(&p->alarm);
        OSSetPeriodicAlarm(&p->alarm, OSGetTime(), OS_MSEC_TO_TICKS(10), WUDiStackAlarmHandler);

        p->libStatus = 2;
        break;

    case BTA_DM_DISABLE_EVT:
        DEBUGPrint("BTA_DISABLE_EVT\n");

        BTE_UnloadStack(WUDiUnloadCallback);
        break;

    case BTA_DM_PIN_REQ_EVT:
        DEBUGPrint("BTA_DM_PIN_REQ_EVT\n");

        pAddr = p->syncType == 0 ? p->hostAddr : d;

        if (_work.sync_type == 4) {
            WUDiRemoveDevice(_work.devAddr);
            btm_remove_acl(_work.devAddr);
        }

        _work.UNK_0x5A = 1;

        enabled = OSDisableInterrupts();

        pin[0] = pAddr[5];
        pin[1] = pAddr[4];
        pin[2] = pAddr[3];
        pin[3] = pAddr[2];
        pin[4] = pAddr[1];
        pin[5] = pAddr[0];

        OSRestoreInterrupts(enabled);

        BTA_DmPinReply(d, TRUE, 6, pin);
        break;

    case BTA_DM_AUTH_CMPL_EVT:
        DEBUGPrint("BTA_DM_AUTH_CMPL_EVT\n");
        DEBUGPrint("  addr : %02x:%02x:%02x:%02x:%02x:%02x\n", d[0], d[1], d[2], d[3], d[4], d[5]);
        DEBUGPrint("  key  : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", d[0xFF], d[0x100],
                   d[0x101], d[0x102], d[0x103], d[0x104], d[0x105], d[0x106], d[0x107], d[0x108], d[0x109], d[0x10A], d[0x10B],
                   d[0x10C], d[0x10D], d[0x10E]);
        DEBUGPrint("  result = %d\n", d[0x10F]);

        if (d[0x10F] != 0) {
            pInfo = WUDiGetDevInfo(d);

            if (memcmp(_work.devAddr, d, WUD_ADDR_LEN) == 0) {
                _work.status = 0xC;

                if (pInfo == NULL) {
                    pInfo = &_work;
                }
            }

            memcpy(pInfo->linkKey, &d[0xFF], sizeof(pInfo->linkKey));
        }
        break;

    case BTA_DM_AUTHORIZE_EVT:
        DEBUGPrint("BTA_DM_AUTHORIZE_EVT\n");
        break;

    case BTA_DM_LINK_UP_EVT:
        DEBUGPrint("BTA_DM_LINK_UP_EVT\n");
        DEBUGPrint("   addr : %02x:%02x:%02x:%02x:%02x:%02x\n", d[0], d[1], d[2], d[3], d[4], d[5]);

        pInfo = WUDiGetDevInfo(d);

        if (pInfo == NULL && memcmp(d, _work.devAddr, WUD_ADDR_LEN) == 0) {
            pInfo = &_work;
        }

        if (pInfo == NULL || WUDiGetLinkNum() == 4) {
            btm_remove_acl(d);

            DEBUGPrint("%s --> %02x:%02x:%02x:%02x:%02x:%02x\n", pInfo == NULL ? "not paired" : "4 links exist", d[0], d[1], d[2],
                       d[3], d[4], d[5]);
        } else {
            pInfo->status = pInfo->status == 2 ? 0xC : 3;
            p->linkedNum++;
        }
        break;

    case BTA_DM_LINK_DOWN_EVT:
        DEBUGPrint("BTA_DM_LINK_DOWN_EVT\n");
        DEBUGPrint("   addr : %02x:%02x:%02x:%02x:%02x:%02x\n", d[0], d[1], d[2], d[3], d[4], d[5]);
        DEBUGPrint("result: %d\n", d[6]);

        pInfo = WUDiGetDevInfo(d);

        if (pInfo != NULL) {
            pInfo->status = 1;
            p->linkedNum--;

            if (memcmp(_work.devAddr, d, WUD_ADDR_LEN) == 0) {
                p->syncState = 0xFF;
            }

            if (d[6] == 0x15) {
                for (i = 0; i < 4; i++) {
                    if (memcmp(_scArray.info[i + 10].bd_addr, d, WUD_ADDR_LEN) == 0) {
                        memset(&_scArray.info[i + 10], 0, sizeof(_scArray.info[0]));
                        lbl_805A0C71 = 1;
                    }
                }
            }
        } else if (memcmp(_work.devAddr, d, WUD_ADDR_LEN) == 0) {
            p->syncState = 0xFF;
            p->linkedNum--;
        } else {
            DEBUGPrint("this device in not paired\n");
        }

        if (p->linkedNum >= 250) {
            OSReport("WARNING: link num count is reset.\n");
            p->linkedNum = 0;
        }
        break;

    case BTA_DM_SIG_STRENGTH_EVT:
        DEBUGPrint("BTA_DM_SIG_STRENGTH_EVT\n");
        break;

    case BTA_DM_BUSY_LEVEL_EVT:
        DEBUGPrint("BTA_DM_BUSY_LEVEL_EVT\n");
        break;
    }
}

static void WUDSearchCallback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH* pData) {
    u8* d = (u8*)pData;
    WUDDiscResp* pResp = (WUDDiscResp*)pData;
    u16 timeout;

    switch (event) {
    case BTA_DM_INQ_RES_EVT:
        DEBUGPrint("INQUIRY RESULT: %02x:%02x:%02x:%02x:%02x:%02x   %02x%02x%02x   %d\n", d[0], d[1], d[2], d[3], d[4], d[5],
                   d[6], d[7], d[8], (s8)d[9]);

        sFoundRssi = d[9];

        if (_wcb.syncSkipChecks == 1 || (_wcb.syncSkipChecks == 0 && WUDiGetLinkNum() < 3)) {
            timeout = 0x1900;
        } else {
            timeout = 0x8000;
        }

        BTM_WritePageTimeout(timeout);
        break;

    case BTA_DM_INQ_CMPL_EVT:
        DEBUGPrint("INQUIRY_COMPLETED\n");
        break;

    case BTA_DM_DISC_RES_EVT:
        memcpy(_discResp.devAddr, pResp->devAddr, sizeof(_discResp.devAddr));
        memcpy(_discResp.devName, pResp->devName, sizeof(_discResp.devName));
        _discResp.services = pResp->services;

        sFoundCount++;

        DEBUGPrint("DISCOVER RESULT:  %02x:%02x:%02x:%02x:%02x:%02x   %s (%04x)\n", _discResp.devAddr[0], _discResp.devAddr[1],
                   _discResp.devAddr[2], _discResp.devAddr[3], _discResp.devAddr[4], _discResp.devAddr[5], _discResp.devName,
                   _discResp.services);
        break;

    case BTA_DM_DISC_CMPL_EVT:
        DEBUGPrint("DISCOVER COMPLETED\n");

        _wcb.syncState = 4;
        break;

    case BTA_DM_SEARCH_CANCEL_CMPL_EVT:
        DEBUGPrint("SEARCH CANCEL\n\n");

        BTM_VendorSpecificCommand(0xFC4C, sizeof(_vscCancelParam), _vscCancelParam, NULL);

        sFoundCount = 0;
        memset(&_discResp, 0, sizeof(_discResp));

        _wcb.syncState = 4;
        break;

    default:
        DEBUGPrint("Warning: Search Callback returns invalid event\n");
        break;
    }
}

static void WUDVendorSpecificCallback(UINT8 len, UINT8* p) {
    WUDCB* pCB = &_wcb;
    WUDSyncDeviceCallback syncCB;
    WUDClearDeviceCallback clearCB;
    BOOL enabled;
    BOOL ret;
    s32 result;

    switch (p[0]) {
    case WUD_VSE_INITIATE_PAIRING:
        DEBUGPrint("VSE:- INITIATE_PAIRING\n");

        WUDiAutoSync();
        break;

    case WUD_VSE_DELETE_ALL_KEYS:
        DEBUGPrint("VSE:- DELETE_ALL_KEYS\n");

        WUDiDeleteAllLinkKeys();
        break;

    case WUD_VSE_SI_PORT_STATUS:
        DEBUGPrint("VSE:- SI_PORT_STATUS  status = %d\n", p[1]);

        pCB->serialPortStatus = p[1];
        break;

    case WUD_VSE_WATCH_DOG_RESET_HW:
        DEBUGPrint("VSE:- WATCH_DOG_RESET  HW error = %d\n", p[1]);

        OSPanic("WUD.c", 4220, "MODULE FATAL ERROR\n");
        break;
    }
}

static void WUDDeviceStatusCallback(tBTM_DEV_STATUS status) {
    DEBUGPrint("WUDDeviceStatusCallback\n");

    if (status == 2) {
        OSReport("---- WARNING: USB FATAL ERROR! ----\n");
    }
}

static void WUDLinkKeyCallback(void* p) {
    u8* d = (u8*)p;
    u8* ent;
    WUDCB* pCB = &_wcb;
    WUDDevInfo* pInfo;
    int i;

    switch (d[0]) {
    case 1:
        DEBUGPrint("BTM_CB_EVT_RETURN_LINK_KEYS\n");

        ent = d + 2;
        for (i = 0; i < d[1]; ent += 0x16, i++) {
            pInfo = WUDiGetDevInfo(ent);

            DEBUGPrint("BD_ADDR:  %02x:%02x:%02x:%02x:%02x:%02x\n", ent[0], ent[1], ent[2], ent[3], ent[4], ent[5]);
            DEBUGPrint("LINKKEY: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", ent[6],
                       ent[7], ent[8], ent[9], ent[10], ent[11], ent[12], ent[13], ent[14], ent[15], ent[16], ent[17], ent[18],
                       ent[19], ent[20], ent[21]);

            if (pInfo == NULL) {
                DEBUGPrint("this device is not stored in NAND\n");

                pInfo = WUDiGetNewDevInfo();
                if (pInfo == NULL) {
                    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++) {
                        if (pCB->smpDevs[i].status == 0) {
                            pInfo = &pCB->smpDevs[i];
                            break;
                        }
                    }
                }

                if (pInfo != NULL) {
                    pInfo->UNK_0x5C = 1;
                    pInfo->status = 1;

                    memcpy(pInfo->devAddr, ent, WUD_ADDR_LEN);
                    memcpy(pInfo->linkKey, ent + 6, sizeof(pInfo->linkKey));
                }
            } else {
                memcpy(pInfo->devAddr, ent, WUD_ADDR_LEN);
                memcpy(pInfo->linkKey, ent + 6, sizeof(pInfo->linkKey));

                if (pInfo->UNK_0x5C == 2) {
                    pInfo->UNK_0x5C = 3;
                }

                memcpy(pCB->pairAddr, ent, WUD_ADDR_LEN);

                DEBUGPrint("   LAST: %02x:%02x:%02x:%02x:%02x:%02x\n", pCB->pairAddr[0], pCB->pairAddr[1], pCB->pairAddr[2],
                           pCB->pairAddr[3], pCB->pairAddr[4], pCB->pairAddr[5]);
            }
        }
        break;

    case 2:
        DEBUGPrint("BTM_CB_EVT_READ_STORED_LINK_KEYS\n");
        DEBUGPrint("  status: %d  max_keys: %d  num_keys: %d\n", d[1], *(u16*)(d + 2), *(u16*)(d + 4));

        pCB->linkKeyState = 0;
        break;

    case 3:
        DEBUGPrint("BTM_CB_EVT_WRITE_STORED_LINK_KEYS\n");
        DEBUGPrint("  status: %d  num_keys: %d\n", d[1], d[2]);

        pCB->linkKeyState = 0;
        break;

    case 4:
        DEBUGPrint("BTM_CB_EVT_DELETE_STORED_LINK_KEYS\n");
        DEBUGPrint("  status: %d  num_keys: %d\n", d[1], *(u16*)(d + 2));

        if (*(u16*)(d + 2) == 0) {
            DEBUGPrint("WARNING: no entry is deleted\n");
        }

        pCB->linkKeyState = 0;
        break;

    default:
        OSPanic("WUD.c", 4372, "Unknown event\n");
        break;
    }
}

static void WUDPowerManagerCallback(BD_ADDR addr, tBTM_PM_STATUS status, UINT16 value, UINT8 hciStatus) {
    WUDDevInfo* pInfo;

    DEBUGPrint("WUDPowerManagerCallback\n");
    DEBUGPrint("hci_status = %d", hciStatus);

    pInfo = WUDiGetDevInfo(addr);

    if (pInfo == NULL) {
        if (memcmp(_work.devAddr, addr, WUD_ADDR_LEN) == 0) {
            pInfo = &_work;
        } else {
            DEBUGPrint("Unknown device is connected and changes the connection type!!!!\n");
            DEBUGPrint(" addr = %02x:%02x:%02x:%02x:%02x:%02x,  status = %d\n", addr[0], addr[1], addr[2], addr[3], addr[4],
                       addr[5], status);
            return;
        }
    }

    switch (status) {
    case BTM_PM_STS_ACTIVE:
        pInfo->status = 8;
        break;
    case BTM_PM_STS_HOLD:
        break;
    case BTM_PM_STS_SNIFF:
        pInfo->status = 9;
        break;
    }

    DEBUGPrint(" addr = %02x:%02x:%02x:%02x:%02x:%02x,  status = %d\n", pInfo->devAddr[0], pInfo->devAddr[1], pInfo->devAddr[2],
               pInfo->devAddr[3], pInfo->devAddr[4], pInfo->devAddr[5], pInfo->status);
}

BD_ADDR_PTR _WUDGetDevAddr(UINT8 handle) {
    BOOL enabled;
    BD_ADDR_PTR addr;

    enabled = OSDisableInterrupts();

    if (handle < WUD_MAX_DEV_ENTRY) {
        addr = _dev_handle_to_bda[handle];
    } else {
        addr = NULL;
    }

    OSRestoreInterrupts(enabled);

    return addr;
}

u16 _WUDGetQueuedSize(s8 handle) {
    BOOL enabled;
    u16 size;

    enabled = OSDisableInterrupts();

    if (handle >= 0 && handle < WUD_MAX_DEV_ENTRY) {
        size = _dev_handle_queue_size[handle];
    } else {
        size = 0;
    }

    OSRestoreInterrupts(enabled);

    return size;
}

u16 _WUDGetNotAckedSize(s8 handle) {
    BOOL enabled;
    u16 size;

    enabled = OSDisableInterrupts();

    if (handle >= 0 && handle < WUD_MAX_DEV_ENTRY) {
        size = _dev_handle_notack_num[handle];
    } else {
        size = 0;
    }

    OSRestoreInterrupts(enabled);

    return size;
}

u8 _WUDGetLinkNumber(void) {
    BOOL enabled;
    u8 num;

    enabled = OSDisableInterrupts();
    num = _wcb.linkedNum;
    OSRestoreInterrupts(enabled);

    return num;
}

// HYPOTHESIS: these three never show up in the dol; only their strings do
void _WUDEnableTestMode(void) {
    DEBUGPrint("_WUDEnableTestMode\n");
}

BOOL _WUDStartSyncDevice(void) {
    DEBUGPrint("_WUDStartSyncDevice()\n");

    return WUDiStartSync(WUD_SYNC_TYPE_STANDARD, 3, 0, 0);
}

void _WUDDeleteStoreDevice(void) {
    DEBUGPrint("_WUDDeleteStoreDevice()\n");
    DEBUGPrint("dev number = %d\n", WUDiGetDevNumber());
}

// parameter block for the vendor command sent once the search has been cancelled
u8 _vscCancelParam[0x1C] = {
    0x30, 0x36, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};


int bte_hcisu_wait = 1;
