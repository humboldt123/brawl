// Havok translation unit hk_weak_802F30D4.o (main.dol 0x802F30D4-0x802F34F0), carved from an unclaimed gap.
// Unit name is scaffolding: the original file and the owning classes are not identified yet.
// Functions in address order:
//   0x802F30D4   432  fn_802F30D4
//   0x802F3284     4  fn_802F3284
//   0x802F3288     4  fn_802F3288
//   0x802F328C     4  fn_802F328C
//   0x802F3290   200  fn_802F3290
//   0x802F3358   200  fn_802F3358
//   0x802F3420    28  fn_802F3420
//   0x802F343C   104  fn_802F343C
//   0x802F34A4    76  fn_802F34A4

#include <types.h>
#include <havok/hkVector4.h>
#include <havok/hkAgentNnMachine.h>
#include <havok/hkWorldAgentUtil.h>

// fn_802F3284: body from https://github.com/jj-link/brawl src/havok/fn_802F3284.cpp (namespaced).
namespace jj_fn_802F3284 {
// TODO: cleanup (shape from jj-link/brawl fn_802F3284)
extern "C" void fn_802F3284(void) {}
} // namespace jj_fn_802F3284

// fn_802F3288: body from https://github.com/jj-link/brawl src/havok/fn_802F3288.cpp (namespaced).
namespace jj_fn_802F3288 {
// TODO: cleanup (shape from jj-link/brawl fn_802F3288)
extern "C" void fn_802F3288(void) {}
} // namespace jj_fn_802F3288

// fn_802F328C: body from https://github.com/jj-link/brawl src/havok/fn_802F328C.cpp (namespaced).
namespace jj_fn_802F328C {
// TODO: cleanup (shape from jj-link/brawl fn_802F328C)
extern "C" void fn_802F328C(void) {}
} // namespace jj_fn_802F328C

// fn_802F3420: body from https://github.com/jj-link/brawl src/havok/fn_802F3420.cpp (namespaced).
namespace jj_fn_802F3420 {
extern "C" {
extern char lbl_804886A8[];

struct Fn802F3420Obj {
    void *ptr;
    short a;
    short b;
    int c;
};

// TODO: cleanup (shape from jj-link/brawl fn_802F3420)
extern "C" void fn_802F3420(Fn802F3420Obj *self, int arg) {
    self->b = 1;
    self->ptr = lbl_804886A8;
    self->c = arg;
}
}
} // namespace jj_fn_802F3420

// fn_802F34A4: body from https://github.com/jj-link/brawl src/havok/fn_802F34A4.cpp (namespaced).
namespace jj_fn_802F34A4 {
extern "C" int fn_80300488(char* a, char* b);
extern "C" void fn_802F3670();

struct Pair {
    char* unk0;
    char* unk4;
};

// TODO: cleanup (shape from jj-link/brawl fn_802F34A4)
extern "C" void fn_802F34A4(void* unused, Pair* arg1) {
    char* temp_r5 = arg1->unk0;
    char* temp_r4 = arg1->unk4;
    hkAgentNnEntry* e = hkAgentNnMachine_FindAgent((const hkLinkedCollidable*)(temp_r5 + (signed char)temp_r5[5]), (const hkLinkedCollidable*)(temp_r4 + (signed char)temp_r4[5]));
    if (e != 0) {
        hkWorldAgentUtil::removeAgent(e);
    }
}
} // namespace jj_fn_802F34A4
