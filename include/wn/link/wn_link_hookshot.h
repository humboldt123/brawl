#pragma once

// Link's hookshot (article kind 4, the Clawshot special). Only the members the fighter calls are declared so far; the
// article itself (src/mo_fighter/ft_link/wn_link_hookshot.cpp) is not reconstructed yet.

#include <wn/weapon.h>

#ifndef WN_LINK_HOOKSHOT_FULL
class wnLinkHookshot : public Weapon {
public:
    // HYPOTHESIS: parameter names beyond the founder, resource and team; the position is the node the hook starts at and the
    // flag is set while Link is in one of the hookshot statuses.
    void activate(int founderTaskId, int resourceId, int team, float lr, const Vec3f& position, bool unk);
};
#endif

#ifdef WN_LINK_HOOKSHOT_FULL
// The complete hookshot, only compiled where its builders are wanted (src/mo_fighter/ft_link/wn_link_hookshot.cpp). The
// fighter unit keeps the incomplete class above.
#include <wn/wn_article_pool.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_simple.h>

// The parameter block (kind 110): three floats and two ints, ids 4000.. and 24000..
// HYPOTHESIS: meanings; the layout follows the pointer table the constructor builds.
struct wnLinkHookshotParam {
    float unk0;
    s32 unk4;
    s32 unk8;
    float unkC;
    float unk10;
};

struct wnLinkHookshotData {
    s32 m_resGroupNo; // The constructor reads the low byte of this first word.
    u8 unk4[0x30];
    const wnLinkHookshotParam* m_param; // +0x34
};

class wnLinkHookshotParamAccesser : public wnExtendParamAccesser<3999, 3, 23999, 2, 0, 0> {
public:
    explicit wnLinkHookshotParamAccesser(const wnLinkHookshotParam* param) :
        wnExtendParamAccesser<3999, 3, 23999, 2, 0, 0>(110) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unkC;
        m_floats[2] = &param->unk10;
        m_ints[0] = &param->unk4;
        m_ints[1] = &param->unk8;
    }
    virtual ~wnLinkHookshotParamAccesser() { }
};

// The hookshot owns one simple article (the claw tip), as Jigglypuff's costume accessory does.
// HYPOTHESIS: the original pool classes are soInstancePool / soLineHierarchy / soArticleMediatorImpl ... over a type list;
// these copies of the generic pool of wn/wn_article_pool.h carry a name that contains the original template name.
class wnInstanceHolderHookshot : public soInstancePoolRoot {
public:
    virtual ~wnInstanceHolderHookshot();
private:
    wnSimple m_instance;
public:
    wnInstanceHolderHookshot(soModuleAccesser* acc);
    wnSimple* getInstance() { return &m_instance; }
};
class soInstancePoolSubHookshot : public soInstancePoolRoot {
public:
    virtual ~soInstancePoolSubHookshot();
private:
    u32 m_terminal;
    wnInstanceHolderHookshot m_holder;
public:
    soInstancePoolSubHookshot(soModuleAccesser* acc) : soInstancePoolRoot(acc), m_holder(acc) { }
    wnSimple* getInstanceAt(s32 index);
    wnSimple* getInstance() { return m_holder.getInstance(); }
};
class soInstancePoolHookshot : public soInstancePoolRoot {
public:
    virtual ~soInstancePoolHookshot() { }
private:
    soInstancePoolSubHookshot m_sub;
public:
    typedef wnSimple Article;
    enum { Num = 1 };
    soInstancePoolHookshot(soModuleAccesser* acc) : soInstancePoolRoot(acc), m_sub(acc) { }
    soInstancePoolSubHookshot& getSub() { return m_sub; }
    template <class Predicate, int N>
    wnSimple* find(Predicate& predicate, const soIntToType<N>&) {
        if (predicate(&static_cast<soArticle&>(*m_sub.getInstance())) == true) {
            return m_sub.getInstance();
        }
        return NULL;
    }
};
class soLineHierarchyHookshot : public soInstancePoolHookshot {
public:
    soLineHierarchyHookshot(soModuleAccesser* acc) : soInstancePoolHookshot(acc) { }
    virtual ~soLineHierarchyHookshot() { }
};
class soArticleMediatorImplHookshot : public soArticleMediator {
    soLineHierarchyHookshot m_pools;
    u8 m_autoRecycle;
    WN_ARTICLE_SLOT_FRIENDS
public:
    soArticleMediatorImplHookshot(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~soArticleMediatorImplHookshot() { }
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser*, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser*, s32 articleId);
};
class soSelectInstanceHolderHookshot {
    soArticleMediatorImplHookshot m_mediator;
public:
    soSelectInstanceHolderHookshot(soModuleAccesser* acc) : m_mediator(acc) { }
    ~soSelectInstanceHolderHookshot();
    soArticleMediator* getMediator() { return &m_mediator; }
};
FT_DOL_ARRAY_VECTOR(soArticle*, 1);
class soGenerateArticleManageModuleBuilderHookshot {
    soArrayVector<soArticle*, 1> m_articles;
    soArrayVector<soArticleEventObserver, 1> m_observers;
    soSelectInstanceHolderHookshot m_mediator;
    soGenerateArticleManageModuleImpl m_module;
public:
    soGenerateArticleManageModuleBuilderHookshot(soModuleAccesser* acc) :
        m_articles(0), m_observers(1, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
    ~soGenerateArticleManageModuleBuilderHookshot();
    void* getModule() { return &m_module; }
    soArticleMediator* getMediator() { return m_mediator.getMediator(); }
};
typedef soGenerateArticleManageModuleBuilderHookshot wnLinkHookshotArticleManageModuleBuilder;

class wnLinkHookshotModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soPhysicsModuleBuildConfigCap<0, soPhysicsModuleImpl> PhysicsModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<7, 6, 1> GeneralWorkBuildConfig;
    typedef wnStatusModuleBuildConfig<9, soGeneralWorkBuildConfig<3, 3, 1>, 9, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnLinkHookshotArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 9, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

class wnLinkHookshot : public wnWeaponBuilder<wnLinkHookshotModuleAccesserBuildConfig> {
    const wnLinkHookshotParam* m_param;
    wnLinkHookshotParamAccesser m_paramAccesser;
public:
    wnLinkHookshot(s32 articleId, const wnArticleBuildInfo& info, const wnLinkHookshotData* data);
#ifdef FT_LINK_HOOKSHOT_DTOR_OUT_OF_LINE
    // ft_link.o owns the plain destructor; the hookshot unit keeps the inline one.
    virtual ~wnLinkHookshot();
#else
    virtual ~wnLinkHookshot() { }
#endif
    void activate(int founderTaskId, int resourceId, int team, float lr, const Vec3f& position, bool unk);
    static bool activeArticle(wnSimple* weapon, soModuleAccesser* acc);
};
#endif
