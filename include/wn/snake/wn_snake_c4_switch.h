#pragma once

// Snake's C4 detonator (article kind 0x19): the remote switch Snake holds up to set off his C4 charges. It has no
// motion, collision, kinetic or animation-command modules of its own, only one status.

#include <wn/wn_weapon_builder.h>
#include <wn/snake/wn_snake_status_builder.h>

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnSnakeC4SwitchData {
    s32 m_resGroupNo;
};

class wnSnakeC4SwitchModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfigNull MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfigNull VisibilityModuleBuildConfig;
    typedef soAnimCmdModuleBuildConfigNull AnimCmdModuleBuildConfig;
    typedef wnSnakeOneStatusBuildConfig StatusModuleBuildConfig; // no tail word (wn_snake_status_builder.h)
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigNoUniq<0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

// The builder of this article carries no appended common energies (what WN_WEAPON_NO_COMMON_ENERGIES selects for a
// whole unit). Specialized for this config, as the Mario cape does.
template <>
class wnWeaponBuilder<wnSnakeC4SwitchModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnSnakeC4SwitchModuleAccesserBuildConfig> m_modules;
    // HYPOTHESIS: trailing alignment of the animation-command storage, as in the no-common-energies builder.
    u32 m_modulesPad;
public:
    // A member template so its body (and the module builders it instantiates) is only analyzed where the article is
    // built: ft_snake.cpp owns this article but instantiates the shared builders itself, later (ft_builder_noinline.h).
    template <class BuildData>
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const BuildData& bd,
                    bool weaponFlag = true) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, this->m_modules.getModuleAccesser(), weaponFlag),
        m_modules(this, bd) {
        Weapon::initialize();
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() { wnDeactivateArticleDescendants(m_modules.m_generateArticleBuilder); }
};

class wnSnakeC4Switch : public wnWeaponBuilder<wnSnakeC4SwitchModuleAccesserBuildConfig> {
public:
    wnSnakeC4Switch(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeC4SwitchData* data);
    virtual ~wnSnakeC4Switch() { }
    void activate(int founderTaskId, int etcResourceId, int team);
};

static_assert(sizeof(wnSnakeC4Switch) == 0x1364, "C4 switch article layout");
