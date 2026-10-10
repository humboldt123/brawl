#pragma once

// Local shadow: BrawlHeaders does not know nw4r::g3d::ScnProc, the scene object with a draw function of its own that the
// stage uses to draw its blocks (main.dol: Construct, user data at 0xF0).

#include <nw4r/g3d/g3d_scnobj.h>
#include <memory.h>
#include <types.h>

namespace nw4r {
namespace g3d {

class ScnProc : public ScnObj {
public:
    typedef void (*DrawProc)(ScnProc* proc, bool opa);

    static ScnProc* Construct(MEMAllocator* allocator, u32* size, DrawProc drawProc, bool opa, bool xlu, u32 userData = 0);

    void SetUserData(void* data) {
        *reinterpret_cast<void**>(reinterpret_cast<u8*>(this) + 0xF0) = data;
    }
    void* GetUserData() {
        return *reinterpret_cast<void**>(reinterpret_cast<u8*>(this) + 0xF0);
    }
};

} // namespace g3d
} // namespace nw4r
