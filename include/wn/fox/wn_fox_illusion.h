#pragma once

#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_weapon_builder.h>

// The three status-process slots call the sora_melee-owned vector constructor.
template <>
class soArrayVector<soStatusUniqProcess*, 3> : public soArrayVectorAbstract<soStatusUniqProcess*> {
    s32 m_topIndex : sizeof(bit_width<3>) + 1;
    s32 m_lastIndex : sizeof(bit_width<3>) + 1;
    s32 m_size : sizeof(bit_width<3>) + 1;
    u32 m_isFull : 1;
    soStatusUniqProcess* m_elements[3];
public:
    soArrayVector(s32);
    virtual s32 size() const;
    virtual ~soArrayVector();
    virtual s32 capacity() const;
    virtual bool isFull() const;
    virtual soStatusUniqProcess*& atFastAbstractSub(s32 index) const;
    virtual soStatusUniqProcess*& getArrayValueConst(s32 index);
    virtual s32 getTopIndex() const;
    virtual s32 getLastIndex() const;
    virtual void setSize(s32 size);
    virtual void setTopIndex(s32 topIndex);
    virtual void setLastIndex(s32 lastIndex);
    virtual void onFull();
    virtual void offFull();
};
template <>
class soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 3>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > {
public:
    soArrayVector<soStatusUniqProcess*, 3> m_array;
    soArraySelectHolder() : m_array(0) { }
    ~soArraySelectHolder() { }
    soArrayVector<soStatusUniqProcess*, 3>* get() { return &m_array; }
};

struct wnFoxIllusionParam {
    s32 unk0;
    s32 unk4;
};

struct wnFoxIllusionData {
    s32 m_resGroupNo; // Native constructor reads the low byte of this first word.
    u8 unk4[0x30]; // HYPOTHESIS: uninterpreted data-block bytes before m_param.
    const wnFoxIllusionParam* m_param; // +0x34, loaded by the native constructor.
};

class wnFoxIllusionParamAccesser : public wnExtendParamAccesser<3999, 0, 23999, 2, 0, 0> {
public:
    explicit wnFoxIllusionParamAccesser(const wnFoxIllusionParam* param) :
        wnExtendParamAccesser<3999, 0, 23999, 2, 0, 0>(32) {
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk4;
    }
    virtual ~wnFoxIllusionParamAccesser() { }
};
// Accessor extent 0x30 is HYPOTHESIS until confirmed by the target getter bodies and sibling layout.

class wnFoxIllusionModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                  wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soCollisionReflectorModuleBuildConfigNull CollisionReflectorModuleBuildConfig;
    typedef soReflectModuleBuildConfigNull ReflectModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3,
                                     soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0>
        AnimCmdModuleSubBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig; // no movement energies
    // Glow and the remaining null/simple defaults are inherited from wnCommonBuildConfig.
};

// The illusion unit builds this article with WN_STATUS_NO_TAIL and WN_WEAPON_NO_COMMON_ENERGIES unit-wide. Fox's fighter
// unit (FT_FOX_ARTICLE_POOLS) keeps the tail word and the common energies for its other articles, so it specializes just
// this config. The article unit itself does not define FT_FOX_ARTICLE_POOLS. Wolf's fighter unit builds its afterimage
// from this same config (wn/wolf/wn_wolf_illusion.h) and opts in with FT_WOLF_ARTICLE_POOLS.
#if defined(WN_WEAPON_BUILDER) && (defined(FT_FOX_ARTICLE_POOLS) || defined(FT_WOLF_ARTICLE_POOLS))
template <>
class soStatusModuleBuilder<wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > > {
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<soGeneralWorkBuildConfig<2, 2, 1> > m_generalWorkBuilder;
    soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > m_transitionBuilder;
    soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 3>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > m_uniqProcs;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_changeRequests;
    soStatusModuleImpl m_module;
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* statusData, void* preCheckData) :
        m_statusTable(statusData, 3),
        m_generalWorkBuilder(),
        m_transitionBuilder(),
        m_uniqProcs(),
        m_changeRequests(-1),
        m_module(acc, &m_statusTable, (soArray<soStatusUniqProcess*>*)m_uniqProcs.get(), m_generalWorkBuilder.getModule(),
                 m_transitionBuilder.getModule(), &m_changeRequests, preCheckData, 3, 1) { }
    soStatusModule* getModule() { return &m_module; }
};

template <>
class wnWeaponBuilder<wnFoxIllusionModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnFoxIllusionModuleAccesserBuildConfig> m_modules;
    u32 m_modulesPad;
public:
    // A member template so the module builders are instantiated in the illusion unit, which calls this, and not in the fighter.
    template <class BuildData>
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const BuildData& bd,
                    bool weaponFlag = true) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, this->m_modules.getModuleAccesser(), weaponFlag),
        m_modules(this, bd) {
        Weapon::initialize();
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() {
        wnDeactivateArticleDescendants(this->m_modules.m_generateArticleBuilder);
    }
};
#endif

class wnFoxIllusion : public wnWeaponBuilder<wnFoxIllusionModuleAccesserBuildConfig> {
    const wnFoxIllusionParam* m_param; // native constructor writes this at +0x1F6C.
    wnFoxIllusionParamAccesser m_paramAccesser;
public:
    wnFoxIllusion(s32 articleId, const wnArticleBuildInfo& info, const wnFoxIllusionData* data);
    virtual ~wnFoxIllusion() { }
    void activate(s32 founderTaskId, s32 unk1, s32 team, u8 situationKind, float lr, bool followUp);
    virtual void processUpdate();
    virtual bool notifyEventCollisionAttackCheck(u32 unk0);
};


static_assert(sizeof(wnWeaponBuilder<wnFoxIllusionModuleAccesserBuildConfig>) == 0x1DB0,
              "Fox Illusion builder extent");
