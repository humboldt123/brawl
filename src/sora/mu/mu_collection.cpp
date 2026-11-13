#include <mu/mu_collection.h>
#include <mu/mu_recorder.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_model.h>
#include <gf/gf_sys_recorder2.h>
#include <gm/gm_global.h>
#include <ms/ms_message.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <gf/gf_collection_io.h>
#include <gf/gf_file_io.h>
#include <if/if_snap_save.h>
#include <mu/mu_notice_wnd.h>
#include <revolution/OS/OSTime.h>
#include <revolution/GX/GXTexture.h>
#include <revolution/OS/OSCache.h>

extern const char lbl_8059E148[1];
extern const char lbl_8059E14C[1];

void muRecorder::saveAdv(u32 slot) {
    m_slot = slot;
    m_request = Request_SaveAdv;
}

void muRecorder::loadAdv(u32 slot) {
    m_slot = slot;
    m_request = Request_LoadAdv;
}

inline muRecorder::Data::~Data() {
    reset(0);
}

void muRecorder::Data::reset(void* data) {
    if (unk0) gfHeapManager::free(unk0);
    unk0 = data;
}

// The name of the model of the object (the node of the model's name string, NULL when it has none).
static inline const char* getResMdlName(MuObject* object) {
    const nw4r::g3d::ResMdlData* data = object->m_resMdl.ptr();
    return data->m_toName != 0 ? reinterpret_cast<const char*>(data) + data->m_toName : 0;
}

muRecorder::muRecorder(int kind, nw4r::g3d::ResFile modelRes, nw4r::g3d::ResFile msgRes, void* msgData, void* scnRoot,
                       int drawPriority, Heaps::HeapType heap, Heaps::HeapType heap2, u8 wndUnk0, u8 wndUnk2,
                       const char* name)
    : gfTask(name, gfTask::Category(8), 15, 8, true),
      muRecorderBase(heap, heap2, msgRes, msgData, drawPriority, wndUnk0, wndUnk2, scnRoot),
      m_modelRes(modelRes), m_sysRecorder(0), m_sysRecorderUsed(false), m_recKind(kind == 1 ? 1 : 3), m_kind(kind), m_request(0) {
    unk2C_b1 = false;
    if (!m_modelRes.IsValid()) {
        m_obj.reset(0);
    } else {
        m_obj.reset(MuObject::create(&m_modelRes, "InfCmnWait_TopN", drawPriority + 1, 0, m_heap));
        m_obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
        m_obj->changeNodeAnimN(getResMdlName(m_obj.get()));
        m_obj->changeTexSrtAnimN(getResMdlName(m_obj.get()));
    }
}

muRecorder::~muRecorder() {}

void muRecorder::check() {
    m_request = Request_Check;
}

// The locals of a subroutine of processDefault: they live on the stack of the script (see muScriptState).
// HYPOTHESIS: the first and the last word are guards; the last one is set to the first one before the frame is pushed.
struct muRecorderFrame3 {
    int m_head;
    int m_slot;
    int m_tail;
};

struct muRecorderFrame4 {
    int m_head;
    int m_slot;
    int m_result;
    int m_tail;
};

// A text that is put together for a notice window.
struct muRecorderText {
    char m_data[0x200];
    int m_size;
};

struct muRecorderSlice {
    char* m_data;
    u32 m_size;
};

// The text of the message with the number id of the message data. BYTES: the result is returned by value (the stack
// holds the local of this function and the returned structure).
static inline muRecorderSlice getText(void* msgData, u32 id) {
    muRecorderSlice slice;
    Message::getPrintIndexData(msgData, id, &slice.m_data, &slice.m_size);
    return slice;
}

static inline void appendText(muRecorderText& text, muRecorderSlice slice) {
    int size = text.m_size;
    memcpy(text.m_data + size, slice.m_data, slice.m_size);
    text.m_size = size + slice.m_size;
}

static inline void appendText(muRecorderText& text, const char* data, int size) {
    int used = text.m_size;
    memcpy(text.m_data + used, data, size);
    text.m_size = used + size;
}

// The text of the message for a number of blocks: the first part, the number and the last part.
// BYTES: the head is a by-value parameter and the tail a reference (one copy of each, as on the stack of the original),
// and the arguments are evaluated from the right (the tail is fetched first).
static inline void appendBlocksText(muRecorderText& text, muRecorderSlice head, int blocks, const muRecorderSlice& tail) {
    appendText(text, head.m_data, head.m_size);
    muRecorderText number;
    number.m_data[0] = 0;
    sprintf(number.m_data + strlen(number.m_data), "%d", blocks);
    appendText(text, number.m_data, strlen(number.m_data));
    appendText(text, tail.m_data, tail.m_size);
}

// BYTES: the position is set by a function that owns the vector (this puts it below the frames of the script).
static inline void setObjectTrans(std::auto_ptr<MuObject>& object, float x, float y, float z) {
    Vec3f trans(x, y, z);
    object->setTrans(&trans);
}

#define MU_WAIT_SYSREC(ret)                         \
    do {                                            \
        muRecorderFrame3 waitFrame = {-1, 0, 0};    \
        MU_CALL(muRecorderFrame3, waitFrame, State_WaitSysRec, ret); \
    } while (0)

#define MU_POP()                                    \
    do {                                            \
        m_script.m_sp--;                            \
        m_script.m_state = *m_script.m_sp;          \
    } while (0)

#define MU_PUSH_RET(callee, ret)                    \
    do {                                            \
        *m_script.m_sp++ = (ret);                   \
        m_script.m_state = (callee);                \
    } while (0)

#define MU_CALL(FrameType, frame, callee, ret)      \
    do {                                            \
        (frame).m_tail = (frame).m_head;            \
        *reinterpret_cast<FrameType*>(m_script.m_sp) = (frame); \
        m_script.m_sp += sizeof(FrameType) / 4;     \
        *m_script.m_sp++ = (ret);                   \
        m_script.m_state = (callee);                \
    } while (0)

