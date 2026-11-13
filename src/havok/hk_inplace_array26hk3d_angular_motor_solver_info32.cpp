// Havok translation unit hk_inplace_array26hk3d_angular_motor_solver_info32.o (main.dol 0x802DB9BC-0x802DBA6C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802DB9BC    28  __ct   [map: hkInplaceArray_26hk3dAngularMotorSolverInfo_32_____ct]
//   0x802DB9D8   148  __dt   [map: hkInplaceArray_26hk3dAngularMotorSolverInfo_32_____dt]

// Weak inline copies emitted in this unit; extern "C" placeholders under their fn_ names until the
// instantiating callers are written (shape from https://github.com/jj-link/brawl src/havok/fn_<addr>.cpp).
// hkInplaceArray<hk3dAngularMotorSolverInfo, 32>::hkInplaceArray(int size)
struct hkInplaceArrayHeader_802DB9BC {
    void* m_data;
    int m_size;
    int m_capacityAndFlags;
};
// TODO: cleanup (shape from jj-link/brawl fn_802DB9BC)
extern "C" void fn_802DB9BC(hkInplaceArrayHeader_802DB9BC* a, int size) {
    a->m_data = (char*)a + 0xC;
    a->m_size = size;
    a->m_capacityAndFlags = 0x80000020;
}

// fn_802DB9D8: body from https://github.com/jj-link/brawl src/havok/fn_802DB9D8.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_802DB9D8 {
class SoHeapAllocator {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void free(void*, unsigned int, int);
};


struct Array {
    void* data;
    int size;
    unsigned int raw;

    bool isOwned() const {
        return (raw & 0x80000000) == 0;
    }

    void dealloc() {
        hkThreadMemory::s_instance->deallocateChunk(data, (raw & 0x3FFFFFFF) * 0x4C, 0x15);
    }

    ~Array() {
        if (isOwned())
            dealloc();
    }
};

// TODO: cleanup (shape from jj-link/brawl fn_802DB9D8)
extern "C" Array* fn_802DB9D8(Array* self, int flag) {
    if (self) {
        self->~Array();
        if (flag > 0)
            ((SoHeapAllocator*)hkMemory::s_instance)->free(self, 0x98c, 0x15);
    }
    return self;
}
} // namespace jj_fn_802DB9D8
