#pragma once

#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Falco Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftFalcoInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftFalcoHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftFalcoParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftFalcoResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<10, 3, soModelModuleImpl> ftFalcoModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<285, 492> ftFalcoAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<492, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftFalcoMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftFalcoCollisionShieldModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 15, 3, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftFalcoCollisionReflectorModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<7, soLinkModuleImpl> ftFalcoLinkModuleBuildConfig;
typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 13, 1, soCollisionHitModuleImpl, 0x3ff, true> ftFalcoCollisionHitModuleBuildConfig;
typedef soStatusModuleBuildConfig<285, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftFalcoStatusModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (weapon pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_FALCO_ARTICLE_POOLS
#include <ft/falco/ft_falco_article_pools.h>
typedef ftFalcoArticleManageModuleBuilder ftFalcoGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x15040, Fighter_Falco> ftFalcoGenerateArticleManageModuleBuilder;
#endif

class ftFalcoBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftFalcoInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftFalcoHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftFalcoParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftFalcoResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftFalcoAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftFalcoModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftFalcoMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftFalcoCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftFalcoCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftFalcoLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftFalcoCollisionHitModuleBuildConfig CollisionHitModuleBuildConfig;
    typedef ftFalcoStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftFalcoGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnFalcoBlaster;
class wnFalcoIllusion;
class wnFoxLandMaster;
struct ftData;

// The constructor's 0x1D0E4 allocation fixes the size; the tail is the status table, the common data pointer, the two
// reflector flags and the trail of the last positions (the Phantasm afterimage).
class ftFalco : public ftFighterBuilder<ftFalcoBuildConfig> {
public:
    // One sample of the afterimage trail: the position and a rotation of the fighter. HYPOTHESIS: names.
    class PostureInfo {
    public:
        Vec3f m_pos;
        float m_rotX;
    };
private:
    soArrayContractibleTable<const soStatusData> m_statusTable; // +0x1D07C
    ftData* m_commonData; // +0x1D08C
public:
    // Set by the reflector collision callbacks for one frame: the hit was too weak (unk1D090) or was stopped (unk1D091).
    u8 unk1D090;
    u8 unk1D091;
private:
    soArrayQueueImpl<soArrayVector<PostureInfo, 4> > m_postureTrail; // +0x1D094
public:
    ftFalco(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual ~ftFalco();
    virtual void processUpdate();
    virtual void onStart(int param);
    virtual void processFixPosition();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual void notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionReflectorCheck();
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnFalcoBlaster* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnFalcoIllusion* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnFoxLandMaster* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftFighterBuilder<ftFalcoBuildConfig>) == 0x1D07C, "Falco status table offset is wrong");
static_assert(sizeof(ftFalco) == 0x1D0E4, "Falco allocation size is wrong!");
