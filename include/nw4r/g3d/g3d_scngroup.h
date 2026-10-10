#pragma once

// Local BrawlHeaders shadow: ScnGroup derives from ScnObj (nw4r hierarchy), so game code can pass a group where a
// scene object is expected without a pointer cast.

#include <StaticAssert.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <types.h>

namespace nw4r {
namespace g3d {
class ScnGroup : public ScnObj {

public:
    // TODO
    virtual u32 IsDerivedFrom(int* unk1);
    virtual void G3dProc(int unk1, int unk2, int unk3);
    virtual ~ScnGroup();
    virtual void* GetTypeObj();
    virtual char* GetTypeName();
    virtual void ForEach();
    virtual void SetScnObjOption();
    virtual int GetScnObjOption();
    virtual void GetValueForSortOpa();
    virtual void GetValueForSortXlu();
    virtual void CalcWorldMtx();
    virtual void Insert(int sceneId, nw4r::g3d::ScnObj* scnObj);
    virtual void Remove(u32);
    virtual void Remove(nw4r::g3d::ScnObj* obj);
    // 0xDC: first member after the ScnObj base
    char spacer2[0x8];
    int sceneItemsCount;
};
}
}
// Size: 220
