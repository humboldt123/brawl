#pragma once

#include <StaticAssert.h>
#include <ec/ec_mgr.h>
#include <mt/mt_vector.h>
#include <so/effect/so_effect_module_impl.h>
#include <types.h>

// MATCH-ONLY: the inline overrides are defined in reverse of their target order (MWCC emits deferred inline
// virtuals backwards).
class ykEffectModuleImpl : public soEffectModule {
public:
    ykEffectModuleImpl();
    virtual ~ykEffectModuleImpl();
    virtual void activate() { }
    virtual void deactivate() { }
    virtual void begin() { }
    virtual void update(bool) { }
    virtual u32 req(EfID effectID, Vec3f* pos, Vec3f* rot, float scale, u32 group, int anmIdx);
    virtual bool req(EfID effectID, int, int*, Vec3f* pos, Vec3f* rot, float scale, Vec3f* posRange, Vec3f* rotRange, bool, u32) { return false; }
    virtual u32 req(EfID effectID, int, Vec3f* pos, Vec3f* rot, float scale, Vec3f* posRange, Vec3f* rotRange, bool, u32) { return 0; }
    virtual u32 req2D(EfID effectID, Vec3f* pos, Vec3f* rot, float scale, u32) { return 0; }
    virtual u32 reqAttachGround(EfID effectID, int*, Vec3f* pos, Vec3f* rot, float scale, Vec3f* rotRange, u32) { return 0; }
    virtual u32 reqFollow(EfID effectID, int, Vec3f*, Vec3f*, float scale, bool, u32, u32 effectIDAdd, int) { return 0; }
    virtual int reqContinual(EfID effectID, float, int, bool, u32 index) { return 0; }
    virtual void removeContinual(u32 index) { }
    virtual void removeAllContinual() { }
    virtual void reqTime(EfID effectID, int, Vec3f* pos, Vec3f* rot, float scale, u32) { }
    virtual void reqTimeFollow(EfID effectID, int, int, Vec3f*, Vec3f*, float scale, u32) { }
    virtual void removeTime(int) { }
    virtual void removeAllTime() { }
    virtual void reqEmit(EfID effectID, u32) { }
    virtual void remove(int) { }
    virtual void removeAll(u32) { }
    virtual void kill(int, bool, bool) { }
    virtual void killKind(int effectKind, bool, bool) { }
    virtual void killAll(u32, bool, bool) { }
    virtual int reqAfterImage(int nodeID1, int nodeID2, int, short, Vec3f*, Vec3f*, u32, EfID effectId, int, Vec3f* pos, Vec3f*, float scale, u8, u8, float) { return 0; }
    // MATCH-ONLY: no return value (the target returns without setting one).
    virtual bool removeAfterImage(int, int) { }
    virtual void removeAllAfterImage(u32, int) { }
    virtual void setPos(int, Vec3f* pos) { }
    virtual void setRot(int, Vec3f* rot) { }
    virtual void setScale(int, Vec3f* scale) { }
    virtual bool isExistEffect(int) { return false; }
    virtual void setVisible(int, bool) { }
    virtual void setWhole(bool) { }
    virtual void reqPause(bool) { }
    virtual void fillScreen(int index, u32, int, int) { }
    virtual void clearScreen(int index, u32) { }
    virtual float getDeadEffectRotZ(Vec3f*, bool* out) { return 0.0f; }
    virtual void reqCommon(int index, float frame) { }
    virtual void resetCommon() { }
    virtual bool isEndCommon() { return false; }
    virtual bool isExistCommon(int) { return false; }
    virtual void removeCommon(int) { }
    virtual void reqScreen(int index) { }
    virtual void removeScreen(int index) { }
    virtual void setSyncVisibility(bool) { }
    virtual bool isSyncVisibility() { return false; }
    virtual void setSyncScale(bool) { }
    virtual bool isSyncScale() { return false; }
    virtual void setShieldEffect(int) { }
    virtual void ignoreScaleZ(bool) { }
};
static_assert(sizeof(ykEffectModuleImpl) == 8, "Class is wrong size!");
