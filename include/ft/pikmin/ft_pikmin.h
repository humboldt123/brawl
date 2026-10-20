#pragma once

#include <ft/ft_fighter_builder.h>
#include <ft/builder/ft_dol_instances.h>
#include <wn/pikmin/wn_pikmin_pikmin_troops_manager.h>
#include <ft/pikmin/ft_pikmin_status_uniq_process.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Pikmin Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftPikminInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftPikminHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftPikminParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftPikminResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftPikminModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<284, 480> ftPikminAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<480, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftPikminMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftPikminCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<13, soLinkModuleImpl> ftPikminLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<284, soGeneralWorkBuildConfig<19, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftPikminStatusModuleBuildConfig;

// HYPOTHESIS: this fighter has its own kinetic transactor (changeKinetic; updateEnergy stays in ftKineticTransactor)
class ftPikminKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    static void changeKineticSub(bool* result, void* pools, Vec2f* speed, soModuleAccesser* acc);
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc) {
        if (energy->isEnable() != true) {
            return;
        }
        if (energy->isSuspend()) {
            return;
        }
        return energy->updateEnergy(acc);
    }
};
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftPikminKineticTransactor, ftKineticTransactor> > ftPikminKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the article pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_PIKMIN_ARTICLE_POOLS
#include <ft/pikmin/ft_pikmin_article_pools.h>
typedef ftPikminArticleManageModuleBuilder ftPikminGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x17878, Fighter_Pikmin> ftPikminGenerateArticleManageModuleBuilder;
#endif

class ftPikminBuildConfig : public ftCommonBuildConfig {
public:
    static const ftKind Kind = Fighter_Pikmin;
    typedef ftPikminInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftPikminHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftPikminParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftPikminResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftPikminAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftPikminModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftPikminMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftPikminCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftPikminLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftPikminStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftPikminKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftPikminGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnPikminPikmin;
class wnPikminDolfin;
class wnPikminChain;
struct ftData;

// The owner info array is a sora_melee instance (the fighter calls its constructor and destructor).
FT_DOL_ARRAY_VECTOR(wnPikminPikminOwnerInfo, 36);

class soDamage;
class soArticle;
class soCollisionLog;

// The line of Pikmin that follow Olimar is kept by the troops manager sub-object; the owner info array (the positions the
// Pikmin of the line walk to) sits behind it.
class ftPikmin : public ftFighterBuilder<ftPikminBuildConfig> {
    ftData* m_commonData; // 0x1FD04
    wnPikminPikminTroopsManager m_troops; // 0x1FD08
    // HYPOTHESIS: the original member is a soArrayVector of owner infos; the storage stays opaque until the constructor is reconstructed.
    soArrayVector<wnPikminPikminOwnerInfo, 36> m_owners; // 0x1FE8C
    // The status processes that are members of the fighter (they get the troops manager of the fighter).
    ftPikminStatusUniqProcessSpecialS m_specialS;                // 0x200D8
    ftPikminStatusUniqProcessAttackAir m_attackAir;              // 0x200E0
    ftPikminStatusUniqProcessAttackS4Start m_attackS4Start;      // 0x200E8
    ftPikminStatusUniqProcessAttackS4Hold m_attackS4Hold;        // 0x200F0
    ftPikminStatusUniqProcessAttackS4 m_attackS4;                // 0x200F8
    ftPikminStatusUniqProcessAttackHi4Start m_attackHi4Start;    // 0x20100
    ftPikminStatusUniqProcessAttackHi4Hold m_attackHi4Hold;      // 0x20108
    ftPikminStatusUniqProcessAttackHi4 m_attackHi4;              // 0x20110
    ftPikminStatusUniqProcessSpecialHi m_specialHi;              // 0x20118
    ftPikminStatusUniqProcessCatch m_catch;                      // 0x20120
    ftPikminStatusUniqProcessCatchWait m_catchWait;              // 0x20128
public:
    ftPikmin(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    soArrayVectorAbstract<wnPikminPikminOwnerInfo>& getOwnersVector() { return m_owners; }
    soArrayFixed<wnPikminPikminOwnerInfo>& getOwners() { return m_owners; }
    virtual ~ftPikmin();
    virtual void onActivate();
    virtual void onStart(int startKind);
    virtual void postStart(int startKind);
    virtual void processUpdate();
    virtual void processFixCamera();
    virtual void notifyArticleEventEject(int unk1, int unk2, int* unk3);
    virtual void notifyArticleEventRemove(int unk1, int* unk2);
    virtual bool notifyEventCaptureStatus(soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3);
    virtual void notifyEventOnDamage(soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    void addPikmin(wnPikminPikmin* pikmin);
    void reducePikminAll();
    wnPikminPikminTroopsManager& getTroops() { return m_troops; }
    void refreshOwnerInfoPos();
    void updatePikmin();
    void updateHoldPikmin();
    void reduceHideWaitPikmin(int status);
};
static_assert(sizeof(ftPikmin) == 0x20130, "ftPikmin size mismatch");