void muRecorder::processDefault() {
    muRecorderText text;

    switch (m_script.m_state) {
    case State_Init:
        m_script.m_state = State_Begin;
        goto begin;

    case State_WaitWnd:
        if (m_wnd->m_step == 3) MU_POP();
        break;

    case State_WaitSysRec:
        if (m_sysRecorder->m_busy == 0) MU_POP();
        break;

    case State_Check:
        m_flag7 = false;
        m_flag6 = false;
        if (!gfSysRecorder2::isEnable(m_recKind)) goto error;
        if (m_sysRecorder->requestCheck(m_heap2, m_recKind)) {
            m_sysRecorderUsed = true;
            MU_WAIT_SYSREC(State_CheckDone);
            break;
        case State_CheckDone:
            m_script.m_sp -= 3;
        } else {
            MU_POP();
            break;
        }
        // BYTES: the pop of a result that is not 0 is placed in front of the cases of the switch.
        if (m_sysRecorder->m_result != 0) goto checkDonePop;
        switch (m_sysRecorder->m_checkResult) {
        checkDonePop:
            MU_POP();
            break;
        case 0:
        default:
            m_flag7 = true;
            MU_POP();
            break;
        case 1: {
            text.m_size = 0;
            switch (m_kind) {
            case 1:
            default: {
                appendText(text, getText(m_msgData, 0x27));
                break;
            }
            case 2:
            case 3:
            case 4: {
                appendText(text, getText(m_msgData, 0x28));
                break;
            }
            }
            appendText(text, getText(m_msgData, 0x2C));
            m_wnd = fn_800BF048(1, text.m_data, text.m_size, &m_msgRes, m_scnRoot, m_drawPriority, 0, m_heap, m_heap2,
                                m_wndUnk0, 1, m_wndUnk2);
            MU_PUSH_RET(State_WaitWnd, State_RenewStart);
            break;
        }
        case 2:
            goto error;
        }
        break;

    case State_RenewStart:
        if (m_wnd->m_selected == 0) goto error;
        m_wnd = fn_800BEEB8(3, m_msgData, 0x26, &m_msgRes, m_scnRoot, m_drawPriority, 0, m_heap, m_heap2, m_wndUnk0, 1,
                            m_wndUnk2);
        if (m_obj.get()) {
            nw4r::g3d::ScnGroup* scnGroup = static_cast<nw4r::g3d::ScnGroup*>(m_scnRoot);
            scnGroup->Insert(scnGroup->Size(), m_obj->m_sceneModel);
            m_obj->setFrameNode(0.0f);
            m_obj->setFrameTexSrt(0.0f);
            setObjectTrans(m_obj, 0.0f, -7.0f, 0.0f);
        }
        if (m_sysRecorder->requestRenew(m_heap2, m_recKind)) {
            m_sysRecorderUsed = true;
            MU_WAIT_SYSREC(State_RenewWait);
            break;
        case State_RenewWait:
            m_script.m_sp -= 3;
        } else {
            MU_POP();
            break;
        }
        if (m_wnd) {
            m_wnd->setStep(2);
            MU_PUSH_RET(State_WaitWnd, State_RenewDone);
            break;
        case State_RenewDone:;
        }
        if (m_obj.get()) m_obj->m_scnMdl->DetachFromParent();
        {
            u8 flags = g_GameGlobal->getEnableSaveLoad();
            u8 bit = 1 << m_recKind;
            u8 enabled = flags & ~bit;
            if (m_sysRecorder->m_result == 0) enabled = flags | bit;
            g_GameGlobal->setEnableSaveLoad(enabled);
        }
        switch (m_sysRecorder->m_result) {
        case 3:
            MU_POP();
            break;
        case 0:
        default:
            m_flag7 = true;
            m_flag6 = true;
            MU_POP();
            break;
        case 1:
            m_wnd = fn_800BEEB8(0, m_msgData, 0x2D, &m_msgRes, m_scnRoot, m_drawPriority, 0, m_heap, m_heap2, m_wndUnk0,
                                1, m_wndUnk2);
            MU_PUSH_RET(State_WaitWnd, State_ErrorWnd);
            break;
        case 2: {
            gfSysRecorder2Require* require = m_sysRecorder->m_require;
            int blocks = 0;
            gfSysRecorder2Require* entry = require;
            int i = 0;
            while (entry->m_kind != 6) {
                if (m_recKind == entry->m_kind) {
                    blocks = require[i].m_unk8;
                    break;
                }
                entry++;
                i++;
            }
            muRecorderText message;
            message.m_size = 0;
            appendBlocksText(message, getText(m_msgData, 0x2E), blocks, getText(m_msgData, 0x2F));
            m_wnd = fn_800BF048(0, message.m_data, message.m_size, &m_msgRes, m_scnRoot, m_drawPriority, 0, m_heap,
                                m_heap2, m_wndUnk0, 1, m_wndUnk2);
            MU_PUSH_RET(State_WaitWnd, State_ErrorWnd2);
            break;
        }
        }
        break;

    case State_ErrorWnd:
    case State_ErrorWnd2:
    error:
        m_wnd = fn_800BEEB8(0, m_msgData, 0x30, &m_msgRes, m_scnRoot, m_drawPriority, 0, m_heap, m_heap2, m_wndUnk0, 1,
                            m_wndUnk2);
        MU_PUSH_RET(State_WaitWnd, State_ErrorWndRet);
        break;

    case State_ErrorWndRet:
        MU_POP();
        break;

    case State_SaveAdv: {
        m_advData.reset(0);
        if (!gfSysRecorder2::isEnable(1)) {
            MU_POP();
            break;
        }
        void* buffer = m_sysRecorder->getBuffer(1);
        if (!buffer) {
            MU_POP();
            break;
        }
        g_GameGlobal->getAdventureSaveData(buffer);
        if (m_sysRecorder->saveAdventure(
                static_cast<u8>(reinterpret_cast<muRecorderFrame3*>(m_script.m_sp - 4)->m_slot))) {
            m_sysRecorderUsed = true;
            MU_WAIT_SYSREC(State_SaveAdvDone);
            break;
        case State_SaveAdvDone:
            m_script.m_sp -= 3;
        } else {
            MU_POP();
            break;
        }
        MU_POP();
        break;
    }

    case State_LoadAdv: {
        reinterpret_cast<muRecorderFrame4*>(m_script.m_sp - 5)->m_result = 0;
        m_advData.reset(0);
        if (!gfSysRecorder2::isEnable(1)) {
            MU_POP();
            break;
        }
        if (m_sysRecorder->loadAdventure(m_heap2, static_cast<u8>(reinterpret_cast<muRecorderFrame4*>(m_script.m_sp - 5)->m_slot))) {
            m_sysRecorderUsed = true;
            MU_WAIT_SYSREC(State_LoadAdvDone);
            break;
        case State_LoadAdvDone:
            m_script.m_sp -= 3;
        } else {
            MU_POP();
            break;
        }
        if (m_sysRecorder->m_result == 0) {
            m_advData.reset(m_sysRecorder->m_handle.getBuffer());
        } else {
            reinterpret_cast<muRecorderFrame4*>(m_script.m_sp - 5)->m_result = 1;
        }
        MU_POP();
        break;
    }

    case State_Begin:
    begin:
        m_script.m_state = State_Idle;
        break;

    case State_Idle:
        if (m_request == 0) goto done;
        m_sysRecorder = gfSysRecorder2::createInstance(m_heap2);
        if (m_request == Request_Check) {
            MU_PUSH_RET(State_Check, State_CheckRet);
            break;
        case State_CheckRet:
            m_failed = m_flag7 ^ 1; // BYTES: xori of the extracted flag (the not operator compiles to cntlzw)
        } else if (m_request == Request_SaveAdv) {
            {
                muRecorderFrame3 frame = {-1, 0, 0};
                frame.m_slot = m_slot;
                MU_CALL(muRecorderFrame3, frame, State_SaveAdv, State_SaveAdvRet);
            }
            break;
        case State_SaveAdvRet:
            m_failed = 0;
            m_script.m_sp -= 3;
        } else if (m_request == Request_LoadAdv) {
            {
                muRecorderFrame4 frame = {-1, 0, 0, 0};
                frame.m_slot = m_slot;
                MU_CALL(muRecorderFrame4, frame, State_LoadAdv, State_LoadAdvRet);
            }
            break;
        case State_LoadAdvRet:
            m_script.m_sp -= 4;
            m_failed = reinterpret_cast<muRecorderFrame4*>(m_script.m_sp)->m_result;
        }
        if (m_sysRecorder) {
            m_sysRecorderUsed = false;
            gfSysRecorder2::dropInstance();
            m_sysRecorder = 0;
        }
    done:
        m_request = 0;
        goto begin;
    }
}

