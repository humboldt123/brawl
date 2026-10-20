#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Popo Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftPopoInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftPopoHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftPopoParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftPopoResourceIdAccesserImpl, soResourceModuleImpl> ftPopoResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftPopoModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<289, 498> ftPopoAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<498, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftPopoMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftPopoCollisionShieldModuleBuildConfig;
typedef soGeneralWorkBuildConfig<66, 34, 3> ftPopoGeneralWorkBuildConfig;
typedef soLinkModuleBuildConfigCap<7, soLinkModuleImpl> ftPopoLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<289, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftPopoStatusModuleBuildConfig;

// HYPOTHESIS: this fighter has its own kinetic transactor (changeKinetic is in a later translation unit)
// Popo's own updateEnergy<E>: with the ordinary kinetic types (below 100) the energy is updated if it is enabled and not suspended;
// the Ice Climbers specific types (100-114, HYPOTHESIS: the Belay / Squall Hammer / Blizzard movement types) dispatch per type.
// Types 101-104 of the gravity and controller energies call a type specific update that lives in sora_melee, and a few more
// (controller 110, stop 112-114) get their own update that also receives a one byte tag object.
// MATCH-ONLY: the original builds one volatile marker byte per case on the stack (as in Sonic's transactor).
struct ftPopoKineticTag {
    u8 m_value;
    ftPopoKineticTag* clear() { m_value = 0; return this; }
};
class ftKineticEnergyGravity;
class ftKineticEnergyController;
class ftKineticEnergyStop;
extern "C" {
void fn_27_347C5C(ftKineticEnergyGravity* energy, soModuleAccesser* acc);
void fn_27_3478AC(ftKineticEnergyController* energy, soModuleAccesser* acc);
void fn_27_347F90(ftKineticEnergyController* energy, ftPopoKineticTag* tag, soModuleAccesser* acc);
void fn_27_34819C(ftKineticEnergyStop* energy, ftPopoKineticTag* tag, soModuleAccesser* acc);
void fn_27_3482C8(ftKineticEnergyStop* energy, ftPopoKineticTag* tag, soModuleAccesser* acc);
void fn_27_348450(ftKineticEnergyStop* energy, ftPopoKineticTag* tag, soModuleAccesser* acc);
}

template <class E, int Type>
struct ftPopoKineticUpdate {
    enum { special = 0 };
    static void run(E* energy, soModuleAccesser* acc) { energy->updateEnergy(acc); }
};
#define FT_POPO_KINETIC_SPECIAL(E, Type, Fn)                                                                   \
    template <>                                                                                                \
    struct ftPopoKineticUpdate<E, Type> {                                                                      \
        enum { special = 1 };                                                                                  \
        static void run(E* energy, soModuleAccesser* acc) { ftPopoKineticTag tag; Fn(energy, tag.clear(), acc); } \
    }
FT_POPO_KINETIC_SPECIAL(ftKineticEnergyController, 110, fn_27_347F90);
FT_POPO_KINETIC_SPECIAL(ftKineticEnergyStop, 112, fn_27_34819C);
FT_POPO_KINETIC_SPECIAL(ftKineticEnergyStop, 113, fn_27_3482C8);
FT_POPO_KINETIC_SPECIAL(ftKineticEnergyStop, 114, fn_27_348450);

template <class E>
struct ftPopoKineticCommon {
    static void run(E* energy, soModuleAccesser* acc) { energy->updateEnergy(acc); }
};
template <>
struct ftPopoKineticCommon<ftKineticEnergyGravity> {
    static void run(ftKineticEnergyGravity* energy, soModuleAccesser* acc) { fn_27_347C5C(energy, acc); }
};
template <>
struct ftPopoKineticCommon<ftKineticEnergyController> {
    static void run(ftKineticEnergyController* energy, soModuleAccesser* acc) { fn_27_3478AC(energy, acc); }
};

