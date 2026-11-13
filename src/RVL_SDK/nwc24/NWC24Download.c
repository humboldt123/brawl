// WiiConnect24 SDK: Download.
#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

// BYTES: preserve calls across SDK source boundaries; original inline helpers
// are marked inline explicitly.
#pragma auto_inline off

/* ---------------------------------------------------------------------------------------------
 * download task list (/shared2/wc24/nwc24dl.bin)
 * ------------------------------------------------------------------------------------------- */

#define DL_HEADER_SIZE 0x800
#define DL_TASK_SIZE 0x200
#define DL_TASK_FLAG_GROUP_WRITABLE 0x40
#define DL_TASK_FLAG_NO_LIMITS 0x40000000

static const char *DLFilePath = "/shared2/wc24/nwc24dl.bin";

// HYPOTHESIS: when set the interval / count limits are not enforced
static BOOL DlIgnoreLimits;

extern int snprintf(char *, size_t, const char *, ...);
extern unsigned long strtoul(const char *, char **, int);
extern void *memset(void *, int, size_t);
extern NWC24Err fn_80399EE0(const char *, u32, u32);
extern int fn_80399F5C(char *, const char *, int);

typedef struct DlWalkState {
    u32 order;
    s32 best;
    s32 last;
    s32 lastIdx;
    u32 firstPass;
    u32 valid;
} DlWalkState;

NWC24Err NWC24CheckDlTask(const NWC24DlTask *pTask, BOOL wantWrite);
NWC24Err fn_803A2A18(NWC24iDlTask *task, int type);
NWC24Err fn_803A2C58(NWC24iDlTask *task, u32 interval);
NWC24Err fn_803A2EA4(NWC24iDlTask *task, const char *url);
NWC24Err fn_803A3058(const NWC24iDlTask *task, char *buf, u32 size);
NWC24Err fn_803A3064(NWC24iDlTask *task, u32 flags);
NWC24Err fn_803A31D0(NWC24iDlTask *task, int count);
NWC24Err fn_803A3324(NWC24iDlTask *out);
NWC24Err fn_803A3540(u16 *idx, BOOL first);
NWC24Err fn_803A367C(DlWalkState *state, u16 *out);
NWC24Err fn_803A38C0(NWC24iDlTask *task);
NWC24Err NWC24DeleteDlTaskForced(NWC24DlTask *pTask);
NWC24Err fn_803A3D64(NWC24iDlTask *task);
NWC24Err NWC24GetDlTask(NWC24DlTask *pTask, u16 id);
NWC24Err fn_803A4110(void);
NWC24Err fn_803A43D0(const NWC24iDlTask *task, const void *key);
NWC24Err fn_803A4684(const NWC24iDlTask *task, const void *hash);
NWC24Err NWC24iOpenDlTaskList(void);
NWC24Err NWC24iCloseDlTaskList(void);
NWC24Err fn_803A49B0(NWC24iDlHeader *hdr, BOOL clear);
NWC24Err fn_803A4D00(void);
NWC24Err fn_803A4E90(const NWC24iDlTask *task, char *buf, u32 size, BOOL withNumber, int number);
NWC24Err fn_803A4FCC(NWC24iDlTask *task, u8 flags);
NWC24Err fn_803A50EC(NWC24iDlTask *task);
NWC24Err fn_803A5284(NWC24iDlTask *task, u16 first, u16 last);
NWC24Err fn_803A58C8(NWC24iDlTask *task);
NWC24Err fn_803A5A8C(const char *path, u32 offset, const void *data, u32 len, u32 pad);
NWC24Err fn_803A5B84(const char *path, u32 offset, void *buf, u32 len);
u32 fn_803A5C38(u16 idx);
u32 fn_803A5C60(u16 idx);
u32 fn_803A5C88(u16 idx);

static inline BOOL SameAppId_i(u32 a, u32 b) {
    return (a & ~0xFF) == (b & ~0xFF);
}

static inline NWC24iDlHeader *DlHeader_i(void) {
    return NWC24WorkP != NULL ? (NWC24iDlHeader *)NWC24WorkP->dlHeader : NULL;
}

static inline NWC24iDlEntry *DlEntry_i(u16 i) {
    return &DlHeader_i()->entries[i];
}

// HYPOTHESIS: ownership checks read the application ID through a task accessor.
// BYTES: its return boundary preserves the native ownership temporary lifetime.
static inline u32 DlTaskAppId_i(const NWC24iDlTask *task) {
    return task->appId;
}

// App ownership ignores the low byte of the application ID.
static inline BOOL IsOwnDlTask_i(const NWC24iDlTask *task) {
    u32 appId = DlTaskAppId_i(task);
    return (NWC24GetAppId() & ~0xFF) == (appId & ~0xFF);
}