namespace {
struct StringLess {
    bool operator()(const char* a, const char* b) const {
        return strcmp(a, b) < 0;
    }
};
} // namespace

char* muCollection::FileInfo::copyFileName(Heaps::HeapType heap) const {
    if (m_count == 0) {
        return 0;
    }
    u32 total = 0;
    const char** it;
    const char** names = (const char**)gfHeapManager::alloc(heap, m_count * sizeof(char*));
    it = names;
    for (u32 i = 0; i < m_count; i++) {
        *it = (char*)m_names.unk0 + total;
        total += strlen(*it) + 1;
        it++;
    }
    if (total == 0) {
        if (names) {
            gfHeapManager::free(names);
        }
        return 0;
    }
    std::sort(names, names + m_count, StringLess());
    char* out = (char*)gfHeapManager::alloc(heap, total);
    char* dst = out;
    it = names;
    for (u32 i = 0; i < m_count; i++) {
        const char* name = *it;
        u32 n = strlen(name);
        u32 len = n < 0x40 ? n : 0x40;
        memcpy(dst, name, len + 1);
        dst[len] = 0;
        dst += len + 1;
        it++;
    }
    if (names) {
        gfHeapManager::free(names);
    }
    return out;
}

muCollection::FileHandle::FileHandle(const char* name) {
    m_name[0] = 0;
    strcat(m_name, lbl_8059E148);
    m_name[0] = 0;
    strcat(m_name, name);
}

muCollection::FileHandle::FileHandle(const char* names, int index) {
    m_name[0] = 0;
    strcat(m_name, lbl_8059E148);
    for (int i = 0; i < index; i++) {
        names += strlen(names) + 1;
    }
    m_name[0] = 0;
    strcat(m_name, names);
}

static inline int getBaseKind(int kind) {
    switch (kind) {
    case 1:
        return 2;
    case 2:
        return 3;
    case 3:
        return 4;
    default:
        return 0;
    }
}

muCollection::muCollection(int kind, nw4r::g3d::ResFile modelRes, nw4r::g3d::ResFile msgRes, void* msgData,
                           void* scnRoot, int drawPriority, Heaps::HeapType heap, Heaps::HeapType heap2, u8 unk0,
                           u8 unk2)
    : muRecorder(getBaseKind(kind), modelRes, msgRes, msgData, scnRoot, drawPriority, heap, heap2, unk0, unk2,
                 "muCollection"),
      m_io(new (m_heap2) gfCollectionIO(m_heap2)), m_ioPending(false), m_fileType(kind), m_loadSize(0), m_busy(0),
      m_result(2) {
    unk2C_b1 = false;
}

muCollection::FileInfo::FileInfo() : unk0(0), unk1(0), m_count(0), m_free(0) {
}

muCollection::FileInfo::~FileInfo() {
}

muCollection::~muCollection() {
    m_io->isDropReady(); // HYPOTHESIS: the result is not used
}

bool muCollection::isDropReady() {
    bool ready = false;
    if (m_busy == 0 && m_io->isDropReady()) {
        ready = true;
    }
    return ready;
}

// PART C of the mu_recorder / mu_collection translation unit: muCollection::processDefault.
//
// The function is a coroutine in the style of a protothread. m_script.m_state is the place to go on at (in the
// original every state is the __LINE__ of the macro that made it, so the values are fixed by the enum
// muCollection::State), m_script.m_stack is the call stack: a "call" pushes the locals frame of the callee (a struct
// of 3 to 8 words: -1, the arguments, a result, -1) and then the state to go on at when the callee returns, and
// "returns" by popping that state. The code after a call is the state of the return.


// HYPOTHESIS: the words at lbl_80423690 (gf_collection_io.cpp, u8[0x10] there): the state of the SD card (0 and 2:
// usable, 1 and 3: windows, others: not usable) and the free blocks of the SD card and of the console.
struct MuCollMedia {
    u32 unk0;
    s32 sdState;
    u32 sdFree;
    u32 vfFree;
};
extern MuCollMedia lbl_80423690;
// HYPOTHESIS: the limit of files of a collection (1000), a variable of the unit (lbl_8059E120) that sprintf reads.
extern int lbl_8059E120;

// HYPOTHESIS: nteSection (nt_etc_file.o) is still fn_ for its constructor, destructor and parser; the second one takes
// the loaded data.
extern "C" void fn_80153488(nteSection* section);
extern "C" bool fn_80153504(nteSection* section, void* data);
extern "C" void fn_80153498(nteSection* section, int unk);


namespace {

// ---- the locals frames of the subroutines (head and tail are -1; see CO_CALL) ----

struct FrIo { // WaitIo, ListFiles(sd)
    int head;
    int sd;
    int tail;
};
struct FrScan { // ScanVf, ScanSd
    int head;
    u8 listFiles;
    u8 reportMissing;
    int tail;
};
struct FrMedia { // MediaWnd
    int head;
    int mode;           // 0: the console, else the SD card
    bool saveFirst;
    int tail;
};
struct FrCount { // ListCount
    int head;
    int drive;
    int result;
    int tail;
};
struct FrDel { // Erase, EraseSd
    int head;
    char* name;
    int result;
    int tail;
};
struct FrLoad { // ReadSummary, LoadFile, Read, LoadForCopy, ReadAll, MoveAll
    int head;
    int sd;
    char* name;
    int result;
    int tail;
};
struct FrReport { // ReportMedia
    int head;
    u8 checkSpace;
    u32 blocks;
    u8 ok;
    int tail;
};
struct FrWrite { // Write, Copy
    int head;
    u8 overwrite;
    char* name;
    void* data;
    u32 size;
    int result;
    int tail;
};
struct FrNew { // NewFile
    int head;
    u8 overwrite;
    int sd;
    char* name;
    void* data;
    u32 size;
    int result;
    int tail;
};

// The text of a window: the messages and numbers one after the other.
struct MuCollText {
    char* str;
    u32 len;
};
struct MuCollTextBuf {
    char data[0x200];
    int len;
};

} // namespace

