#define SO_KINETIC_ROTATION_FIELDWISE_ZERO
#define FT_ARTICLE_MEDIATOR_DTOR_INLINE
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PIKACHU_ARTICLE_POOLS
#define FT_PIKACHU_SELECT_HOLDER_NAME
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define FT_PIKACHU_WEAPON_MOTION_DTOR
#define WN_ARTICLE_BUILDER_DTORS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#define FT_WEAK_KEPT_OBSERVERS
// The article headers define the rotational-energy destructor inline. This unit emits it out of line.
#define FT_ROT_ENERGY_DTOR_OUT_OF_LINE
// The cloud's command-thread builder destructor is a separate function in this unit.
#define FT_ANIMCMD_SUB_DTOR_NOINLINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/fighter.h>
#include <ft/ft_owner.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/pikachu/ft_pikachu.h>
#include <ft/pikachu/ft_pikachu_status_uniq_process.h>
#include <ft/pikachu/ft_pikachu_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <ft/pikachu/ft_pikachu_transactor.h>
#include <so/so_value_accesser.h>
#include <so/so_value_accesser.h>
#include <ft/ft_common_data_accesser.h>
#include <wn/pikachu/wn_pikachu_thunder_jolt2.h>
#include <ft/ft_kinetic_energy_controller.h>

// Quick Attack aim: the controller energy takes the stick speed. The other modes keep a dead flag and update normally.
// Defined before the builder instantiations so the call inlines into updateEnergy.
inline void ftPikachuEnergyModeUpdate<0x64, ftKineticEnergyController>::update(ftKineticEnergyController* energy, soModuleAccesser* acc) {
    volatile bool dead = false;
    bool passed = false;
    ftPikachuKineticTransactor::updateEnergySub(energy, &passed, acc);
}

// ftManager::setParamPattern selects the shared parameter-table variation (same accessor as ft_marth.cpp).
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

#define FT_BC ftPikachuBuildConfig
#undef WN_ARTICLE_NATIVE_BUILDERS
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

extern "C" {
extern soStatusUniqProcess lbl_27_bss_7C64;
}

ftPikachuExtendParamAccesser g_ftPikachuExtendParamAccesser;

ftClassInfoImpl<Fighter_Pikachu, ftPikachu> g_ftClassInfoPikachu;

ftPikachu::ftPikachu(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPikachuBuildConfig>(entryId,
                                         Fighter_Pikachu,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Pikachu)) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[14];
    processes[0] = &lbl_27_bss_7C64;
    processes[1] = &g_ftPikachuStatusUniqProcessSpecialSkullBash;
    processes[2] = &g_ftPikachuStatusUniqProcessSpecialQuickAttackStart;
    processes[3] = &g_ftPikachuStatusUniqProcessSpecialThunder;
    processes[4] = &g_ftPikachuStatusUniqProcessSpecialVortexStart;
    processes[5] = &g_ftPikachuStatusUniqProcessSpecialQuickAttackWarp;
    processes[6] = &g_ftPikachuStatusUniqProcessSpecialQuickAttackEnd;
    processes[7] = &g_ftPikachuStatusUniqProcessSpecialSkullBashHold;
    processes[8] = &g_ftPikachuStatusUniqProcessSpecialSkullBashWeak;
    processes[9] = &g_ftPikachuStatusUniqProcessSpecialSkullBashAttack;
    processes[10] = &g_ftPikachuStatusUniqProcessSpecialSkullBashEnd;
    processes[11] = &g_ftPikachuStatusUniqProcessSpecialThunderHit;
    processes[12] = &g_ftPikachuStatusUniqProcessSpecialVortexAttack;
    processes[13] = &g_ftPikachuStatusUniqProcessSpecialVortexEnd;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 14);
}

// The status-data table lives past the builder tail; its destructor is the sora_melee function.
ftPikachu::~ftPikachu() { }

