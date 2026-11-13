// Havok translation unit hkAgent1nMachine.o (main.dol 0x802FC6AC-0x802FEDA0).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802FC6AC   376  hkAgent1nMachine_Destroy   [map: hkAgent1nMachine__hkAgent1nMachine_Destroy]
//   0x802FC824   356  hkAgent1nMachine_InvalidateTim   [map: hkAgent1nMachine__hkAgent1nMachine_InvalidateTim]
//   0x802FC988   396  hkAgent1nMachine_WarpTime   [map: hkAgent1nMachine__hkAgent1nMachine_WarpTime]
//   0x802FCB14   152  hkAgent1nMachine_Create   [map: hkAgent1nMachine__hkAgent1nMachine_Create]
//   0x802FCBAC   232  hkAgent1nMachine_initInputAtTime   [map: hkAgent3Input__hkAgent1nMachine_initInputAtTime]
//   0x802FCC94    92  hkAgent1nMachine_flipInput   [map: hkAgent3ProcessInput__hkAgent1nMachine_flipInput]
//   0x802FCCF0  4120  hkAgent1nMachine_Process   [map: hkAgent1nMachine__hkAgent1nMachine_Process]
//   0x802FDD08  2232  hkAgent1nMachine_Weld   [map: hkAgent1nMachine__hkAgent1nMachine_Weld]
//   0x802FE5C0   956  hkAgent1nMachine_VisitAllAgents   [map: hkAgent1nTrack__hkAgent1nMachine_VisitAllAgents]
//   0x802FE97C   216  hkAgent1nMachine_UpdateShapeCollectionFilterVisitor   [map: hkAgent1nMachine_VisitorInput__hkAgent1nMachine_UpdateShapeCollectionFilterVisitor]
//   0x802FEA54    12  hkAgent1nMachine_UpdateShapeCollectionFilter   [map: hkAgent1nMachine__hkAgent1nMachine_UpdateShapeCollectionFilter]
//   0x802FEA60   232  calc1nStatistics   [map: hkAgentMachineUtil__calc1nStatistics]
//   0x802FEB48   392  calcNnStatistics   [map: hkAgentMachineUtil__calcNnStatistics]
//   0x802FECD0   208  calcNnStatisticsContactMgrsOnly   [map: hkAgentMachineUtil__calcNnStatisticsContactMgrsOnly]

#include <havok/hkAgent1nTrack.h>

// Function body below is taken from https://github.com/jj-link/brawl (src/havok/fn_802FEA54.cpp).
// hkAgent1nMachine_UpdateShapeCollectionFilter = VisitAllAgents(track, input, UpdateShapeCollectionFilterVisitor)
extern "C" void fn_802FE5C0(void*, void*, void*);
extern "C" void fn_802FE97C(void);
// TODO: cleanup (shape from jj-link/brawl fn_802FEA54)
extern "C" void fn_802FEA54(void* a, void* b) {
    fn_802FE5C0(a, b, (void*)&fn_802FE97C);
}


// Shape from https://github.com/jj-link/brawl src/havok/fn_802FCC94.cpp.
// hkAgent1nMachine_flipInput (map name)
// TODO: cleanup (shape from jj-link/brawl fn_802FCC94)
extern "C" void fn_802FCC94(hkAgent3ProcessInput& in, hkAgent3ProcessInput& out) {
    out.m_bodyA = in.m_bodyB;
    out.m_bodyB = in.m_bodyA;
    out.m_input = in.m_input;
    out.m_contactMgr = in.m_contactMgr;
    out.m_linearTimInfo(0) = -in.m_linearTimInfo(0);
    out.m_linearTimInfo(1) = -in.m_linearTimInfo(1);
    out.m_linearTimInfo(2) = -in.m_linearTimInfo(2);
    out.m_linearTimInfo(3) = in.m_linearTimInfo(3);
    out.m_aTb.setInverse(in.m_aTb);
}

// fn_802FE97C: body from https://github.com/jj-link/brawl src/havok/fn_802FE97C.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802FE97C {
class Class_802FE97C_Vf {
public:
    virtual u32 vf0(void* a, void* b, void* c, void* d, void* e);
};

// TODO: cleanup (shape from jj-link/brawl fn_802FE97C)
extern "C" void* fn_802FE97C(u8* self, u8* rec, u32 arg2) {
    Class_802FE97C_Vf* sub = *(Class_802FE97C_Vf**)(*(u8**)(self + 0xC) + 4);
    u32 ret = sub->vf0(*(void**)(self + 0xC), *(void**)self, *(void**)(self + 4), *(void**)(self + 8), *(void**)(rec + 8));
    if ((s8)(ret >> 24) != 0 && rec[0] != 0) {
        return rec + rec[3];
    }
    if (rec[0] == 0) {
        return rec + rec[3];
    }
    u8* base = *(u8**)(*(u8**)(self + 0xC));
    void (*fp)(void*, u32, u32) = *(void (**)(void*, u32, u32))(base + rec[1] * 0x34 + 0x1698);
    fp(rec, arg2, *(u32*)(self + 0x10));
    return rec;
}
} // namespace jj_fn_802FE97C

