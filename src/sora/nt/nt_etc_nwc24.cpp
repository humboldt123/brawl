#include <revolution/NWC24/NWC24Download.h>
#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nt/nt_etc_global.h>
#include <nt/nt_etc_nwc24.h>
#include <nt/nt_offline.h>
#include <revolution/NWC24/NWC24Config.h>
#include <revolution/OS/OSTime.h>
#include <revolution/OS/OSMutex.h>
#include <revolution/NWC24/NWC24Manage.h>
#include <revolution/NWC24/NWC24MsgCommit.h>
#include <sr/sr_getappname.h>
#include <string.h>
#include <types.h>

// gfMutex is identified by the global object's registered destructor.
class gfMutex {
public:
    gfMutex() { OSInitMutex(&m_mutex); }
    ~gfMutex();
    OSMutex m_mutex;
};

// HYPOTHESIS: the scoped guard's original class name is unknown.
class NwcScopedLock {
public:
    NwcScopedLock(OSMutex* mutex) : m_mutex(mutex), m_locked(1) {
        if (m_locked) OSLockMutex(m_mutex);
    }
    ~NwcScopedLock() {
        if (m_locked) OSUnlockMutex(m_mutex);
    }
private:
    OSMutex* m_mutex;
    // BYTES: the destructor reads and rewrites the complete eight-bit lock flag.
    u8 m_locked : 8;
};

gfMutex lbl_8049ECC8 __attribute__((aligned(8)));
// Shared SDK workspace and count of objects that own an open operation.
NWC24Work* lbl_805A0450 = 0;
int lbl_805A0454 = 0;

// WiiConnect24 SDK calls, under the names RVL_SDK/nwc24/nwc24.c defines them with.
extern "C" {
// HYPOTHESIS: SDK signatures follow the argument registers and the records accessed by their callees.
s32 NWC24GetErrorCode();
int NWC24GetMsgSize(const NWC24MsgObj* message, u32* size);
int fn_8039B1B0(NWC24MsgObj* message, u32* id);
int fn_8039B5AC(int box, u32 id);
int fn_8039A350(int kind);
int fn_803A3324(NWC24DlTask* task);
int fn_803A3058(const NWC24DlTask* task, char* url, u32 capacity);
int fn_803A3D64(NWC24DlTask* task);
int fn_803A38C0(NWC24DlTask* task);
int fn_803A0158();
int fn_803A01D0(int box);
int fn_803A0240(u32 appId);
int fn_803A02A4(NWC24MsgObj* message, u32 count, u32* found, u32* remaining);
int fn_803A1AE8(u32* count);
int fn_803A1A7C(u32* count);
int fn_803A1C10(u32 index);
int fn_803A19D8(void* info, u32 index);
int fn_803A2A18(NWC24DlTask* task, int type);
// HYPOTHESIS: the map labels the URL setter NWC24SetDlPriority; its argument follows the callee's string accesses.
int fn_803A2EA4(NWC24DlTask* task, const char* url);
int fn_803A3064(NWC24DlTask* task, u32 flags);
int fn_803A43D0(NWC24DlTask* task, const u8* key);
int fn_803A4684(NWC24DlTask* task, const u8* key);
int fn_803A4FCC(NWC24DlTask* task, u32 flags);
int fn_803A2C58(NWC24DlTask* task, u16 interval);
int fn_803A31D0(NWC24DlTask* task, s16 count);
}

extern const char lbl_80460F48[];
extern const char* lbl_80460F04[];
extern const u8 lbl_80409660[];
extern const u8 lbl_80409760[];
extern u16 lbl_8059E7D4;
extern s16 lbl_8059E7D2;
extern u8 lbl_8059E7D8[8];

// NWC24IterateDlTask (the map name); src/RVL_SDK/nwc24/nwc24.c calls it by this stand-in name.
extern "C" NWC24Err fn_803A3540(u16* idx, BOOL first);