// Calls the subroutine `callee` and goes on at `ret` (a case label) when it returns. FT is the frame of the callee.
#define CO_CALL(FT, callee, ret, ...)                                                                                  \
    {                                                                                                                  \
        FT f_ = __VA_ARGS__;                                                                                           \
        f_.tail = f_.head;                                                                                             \
        *reinterpret_cast<FT*>(m_script.m_sp) = f_;                                                                    \
        m_script.m_sp += sizeof(FT) / 4;                                                                               \
        *m_script.m_sp++ = (ret);                                                                                      \
        m_script.m_state = (callee);                                                                                   \
        return;                                                                                                        \
    }                                                                                                                  \
    case (ret):                                                                                                        \
        m_script.m_sp -= sizeof(FT) / 4;

// The same for a subroutine without a frame.
#define CO_CALL0(callee, ret)                                                                                          \
    {                                                                                                                  \
        *m_script.m_sp++ = (ret);                                                                                      \
        m_script.m_state = (callee);                                                                                   \
        return;                                                                                                        \
    }                                                                                                                  \
    case (ret):

// Returns to the caller (the frame of the running subroutine stays for the caller to read the result from).
#define CO_RETURN()                                                                                                    \
    do {                                                                                                               \
        m_script.m_sp--;                                                                                               \
        m_script.m_state = *m_script.m_sp;                                                                             \
        return;                                                                                                        \
    } while (0)

// The locals of the running subroutine (just below the return state), and the frame of the subroutine that returned.
#define CO_FR(FT) reinterpret_cast<FT*>(m_script.m_sp - (sizeof(FT) / 4 + 1))
#define CO_CALLEE(FT) reinterpret_cast<FT*>(m_script.m_sp)

// Opens the notice window with the message `id` and closes the one that is open.
#define CO_OPEN_WINDOW(id)                                                                                             \
    m_wnd.reset(fn_800BEEB8(0, m_msgData, (id), &m_msgRes, m_scnRoot, m_drawPriority, 0, m_heap, m_heap2, m_wndUnk0, 1, \
                            m_wndUnk2))

// The same with the text that was put together.
static inline void muCollOpenTextWindow(muCollection* t, MuCollTextBuf* text) {
    muNoticeWndTask* wnd = fn_800BF048(0, text->data, text->len, &t->m_msgRes, t->m_scnRoot, t->m_drawPriority, 0,
                                       t->m_heap, t->m_heap2, t->m_wndUnk0, 1, t->m_wndUnk2);
    t->m_wnd.reset(wnd);
}

static inline void muCollAdd(MuCollTextBuf* buf, const char* str, int len) {
    int pos = buf->len;
    memcpy(buf->data + pos, str, len);
    buf->len = pos + len;
}

static inline MuCollText muCollGetText(void* msgData, u32 id) {
    MuCollText text;
    Message::getPrintIndexData(msgData, id, &text.str, &text.len);
    return text;
}

static inline void muCollAddText(MuCollTextBuf* buf, MuCollText text) {
    muCollAdd(buf, text.str, text.len);
}

// The text of a window: head, the number, tail.
static inline void muCollMakeText(MuCollTextBuf* buf, MuCollText head, int number, MuCollText tail) {
    muCollAdd(buf, head.str, head.len);
    char digits[0x204]; // BYTES: 0x204, like MuCollTextBuf, gives the stack layout of the original
    digits[0] = 0;
    sprintf(digits + strlen(digits), "%d", number);
    muCollAdd(buf, digits, strlen(digits));
    muCollAdd(buf, tail.str, tail.len);
}

// The same with the limit of files (1000).
static inline void muCollMakeLimitText(MuCollTextBuf* buf, MuCollText head, MuCollText tail) {
    muCollAdd(buf, head.str, head.len);
    char digits[0x204]; // BYTES: see above
    digits[0] = 0;
    sprintf(digits + strlen(digits), "%d", lbl_8059E120);
    muCollAdd(buf, digits, strlen(digits));
    muCollAdd(buf, tail.str, tail.len);
}

