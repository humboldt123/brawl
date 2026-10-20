#pragma once

#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>
#include <so/turn/so_turn_module.h>
#include <ft/ft_common_data_accesser.h>

////////////////////////////////////////
// Fox Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftFoxInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftFoxHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftFoxParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftFoxResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<10, 3, soModelModuleImpl> ftFoxModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<288, 498> ftFoxAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<498, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftFoxMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftFoxCollisionShieldModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 15, 3, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftFoxCollisionReflectorModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<7, soLinkModuleImpl> ftFoxLinkModuleBuildConfig;
typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 13, 1, soCollisionHitModuleImpl, 0x3ff, true> ftFoxCollisionHitModuleBuildConfig;
typedef soStatusModuleBuildConfig<288, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftFoxStatusModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (weapon pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_FOX_ARTICLE_POOLS
#include <ft/fox/ft_fox_article_pools.h>
typedef ftFoxArticleManageModuleBuilder ftFoxGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x14CD0, Fighter_Fox> ftFoxGenerateArticleManageModuleBuilder;
#endif

class ftFoxBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftFoxInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftFoxHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftFoxParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftFoxResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftFoxAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftFoxModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftFoxMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftFoxCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftFoxCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftFoxLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftFoxCollisionHitModuleBuildConfig CollisionHitModuleBuildConfig;
    typedef ftFoxStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftFoxGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnFoxBlaster;
class wnFoxIllusion;
class wnFoxLandMaster;
// Only the behavior-accessed tail is recovered (the turn record of the reflector).
// HYPOTHESIS: the record grouping extends ftData, as for Wolf.
struct ftFoxData : ftData {
    u8 unk58[0x94 - sizeof(ftData)];
    soTurnData reflectorTurnData;
};
static_assert(sizeof(ftFoxData) == 0x9c, "Reflector turn data offset is wrong!");

// The constructor's 0x1CD98 allocation fixes the size; the tail is the common data pointer, the status table, the two
// reflector flags and the trail of the last positions (the Phantasm afterimage).
class ftFox : public ftFighterBuilder<ftFoxBuildConfig> {
public:
    // One sample of the afterimage trail: the position and a rotation of the fighter. HYPOTHESIS: names.
    class PostureInfo {
    public:
        Vec3f m_pos;
        float m_rotX;
    };
public:
    ftFoxData* m_commonData; // +0x1CD30
private:
    soArrayContractibleTable<const soStatusData> m_statusTable; // +0x1CD34
public:
    // Set by the reflector collision callbacks for one frame: the hit was too weak (unk1CD44) or was stopped (unk1CD45).
    u8 unk1CD44;
    u8 unk1CD45;
private:
    soArrayQueueImpl<soArrayVector<PostureInfo, 4> > m_postureTrail; // +0x1CD48
public:
    ftFox(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual ~ftFox();
    virtual void processUpdate();
    virtual void onStart(int param);
    virtual void processFixPosition();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual void notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionReflectorCheck();
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnFoxBlaster* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnFoxIllusion* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnFoxLandMaster* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftFighterBuilder<ftFoxBuildConfig>) == 0x1CD30, "Fox status table offset is wrong");
static_assert(sizeof(ftFox) == 0x1CD98, "Fox allocation size is wrong!");