// fn_802FEA60: body from https://github.com/jj-link/brawl src/havok/fn_802FEA60.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802FEA60 {
#pragma use_lmw_stmw on
extern "C" char lbl_80414180[];
class Object_vptr_0 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2(const char*, s32, void*, s32, s32, void*);
 virtual void slot3(const char*, s32, void*);
};
// TODO: cleanup (shape from jj-link/brawl fn_802FEA60)
extern "C" void fn_802FEA60(void* arg0, Object_vptr_0* arg1) {
 u8* entry;
 u8* end;
 s32 offset = 0;
 u8* cursor;
 s32 size;
 goto setup;
scan:
 switch (*cursor) {
 case 1: goto finish;
 case 6:
  arg1->slot3(lbl_80414180, 8, *(void**)(cursor + 4));
  cursor += cursor[3];
  break;
 case 0: case 2: case 3: case 4: case 5:
  cursor += cursor[3];
  break;
 }
 if (cursor < end) goto scan;
setup:
 entry = *(u8**)(*(u8**)arg0 + offset);
 size = *(s32*)entry;
 cursor = entry + 0x10;
 end = entry + size + 0x10;
 arg1->slot2(lbl_80414180 + 6, 8, entry, size + 0x10, 0x200, entry + size);
 offset += 4;
 goto scan;
finish:;
}
} // namespace jj_fn_802FEA60

// fn_802FC988: body from https://github.com/jj-link/brawl src/havok/fn_802FC988.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802FC988 {
extern "C" {
extern "C" const float lbl_805A4560;
}

typedef void (*EntryFunc)(void*, void*, void*, float, float);

// TODO: cleanup (shape from jj-link/brawl fn_802FC988)
extern "C" void fn_802FC988(void*** arg0, void** arg1, float farg1, float farg2) {
    float neg = lbl_805A4560;
    int idx = 4;
    unsigned char* blk = (unsigned char*)**arg0;
    unsigned char* entry = blk + 0x10;
    unsigned char* end = (unsigned char*)((int)blk + *(int*)blk + 0x10);

    for (;;) {
        unsigned char* e = entry;
        unsigned char* dat;
        EntryFunc fn;

        switch (e[0]) {
        case 2:
        case 3:
        case 6:
            dat = entry + 0x10;
            entry += e[3];
            fn = *(EntryFunc*)((char*)*arg1 + e[1] * 0x34 + 0x16b4);
            if (fn != 0) {
                fn(e, dat, arg1, farg1, farg2);
            }
            break;
        case 4:
        case 5:
            dat = entry + 0x20;
            if (*(float*)(e + 0xc) == farg1) {
                *(float*)(e + 0xc) = farg2;
            } else {
                *(float*)(e + 0xc) = neg;
            }
            entry += e[3];
            fn = *(EntryFunc*)((char*)*arg1 + e[1] * 0x34 + 0x16b4);
            if (fn != 0) {
                fn(e, dat, arg1, farg1, farg2);
            }
            break;
        case 1:
            goto done;
        case 0:
            entry += e[3];
            break;
        }

        if (entry >= end) {
            blk = (unsigned char*)*(void**)((char*)*arg0 + idx);
            idx += 4;
            entry = blk + 0x10;
            blk += *(int*)blk;
            end = blk + 0x10;
        }
    }
done:
    return;
}
} // namespace jj_fn_802FC988

// fn_802FCB14: body from https://github.com/jj-link/brawl src/havok/fn_802FCB14.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_802FCB14 {

// TODO: cleanup (shape from jj-link/brawl fn_802FCB14)
extern "C" void fn_802FCB14(char* self) {
    *(int*)(self + 0x4) = 0;
    if (*(int*)(self + 0x4) == (int)(*(unsigned int*)(self + 0x8) & 0x3FFFFFFF)) {
        hkArrayUtil::_reserveMore((hkArrayBase*)(self), 4);
    }
    *(int*)(self + 0x4) = *(int*)(self + 0x4) + 1;
    char* p = (char*)hkThreadMemory::s_instance->allocateChunk(0x200, 0x1D);
    if (p != 0) {
        *(int*)p = 0;
    }
    **(void***)self = p;
    p[0x13] = 0x10;
    p[0x10] = 1;
    *(int*)(p + 0x18) = -1;
    *(int*)p = 0x10;
}
} // namespace jj_fn_802FCB14
