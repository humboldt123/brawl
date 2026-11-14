#define FT_SZEROSUIT_ARTICLE_DTORS
#define FT_ANIMCMD_UNIT_DTOR_OUTLINE
#define FT_PARAM_CUSTOMIZE_BUILDER_DTOR_NOINLINE
#define FT_SZEROSUIT_ARMOR_SHOOT_FLOAT_FIRST
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define SO_TEAM_DTOR_OUT_OF_LINE
#define WN_ARTICLE_BUILDER_DTORS
#define FT_ARTICLE_BUILDER_DTOR_OUTLINE
#define FT_ANIMCMD_UNIQ_DTOR_OUTLINE
#define FT_SZEROSUIT_TRANSACTOR_3ARG
#define FT_STATUS_NO_TAIL_FOR_WN
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define FT_NOINLINE_SKIP_SHARED_DEFS
#define FT_SZEROSUIT_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/builder/ft_builder_article.h>
#include <ft/ft_manager.h>
#include <ft/ft_class_info_impl.h>
#include <ft/szerosuit/ft_szerosuit.h>
#include <ft/szerosuit/ft_szerosuit_extend_param_accesser.h>
#include <it/it_gen_archive.h>
#include <so/so_value_accesser.h>
#include <it/it_manager.h>
#include <so/collision/so_collision_manager.h>
#include <so/so_external_value_accesser.h>
#include <mt/mt_prng.h>
#include <ft/samus/ft_samus_change_succeed.h>
#include <ft/samus/ft_samus_status_uniq_process.h>

#undef WN_ARTICLE_NATIVE_BUILDERS
#define FT_BC ftSZerosuitBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftSZerosuitExtendParamAccesser g_ftSZerosuitExtendParamAccesser;

// HYPOTHESIS: item generation sheets of the fighter kind (level 0 and 1) that the TU keeps as records.
itGenSheetKind g_ftSZerosuitGenSheetKind0(static_cast<Stages::srStageKind>(0xff), 0);
itGenSheetKind g_ftSZerosuitGenSheetKind1(static_cast<Stages::srStageKind>(0xff), 1);

ftClassInfoImpl<Fighter_SZeroSuit, ftSZerosuit> g_ftClassInfoSZerosuit;

extern soStatusUniqProcess g_ftSZerosuitStatusUniqProcessSpecialLw;
extern soStatusUniqProcess g_ftSZerosuitStatusUniqProcessSpecialLwKick;
extern soStatusUniqProcess g_ftSZerosuitStatusUniqProcessSpecialLwStep;
extern soStatusUniqProcess g_ftSZerosuitStatusUniqProcessSpecialLwFlip;
extern soStatusUniqProcess g_ftSZerosuitStatusUniqProcessFinalSamus;

ftSZerosuit::ftSZerosuit(s32 entryId,
                         Heaps::HeapType instHeap,
                         Heaps::HeapType nwModelInstHeap,
                         Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftSZerosuitBuildConfig>(entryId,
                                             Fighter_SZeroSuit,
                                             instHeap,
                                             nwModelInstHeap,
                                             nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_SZeroSuit)) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[15] = {0};
    processes[3] = &g_ftSZerosuitStatusUniqProcessSpecialLw;
    processes[8] = &g_ftSZerosuitStatusUniqProcessSpecialLw;
    processes[9] = &g_ftSZerosuitStatusUniqProcessSpecialLwKick;
    processes[10] = &g_ftSZerosuitStatusUniqProcessSpecialLwStep;
    processes[11] = &g_ftSZerosuitStatusUniqProcessSpecialLwFlip;
    processes[12] = &g_ftSZerosuitStatusUniqProcessSpecialLw;
    processes[13] = &g_ftSZerosuitStatusUniqProcessSpecialLw;
    processes[14] = &g_ftSZerosuitStatusUniqProcessFinalSamus;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 15);
    // HYPOTHESIS: the area data of the kind sits at +0x9C of the fighter data.
    m_moduleAccesser->getAreaModule().addArea(reinterpret_cast<soSet<soAreaData>*>(reinterpret_cast<u8*>(m_data) + 0x9c));
}