#define FT_POPO_KINETIC_CASE(n) \
    case n: { \
        if (ftPopoKineticUpdate<E, n>::special) { \
            ftPopoKineticUpdate<E, n>::run(energy, acc); \
        } else { \
            volatile u8 marker = 0; \
            ftPopoKineticUpdate<E, n>::run(energy, acc); \
        } \
        break; \
    }
class ftPopoKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc) {
        int type = acc->getKineticModule().getKineticType();
        if (type <= 99) {
            if (energy->isEnable() == true && !energy->isSuspend()) {
                energy->updateEnergy(acc);
            }
        } else if (energy->isEnable()) {
            if (energy->isSuspend() == true) {
            } else if (static_cast<unsigned int>(type - 101) <= 3) {
                ftPopoKineticCommon<E>::run(energy, acc);
            } else {
                switch (acc->getKineticModule().getKineticType()) {
                    FT_POPO_KINETIC_CASE(100)
                    FT_POPO_KINETIC_CASE(101)
                    FT_POPO_KINETIC_CASE(102)
                    FT_POPO_KINETIC_CASE(103)
                    FT_POPO_KINETIC_CASE(104)
                    FT_POPO_KINETIC_CASE(105)
                    FT_POPO_KINETIC_CASE(106)
                    FT_POPO_KINETIC_CASE(107)
                    FT_POPO_KINETIC_CASE(108)
                    FT_POPO_KINETIC_CASE(109)
                    FT_POPO_KINETIC_CASE(110)
                    FT_POPO_KINETIC_CASE(111)
                    FT_POPO_KINETIC_CASE(112)
                    FT_POPO_KINETIC_CASE(113)
                    FT_POPO_KINETIC_CASE(114)
                }
            }
        }
    }
};
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftPopoKineticTransactor> > ftPopoKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the article pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_POPO_ARTICLE_POOLS
#include <ft/iceclimber/ft_iceclimber_article_pools.h>
typedef ftPopoArticleManageModuleBuilder ftPopoGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x17ACC, Fighter_Popo> ftPopoGenerateArticleManageModuleBuilder;
#endif

class ftPopoBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftPopoInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftPopoHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftPopoParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftPopoResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftPopoAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftPopoModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftPopoMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftPopoCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftPopoGeneralWorkBuildConfig GeneralWorkBuildConfig;
    typedef ftPopoLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftPopoStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftPopoKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftPopoGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnPopoBlizzard;
class wnPopoIceShot;
class wnPopoIceBerg;
class wnPopoRubber;
class wnPopoIceBergWind;

class ftPopo : public ftFighterBuilder<ftPopoBuildConfig> {
    u8 unkTail[0x1FE38 - sizeof(ftFighterBuilder<ftPopoBuildConfig>)];
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
    bool m_isPartner; // HYPOTHESIS: set for the second fighter of the pair (fighter number 1), whose input and owner are the sub ones
    u8 unkTail2[0x1FE50 - 0x1FE4D];
public:
    ftPopo(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual void processUpdate();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void postCreate();
    virtual bool notifyUseItem(BaseItem* item, u32 nodeIndex, int* unk);
    virtual void setCursor(bool enable);
    virtual void setNameCursor(bool enable);
    virtual void setLoupe(bool enable);
    virtual void setLoupeDamage(bool enable);
    virtual void onActivate();
    virtual ftOwner* getOwner();
    virtual Fighter* getPartner();
    virtual Input* getInput();
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4);
    virtual void notifyEventChangeMotion(int motionKind, int unk2, void* unk3, soModuleAccesser* moduleAccesser);
    virtual void onDeadEnd();
    virtual void onStart(int startKind);
    virtual void toKnockOut();
    virtual void analyzeSeal(void* seal);
    virtual bool checkTransitionStatus(u32 statusKind);
    virtual void adjustParentGroundCollision(int unk1, float* unk2);
    static bool isModelSwapColor(int color);
    static bool isEnableCoupleTech(int status);
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnPopoBlizzard* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnPopoIceBerg* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnPopoRubber* weapon, soModuleAccesser* acc);
};
