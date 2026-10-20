#pragma once

// The star that Dedede spits out of his inhale (article kind 0x35): flies straight until it hits, can be reflected and turns
// into the Dedede star-missile status.

#include <wn/wn_weapon_builder.h>

// The parameter block of the article. HYPOTHESIS: meaning of the fields.
struct wnDededeStarmissileParam {
    float unk0;  // speed
    float unk4;  // rotation speed
    float unk8;
    float unkC;  // rhombus upper extent
    float unk10; // rhombus lower extent
    float unk14; // rhombus half width
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnDededeStarmissileData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnDededeStarmissileParam* m_param; // +0x34
};

class wnDededeStarmissileModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnDededeStarmissile : public wnWeaponBuilder<wnDededeStarmissileModuleAccesserBuildConfig> {
    const wnDededeStarmissileParam* m_param; // +0x1FB4
public:
    wnDededeStarmissile(s32 articleId, const wnArticleBuildInfo& info, const wnDededeStarmissileData* data);
    virtual ~wnDededeStarmissile() { }
    virtual void onDeactivate() { }
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    const wnDededeStarmissileData* getData();
    // HYPOTHESIS: parameter names beyond the position and facing. A nonzero mode keeps the previous speed.
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float unk, int unk2, int mode);
};
static_assert(sizeof(wnDededeStarmissile) == 0x1FB8, "Dedede star missile article extent");