// ---- the article holders --------------------------------------------------------------------------------------------
struct ftSZerosuitArticleKind : wnArticleKindInfo {
    ftKind getSubKind() const { return static_cast<ftKind>(subKind); }
    ftSZerosuitArticleKind() {
        kind = 0;
        subKind = Fighter_SZeroSuit;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_148BD0(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

FT_DEFINE_ARTICLE_HOLDER_MEMBER(inline, wnSZerosuitParalyzer, const wnSZerosuitParalyzerData, 0, ftSZerosuitArticleKind, getWeaponData<wnSZerosuitParalyzerData>)
FT_DEFINE_ARTICLE_HOLDER(, wnSZerosuitParalyzerBullet, wnSZerosuitParalyzerBulletData, 1, ftSZerosuitArticleKind, fn_27_148BD0)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(inline, wnSZerosuitWhip, const wnSZerosuitWhipData, 2, ftSZerosuitArticleKind, getWeaponData<wnSZerosuitWhipData>)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(, wnSZerosuitArmor, const wnSZerosuitArmorData, 3, ftSZerosuitArticleKind, getWeaponData<wnSZerosuitArmorData>)

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnSZerosuitParalyzer>::~wnArticleHolder();
template wnArticleHolder<wnSZerosuitParalyzerBullet>::~wnArticleHolder();
template wnArticleHolder<wnSZerosuitWhip>::~wnArticleHolder();
template wnArticleHolder<wnSZerosuitArmor>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftSZerosuitArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftSZerosuitArticleMediator, 10>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftSZerosuitArticleMediator, 10>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftSZerosuitArticleMediator::~ftSZerosuitArticleMediator() { }

// The article destructors call these builder teardowns out of line.
template <typename BC>
soGeneralWorkBuilder<BC>::~soGeneralWorkBuilder() { }
template <s32 I, typename V, typename Null>
soArraySelectHolder<I, V, Null>::~soArraySelectHolder() { }
template <u32 S, typename T>
soModelModuleBuilder<soModelModuleBuildConfig<S, 0, T> >::~soModelModuleBuilder() { }
template soGeneralWorkBuilder<soGeneralWorkBuildConfig<2, 2, 1> >::~soGeneralWorkBuilder();
template soGeneralWorkBuilder<soGeneralWorkBuildConfig<7, 6, 1> >::~soGeneralWorkBuilder();
template soModelModuleBuilder<soModelModuleBuildConfig<1, 0, soModelModuleImpl> >::~soModelModuleBuilder();
template soArraySelectHolder<1, soArrayVector<acCmdInterpreterStackData, 1>, soSingletonHolder<soArrayNull<acCmdInterpreterStackData> > >::~soArraySelectHolder();
soParamAccesser::~soParamAccesser() { }
template <class BC>
soAnimCmdControlUnitBuilder<BC>::~soAnimCmdControlUnitBuilder() { }
template soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfig<0, 1, 1, 0, 0, 0, 0, 2> >::~soAnimCmdControlUnitBuilder();
template <class BC>
soAnimCmdControlUnitBuilderUniq<BC>::~soAnimCmdControlUnitBuilderUniq() { }
template soAnimCmdControlUnitBuilderUniq<soAnimCmdControlUnitBuildConfigUniq<4, 2> >::~soAnimCmdControlUnitBuilderUniq();
template <class BC>
soStopModuleBuilder<BC>::~soStopModuleBuilder() { }
template soStopModuleBuilder<soStopModuleBuildConfig<soStopModuleImpl> >::~soStopModuleBuilder();
template <s32 Kind, typename T>
soReflectModuleBuilder<soReflectModuleBuildConfig<Kind, T> >::~soReflectModuleBuilder() { }
template soReflectModuleBuilder<soReflectModuleBuildConfig<0, soReflectModuleImpl> >::~soReflectModuleBuilder();
template <class T>
soEffectModuleBuilder<wnEffectModuleBuildConfig<1, 1, 0, 1, -1, T> >::~soEffectModuleBuilder() { }
template soEffectModuleBuilder<wnEffectModuleBuildConfig<1, 1, 0, 1, -1, soEffectModuleImpl> >::~soEffectModuleBuilder();
soSlowModuleBuilder<soSlowModuleBuildConfig<soSlowModuleSimple> >::~soSlowModuleBuilder() { }
soGlowModuleBuilder<soGlowModuleBuildConfig<soGlowModuleSimple> >::~soGlowModuleBuilder() { }

#pragma auto_inline off
template <class T> void ftBuilderDtorUse(T* p) { p->~T(); }
#pragma auto_inline on
typedef soGeneralWorkBuilder<soGeneralWorkBuildConfig<2, 2, 1> > ftSZerosuitArticleWork221Builder;
template void ftBuilderDtorUse<ftSZerosuitArticleWork221Builder>(ftSZerosuitArticleWork221Builder*);
typedef soGeneralWorkBuilder<soGeneralWorkBuildConfig<7, 6, 1> > ftSZerosuitArticleWork761Builder;
template void ftBuilderDtorUse<ftSZerosuitArticleWork761Builder>(ftSZerosuitArticleWork761Builder*);
typedef soAnimCmdModuleBuilder<soAnimCmdModuleBuildConfig<6, soAnimCmdModuleImpl> > ftSZerosuitArticleAnimCmdBuilder;
template void ftBuilderDtorUse<ftSZerosuitArticleAnimCmdBuilder>(ftSZerosuitArticleAnimCmdBuilder*);
typedef soLinkModuleBuilder<soLinkModuleBuildConfigCap<5, soLinkModuleImpl> > ftSZerosuitArticleLinkBuilder;
template void ftBuilderDtorUse<ftSZerosuitArticleLinkBuilder>(ftSZerosuitArticleLinkBuilder*);
typedef soShakeModuleBuilder<soShakeModuleBuildConfig<0, soShakeModuleSimple> > ftSZerosuitArticleShakeBuilder;
template void ftBuilderDtorUse<ftSZerosuitArticleShakeBuilder>(ftSZerosuitArticleShakeBuilder*);
typedef soTeamModuleBuilder<soTeamModuleBuildConfig<soTeamImpl, soTeamModuleImpl> > ftSZerosuitArticleTeamBuilder;
template void ftBuilderDtorUse<ftSZerosuitArticleTeamBuilder>(ftSZerosuitArticleTeamBuilder*);
typedef soModelModuleBuilder<soModelModuleBuildConfig<5, 0, soModelModuleImpl> > ftSZerosuitArticleModel5Builder;
template void ftBuilderDtorUse<ftSZerosuitArticleModel5Builder>(ftSZerosuitArticleModel5Builder*);
typedef soModelModuleBuilder<soModelModuleBuildConfig<1, 0, soModelModuleImpl> > ftSZerosuitArticleModel1Builder;
template void ftBuilderDtorUse<ftSZerosuitArticleModel1Builder>(ftSZerosuitArticleModel1Builder*);
typedef soResourceModuleBuilder<soResourceModuleBuildConfig<0, soResourceIdAccesserImpl, soResourceModuleImpl> > ftSZerosuitArticleResourceBuilder;
template void ftBuilderDtorUse<ftSZerosuitArticleResourceBuilder>(ftSZerosuitArticleResourceBuilder*);
typedef soVisibilityModuleBuilder<soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> > ftSZerosuitArticleVisibilityBuilder;
template void ftBuilderDtorUse<ftSZerosuitArticleVisibilityBuilder>(ftSZerosuitArticleVisibilityBuilder*);
typedef soTransitionModuleBuilder<soTransitionModuleBuildConfig<soTypeList<soIntToType<2>, soTypeListNullType> > > ftSZerosuitWhipTransitionBuilder;
template void ftBuilderDtorUse<ftSZerosuitWhipTransitionBuilder>(ftSZerosuitWhipTransitionBuilder*);
template void ftBuilderDtorUse<wnKineticModuleImpl>(wnKineticModuleImpl*);
typedef soArraySelectHolder<1, soArrayVector<acCmdInterpreterStackData, 1>, soSingletonHolder<soArrayNull<acCmdInterpreterStackData> > > ftSZerosuitArticleStackHolder1;
template void ftBuilderDtorUse<ftSZerosuitArticleStackHolder1>(ftSZerosuitArticleStackHolder1*);
#pragma dont_inline off

inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
typedef soSoundModuleBuilder<wnSoundModuleBuildConfig<0, soSound3dGeneratorAccesserImpl, soSoundModuleImpl> > ftSZerosuitArticleSoundBuilder;
template void ftBuilderDtorUse<ftSZerosuitArticleSoundBuilder>(ftSZerosuitArticleSoundBuilder*);

__declspec(weak) s32 ftSZerosuitArticleMediator::getMediateNum() { return 4; }
void ftSZerosuitArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Paralyzer, 1 ParalyzerBullet, 2 Whip, 3 Armor (the rest are unused).

#define FT_SZEROSUIT_POOL_PARALYZER_BASE static_cast<wnArticlePool<wnSZerosuitParalyzer, 1, ftSZerosuitBulletPool>&>(m_pools)
#define FT_SZEROSUIT_POOL_PARALYZER FT_SZEROSUIT_POOL_PARALYZER_BASE.getSub()
#define FT_SZEROSUIT_POOL_BULLET_BASE static_cast<wnArticlePool<wnSZerosuitParalyzerBullet, 2, ftSZerosuitWhipPool>&>(m_pools)
#define FT_SZEROSUIT_POOL_BULLET FT_SZEROSUIT_POOL_BULLET_BASE.getSub()
#define FT_SZEROSUIT_POOL_WHIP_BASE static_cast<wnArticlePool<wnSZerosuitWhip, 1, ftSZerosuitArmorPool>&>(m_pools)
#define FT_SZEROSUIT_POOL_WHIP FT_SZEROSUIT_POOL_WHIP_BASE.getSub()
#define FT_SZEROSUIT_POOL_ARMOR_BASE static_cast<wnArticlePool<wnSZerosuitArmor, 8, soInstancePoolRoot>&>(m_pools)
#define FT_SZEROSUIT_POOL_ARMOR FT_SZEROSUIT_POOL_ARMOR_BASE.getSub()

#define FT_SZEROSUIT_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

void ftSZerosuitArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnSZerosuitParalyzer* weapon = FT_SZEROSUIT_POOL_PARALYZER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnSZerosuitParalyzerBullet* weapon = FT_SZEROSUIT_POOL_BULLET.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSZerosuitWhip* weapon = FT_SZEROSUIT_POOL_WHIP.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 8; ++i) {
        wnSZerosuitArmor* weapon = FT_SZEROSUIT_POOL_ARMOR.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftSZerosuitArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(2, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerateMaxNum(8, WN_ARTICLE_SLOT_TAGS);
    FT_SZEROSUIT_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftSZerosuitActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftSZerosuitArticleMediator::getActiveNum(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnSZerosuitParalyzer, 1, ftSZerosuitBulletPool>, ftSZerosuitActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnSZerosuitParalyzerBullet, 2, ftSZerosuitWhipPool>, ftSZerosuitActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotActiveNum<wnArticlePool<wnSZerosuitWhip, 1, ftSZerosuitArmorPool>, ftSZerosuitActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotActiveNum<wnArticlePool<wnSZerosuitArmor, 8, soInstancePoolRoot>, ftSZerosuitActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_SZEROSUIT_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftSZerosuitArticleMediator::isGeneratable(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnSZerosuitParalyzer, 1, ftSZerosuitBulletPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnSZerosuitParalyzerBullet, 2, ftSZerosuitWhipPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotIsGeneratable<wnArticlePool<wnSZerosuitWhip, 1, ftSZerosuitArmorPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotIsGeneratable<wnArticlePool<wnSZerosuitArmor, 8, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_SZEROSUIT_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftSZerosuitArticleActivator;
template <> struct ftSZerosuitArticleActivator<wnSZerosuitParalyzer> {
    static bool activate(wnSZerosuitParalyzer* weapon, soModuleAccesser* acc) {
        return soSingletonHolder<ftSZerosuitTransactor>::getInstance()->activeArticle1(
            weapon, acc, acc->getResourceModule().getResourceIdAccesser()->getMdlResId());
    }
};
template <> struct ftSZerosuitArticleActivator<wnSZerosuitParalyzerBullet> {
    static bool activate(wnSZerosuitParalyzerBullet* weapon, soModuleAccesser* acc) { return soSingletonHolder<ftSZerosuitTransactor>::getInstance()->activeArticle2(weapon, acc); }
};
template <> struct ftSZerosuitArticleActivator<wnSZerosuitWhip> {
    static bool activate(wnSZerosuitWhip* weapon, soModuleAccesser* acc) { return ftSZerosuit::activeArticle(weapon, acc); }
};
template <> struct ftSZerosuitArticleActivator<wnSZerosuitArmor> {
    static bool activate(wnSZerosuitArmor* weapon, soModuleAccesser* acc) { return ftSZerosuit::activeArticle1(weapon, acc); }
};

__declspec(weak) soArticle* ftSZerosuitArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftSZerosuitArticleActivator<wnSZerosuitParalyzer> >(static_cast<wnArticlePool<wnSZerosuitParalyzer, 1, ftSZerosuitBulletPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftSZerosuitArticleActivator<wnSZerosuitParalyzerBullet> >(static_cast<wnArticlePool<wnSZerosuitParalyzerBullet, 2, ftSZerosuitWhipPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerate<ftSZerosuitArticleActivator<wnSZerosuitWhip> >(static_cast<wnArticlePool<wnSZerosuitWhip, 1, ftSZerosuitArmorPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerate<ftSZerosuitArticleActivator<wnSZerosuitArmor> >(static_cast<wnArticlePool<wnSZerosuitArmor, 8, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_SZEROSUIT_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

template <int S, int L>
inline bool ftSZerosuitShootSlot3(soModuleAccesser* acc, soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    return ftSZerosuit::shootArticle(&dynamic_cast<wnSZerosuitArmor&>(*article), acc);
}

bool ftSZerosuitArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnSZerosuitParalyzer>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotShoot<wnSZerosuitParalyzerBullet>(article, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotShoot<wnSZerosuitWhip>(article, WN_ARTICLE_SLOT_TAGS);
    case 3: return ftSZerosuitShootSlot3(acc, article, WN_ARTICLE_SLOT_TAGS);
    FT_SZEROSUIT_UNUSED_SLOTS(true)
    default: return false;
    }
}

// ---- the fighter -----------------------------------------------------------------------------------------------------
// HYPOTHESIS: work variable 0x10000041 holds the task id of the paralyzer article.
void ftSZerosuit::onActivate() {
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000041);
}

void ftSZerosuit::onDeactivate() {
    itManager::getInstance()->removeItem1(m_taskId);
}

// The paralyzer is generated once and entered; its task id is forgotten again.
void ftSZerosuit::onStart(int startKind) {
    Fighter::onStart(startKind);
    soGenerateArticleManageModule& articles = m_moduleAccesser->getGenerateArticleManageModule();
    if (!articles.isExist(0)) {
        soArticle* article = articles.generate(0, 0, false);
        if (!reinterpret_cast<ftSamusArticleNullView*>(article)->isNull()) {
            articles.entry(article);
        }
    }
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000041);
}

// HYPOTHESIS: the status kinds name the moves that keep or lose the whip (article 2) and the paralyzer (article 0).
void ftSZerosuit::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    soGenerateArticleManageModule& articles = m_moduleAccesser->getGenerateArticleManageModule();
    if (prevStatusKind == 0x2c || prevStatusKind == 0x32 || prevStatusKind == 0x33 || prevStatusKind == 0x35 || prevStatusKind == 0x37 ||
        prevStatusKind == 0x3c || prevStatusKind == 0x10c || prevStatusKind == 0x10f) {
        articles.removeExist(2, 0);
    } else if (static_cast<u32>(prevStatusKind - 0x113) <= 1) {
        if (statusKind != 0x81 && statusKind != 0x80) {
            articles.removeExist(2, 0);
        }
    } else if (prevStatusKind == 0x34 || prevStatusKind == 0x36 || prevStatusKind == 0x38) {
        if (statusKind != 0x35 && statusKind != 0x37) {
            articles.removeExist(2, 0);
        }
    }
    if (articles.isExist(0)) {
        if (statusKind == 0xbd) {
            articles.remove(0, 0);
        } else {
            if (statusKind != 0x81 && statusKind != 0x80 && statusKind != 0x82 && statusKind != 0x83 &&
                statusKind != 0x35 && statusKind != 0x37) {
                articles.changeMotion(0, -1, false);
            }
            articles.changeStatus(0, 0, 0);
        }
    }
}

bool ftSZerosuit::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    switch (acmd->getType()) {
    case 5:
        // HYPOTHESIS: command 5 switches to the other fighter of a Samus pair (the transformation).
        if (static_cast<u8>(g_ftManager->getFighterCount(m_entryId)) > 1) {
            int fighterNo = g_ftManager->getFighterNo(m_entryId, m_taskId);
            u8 other = (u8)fighterNo == 0 ? 1 : 0;
            g_ftManager->toChange(m_entryId, other, 1, 0);
        }
        return true;
    }
    return Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
}

// The events of the link with the paralyzer (a stage object of subkind 0x41) tell the fighter what the article needs.
// HYPOTHESIS: the event payload sits behind the two header words: a flag word at +4, the value or position at +8.
struct ftSZerosuitLinkEventView {
    int kind;
    bool done;
    int value;
};

void ftSZerosuit::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftSZerosuitLinkEventView* event = reinterpret_cast<ftSZerosuitLinkEventView*>(eventInfo);
    switch (stageObj->soGetSubKind()) {
    case 0x41:
        switch (event->kind) {
        case 0x838:
            if (m_moduleAccesser->getLinkModule().isLinked(0)) {
                StageObject* node = m_moduleAccesser->getLinkModule().getNode(0);
                if (node != NULL) {
                    Vec3f pos = soExternalValueAccesser::getPos(node);
                    *reinterpret_cast<Vec3f*>(&event->value) = pos;
                    event->done = true;
                }
            }
            break;
        case 0x839:
            moduleAccesser->getStatusModule().changeStatusRequest(0x39, moduleAccesser);
            break;
        case 0x83a:
            event->value = moduleAccesser->getModelModule().getCorrectNodeId(0x12e);
            event->done = true;
            break;
        case 0x83b:
            int* constant = static_cast<int*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA80E, 0));
            moduleAccesser->getWorkManageModule().setInt(*constant, 0x20000001);
            moduleAccesser->getStatusModule().changeStatusRequest(0x81, moduleAccesser);
            moduleAccesser->getGenerateArticleManageModule().changeMotion(0, 0xf, false);
            if (moduleAccesser->getGenerateArticleManageModule().isExist(2) == true) {
                soLinkModule& link = moduleAccesser->getLinkModule();
                if (link.link(3, moduleAccesser->getGenerateArticleManageModule().getArticle(2)->getTaskId()) == true) {
                    soLinkModule& linkAgain = moduleAccesser->getLinkModule();
                    linkAgain.setModelConstraintPosOrt(3, moduleAccesser->getModelModule().getCorrectNodeId(0x12e), event->value, 5, false);
                }
            }
            break;
        case 0x83c:
            if (moduleAccesser->getGroundModule().isStCliff(0)) {
                moduleAccesser->getStatusModule().changeStatusRequest(0x75, moduleAccesser);
            } else {
                moduleAccesser->getStatusModule().changeStatusRequest(0x7b, moduleAccesser);
            }
            break;
        case 0x83e:
            moduleAccesser->getStatusModule().changeStatusRequest(0x83, moduleAccesser);
            break;
        case 0:
        case 2:
        case 4: {
            int logGroup = moduleAccesser->getWorkManageModule().getInt(0x10000041);
            if (logGroup != -1) {
                moduleAccesser->getCollisionAttackModule().unRefLogGroup(0);
                soCollisionManager::getInstance()->leaveFreeRefLogGroup(logGroup);
                soCollisionManager::getInstance()->freeLogGroup(logGroup);
                moduleAccesser->getWorkManageModule().setInt(-1, 0x10000041);
            }
            break;
        }
        }
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}
// The fighter uses the global collision manager shared with the main module.
soCollisionManager* soCollisionManager::getInstance() {
    if (::g_soCollisionManager == NULL) {
        ::g_soCollisionManager = new (Heaps::System) soCollisionManager;
    }
    return ::g_soCollisionManager;
}

void ftSZerosuit::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
    if (kind != 2) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    }
}

// The dummy fighter that takes over the move gets its collision shapes from the constant table of the kind.
void ftSZerosuit::setupChangeSucceedWork(void* work) {
    Fighter::setupChangeSucceedWork(work);
    soModuleAccesser* acc = m_moduleAccesser;
    ftSamusChangeSucceedPoints hurt;
    hurt.point[0][0] = 0.0f;
    hurt.point[1][0] = soValueAccesser::getConstantFloat(acc, 0xbd6, 0);
    hurt.point[2][0] = 0.0f;
    hurt.point[3][0] = soValueAccesser::getConstantFloat(acc, 0xbd8, 0);
    hurt.point[0][1] = 0.0f;
    hurt.point[1][1] = soValueAccesser::getConstantFloat(acc, 0xbd1, 0);
    hurt.point[2][1] = 0.0f;
    hurt.point[3][1] = 0.0f;
    ftSamusChangeSucceedPoints hit;
    hit.point[0][0] = 0.0f;
    hit.point[1][0] = soValueAccesser::getConstantFloat(acc, 0xbbc, 0);
    hit.point[2][0] = 0.0f;
    hit.point[3][0] = -1.0f;
    hit.point[0][1] = 0.0f;
    hit.point[1][1] = 0.0f;
    hit.point[2][1] = 0.0f;
    hit.point[3][1] = 0.0f;
    Vec2f* speed = &acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(0xFFFF));
    soPostureModule& posture = acc->getPostureModule();
    static_cast<ftChangeSucceedWork*>(work)->activateDummyFighter(&posture.getPos(), speed, &hit.point[0][0], &hurt.point[0][0], 0xf60003, posture.getScale(), posture.getLr());
}

