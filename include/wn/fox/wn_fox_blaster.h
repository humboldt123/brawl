#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 30; only passed as a pointer, the
// layout is unreconstructed (mirror of the bullet family's data role).
struct wnFoxBlasterData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1D7C); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
#if defined(WN_WEAPON_BUILDER) && defined(FT_FOX_ARTICLE_POOLS)
// The fighter unit owns the complete article (its destructor is emitted there). Layout from the destructor's member
// offsets: no kinetic, ground, collision-attack, reflect or glow builders, a color blend module, one other-animation
// slot, three model event observers, and the common four command threads (map names of the out-of-line units).
#include <wn/wn_extend_param_accesser.h>
class wnFoxBlasterModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 3, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1,
                                      soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    // HYPOTHESIS: color blend counts (the destructor only shows the module).
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soReflectModuleBuildConfigNull ReflectModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
};

// HYPOTHESIS: one float parameter (the accesser extent is 0x2C); its kind id is set by the native constructor.
struct wnFoxBlasterParam;
class wnFoxBlasterParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    virtual ~wnFoxBlasterParamAccesser() { }
};

// The Blaster's status builder ends without the tail word (as the Illusion's in this unit); its teardown is out of line.
template <>
class soStatusModuleBuilder<wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > > {
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<soGeneralWorkBuildConfig<2, 2, 1> > m_generalWorkBuilder;
    soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > m_transitionBuilder;
    soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 1>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > m_uniqProcs;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_changeRequests;
    soStatusModuleImpl m_module;
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* statusData, void* preCheckData) :
        m_statusTable(statusData, 1),
        m_generalWorkBuilder(),
        m_transitionBuilder(),
        m_uniqProcs(),
        m_changeRequests(-1),
        m_module(acc, &m_statusTable, (soArray<soStatusUniqProcess*>*)m_uniqProcs.get(), m_generalWorkBuilder.getModule(),
                 m_transitionBuilder.getModule(), &m_changeRequests, preCheckData, 1, 1) { }
    // BYTES: the Blaster destructor calls this teardown out of line (the weak copy at 0x3CB8).
    ~soStatusModuleBuilder() __attribute__((never_inline)) { }
    soStatusModule* getModule() { return &m_module; }
};

// The Blaster has no kinetic module, so its builder adds none of the common energies.
template <>
class wnWeaponBuilder<wnFoxBlasterModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnFoxBlasterModuleAccesserBuildConfig> m_modules;
    // HYPOTHESIS: as in the Illusion's builder, the native module storage ends one word later than the generic builder.
    u32 m_modulesPad;
public:
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() {
        wnDeactivateArticleDescendants(this->m_modules.m_generateArticleBuilder);
    }
};

class wnFoxBlaster : public wnWeaponBuilder<wnFoxBlasterModuleAccesserBuildConfig> {
    const wnFoxBlasterParam* m_param;
    wnFoxBlasterParamAccesser m_paramAccesser;
#else
class wnFoxBlaster : public Weapon {
    u8 m_storage[0x1D7C - sizeof(Weapon)];
#endif
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnFoxBlaster(s32 articleId, const wnArticleBuildInfo& info, const wnFoxBlasterData* data);
    // HYPOTHESIS: parameter types follow ftFoxTransactor::activeArticle1, which forwards its model and texture
    // resource ids and two flags; the founder task id comes first and the team last.
    void activate(int founderTaskId, u32 mdlResId, u32 texResId, bool unk1, bool unk2, int team);
    virtual void notifyEventLink(soLinkEventArgs* event, soModuleAccesser* moduleAccesser, StageObject* sender,
                                 int selector);
#if defined(WN_WEAPON_BUILDER) && defined(FT_FOX_ARTICLE_POOLS)
    virtual ~wnFoxBlaster() { }
#else
    virtual ~wnFoxBlaster();
#endif
};
static_assert(sizeof(wnFoxBlaster) == 0x1D7C, "Fox Blaster article extent");
