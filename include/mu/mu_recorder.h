#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <sr/sr_common.h>
#include <std/auto_ptr.h>
#include <types.h>
#include <mu/mu_notice_wnd.h>

class MuObject;
class muNoticeWndTask;
class gfSysRecorder2;

// HYPOTHESIS: the state of the processDefault of muRecorder and muCollection. Both tasks run a coroutine: the state is
// the __LINE__ of the source line that the function goes on at (every value below is such a line number, so the original
// source has the recorder at about lines 321-597 and the collection at about 992-1910 of ONE file), and the stack holds
// what a "call" needs: the locals frame of the callee (12 to 32 bytes, copied from a template in .rodata) followed by
// the state to go on with when the callee "returns". The stack grows up, m_sp is the first free word.
template <int N>
struct muScriptState {
    int m_state;                    // 0x00
    int m_stack[N];                 // 0x04
    int* m_sp;                      // 0x04 + N * 4

    muScriptState() : m_state(0), m_sp(m_stack) {}
};
static_assert(sizeof(muScriptState<32>) == 0x88, "Class is wrong size!");
static_assert(sizeof(muScriptState<64>) == 0x108, "Class is wrong size!");

// HYPOTHESIS: the window that a task has open; a new window ends the old one (exit), and so does the destructor.
class muNoticeWndPtr {
public:
    muNoticeWndPtr() : m_task(0) {}
    ~muNoticeWndPtr() {
        if (m_task) m_task->exit();
        m_task = 0;
    }
    void reset(muNoticeWndTask* task) {
        if (m_task) m_task->exit();
        m_task = task;
    }
    muNoticeWndPtr& operator=(muNoticeWndTask* task) {
        reset(task);
        return *this;
    }
    muNoticeWndTask* get() const { return m_task; }
    muNoticeWndTask* operator->() const { return m_task; }
    operator bool() const { return m_task != 0; }

private:
    muNoticeWndTask* m_task;
};

// The first base class after gfTask: the notice window (muNoticeWndTask) that the task shows, and what it needs to open
// one. RTTI: "muRecorderBase" is a base of muRecorder at offset 0x40 and has no vtable; its constructor is inlined into
// the constructor of muRecorder (stores 0x40..0x5C, then the vptr) and its destructor into the destructor.
// HYPOTHESIS: the names follow the arguments of muNoticeWndTask::create (fn_800BEEB8/fn_800BF048) that they are passed on as.
class muRecorderBase {
public:
    Heaps::HeapType m_heap;         // 0x40 (the heap of the windows: argument 8 of create; argument 1 of MuObject::create)
    Heaps::HeapType m_heap2;        // 0x44 (argument 9 of create; the heap of the sys recorder and of the collection io)
    muNoticeWndPtr m_wnd;           // 0x48 the window that is open (exit() when replaced; step 0x104 is waited for)
    nw4r::g3d::ResFile m_msgRes;    // 0x4C the resource file of the windows (passed as &m_msgRes)
    void* m_msgData;                // 0x50 the message data (passed to Message::getPrintIndexData and create)
    int m_drawPriority;             // 0x54 argument 6 of create (and drawPriority + 1 of the object of the task)
    u8 m_wndUnk0;                   // 0x58 argument 10 of create (unk0)
    u8 m_wndUnk2;                   // 0x59 argument 12 of create (unk2, 0xF0 for the collection)
    u8 m_pad5A[2];
    void* m_scnRoot;                // 0x5C the scene the window is added to

    // HYPOTHESIS: the constructor and destructor are inline (they are part of those of muRecorder).
    muRecorderBase(Heaps::HeapType heap, Heaps::HeapType heap2, nw4r::g3d::ResFile msgRes, void* msgData,
                   int drawPriority, u8 wndUnk0, u8 wndUnk2, void* scnRoot)
        : m_heap(heap), m_heap2(heap2), m_msgRes(msgRes), m_msgData(msgData), m_drawPriority(drawPriority),
          m_wndUnk0(wndUnk0), m_wndUnk2(wndUnk2), m_scnRoot(scnRoot) {}
};
static_assert(sizeof(muRecorderBase) == 0x20, "Class is wrong size!");

// The menu task that checks, renews, saves and loads the save data of the NAND for the adventure mode (module sora,
// vtable 0x804570C0). It is the base class of muCollection. HYPOTHESIS: the names follow processDefault and the callers.
class muRecorder : public gfTask, public muRecorderBase {
public:
    // The buffer that holds the loaded adventure data; the destructor gives it back with gfHeapManager::free
    // (inline in the destructors; muRecorder::Data::reset is the out of line function, 0x4C bytes).
    class Data {
    public:
        void* unk0;
        Data() : unk0(0) {}
        ~Data();
        void reset(void* data);
    };

    // What m_request asks for; processDefault does it, then sets the request back to 0.
    enum Request {
        Request_None = 0,
        Request_Check = 1,          // check(): check the save data of the system and renew it when it is needed
        Request_SaveAdv = 2,        // saveAdv(): save the adventure data of the slot m_slot
        Request_LoadAdv = 3,        // loadAdv(): load the adventure data of the slot m_slot into m_advData
    };