// The fighter that takes over (the transformation partner) is started with the work of this one.
void ftSZerosuit::change(void* work, int mode) {
    onStart(mode);
    Fighter::changeSucceedCore(work, 0x120);
}


// ---- the whip and the armor ------------------------------------------------------------------------------------------
// The whip starts at the hip node with the facing direction, the attack state and a fresh collision log group.
// HYPOTHESIS: status kinds 0x113 and 0x114 are the first whip attacks, and 0x34/0x36/0x38 the tilts that link the whip to the model.
// MATCH-ONLY: inline helpers retain the native callback register lifetimes.
static inline int checkWhipSpecial(int statusKind) {
    int isSpecial = 1;
    if (statusKind != 0x113 && statusKind != 0x114) {
        isSpecial = 0;
    }
    return isSpecial;
}
static inline int checkWhipTilt(int statusKind) {
    int isTilt = 0;
    if (static_cast<u32>(statusKind - 0x34) <= 4 && ((1 << (statusKind - 0x34)) & 0x15)) {
        isTilt = 1;
    }
    return isTilt;
}
static inline int getFighterTaskId(Fighter& fighter) {
    return fighter.m_taskId;
}
static inline void activateWhipCall(wnSZerosuitWhip* weapon, soModuleAccesser* acc, Fighter& fighter, Vec3f& pos, float lr, int isSpecial, int isTilt, int logGroup) {
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = getFighterTaskId(fighter);
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam(), lr, &pos, isSpecial, isTilt, logGroup);
}
bool ftSZerosuit::activeArticle(wnSZerosuitWhip* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0x36, false);
    float lr = acc->getPostureModule().getLr();
    int statusKind = acc->getStatusModule().getStatusKind();
    int isSpecial = checkWhipSpecial(statusKind);
    int isTilt = checkWhipTilt(statusKind);
    int logGroup = soCollisionManager::getInstance()->allocateLogGroup();
    acc->getWorkManageModule().setInt(logGroup, 0x10000041);
    if (logGroup != -1) {
        acc->getCollisionAttackModule().refLogGroup(0, logGroup);
    }
    activateWhipCall(weapon, acc, fighter, pos, lr, isSpecial, isTilt, logGroup);
    soLinkModule& link = acc->getLinkModule();
    if (isTilt) {
        if (link.link(3, weapon->getTaskId())) {
            link.setModelConstraintPosOrt(3, acc->getModelModule().getCorrectNodeId(0x12c), 0xa, 0x11, false);
        }
    } else {
        if (link.link(6, weapon->getTaskId())) {
            link.setAttribute(6, static_cast<soLinkConnection::Attribute>(0), true);
        }
    }
    return true;
}