// The Thunder Jolt starts through the transactor of the fighter with the effect resource of the fighter.
// BYTES: the native function addresses the local static's guard, object and destructor registration separately
// instead of from one pooled BSS base. The setting is per function: unit-wide it breaks the pooled constructor and
// __sinit, and an in-class inline definition makes the static weak and moves it to the end of .bss.
#pragma pool_data off
bool ftPikachu::activeArticle(wnPikachuThunderJolt* weapon, soModuleAccesser* acc) {
    static ftPikachuTransactor instance;
    return instance.activeArticle(weapon, acc, acc->getResourceModule().getResourceIdAccesser()->getEtcResId());
}
#pragma pool_data reset

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Pikachu, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftPikachuArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftPikachuArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftPikachuArticleKind : wnArticleKindInfo {
    ftPikachuArticleKind() {
        kind = 0;
        subKind = Fighter_Pikachu;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftPikachuArticleBuildInfo : wnArticleBuildInfo {
    ftPikachuArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_148050(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1480CC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftPikachuArticleTraits;
#define FT_PIKACHU_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftPikachuArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftPikachuArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Pikachu, &tag.m_resourceGroup)); \
        } \
    };
#define FT_PIKACHU_TRAITS_MEMBER(W, DATA, ID, GETTER) \
    template <> struct ftPikachuArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftPikachuArticleDataTag tag; \
            return static_cast<const DATA*>(g_ftCommonDataAccesser.GETTER(Fighter_Pikachu, &tag.m_resourceGroup)); \
        } \
    };

