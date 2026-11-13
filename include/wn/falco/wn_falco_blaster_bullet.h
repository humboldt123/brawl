#pragma once

struct wnArticleBuildInfo;

#if defined(WN_WEAPON_BUILDER) && defined(FT_FALCO_ARTICLE_POOLS)
// The fighter unit owns the complete article (its destructor is emitted there). The destructor has exactly the shape of
// Fox's bullet (wn/fox/wn_fox_blaster_bullet.h), so the layout is Fox's; the allocation is one word shorter.
#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>

FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 4);

struct wnFalcoBlasterBulletParam {
    s32 unk0; // Native activation copies this into the lifetime field.
    float unk4;
    float unk8;
};

struct wnFalcoBlasterBulletData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnFalcoBlasterBulletParam* m_param;
};
// HYPOTHESIS: Fox's parameter layout (two floats, one int); the kind id is set by the native constructor.
class wnFalcoBlasterBulletParamAccesser : public wnExtendParamAccesser<3999, 2, 23999, 1, 0, 0> {
public:
    virtual ~wnFalcoBlasterBulletParamAccesser() { }
};
static_assert(sizeof(wnFalcoBlasterBulletParamAccesser) == 0x34, "Falco bullet parameter accesser extent");

class wnFalcoBlasterBulletModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soResourceModuleBuildConfigNull ResourceModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 4, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<2, 2, 1>, 2,
                                      soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};
static_assert(sizeof(wnWeaponBuilder<wnFalcoBlasterBulletModuleAccesserBuildConfig>) == 0x213C,
              "Falco bullet weapon builder extent");

class wnFalcoBlasterBullet : public wnWeaponBuilder<wnFalcoBlasterBulletModuleAccesserBuildConfig> {
    const wnFalcoBlasterBulletParam* m_param;
    wnFalcoBlasterBulletParamAccesser m_paramAccesser;
#else
#include <wn/wn_weapon_builder.h>

struct wnFalcoBlasterBulletData;
// HYPOTHESIS: only the first word, which activate copies into the lifetime, is known.
struct wnFalcoBlasterBulletParam {
    s32 unk0;
};

// HYPOTHESIS: as in the Fox bullet, the parameter pointer (+0x213C) and the parameter accesser (+0x2140)
// follow a 0x213C-byte weapon builder.
class wnFalcoBlasterBullet : public Weapon {
    u8 m_storage[0x213C - sizeof(Weapon)];
    const wnFalcoBlasterBulletParam* m_param;
    u8 m_paramAccesser[0x2174 - 0x2140];
#endif
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnFalcoBlasterBullet(s32 articleId, const wnArticleBuildInfo& info, const wnFalcoBlasterBulletData* data);
#if defined(WN_WEAPON_BUILDER) && defined(FT_FALCO_ARTICLE_POOLS)
    virtual ~wnFalcoBlasterBullet() { }
#else
    virtual ~wnFalcoBlasterBullet();
#endif
    // HYPOTHESIS: r5 is unused; the remaining arguments follow the Fox and Wolf bullet activate.
    void activate(int founderTaskId, int unk5, int team, Vec3f* pos, int unkMode, EfID effectId, float lr,
                  float speed, float angle);
    virtual bool reflect();
    virtual float getCollisionLr(soModuleAccesser* moduleAccesser);
    virtual void processUpdate();
    virtual void updateNodeSRT();
};
static_assert(sizeof(wnFalcoBlasterBullet) == 0x2174, "Falco Blaster Bullet article extent");
