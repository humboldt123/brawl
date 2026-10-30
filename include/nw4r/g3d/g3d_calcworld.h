#pragma once

#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/math/math_types.h>
#include <types.h>

namespace nw4r {
namespace g3d {

// Declarations soModelModuleImpl's calc-world callbacks need (the anim result and the world matrix manipulator).
class ChrAnmResult {
public:
    void SetScale(const math::VEC3* pScale);
    void SetRotateDeg(const math::VEC3* pRotate);
    void GetRotateDeg(math::VEC3* pRotate) const;
    void SetTranslate(const math::VEC3* pTranslate);
};

class WorldMtxManip {
public:
    void SetScale(f32 x, f32 y, f32 z);

    math::MTX34* m_mtx; // 0x0
};
class ICalcWorldCallback;

class FuncObjCalcWorld {
public:
    u16 GetCallbackNodeID() const { return mNodeID; }
    void SetCallbackNodeID(u16 id) { mNodeID = id; }

    ICalcWorldCallback* mpCallback; // 0x0
    u8 mTiming;                     // 0x4
    u8 mDeleteOption;               // 0x5
    u16 mNodeID;                    // 0x6
};

class ICalcWorldCallback {
public:
    virtual ~ICalcWorldCallback() { }
    virtual void ExecCallbackA(ChrAnmResult* pResult, ResMdl mdl, FuncObjCalcWorld* pFuncObj) = 0;
    virtual void ExecCallbackB(WorldMtxManip* pManip, ResMdl mdl, FuncObjCalcWorld* pFuncObj) { }
    virtual void ExecCallbackC(math::MTX34* pMtxArray, ResMdl mdl, FuncObjCalcWorld* pFuncObj) { }
};

} // namespace g3d
} // namespace nw4r