NWC24Err Patch_NWC24DeleteDlTasksForOptOut(u32 appId, BOOL all) {
    NWC24Err result = NWC24_OK;
    NWC24Err previousError = NWC24_OK;
    u16 index = 0;
    NWC24DlTask task;
    NWC24Err iteration = fn_803A3540(&index, TRUE);
    while (iteration >= 0) {
        if (previousError != NWC24_OK) result = previousError;
        previousError = result;
        result = NWC24GetDlTask(&task, index);
        if (result >= 0 && task.optFlags != 0 && (all || appId == task.appId)) {
            result = NWC24DeleteDlTaskForced(&task);
        }
        iteration = fn_803A3540(&index, FALSE);
    }
    return previousError;
}

u32 nteWiiConnect24::_getMailSentSizeToday() {
    u32 size = 0;
    OSCalendarTime sentDate;
    OSCalendarTime today;
    OSTicksToCalendarTime(OSGetTime(), &today);
    OSTicksToCalendarTime(nteGlobal::getNWCSendDate(), &sentDate);
    if (sentDate.year == today.year && sentDate.yday == today.yday) {
        size = nteGlobal::getNWCSendSize();
    }
    return size;
}

u8 nteWiiConnect24::getLangId() {
    u32 language = g_GameGlobal->getLanguage();
    if (language >= 7) language = 0;
    u8 languageId = lbl_8059E7D8[language];
    if (languageId == 17) languageId = 1;
    return languageId;
}

u64 nteWiiConnect24::getMyWiiID() {
    if (!_openLibrary(1)) return 0;
    u64 id;
    if (NWC24GetMyUserId(&id) != 0) id = 0;
    return id;
}

u16* nteWiiConnect24::friend_getName(u32 index) {
    if (index >= (u32)m_bufferSize) return 0;
    // HYPOTHESIS: each cached friend contains a UTF-16 name and two identifiers.
    struct { u16 unk0[12]; u64 unk18; u64 unk20; } *friends;
    friends = static_cast<__typeof__(friends)>(m_buffer);
    return friends[index].unk0;
}

u64 nteWiiConnect24::friend_getWiiID(u32 index) {
    if (index >= (u32)m_bufferSize) return 0;
    // HYPOTHESIS: each cached friend contains a UTF-16 name and two identifiers.
    struct { u16 unk0[12]; u64 unk18; u64 unk20; } *friends;
    friends = static_cast<__typeof__(friends)>(m_buffer);
    return friends[index].unk20;
}

void nteWiiConnect24::startSearchMail(u32 appId) {
    if (_openLibrary(1)) {
        fn_803A0158();
        fn_803A01D0(1);
        fn_803A0240(appId);
    }
}

bool nteWiiConnect24::getNextMailForMe(NWC24MsgObj* message, u32* remaining) {
    u32 found;
    u32 next;
    *remaining = 0;
    int error = fn_803A02A4(message, 1, &found, &next);
    if (error != 0) {
        _procGeneralError(error);
        return false;
    }
    if (found == 0) return false;
    *remaining = next;
    return true;
}

bool nteWiiConnect24::friend_makeList() {
    if (!_openLibrary(1)) return false;
    if (m_buffer) {
        gfHeapManager::free(m_buffer);
        m_bufferSize = 0;
        m_buffer = 0;
    }
    u32 count;
    int error = fn_803A1AE8(&count);
    if (error != 0 && !_procGeneralError(error)) return false;
    if (count == 0) return true;
    m_buffer = gfHeapManager::alloc(m_heap, count * 0x28);
    if (!m_buffer) return false;
    u32 capacity;
    error = fn_803A1A7C(&capacity);
    if (error != 0) {
        _procGeneralError(error);
        return false;
    }
    NtOffline offline;
    // HYPOTHESIS: the SDK friend record has two status words, a UTF-16 name, and two identifiers.
    struct {
        u32 unk0;
        u32 unk4;
        u16 unk8[12];
        u64 unk20;
        u8 unk28[0x18];
        u64 unk40;
        u8 unk48[0xf8];
    } info __attribute__((aligned(32)));
    for (u32 i = 0; i < capacity; ++i) {
        if (!fn_803A1C10(i)) continue;
        error = fn_803A19D8(&info, i);
        if (error == 0) {
            if (info.unk0 != 1 || info.unk4 != 2) continue;
            if (offline.getFriendIndexHaveWiiID(info.unk40) == 0xff) continue;
            // HYPOTHESIS: each cached friend contains a UTF-16 name and two identifiers.
            struct { u16 unk0[12]; u64 unk18; u64 unk20; } *friends;
            friends = m_bufferSize + static_cast<__typeof__(friends)>(m_buffer);
            friends->unk18 = info.unk20;
            friends->unk20 = info.unk40;
            memcpy(friends->unk0, info.unk8, 0x18);
            ++m_bufferSize;
        } else if (!_procGeneralError(error)) {
            return false;
        }
    }
    return true;
}

