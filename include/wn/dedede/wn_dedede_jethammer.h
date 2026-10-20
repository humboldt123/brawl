#pragma once

// The Jet Hammer article of Dedede (article kind 0x32): the hammer model that Dedede carries, driven by link events
// (fighter animation commands select its status 0-9).

#include <wn/wn_weapon_builder.h>

WN_STATUS_PROC_VECTOR_ARG_CTOR(10);

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnDededeJethammerData {
    s32 m_resGroupNo;
};

class wnDededeJethammerModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                  wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soPostureModuleBuildConfigNull PostureModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<14, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<10, soGeneralWorkBuildConfig<4, 2, 7>, 10, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 10, 0, 0, 1, 14, 0, 0, 1, 14, 0, 0, 1, 14, 0, 0>
        AnimCmdModuleSubBuildConfig;
};

// The hammer has no kinetic module, so its builder adds no kinetic energies (the generic builder adds a movement and a
// rotation energy).
template <>
class wnWeaponBuilder<wnDededeJethammerModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnDededeJethammerModuleAccesserBuildConfig> m_modules; // +0xCC
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd,
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

// The status process shared by the hammer's statuses (wn_dedede_jethammer_status_uniq_process.cpp): it spawns the hammer's
// effects when the status is entered.
class wnDededeJetHammerStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnDededeJetHammerStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};
extern wnDededeJetHammerStatusUniqProcess g_wnDededeJetHammerStatusUniqProcess; // bss 0x47C

class wnDededeJethammer : public wnWeaponBuilder<wnDededeJethammerModuleAccesserBuildConfig> {
public:
    wnDededeJethammer(s32 articleId, const wnArticleBuildInfo& info, const wnDededeJethammerData* data);
    virtual ~wnDededeJethammer() { }
    void activate(int founderTaskId, int modelResourceId, float lr);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
};
