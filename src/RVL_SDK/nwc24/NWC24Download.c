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
#define DL_TASK_FLAG_ANYONE 0x40
#define DL_TASK_FLAG_NO_LIMITS 0x40000000

static const char *DLFilePath = "/shared2/wc24/nwc24dl.bin";

// HYPOTHESIS: when set the interval / count limits are not enforced
static BOOL DlIgnoreLimits;

extern int snprintf(char *, size_t, const char *, ...);
extern unsigned long strtoul(const char *, char **, int);

static inline NWC24iDlEntry *DlEntry_i(u16 i);

static inline BOOL SameAppId_i(u32 a, u32 b) {
    return (a & ~0xFF) == (b & ~0xFF);
}

static inline NWC24iDlHeader *DlHeader_i(void) {
    return NWC24WorkP != NULL ? (NWC24iDlHeader *)NWC24WorkP->dlHeader : NULL;
}

static inline NWC24iDlEntry *DlEntry_i(u16 i) {
    return &DlHeader_i()->entries[i];
}

// the app that owns a task, or one with the same group, may change it
static inline NWC24Err CheckDlTask_i(const NWC24iDlTask *task, BOOL wantWrite) {
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
            BOOL ok = FALSE;
            u16 group = task->groupId;

            if ((task->flags & DL_TASK_FLAG_ANYONE) && group == NWC24GetGroupId()) {
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

NWC24Err NWC24CheckDlTask(const NWC24DlTask *pTask, BOOL wantWrite) {
    return CheckDlTask_i((const NWC24iDlTask *)pTask, wantWrite);
}

// stamps "now + minutes" into the next-update time of the task's list entry
static inline void SetNextTime_i(NWC24iDlTask *task, u32 minutes) {
    s64 now = 0;
    NWC24Err result;

    result = NWC24iGetUniversalTime(&now);
    if (result >= 0) {
        s64 when = now + task->interval * 60;

        result = CheckDlTask_i(task, FALSE);
        if (result == NWC24_OK) {
            u16 id = task->id;

            if (id != 0xFFFF) {
                DlHeader_i()->entries[id].nextTime = when / 60;
            }
        }
    }
}

// sets up a fresh task of the given type
NWC24Err fn_803A2A18(NWC24iDlTask *task, int type) {
    char dir[64] = {0};
    u32 titleHi;
    u32 titleLo;
    NWC24iDlHeader *hdr;
    NWC24Err result;
    BOOL isOctet;

    NANDGetHomeDir(dir);
    dir[15] = 0;
    titleHi = strtoul(&dir[7], NULL, 16);
    dir[24] = 0;
    titleLo = strtoul(&dir[16], NULL, 16);

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

    memset(task, 0, DL_TASK_SIZE);
    task->type = type;
    task->priority = 0x7F;
    task->appId = NWC24GetAppId();
    task->groupId = NWC24GetGroupId();
    task->titleIdHi = titleHi;
    task->titleIdLo = titleLo;
    task->id = 0xFFFF;
    task->count = 1;
    task->interval = 0xB40;
    task->margin = 0x5A0;

    isOctet = type == NWC24_DLTYPE_OCTETSTREAM_V1 || type == NWC24_DLTYPE_OCTETSTREAM_V2;
    if (isOctet) {
        strcpy(task->fileName, "content.bin");
    }

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
            SetNextTime_i(task, interval);
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

    result = fn_80399EE0(url, 7, 0x100);
    if (result >= 0) {
        if (strncmp(url, "http://", 7) != 0 && strncmp(url, "https://", 8) != 0) {
            result = NWC24_ERR_FORMAT;
        } else {
            result = NWC24_OK;
        }
    }

    if (result < 0) {
        return result;
    }

    if (!DlIgnoreLimits && (task->flags & 4) && strncmp(url, "http://", 7) == 0) {
        return NWC24_ERR_INVALID_VALUE;
    }

    fn_80399F5C(task->url, url, 0xEC);
    return NWC24_OK;
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

// ---- list entry accessors, they get handed to the sorter below ----

u32 fn_803A5C38(u16 idx) {
    return DlHeader_i()->entries[idx].lastAccess;
}

u32 fn_803A5C60(u16 idx) {
    return DlHeader_i()->entries[idx].nextTime;
}

u8 fn_803A5C88(u16 idx) {
    return DlHeader_i()->entries[idx].flags;
}

// ---- raw file helpers ----

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

// clears a task's slot in the file and in the list header
NWC24Err fn_803A58C8(NWC24iDlTask *task) {
    NWC24File file;
    NWC24Err result;
    NWC24Err close;
    u16 id;
    NWC24iDlTask *empty;

    result = NWC24FOpen(&file, DLFilePath, NWC24_OPEN_NAND_RW);
    if (result < 0) {
        return result;
    }

    empty = (NWC24iDlTask *)NWC24WorkP->dlTask;
    id = task->id;
    memset(empty, 0, DL_TASK_SIZE);
    empty->type = 0xFF;
    empty->id = id;
    memset(&NWC24WorkP->dlHeader[sizeof(NWC24iDlEntry) * id + 0x80], 0, sizeof(NWC24iDlEntry));

    result = WriteDlTaskEntry_i(empty, &file);
    if (result >= 0) {
        memset(&NWC24WorkP->dlHeader[sizeof(NWC24iDlEntry) * task->id + 0x80], 0, sizeof(NWC24iDlEntry));
        result = WriteDlHeader_i(&file);
    }

    close = NWC24FClose(&file);
    return result != NWC24_OK ? result : close;
}

// ---- looking at tasks ----

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

    for (i = *idx; i < DlHeader_i()->maxTasks; i++) {
        if (CheckDlEntryAvailable_i(i) >= 0) {
            *idx = i;
            return NWC24_OK;
        }
    }

    return NWC24_ERR_DONE;
}

static inline NWC24Err DeleteDlTaskForced_i(NWC24iDlTask *task) {
    NWC24Err result;

    result = CheckDlTask_i(task, FALSE);
    if (result != NWC24_OK) {
        return result;
    }

    result = fn_803A58C8(task);
    if (result >= 0) {
        task->id = 0xFFFF;
    }

    return result;
}

NWC24Err NWC24DeleteDlTaskForced(NWC24DlTask *pTask) {
    return DeleteDlTaskForced_i((NWC24iDlTask *)pTask);
}

static inline NWC24Err GetDlTask_i(NWC24iDlTask *pTask, u16 id) {
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
        result = NWC24FRead(pTask, DL_TASK_SIZE, &file);
        close = NWC24_OK;
        if (result < 0) {
            close = result;
        }
    }

    result = NWC24FClose(&file);
    return close != NWC24_OK ? close : result;
}

NWC24Err NWC24GetDlTask(NWC24DlTask *pTask, u16 id) {
    return GetDlTask_i((NWC24iDlTask *)pTask, id);
}

// the sorted walk over the task list. the state says what to sort by (last access / next time / flags),
// which direction and where the last call ended
typedef struct DlWalkState {
    u32 order;
    s32 best;
    s32 last;
    s32 lastIdx;
    u32 started;
    u32 valid;
} DlWalkState;

NWC24Err fn_803A367C(DlWalkState *state, u16 *out) {
    typedef u32 (*KeyFn)(u16);
    KeyFn getKey;
    NWC24Err result;
    BOOL desc;
    BOOL found = FALSE;
    u16 idx;
    s32 key;

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
        getKey = (KeyFn)fn_803A5C88;
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    if (state->started == 0) {
        result = fn_803A3540(&idx, TRUE);
        while (result >= 0) {
            key = getKey(idx);
            if (key == state->best && state->lastIdx < idx) {
                *out = idx;
                state->best = key;
                state->last = key;
                state->lastIdx = idx;
                return NWC24_OK;
            }

            result = fn_803A3540(&idx, FALSE);
        }
    }

    if (!(state->order & 0x80000000)) {
        state->best = 0x7FFFFFFF;
    } else {
        state->best = 0x80000001;
    }

    result = fn_803A3540(&idx, TRUE);
    while (result >= 0) {
        key = getKey(idx);
        if (desc ? (state->last < key) : (key < state->last)) {
            if (desc ? (key < state->best) : (state->best < key)) {
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

// throws the least recently used task out of the list to make space
NWC24Err fn_803A4110(void) {
    DlWalkState state;
    NWC24iDlTask task;
    u16 idx;
    NWC24Err result;
    NWC24Err close;
    NWC24File file;
    u16 max;

    memset(&state, 0, sizeof(state));
    state.best = 0x7FFFFFFF;
    state.last = 0x80000001;
    state.order = 0;
    state.lastIdx = -1;
    state.started = 1;
    state.valid = 1;

    do {
        result = fn_803A367C(&state, &idx);
    } while (result == NWC24_OK && idx < DlHeader_i()->privateTasks);

    if (result >= 0) {
        u16 id = idx;

        if (DlHeader_i() == NULL) {
            result = NWC24_ERR_LIB_NOT_OPENED;
        } else {
            result = CheckDlEntryAvailable_i(id);
            if (result >= 0) {
                result = NWC24FOpen(&file, DLFilePath, NWC24_OPEN_NAND_RBUFF);
                if (result >= 0) {
                    max = DlHeader_i()->maxTasks;
                    if (max > NWC24i_DL_TASK_MAX || id >= max) {
                        result = NWC24_ERR_INVALID_VALUE;
                    } else {
                        result = NWC24FSeek(&file, DL_HEADER_SIZE + id * DL_TASK_SIZE, NWC24_SEEK_BEG);
                    }

                    if (result < 0) {
                        close = result;
                    } else {
                        result = NWC24FRead(&task, DL_TASK_SIZE, &file);
                        close = NWC24_OK;
                        if (result < 0) {
                            close = result;
                        }
                    }

                    result = NWC24FClose(&file);
                    if (close != NWC24_OK) {
                        result = close;
                    }
                }
            }
        }

        if (result >= 0) {
            result = CheckDlTask_i(&task, FALSE);
            if (result == NWC24_OK) {
                result = fn_803A58C8(&task);
                if (result >= 0) {
                    task.id = 0xFFFF;
                }
            }
        }

        return result;
    }

    if (result == NWC24_ERR_DONE) {
        return NWC24_ERR_FAILED;
    }

    return result;
}

// is the sub task counter pointing at a sub task that is switched on? (-39 says "no, try the next one")
static inline NWC24Err SubTaskStatus_i(NWC24iDlTask *task, u8 counter) {
    NWC24Err check = CheckDlTask_i(task, FALSE);

    if (check == NWC24_OK) {
        if (task->subTaskType == 0) {
            check = -40;
        } else if (task->subTaskMask == 0) {
            check = NWC24_ERR_FATAL;
        } else if (counter > 31) {
            check = NWC24_ERR_INVALID_VALUE;
        } else if (!((1 << counter) & task->subTaskMask)) {
            check = -39;
        } else {
            check = NWC24_OK;
        }
    }

    return check;
}

// marks a task as just used: stamps the time, clears the errors, moves on to the next sub task and saves
static inline NWC24Err UpdateDlTask_i(NWC24iDlTask *task) {
    NWC24Err result;
    NWC24Err check;
    s64 now;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (task->id == 0xFFFF || task->id >= DlHeader_i()->maxTasks) {
        return NWC24_ERR_INVALID_VALUE;
    }

    result = NWC24iGetUniversalTime(&now);
    if (result >= 0) {
        s64 minutes = now;
        u16 id;

        check = CheckDlTask_i(task, FALSE);
        if (check == NWC24_OK) {
            id = task->id;
            if (id == 0xFFFF) {
                check = NWC24_ERR_FAILED;
            } else {
                DlEntry_i(id)->lastAccess = minutes / 60;
                check = NWC24_OK;
            }
        }

        result = check;
    }

    if (result < 0) {
        return result;
    }

    if (CheckDlTask_i(task, TRUE) == NWC24_OK) {
        task->lastError = 0;
        task->errorCount = 0;
    }

    if (task->subTaskType == 1) {
        while ((check = SubTaskStatus_i(task, task->subTaskCounter)) == -39) {
            task->subTaskCounter = (task->subTaskCounter + 1) & 0x1F;
        }

        if (check < 0) {
            return check;
        }
    }

    return fn_803A50EC(task);
}

NWC24Err fn_803A38C0(NWC24iDlTask *task) {
    return UpdateDlTask_i(task);
}

// takes a task for the list: gets a slot for it when it has none yet (kicking the oldest one out when the
// list is full), then saves it
NWC24Err fn_803A5284(NWC24iDlTask *task, u16 first, u16 last) {
    NWC24Err result;
    int url;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (task->id >= DlHeader_i()->maxTasks && task->id != 0xFFFF) {
        result = NWC24_ERR_INVALID_VALUE;
    } else {
        result = fn_80399EE0(task->url, 7, 0x100);
        if (result >= 0) {
            if (strncmp(task->url, "http://", 7) != 0 && strncmp(task->url, "https://", 8) != 0) {
                result = NWC24_ERR_FORMAT;
            } else {
                result = NWC24_OK;
            }
        }
    }

    if (result < 0) {
        return result;
    }

    for (;;) {
        if (task->id != 0xFFFF) {
            return UpdateDlTask_i(task);
        }

        if (task == NULL || first > last || first >= DlHeader_i()->maxTasks || last > DlHeader_i()->maxTasks) {
            result = NWC24_ERR_INVALID_VALUE;
        } else {
            u16 i;

            result = NWC24_ERR_FULL;
            for (i = first; i < last; i++) {
                if (DlEntry_i(i)->app == 0) {
                    task->id = i;
                    result = NWC24_OK;
                    break;
                }
            }
        }

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

// older list files only have a count for the sub tasks, this fills in the task limits of the header
static inline void FixDlHeader_i(void) {
    NWC24iDlHeader *hdr = DlHeader_i();

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
}

static inline NWC24Err CheckDlHeaderLimits_i(NWC24iDlHeader *hdr) {
    if (hdr->maxTasks < 1 || hdr->privateTasks < 1 || hdr->maxTasks < hdr->privateTasks) {
        return NWC24_ERR_BROKEN;
    }

    return NWC24_OK;
}

// puts a task into the list, filling in the next update time
NWC24Err fn_803A3D64(NWC24iDlTask *task) {
    NWC24Err result;

    FixDlHeader_i();
    result = CheckDlHeaderLimits_i(DlHeader_i());
    if (result < 0) {
        return result;
    }

    result = fn_803A5284(task, DlHeader_i()->privateTasks, DlHeader_i()->maxTasks);
    if (result < 0) {
        return result;
    }

    {
        s64 now = 0;

        result = NWC24iGetUniversalTime(&now);
        if (result >= 0) {
            s64 when = now + task->interval * 60;

            result = CheckDlTask_i(task, FALSE);
            if (result == NWC24_OK) {
                u16 id = task->id;

                if (id == 0xFFFF) {
                    result = NWC24_ERR_FAILED;
                } else {
                    DlEntry_i(id)->nextTime = when / 60;
                    result = NWC24_OK;
                }
            }
        }
    }

    return result;
}

// reads the list header from the file
NWC24Err fn_803A4D00(void) {
    NWC24File file;
    NWC24Err result;
    NWC24Err read = NWC24_OK;
    u32 length = 0;

    result = NWC24FOpen(&file, DLFilePath, NWC24_OPEN_READ);
    if (result < 0) {
        return result;
    }

    result = NWC24FSeek(&file, 0, NWC24_SEEK_BEG);
    if (result >= 0) {
        result = NWC24FRead(NWC24WorkP->dlHeader, DL_HEADER_SIZE, &file);
        if (result < 0) {
            read = result;
        }

        result = read;
    }

    if (result < 0) {
        return result;
    }

    read = NWC24FGetLength(&file, &length);
    if (read >= 0) {
        FixDlHeader_i();
        read = CheckDlHeaderLimits_i(DlHeader_i());
    }

    result = NWC24FClose(&file);
    return read != NWC24_OK ? read : result;
}

// walks the list and drops tasks that cannot be read back or that have run out of runs
NWC24Err fn_803A49B0(NWC24iDlHeader *hdr, BOOL clear) {
    NWC24iDlTask task;
    u16 i;

    for (i = 0; i < hdr->maxTasks; i++) {
        if (CheckDlEntryAvailable_i(i) == NWC24_OK && clear) {
            if (GetDlTask_i(&task, i) < 0) {
                DeleteDlTaskForced_i(&task);
            } else if (i >= DlHeader_i()->privateTasks && task.count == 0) {
                DeleteDlTaskForced_i(&task);
            }
        }
    }

    return NWC24_OK;
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

// copies the url out, optionally with a ".nn" counter on the end
NWC24Err fn_803A4E90(NWC24iDlTask *task, char *buf, u32 size, BOOL withNumber, int number) {
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
        snprintf(buf + len, size - len, ".%02d", number);
    }

    return NWC24_OK;
}

NWC24Err fn_803A3058(NWC24iDlTask *task, char *buf, u32 size) {
    return fn_803A4E90(task, buf, size, FALSE, 0xFF);
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

    return GetDlTask_i(out, idx);
}

static const char *WC24PubKFile = "wc24pubk.mod";

// <title data dir>/<file> for the title the task belongs to
static inline NWC24Err DlTaskCheckApp_i(const NWC24iDlTask *task) {
    NWC24Err result = CheckDlTask_i(task, FALSE);

    if (result != NWC24_OK) {
        return result;
    }

    if (task->appId == 0) {
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

static inline NWC24Err DlTaskDataPath_i(const NWC24iDlTask *task, char *path, const char *file) {
    NWC24Err result = DlTaskCheckApp_i(task);

    if (result != NWC24_OK) {
        return result;
    }

    result = DlTaskCheckApp_i(task);
    if (result != NWC24_OK) {
        return result;
    }

    snprintf(path, 0x40, "/title/%08x/%08x/data", task->titleIdHi, task->titleIdLo);
    return NWC24_OK;
}

// stores the 256 byte public key of the task's server
NWC24Err fn_803A43D0(NWC24iDlTask *task, const void *key) {
    char path[64];
    NWC24Err result;
    const char *file = WC24PubKFile;
    int len;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (key == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    result = DlTaskDataPath_i(task, path, file);
    if (result >= 0) {
        len = strlen(path);
        snprintf(path + len, 0x40 - len, "/%s", file);
        result = NWC24_OK;
    }

    if (result < 0) {
        return result;
    }

    result = NANDCreate(path, 0x3D, 0);
    if (result != 0 && result != NAND_RESULT_EXISTS) {
        return -21;
    }

    return fn_803A5A8C(path, 0, key, 0x100, 0);
}

// stores the 16 byte hash that belongs to the public key
NWC24Err fn_803A4684(NWC24iDlTask *task, const void *hash) {
    char path[64];
    NWC24Err result;
    const char *file = WC24PubKFile;
    int len;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (hash == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    result = DlTaskDataPath_i(task, path, file);
    if (result >= 0) {
        len = strlen(path);
        snprintf(path + len, 0x40 - len, "/%s", file);
        result = NWC24_OK;
    }

    if (result < 0) {
        return result;
    }

    if (fn_803A5B84(path, 0, NULL, 0) < 0) {
        return -40;
    }

    fn_803A5A8C(path, 0x100, NULL, 0, 0x100);
    return fn_803A5A8C(path, 0x200, hash, 0x10, 0);
}