bool nteWiiConnect24::_setupDlTask(NWC24DlTask* task, u16 index, int* error) {
    int result = fn_803A2A18(task, 2);
    if (result != 0) {
        *error = result;
        return false;
    }
    char url[128];
    strcpy(url, lbl_80460F48);
    strcat(url, lbl_80460F04[index]);
    result = fn_803A2EA4(task, url);
    if (result != 0) {
        *error = result;
        return false;
    }
    result = fn_803A3064(task, 10);
    if (result != 0) {
        *error = result;
        return false;
    }
    result = fn_803A43D0(task, lbl_80409660);
    if (result != 0) {
        *error = result;
        return false;
    }
    result = fn_803A4684(task, lbl_80409760);
    if (result != 0) {
        *error = result;
        return false;
    }
    if (fn_803A4FCC(task, 1) != 0) return false;
    result = fn_803A2C58(task, lbl_8059E7D4);
    if (result != 0) {
        *error = result;
        return false;
    }
    result = fn_803A31D0(task, lbl_8059E7D2);
    if (result != 0) {
        *error = result;
        return false;
    }
    return true;
}

// Query first: the SDK error query precedes reloading the current callback.
static inline void NotifySdkError(nteWiiConnect24* owner, int kind, int error) {
    s32 code = NWC24GetErrorCode();
    IWiiConnect24_Error* handler = owner->m_errorHandler;
    if (handler) {
        int display = code;
        if (code < 0) display = static_cast<int>(0u - static_cast<u32>(code));
        handler->notify_Error(kind, error, display);
    }
}

nteWiiConnect24::nteWiiConnect24(Heaps::HeapType heap) {
    m_heap = heap;
    m_bufferSize = 0;
    m_buffer = 0;
    m_opened = false;
    m_errorHandler = 0;
}

nteWiiConnect24::~nteWiiConnect24() {
    if (m_buffer) {
        gfHeapManager::free(m_buffer);
        m_bufferSize = 0;
        m_buffer = 0;
    }
    NwcScopedLock lock(&lbl_8049ECC8.m_mutex);
    if (m_opened) {
        if (--lbl_805A0454 == 0) {
            NWC24CloseLib();
            if (lbl_805A0450) {
                gfHeapManager::free(lbl_805A0450);
                lbl_805A0450 = 0;
            }
        }
        m_opened = false;
    }
}

void nteWiiConnect24::setErrorHandler(IWiiConnect24_Error* handler) {
    m_errorHandler = handler;
}

bool nteWiiConnect24::check_getStatus(int kind) {
    if (!_openLibrary(1)) return false;
    bool ready = false;
    int result = fn_8039A350(kind);
    switch (result) {
    case 0: ready = true; break;
    case NWC24_ERR_DISABLED: NotifySdkError(this, 5, result); break;
    case NWC24_ERR_NETWORK: NotifySdkError(this, 6, result); break;
    case NWC24_ERR_SERVER: NotifySdkError(this, 7, result); break;
    case NWC24_ERR_FULL: NotifySdkError(this, 8, result); break;
    default: NotifySdkError(this, 3, result); break;
    }
    return ready;
}

bool nteWiiConnect24::dl_isRegisterDLTask() {
    if (!_openLibrary(1)) return false;
    NWC24DlTask task;
    if (fn_803A3324(&task) != 0) return false;
    if (NWC24CheckDlTask(&task, FALSE) != 0) return false;
    char url[128];
    url[0] = 0;
    if (fn_803A3058(&task, url, sizeof(url)) != 0 ||
        strncmp(url, lbl_80460F48, strlen(lbl_80460F48)) != 0) return false;
    return true;
}

