#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 35; only passed as a pointer, the
// layout is unreconstructed (mirror of the bullet family's data role).
struct wnFalcoBlasterData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1D88).
#if defined(WN_WEAPON_BUILDER) && defined(FT_FALCO_ARTICLE_POOLS)
// The fighter unit owns the complete article (its destructor is emitted there). The destructor has the shape of Fox's
// Blaster (wn/fox/wn_fox_blaster.h) with every member 0xC later: the inside-event builder keeps four model observers
// (the configuration of the bullet and the afterimage), and its teardown is called out of line.
#include <wn/wn_extend_param_accesser.h>
class wnFalcoBlasterModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
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

// HYPOTHESIS: one float parameter (the accesser extent is 0x2C), as in Fox's Blaster.
struct wnFalcoBlasterParam;
class wnFalcoBlasterParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    virtual ~wnFalcoBlasterParamAccesser() { }
};

// The Blaster's status builder ends without the tail word (as the Illusion's); its teardown is out of line.
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
    // BYTES: the Blaster destructor calls this teardown out of line (a weak copy of this unit).
    ~soStatusModuleBuilder() __attribute__((never_inline)) { }
    soStatusModule* getModule() { return &m_module; }
};

// The Blaster has no kinetic module, so its builder adds none of the common energies.
template <>
class wnWeaponBuilder<wnFalcoBlasterModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnFalcoBlasterModuleAccesserBuildConfig> m_modules;
    // HYPOTHESIS: as in the Illusion's builder, the native module storage ends one word later than the generic builder.
    u32 m_modulesPad;
public:
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() {
        wnDeactivateArticleDescendants(this->m_modules.m_generateArticleBuilder);
    }
};

class wnFalcoBlaster : public wnWeaponBuilder<wnFalcoBlasterModuleAccesserBuildConfig> {
    const wnFalcoBlasterParam* m_param;
    wnFalcoBlasterParamAccesser m_paramAccesser;
#else
class wnFalcoBlaster : public Weapon {
    u8 m_storage[0x1D88 - sizeof(Weapon)];
#endif
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnFalcoBlaster(s32 articleId, const wnArticleBuildInfo& info, const wnFalcoBlasterData* data);
    // HYPOTHESIS: parameter types follow ftFalcoTransactor::activeArticle1, which forwards its model and texture
    // resource ids and two flags; the founder task id comes first and the team last.
    void activate(int founderTaskId, u32 mdlResId, u32 texResId, bool unk1, bool unk2, int team);
    virtual void notifyEventLink(soLinkEventArgs* event, soModuleAccesser* moduleAccesser, StageObject* sender,
                                 int selector);
#if defined(WN_WEAPON_BUILDER) && defined(FT_FALCO_ARTICLE_POOLS)
    virtual ~wnFalcoBlaster() { }
#else
    virtual ~wnFalcoBlaster();
#endif
};
static_assert(sizeof(wnFalcoBlaster) == 0x1D88, "Falco Blaster article extent");