void muCollection::processDefault() {
    // The texts of the windows (one for each state that makes one).
    MuCollTextBuf countText;
    MuCollTextBuf writeText;
    MuCollTextBuf reportText;

    muRecorder::processDefault();

    // The state of the drives (from the state of the SD card in lbl_80423690).
    bool sdUsable = false;
    const MuCollMedia* media = &lbl_80423690;
    m_drives[0].m_state = 0;
    m_drives[0].m_free = media->vfFree;
    m_drives[1].unk1 = media->sdState == 0;
    if (media->sdState == 0 || media->sdState == 2) {
        sdUsable = true;
    }
    m_drives[1].unk0 = sdUsable;
    m_drives[1].m_free = media->sdFree;
    switch (media->sdState) {
    case 0:
    case 2:
        m_drives[1].m_state = 1;
        break;
    case 3:
        m_drives[1].m_state = 3;
        break;
    default:
        m_drives[1].m_state = 2;
        break;
    }
    if (!m_drives[1].unk0) {
        m_drives[1].m_count = 0;
    }

    switch (m_script.m_state) {
    case State_Init:
        m_script.m_state = State_Start;
        goto start;

    // wait until the window was answered
    case State_WaitWnd:
        if (m_wnd->m_step != 3) {
            return;
        }
        CO_RETURN();

    // wait until the io task is ready
    case State_WaitIo:
        if (!m_io->m_isReady) {
            return;
        }
        CO_RETURN();

    // let the io task go and make a new one (m_request is the mutex of the muRecorder part)
    case State_ResetIo:
        if (m_request != 0) {
            return;
        }
        if (!m_io->isDropReady()) {
            return;
        }
        if (m_io) {
            gfTask* task = m_io;
            task->exit();
        }
        m_io = 0;
        m_request = 1;
        m_script.m_state = State_NewIo;
        return;

    case State_NewIo: {
        if (m_request != 0) {
            return;
        }
        gfCollectionIO* io = new (m_heap2) gfCollectionIO(m_heap2);
        if (m_io) {
            gfTask* task = m_io;
            task->exit();
        }
        m_io = io;
        m_drives[0].unk0 = m_flag7;
        m_drives[0].unk1 = m_flag7;
        if (!m_flag7) {
            m_drives[0].m_count = 0;
        }
        CO_RETURN();
    }

    // (sd): the names of the files of the drive into m_drives[sd]
    case State_ListFiles: {
        bool ok;
        if (CO_FR(FrIo)->sd) {
            ok = m_io->requestListSD(m_fileType);
        } else {
            ok = m_io->requestListVF(m_fileType);
        }
        if (ok) {
            m_ioPending = true;
            CO_CALL(FrIo, State_WaitIo, State_ListFilesDone, {-1, CO_FR(FrIo)->sd, 0});
        } else {
            CO_RETURN();
        }
        FileInfo* drive = &m_drives[CO_FR(FrIo)->sd];
        drive->m_count = 0;
        if (m_io->m_result != 0) {
            CO_RETURN();
        }
        char* names = reinterpret_cast<char*>(m_io->unkE4);
        muRecorder::Data* slot = &drive->m_names;
        slot->reset(names);
        if (!names) {
            CO_RETURN();
        }
        drive->m_count = m_io->unkE8;
        CO_RETURN();
    }

    // (listFiles, reportMissing): scan the console
    case State_ScanVf:
        CO_CALL0(State_ResetIo, State_ScanVfList);
        if (m_flag7 && CO_FR(FrScan)->listFiles) {
            CO_CALL(FrIo, State_ListFiles, State_ScanVfRet, {-1, 0, 0});
        }
        CO_RETURN();

    // (listFiles, reportMissing): scan the SD card
    case State_ScanSd:
        if (!gfFileIO::checkSDInserted()) {
            if (CO_FR(FrScan)->reportMissing) {
                CO_OPEN_WINDOW(0x36);
                CO_CALL0(State_WaitWnd, State_ScanSdWndRet);
            }
        } else {
            if (CO_FR(FrScan)->listFiles) {
                CO_CALL(FrIo, State_ListFiles, State_ScanSdListed, {-1, 1, 0});
                if (gfFileIO::checkSDInserted() && m_drives[1].m_count == 0 && CO_FR(FrScan)->reportMissing) {
                    CO_OPEN_WINDOW(0x39);
                    CO_CALL0(State_WaitWnd, State_ScanSdWndRet2);
                }
            }
        }
        CO_RETURN();

    // (mode, saveFirst): a window about the free blocks of the drive
    case State_MediaWnd: {
        const MuCollMedia* media = &lbl_80423690;
        if (CO_FR(FrMedia)->mode == 0) {
            if (media->vfFree <= 1) {
                CO_OPEN_WINDOW(CO_FR(FrMedia)->saveFirst ? 0x34 : 0x35);
                CO_CALL0(State_WaitWnd, State_MediaWndRet);
            }
        } else {
            if (gfFileIO::checkSDInserted() && media->sdState == 0 && media->sdFree <= 1) {
                CO_OPEN_WINDOW(CO_FR(FrMedia)->saveFirst ? 0x3F : 0x40);
                CO_CALL0(State_WaitWnd, State_MediaWndRet2);
            }
        }
        CO_RETURN();
    }

    // (drive): count the files; result 4 when there are 1000 or more (with a window), else 0
    case State_ListCount: {
        CO_FR(FrCount)->result = 2;
        bool ok;
        if (CO_FR(FrCount)->drive) {
            ok = m_io->requestListCountSD(m_fileType);
        } else {
            ok = m_io->requestListCountVF(m_fileType);
        }
        if (ok) {
            m_ioPending = true;
            CO_CALL(FrIo, State_WaitIo, State_ListCountDone, {-1, CO_FR(FrCount)->drive, 0});
        } else {
            CO_RETURN();
        }
        if (m_io->m_result == 0 && (u32)m_io->unkE8 >= 1000) {
            countText.len = 0;
            muCollMakeLimitText(&countText, muCollGetText(m_msgData, 0x42), muCollGetText(m_msgData, 0x43));
            muCollOpenTextWindow(this, &countText);
            CO_CALL0(State_WaitWnd, State_ListCountFullRet);
            CO_FR(FrCount)->result = 4;
            CO_RETURN();
        } else {
            CO_FR(FrCount)->result = 0;
            CO_RETURN();
        }
    }

    // (sd, name): read the summary of the file
    case State_ReadSummary: {
        CO_FR(FrLoad)->result = 2;
        bool ok;
        if (CO_FR(FrLoad)->sd) {
            ok = m_io->requestSummarySD(m_fileType, CO_FR(FrLoad)->name);
        } else {
            ok = m_io->requestSummaryVF(m_fileType, CO_FR(FrLoad)->name);
        }
        if (ok) {
            m_ioPending = true;
            CO_CALL(FrIo, State_WaitIo, State_ReadSummaryDone, {-1, CO_FR(FrLoad)->sd, 0});
        } else {
            CO_RETURN();
        }
        m_summary.m_info.reset(0);
        m_summary.m_decoded.m_pixels.reset(0);
        m_summary.m_jpegInfo = 0;
        m_summary.m_size = 0;
        if (m_io->m_result != 0) {
            CO_RETURN();
        }
        bool match = false;
        switch (m_io->unkEC) {
        case 4:
            match = m_fileType == 1;
            break;
        case 5:
            match = m_fileType == 2;
            break;
        case 6:
            match = m_fileType == 3;
            break;
        }
        if (!match) {
            CO_RETURN();
        }
        m_summary.m_info.reset(reinterpret_cast<gfCollSummaryCommon*>(m_io->unkE4));
        m_summary.m_size = m_io->unkE8;
        switch (m_io->unkEC) {
        case 4:
            // HYPOTHESIS: the picture of the summary of a snapshot (the summary object has it at +0x54)
            m_summary.m_jpegInfo = reinterpret_cast<u8*>(m_summary.m_info.get()) + 0x54;
            break;
        case 6:
            // HYPOTHESIS: the same inside the summary of a replay (+0x78)
            m_summary.m_jpegInfo = reinterpret_cast<u8*>(m_summary.m_info.get()) + 0x78;
            break;
        }
        if (m_summary.m_jpegInfo) {
            // MATCH-ONLY: an unused Decoded (its pixels are zero before the call and freed after it) is part of the original
            Summary::Decoded unusedDecoded;
            m_summary.m_decoded = m_summary.decodeJpeg(m_heap2, m_fileType == 1 ? 4 : 1);
        } else {
            m_summary.m_decoded.m_pixels.reset(0);
        }
        CO_FR(FrLoad)->result = 0;
        CO_RETURN();
    }

    // (overwrite, sd, name, data, size): make a new file or overwrite the one that is named
    case State_NewFile:
        CO_FR(FrNew)->result = 2;
        if (m_drives[CO_FR(FrNew)->sd].m_free == 0) {
            CO_RETURN();
        }
        if (!CO_FR(FrNew)->overwrite) {
            // count the files first (a window when there are too many)
            CO_CALL(FrCount, State_ListCount, State_NewFileCount, {-1, CO_FR(FrNew)->sd, 0});
            if (CO_CALLEE(FrCount)->result != 0) {
                CO_FR(FrNew)->result = CO_CALLEE(FrCount)->result;
                CO_RETURN();
            }
        } else {
            // overwrite only when there are 1000 files or more
            bool ok;
            if (CO_FR(FrNew)->sd) {
                ok = m_io->requestListCountSD(m_fileType);
            } else {
                ok = m_io->requestListCountVF(m_fileType);
            }
            if (ok) {
                m_ioPending = true;
                CO_CALL(FrIo, State_WaitIo, State_NewFileName, {-1, CO_FR(FrNew)->sd, 0});
            } else {
                CO_RETURN();
            }
            if (m_io->m_result != 0 || (u32)m_io->unkE8 < 1000) {
                CO_FR(FrNew)->overwrite = false;
            }
        }
        if (!CO_FR(FrNew)->overwrite) {
            // the io task makes the name
            bool ok;
            if (CO_FR(FrNew)->sd) {
                ok = m_io->requestNewFileNameSD(m_fileType);
            } else {
                ok = m_io->requestNewFileNameVF(m_fileType);
            }
            if (ok) {
                m_ioPending = true;
                CO_CALL(FrIo, State_WaitIo, State_NewFileList, {-1, CO_FR(FrNew)->sd, 0});
            } else {
                CO_RETURN();
            }
            gfCollectionIO* io = m_io;
            if (io->m_result != 0) {
                CO_FR(FrNew)->result = 3;
                CO_RETURN();
            }
            m_fileName = FileHandle(io->m_fileName);
            CO_FR(FrNew)->name = m_io->m_fileName;
        } else {
            // the file that is overwritten has to exist
            bool ok;
            if (CO_FR(FrNew)->sd) {
                ok = m_io->requestListSD(m_fileType);
            } else {
                ok = m_io->requestListVF(m_fileType);
            }
            if (ok) {
                m_ioPending = true;
                CO_CALL(FrIo, State_WaitIo, State_NewFileCheck, {-1, CO_FR(FrNew)->sd, 0});
            } else {
                CO_RETURN();
            }
            bool found = false;
            gfCollectionIO* io = m_io;
            if (io->m_result == 0) {
                char* names = reinterpret_cast<char*>(io->unkE4);
                char* name = CO_FR(FrNew)->name;
                u32 count = io->unkE8;
                char* p = names;
                for (u32 i = 0; i < count; i++) {
                    if (strcmp(p, name) == 0) {
                        found = true;
                        break;
                    }
                    p += strlen(p) + 1;
                }
                if (names) {
                    gfHeapManager::free(names);
                }
            }
            if (!found) {
                CO_OPEN_WINDOW(0x41);
                CO_CALL0(State_WaitWnd, State_NewFileSave);
                CO_FR(FrNew)->result = 5;
                CO_RETURN();
            }
        }
        {
            FrNew* fr;
            fr = CO_FR(FrNew);
            bool ok;
            if (fr->sd) {
                ok = m_io->requestSaveSD(m_fileType, fr->name, fr->data, fr->size, false);
            } else {
                ok = m_io->requestSaveVF(m_fileType, fr->name, fr->data, fr->size, false);
            }
            if (ok) {
                m_ioPending = true;
                CO_CALL(FrIo, State_WaitIo, State_NewFileSaved, {-1, fr->sd, 0});
            } else {
                CO_RETURN();
            }
            if (m_io->m_result == 0) {
                CO_FR(FrNew)->result = CO_FR(FrNew)->overwrite != 0;
            }
            CO_RETURN();
        }

    // (overwrite, name, data, size): write the data to the console
    case State_Write:
        CO_FR(FrWrite)->result = 2;
    writeReset:
        CO_CALL0(State_ResetIo, State_WriteIo);
        if (!m_flag7) {
            CO_RETURN();
        }
        if (m_flag6) {
            goto writeReset;
        }
        CO_CALL(FrNew, State_NewFile, State_WriteReport,
                {-1, CO_FR(FrWrite)->overwrite, 0, CO_FR(FrWrite)->name, CO_FR(FrWrite)->data, CO_FR(FrWrite)->size,
                 CO_FR(FrWrite)->result});
        CO_FR(FrWrite)->result = CO_CALLEE(FrNew)->result;
        if (CO_CALLEE(FrNew)->result == 2) {
            // not enough free blocks: say how many are needed
            writeText.len = 0;
            muCollMakeText(&writeText, muCollGetText(m_msgData, 0x32), (CO_FR(FrWrite)->size + 0x1FFFF) >> 17,
                           muCollGetText(m_msgData, 0x33));
            muCollOpenTextWindow(this, &writeText);
            CO_CALL0(State_WaitWnd, State_WriteReportRet);
        } else {
            CO_CALL(FrMedia, State_MediaWnd, State_WriteReportRet2, {-1, 0, true});
        }
        CO_RETURN();

    // (name): erase the file of the console
    case State_Erase:
        CO_FR(FrDel)->result = 2;
    eraseReset:
        CO_CALL0(State_ResetIo, State_EraseIo);
        if (!m_flag7) {
            CO_RETURN();
        }
        if (m_flag6) {
            goto eraseReset;
        }
        if (m_io->requestDeleteVF(m_fileType, CO_FR(FrDel)->name)) {
            m_ioPending = true;
            CO_CALL(FrIo, State_WaitIo, State_EraseDone, {-1, 0, 0});
        } else {
            CO_RETURN();
        }
        if (m_io->m_result == 0) {
            CO_FR(FrDel)->result = 0;
            CO_CALL(FrMedia, State_MediaWnd, State_EraseRet, {-1, 0, false});
        }
        CO_RETURN();

    // (checkSpace, blocks): the SD card works (ok) or a window says what is wrong with it
    case State_ReportMedia: {
        FrReport* fr = CO_FR(FrReport);
        fr->ok = false;
        const MuCollMedia* media = &lbl_80423690;
        u32 blocks = fr->blocks;
        if (media->sdState == 1) {
            CO_OPEN_WINDOW(0x36);
            CO_CALL0(State_WaitWnd, State_ReportMediaRet);
            CO_RETURN();
        } else if (media->sdState == 3) {
            CO_OPEN_WINDOW(0x37);
            CO_CALL0(State_WaitWnd, State_ReportMediaRet2);
            CO_RETURN();
        } else if (CO_FR(FrReport)->checkSpace) {
            if (media->sdState == 2) {
                CO_OPEN_WINDOW(0x38);
                CO_CALL0(State_WaitWnd, State_ReportMediaNoSpace);
                CO_RETURN();
            } else if (media->sdFree < blocks) {
                reportText.len = 0;
                muCollAddText(&reportText, muCollGetText(m_msgData, 0x50));
                muCollOpenTextWindow(this, &reportText);
                CO_CALL0(State_WaitWnd, State_ReportMediaRet3);
                CO_RETURN();
            }
        }
        CO_FR(FrReport)->ok = true;
        CO_RETURN();
    }

    // (sd, name): load the file into m_loadData
    case State_LoadFile: {
        CO_FR(FrLoad)->result = 2;
        FrLoad* fr;
        fr = CO_FR(FrLoad);
        bool ok;
        if (fr->sd) {
            ok = m_io->requestLoadSD(m_fileType, fr->name);
        } else {
            ok = m_io->requestLoadVF(m_fileType, fr->name);
        }
        if (ok) {
            m_ioPending = true;
            CO_CALL(FrIo, State_WaitIo, State_LoadFileDone, {-1, fr->sd, 0});
        } else {
            CO_RETURN();
        }
        if (m_io->m_result != 0) {
            m_loadData.reset(0);
            m_loadSize = 0;
            CO_RETURN();
        }
        m_loadData.reset(reinterpret_cast<void*>(m_io->unkE4));
        m_loadSize = m_io->unkE8;
        CO_FR(FrLoad)->result = 0;
        CO_RETURN();
    }

    // (sd, name): read the file
    case State_Read:
        CO_FR(FrLoad)->result = 2;
        if (CO_FR(FrLoad)->sd == 1) {
            CO_CALL(FrReport, State_ReportMedia, State_ReadIo, {-1, false, 0, false});
            if (!CO_CALLEE(FrReport)->ok) {
                CO_RETURN();
            }
        }
        CO_CALL(FrLoad, State_LoadFile, State_ReadRet, {-1, CO_FR(FrLoad)->sd, CO_FR(FrLoad)->name, 0});
        CO_FR(FrLoad)->result = CO_CALLEE(FrLoad)->result;
        CO_RETURN();

    // (overwrite, name, data, size): write the data to the SD card
    case State_Copy:
        CO_FR(FrWrite)->result = 2;
        CO_CALL(FrReport, State_ReportMedia, State_CopyNewFile,
                {-1, true, (CO_FR(FrWrite)->size + 0x1FFFF) >> 17, false});
        if (!CO_CALLEE(FrReport)->ok) {
            CO_RETURN();
        }
        CO_CALL(FrNew, State_NewFile, State_CopyReport,
                {-1, CO_FR(FrWrite)->overwrite, 1, CO_FR(FrWrite)->name, CO_FR(FrWrite)->data, CO_FR(FrWrite)->size,
                 CO_FR(FrWrite)->result});
        CO_FR(FrWrite)->result = CO_CALLEE(FrNew)->result;
        if (CO_CALLEE(FrNew)->result - 2 <= 1u) {
            CO_OPEN_WINDOW(0x3D);
            CO_CALL0(State_WaitWnd, State_CopyReportRet);
        } else {
            CO_CALL(FrMedia, State_MediaWnd, State_CopyReportRet2, {-1, 1, true});
        }
        CO_RETURN();

    // (name): erase the file of the SD card
    case State_EraseSd: {
        CO_FR(FrDel)->result = 2;
        CO_CALL(FrReport, State_ReportMedia, State_EraseSdIo, {-1, true, 0, false});
        if (!CO_CALLEE(FrReport)->ok) {
            CO_RETURN();
        }
        if (m_io->requestDeleteSD(m_fileType, CO_FR(FrDel)->name)) {
            m_ioPending = true;
            CO_CALL(FrIo, State_WaitIo, State_EraseSdReport, {-1, 1, 0});
        } else {
            CO_RETURN();
        }
        if (m_io->m_result != 0) {
            CO_OPEN_WINDOW(0x3E);
            CO_CALL0(State_WaitWnd, State_EraseSdReportRet);
        } else {
            CO_CALL(FrMedia, State_MediaWnd, State_EraseSdReportRet2, {-1, 1, false});
            CO_FR(FrDel)->result = 0;
        }
        CO_RETURN();
    }

    // (sd, name): load a file and keep it for copy / move
    case State_LoadForCopy: {
        CO_FR(FrLoad)->result = 2;
        CO_CALL(FrLoad, State_LoadFile, State_LoadForCopyDone, {-1, CO_FR(FrLoad)->sd, CO_FR(FrLoad)->name, 0});
        if (CO_CALLEE(FrLoad)->result != 0) {
            CO_RETURN();
        }
        nteSection section;
        fn_80153488(&section);
        if (fn_80153504(&section, m_loadData.unk0)) {
            section.setOwnState(OSGetTime(), 0);
        }
        fn_80153498(&section, -1);
        if (CO_FR(FrLoad)->sd == 0) {
            CO_CALL(FrWrite, State_Copy, State_LoadForCopyRet,
                    {-1, false, 0, m_loadData.unk0, m_loadSize, 0});
            CO_FR(FrLoad)->result = CO_CALLEE(FrWrite)->result;
        } else if (CO_FR(FrLoad)->sd == 1) {
            CO_CALL(FrWrite, State_Write, State_LoadForCopyRet2,
                    {-1, false, 0, m_loadData.unk0, m_loadSize, 0});
            CO_FR(FrLoad)->result = CO_CALLEE(FrWrite)->result;
        }
        CO_RETURN();
    }

    // (sd, name): copy the file from the drive to the other one
    case State_ReadAll:
        CO_FR(FrLoad)->result = 2;
        if (CO_FR(FrLoad)->sd == 1) {
            CO_CALL(FrReport, State_ReportMedia, State_ReadAllLoaded, {-1, false, 0, false});
            if (!CO_CALLEE(FrReport)->ok) {
                CO_RETURN();
            }
        }
        CO_CALL(FrLoad, State_LoadForCopy, State_ReadAllRet, {-1, CO_FR(FrLoad)->sd, CO_FR(FrLoad)->name, 0});
        CO_FR(FrLoad)->result = CO_CALLEE(FrLoad)->result;
        CO_RETURN();

    // (sd, name): copy the file and erase the original
    case State_MoveAll:
        CO_FR(FrLoad)->result = 2;
        if (CO_FR(FrLoad)->sd == 1) {
            CO_CALL(FrReport, State_ReportMedia, State_MoveAllLoaded, {-1, true, 0, false});
            if (!CO_CALLEE(FrReport)->ok) {
                CO_RETURN();
            }
        }
        CO_CALL(FrLoad, State_LoadForCopy, State_MoveAllCopied, {-1, CO_FR(FrLoad)->sd, CO_FR(FrLoad)->name, 0});
        if (CO_CALLEE(FrLoad)->result != 0) {
            CO_RETURN();
        }
        if (CO_FR(FrLoad)->sd == 0) {
            CO_CALL(FrDel, State_Erase, State_MoveAllErased, {-1, CO_FR(FrLoad)->name, 0});
            CO_FR(FrLoad)->result = CO_CALLEE(FrDel)->result;
        } else if (CO_FR(FrLoad)->sd == 1) {
            CO_CALL(FrDel, State_EraseSd, State_MoveAllRet, {-1, CO_FR(FrLoad)->name, 0});
            CO_FR(FrLoad)->result = CO_CALLEE(FrDel)->result;
        }
        CO_RETURN();

    // the loop: wait for a request (m_busy)
    start:
    case State_Start:
        m_script.m_state = State_Idle;
        return;

    case State_Idle:
        if (m_busy == Busy_Locked) {
            return;
        }
        if (m_busy == Busy_Scan) {
            if (m_scanDrive0) {
                CO_CALL(FrScan, State_ScanVf, State_ScanVfRet2, {-1, m_scanFlagA, m_scanFlagB});
            }
            if (m_scanDrive1) {
                CO_CALL(FrScan, State_ScanSd, State_ScanSdRet2, {-1, m_scanFlagA, m_scanFlagB});
            }
        } else if (m_busy == Busy_CheckLimit) {
            m_result = 0;
            if (m_reqDrive == 1) {
                CO_CALL(FrReport, State_ReportMedia, State_CheckLimitDone, {-1, true, 1, false});
                if (!CO_CALLEE(FrReport)->ok) {
                    m_result = 2;
                }
            }
            if (m_result == 0) {
                CO_CALL(FrCount, State_ListCount, State_CheckLimitRet, {-1, m_reqDrive, 0});
                m_result = CO_CALLEE(FrCount)->result;
            }
        } else if (m_busy == Busy_Write) {
            if (m_writeDrive == 0) {
                CO_CALL(FrWrite, State_Write, State_WriteNewRet, {-1, m_writeNamed, m_writeName, m_writeData, m_writeSize, 0});
                m_result = CO_CALLEE(FrWrite)->result;
            } else {
                CO_CALL(FrWrite, State_Copy, State_WriteRet, {-1, m_writeNamed, m_writeName, m_writeData, m_writeSize, 0});
                m_result = CO_CALLEE(FrWrite)->result;
            }
        } else if (m_busy == Busy_Read) {
            CO_CALL(FrLoad, State_Read, State_ReadDone, {-1, m_reqDrive, m_reqName, 0});
            m_result = CO_CALLEE(FrLoad)->result;
        } else if (m_busy == Busy_Erase) {
            if (m_reqDrive == 0) {
                CO_CALL(FrDel, State_Erase, State_EraseVfRet, {-1, m_reqName, 0});
                m_result = CO_CALLEE(FrDel)->result;
            } else {
                CO_CALL(FrDel, State_EraseSd, State_EraseSdRet3, {-1, m_reqName, 0});
                m_result = CO_CALLEE(FrDel)->result;
            }
        } else if (m_busy == Busy_Copy) {
            CO_CALL(FrLoad, State_ReadAll, State_CopyDone, {-1, m_reqDrive, m_reqName, 0});
            m_result = CO_CALLEE(FrLoad)->result;
        } else if (m_busy == Busy_Move) {
            CO_CALL(FrLoad, State_MoveAll, State_MoveDone, {-1, m_reqDrive, m_reqName, 0});
            m_result = CO_CALLEE(FrLoad)->result;
        } else if (m_busy == Busy_ReadSummary) {
            CO_CALL(FrLoad, State_ReadSummary, State_ReadSummaryRet, {-1, m_reqDrive, m_reqName, 0});
            m_result = CO_CALLEE(FrLoad)->result;
        }
        m_busy = Busy_None;
        goto start;
    }
}

