// Havok translation unit hk_weak_802D5CAC.o (main.dol 0x802D5CAC-0x802D5CBC), carved from an unclaimed gap.
// Unit name is scaffolding: the original file and the owning classes are not identified yet.
// Functions in address order:
//   0x802D5CAC     8  fn_802D5CAC
//   0x802D5CB4     8  fn_802D5CB4

#include <types.h>

// fn_802D5CAC: body from https://github.com/jj-link/brawl src/havok/fn_802D5CAC.cpp (namespaced).
namespace jj_fn_802D5CAC {
extern "C" void fn_802D54A0(void*);

// TODO: cleanup (shape from jj-link/brawl fn_802D5CAC)
extern "C" void fn_802D5CAC(void* self) {
    fn_802D54A0(reinterpret_cast<u8*>(self) - 0xc);
}
} // namespace jj_fn_802D5CAC

// fn_802D5CB4: body from https://github.com/jj-link/brawl src/havok/fn_802D5CB4.cpp (namespaced).
namespace jj_fn_802D5CB4 {
// MATCH-ONLY: secondary-base this-adjusting thunk; the callee is bound by its mangled name until the
// owning class (hkShapeContainer base at +0xC) is modelled.
extern "C" void __dt__11hkMeshShapeFv(void*);

// TODO: cleanup (shape from jj-link/brawl fn_802D5CB4)
extern "C" void fn_802D5CB4(void* self) {
    __dt__11hkMeshShapeFv(reinterpret_cast<u8*>(self) - 0xc);
}
} // namespace jj_fn_802D5CB4
