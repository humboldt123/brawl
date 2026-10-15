#pragma once

// Pikachu's Final Smash vortex article (article kind 0x7F).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

WN_STATUS_PROC_VECTOR_DEFAULT_CTOR(3);

// The parameter block of the article (8 floats, id 4000).
// HYPOTHESIS: meaning of the parameters.
struct wnPikachuVortexParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    s32 unk20;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnPikachuVortexData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPikachuVortexParam* m_param; // +0x34
    u8 unk38[8];                         // HYPOTHESIS: a sub-block whose address the build data keeps
};

class wnPikachuVortexParamAccesser : public wnExtendParamAccesser<3999, 8, 23999, 1, 0, 0> {
public:
    wnPikachuVortexParamAccesser(const wnPikachuVortexParam* param) : wnExtendParamAccesser<3999, 8, 23999, 1, 0, 0>(0x7F) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->unk10;
        m_floats[5] = &param->unk14;
        m_floats[6] = &param->unk18;
        m_floats[7] = &param->unk1C;
        m_ints[0] = &param->unk20;
    }
    virtual ~wnPikachuVortexParamAccesser() { }
};

class wnPikachuVortexModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<8, 6, 1> GeneralWorkBuildConfig;
    typedef soCameraModuleBuildConfig<soCameraModuleImpl> CameraModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

// The helpers shared by the article and its status processes work on the module accesser of the vortex.
// HYPOTHESIS: work int 0x10000005 / 0x10000006 hold the handles of the two looks of the vortex effect.
// HYPOTHESIS: work floats 0x11000001/2 are the speed of the vortex, 0x11000003 the target and 0x11000004 the current
// follow speed, 0x11000005 the follow acceleration.
void wnPikachuVortexSetEffect(soModuleAccesser* moduleAccesser, u32 index, EfID effectId);
void wnPikachuVortexRemoveEffect(soModuleAccesser* moduleAccesser, u32 index);
void wnPikachuVortexSetEffectLook(soModuleAccesser* moduleAccesser, int look);

class wnPikachuVortex : public wnWeaponBuilder<wnPikachuVortexModuleAccesserBuildConfig> {
public:
    const wnPikachuVortexParam* m_param; // +0x1F78
private:
    wnPikachuVortexParamAccesser m_paramAccesser; // +0x1F7C
public:
    wnPikachuVortex(s32 articleId, const wnArticleBuildInfo& info, const wnPikachuVortexData* data);
    virtual ~wnPikachuVortex() { }
    virtual void onDeactivate();
    virtual void updateNodeSRT();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    // HYPOTHESIS: the second parameter is unused by the activation.
    void activate(int founderTaskId, int unused, int team, const Vec3f* position);
};