// Writes allow owner/group/tool access before checking the assigned slot bounds.
static inline NWC24Err CheckDlTaskInHeader_i(const NWC24iDlTask *task, BOOL wantWrite, NWC24iDlHeader *hdr) {
    if (task == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (hdr == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (wantWrite && !NWC24IsMsgLibOpenedByTool()) {
        BOOL same = IsOwnDlTask_i(task);

        if (!same) {
            u32 group = task->groupId;
            BOOL ok = FALSE;

            if ((task->flags & DL_TASK_FLAG_GROUP_WRITABLE) && group == NWC24GetGroupId()) {
                ok = TRUE;
            }

            if (!ok) {
                return NWC24_ERR_PROTECTED;
            }
        }
    }

    if (task->id != 0xFFFF && task->id >= hdr->maxTasks) {
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

static inline NWC24Err CheckDlTask_i(const NWC24iDlTask *task, BOOL wantWrite) {
    return CheckDlTaskInHeader_i(task, wantWrite, DlHeader_i());
}

// Schedules the task's next update from its interval and reports clock,
// validation and unassigned-slot errors. The interval setter ignores this status.
static inline NWC24Err SetNextTime_i(const NWC24iDlTask *task) {
    s64 now = 0;
    NWC24Err result = NWC24iGetUniversalTime(&now);
    NWC24Err check;
    s64 when;
    u16 id;
    if (result < 0) {
        return result;
    }
    when = now + task->interval * 60;
    check = CheckDlTask_i(task, FALSE);
    if (check == NWC24_OK) {
        id = task->id;
        if (id == 0xFFFF) {
            check = NWC24_ERR_FAILED;
        } else {
            DlEntry_i(id)->nextTime = when / 60;
            check = NWC24_OK;
        }
    }
    return check;
}

// HYPOTHESIS: record defaults are a separate stage receiving the full 64-bit title ID.
static inline void InitDlTaskFields_i(NWC24iDlTask *task, int type, u64 titleId) {
    BOOL isOctet;
    memset(task, 0, DL_TASK_SIZE);
    task->type = type;
    task->priority = 0x7F;
    task->appId = NWC24GetAppId();
    task->groupId = NWC24GetGroupId();
    task->titleIdHi = titleId >> 32;
    task->titleIdLo = (u32)titleId;
    task->id = 0xFFFF;
    task->count = 1;
    task->interval = 0xB40;
    task->margin = 0x5A0;

    if (type == NWC24_DLTYPE_OCTETSTREAM_V1 || type == NWC24_DLTYPE_OCTETSTREAM_V2) {
        isOctet = TRUE;
    } else {
        isOctet = FALSE;
    }
    if (isOctet) {
        strcpy(task->fileName, "content.bin");
    }

}

// Decodes the two title-ID components of the home path; the NAND status is ignored.
static inline u64 ReadDlHomeTitle_i(void) {
    char dir[64] = {0};
    u32 hi;
    u32 lo;
    NANDGetHomeDir(dir);
    dir[15] = 0;
    hi = strtoul(&dir[7], NULL, 16);
    dir[24] = 0;
    lo = strtoul(&dir[16], NULL, 16);
    return ((u64)hi << 32) | lo;
}

// Validates the URL length and its HTTP/HTTPS scheme.
static inline NWC24Err CheckDlUrl_i(const char *url) {
    NWC24Err result = fn_80399EE0(url, 7, 0x100);
    if (result < 0) {
        return result;
    }
    if (strncmp(url, "http://", 7) != 0 && strncmp(url, "https://", 8) != 0) {
        return NWC24_ERR_FORMAT;
    }
    return NWC24_OK;
}

// ---- writing tasks / header back to the list file ----
static inline NWC24Err WriteDlTaskEntry_i(const NWC24iDlTask *task, NWC24File *file) {
    NWC24Err result;
    u16 id = task->id;
    u16 max = DlHeader_i()->maxTasks;

    if (max > NWC24i_DL_TASK_MAX || id >= max) {
        result = NWC24_ERR_INVALID_VALUE;
    } else {
        result = NWC24FSeek(file, DL_HEADER_SIZE + id * DL_TASK_SIZE, NWC24_SEEK_BEG);
    }

    if (result < 0) {
        return result;
    }

    memcpy(NWC24WorkP->dlTask, task, DL_TASK_SIZE);
    result = NWC24FWrite(NWC24WorkP->dlTask, DL_TASK_SIZE, file);
    if (result < 0) {
        return result;
    }

    return NWC24_OK;
}

static inline NWC24Err WriteDlHeader_i(NWC24File *file) {
    NWC24Err result = NWC24FSeek(file, 0, NWC24_SEEK_BEG);

    if (result < 0) {
        return result;
    }

    result = NWC24FWrite(DlHeader_i(), DL_HEADER_SIZE, file);
    if (result < 0) {
        return result;
    }

    return NWC24_OK;
}

// clears a task's slot in the file and in the list header
static inline void InitDlTaskEntry_i(u16 id) {
    memset(&((NWC24iDlHeader *)NWC24WorkP->dlHeader)->entries[id], 0, sizeof(NWC24iDlEntry));
}

static inline NWC24Err ClearDlTaskEntry_i(u16 id, NWC24File *file) {
    NWC24iDlTask *empty = (NWC24iDlTask *)NWC24WorkP->dlTask;
    memset(empty, 0, DL_TASK_SIZE);
    empty->type = 0xFF;
    empty->id = id;
    InitDlTaskEntry_i(id);
    return WriteDlTaskEntry_i(empty, file);
}

// is there a task in the slot?
static inline NWC24Err CheckDlEntryAvailable_i(u16 i) {
    if (i >= DlHeader_i()->maxTasks || i == 0xFFFF) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (DlEntry_i(i)->app == 0) {
        return NWC24_ERR_NOT_FOUND;
    }

    return NWC24_OK;
}

static inline NWC24Err DeleteDlTaskForced_i(NWC24DlTask *pTask) {
    NWC24iDlTask *task = (NWC24iDlTask *)pTask;
    NWC24Err result;

    result = CheckDlTask_i(task, FALSE);
    if (result != NWC24_OK) {
        return result;
    }

    result = fn_803A58C8(task);
    if (result < 0) {
        return result;
    }

    task->id = 0xFFFF;
    return result;
}

static inline NWC24Err GetDlTask_i(NWC24DlTask *pTask, u16 id) {
    NWC24iDlTask *task = (NWC24iDlTask *)pTask;
    NWC24File file;
    NWC24Err result;
    NWC24Err close;
    u16 max;

    if (DlHeader_i() == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    result = CheckDlEntryAvailable_i(id);
    if (result < 0) {
        return result;
    }

    result = NWC24FOpen(&file, DLFilePath, NWC24_OPEN_NAND_RBUFF);
    if (result < 0) {
        return result;
    }

    max = DlHeader_i()->maxTasks;
    if (max > NWC24i_DL_TASK_MAX || id >= max) {
        result = NWC24_ERR_INVALID_VALUE;
    } else {
        result = NWC24FSeek(&file, DL_HEADER_SIZE + id * DL_TASK_SIZE, NWC24_SEEK_BEG);
    }

    if (result < 0) {
        close = result;
    } else {
        result = NWC24FRead(task, DL_TASK_SIZE, &file);
        close = NWC24_OK;
        if (result < 0) {
            close = result;
        }
    }

    result = NWC24FClose(&file);
    return close != NWC24_OK ? close : result;
}

// Initializes an ascending or descending ordered walk.
static inline NWC24Err InitDlWalk_i(DlWalkState *state, u32 order) {
    memset(state, 0, sizeof(*state));
    if (!(order & 0x80000000)) {
        state->best = 0x7FFFFFFF;
        state->last = 0x80000001;
    } else {
        state->best = 0x80000001;
        state->last = 0x7FFFFFFF;
    }
    state->order = order;
    state->lastIdx = -1;
    state->firstPass = TRUE;
    state->valid = TRUE;
    return NWC24_OK;
}

// is the sub task counter pointing at a sub task that is switched on? (-39 says "no, try the next one")
static inline NWC24Err SubTaskStatus_i(const NWC24iDlTask *task, u8 counter) {
    // BYTES: the loop validator selects its header here rather than expanding the accessor.
    NWC24Err check = CheckDlTaskInHeader_i(
        task, FALSE, (NWC24iDlHeader *)(NWC24WorkP == NULL ? NULL : NWC24WorkP->dlHeader));
    if (check != NWC24_OK) {
        return check;
    }
    if (task->subTaskType == 0) {
        return NWC24_ERR_INVALID_OPERATION;
    }
    if (task->subTaskMask == 0) {
        return NWC24_ERR_FATAL;
    }
    if (counter > 31) {
        return NWC24_ERR_INVALID_VALUE;
    }
    check = NWC24_OK;
    if (!((1 << counter) & task->subTaskMask)) {
        check = NWC24_ERR_DISABLED;
    }
    return check;
}

// marks a task as just used: stamps the time, clears the errors, moves on to the next sub task and saves
// HYPOTHESIS: these inline stage boundaries follow the shared SDK update pipeline.
// Timestamping, error clearing, subtask selection and persistence retain separate status handling.
static inline NWC24Err SetDlLastAccess_i(const NWC24iDlTask *task, s64 when) {
    NWC24Err result = CheckDlTask_i(task, FALSE);
    u16 id;

    if (result != NWC24_OK) {
        return result;
    }
    id = task->id;
    if (id == 0xFFFF) {
        return NWC24_ERR_FAILED;
    }
    DlEntry_i(id)->lastAccess = when / 60;
    return NWC24_OK;
}

static inline NWC24Err StampDlTask_i(const NWC24iDlTask *task) {
    s64 now;
    NWC24Err result = NWC24iGetUniversalTime(&now);
    if (result >= 0) {
        result = SetDlLastAccess_i(task, now);
        if (result < 0) {
            return result;
        }
    }
    return result;
}

static inline void ClearDlTaskError_i(NWC24iDlTask *task) {
    // BYTES: keep this header selection within the error-reset stage when update is inlined.
    if (CheckDlTaskInHeader_i(
            task, TRUE, (NWC24iDlHeader *)(NWC24WorkP != NULL ? NWC24WorkP->dlHeader : NULL)) == NWC24_OK) {
        task->lastError = 0;
        task->errorCount = 0;
    }
}

static inline NWC24Err AdvanceDlSubTask_i(NWC24iDlTask *task) {
    NWC24Err result;
    while ((result = SubTaskStatus_i(task, task->subTaskCounter)) == NWC24_ERR_DISABLED) {
        task->subTaskCounter = (task->subTaskCounter + 1) & 0x1F;
    }
    return result;
}

static inline NWC24Err CommitDlTask_i(NWC24iDlTask *task) {
    NWC24Err result;
    if (task->subTaskType == NWC24_DL_STTYPE_INCREMENT && (result = AdvanceDlSubTask_i(task)) < 0) {
        return result;
    }
    return fn_803A50EC(task);
}

// takes a task for the list: gets a slot for it when it has none yet (kicking the oldest one out when the
// list is full), then saves it
// BYTES: keep the public update entrypoint while also expanding it in slot allocation.
static inline const char *DlTaskUrl_i(const NWC24iDlTask *task) {
    return task->url;
}

// Checks the slot assignment and URL before allocation mutates the task.
static inline NWC24Err CheckDlTaskParam_i(NWC24iDlTask *task) {
    const char *url;
    NWC24Err result;
    if (task->id >= DlHeader_i()->maxTasks && task->id != 0xFFFF) {
        return NWC24_ERR_INVALID_VALUE;
    }
    url = DlTaskUrl_i(task);
    result = CheckDlUrl_i(url);
    if (result < 0) {
        return result;
    }
    return NWC24_OK;
}

// Assigns the first unused slot in the half-open range [first, last).
static inline NWC24Err FindFreeDlId_i(NWC24iDlTask *task, u32 first, u32 last) {
    NWC24iDlHeader *hdr = DlHeader_i();
    u16 i;
    if (task == NULL || first > last || first >= hdr->maxTasks || last > hdr->maxTasks) {
        return NWC24_ERR_INVALID_VALUE;
    }
    for (i = first; i < last; i++) {
        if (DlEntry_i(i)->app == 0) {
            task->id = i;
            return NWC24_OK;
        }
    }
    return NWC24_ERR_FULL;
}

// Checks the task counts carried by a repaired or current-format header.
static inline NWC24Err CheckDlHeaderLimits_i(const NWC24iDlHeader *hdr) {
    if (hdr->maxTasks < 1 || hdr->privateTasks < 1 || hdr->maxTasks < hdr->privateTasks) {
        return NWC24_ERR_BROKEN;
    }

    return NWC24_OK;
}

// Older list files only have a subtask count. Repair the supplied record,
// then validate that same record after the file operations.
static inline NWC24Err FixDlHeader_i(NWC24iDlHeader *hdr) {
    if (hdr->maxTasks == 0 && hdr->maxSubTasks != 0) {
        NWC24File file;

        hdr->maxTasks = hdr->maxSubTasks;
        if (hdr->maxSubTasks > NWC24i_DL_SUBTASK_MAX) {
            hdr->maxSubTasks = NWC24i_DL_SUBTASK_MAX;
        }

        if (NWC24FOpen(&file, DLFilePath, NWC24_OPEN_NAND_RW) >= 0) {
            if (NWC24FSeek(&file, 0, NWC24_SEEK_BEG) >= 0) {
                NWC24FWrite(DlHeader_i(), DL_HEADER_SIZE, &file);
            }

            NWC24FClose(&file);
        }
    }
    return CheckDlHeaderLimits_i(hdr);
}

// reads the list header from the file
static inline NWC24Err ReadDlHeader_i(NWC24File *file) {
    NWC24Err result = NWC24FSeek(file, 0, NWC24_SEEK_BEG);
    if (result < 0) {
        return result;
    }
    result = NWC24FRead(NWC24WorkP->dlHeader, DL_HEADER_SIZE, file);
    if (result < 0) {
        return result;
    }
    return NWC24_OK;
}


NWC24Err NWC24CheckDlTask(const NWC24DlTask *pTask, BOOL wantWrite) {
    const NWC24iDlTask *task = (const NWC24iDlTask *)pTask;
    NWC24iDlHeader *hdr = DlHeader_i();

    if (task == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (hdr == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (wantWrite && !NWC24IsMsgLibOpenedByTool()) {
        u32 appId = task->appId;
        BOOL same = (NWC24GetAppId() & ~0xFF) == (appId & ~0xFF);

        if (!same) {
            u32 group = task->groupId;
            BOOL ok = FALSE;

            if ((task->flags & DL_TASK_FLAG_GROUP_WRITABLE) && group == NWC24GetGroupId()) {
                ok = TRUE;
            }

            if (!ok) {
                return NWC24_ERR_PROTECTED;
            }
        }
    }

    if (task->id != 0xFFFF && task->id >= hdr->maxTasks) {
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

// Initializes a task for the current title after the library, pointer and type guards.
NWC24Err fn_803A2A18(NWC24iDlTask *task, int type) {
    u64 titleId = ReadDlHomeTitle_i();
    NWC24iDlHeader *hdr;
    NWC24Err result;


    hdr = DlHeader_i();
    if (hdr == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (task == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (type >= 4) {
        return NWC24_ERR_INVALID_VALUE;
    }

    InitDlTaskFields_i(task, type, titleId);

    result = CheckDlTask_i(task, TRUE);
    return result >> 31;
}

// how often (in minutes) the task should run
NWC24Err fn_803A2C58(NWC24iDlTask *task, u32 interval) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (!DlIgnoreLimits && task->id >= DlHeader_i()->privateTasks && !(task->flags & DL_TASK_FLAG_NO_LIMITS)) {
        if (interval < 180 || interval > 10080) {
            return NWC24_ERR_INVALID_VALUE;
        }
    }

    if (task->interval != interval) {
        task->interval = interval;
        if (task->id != 0xFFFF) {
            SetNextTime_i(task);
        }
    }

    return NWC24_OK;
}

// the url to download from, http or https
NWC24Err fn_803A2EA4(NWC24iDlTask *task, const char *url) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    result = CheckDlUrl_i(url);

    if (result < 0) {
        return result;
    }

    if (!DlIgnoreLimits && (task->flags & 4) && strncmp(url, "http://", 7) == 0) {
        return NWC24_ERR_INVALID_VALUE;
    }

    fn_80399F5C(task->url, url, 0xEC);
    return NWC24_OK;
}

NWC24Err fn_803A3058(const NWC24iDlTask *task, char *buf, u32 size) {
    return fn_803A4E90(task, buf, size, FALSE, 0xFF);
}

// task flags
NWC24Err fn_803A3064(NWC24iDlTask *task, u32 flags) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (!DlIgnoreLimits && (flags & 4) && strncmp(task->url, "http://", 7) == 0) {
        return NWC24_ERR_INVALID_VALUE;
    }

    switch (task->type) {
    case NWC24_DLTYPE_MULTIPART_V1:
    case NWC24_DLTYPE_OCTETSTREAM_V1:
        // the old types know nothing about the newer flag bits
        if ((flags & 0x80000000) | (flags & 0x38)) {
            return -40;
        }
        break;
    case NWC24_DLTYPE_MULTIPART_V2:
    case NWC24_DLTYPE_OCTETSTREAM_V2:
        break;
    default:
        break;
    }

    task->flags = flags;
    return NWC24_OK;
}

// how many times the task may run
NWC24Err fn_803A31D0(NWC24iDlTask *task, int count) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (count < 1 || (task->id >= DlHeader_i()->privateTasks && !(task->flags & DL_TASK_FLAG_NO_LIMITS) && count > 100)) {
        return NWC24_ERR_INVALID_VALUE;
    }

    task->count = count;
    return NWC24_OK;
}

// finds the first task of the running app and loads it
NWC24Err fn_803A3324(NWC24iDlTask *out) {
    NWC24Err result;
    u16 idx;
    u32 appId = NWC24GetAppId();

    if (DlHeader_i() == NULL) {
        result = NWC24_ERR_LIB_NOT_OPENED;
    } else if (appId == 0) {
        result = NWC24_ERR_INVALID_VALUE;
    } else {
        result = fn_803A3540(&idx, TRUE);
        while (result >= 0) {
            if (DlEntry_i(idx)->app == appId) {
                result = NWC24_OK;
                goto found;
            }

            result = fn_803A3540(&idx, FALSE);
        }

        result = NWC24_ERR_NOT_FOUND;
    }

found:
    if (result < 0) {
        return result;
    }

    return GetDlTask_i((NWC24DlTask *)out, idx);
}

// steps *idx to the next used slot (or the first one)
NWC24Err fn_803A3540(u16 *idx, BOOL first) {
    u16 i;

    if (DlHeader_i() == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (idx == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (first) {
        *idx = 0;
    } else {
        (*idx)++;
    }

    if (*idx >= DlHeader_i()->maxTasks) {
        return NWC24_ERR_DONE;
    }

    // BYTES: selecting the nullable header here preserves the native loop branch layout.
    for (i = *idx;
         i < ((NWC24iDlHeader *)(NWC24WorkP == NULL ? NULL : NWC24WorkP->dlHeader))->maxTasks;
         i++) {
        if (CheckDlEntryAvailable_i(i) >= 0) {
            *idx = i;
            return NWC24_OK;
        }
    }

    return NWC24_ERR_DONE;
}

NWC24Err fn_803A367C(DlWalkState *state, u16 *out) {
    typedef u32 (*KeyFn)(u16);
    BOOL desc;
    NWC24Err result;
    BOOL found = FALSE;
    KeyFn getKey;
    u16 idx;
    s32 key;
    s32 previous;
    s32 best;

    if (state->valid == 0) {
        return NWC24_ERR_INVALID_VALUE;
    }

    desc = state->order >> 31;
    switch (state->order & 0xFFFF) {
    case 0:
        getKey = fn_803A5C38;
        break;
    case 1:
        getKey = fn_803A5C60;
        break;
    case 2:
        getKey = fn_803A5C88;
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    // After the first key, visit equal keys in increasing slot order.
    if (state->firstPass == 0) {
        result = fn_803A3540(&idx, TRUE);
        while (result >= 0) {
            key = getKey(idx);
            if (key == state->best && state->lastIdx < idx) {
                *out = idx;
                state->best = key;
                state->lastIdx = idx;
                state->last = key;
                return NWC24_OK;
            }

            result = fn_803A3540(&idx, FALSE);
        }
    } else {
        state->firstPass = 0;
    }

    if (!(state->order & 0x80000000)) {
        state->best = 0x7FFFFFFF;
    } else {
        state->best = 0x80000001;
    }

    result = fn_803A3540(&idx, TRUE);
    while (result >= 0) {
        key = getKey(idx);
        previous = state->last;
        if (desc ? (key < previous) : (previous < key)) {
            best = state->best;
            if (desc ? (best < key) : (key < best)) {
                found = TRUE;
                *out = idx;
                state->best = key;
                state->lastIdx = idx;
            }
        }

        result = fn_803A3540(&idx, FALSE);
    }

    if (found) {
        state->last = state->best;
        return NWC24_OK;
    }

    state->valid = 0;
    return NWC24_ERR_DONE;
}

// BYTES: keep the public update entrypoint while also expanding it in slot allocation.
#pragma push
#pragma auto_inline on
NWC24Err fn_803A38C0(NWC24iDlTask *task) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (task->id == 0xFFFF || task->id >= DlHeader_i()->maxTasks) {
        return NWC24_ERR_INVALID_VALUE;
    }

    result = StampDlTask_i(task);

    if (result < 0) {
        return result;
    }

    ClearDlTaskError_i(task);
    return CommitDlTask_i(task);
}
#pragma pop

NWC24Err NWC24DeleteDlTaskForced(NWC24DlTask *pTask) {
    return DeleteDlTaskForced_i(pTask);
}

// puts a task into the list, filling in the next update time
NWC24Err fn_803A3D64(NWC24iDlTask *task) {
    NWC24Err result;

    result = FixDlHeader_i(DlHeader_i());
    if (result < 0) {
        return result;
    }

    {
        // Preserve the native order of these two header reads.
        u16 first = DlHeader_i()->privateTasks;
        u16 last = DlHeader_i()->maxTasks;
        result = fn_803A5284(task, first, last);
    }
    // Schedule the first run only after slot assignment succeeds.
    if (result >= 0) {
        s64 now = 0;
        NWC24Err clockResult = NWC24iGetUniversalTime(&now);
        NWC24Err check;
        s64 when;
        u16 id;
        if (clockResult < 0) {
            return clockResult;
        }
        when = now + task->interval * 60;
        check = CheckDlTask_i(task, FALSE);
        switch (check) {
        case NWC24_OK:
            id = task->id;
            if (id == 0xFFFF) {
                check = NWC24_ERR_FAILED;
            } else {
                DlEntry_i(id)->nextTime = when / 60;
                check = NWC24_OK;
            }
            break;
        }
        return check;
    }
    return result;
}

NWC24Err NWC24GetDlTask(NWC24DlTask *pTask, u16 id) {
    NWC24iDlTask *task = (NWC24iDlTask *)pTask;
    NWC24File file;
    NWC24Err result;
    NWC24Err readResult;
    u16 max;

    if (DlHeader_i() == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    result = CheckDlEntryAvailable_i(id);
    if (result < 0) {
        return result;
    }

    result = NWC24FOpen(&file, DLFilePath, NWC24_OPEN_NAND_RBUFF);
    if (result < 0) {
        return result;
    }

    max = DlHeader_i()->maxTasks;
    if (max > NWC24i_DL_TASK_MAX || id >= max) {
        result = NWC24_ERR_INVALID_VALUE;
    } else {
        result = NWC24FSeek(&file, DL_HEADER_SIZE + id * DL_TASK_SIZE, NWC24_SEEK_BEG);
    }

    if (result < 0) {
        readResult = result;
    } else {
        result = NWC24FRead(task, DL_TASK_SIZE, &file);
        readResult = NWC24_OK;
        if (result < 0) {
            readResult = result;
        }
    }

    result = NWC24FClose(&file);
    return readResult != NWC24_OK ? readResult : result;
}

// Evicts the least recently used public task, preserving read/delete failures.
NWC24Err fn_803A4110(void) {
    DlWalkState state;
    NWC24DlTask task;
    u16 idx;
    NWC24Err result;
    result = InitDlWalk_i(&state, 0);
    if (result < 0) {
        return result;
    }
    while ((result = fn_803A367C(&state, &idx)) == NWC24_OK) {
        if (idx >= DlHeader_i()->privateTasks) {
            break;
        }
    }
    if (result >= 0) {
        result = GetDlTask_i(&task, idx);
        if (result < 0) {
            return result;
        }
        result = DeleteDlTaskForced_i(&task);
        if (result < 0) {
            return result;
        }
    } else if (result == NWC24_ERR_DONE) {
        result = NWC24_ERR_FAILED;
    }
    return result;
}

// BYTES: these used format arrays preserve the native data and small-data order.
static char DlDataDirFormat[] = "/title/%08x/%08x/data";
static char DlSubTaskSuffix[] = ".%02d";

// Builds the owning title's data directory; callers append the file name.
static inline NWC24Err DlTaskDataDir_i(const NWC24DlTask *pTask, char *path, u32 capacity) {
    const NWC24iDlTask *task = (const NWC24iDlTask *)pTask;
    NWC24Err result = CheckDlTask_i(task, FALSE);
    if (result != NWC24_OK) {
        return result;
    }
    if (task->appId == 0) {
        return NWC24_ERR_INVALID_VALUE;
    }
    if (capacity < 30) {
        return NWC24_ERR_NOMEM;
    }
    snprintf(path, capacity, DlDataDirFormat, task->titleIdHi, task->titleIdLo);
    return NWC24_OK;
}

static inline NWC24Err DlTaskFilePath_i(const NWC24iDlTask *task, char *path, u32 capacity, const char *file) {
    NWC24Err result;
    u32 len;

    result = CheckDlTask_i(task, FALSE);
    if (result != NWC24_OK) {
        return result;
    }
    if (task->appId == 0) {
        return NWC24_ERR_INVALID_VALUE;
    }
    result = DlTaskDataDir_i((const NWC24DlTask *)task, path, capacity);
    if (result < 0) {
        return result;
    }
    len = strlen(path);
    snprintf(path + len, capacity - len, "/%s", file);
    return NWC24_OK;
}

// Stores the task server's 256-byte public key.
NWC24Err fn_803A43D0(const NWC24iDlTask *task, const void *key) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (key == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    {
        char path[64];
        const char *file = "wc24pubk.mod";

        result = DlTaskFilePath_i(task, path, sizeof(path), file);

        if (result < 0) {
            return result;
        }

        result = NANDCreate(path, 0x3D, 0);
        if (result != 0 && result != NAND_RESULT_EXISTS) {
            return -21;
        }

        return fn_803A5A8C(path, 0, key, 0x100, 0);
    }
}

// stores the 16 byte hash that belongs to the public key
NWC24Err fn_803A4684(const NWC24iDlTask *task, const void *hash) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (hash == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    {
        char path[64];
        const char *file = "wc24pubk.mod";

        result = DlTaskFilePath_i(task, path, sizeof(path), file);

        if (result < 0) {
            return result;
        }

        if (fn_803A5B84(path, 0, NULL, 0) < 0) {
            return -40;
        }

        fn_803A5A8C(path, 0x100, NULL, 0, 0x100);
        return fn_803A5A8C(path, 0x200, hash, 0x10, 0);
    }
}

NWC24Err NWC24iOpenDlTaskList(void) {
    NWC24Err result;

    result = fn_803A4D00();
    if (result >= 0) {
        NWC24iSynchronizeRtcCounter(FALSE);
        result = fn_803A49B0(DlHeader_i(), FALSE);
        if (result >= 0) {
            return NWC24_OK;
        }
    }

    return result;
}

NWC24Err NWC24iCloseDlTaskList(void) {
    return NWC24_OK;
}

// walks the list and drops tasks that cannot be read back or that have run out of runs
NWC24Err fn_803A49B0(NWC24iDlHeader *hdr, BOOL clear) {
    NWC24iDlTask storage;
    u16 i;

    for (i = 0; i < hdr->maxTasks; i++) {
        if (CheckDlEntryAvailable_i(i) == NWC24_OK && clear) {
            if (GetDlTask_i((NWC24DlTask *)&storage, i) < 0) {
                DeleteDlTaskForced_i((NWC24DlTask *)&storage);
            } else if (i >= DlHeader_i()->privateTasks && storage.count == 0) {
                DeleteDlTaskForced_i((NWC24DlTask *)&storage);
            }
        }
    }

    return NWC24_OK;
}

NWC24Err fn_803A4D00(void) {
    NWC24File file;
    NWC24Err result;
    NWC24Err read = NWC24_OK;
    u32 length = 0;

    result = NWC24FOpen(&file, DLFilePath, NWC24_OPEN_READ);
    if (result < 0) {
        return result;
    }

    result = ReadDlHeader_i(&file);

    if (result < 0) {
        return result;
    }

    read = NWC24FGetLength(&file, &length);
    if (read >= 0) {
        read = FixDlHeader_i(DlHeader_i());
    }

    result = NWC24FClose(&file);
    return read != NWC24_OK ? read : result;
}

// copies the url out, optionally with a ".nn" counter on the end
NWC24Err fn_803A4E90(const NWC24iDlTask *task, char *buf, u32 size, BOOL withNumber, int number) {
    NWC24Err result;
    int len;

    result = CheckDlTask_i(task, FALSE);
    if (result != NWC24_OK) {
        return result;
    }

    len = strlen(task->url);
    if (len == 0) {
        return NWC24_ERR_NOT_FOUND;
    }

    if ((u32)len < 8) {
        return NWC24_ERR_FAILED;
    }

    if (len + (withNumber ? 3 : 0) + 1 > size) {
        return NWC24_ERR_NOMEM;
    }

    strncpy(buf, task->url, len);
    buf[len] = 0;
    if (withNumber) {
        snprintf(buf + len, size - len, DlSubTaskSuffix, number);
    }

    return NWC24_OK;
}

// bits 0 and 1 of the optional flags exclude each other
NWC24Err fn_803A4FCC(NWC24iDlTask *task, u8 flags) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if ((flags & 1) && (flags & 2)) {
        return NWC24_ERR_INVALID_VALUE;
    }

    task->optFlags = flags;
    return NWC24_OK;
}

// writes one task (and its list entry) out
NWC24Err fn_803A50EC(NWC24iDlTask *task) {
    NWC24File file;
    NWC24Err result;
    NWC24Err close;

    result = NWC24FOpen(&file, DLFilePath, NWC24_OPEN_NAND_RW);
    if (result < 0) {
        return result;
    }

    result = WriteDlTaskEntry_i(task, &file);
    if (result >= 0) {
        NWC24iDlEntry *entry = DlEntry_i(task->id);

        entry->app = task->appId;
        result = NWC24_OK;
        entry->flags = task->priority;
        if (result >= 0) {
            result = WriteDlHeader_i(&file);
        }
    }

    close = NWC24FClose(&file);
    return result != NWC24_OK ? result : close;
}

// Allocates in [first, last), evicting the oldest public task when full, then updates the task.
#pragma push
#pragma auto_inline on
NWC24Err fn_803A5284(NWC24iDlTask *task, u16 first, u16 last) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    result = CheckDlTaskParam_i(task);

    if (result < 0) {
        return result;
    }

    for (;;) {
        if (task->id != 0xFFFF) {
            return fn_803A38C0(task);
        }

        result = FindFreeDlId_i(task, first, last);

        if (result == NWC24_ERR_FULL) {
            result = fn_803A4110();
            if (result < 0) {
                return result;
            }
        } else if (result < 0) {
            return result;
        }
    }
}
#pragma pop

NWC24Err fn_803A58C8(NWC24iDlTask *task) {
    NWC24File file;
    NWC24Err result;
    NWC24Err close;

    result = NWC24FOpen(&file, DLFilePath, NWC24_OPEN_NAND_RW);
    if (result < 0) {
        return result;
    }

    result = ClearDlTaskEntry_i(task->id, &file);
    if (result >= 0) {
        InitDlTaskEntry_i(task->id);
        result = WriteDlHeader_i(&file);
    }

    close = NWC24FClose(&file);
    return result != NWC24_OK ? result : close;
}

// writes some data (and then some zero padding) at an offset into a file, creating it if needed
NWC24Err fn_803A5A8C(const char *path, u32 offset, const void *data, u32 len, u32 pad) {
    NWC24File file;
    NWC24Err result;
    NWC24Err close;
    u32 i;
    u8 zero;

    result = NWC24FOpen(&file, path, NWC24_OPEN_NAND_WBUFF);
    if (result < 0) {
        return result;
    }

    result = NWC24FSeek(&file, offset, NWC24_SEEK_BEG);
    if (result < 0) {
        return result;
    }

    if (data != NULL && len != 0) {
        result = NWC24FWrite(data, len, &file);
    }

    for (i = 0; i < pad; i++) {
        zero = 0;
        result = NWC24FWrite(&zero, 1, &file);
        if (result < 0) {
            break;
        }
    }

    close = NWC24FClose(&file);
    return result != NWC24_OK ? result : close;
}

// reads some data from an offset in a file
NWC24Err fn_803A5B84(const char *path, u32 offset, void *buf, u32 len) {
    NWC24File file;
    NWC24Err result;
    NWC24Err close;

    result = NWC24FOpen(&file, path, NWC24_OPEN_NAND_RBUFF);
    if (result < 0) {
        return result;
    }

    result = NWC24FSeek(&file, offset, NWC24_SEEK_BEG);
    if (result < 0) {
        return result;
    }

    if (buf != NULL && len != 0) {
        result = NWC24FRead(buf, len, &file);
    }

    close = NWC24FClose(&file);
    return result != NWC24_OK ? result : close;
}

// ---- list entry accessors, they get handed to the sorter below ----
u32 fn_803A5C38(u16 idx) {
    return DlHeader_i()->entries[idx].lastAccess;
}

u32 fn_803A5C60(u16 idx) {
    return DlHeader_i()->entries[idx].nextTime;
}

u32 fn_803A5C88(u16 idx) {
    return DlHeader_i()->entries[idx].flags;
}
