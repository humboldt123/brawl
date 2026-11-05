#pragma once
// wnPitParthena: Palutena's Final Smash article. It owns the pool of ten Ikarus articles (the army) through its generate
// article manage module, as F.L.U.D.D. (include/wn/mario/wn_mario_pump.h) owns its water. Only compiled where the complete
// builders are wanted (WN_PIT_IKARUS_FULL_BUILDER, see wn_pit_ikarus.h).
#ifdef WN_PIT_IKARUS_FULL_BUILDER
#include <wn/pit/wn_pit_ikarus.h>
#include <wn/wn_article_pool.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <so/controller/so_controller_module_link_ref.h>

struct wnPitParthenaParam;
// The ctor reads the resource group at 0x0 and the param block at 0x34 (same shape as wnCaptainFinalBgData).
struct wnPitParthenaData {
    s32 m_resGroupNo;
    u8 unk4[0x34 - 0x4];
    const wnPitParthenaParam* m_param;
};

// HYPOTHESIS: three floats and seven ints (wnExtendParamAccesser<3999, 3, 23999, 7, 0, 0> in the map); roles unknown.
// Float/int order from the param accesser ctor: floats at 0x0, 0x18, 0x1C; ints at 0x4-0x14, 0x20, 0x24.
struct wnPitParthenaParam {
    float unk0;
    s32 unk4, unk8, unkC, unk10, unk14;
    float unk18, unk1C;
    s32 unk20, unk24;
};
class wnPitParthenaParamAccesser : public wnExtendParamAccesser<3999, 3, 23999, 7, 0, 0> {
public:
    explicit wnPitParthenaParamAccesser(const wnPitParthenaParam* param) : wnExtendParamAccesser<3999, 3, 23999, 7, 0, 0>(0x7E) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk18;
        m_floats[2] = &param->unk1C;
        m_ints[0] = &param->unk4;
        m_ints[1] = &param->unk8;
        m_ints[2] = &param->unkC;
        m_ints[3] = &param->unk10;
        m_ints[4] = &param->unk14;
        m_ints[5] = &param->unk20;
        m_ints[6] = &param->unk24;
    }
    virtual ~wnPitParthenaParamAccesser() { }
};

// HYPOTHESIS: source wrapper name for soArticleMediatorImpl<soTypeList<soInstancePoolInfo<wnPitIkarus, 10, wnInstanceHolder<...> > > >
// (certain from the map names: ten Ikarus holders).
typedef wnArticleHierarchy<wnPitIkarus, 10, soInstancePoolRoot> wnPitParthenaIkarusPool;
class wnPitParthenaMediator : public soArticleMediator {
    wnPitParthenaIkarusPool m_pools;
    u8 m_autoRecycle;
    WN_ARTICLE_SLOT_FRIENDS
public:
    wnPitParthenaMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~wnPitParthenaMediator() { }
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser*, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser*, s32 articleId);
};
FT_DOL_ARRAY_VECTOR(soArticle*, 1);
typedef wnArticleManageModuleBuilder<wnPitParthenaMediator, 1> wnPitParthenaArticleManageModuleBuilder;

// Module typedefs certain from the weak destructor map names: status <2, work <3,2,1>, 2>, motion <3, Impl, 0, 1>,
// visibility <Impl, 1>, anim command sub <1,2,0,0, 1,3,0,0, 1,3,0,0, 1,3,0,0>; the null modules (ground, posture, hit,
// damage) and counts of the constructor's vectors (model 5, link 5, motion transition 4 terms, status transition 6).
// HYPOTHESIS: the rest follows the sibling launcher (F.L.U.D.D.).
class wnPitParthenaModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 1,
                                      soTransitionModuleBuildConfig<soTypeList<soIntToType<4>, soTypeListNullType> >,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<3, 2, 1>, 2,
                                      soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnPitParthenaArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 2, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

class wnPitParthena : public wnWeaponBuilder<wnPitParthenaModuleAccesserBuildConfig> {
    const wnPitParthenaParam* m_param;
    wnPitParthenaParamAccesser m_paramAccesser;
public:
    wnPitParthena(s32 articleId, const wnArticleBuildInfo& info, const wnPitParthenaData* data);
#ifdef WN_PIT_PARTHENA_INLINE_DTOR
    // BYTES: wn_pit_parthena.o carries the dtor's @64@/@136@/@144@ thunks, which MWCC only emits
    // here when the dtor body is visible; the out-of-line copy lives in ft_pit.cpp.
    virtual ~wnPitParthena() { }
#else
    virtual ~wnPitParthena();
#endif
    virtual void updateNodeSRT();
    void activate(int founderTaskId, int resourceId, int team, float scale);
    static bool activeArticle(wnPitIkarus* weapon, soModuleAccesser* acc);
};
#endif
