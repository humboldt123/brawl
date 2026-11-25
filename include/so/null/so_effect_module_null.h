#pragma once

#include <so/effect/so_effect_module_impl.h>

class soEffectModuleNull : public soNull, public soNullable {
public:
    virtual ~soEffectModuleNull();
    virtual void activate();
    virtual void deactivate();
    virtual void begin();
    virtual void update(bool);
    virtual u32 req(EfID effectID, Vec3f* pos, Vec3f* rot, float scale, u32, int);
    virtual u32 req2D(EfID effectID, Vec3f* pos, Vec3f* rot, float scale, u32);
    virtual u32 req(EfID effectID, int, Vec3f* pos, Vec3f* rot, float scale, Vec3f* posRange, Vec3f* rotRange, bool, u32);
    virtual bool req(EfID effectID, int, int*, Vec3f* pos, Vec3f* rot, float scale, Vec3f* posRange, Vec3f* rotRange, bool, u32);
    virtual u32 reqAttachGround(EfID effectID, int*, Vec3f* pos, Vec3f* rot, float scale, Vec3f* rotRange, u32);
    virtual u32 reqFollow(EfID effectID, int, Vec3f*, Vec3f*, float scale, bool, u32, u32 effectIDAdd, int);
    virtual int reqContinual(EfID effectID, float, int, bool, u32 index);
    virtual void removeContinual(u32 index);
    virtual void removeAllContinual();
    virtual void reqTime(EfID effectID, int, Vec3f* pos, Vec3f* rot, float scale, u32);
    virtual void reqTimeFollow(EfID effectID, int, int, Vec3f*, Vec3f*, float scale, u32);
    virtual void removeTime(int);
    virtual void removeAllTime();
    virtual void reqEmit(EfID effectID, u32);
    virtual void remove(int);
    virtual void removeAll(u32);
    virtual void kill(int, bool, bool);
    virtual void killKind(int effectKind, bool, bool);
    virtual void killAll(u32, bool, bool);
    virtual int reqAfterImage(int nodeID1, int nodeID2, int, short, Vec3f*, Vec3f*, u32, EfID effectId, int, Vec3f* pos, Vec3f*, float scale, u8, u8, float);
    virtual void removeAfterImage(int, int);
    virtual void removeAllAfterImage(u32, int);
    virtual void setPos(int, Vec3f* pos);
    virtual void setRot(int, Vec3f* rot);
    virtual void setScale(int, Vec3f* scale);
    virtual bool isExistEffect(int);
    virtual void setVisible(int, bool);
    virtual void setWhole(bool);
    virtual void reqPause(bool);
#ifdef FT_FIGHTER_FIELDS
    virtual void fillScreen(int index, u32 frames, GXColor* color, int, int flags);
#else
    virtual void fillScreen(int index, u32, int, int);
#endif
    virtual void clearScreen(int index, u32);
    virtual float getDeadEffectRotZ(Vec3f*, bool* out);
    virtual void reqCommon(int index, float frame);
    virtual void removeCommon(int);
    virtual void resetCommon();
    virtual bool isEndCommon();
    virtual bool isExistCommon(int);
    virtual void reqScreen(int index);
    virtual void removeScreen(int index);
    virtual void setSyncVisibility(bool);
    virtual bool isSyncVisibility();
    virtual void setSyncScale(bool);
    virtual bool isSyncScale();
    virtual void setShieldEffect(int);
    virtual void ignoreScaleZ(bool);
};
