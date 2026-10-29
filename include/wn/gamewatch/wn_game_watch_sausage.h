#pragma once

#include <wn/wn_weapon_builder.h>

struct wnArticleBuildInfo;

// One launch record per throw variation: 0x14 bytes, read by activate.
// HYPOTHESIS: meaning of the fields (horizontal speed, vertical speed, gravity, speed limit, spin speed).
struct wnGameWatchSausageSpeed {
    float m_speedX;
    float m_speedY;
    float m_gravity;
    float m_limitY;
    float m_rotSpeed;
};

// The parameter record reached through the article data +0x34: one float, two ints, then the launch table.
// HYPOTHESIS: meaning of the scalar parameters.
struct wnGameWatchSausageParam {
    float unk0;
    s32 unk4;
    s32 unk8;
    const wnGameWatchSausageSpeed* m_speeds; // +0xC
};

#ifdef WN_WEAPON_BUILDER
#include <so/anim/so_anim_obj.h>
#include <wn/wn_extend_param_accesser.h>

// HYPOTHESIS: layout of the untouched part of the static article data.
struct wnGameWatchSausageData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnGameWatchSausageParam* m_param; // +0x34
};

class wnGameWatchSausageParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 2, 0, 0> {
public:
    wnGameWatchSausageParamAccesser(const wnGameWatchSausageParam* param) :
        wnExtendParamAccesser<3999, 1, 23999, 2, 0, 0>(117) {
        m_floats[0] = &param->unk0;
        m_ints[0] = &param->unk4;
        m_ints[1] = &param->unk8;
    }
    virtual ~wnGameWatchSausageParamAccesser() { }
};
static_assert(sizeof(wnGameWatchSausageParamAccesser) == 0x34, "Sausage parameter accessor extent");

class wnGameWatchSausageModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
        wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<6, soMotionModuleImpl, 0, 1,
        soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
        soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0,
        soCollisionAttackModuleImpl, 1, false, true> CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<2, 2, 1>, 2,
        soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 2, 0, 0, 1, 6, 0, 0, 1, 6, 0, 0, 1, 6, 0, 0>
        AnimCmdModuleSubBuildConfig;
};

class wnGameWatchSausage : public wnWeaponBuilder<wnGameWatchSausageModuleAccesserBuildConfig> {
    const wnGameWatchSausageParam* m_param;          // +0x2148
    wnGameWatchSausageParamAccesser m_paramAccesser; // +0x214C
    soAnimObj m_animObj;                             // +0x2180
#else
// Call-side view keeps the native extent without instantiating the builder.
struct wnGameWatchSausageData;
class wnGameWatchSausage : public Weapon {
    u8 m_storage[0x21A8 - sizeof(Weapon)];
#endif
public:
    wnGameWatchSausage(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchSausageData* data);
#ifdef WN_WEAPON_BUILDER
    virtual ~wnGameWatchSausage() { }
#else
    virtual ~wnGameWatchSausage();
#endif
    // HYPOTHESIS: parameter names and the order of the integer and float groups (the native code only fixes the
    // order within each group).
    void activate(int founderTaskId, int resourceId, int variation, int team, const Vec3f& position, int speedIndex,
                  int unk40, float lr, float frame);
    virtual void deactivate(bool force);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
#ifdef WN_WEAPON_BUILDER
static_assert(sizeof(wnWeaponBuilder<wnGameWatchSausageModuleAccesserBuildConfig>) == 0x2148, "Sausage builder extent");
#endif
static_assert(sizeof(wnGameWatchSausage) == 0x21A8, "Game & Watch sausage article extent");
