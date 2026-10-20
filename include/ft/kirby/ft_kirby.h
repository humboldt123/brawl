#pragma once

#include <ft/ft_fighter_builder.h>
#include <so/motion/so_motion_module_impl.h>
#include <sr/sr_common.h>
#include <so/link/so_link_event_presenter.h>
#include <types.h>

////////////////////////////////////////
// Kirby-only builder variants
////////////////////////////////////////

// HYPOTHESIS: soMotionModuleImplMultiRes is a soMotionModuleImpl with 8 more bytes of state (Kirby swaps motion resources
// for the copy abilities). Opaque stand-in: constructor and destructor are not defined here.
class soMotionModuleImplMultiRes : public soMotionModuleImpl {
    u8 unk170[8];
public:
    soMotionModuleImplMultiRes(soModuleAccesser* acc, soArrayFixed<const soMotionData>* motionData, soTransitionModule* transition, soArray<soPartialAnim>* partialAnims, soArray<soOtherAnim>* otherAnims, soArray<soTransitionTermPack>* termPacks, soArray<u32>* u32s, s16 unitId, bool b, soMotionAnimObjCacheModule* animObjCache);
    virtual ~soMotionModuleImplMultiRes();
};

// Kirby has a soCollisionSearchModuleBuilder (2 parts, 1 group) right after the collision catch builder; it is an own member of
// soModuleAccesserBuilder (see ftKirbyCollisionSearchModuleBuilder).
// MATCH-ONLY: the REL's catch builder constructor carries the search storage size (596) in its template arguments, so the catch
// builder keeps that spelling although the storage is not part of it.
template <typename T, u32 SearchSize>
class soCollisionCatchModuleBuildConfigWithSearch {
public:
    typedef T ModuleType;
};

template <typename T, u32 SearchSize>
class soCollisionCatchModuleBuilder<soCollisionCatchModuleBuildConfigWithSearch<T, SearchSize> > {
    soCollisionCatchModuleBuilder<soCollisionCatchModuleBuildConfig<T> > m_catchBuilder;
public:
    soCollisionCatchModuleBuilder(soModuleAccesser* acc, int taskId, gfTask::Category category, soEventObserverRegistrationDesc* regDesc) :
        m_catchBuilder(acc, taskId, category, regDesc) { }
    T* getModule() { return m_catchBuilder.getModule(); }
};

template <s32 CapA, s32 CapB, typename T>
class soItemManageModuleBuildConfigCaps {
public:
    typedef T ModuleType;
};

template <s32 CapA, s32 CapB, typename T>
class soItemManageModuleBuilder<soItemManageModuleBuildConfigCaps<CapA, CapB, T> > : public soArraySelectHolder<1, soArrayVector<soItemInfo, CapA>, soArrayNull<soItemInfo> > {
    soArraySelectHolder<1, soArrayVector<soItemInfo, CapB>, soArrayNull<soItemInfo> > m_items2;
    T m_itemModule;
public:
    soItemManageModuleBuilder(soModuleAccesser* acc, void* itemNodeData) :
        soArraySelectHolder<1, soArrayVector<soItemInfo, CapA>, soArrayNull<soItemInfo> >(CapA, 0), m_items2(), m_itemModule(acc, this->get(), m_items2.get(), itemNodeData, g_soItemManageNullA, g_soItemManageNullB) { }
    T* getModule() { return &m_itemModule; }
};

// Area module builder with another size (STUB: storage only, like soAreaModuleBuilder).
template <u32 Size, typename T>
class soAreaModuleBuildConfigSized {
public:
    typedef T ModuleType;
};

template <u32 Size, typename T>
class soAreaModuleBuilder<soAreaModuleBuildConfigSized<Size, T> > {
    u8 m_data[Size];
public:
    ~soAreaModuleBuilder() { m_data[1] = 1; m_data[2] = 2; m_data[3] = 3; m_data[4] = 4; m_data[5] = 5; } // STUB: non-trivial so the dtor call exists
    soAreaModuleBuilder(soModuleAccesser* acc, u8 areaCategory, soEventObserverRegistrationDesc* regDesc) { m_data[0] = 0; m_data[1] = 1; m_data[2] = 2; m_data[3] = 3; m_data[4] = 4; m_data[5] = 5; m_data[6] = 6; m_data[7] = 7; } // STUB
    void* getModule() { return m_data + 0x10; }
};

////////////////////////////////////////
// Kirby Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftKirbyInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftKirbyHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftKirbyParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftKirbyResourceIdAccesserImpl, soResourceModuleImpl> ftKirbyResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImplVariable> ftKirbyModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<448, 797> ftKirbyAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<797, soMotionModuleImplMultiRes, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftKirbyMotionModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 9, 3, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftKirbyCollisionReflectorModuleBuildConfig;
typedef soCollisionCatchModuleBuildConfigWithSearch<soCollisionCatchModuleImpl, 0x254> ftKirbyCollisionCatchModuleBuildConfig;
typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfigParts<2, soCollisionSearchModuleImpl> > ftKirbyCollisionSearchModuleBuilder;
typedef soLinkModuleBuildConfigCap<8, soLinkModuleImpl> ftKirbyLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<448, soGeneralWorkBuildConfig<35, 18, 17>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftKirbyStatusModuleBuildConfig;
typedef soAreaModuleBuildConfigSized<0x3E0, soAreaModuleImpl> ftKirbyAreaModuleBuildConfig;
typedef soPhysicsModuleBuildConfigCap<0, soPhysicsModuleImpl> ftKirbyPhysicsModuleBuildConfig;
typedef soItemManageModuleBuildConfigCaps<7, 4, soItemManageModuleImpl> ftKirbyItemManageModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (wnKirbyHammer and the copy ability pools), not reconstructed yet
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x12970, Fighter_Kirby> ftKirbyGenerateArticleManageModuleBuilder;

class ftKirbyBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftKirbyInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftKirbyHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftKirbyParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftKirbyResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftKirbyAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftKirbyModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftKirbyMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftKirbyCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftKirbyCollisionCatchModuleBuildConfig CollisionCatchModuleBuildConfig;
    typedef ftKirbyCollisionSearchModuleBuilder CollisionSearchModuleBuilder;
    typedef ftKirbyLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftKirbyStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftKirbyAreaModuleBuildConfig AreaModuleBuildConfig;
    typedef ftKirbyPhysicsModuleBuildConfig PhysicsModuleBuildConfig;
    typedef ftKirbyItemManageModuleBuildConfig ItemManageModuleBuildConfig;
    typedef ftKirbyGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

// Link events Kirby sends to the articles he owns (ids 0x838-0x83a).
struct ftKirbyLinkEvent : soLinkEventArgs {
    ftKirbyLinkEvent(int kind) : soLinkEventArgs(kind) { }
};

// HYPOTHESIS: the link events the articles of the copy abilities send to Kirby (layout from the reads of notifyEventLink):
// a result word (the node the article reports back), then either two floats or a flag.
struct ftKirbyLinkEventIn : soLinkEventArgs {
    u32 m_result;
    int unkC;
    union {
        struct {
            float m_x;
            float m_y;
        };
        u8 m_flag;
    };
};

// HYPOTHESIS: entry of the Final Ray table in the fighter data (stage kind, limit) and the query of the check.
struct ftKirbyFinalRayEntry {
    int stageKind;
    float limit;
};
struct ftKirbyFinalRayQuery {
    int unk0;
    float limit;
};

// HYPOTHESIS: 16 byte records with an empty constructor (two arrays of four follow the status table).
struct ftKirbyUnk10 {
    u8 unk0[0x10];
    ftKirbyUnk10() { }
};

// Kirby's copy ability state (map: ftKirbyCopyAbilityModule); only the members the fighter calls are declared, their bodies are
// still the REL's own (not yet reconstructed).
class soStatusData;
class soCollisionLog;
class ftKirbyCopyAbilityModule {
    u8 unk0[0xB64];
public:
    void changeCopyAbility(int copyKind, int playerNo, bool spycloak, int unk);
    void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind);
    void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    void notifyEventCollisionShield();
    bool notifyEventCollisionShieldCheck();
    void notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    bool notifyEventCollisionAttackCheck();
    void notifyEventOnDamage();
    void processUpdate();
    void processCollision();
    void processFixPosition();
    void notifyArticleEventEject();
    void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    bool notifyEventCollisionSearchCheck();
    void prepareCopyAbility(int copyKind);
    void disposeCopyAbility(int copyKind);
};

class ftKirby : public ftFighterBuilder<ftKirbyBuildConfig> {
    ftData* m_data;                                              // +0x1B678
    soArrayContractibleTable<const soStatusData> m_statusTable;  // +0x1B67C
    u8 unk1B68C;
    u8 unk1B68D;
    u8 unk1B68E[0x12];
    struct Unk1B6A0 {
        ftKirbyUnk10 first[4];
        ftKirbyUnk10 second[4];
    } unk1B6A0;
    u8 unk1B720[0xC];
    ftKirbyCopyAbilityModule m_copyAbility;                      // +0x1B72C
    Heaps::HeapType m_nwModelHeap;                               // +0x1C290
public:
    ftKirby(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);

    // ---- fighter overrides (vtable slots in the order of the Kirby vtable) ----
    virtual void processFixPosition();
    virtual void onDeactivate();
    virtual void onStart(int);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
    virtual void processCollision();
    virtual void processUpdate();
    virtual bool isObserv(char kind);
    virtual void notifyArticleEventEject(int unk1, int unk2, int* unk3);
    virtual void notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4);
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionSearchCheck();
    virtual void notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionShieldCheck();
    virtual void notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionReflectorCheck();
    virtual void notifyEventOnDamage(soDamage* damage, bool reaction, soModuleAccesser* moduleAccesser);

    void* getExtendParam();
    u8* getWorkStone();
    void decisionStoneMetamorphosis();
    void copyCancel(soModuleAccesser* moduleAccesser, bool unk);
    void copyCancelDamage(soModuleAccesser* moduleAccesser);
    void copyCancelDamageNoReaction(soModuleAccesser* moduleAccesser);
    void updateShineEye();
    void copySetup(soModuleAccesser* moduleAccesser, int slotNo, int copyKind, bool effect);
    void copyCancelTransitionStatus();
    int getCopyKind();
    int getCopySlotNo();
    bool isCopyStatus();
    bool checkKirbyFinalRayCheck(ftKirbyFinalRayQuery* query);
    // (overrides of the ftEntryEventObserver events: they get their own slots in the primary vtable)
    virtual void notifyEventKirbyResourceLoaded(int index);
    virtual void notifyEventKirbyResourceUnLoaded(int index);
    virtual void notifyEventExitFighter(int slotNo, int copyKind);
};
static_assert(sizeof(ftFighterBuilder<ftKirbyBuildConfig>) == 0x1B678, "Kirby data offset is wrong");
static_assert(sizeof(ftKirby) == 0x1C294, "Kirby allocation size is wrong");
