#pragma once

// Wolf's Phantasm afterimage is Fox's article under another name: the original object builds it from Fox's module
// accesser configuration (wnModuleAccesserBuilder<wnFoxIllusionModuleAccesserBuildConfig>) but names its weapon builder
// after its own configuration, so Wolf gets a distinct config tag and a weapon builder over Fox's modules.
// HYPOTHESIS: all class names and the data/param layouts, taken to be the same as Fox's.

#include <wn/fox/wn_fox_illusion.h>

struct wnWolfIllusionParam {
    s32 unk0;
    s32 unk4;
};

struct wnWolfIllusionData {
    s32 m_resGroupNo; // The constructor reads the low byte of this first word.
    u8 unk4[0x30];    // HYPOTHESIS: uninterpreted data-block bytes before m_param.
    const wnWolfIllusionParam* m_param; // +0x34
};

class wnWolfIllusionParamAccesser : public wnExtendParamAccesser<3999, 0, 23999, 2, 0, 0> {
public:
    explicit wnWolfIllusionParamAccesser(const wnWolfIllusionParam* param) :
        wnExtendParamAccesser<3999, 0, 23999, 2, 0, 0>(40) { // HYPOTHESIS: kind
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk4;
    }
    virtual ~wnWolfIllusionParamAccesser() { }
};

class wnWolfIllusionModuleAccesserBuildConfig : public wnFoxIllusionModuleAccesserBuildConfig { };

// The weapon builder carries Wolf's name but is made of Fox's modules.
template <>
struct wnWeaponBuilderModules<wnWolfIllusionModuleAccesserBuildConfig> {
    typedef wnFoxIllusionModuleAccesserBuildConfig Config;
};

#if defined(WN_WEAPON_BUILDER) && defined(FT_WOLF_ARTICLE_POOLS)
// The fighter unit specializes the builder as Fox's does (wn/fox/wn_fox_illusion.h): Fox's modules, a tail word, and a
// member-template constructor so the module builders are instantiated in the illusion unit and not here.
template <>
class wnWeaponBuilder<wnWolfIllusionModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnFoxIllusionModuleAccesserBuildConfig> m_modules;
    u32 m_modulesPad;
public:
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

class wnWolfIllusion : public wnWeaponBuilder<wnWolfIllusionModuleAccesserBuildConfig> {
    const wnWolfIllusionParam* m_param;
    wnWolfIllusionParamAccesser m_paramAccesser;
public:
    wnWolfIllusion(s32 articleId, const wnArticleBuildInfo& info, const wnWolfIllusionData* data);
    virtual ~wnWolfIllusion() { }
    void activate(s32 founderTaskId, s32 unk1, s32 team, u8 situationKind, float lr, bool followUp);
    virtual void processUpdate();
    virtual bool notifyEventCollisionAttackCheck(u32 unk0);
};
#ifndef FT_WOLF_ARTICLE_POOLS // the fighter unit keeps the tail word and common energies (wn/fox/wn_fox_illusion.h)
static_assert(sizeof(wnWolfIllusion) == 0x1DE4, "Wolf Illusion layout is wrong!");
#endif
