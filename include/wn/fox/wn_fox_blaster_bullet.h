#pragma once

#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>

FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 4);

struct wnFoxBlasterBulletParam {
    s32 unk0; // Native activation copies this into the lifetime field.
    float unk4;
    float unk8;
};
struct wnFoxBlasterBulletData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnFoxBlasterBulletParam* m_param;
};
class wnFoxBlasterBulletParamAccesser : public wnExtendParamAccesser<3999, 2, 23999, 1, 0, 0> {
public:
    explicit wnFoxBlasterBulletParamAccesser(const wnFoxBlasterBulletParam* param) :
        wnExtendParamAccesser<3999, 2, 23999, 1, 0, 0>(31) {
        m_floats[0] = &param->unk4;
        m_floats[1] = &param->unk8;
        m_ints[0] = &param->unk0;
    }
    virtual ~wnFoxBlasterBulletParamAccesser() { }
};
static_assert(sizeof(wnFoxBlasterBulletParamAccesser) == 0x34, "Fox bullet parameter accesser extent");

class wnFoxBlasterBulletModuleAccesserBuildConfig : public wnCommonBuildConfig {
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
static_assert(sizeof(soModuleAccesserBuilder<wnFoxBlasterBulletModuleAccesserBuildConfig>) == 0x1A44,
              "Fox bullet module builder extent");
static_assert(sizeof(wnWeaponBuilder<wnFoxBlasterBulletModuleAccesserBuildConfig>) == 0x213C,
              "Fox bullet weapon builder extent");

class wnFoxBlasterBullet : public wnWeaponBuilder<wnFoxBlasterBulletModuleAccesserBuildConfig> {
    const wnFoxBlasterBulletParam* m_param;
    wnFoxBlasterBulletParamAccesser m_paramAccesser;
    s32 unk2174;
public:
    wnFoxBlasterBullet(s32 articleId, const wnArticleBuildInfo& info, const wnFoxBlasterBulletData* data);
    virtual ~wnFoxBlasterBullet() { }
    // HYPOTHESIS: spelling of the unused second integer and mode parameter.
    void activate(int founderTaskId, int unk5, int team, Vec3f* pos, int unkMode, EfID effectId,
                  float lr, float speed, float angle);
    virtual bool reflect();
    virtual float getCollisionLr(soModuleAccesser* moduleAccesser);
    virtual void processUpdate();
    virtual void updateNodeSRT();
};
static_assert(sizeof(wnFoxBlasterBullet) == 0x2178, "Fox bullet allocation extent");
