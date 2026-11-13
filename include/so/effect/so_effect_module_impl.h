// SHADOW of BrawlHeaders/so/effect/so_effect_module_impl.h: identical except for the FT_MODULE_BUILDER-guarded declarations
// (constructors/destructors of sora_melee classes that fighter module builders call). Built only for fighter RELs.
// Also: soEffectModule's destructor is inline (ykEffectModuleImpl's destructor does not call it).
#pragma once
struct soEffectContinual;
struct soEffectTime;
#if defined(FT_MODULE_BUILDER) || defined(WN_EM_SIMPLE_UNIT)
class efScreenHandle;
#endif

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <ec/ec_mgr.h>
#include <so/event/so_event_presenter.h>
#include <so/model/so_model_event_presenter.h>
#include <so/status/so_status_event_presenter.h>
#include <so/collision/so_collision_attack_event_presenter.h>
#include <so/collision/so_collision_shield_event_presenter_shield.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/so_null.h>
#include <types.h>
#ifdef SO_EFFECT_COMMON_LIST
#include <so/so_array.h>
#endif

class soModuleAccesser;

class soEffectModule : public soNullable {
public:
    virtual ~soEffectModule() { }
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
    virtual bool removeAfterImage(int, int);
    virtual void removeAllAfterImage(u32, int);
    virtual void setPos(int, Vec3f* pos);
    virtual void setRot(int, Vec3f* rot);
    virtual void setScale(int, Vec3f* scale);
    virtual bool isExistEffect(int);
    virtual void setVisible(int, bool);
    virtual void setWhole(bool);
    virtual void reqPause(bool);
    virtual void fillScreen(int index, u32, int, int);
    virtual void clearScreen(int index, u32);
    virtual float getDeadEffectRotZ(Vec3f*, bool* out);
    // HYPOTHESIS: formal order from native callers; index selects the common
    // effect descriptor and frame is stored in the queued request.
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
static_assert(sizeof(soEffectModule) == 8, "Class is wrong size!");

class soEffectModuleImpl : public soEffectModule, public soAnimCmdEventObserver, public soStatusEventObserver, public soModelEventObserver, public soCollisionAttackEventObserver, public soCollisionShieldEventObserver {
#if defined(FT_MODULE_BUILDER) || defined(WN_EM_SIMPLE_UNIT)
public:
    soEffectModuleImpl(soModuleAccesser* acc, soArray<soEffectContinual>* continuals, void* nodeData, soArray<u32>* u32s, soArray<soEffectTime>* times, void* emitData, soEventObserverRegistrationDesc* regDesc, void* commonData, int n, void* screenData, soArray<efScreenHandle>* screens);
#ifdef FT_MODULE_BUILDER
private:
#endif
#endif
public:
    // HYPOTHESIS: one queued common-effect request. The list unit is 16 bytes with the element at offset 4,
    // and the queue stores this index beside the frame an anim-command reset returned.
    struct soEffectCommonPreserver {
        s32 m_index;
        float m_frame;
    };


    soModuleAccesser* m_moduleAccesser;
    u32 m_groupId; // HYPOTHESIS: the ecMgr group of this module's effects
    soArray<soEffectContinual>* m_continuals;
    s32 unk50;
    s32 unk54;
    u8 unk58[0x4];
    soArray<soEffectTime>* m_times;
    u8 unk60[0x4];
    s32 unk64;
    int m_shieldEffect;
    union {
        u8 unk6C[0x8];
        struct {
            u8 _pad6C[0x4];
            s32 unk70;
        };
    };
    s32 unk74;
#ifdef SO_EFFECT_COMMON_LIST
    // HYPOTHESIS: the common-effect queue occupies this+0x78. The following bytes stay opaque.
    soArrayList<soEffectCommonPreserver, 10> m_commonEffects;
    u8 unk120[0x14];
#else
    u8 unk78[0xBC];
#endif
    u8 unk134;
    u8 unk135_0 : 1;
    u8 unk135_1 : 1;
    u8 unk135_2 : 1;
    u8 m_isWhole : 1;
    u8 m_isSyncVisibility : 1;
    u8 m_isSyncScale : 1;
    u8 unk135_6 : 2;
    u8 unk136[0x2];

    // HYPOTHESIS: erases one queued common-effect request with this index. Returns whether one was erased.
    bool removeCommonPreserver(int index) __attribute__((never_inline));
    void pushGroup(u32 group); // HYPOTHESIS: parameter type from ecMgr::pushsetCurrentGroup
    void pushGroupIgnoreScaleZ(u32 group); // HYPOTHESIS: pushGroup followed by ecMgr::setFlatMode(false)
    void pushGroupScaleZ(u32 group); // HYPOTHESIS: pushGroup followed by ecMgr::setFlatMode(true)

    virtual ~soEffectModuleImpl();
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
    virtual bool removeAfterImage(int, int);
    virtual void removeAllAfterImage(u32, int);
    virtual void setPos(int, Vec3f* pos);
    virtual void setRot(int, Vec3f* rot);
    virtual void setScale(int, Vec3f* scale);
    virtual bool isExistEffect(int);
    virtual void setVisible(int, bool);
    virtual void setWhole(bool);
    virtual void reqPause(bool);
    virtual void fillScreen(int index, u32, int, int);
    virtual void clearScreen(int index, u32);
    virtual float getDeadEffectRotZ(Vec3f*, bool* out);
    // HYPOTHESIS: formal order from native callers; index selects the common
    // effect descriptor and frame is stored in the queued request.
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

    virtual bool notifyEventCollisionShieldCheck();
    virtual void notifyEventDestructInstance(soModuleAccesser* moduleAccesser);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionShieldSearch(soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser);
    virtual bool isObserv(char unk1);
};
static_assert(sizeof(soEffectModuleImpl) == 0x138, "Class is wrong size!");
