#pragma once

// The fighter unit owns the complete article (Fox's form, wn/fox/wn_fox_blaster_bullet.h); the sora_melee article unit
// keeps the opaque form below.
#if defined(WN_WEAPON_BUILDER) && defined(FT_WOLF_ARTICLE_POOLS)

#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>

FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 4);

struct wnWolfBlasterBulletParam {
    s32 unk0; // Native activation copies this into the lifetime field.
    float unk4;
    float unk8;
};
struct wnWolfBlasterBulletData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnWolfBlasterBulletParam* m_param;
};
class wnWolfBlasterBulletParamAccesser : public wnExtendParamAccesser<3999, 2, 23999, 1, 0, 0> {
public:
    explicit wnWolfBlasterBulletParamAccesser(const wnWolfBlasterBulletParam* param) :
        wnExtendParamAccesser<3999, 2, 23999, 1, 0, 0>(31) {
        m_floats[0] = &param->unk4;
        m_floats[1] = &param->unk8;
        m_ints[0] = &param->unk0;
    }
    virtual ~wnWolfBlasterBulletParamAccesser() { }
};
static_assert(sizeof(wnWolfBlasterBulletParamAccesser) == 0x34, "Wolf bullet parameter accesser extent");

class wnWolfBlasterBulletModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16,32,4,2,4,4,4,4,2,4,4,4,4,4,1,1,0,0,0,
                                                  wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soResourceModuleBuildConfigNull ResourceModuleBuildConfig;
    typedef soModelModuleBuildConfig<5,0,soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2,soMotionModuleImpl,0,0,
        soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
        soMotionAnimObjCacheModuleBuildConfig<0,soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter,4,0,soCollisionAttackModuleImpl,1,false,true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5,soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2,soGeneralWorkBuildConfig<2,2,1>,2,
        soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1,2,0,0,1,2,0,0,1,2,0,0,1,2,0,0> AnimCmdModuleSubBuildConfig;
};
static_assert(sizeof(soModuleAccesserBuilder<wnWolfBlasterBulletModuleAccesserBuildConfig>) == 0x1A44,
              "Wolf bullet module builder extent");
static_assert(sizeof(wnWeaponBuilder<wnWolfBlasterBulletModuleAccesserBuildConfig>) == 0x213C,
              "Wolf bullet weapon builder extent");

class wnWolfBlasterBullet : public wnWeaponBuilder<wnWolfBlasterBulletModuleAccesserBuildConfig> {
    const wnWolfBlasterBulletParam* m_param;
    wnWolfBlasterBulletParamAccesser m_paramAccesser;
    s32 unk2174;
public:
    wnWolfBlasterBullet(s32 articleId, const wnArticleBuildInfo& info, const wnWolfBlasterBulletData* data);
    virtual ~wnWolfBlasterBullet() { }
    // HYPOTHESIS: spelling of the unused second integer and mode parameter.
    void activate(int founderTaskId, int unk5, int team, Vec3f* pos, int unkMode, EfID effectId,
                  float lr, float speed, float angle);
    virtual bool reflect();
    virtual float getCollisionLr(soModuleAccesser* moduleAccesser);
    virtual void processUpdate();
    virtual void updateNodeSRT();
};
static_assert(sizeof(wnWolfBlasterBullet) == 0x2178, "Wolf bullet allocation extent");
#else

#include <wn/wn_weapon_builder.h>
#include <wn/wn_kinetic_energy_normal.h>

struct wnWolfBlasterBulletModuleAccesserBuildConfig;
// HYPOTHESIS: the article data type the class info fetches for kind 39; only passed as a pointer, the
// layout is unreconstructed (mirror of the Fox family's wnFoxBlasterBulletData role).
struct wnWolfBlasterBulletData;
struct wnArticleBuildInfo;

// Constructor/destructor offsets establish the builder extent through the
// embedded normal energy at +0x20C0. The remaining module-builder storage is
// opaque until the configuration is reconstructed.
template <>
class wnWeaponBuilder<wnWolfBlasterBulletModuleAccesserBuildConfig> : public Weapon {
    u8 unkCC[0x20C0 - sizeof(Weapon)];
    wnKineticEnergyNormal m_normalEnergy;
public:
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
static_assert(sizeof(wnWeaponBuilder<wnWolfBlasterBulletModuleAccesserBuildConfig>) == 0x20F8,
              "Wolf Bullet builder extent");

class wnWolfBlasterBullet : public wnWeaponBuilder<wnWolfBlasterBulletModuleAccesserBuildConfig> {
    void* unk20F8;
    u8 unk20FC[0x40];
    int* unk213C; // HYPOTHESIS: native activate reads one word through this pointer.
    u8 unk2140[0x38];
public:
    wnWolfBlasterBullet(s32 articleId, const wnArticleBuildInfo& info, const wnWolfBlasterBulletData* data);
    virtual ~wnWolfBlasterBullet();
    // HYPOTHESIS: r5 is unused; remaining arguments follow native caller/register stores.
    void activate(int founderTaskId, int unk5, int team, Vec3f* pos, int unkMode, EfID effectId, float lr, float speed, float angle);
    virtual bool reflect();
    virtual float getCollisionLr(soModuleAccesser* moduleAccesser);
    virtual void processUpdate();
    virtual void updateNodeSRT();
};
static_assert(sizeof(wnWolfBlasterBullet) == 0x2178, "Wolf Bullet article extent");
#endif