static inline void NotifyError(IWiiConnect24_Error* handler, int kind, int error, int code) {
    if (handler) handler->notify_Error(kind, error, code);
}

static inline void NotifyDownloadError(nteWiiConnect24* owner, const int& error) {
    if (!owner->_procGeneralError(error)) return;
    if (!nteGlobal::isEnableNWCMail() && nteGlobal::isEnableNWCDL() == 1) {
        NotifyError(owner->m_errorHandler, 6, error, 109144);
    } else {
        NotifyError(owner->m_errorHandler, 3, error, 109999);
    }
}

static inline void NotifyUnhandledError(nteWiiConnect24* owner, const int& error) {
    if (owner->_procGeneralError(error)) {
        NotifyError(owner->m_errorHandler, 3, error, 109999);
    }
}

bool nteWiiConnect24::dl_registerDLTask() {
    u16 language = getLangId();
    if (language >= 17 || lbl_80460F04[language][0] == 0) return false;
    if (!nteGlobal::isPermitService()) return false;
    if (!_openLibrary(1)) return false;
    NWC24DlTask task;
    int error = fn_803A3324(&task);
    if (error == NWC24_ERR_NOT_FOUND) {
        if (!_setupDlTask(&task, language, &error)) {
            NotifyUnhandledError(this, error);
            return false;
        }
        error = fn_803A3D64(&task);
        if (error != 0) {
            NotifyDownloadError(this, error);
            return false;
        }
    } else if (error != 0) {
        NotifyUnhandledError(this, error);
        return false;
    } else {
        error = fn_803A2C58(&task, lbl_8059E7D4);
        if (error != 0) {
            NotifyUnhandledError(this, error);
            return false;
        }
        error = fn_803A31D0(&task, lbl_8059E7D2);
        if (error != 0) {
            NotifyUnhandledError(this, error);
            return false;
        }
        error = fn_803A38C0(&task);
        if (error != 0) {
            NotifyDownloadError(this, error);
            return false;
        }
    }
    return true;
}

bool nteWiiConnect24::dl_deleteDLTask() {
    if (!_openLibrary(1)) return false;
    int error = Patch_NWC24DeleteDlTasksForOptOut(srGetAppInitialCode(), FALSE);
    if (error != 0) {
        NotifyUnhandledError(this, error);
        return false;
    }
    return true;
}

bool nteWiiConnect24::dl_deleteDLTaskAll() {
    if (!_openLibrary(1)) return false;
    int error = Patch_NWC24DeleteDlTasksForOptOut(srGetAppInitialCode(), TRUE);
    if (error != 0) {
        NotifyUnhandledError(this, error);
        return false;
    }
    return true;
}