FT_PIKACHU_TRAITS(wnPikachuThunderJolt, wnPikachuThunderJoltData, 0, fn_27_148050)
FT_PIKACHU_TRAITS_MEMBER(wnPikachuCloud, wnPikachuCloudData, 1, getWeaponData<wnPikachuCloudData>)
FT_PIKACHU_TRAITS_MEMBER(wnPikachuVortex, wnPikachuVortexData, 2, getWeaponData<wnPikachuVortexData>)
FT_PIKACHU_TRAITS(wnPikachuThunder, wnPikachuThunderData, 3, fn_27_1480CC)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftPikachuArticleTraits<W>::ArticleId, ftPikachuArticleBuildInfo(ftPikachuArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftPikachuArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnPikachuThunderJolt>::~wnArticleHolder();
template wnArticleHolder<wnPikachuCloud>::~wnArticleHolder();
template wnArticleHolder<wnPikachuVortex>::~wnArticleHolder();
template wnArticleHolder<wnPikachuThunder>::~wnArticleHolder();
template <class Mediator>
soSelectInstanceHolderArticle<Mediator>::~soSelectInstanceHolderArticle() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template soSelectInstanceHolderArticle<ftPikachuArticleMediator>::~soSelectInstanceHolderArticle();
template wnArticleManageModuleBuilder<ftPikachuArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftPikachuArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
// The module accesser builders of the articles are destroyed out of line by the article destructors.
soSlowModuleBuilder<soSlowModuleBuildConfig<soSlowModuleSimple> >::~soSlowModuleBuilder() { }
soGlowModuleBuilder<soGlowModuleBuildConfig<soGlowModuleSimple> >::~soGlowModuleBuilder() { }
template <class T> void ftBuilderDtorUse(T* p) { p->~T(); }
typedef soStopModuleBuilder<soStopModuleBuildConfig<soStopModuleImpl> > ftPikachuArticleStopBuilder;
template void ftBuilderDtorUse<ftPikachuArticleStopBuilder>(ftPikachuArticleStopBuilder*);
typedef soReflectModuleBuilder<soReflectModuleBuildConfig<0, soReflectModuleImpl> > ftPikachuArticleReflectBuilder;
template void ftBuilderDtorUse<ftPikachuArticleReflectBuilder>(ftPikachuArticleReflectBuilder*);
typedef soEffectModuleBuilder<wnEffectModuleBuildConfig<1, 1, 0, 1, -1, soEffectModuleImpl> > ftPikachuArticleEffectBuilder;
template void ftBuilderDtorUse<ftPikachuArticleEffectBuilder>(ftPikachuArticleEffectBuilder*);
typedef wnAnimCmdModuleSubBuilder<wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> > ftPikachuArticleAnimCmdSubBuilder;
template void ftBuilderDtorUse<ftPikachuArticleAnimCmdSubBuilder>(ftPikachuArticleAnimCmdSubBuilder*);
typedef soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfigUniq<4, 8> > ftPikachuArticleAnimCmdUniqBuilder;
template void ftBuilderDtorUse<ftPikachuArticleAnimCmdUniqBuilder>(ftPikachuArticleAnimCmdUniqBuilder*);
// Weapon-module teardowns the thunder article calls out of line.
typedef soGeneralWorkBuilder<soGeneralWorkBuildConfig<2, 2, 1> > ftPikachuWeaponWorkBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponWorkBuilder>(ftPikachuWeaponWorkBuilder*);
typedef soVisibilityModuleBuilder<soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> > ftPikachuWeaponVisibilityBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponVisibilityBuilder>(ftPikachuWeaponVisibilityBuilder*);
typedef soShakeModuleBuilder<soShakeModuleBuildConfig<0, soShakeModuleSimple> > ftPikachuWeaponShakeBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponShakeBuilder>(ftPikachuWeaponShakeBuilder*);
typedef soTeamModuleBuilder<soTeamModuleBuildConfig<soTeamImpl, soTeamModuleImpl> > ftPikachuWeaponTeamBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponTeamBuilder>(ftPikachuWeaponTeamBuilder*);
typedef soAnimCmdModuleBuilder<soAnimCmdModuleBuildConfig<6, soAnimCmdModuleImpl> > ftPikachuWeaponAnimCmdBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponAnimCmdBuilder>(ftPikachuWeaponAnimCmdBuilder*);
typedef soModelModuleBuilder<soModelModuleBuildConfig<5, 0, soModelModuleImpl> > ftPikachuWeaponModelBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponModelBuilder>(ftPikachuWeaponModelBuilder*);
typedef soLinkModuleBuilder<soLinkModuleBuildConfigCap<5, soLinkModuleImpl> > ftPikachuWeaponLinkBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponLinkBuilder>(ftPikachuWeaponLinkBuilder*);
typedef soKineticModuleBuilder<soKineticModuleBuildConfigMediator<wnKineticModuleImpl, soKineticMediatorNull> > ftPikachuWeaponKineticBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponKineticBuilder>(ftPikachuWeaponKineticBuilder*);
typedef soSoundModuleBuilder<wnSoundModuleBuildConfig<0, soSound3dGeneratorAccesserImpl, soSoundModuleImpl> > ftPikachuWeaponSoundBuilder;
typedef soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > ftPikachuWeaponStatusTransitionBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponStatusTransitionBuilder>(ftPikachuWeaponStatusTransitionBuilder*);
typedef soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnMotionTransitionTypeList> > ftPikachuWeaponMotionTransitionBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponMotionTransitionBuilder>(ftPikachuWeaponMotionTransitionBuilder*);
// The one-status article builder's teardown is its own function. The primary template inlines it into the caller.
template <>
class soStatusModuleBuilder<wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > > {
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<soGeneralWorkBuildConfig<2, 2, 1> > m_generalWorkBuilder;
    soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > m_transitionBuilder;
    soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 1>, soArrayNull<soStatusUniqProcess*> > m_uniqProcs;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_changeRequests;
    soStatusModuleImpl m_module;
public:
    ~soStatusModuleBuilder();
};
typedef soStatusModuleBuilder<wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > > ftPikachuWeaponStatusBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponStatusBuilder>(ftPikachuWeaponStatusBuilder*);
typedef soMotionModuleBuilder<soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > > ftPikachuWeaponMotionBuilder;
template <u32 DataNum, typename T, typename TransitionConfig>
__declspec(weak) soMotionModuleBuilder<soMotionModuleBuildConfig<DataNum, T, 0, 0, TransitionConfig, soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > >::~soMotionModuleBuilder() { }
template void ftBuilderDtorUse<ftPikachuWeaponMotionBuilder>(ftPikachuWeaponMotionBuilder*);
typedef soResourceModuleBuilder<soResourceModuleBuildConfig<0, soResourceIdAccesserImpl, soResourceModuleImpl> > ftPikachuWeaponResourceBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponResourceBuilder>(ftPikachuWeaponResourceBuilder*);
typedef soCollisionAttackModuleBuilder<soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true> > ftPikachuWeaponAttackBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponAttackBuilder>(ftPikachuWeaponAttackBuilder*);
typedef soInsideEventManageModuleBuilder<soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>, wnInsideEventManageModuleTypes> ftPikachuWeaponInsideEventBuilder;
template void ftBuilderDtorUse<ftPikachuWeaponInsideEventBuilder>(ftPikachuWeaponInsideEventBuilder*);
#pragma dont_inline off

// Instantiated with inlining enabled so the kind record is stored before the article data call.
template wnArticleHolder<wnPikachuThunderJolt>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnPikachuCloud>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnPikachuThunder>::wnArticleHolder(soModuleAccesser*);

// The article sound builder destroys only the module. The generator teardown is empty and has to inline.
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
template void ftBuilderDtorUse<ftPikachuWeaponSoundBuilder>(ftPikachuWeaponSoundBuilder*);

// Outside dont_inline so the process-array holder inlines and leaves the vector teardown call.
template <>
__declspec(weak) soStatusModuleBuilder<wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > >::~soStatusModuleBuilder() { }

// Outside dont_inline so the empty soArticleMediator base inlines and the pool base inlines to wnArticlePool.
ftPikachuArticleMediator::~ftPikachuArticleMediator() { }

// The original names these on soArticleMediatorImpl. Keep the bodies, and keep them weak so they are not a second global.
__declspec(weak) s32 ftPikachuArticleMediator::getMediateNum() { return 4; }
__declspec(weak) void ftPikachuArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// Empty out-of-line teardowns. The base destructors do the work.
ftKineticEnergyController::~ftKineticEnergyController() { }
soKineticEnergyRotNormal::~soKineticEnergyRotNormal() { }

// Thunder Jolt 2's pool lives in this unit (three instances, kind info <0, 7>, index 3).
// HYPOTHESIS: the parent of this level is the non-polymorphic pool root. The primary templates define these
// destructors inline, so each level is specialized here and the teardown is only the member destructors.
typedef soKindInfoGeneric<0, 7> ftPikachuJolt2Kind;
typedef soIntToType<3> ftPikachuJolt2Index;
typedef soInstancePoolInfo<wnPikachuThunderJolt2, 1, wnInstanceHolder, ftPikachuJolt2Index> ftPikachuJolt2Info1;
typedef soInstancePoolInfo<wnPikachuThunderJolt2, 2, wnInstanceHolder, ftPikachuJolt2Index> ftPikachuJolt2Info2;
typedef soInstancePoolInfo<wnPikachuThunderJolt2, 3, wnInstanceHolder, ftPikachuJolt2Index> ftPikachuJolt2Info3;

template <>
class wnInstanceHolder<wnPikachuThunderJolt2, ftPikachuJolt2Kind, ftPikachuJolt2Index> {
public:
    virtual ~wnInstanceHolder();
private:
    wnPikachuThunderJolt2 m_instance;
};

template <>
class soInstancePoolSub<ftPikachuJolt2Info1, ftPikachuJolt2Kind> {
public:
    virtual ~soInstancePoolSub();
private:
    soInstancePoolSubNull<wnPikachuThunderJolt2> m_next;
    wnInstanceHolder<wnPikachuThunderJolt2, ftPikachuJolt2Kind, ftPikachuJolt2Index> m_holder;
};

template <>
class soInstancePoolSub<ftPikachuJolt2Info2, ftPikachuJolt2Kind> {
public:
    virtual ~soInstancePoolSub();
private:
    soInstancePoolSub<ftPikachuJolt2Info1, ftPikachuJolt2Kind> m_next;
    wnInstanceHolder<wnPikachuThunderJolt2, ftPikachuJolt2Kind, ftPikachuJolt2Index> m_holder;
};

template <>
class soInstancePoolSub<ftPikachuJolt2Info3, ftPikachuJolt2Kind> {
public:
    virtual ~soInstancePoolSub();
private:
    soInstancePoolSub<ftPikachuJolt2Info2, ftPikachuJolt2Kind> m_next;
    wnInstanceHolder<wnPikachuThunderJolt2, ftPikachuJolt2Kind, ftPikachuJolt2Index> m_holder;
};

template <>
class soInstancePool<ftPikachuJolt2Info3, soInstancePoolRoot, ftPikachuJolt2Kind, 3> : public soInstancePoolRoot {
public:
    virtual ~soInstancePool();
private:
    soInstancePoolSub<ftPikachuJolt2Info3, ftPikachuJolt2Kind> m_sub;
};

// Weak: the primary pool templates also emit these teardowns. A strong copy here is a second global.
#pragma force_active on
__declspec(weak) wnInstanceHolder<wnPikachuThunderJolt2, ftPikachuJolt2Kind, ftPikachuJolt2Index>::~wnInstanceHolder() { }
__declspec(weak) soInstancePoolSub<ftPikachuJolt2Info1, ftPikachuJolt2Kind>::~soInstancePoolSub() { }
__declspec(weak) soInstancePoolSub<ftPikachuJolt2Info2, ftPikachuJolt2Kind>::~soInstancePoolSub() { }
__declspec(weak) soInstancePoolSub<ftPikachuJolt2Info3, ftPikachuJolt2Kind>::~soInstancePoolSub() { }
__declspec(weak) soInstancePool<ftPikachuJolt2Info3, soInstancePoolRoot, ftPikachuJolt2Kind, 3>::~soInstancePool() { }
#pragma force_active reset

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 ThunderJolt, 1 Cloud, 2 Vortex, 3 Thunder (the rest are unused).

#define FT_PIKACHU_POOL_THUNDERJOLT_BASE static_cast<wnArticlePool<wnPikachuThunderJolt, 3, ftPikachuCloudPool>&>(m_pools)
#define FT_PIKACHU_POOL_THUNDERJOLT FT_PIKACHU_POOL_THUNDERJOLT_BASE.getSub()
#define FT_PIKACHU_POOL_CLOUD_BASE static_cast<wnArticlePool<wnPikachuCloud, 2, ftPikachuVortexPool>&>(m_pools)
#define FT_PIKACHU_POOL_CLOUD FT_PIKACHU_POOL_CLOUD_BASE.getSub()
#define FT_PIKACHU_POOL_VORTEX_BASE static_cast<wnArticlePool<wnPikachuVortex, 1, ftPikachuThunderPool>&>(m_pools)
#define FT_PIKACHU_POOL_VORTEX FT_PIKACHU_POOL_VORTEX_BASE.getSub()
#define FT_PIKACHU_POOL_THUNDER_BASE static_cast<wnArticlePool<wnPikachuThunder, 3, soInstancePoolRoot>&>(m_pools)
#define FT_PIKACHU_POOL_THUNDER FT_PIKACHU_POOL_THUNDER_BASE.getSub()

#define FT_PIKACHU_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

void ftPikachuArticleMediator::deactivate() {
    for (s32 i = 0; i < 3; ++i) {
        wnPikachuThunderJolt* weapon = FT_PIKACHU_POOL_THUNDERJOLT.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnPikachuCloud* weapon = FT_PIKACHU_POOL_CLOUD.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnPikachuVortex* weapon = FT_PIKACHU_POOL_VORTEX.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 3; ++i) {
        wnPikachuThunder* weapon = FT_PIKACHU_POOL_THUNDER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftPikachuArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(3, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(2, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerateMaxNum(3, WN_ARTICLE_SLOT_TAGS);
    FT_PIKACHU_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftPikachuActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftPikachuArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnPikachuThunderJolt, 3, ftPikachuCloudPool>, ftPikachuActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnPikachuCloud, 2, ftPikachuVortexPool>, ftPikachuActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotActiveNum<wnArticlePool<wnPikachuVortex, 1, ftPikachuThunderPool>, ftPikachuActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotActiveNum<wnArticlePool<wnPikachuThunder, 3, soInstancePoolRoot>, ftPikachuActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_PIKACHU_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftPikachuArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnPikachuThunderJolt, 3, ftPikachuCloudPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnPikachuCloud, 2, ftPikachuVortexPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotIsGeneratable<wnArticlePool<wnPikachuVortex, 1, ftPikachuThunderPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotIsGeneratable<wnArticlePool<wnPikachuThunder, 3, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_PIKACHU_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftPikachuArticleActivator;
template <> struct ftPikachuArticleActivator<wnPikachuThunderJolt> {
    static bool activate(wnPikachuThunderJolt* weapon, soModuleAccesser* acc) { return ftPikachu::activeArticle(weapon, acc); }
};
template <> struct ftPikachuArticleActivator<wnPikachuCloud> {
    static bool activate(wnPikachuCloud* weapon, soModuleAccesser* acc) { return ftPikachu::activeArticle1(weapon, acc); }
};
template <> struct ftPikachuArticleActivator<wnPikachuVortex> {
    static bool activate(wnPikachuVortex* weapon, soModuleAccesser* acc) { return ftPikachu::activeArticle2(weapon, acc); }
};
template <> struct ftPikachuArticleActivator<wnPikachuThunder> {
    static bool activate(wnPikachuThunder* weapon, soModuleAccesser* acc) { return weapon->activeArticle(acc); }
};

soArticle* ftPikachuArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftPikachuArticleActivator<wnPikachuThunderJolt> >(static_cast<wnArticlePool<wnPikachuThunderJolt, 3, ftPikachuCloudPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftPikachuArticleActivator<wnPikachuCloud> >(static_cast<wnArticlePool<wnPikachuCloud, 2, ftPikachuVortexPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerate<ftPikachuArticleActivator<wnPikachuVortex> >(static_cast<wnArticlePool<wnPikachuVortex, 1, ftPikachuThunderPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerate<ftPikachuArticleActivator<wnPikachuThunder> >(static_cast<wnArticlePool<wnPikachuThunder, 3, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_PIKACHU_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftPikachuArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnPikachuThunderJolt>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotShoot<wnPikachuCloud>(article, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotShoot<wnPikachuVortex>(article, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotShoot<wnPikachuThunder>(article, WN_ARTICLE_SLOT_TAGS);
    FT_PIKACHU_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftPikachu is done
__declspec(weak) void testBuilder() {
    soInsideEventManageModuleBuilder<ftPikachuInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftPikachuInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

void fn_98_9AAC() {}
void fn_98_9C24() {}
u8 fn_98_9C28(u8* p) { return *(u8*)(p + 0x44); }
void fn_98_9C30() {}
void fn_98_9C70() {}
void fn_98_9C74() {}
void fn_98_9D04() {}
void fn_98_9D30() {}
void fn_98_9D34() {}
void fn_98_9D38() {}
void fn_98_9D3C() {}
void fn_98_9D40() {}
void fn_98_9D44() {}
void fn_98_9D48() {}
void fn_98_9D4C() {}
void fn_98_9D50() {}
void fn_98_9D54() {}
void fn_98_9D58() {}
void fn_98_9D5C() {}
void fn_98_9D60() {}
void fn_98_9D64() {}
void fn_98_9D68() {}
void fn_98_9D74() {}
void fn_98_9D78() {}
void fn_98_9D7C() {}
void fn_98_9D80() {}
void fn_98_9DAC() {}
void fn_98_9DB0() {}
void fn_98_9DB4() {}
void fn_98_9DB8() {}
void fn_98_9DBC() {}
int fn_98_9DC0(u8* p) { return *(int*)(p + 0x110); }
void fn_98_9DE0() {}
void fn_98_9DE4() {}
void fn_98_9DE8() {}
int fn_98_9DEC() { return 0x0; }
int fn_98_9DF4() { return 0x0; }
void fn_98_9DFC() {}
void fn_98_9E00() {}
void fn_98_9E04() {}
void fn_98_9E08() {}
void fn_98_9E0C() {}
void fn_98_9E10() {}
int fn_98_9F84(u8* p) { return *(int*)(p + 0x20); }
int fn_98_A060(u8* p) { return *(int*)(p + 0x18); }
int fn_98_A0F8(u8* p) { return *(int*)(p + 0x10); }
int fn_98_B59C() { return 0x4; }
void fn_98_C430() {}
void fn_98_CCDC() {}
int fn_98_CD18() { return 0x8; }

// soArticleMediatorImpl<...wnPikachuThunderJolt...>::setAutoRecycle: stores the flag at this+0x22A2C.
void fn_98_B5A4(u8* p, u8 flag) { *(u8*)(p + 0x22A2C) = flag; }

// ftPikachu::notify_thunder_reflect / adjust_camera_subject: forward this+0x60 to the thunder-hit process handlers.
void ftPikachu::notify_thunder_reflect() { ftPikachuStatusUniqProcessSpecialThunderHit::notifyReflect(m_moduleAccesser); }
void ftPikachu::adjust_camera_subject(soCameraModule* source) { ftPikachuStatusUniqProcessSpecialVortexAttack::adjust_camera_subject(m_moduleAccesser, source); }

void ftPikachu::onActivate() {
    m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000042);
}

void ftPikachu::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (prevStatusKind == 0x2c) {
        moduleAccesser->getEffectModule().killAll(2, true, true);
    }
}

void ftPikachu::notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x11b:
        ftPikachuStatusUniqProcessSpecialSkullBashAttack::stop(moduleAccesser);
        break;
    }
}

void** ftPikachu::getExtendParam() { return m_data->extendParam; }

__declspec(weak) void* ftPikachu::getThunderParam() {
    return g_ftCommonDataAccesser.getData(Fighter_Pikachu)->extendParam[3];
}

// The cloud of Thunder starts at the fighter and is remembered in the fighter's work area.
bool ftPikachu::activeArticle1(wnPikachuCloud* weapon, soModuleAccesser* acc) {
    // BYTES: declare the saved ID before the other persistent locals.
    s32 founderTaskId;
    ftPikachu& pikachu = dynamic_cast<ftPikachu&>(acc->getStageObject());
    founderTaskId = pikachu.m_taskId;
    int team = pikachu.getOwner()->getTeam();
    Vec3f position = acc->getPostureModule().getPos();
    int etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
    weapon->activate(founderTaskId, team, &position, etcResId, &pikachu);
    pikachu.rec_cloud_pointer(weapon);
    return false;
}

// The Final Smash vortex: the fighter's work area keeps the article's task, the vortex starts at the fighter.
bool ftPikachu::activeArticle2(wnPikachuVortex* weapon, soModuleAccesser* acc) {
    // BYTES: the task id is declared before the cast so it keeps r30 and the fighter keeps r29.
    s32 founderTaskId;
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    acc->getWorkManageModule().setInt(weapon->m_taskId, 0x20000003);
    founderTaskId = fighter.m_taskId;
    int finalResId = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser()).getFinalResId();
    int team = fighter.getOwner()->getTeam();
    Vec3f position;
    position = acc->getPostureModule().getPos();
    weapon->activate(founderTaskId, finalResId, team, &Vec3f(position));
    return true;
}

void ftPikachu::rec_cloud_pointer(wnPikachuCloud* cloud) {
    m_moduleAccesser->getWorkManageModule().setInt(reinterpret_cast<int>(cloud), 0x10000043);
}

wnPikachuCloud* ftPikachu::get_cloud_pointer(soModuleAccesser* acc) {
    return reinterpret_cast<wnPikachuCloud*>(acc->getWorkManageModule().getInt(0x10000043));
}

void ftPikachu::rec_cloud_pointer1(soModuleAccesser* acc, wnPikachuCloud* cloud) {
    acc->getWorkManageModule().setInt(reinterpret_cast<int>(cloud), 0x10000043);
}

// soKineticMediatorImpl::addSpeed:forwards to the transactor with the pools at this+4.
void fn_98_CCE0(u8* p, void* speed, soModuleAccesser* acc) { ftKineticTransactor::addSpeed(speed, p + 4, acc); }

// ftPikachu::getThunderParam:the common fighter data entry for Pikachu, field at 0x88.
void* fn_98_9540() { return *(void**)((u8*)g_ftCommonDataAccesser.getData(Fighter_Pikachu) + 0x88); }

}

// The situation-change thunk is still named by address. The other Fighter thunks are the real virtuals.
extern "C" {
void fn_98_E454(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_98_1DD8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4048(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_47F8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4B58(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4DA0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4DE0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"