// HYPOTHESIS: the jpeg decoder (stand-ins; the context is the stack of the decoder).
struct muJpegSize {
    u32 m_width;
    u32 m_height;
    u32 m_size;
};
struct muJpegInfo {
    u32 unk0;
    u32 m_jpegSize;     // HYPOTHESIS
    u32 m_jpegData;     // HYPOTHESIS
};
extern "C" void fn_80260730(void* context, u32 jpegData, u32 jpegSize, int format, int unk, int unk2, muJpegSize* out);
extern "C" void fn_802608BC(void* context, u32 jpegData, u32 jpegSize, void* pixels, u32 size, int format, int unk,
                            int unk2);

void* muCollection::lock() {
    m_busy = 1;
    return m_io;
}

// HYPOTHESIS: processDefault takes the three words as (drive, data, size); see the note at muCollection::m_argFlag.
void muCollection::write(void* data, int size, int unk) {
    m_argFlag = 0;
    m_argData = data;
    m_writeName = 0;
    m_argSize = size;
    m_argUnk = unk;
    m_busy = 4;
}

void muCollection::overwriteIf1000(FileKey key, int size, int unk) {
    s32 id = key.m_drive;
    m_keyName.m_name[0] = 0;
    strcat(m_keyName.m_name, key.m_name);
    m_argFlag = 1;
    m_argId = id;
    m_writeName = m_keyName.m_name;
    m_argSize = size;
    m_argUnk = unk;
    m_busy = 4;
}

