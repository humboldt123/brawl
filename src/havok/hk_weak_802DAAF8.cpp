// Havok translation unit hk_weak_802DAAF8.o (main.dol 0x802DAAF8-0x802DAD64), carved from an unclaimed gap.
// Unit name is scaffolding: the original file and the owning classes are not identified yet.
// Functions in address order:
//   0x802DAAF8     8  fn_802DAAF8
//   0x802DAB00     8  fn_802DAB00
//   0x802DAB08    80  fn_802DAB08
//   0x802DAB58    52  fn_802DAB58
//   0x802DAB8C    20  fn_802DAB8C
//   0x802DABA0    72  fn_802DABA0
//   0x802DABE8    88  fn_802DABE8
//   0x802DAC40   292  fn_802DAC40

#include <types.h>
#include <havok/hkBallSocketChainData.h>
void cleanupLoadedObjecthkBallSocketChainData(hkBallSocketChainData* self);
#include <havok/hkConstraintAtom.h>

// fn_802DAAF8: body from https://github.com/jj-link/brawl src/havok/fn_802DAAF8.cpp (namespaced).
namespace jj_fn_802DAAF8 {
// TODO: cleanup (shape from jj-link/brawl fn_802DAAF8)
extern "C" u32 fn_802DAAF8(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x1c);
}
} // namespace jj_fn_802DAAF8

// fn_802DAB00: body from https://github.com/jj-link/brawl src/havok/fn_802DAB00.cpp (namespaced).
namespace jj_fn_802DAB00 {
typedef signed long s32;

// TODO: cleanup (shape from jj-link/brawl fn_802DAB00)
extern "C" s32 fn_802DAB00(void) {
    return 0x01000000;
}
} // namespace jj_fn_802DAB00

// fn_802DAB08: body from https://github.com/jj-link/brawl src/havok/fn_802DAB08.cpp (namespaced).
namespace jj_fn_802DAB08 {
extern "C" {
    extern char lbl_804121D8[];
}

struct Holder {
    void* unk0;
    void* unk4;
    void* unk8;
    int unkC;
};

extern "C" Holder lbl_80532E40;

// TODO: cleanup (shape from jj-link/brawl fn_802DAB08)
extern "C" void fn_802DAB08() {
    int temp_r3;

    temp_r3 = (int)hkBallSocketChainData::getVtablehkBallSocketChainData();
    lbl_80532E40.unk0 = lbl_804121D8;
    lbl_80532E40.unk4 = (void*)hkBallSocketChainData::finishLoadedObjecthkBallSocketChainData;
    lbl_80532E40.unk8 = (void*)::cleanupLoadedObjecthkBallSocketChainData;
    lbl_80532E40.unkC = temp_r3;
}

__declspec(section ".ctors") extern void* const fn_802DAB08_ctor = (void*)fn_802DAB08;
} // namespace jj_fn_802DAB08

// fn_802DAB58: body from https://github.com/jj-link/brawl src/havok/fn_802DAB58.cpp (namespaced).
namespace jj_fn_802DAB58 {
struct Value { s32 value; };

extern "C" void fn_802DABE8(s32, Value*);

// TODO: cleanup (shape from jj-link/brawl fn_802DAB58)
extern "C" void fn_802DAB58(s32 arg0) {
    Value value;
    value.value = 1;
    if (arg0 != 0) {
        Value copy = value;
        fn_802DABE8(arg0, &copy);
    }
}
} // namespace jj_fn_802DAB58

// fn_802DAB8C: body from https://github.com/jj-link/brawl src/havok/fn_802DAB8C.cpp (namespaced).
namespace jj_fn_802DAB8C {
class Object {
public:
    virtual void slot0(int);
};

// TODO: cleanup (shape from jj-link/brawl fn_802DAB8C)
extern "C" void fn_802DAB8C(Object* self) {
    self->slot0(-1);
}
} // namespace jj_fn_802DAB8C

// fn_802DABA0: body from https://github.com/jj-link/brawl src/havok/fn_802DABA0.cpp (namespaced).
namespace jj_fn_802DABA0 {
struct Result {
    int value;
    char rest[60];
};
extern "C" void fn_802DABE8(Result*, int*);
// TODO: cleanup (shape from jj-link/brawl fn_802DABA0)
extern "C" int fn_802DABA0() {
    __attribute__((aligned(16))) Result out;
    int zero = 0;
    if (&out != 0) {
        int in = zero;
        fn_802DABE8(&out, &in);
    }
    return out.value;
}
} // namespace jj_fn_802DABA0

// fn_802DABE8: body from https://github.com/jj-link/brawl src/havok/fn_802DABE8.cpp (namespaced).
namespace jj_fn_802DABE8 {
extern "C" {
extern char lbl_80487A38[];
void fn_802D95E4(void* self, void* v);

// TODO: cleanup (shape from jj-link/brawl fn_802DABE8)
extern "C" void* fn_802DABE8(void* p) {
    u8* self = (u8*)p;
    void* saved = *(void**)(self + 0x14);
    *(s16*)(self + 6) = 1;
    *(void**)self = (void*)lbl_80487A38;
    ((hkBridgeConstraintAtom*)(self + 0xC))->init((hkConstraintData*)saved);
    ((hkBridgeConstraintAtom*)(self + 0xC))->init((hkConstraintData*)self);
    return p;
}
}
} // namespace jj_fn_802DABE8

// fn_802DAC40: body from https://github.com/jj-link/brawl src/havok/fn_802DAC40.cpp (namespaced).
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
namespace jj_fn_802DAC40 {
#pragma use_lmw_stmw on
extern "C" void fn_8027EABC(void*, void*, unsigned int, int);
extern "C" char lbl_80487A38[];
class SoHeapAllocator {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void free(void*, unsigned short, int);
};
class Elem {
public:
    virtual void vf0(int);
    unsigned short m04;
    short m06;
};
struct Entry {
    char pad[0x40];
    Elem* e[3];
    char pad2[4];
};
struct Array {
    Entry* data;
    int count;
    unsigned int raw;
    bool isOwned() const { return (raw & 0x80000000) == 0; }
    void dealloc() { hkThreadMemory::s_instance->deallocateChunk(data, (raw & 0x3fffffff) * 0x50, 0x15); }
};
struct Obj {
    void* vtable;
    unsigned short type;
    char pad[0x12];
    Array array;
};
// TODO: cleanup (shape from jj-link/brawl fn_802DAC40)
extern "C" Obj* fn_802DAC40(Obj* self, int flag) {
    if (self) {
        self->vtable = lbl_80487A38;
        for (int i = 0; i < self->array.count; i++) {
            for (int j = 0; j < 3; j++) {
                Elem* e = self->array.data[i].e[j];
                if (e && e->m04 != 0) {
                    if (--e->m06 == 0 && e != 0) {
                        e->vf0(1);
                    }
                }
            }
        }
        if ((char*)&self->array != 0 && self->array.isOwned())
            self->array.dealloc();
        if (flag > 0)
            ((SoHeapAllocator*)hkMemory::s_instance)->free(self, self->type, 0x2a);
    }
    return self;
}
} // namespace jj_fn_802DAC40