    // The states of processDefault (see muScriptState). Names are HYPOTHESIS, values are the lines of the source.
    // The ones that end in "Ret" only return to the caller. 0x1F9 is a second copy of 0x1E3 (identical code: the
    // compiler merges the two case bodies).
    enum State {
        State_Init = 0,
        State_Begin = 0x141,        // go to State_Idle
        State_WaitWnd = 0x146,      // subroutine: wait until m_wnd->m_step is 3
        State_WaitSysRec = 0x14E,   // subroutine: wait until the sys recorder is not busy
        State_Check = 0x158,        // subroutine: clear the flags, requestCheck(m_heap2, m_recKind)
        State_CheckDone = 0x164,    // result of the check: open (flag 7), ask to renew (msg 0x27/0x28 + 0x2C), or error
        State_RenewStart = 0x1A3,   // the answer of the window: renew (msg 0x26 window, requestRenew) or error
        State_RenewWait = 0x1AF,    // renew finished: close the window
        State_RenewDone = 0x1B2,    // result of the renew: detach the object, set the GameGlobal bit, result windows
        State_ErrorWnd = 0x1E3,     // notice window (msg 0x30), then State_ErrorWndRet
        State_ErrorWnd2 = 0x1F9,    // same code as 0x1E3
        State_ErrorWndRet = 0x201,
        State_SaveAdv = 0x205,      // subroutine(slot): saveAdventure(slot) on the buffer of the sys recorder
        State_SaveAdvDone = 0x216,
        State_LoadAdv = 0x220,      // subroutine(slot): loadAdventure(m_heap2, slot)
        State_LoadAdvDone = 0x22C,  // takes the buffer of the sys recorder for m_advData
        State_Idle = 0x240,         // wait for m_request
        State_CheckRet = 0x249,
        State_SaveAdvRet = 0x24F,
        State_LoadAdvRet = 0x255,
    };

    typedef muScriptState<32> Script;

    nw4r::g3d::ResFile m_modelRes;      // 0x60 the models of the task (argument 3)
    std::auto_ptr<MuObject> m_obj;      // 0x64 the object "InfCmnWait_TopN" of the wait window
    gfSysRecorder2* m_sysRecorder;      // 0x68 created for a request (createInstance), dropped when it is done
    bool m_sysRecorderUsed;             // 0x6C set when a request of the sys recorder is started
    u8 m_pad6D[3];
    int m_recKind;                      // 0x70 the kind of save data for the sys recorder and the bit of GameGlobal's
                                        //      enable switches (1 for kind 1, otherwise 3)
    int m_kind;                         // 0x74 argument 2 of the constructor: 1 for the adventure data, muCollection passes
                                        //      2..4 (its kinds 1..3, 0 otherwise); selects the messages
    Script m_script;                    // 0x78 state, 0x7C stack[32], 0xFC stack pointer

    union {
        int m_request;                  // 0x100 a Request; set by check/saveAdv/loadAdv, cleared when it is done
        int unk100;
    };
    union {
        int m_failed;                   // 0x104 set when the last save or load did not go through
        int unk104;
    };
    Data m_advData;                     // 0x108 the loaded adventure data, taken by getAdvData
    union {
        u32 m_slot;                     // 0x10C the adventure slot of saveAdv/loadAdv
        u32 unk10C;
        struct {
            // HYPOTHESIS: the flags are set by the check: usable (7) and (6); muCollection reads them. The slot is
            // small, so these bits (the top bits of the word) do not collide with it.
            bool m_flag7 : 1;
            bool m_flag6 : 1;
            u8 m_flagRest : 6;
            u8 m_padFlags[3];
        };
    };

    // HYPOTHESIS: argument types and order from the registers and from the call in muCollection::muCollection.
    // fn_800C1B3C = __ct__10muRecorderFiQ34nw4r3g3d7ResFileQ34nw4r3g3d7ResFilePvPviQ25Heaps8HeapTypeQ25Heaps8HeapTypeUcUcPCc
    muRecorder(int kind, nw4r::g3d::ResFile modelRes, nw4r::g3d::ResFile msgRes, void* msgData, void* scnRoot,
               int drawPriority, Heaps::HeapType heap, Heaps::HeapType heap2, u8 wndUnk0, u8 wndUnk2, const char* name);
    virtual ~muRecorder();              // fn_800C1D9C (vtable 0x804570C0; the destructor of std::auto_ptr<MuObject> is the weak 0x800C1D2C)
    virtual void processDefault();      // fn_800C1E68, 0xD20 bytes

    void check();                       // fn_800C2B88: m_request = Request_Check
    Data getAdvData();
    // HYPOTHESIS: the adventure slot is a word, narrowed by gfSysRecorder2.
    void saveAdv(u32 slot);
    void loadAdv(u32 slot);
};
static_assert(sizeof(muRecorder) == 0x110, "Class is wrong size!");
