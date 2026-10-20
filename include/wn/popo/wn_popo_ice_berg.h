#pragma once

// Popo's ice berg article (article kind 0x82): the block of ice of the Ice Climbers' down special. It listens for damage
// (the berg is destroyed by hits) and hands a wind article (wnPopoIceBergWind) out of its article module.

#include <so/damage/so_damage_event_presenter.h>
#include <wn/wn_weapon_builder.h>
#include <wn/wn_kinetic_mediator_impl.h>
#include <wn/wn_kinetic_transactor.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_article_manage_module.h>
#include <ft/builder/ft_module_accesser_builder.h>

WN_STATUS_PROC_VECTOR_ARG_CTOR(4);
// The array instances the sora_melee constructors build.
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 7);
FT_DOL_ARRAY_VECTOR(soShakeTerm, 1);

// The parameter block of the article (3 float and 12 int parameters, ids 4000.. and 24000..).
// HYPOTHESIS: meaning of the parameters.
struct wnPopoIceBergParam {
    s32 unk0;
    s32 unk4;
    float unk8;
    s32 unkC;
    float unk10;
    float unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
};
static_assert(sizeof(wnPopoIceBergParam) == 0x3C, "Ice berg parameter block extent");

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnPopoIceBergData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPopoIceBergParam* m_param; // +0x34
    u8 unk38[8];                       // start of the collision hit data set handed to the hit module
    s32 unk40;                         // HYPOTHESIS: handed to the module build data
};

class wnPopoIceBergParamAccesser : public wnExtendParamAccesser<3999, 3, 23999, 12, 0, 0> {
public:
    wnPopoIceBergParamAccesser(const wnPopoIceBergParam* param) : wnExtendParamAccesser<3999, 3, 23999, 12, 0, 0>(0x82) {
        m_floats[0] = &param->unk8;
        m_floats[1] = &param->unk10;
        m_floats[2] = &param->unk14;
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk4;
        m_ints[2] = &param->unkC;
        m_ints[3] = &param->unk18;
        m_ints[4] = &param->unk1C;
        m_ints[5] = &param->unk20;
        m_ints[6] = &param->unk24;
        m_ints[7] = &param->unk28;
        m_ints[8] = &param->unk2C;
        m_ints[9] = &param->unk30;
        m_ints[10] = &param->unk34;
        m_ints[11] = &param->unk38;
    }
    virtual ~wnPopoIceBergParamAccesser() { }
};
static_assert(sizeof(wnPopoIceBergParamAccesser) == 0x64, "Ice berg parameter accesser extent");

// One normal energy; the berg never changes its kinetic kind (the shared transactor does nothing).
typedef soTypeList<soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<0, 1> >,
                   soTypeListNullType> wnPopoIceBergKineticEnergyTypeList;
typedef soKineticTransactorInfo<wnKineticTransactor, wnKineticTransactor, wnKineticTransactor, soKineticTransactHelper>
    wnPopoIceBergKineticTransactorInfo;

FT_DOL_ARRAY_VECTOR(soArticle*, 1);
WN_ARTICLE_EVENT_OBSERVER_VECTOR(1);
static_assert(sizeof(soArrayVector<soArticleEventObserver, 1>) == 0x1c, "Observer array layout is wrong!");
typedef wnNullMediatorArticleManageModuleBuilder<1> wnPopoIceBergArticleManageModuleBuilder;

class wnPopoIceBergModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soKineticModuleBuildConfig<soKineticModuleGenericImpl,
                                       soKineticMediatorImpl<wnPopoIceBergKineticEnergyTypeList, wnPopoIceBergKineticTransactorInfo> >
        KineticModuleBuildConfig;
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 7, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 2, 1, soCollisionHitModuleImpl, 0x33f, true>
        CollisionHitModuleBuildConfig;
    typedef wnDamageModuleBuildConfig<soDamageModuleImpl, 1, soSingletonHolder<soDamageTransactorNull>, soSingletonHolder<soDamageEffectorNull> >
        DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soShakeModuleBuildConfig<1, soShakeModuleImpl> ShakeModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<4, soGeneralWorkBuildConfig<2, 2, 1>, 4, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 4, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef wnPopoIceBergArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

// The builder of this article carries no appended common energies (WN_WEAPON_NO_COMMON_ENERGIES in the unit): its only
// energy is the normal one of the kinetic mediator.
static_assert(sizeof(wnWeaponBuilder<wnPopoIceBergModuleAccesserBuildConfig>) == 0x2708, "Ice berg builder extent");

class wnPopoIceBerg : public wnWeaponBuilder<wnPopoIceBergModuleAccesserBuildConfig>, public soDamageEventObserver {
public:
    const wnPopoIceBergParam* m_param;         // +0x2714
    wnPopoIceBergParamAccesser m_paramAccesser; // +0x2718
    wnPopoIceBerg(s32 articleId, const wnArticleBuildInfo& info, const wnPopoIceBergData* data);
    virtual ~wnPopoIceBerg() { }
};

static_assert(sizeof(wnPopoIceBerg) == 0x277C, "Ice berg article extent");
