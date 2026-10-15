#pragma once

// Ness's PK Thunder (article kind 104): the head of the lightning bolt that Ness steers. It owns a chain of six trailing
// bolts (wnNessPKThunderChild, article kind 105) that it generates one after the other while it flies.
// HYPOTHESIS: all field names; offsets follow the native code.

#include <wn/ness/wn_ness_pk_thunder_child.h>
#include <so/controller/so_controller_module_link_ref.h>
#include <wn/wn_article_pool.h>
#include <ft/builder/ft_dol_instances.h>

// The parameter block of the bolt: five floats (ids 4000..).
struct wnNessPKThunderParam {
    float unk0;
    float m_speed;         // +4: speed of the head
    float m_startAngleMul; // +8: factor of the angle (degrees) the head starts with
    float m_aimTolerance;  // +0xC: distance to the aim on both axes below which the head stops steering
    float m_turnRate;      // +0x10: degrees per frame the head can turn
};

// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnNessPKThunderData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnNessPKThunderParam* m_param; // +0x34
};

class wnNessPKThunderParamAccesser : public wnExtendParamAccesser<3999, 5, 23999, 0, 0, 0> {
public:
    wnNessPKThunderParamAccesser(const wnNessPKThunderParam* param) : wnExtendParamAccesser<3999, 5, 23999, 0, 0, 0>(104) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->m_speed;
        m_floats[2] = &param->m_startAngleMul;
        m_floats[3] = &param->m_aimTolerance;
        m_floats[4] = &param->m_turnRate;
    }
    virtual ~wnNessPKThunderParamAccesser() { }
};

// The mediator with the pool of the six trailing bolts.
typedef wnArticleHierarchy<wnNessPKThunderChild, 6, soInstancePoolRoot> wnNessPKThunderChildPool;
// The hierarchy emits its destructor first so that it can inline the (implicit) one of the pool.
template wnArticleHierarchy<wnNessPKThunderChild, 6, soInstancePoolRoot>::~wnArticleHierarchy();
class wnNessPKThunderMediator : public soArticleMediator {
    wnNessPKThunderChildPool m_pools;
    u8 m_autoRecycle;
public:
    wnNessPKThunderMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~wnNessPKThunderMediator();
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
};

// The list of the articles of the module lives in sora_melee (only declared here).
FT_DOL_ARRAY_VECTOR(soArticle*, 6);

typedef wnArticleManageModuleBuilder<wnNessPKThunderMediator, 6> wnNessPKThunderArticleManageModuleBuilder;

class wnNessPKThunderModuleAccesserBuildConfig : public wnNessPKThunderChildModuleAccesserBuildConfig {
public:
    typedef soGroundModuleBuildConfig<1, soGroundModuleImpl> GroundModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 0, 0> ControllerModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<4, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnNessPKThunderArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnNessPKThunder : public wnWeaponBuilder<wnNessPKThunderModuleAccesserBuildConfig> {
public:
    const wnNessPKThunderParam* m_param;          // +0xDDD0
    wnNessPKThunderParamAccesser m_paramAccesser; // +0xDDD4
    wnNessPKThunder(s32 articleId, const wnArticleBuildInfo& info, const wnNessPKThunderData* data);
    virtual ~wnNessPKThunder() { }
    void activate(int founderTaskId, int team, Vec3f* pos, float lr);
    virtual bool reflect();
    // The activator of the bolts the mediator hands out.
    static bool activeArticle(wnNessPKThunderChild* weapon, soModuleAccesser* acc);
};
