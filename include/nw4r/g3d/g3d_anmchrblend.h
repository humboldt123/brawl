#pragma once

// The blend of several node animations of a model (as in the nw4r library). Only what the game calls is declared; the
// virtual functions are in the order of the nw4r animation objects, so that the blend's weights are at their slots.

#include <StaticAssert.h>
#include <MEM.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <types.h>

namespace nw4r {
namespace g3d {
class AnmObjChrBlend : public G3dObj {
public:
    virtual u32 IsDerivedFrom(int* unk1);
    virtual void G3dProc(int unk1, int unk2, int unk3);
    virtual ~AnmObjChrBlend();
    virtual int* GetTypeObj();
    virtual int GetTypeName();
    virtual void SetFrame(float frame);
    virtual float GetFrame();
    virtual void UpdateFrame();
    virtual void SetUpdateRate(float rate);
    virtual float GetUpdateRate();
    virtual int Bind(ResMdl mdl);
    virtual void Release();
    virtual int* GetResult(int* result, u32 idx);
    virtual AnmObjChrRes* Attach(int idx, AnmObjChrRes* res);  // 0x3C
    virtual AnmObjChrRes* Detach(int idx);
    virtual void DetachAll();
    virtual void SetWeight(int idx, float weight);              // 0x48
    virtual float GetWeight(int idx);                           // 0x4C

    static AnmObjChrBlend* Construct(MEMAllocator* allocator, u32* size, ResMdl mdl, int numChildren);
};
}
}
