// Havok translation unit hk_weak_802DB990.o (main.dol 0x802DB990-0x802DB9BC), carved from an unclaimed gap.
// Unit name is scaffolding: the original file and the owning classes are not identified yet.
// Functions in address order:
//   0x802DB990    28  fn_802DB990
//   0x802DB9AC     8  fn_802DB9AC
//   0x802DB9B4     8  fn_802DB9B4

#include <types.h>

// fn_802DB990: body from https://github.com/jj-link/brawl src/havok/fn_802DB990.cpp (namespaced).
namespace jj_fn_802DB990 {
struct fn_802DB990_work {
    void* m0;
    void* m4;
    unsigned int m8;
};

// TODO: cleanup (shape from jj-link/brawl fn_802DB990)
extern "C" void fn_802DB990(fn_802DB990_work* self, void* value)
{
    self->m0 = (char*)self + 0xc;
    self->m4 = value;
    self->m8 = 0x80000020;
}
} // namespace jj_fn_802DB990

// fn_802DB9AC: body from https://github.com/jj-link/brawl src/havok/fn_802DB9AC.cpp (namespaced).
namespace jj_fn_802DB9AC {
// TODO: cleanup (shape from jj-link/brawl fn_802DB9AC)
extern "C" u32 fn_802DB9AC(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DB9AC

// fn_802DB9B4: body from https://github.com/jj-link/brawl src/havok/fn_802DB9B4.cpp (namespaced).
namespace jj_fn_802DB9B4 {
// TODO: cleanup (shape from jj-link/brawl fn_802DB9B4)
extern "C" u32 fn_802DB9B4(void* self) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0x0);
}
} // namespace jj_fn_802DB9B4
