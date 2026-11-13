#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Pikachu Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftPikachuInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftPikachuHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftPikachuParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftPikachuResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftPikachuModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<288, 486> ftPikachuAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<486, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftPikachuMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftPikachuCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftPikachuLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<288, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftPikachuStatusModuleBuildConfig;

class ftKineticEnergyController;

// Modes 0x64..0x6B. Each case keeps its own dead false flag (a volatile so the store survives).
// Only Quick Attack aim (0x64) on the controller energy calls updateEnergySub; the specialization is in the fighter TU.
template <int Mode, typename Energy>
struct ftPikachuEnergyModeUpdate {
    static void update(Energy* energy, soModuleAccesser* acc) {
        volatile bool dead = false;
        energy->updateEnergy(acc);
    }
};

template <>
struct ftPikachuEnergyModeUpdate<0x64, ftKineticEnergyController> {
    static void update(ftKineticEnergyController* energy, soModuleAccesser* acc);
};

// HYPOTHESIS: this fighter has its own kinetic transactor (updateEnergy and changeKinetic)
class ftPikachuKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    // The Quick Attack aiming mode (0x64): the controller energy only accelerates with the stick (see updateEnergySub).
    static void changeKineticSub(bool* result, void* pools, soModuleAccesser* acc);
    // HYPOTHESIS: sets the speed of the energy from the regulated stick position.
    static void updateEnergySub(soKineticEnergyNormal* energy, void* unused, soModuleAccesser* acc);
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc) {
        // Ordinary kinetic types update the energy when it is enabled and not suspended.
        if (acc->getKineticModule().getKineticType() <= 0x63) {
            if (energy->isEnable() != true) {
                return;
            }
            if (energy->isSuspend()) {
                return;
            }
            energy->updateEnergy(acc);
            return;
        }
        // 0x64..0x6B are Pikachu's own modes (Quick Attack and the ones changeKinetic switches on).
        switch (acc->getKineticModule().getKineticType()) {
        case 0x64: ftPikachuEnergyModeUpdate<0x64, E>::update(energy, acc); break;
        case 0x65: ftPikachuEnergyModeUpdate<0x65, E>::update(energy, acc); break;
        case 0x66: ftPikachuEnergyModeUpdate<0x66, E>::update(energy, acc); break;
        case 0x67: ftPikachuEnergyModeUpdate<0x67, E>::update(energy, acc); break;
        case 0x68: ftPikachuEnergyModeUpdate<0x68, E>::update(energy, acc); break;
        case 0x69: ftPikachuEnergyModeUpdate<0x69, E>::update(energy, acc); break;
        case 0x6a: ftPikachuEnergyModeUpdate<0x6a, E>::update(energy, acc); break;
        case 0x6b: ftPikachuEnergyModeUpdate<0x6b, E>::update(energy, acc); break;
        }
    }
};
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftPikachuKineticTransactor> > ftPikachuKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the article pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_PIKACHU_ARTICLE_POOLS
// These observer capacities live in sora_melee. Declared before the article types so the event builder calls them.
template <> class soEventUnitWithWorkArea<soStatusEventObserver, 16> : public soEventUnitImpl<soStatusEventObserver> {
    soInstanceManagerFullPropertyVector<soStatusEventObserver*, 16> m_observerList;
public:
    soEventUnitWithWorkArea(s16, s16);
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const;
};
template <> class soEventUnitWithWorkArea<soAnimCmdEventObserver, 32> : public soEventUnitImpl<soAnimCmdEventObserver> {
    soInstanceManagerFullPropertyVector<soAnimCmdEventObserver*, 32> m_observerList;
public:
    soEventUnitWithWorkArea(s16, s16);
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const;
};
template <> class soEventUnitWithWorkArea<soLinkEventObserver, 2> : public soEventUnitImpl<soLinkEventObserver> {
    soInstanceManagerFullPropertyVector<soLinkEventObserver*, 2> m_observerList;
public:
    soEventUnitWithWorkArea(s16, s16);
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const;
};
template <> class soEventUnitWithWorkArea<soArticleEventObserver, 2> : public soEventUnitImpl<soArticleEventObserver> {
    soInstanceManagerFullPropertyVector<soArticleEventObserver*, 2> m_observerList;
public:
    soEventUnitWithWorkArea(s16, s16);
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const;
};
#include <ft/pikachu/ft_pikachu_article_pools.h>
typedef ftPikachuArticleManageModuleBuilder ftPikachuGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x22AE8, Fighter_Pikachu> ftPikachuGenerateArticleManageModuleBuilder;
#endif

class ftPikachuBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftPikachuInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftPikachuHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftPikachuParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftPikachuResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftPikachuAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftPikachuModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftPikachuMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftPikachuCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftPikachuLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftPikachuStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftPikachuKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftPikachuGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class soCameraModule;
class wnPikachuThunderJolt;
class wnPikachuCloud;
class wnPikachuVortex;
class wnPikachuThunder;

class ftPikachu : public ftFighterBuilder<ftPikachuBuildConfig> {
    // The builder already ends at the status table. A zero-length tail is a flexible array, and MWCC then skips this member's destructor.
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
public:
    ftPikachu(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual ~ftPikachu();
    virtual void onActivate();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    void** getExtendParam();
    static void* getThunderParam();
    void rec_cloud_pointer(wnPikachuCloud* cloud);
    // The cloud article of Thunder is kept in a work int of the fighter (HYPOTHESIS: names follow the module map).
    void notify_thunder_reflect();
    void adjust_camera_subject(soCameraModule* source);
    static wnPikachuCloud* get_cloud_pointer(soModuleAccesser* acc);
    static void rec_cloud_pointer1(soModuleAccesser* acc, wnPikachuCloud* cloud);
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnPikachuThunderJolt* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnPikachuCloud* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnPikachuVortex* weapon, soModuleAccesser* acc);
    static bool activeArticle3(wnPikachuThunder* weapon, soModuleAccesser* acc);
};
