#pragma once

// Zelda's Din's Fire that she steers (article kind 0x29): it flies with a normal energy while the move status runs.
// The status units of the article use the smaller view in wn_zelda_dein_view.h.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_article_manage_module.h>
#include <wn/wn_article_pool.h>
#include <so/controller/so_controller_module_link_ref.h>

WN_STATUS_PROC_VECTOR_ARG_CTOR(2);
FT_DOL_ARRAY_VECTOR(soArticle*, 2);
WN_ARTICLE_EVENT_OBSERVER_VECTOR(2);

// The mediator of the articles Din's Fire hands out. HYPOTHESIS: it owns no pool of its own (the articles are built by the
// fighter), only the auto recycle flag.
class wnZeldaDeinMediator : public soArticleMediator {
    u8 unk8;
    u8 m_autoRecycle;
public:
    wnZeldaDeinMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_autoRecycle(false) { }
    virtual ~wnZeldaDeinMediator();
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
};
typedef wnArticleManageModuleBuilder<wnZeldaDeinMediator, 2> wnZeldaDeinArticleManageModuleBuilder;

// The parameter block of the article: twelve floats (ids 4000..). HYPOTHESIS: meaning of the parameters (see the view header
// for the offsets the status units use).
struct wnZeldaDeinParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
};

// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnZeldaDeinData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnZeldaDeinParam* m_param; // +0x34
};

class wnZeldaDeinParamAccesser : public wnExtendParamAccesser<3999, 12, 23999, 0, 0, 0> {
public:
    wnZeldaDeinParamAccesser(const wnZeldaDeinParam* param) : wnExtendParamAccesser<3999, 12, 23999, 0, 0, 0>(0x29) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->unk10;
        m_floats[5] = &param->unk14;
        m_floats[6] = &param->unk18;
        m_floats[7] = &param->unk1C;
        m_floats[8] = &param->unk20;
        m_floats[9] = &param->unk24;
        m_floats[10] = &param->unk28;
        m_floats[11] = &param->unk2C;
    }
    virtual ~wnZeldaDeinParamAccesser() { }
};

class wnZeldaDeinModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 0, 0> ControllerModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<4, 8, 1>, 2, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef wnZeldaDeinArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnZeldaDein : public wnWeaponBuilder<wnZeldaDeinModuleAccesserBuildConfig> {
public:
    const wnZeldaDeinParam* m_param;          // +0x207C
    wnZeldaDeinParamAccesser m_paramAccesser; // +0x2080
    wnZeldaDein(s32 articleId, const wnArticleBuildInfo& info, const wnZeldaDeinData* data);
    virtual ~wnZeldaDein() { }
};
static_assert(sizeof(wnZeldaDein) == 0x20D8, "Zelda Din's Fire extent");