void muCollection::read(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        m_readName = name;
        m_argDrive = drive;
        m_argData = m_readName.m_name;
        m_busy = 5;
    }
}

void muCollection::copy(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        m_readName = name;
        m_argDrive = drive;
        m_argData = m_readName.m_name;
        m_busy = 7;
    }
}

void muCollection::erase(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        FileName fileName;
        fileName.m_name[0] = 0;
        strcat(fileName.m_name, name.m_name);
        m_readName = fileName;
        m_argDrive = drive;
        m_argData = m_readName.m_name;
        m_busy = 6;
    }
}

void muCollection::erase(const FileKey& key) {
    s32 id = key.m_drive;
    FileName fileName;
    fileName.m_name[0] = 0;
    strcat(fileName.m_name, key.m_name);
    m_readName = fileName;
    m_argDrive = id;
    m_argData = m_readName.m_name;
    m_busy = 6;
}

void muCollection::move(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        m_readName = name;
        m_argDrive = drive;
        m_argData = m_readName.m_name;
        m_busy = 8;
    }
}

void muCollection::readSummary(int drive, FileName name) {
    bool differs = strcmp(name.m_name, lbl_8059E14C) != 0;
    if (differs) {
        m_summaryName = name;
        m_argDrive = drive;
        m_argData = m_summaryName.m_name;
        m_busy = 9;
    }
}