#pragma push
// BYTES: callers retain the out-of-line library and general-error boundaries.
#pragma auto_inline off
int nteWiiConnect24::_openLibrary(int kind) {
    OSMutex* mutex = &lbl_8049ECC8.m_mutex;
    OSLockMutex(mutex);
    if (m_opened) {
        OSUnlockMutex(mutex);
        return 1;
    }
    if (lbl_805A0454 != 0) {
        OSUnlockMutex(mutex);
        return 1;
    }
    OSUnlockMutex(mutex);
    NWC24Work* work = lbl_805A0450;
    if (!work) {
        work = static_cast<NWC24Work*>(gfHeapManager::alloc(m_heap, 0x4000, 0x20));
        lbl_805A0450 = work;
        if (!work) {
            if (m_errorHandler) m_errorHandler->notify_Error(3, NWC24_ERR_FATAL, 109999);
            return 0;
        }
    }
    // The game makes one open attempt, then sleeps on transient errors before reporting failure.
    int result = NWC24_ERR_FATAL;
    if (work) {
        result = NWC24OpenLib(lbl_805A0450);
        if (kind) {
            switch (result) {
            case NWC24_ERR_MUTEX:
            case NWC24_ERR_BUSY:
            case NWC24_ERR_INPROGRESS:
                OSSleepTicks(OS_MSEC_TO_TICKS(1000LL));
                break;
            }
        }
    }
    if (result != 0 && lbl_805A0450) {
        gfHeapManager::free(lbl_805A0450);
        lbl_805A0450 = 0;
    }
    switch (result) {
    case 0:
        OSLockMutex(&lbl_8049ECC8.m_mutex);
        m_opened = true;
        ++lbl_805A0454;
        OSUnlockMutex(&lbl_8049ECC8.m_mutex);
        return 1;
    case NWC24_ERR_MUTEX: case NWC24_ERR_BUSY: case NWC24_ERR_INPROGRESS:
        NotifySdkError(this, 4, result);
        break;
    case NWC24_ERR_FILE_BROKEN: case NWC24_ERR_INTERNAL_VF: case NWC24_ERR_NAND_CORRUPT:
    case NWC24_ERR_FILE_OTHER: case NWC24_ERR_FILE_NOEXISTS: case NWC24_ERR_FILE_WRITE: case NWC24_ERR_FILE_READ: case NWC24_ERR_FILE_CLOSE: case NWC24_ERR_FILE_OPEN:
    case NWC24_ERR_BROKEN:
        NotifySdkError(this, 1, result);
        break;
    case NWC24_ERR_OLD_SYSTEM:
        NotifySdkError(this, 2, result);
        break;
    case NWC24_ERR_INTERNAL_IPC: case NWC24_ERR_FATAL:
        NotifySdkError(this, 3, result);
        break;
    default:
        NotifySdkError(this, 3, result);
        break;
    }
    return 0;
}

int nteWiiConnect24::_procGeneralError(int error) {
    int unhandled = 0;
    if ((error <= NWC24_ERR_FILE_OPEN && error >= NWC24_ERR_FILE_OTHER) || error == NWC24_ERR_NAND_CORRUPT || error == NWC24_ERR_FILE_EXISTS ||
        error == NWC24_ERR_INTERNAL_VF || error == NWC24_ERR_FILE_BROKEN) {
        NotifySdkError(this, 1, error);
    } else if (error == NWC24_ERR_FATAL || error == NWC24_ERR_NOMEM || error == NWC24_ERR_INTERNAL_IPC) {
        NotifySdkError(this, 3, error);
    } else {
        unhandled = 1;
    }
    return unhandled;
}
#pragma pop

bool nteWiiConnect24::sendMails(u32 count, NWC24MsgObj** messages) {
    if (!_openLibrary(1)) return false;
    int error;
    u32 sent = 0;
    u32 totalSize = 0;
    bool failed = false;
    for (u32 i = 0; i < count; ++i) {
        error = NWC24CommitMsg(messages[i]);
        if (error != 0) {
            if (_procGeneralError(error)) {
                switch (error) {
                case NWC24_ERR_BROKEN: NotifySdkError(this, 1, error); break;
                case NWC24_ERR_OVERFLOW: case NWC24_ERR_FULL: NotifySdkError(this, 8, error); break;
                default: NotifySdkError(this, 3, error); break;
                }
            }
            failed = true;
            break;
        }
        u32 size = 0;
        if (NWC24GetMsgSize(messages[i], &size) == 0) totalSize += size;
        ++sent;
    }
    if (failed) {
        // Roll back messages already committed by this batch.
        for (u32 i = 0; i < sent; ++i) {
            u32 id = 0;
            if (fn_8039B1B0(messages[i], &id) == 0) fn_8039B5AC(0, id);
        }
        return false;
    }
    OSCalendarTime today;
    OSCalendarTime sentDate;
    OSTicksToCalendarTime(OSGetTime(), &today);
    OSTicksToCalendarTime(nteGlobal::getNWCSendDate(), &sentDate);
    if (sentDate.year == today.year && sentDate.yday == today.yday) {
        u32 size = nteGlobal::getNWCSendSize();
        size += totalSize;
        nteGlobal::setNWCSendSize(size);
    } else {
        nteGlobal::setNWCSendDate(OSGetTime());
        nteGlobal::setNWCSendSize(totalSize);
    }
    return true;
}
