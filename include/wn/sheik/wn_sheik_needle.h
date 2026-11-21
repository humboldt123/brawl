#pragma once

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// HYPOTHESIS: the article data type the class info fetches for kind 68; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnSheikNeedleData;
struct wnArticleBuildInfo;

// HYPOTHESIS: meaning of the parameters; the accesser maps unk8/unkC to its floats and unk0/unk4 to its
// ints, and unkC is the flight speed of an activated or reflected needle.
struct wnSheikNeedleParam {
    s32 unk0;
    s32 unk4;
    float unk8;
    float unkC;
};

class wnSheikNeedleParamAccesser : public wnExtendParamAccesser<3999, 2, 23999, 2, 0, 0> {
public:
    explicit wnSheikNeedleParamAccesser(const wnSheikNeedleParam* param) :
        wnExtendParamAccesser<3999, 2, 23999, 2, 0, 0>(68) {
        m_floats[0] = &param->unk8;
        m_floats[1] = &param->unkC;
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk4;
    }
    virtual ~wnSheikNeedleParamAccesser() { }
};
static_assert(sizeof(wnSheikNeedleParamAccesser) == 0x38, "Sheik needle parameter accesser extent");

#if defined(WN_WEAPON_BUILDER) && defined(WN_SHEIK_NEEDLE_UNIT) // the article unit (other units see the call-side view below)
// BYTES: the sora_melee unit mode of the module accesser builder is shared with the Light Bow (wn/zelda/wn_zelda_lighting_bow.h).
#define WN_ZELDA_LIGHTING_BOW_UNIT
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/wn_anim_cmd_full_property.h>
#include <wn/wn_inside_event_all_units.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_kinetic_energy_rot_normal.h>

class wnSheikNeedleModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<12, 20, 0, 1, 4, 0, 4, 4, 0, 0, 1, 0, 0, 0, 0, 0, 0, 6, 0, wnAllInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnAllInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<5, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 4, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<4, soGeneralWorkBuildConfig<2, 2, 1>, 4, soTransitionModuleBuildConfig<soTypeList<soIntToType<2>, soTypeListNullType> > >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 4, 0, 0, 2, 1, 5, 0, 0, 2, 1, 5, 0, 0, 2, 1, 5, 0, 0, 2> AnimCmdModuleSubBuildConfig;
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
};

template <>
class soModuleAccesserBuilder<wnSheikNeedleModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnSheikNeedleModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    __declspec(weak) soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};

class wnSheikNeedle : public wnWeaponBuilder<wnSheikNeedleModuleAccesserBuildConfig> {
public:
    const wnSheikNeedleParam* m_param;
private:
    wnSheikNeedleParamAccesser m_paramAccesser;
public:
    wnSheikNeedle(s32 articleId, const wnArticleBuildInfo& info, const wnSheikNeedleData* data);
    virtual ~wnSheikNeedle() { }
    virtual void updateNodeSRT();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    // HYPOTHESIS: parameter meanings from ftSheikTransactor::activeArticle1 (flag = work flag 0x22000012,
    // situation = the fighter's situation kind, lr = the fighter's facing).
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* pos, bool flag, int situation, float lr);
};
#else
// Extent known from the class info createArray allocation (0x1BD0); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnSheikNeedle : public Weapon {
    u8 m_storage[0x1B94 - sizeof(Weapon)];
    const wnSheikNeedleParam* m_param;
    wnSheikNeedleParamAccesser m_paramAccesser;
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnSheikNeedle(s32 articleId, const wnArticleBuildInfo& info, const wnSheikNeedleData* data);
    virtual ~wnSheikNeedle();
    virtual void updateNodeSRT();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    // HYPOTHESIS: parameter meanings from ftSheikTransactor::activeArticle1 (flag = work flag 0x22000012,
    // situation = the fighter's situation kind, lr = the fighter's facing).
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* pos, bool flag, int situation, float lr);
};
#endif
static_assert(sizeof(wnSheikNeedle) == 0x1BD0, "Sheik needle article extent");