void muCollection::checkLimit(int drive) {
    m_argDrive = drive;
    m_busy = 3;
}

void muCollection::scanAll(int unk, int unk2) {
    m_scanDrive0 = 1;
    m_scanDrive1 = 1;
    m_scanFlagA = unk;
    m_scanFlagB = unk2;
    m_busy = 2;
}

void muCollection::scan(int drive, int unk, int unk2) {
    m_scanDrive0 = drive == 0;
    m_scanDrive1 = drive == 1;
    m_scanFlagA = unk;
    m_scanFlagB = unk2;
    m_busy = 2;
}

void muCollection::unlock() {
    m_busy = 0;
}

muCollection::Summary::Decoded muCollection::Summary::decodeJpeg(Heaps::HeapType heap, int unk) const {
    const muJpegInfo* info = static_cast<const muJpegInfo*>(m_jpegInfo);
    u32 jpegData = info->m_jpegData;
    u32 jpegSize = info->m_jpegSize;
    u8 context[0x1C00];
    muJpegSize jpeg;
    fn_80260730(context, jpegData, jpegSize, 6, unk, 0, &jpeg);
    Decoded result;
    u32 size = jpeg.m_size;
    result.m_pixels.reset(gfHeapManager::alloc(heap, size, 0x20));
    result.m_width = jpeg.m_width;
    result.m_height = jpeg.m_height;
    DCInvalidateRange(result.m_pixels.unk0, size);
    fn_802608BC(context, jpegData, jpegSize, result.m_pixels.unk0, size, 6, unk, 0);
    DCStoreRange(result.m_pixels.unk0, size);
    GXInvalidateTexAll();
    return result;
}
