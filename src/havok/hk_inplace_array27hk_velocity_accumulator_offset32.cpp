// Havok translation unit hk_inplace_array27hk_velocity_accumulator_offset32.o (main.dol 0x802DAA68-0x802DAAF8).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802DAA68   144  __dt   [map: hkInplaceArray_27hkVelocityAccumulatorOffset_32_____dt]

// fn_802DAA68: body from https://github.com/jj-link/brawl src/havok/fn_802DAA68.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_802DAA68 {
class SoHeapAllocator {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void free(void*, unsigned short, int);
};


struct Array {
    void* data;
    int size;
    unsigned int raw;

    bool isOwned() const {
        return (raw & 0x80000000) == 0;
    }

    void dealloc() {
        hkThreadMemory::s_instance->deallocateChunk(data, raw << 2, 0x15);
    }

    ~Array() {
        if (isOwned())
            dealloc();
    }
};

// TODO: cleanup (shape from jj-link/brawl fn_802DAA68)
extern "C" Array* fn_802DAA68(Array* arg1, int arg2) {
    if (arg1) {
        if (arg1) {
            if (arg1->isOwned())
                arg1->dealloc();
        }
        if (arg2 > 0)
            ((SoHeapAllocator*)hkMemory::s_instance)->free(arg1, 0x8C, 0x15);
    }
    return arg1;
}
} // namespace jj_fn_802DAA68