// One of the armor pieces flies off: the work counter picks the piece and its parameters.
// MATCH-ONLY: the inline helper retains the native callback register lifetimes.
static inline void activateArmorCall(wnSZerosuitArmor* weapon, soModuleAccesser* acc, Fighter& fighter, int index, u32* entry) {
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), index, resources.getResourceIdAccesser()->getTexResId(), entry[0], entry[1], fighter.getOwner()->getTeam());
}
bool ftSZerosuit::activeArticle1(wnSZerosuitArmor* weapon, soModuleAccesser* acc) {
    int index;
    u32* entry;
    ftData* data;
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    data = g_ftCommonDataAccesser.getData(Fighter_SZeroSuit);
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0x36, false);
    acc->getPostureModule().getLr();
    index = acc->getWorkManageModule().getInt(0x10000040);
    entry = reinterpret_cast<u32*>(*reinterpret_cast<u8**>(reinterpret_cast<u8*>(data) + 0xa4) + index * 16);
    activateArmorCall(weapon, acc, fighter, index, entry);
    return true;
}

// The piece is thrown with a speed between the two limits of the kind and the next piece is selected.
bool ftSZerosuit::shootArticle(wnSZerosuitArmor* weapon, soModuleAccesser* acc) {
    int index = acc->getWorkManageModule().getInt(0x10000040);
    ftData* data = g_ftCommonDataAccesser.getData(Fighter_SZeroSuit);
    u8* entry = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(data) + 0xa4) + index * 16;
    const float* range = static_cast<const float*>(data->extendParam[3]);
    float lr = acc->getPostureModule().getLr();
    weapon->shoot(range[0] + (range[1] - range[0]) * randf(), *reinterpret_cast<float*>(entry + 8), lr, *reinterpret_cast<int*>(entry + 4));
    acc->getWorkManageModule().setInt(index + 1, 0x10000040);
    return false;
}
