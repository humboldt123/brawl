#pragma once

// HYPOTHESIS: layout from the constructor, initProc, the destructor and the step functions; names from the map
// (if_snap_save.o). IfSnapSaveTask is the task that shoots, encodes and saves a snapshot or a replay.

#include <StaticAssert.h>
#include <revolution/OS/OSTime.h>
#include <gf/gf_task.h>
#include <sr/sr_common.h>
#include <types.h>

class MuObject;
class MuMsg;
class gfJpegEncodeWork;
namespace nw4r { namespace g3d { class ResFile; class ScnGroup; } }

// HYPOTHESIS: one storage drive of the collection task (0x14 bytes, two of them at 0x358).
struct muCollectionDrive {
    u8 unk0;
    u8 unk1;       // 0x01 HYPOTHESIS: the drive can be written
    u8 unk2[2];
    s32 m_state;   // 0x04
    s32 unk8;
    u32 m_free;    // 0x0C HYPOTHESIS: free blocks
    u8 unk10[4];
};
static_assert(sizeof(muCollectionDrive) == 0x14, "Class is the wrong size!");

// HYPOTHESIS: view of the collection (album) save task (mu_collection.o); only the members read here.
class muCollection : public gfTask {
public:
    u8 unk40[0x310];
    s32 m_busy;                    // 0x350 HYPOTHESIS: non-zero while a request runs
    u8 unk354[0x4];
    muCollectionDrive m_drives[2]; // 0x358
    u8 unk380[0x14];
};
static_assert(sizeof(muCollection) == 0x394, "Class is the wrong size!");

// HYPOTHESIS: the photo data object (nt_etc_file.o) has a virtual destructor after eight bytes of data.
class nteFileDataPhotoBase {
public:
    u8 unk0[0x8];
};
class nteFileDataPhoto : public nteFileDataPhotoBase {
public:
    virtual ~nteFileDataPhoto();
    u8 unkC[0x8];
};

// HYPOTHESIS: the save section (nt_etc_file.o); its constructor, destructor and getSize are still fn_.
struct nteSection {
    u8 unk0[0x8];
};

struct IfSnapSaveMedia {
    s32 m_status; // 0x00 HYPOTHESIS: 1 ready, 2..5 error messages
    u32 m_free;   // 0x04 HYPOTHESIS: free blocks
};

class IfSnapSaveTask : public gfTask {
public:
    // HYPOTHESIS: the step work word is a counter in most steps and a flag in the confirm step.
    union StepWork {
        s32 m_count;
        bool m_flag;
    };

    nw4r::g3d::ScnGroup* m_scnGroup;  // 0x40
    MuObject* m_objects[6];           // 0x44
    nw4r::g3d::ScnGroup* m_expand;    // 0x5C HYPOTHESIS: a ScnMdlExpand on the frame model
    s32 m_step;                       // 0x60
    StepWork m_stepWork;              // 0x64
    s32 unk68;                        // 0x68
    s32 unk6C;                        // 0x6C
    s32 m_cursorState;                // 0x70
    IfSnapSaveMedia m_media[2];       // 0x74
    void* m_captureBuffer;            // 0x84
    void* m_jpegBuffer;               // 0x88
    MuMsg* m_msg;                     // 0x8C
    s32 m_result;                     // 0x90
    s32 unk94;                        // 0x94
    s32 unk98;                        // 0x98
    nteSection m_section;             // 0x9C
    s32 unkA4;                        // 0xA4
    s32 unkA8;                        // 0xA8
    muCollection* m_collection;       // 0xAC
    bool m_scanEnable;                // 0xB0
    s32 m_scanWait;                   // 0xB4
    s32 m_scanState;                  // 0xB8
    nteFileDataPhoto* m_photoData;    // 0xBC
    s32 unkC0;                        // 0xC0
    gfJpegEncodeWork* m_jpegWork;     // 0xC4
    s64 unkC8;                        // 0xC8
    s32 m_kind;                       // 0xD0
    Heaps::HeapType m_dataHeap;       // 0xD4
    u8 unkD8;                         // 0xD8
    bool m_scanned;                   // 0xD9
    u8 unkDA[0x6];

    IfSnapSaveTask();
    // HYPOTHESIS: parameter types from the callers in if_photo.o and if_adv_pause.o.
    static IfSnapSaveTask* create(nw4r::g3d::ResFile* resFile, nw4r::g3d::ResFile* resFile2, void* msgData,
                                  nw4r::g3d::ScnGroup* scnGroup, int drawPriority, Heaps::HeapType heap,
                                  Heaps::HeapType heap2, Heaps::HeapType dataHeap);
    void initProc(int kind, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResFile* resFile2, void* msgData,
                  nw4r::g3d::ScnGroup* scnGroup, int drawPriority, Heaps::HeapType heap, Heaps::HeapType heap2,
                  Heaps::HeapType dataHeap, bool capture);
    virtual ~IfSnapSaveTask();
    // BYTES: never_inline keeps the native out-of-line calls.
    void createModel(nw4r::g3d::ResFile* resFile, int drawPriority, Heaps::HeapType heap, Heaps::HeapType heap2)
        __attribute__((never_inline));
    void initMsg(void* msgData, Heaps::HeapType heap, Heaps::HeapType heap2);
    void destroyModel();
    void updateMediaFreeState();
    void mediaCursorStateInit(int state) __attribute__((never_inline));
    void mediaCursorStateMain() __attribute__((never_inline));
    virtual void processDefault();
    void scanMediaMain();
    void printMediaStatus(u32 msgIndex, u32 lineIndex, s32 status, u32 freeSize);

    // HYPOTHESIS: the step functions are static members that take the task (the step table holds plain function
    // pointers and processDefault passes the task as the only argument); a main function returns the next step.
    static void photoStepNoneInit(IfSnapSaveTask* task);
    static void photoStepShootInit(IfSnapSaveTask* task);
    static s32 photoStepShootMain(IfSnapSaveTask* task);
    static void photoStepCaptureInit(IfSnapSaveTask* task);
    static s32 photoStepCaptureMain(IfSnapSaveTask* task);
    static void photoStepFadeInit(IfSnapSaveTask* task);
    static s32 photoStepFadeMain(IfSnapSaveTask* task);
    static void photoStepMediaScanWaitInit(IfSnapSaveTask* task);
    static s32 photoStepMediaScanWaitMain(IfSnapSaveTask* task);
    static void photoStepSelectMediaInit(IfSnapSaveTask* task);
    static s32 photoStepSelectMediaMain(IfSnapSaveTask* task);
    static void photoStepErrorInit(IfSnapSaveTask* task);
    static s32 photoStepErrorMain(IfSnapSaveTask* task);
    static void photoStepConfirmStartInit(IfSnapSaveTask* task);
    static s32 photoStepConfirmStartMain(IfSnapSaveTask* task);
    static void photoStepConfirmInit(IfSnapSaveTask* task);
    static s32 photoStepConfirmMain(IfSnapSaveTask* task);
    static void photoStepSavingInit(IfSnapSaveTask* task);
    static s32 photoStepSavingMain(IfSnapSaveTask* task);
    static void photoStepDoneInit(IfSnapSaveTask* task);
    static s32 photoStepDoneMain(IfSnapSaveTask* task);
    static void photoStepWaitMediaInit(IfSnapSaveTask* task);
    static s32 photoStepWaitMediaMain(IfSnapSaveTask* task);
};
static_assert(sizeof(IfSnapSaveTask) == 0xE0, "Class is the wrong size!");
