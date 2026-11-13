// Havok translation unit hk_weak_802D7FD0.o (main.dol 0x802D7FD0-0x802D8064), carved from an unclaimed gap.
// Unit name is scaffolding: the original file and the owning classes are not identified yet.
// Functions in address order:
//   0x802D7FD0     8  fn_802D7FD0
//   0x802D7FD8     8  fn_802D7FD8
//   0x802D7FE0    32  fn_802D7FE0
//   0x802D8000     8  fn_802D8000
//   0x802D8008    92  fn_802D8008

#include <types.h>

// fn_802D7FD0: body from https://github.com/jj-link/brawl src/havok/fn_802D7FD0.cpp (namespaced).
namespace jj_fn_802D7FD0 {
// TODO: cleanup (shape from jj-link/brawl fn_802D7FD0)
extern "C" s32 fn_802D7FD0(void) {
    return 0x0;
}
} // namespace jj_fn_802D7FD0

// fn_802D7FD8: body from https://github.com/jj-link/brawl src/havok/fn_802D7FD8.cpp (namespaced).
namespace jj_fn_802D7FD8 {
// TODO: cleanup (shape from jj-link/brawl fn_802D7FD8)
extern "C" s32 fn_802D7FD8(void) {
    return 0x1;
}
} // namespace jj_fn_802D7FD8

// fn_802D7FE0: body from https://github.com/jj-link/brawl src/havok/fn_802D7FE0.cpp (namespaced).
namespace jj_fn_802D7FE0 {
class Foo {
public:
    virtual void a(int);
    virtual void b();
    virtual void c();
};

// TODO: cleanup (shape from jj-link/brawl fn_802D7FE0)
extern "C" void fn_802D7FE0(Foo* p) {
    if (!p) return;
    p->a(1);
}
} // namespace jj_fn_802D7FE0

// fn_802D8000: body from https://github.com/jj-link/brawl src/havok/fn_802D8000.cpp (namespaced).
namespace jj_fn_802D8000 {
// TODO: cleanup (shape from jj-link/brawl fn_802D8000)
extern "C" s32 fn_802D8000(void) {
    return 0x0;
}
} // namespace jj_fn_802D8000

// fn_802D8008: body from https://github.com/jj-link/brawl src/havok/fn_802D8008.cpp (namespaced).
#include <havok/hkMemory.h>
namespace jj_fn_802D8008 {
class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5(void*, u16, s32);
};

struct DtorObj {
    void* vtable;
    u16 type;
};

// TODO: cleanup (shape from jj-link/brawl fn_802D8008)
extern "C" void* fn_802D8008(void* arg0, s32 arg1) {
    DtorObj* self = (DtorObj*)arg0;
    if (self != 0 && arg1 > 0) {
        ((Object_vptr_0*)hkMemory::s_instance)->slot5(self, self->type, 0x20);
    }
    return self;
}
} // namespace jj_fn_802D8008
