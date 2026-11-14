#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
// MATCH-ONLY: the custom kinetic helper calls the shared vector constructor.
#define MT_VEC3F_CTOR_NOINLINE
#define FT_NOINLINE_SEARCH_BUILDER
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_WARIO_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#define FT_REL_LINK_EXTERN
#define FT_KEEP_OBSERVER_VTABLES_WEAK
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define WN_KINETIC_GRAVITY_INLINE_DTOR
#define WN_WEAPON_BUILDER
#define WN_KINETIC_GENERIC_IN_FIGHTER_TU
#define WN_KINETIC_GENERIC_BUILDER
#define WN_DAMAGE_ACTOR_INTERFACE
#include <ft/builder/ft_dol_array_list.h>
FT_DOL_ARRAY_VECTOR(soLinkConnection, 9);
FT_DOL_ARRAY_VECTOR(soStatusUniqProcess*, 19);
FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 4);
FT_DOL_ARRAY_VECTOR(soCollisionAttackAbsolute, 1);
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 4);
template <> soArrayVector<ftAnimCmdConvPtr, 305>::~soArrayVector();
#include <so/sound/so_sound_3d_generator_accesser_impl.h>
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
// BYTES: StageObject's weapon-builder inlines are defined after globals so any local static follows unit globals.
#undef WN_WEAPON_BUILDER
#define SO_INPUT_NULL_CTOR
#include <so/stageobject.h>
#include <ip/ip_null.h>
#define WN_WEAPON_BUILDER
inline bool StageObject::checkTransitionStatus(u32) { return true; }
inline bool StageObject::isTreadPassive() { return false; }
#include <ft/ft_class_info_impl.h>
#include <ft/wario/ft_wario_builder.h>
#include <ft/wario/ft_wario_extend_param_accesser.h>
#include <ft/wario/ft_wario_common.h>
#include <ft/wario/ft_wario_transactor.h>
#include <ft/wario/ft_wario_unk8.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_owner.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/so_kinetic_utility.h>

#undef WN_ARTICLE_NATIVE_BUILDERS
#pragma dont_inline on
template soArrayVector<soStatusUniqProcess*, 305>::~soArrayVector();
template <class Mediator>
soSelectInstanceHolder<Mediator, soArticleMediatorNull>::~soSelectInstanceHolder() { }
template soSelectInstanceHolder<ftWarioArticleMediator, soArticleMediatorNull>::~soSelectInstanceHolder();
#pragma dont_inline off
#define FT_BC ftWarioBuildConfig
#include <ft/builder/ft_builder_noinline.h>

inline soControllerModuleLinkRef::~soControllerModuleLinkRef() { }
soStatusModuleImpl::~soStatusModuleImpl() { }

#pragma dont_inline on
ftWarioUnk8::ftWarioUnk8(int a, int b) : unk0(a), unk4(b) { }
#pragma dont_inline off

ftWarioExtendParamAccesser g_ftWarioExtendParamAccesser;
static ftWarioUnk8 s_unkTag0(0xff, 0);
static ftWarioUnk8 s_unkTag1(0xff, 1);
ftClassInfoImpl<Fighter_Wario, ftWario> g_ftClassInfoWario;

inline Input* StageObject::getInput() {
    static IpNull sNullInput;
    return &sNullInput;
}

// Wario extends the common fighter data with an area set and model settings
// used by the second six-color costume group. Calls through the real area and
// model interfaces establish both the record offsets and the argument types.
struct ftWarioData : ftData {
    soSet<soAreaData> additionalAreas; // +0x94
    u32** costumeNodeConvertData; // +0x9C
    int costumeShadowNode; // +0xA0
};
static_assert(sizeof(ftWarioData) == 0xA4, "Wario data prefix");

// Only the primary status-process view is needed for registration, as in the
// other fighters. Concrete implementations remain in their status units.
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNOpen;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialS;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialHiStart;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialLw;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNBite;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNBiteEnd;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNEatStart;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialNEat;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSStart;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSDrive;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSWheelie;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSCommon;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSEscape;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialSDown;
extern soStatusUniqProcess g_ftWarioStatusUniqProcessSpecialHiJump;

ftWario::ftWario(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftWarioBuildConfig>(entryId,
                                         Fighter_Wario,
                                         instHeap,
                                         instHeap, // Wario uses the instance heap for models too.
                                         nwMotionInstHeap),
    m_data(static_cast<ftWarioData*>(g_ftCommonDataAccesser.getData(Fighter_Wario))) {
    // Chomp's open, bite and eating phases share handlers where their behavior
    // agrees. Null entries install no character-specific process.
    soStatusUniqProcess* processes[30] = {0};
    processes[0] = &g_ftWarioStatusUniqProcessSpecialNOpen;
    processes[1] = &g_ftWarioStatusUniqProcessSpecialS;
    processes[2] = &g_ftWarioStatusUniqProcessSpecialHiStart;
    processes[3] = &g_ftWarioStatusUniqProcessSpecialLw;
    processes[5] = &g_ftWarioStatusUniqProcessSpecialNOpen;
    processes[7] = &g_ftWarioStatusUniqProcessSpecialNBite;
    processes[8] = &g_ftWarioStatusUniqProcessSpecialNBite;
    processes[9] = &g_ftWarioStatusUniqProcessSpecialNBiteEnd;
    processes[10] = &g_ftWarioStatusUniqProcessSpecialNEatStart;
    processes[11] = &g_ftWarioStatusUniqProcessSpecialNEat;
    processes[12] = &g_ftWarioStatusUniqProcessSpecialNEat;
    processes[13] = &g_ftWarioStatusUniqProcessSpecialNEat;
    processes[14] = &g_ftWarioStatusUniqProcessSpecialNEat;

    // Mounting, riding, wheelies, dismounting and the shared bike states.
    processes[15] = &g_ftWarioStatusUniqProcessSpecialSStart;
    processes[17] = &g_ftWarioStatusUniqProcessSpecialSStart;
    processes[18] = &g_ftWarioStatusUniqProcessSpecialSDrive;
    processes[19] = &g_ftWarioStatusUniqProcessSpecialSWheelie;
    processes[20] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[21] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[22] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[23] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[24] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[25] = &g_ftWarioStatusUniqProcessSpecialSEscape;
    processes[26] = &g_ftWarioStatusUniqProcessSpecialSDown;
    processes[27] = &g_ftWarioStatusUniqProcessSpecialSCommon;
    processes[28] = &g_ftWarioStatusUniqProcessSpecialHiJump;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 30);

    m_moduleAccesser->getAreaModule().addArea(&m_data->additionalAreas);
    // HYPOTHESIS: colors 6..11 are the classic-costume group. Their distinct
    // node conversion and shadow anchor are explicit in the native setup.
    if (getOwner()->getFighterColor() >= 6 && getOwner()->getFighterColor() <= 11) {
        m_moduleAccesser->getModelModule().setNodeConvertData(m_data->costumeNodeConvertData);
        m_moduleAccesser->getModelModule().setShadowNode(m_data->costumeShadowNode);
    }
}

// The common implementation keeps Waft charge, bike links and transformation
// state consistent between normal Wario and Wario-Man.
// MATCH-ONLY: the kinetic setup uses the shared getter call boundary.
#pragma dont_inline on
ftWarioExtendParam* ftWario::getExtendParam() {
    return reinterpret_cast<ftWarioExtendParam*>(&m_data->extendParam[0]);
}
#pragma dont_inline reset

void ftWario::onActivate() {
    ftWarioExtendParam* param = reinterpret_cast<ftWarioExtendParam*>(&m_data->extendParam[0]);
    g_ftWarioCommon.onActivate(m_moduleAccesser, &m_waftAnim, param->bikeRider->remountCooldownFrames);
}

void ftWario::onDeactivate() {
    g_ftWarioCommon.onDeactivate(m_moduleAccesser, &m_waftAnim);
}

void ftWario::onStart(int startKind) {
    Fighter::onStart(startKind);
    g_ftWarioCommon.onStart(m_moduleAccesser, startKind);
}

void ftWario::toDead(Dead::Reason reason) {
    g_ftWarioCommon.toDead(m_moduleAccesser, reason);
    Fighter::toDead(reason);
}

void ftWario::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc, StageObject* sender, int selector) {
    if (!g_ftWarioCommon.notifyEventLink(static_cast<ftWarioLinkEventArgs*>(event), acc, sender, selector))
        Fighter::notifyEventLink(event, acc, sender, selector);
}

void ftWario::notifyEventChangeStatus(int status, int previous, soStatusData* data, soModuleAccesser* acc) {
    Fighter::notifyEventChangeStatus(status, previous, data, acc);
    g_ftWarioCommon.notifyEventChangeStatus(status, previous, data, acc);
}

bool ftWario::notifyEventAnimCmd(acAnimCmd* command, soModuleAccesser* acc, s32 index) {
    s8 group = command->getGroup();
    if (!isObserv(group)) return false;
    switch (command->getType()) {
    case 5:
        g_ftWarioCommon.toChange(acc, m_entryId, 1, 1);
        return true;
    }
    return Fighter::notifyEventAnimCmd(command, acc, index);
}

// BYTES: the native function addresses the local static's guard, object and destructor registration separately
// instead of from one pooled BSS base. The setting is per function: unit-wide it breaks the pooled constructor and
// __sinit, and an in-class inline definition makes the static weak and moves it to the end of .bss.
#pragma pool_data off
void ftWario::notifyEventCollisionSearch(soCollisionLog* log, soModuleAccesser* acc) {
    static ftWarioTransactor transactor;
    static_cast<ftKirbyCopyAbilityProcess&>(transactor).notifyEventCollisionSearch(log, acc);
    Fighter::notifyEventCollisionSearch(log, acc);
}
#pragma pool_data reset

// Restore the saved fighter state when returning from Wario-Man. The native
// virtual calls preserve overrides of both the start and restoration hooks.
void ftWario::change(void* work, int startKind) {
    onStart(startKind);
    changeSucceedCore(work, 0x130);
}

void ftWario::setupChangeSucceedWork(void* work) {
    Fighter::setupChangeSucceedWork(work);
    g_ftWarioCommon.setupChangeSucceedWork(m_moduleAccesser, work);
}

void ftWario::changeSucceedCore(void* work, int kind) {
    Fighter::changeSucceedCore(work, kind);
    g_ftWarioCommon.changeSucceedCore(m_moduleAccesser, work, kind);
}

void ftWario::processUpdate() {
    Fighter::processUpdate();
    ftWarioExtendParam* param = reinterpret_cast<ftWarioExtendParam*>(&m_data->extendParam[0]);
    g_ftWarioCommon.processUpdate(param->specialLw, m_moduleAccesser, &m_waftAnim);
}

bool ftWario::activeArticle(wnWarioGarlic* article, soModuleAccesser* acc) {
    int founderTaskId;
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resource = acc->getResourceModule();
    founderTaskId = fighter.m_taskId;
    article->activate(founderTaskId, resource.getResourceIdAccesser()->getEtcResId(),
                      fighter.getOwner()->getTeam());
    return true;
}

// ---- Wario article holders, builder, and mediator -------------------------------------------------------------------

#include <ft/builder/ft_builder_article.h>

struct ftWarioArticleKind : wnArticleKindInfo {
    ftKind getSubKind() const { return static_cast<ftKind>(subKind); }
    ftWarioArticleKind() {
        kind = 0;
        subKind = Fighter_Wario;
    }
};

extern "C" void* fn_27_14948C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1494BC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <>
inline wnInstanceHolder<wnWarioBike, ftWarioArticleKindInfo, soIntToType<0> >::wnInstanceHolder(soModuleAccesser* acc) :
    m_instance(0, ftArticleBuildInfo(ftWarioArticleKind(), &acc->getHeapModule()),
               static_cast<const wnWarioBikeData*>(fn_27_14948C(&g_ftCommonDataAccesser,
                                                               ftWarioArticleKind().getSubKind(),
                                                               &ftArticleDataFlags().get().resourceGroup))) { }

template <>
inline wnInstanceHolder<wnWarioGarlic, ftWarioArticleKindInfo, soIntToType<1> >::wnInstanceHolder(soModuleAccesser* acc) :
    m_instance(1, ftArticleBuildInfo(ftWarioArticleKind(), &acc->getHeapModule()),
               static_cast<const wnWarioGarlicData*>(fn_27_1494BC(&g_ftCommonDataAccesser,
                                                                 ftWarioArticleKind().getSubKind(),
                                                                 &ftArticleDataFlags().get().resourceGroup))) { }

#pragma dont_inline on
template <class Mediator, int N>
soGenerateArticleManageModuleBuilder<Mediator, N>::~soGenerateArticleManageModuleBuilder() { }
template <class Mediator, int N>
soGenerateArticleManageModuleBuilder<Mediator, N>::soGenerateArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template soGenerateArticleManageModuleBuilder<ftWarioArticleMediator, 5>::~soGenerateArticleManageModuleBuilder();
template soGenerateArticleManageModuleBuilder<ftWarioArticleMediator, 5>::soGenerateArticleManageModuleBuilder(soModuleAccesser*);
template <class PoolList, class Founder>
soArticleMediatorImpl<PoolList, Founder>::~soArticleMediatorImpl() { }
#pragma dont_inline off

template <class PoolList, class Founder>
s32 soArticleMediatorImpl<PoolList, Founder>::getMediateNum() { return 2; }

template <class PoolList, class Founder>
void soArticleMediatorImpl<PoolList, Founder>::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

template <class Level, class W, int N, class Mediator, int S, int L>
inline s32 ftWarioSlotActiveNum(Mediator* mediator, const soIntToType<S>&, const soIntToType<L>&) {
    wnArticleSlotCounter counter;
    counter.m_isActive = wnArticleIsActive;
    counter.m_activeNum = 0;
    counter.m_inactiveNum = 0;
    for (s32 i = 0; i < N; ++i) {
        wnArticleSlotCounter* c = &counter;
        if (c->m_isActive(&static_cast<soArticle&>(*static_cast<Level&>(mediator->getPools()).getSub().getInstanceAt(i))) == true) {
            ++c->m_activeNum;
        } else {
            ++c->m_inactiveNum;
        }
    }
    return counter.m_activeNum;
}

template <class Level, int N, class Mediator, int S, int L>
inline bool ftWarioSlotIsGeneratable(Mediator* mediator, const soIntToType<S>&, const soIntToType<L>&) {
    bool allActive;
    s32 i;
    for (i = 0; i < N; ++i) {
        if (static_cast<soArticle&>(*static_cast<Level&>(mediator->getPools()).getSub().getInstanceAt(i)).isActiveArticle() == false) {
            allActive = false;
            goto done;
        }
    }
    allActive = true;
done:
    return allActive == false;
}

// HYPOTHESIS: pool-level tag parameter matches the one-byte zero temporary in native generate().
template <class W, class Level, class Predicate, int Tag>
inline W* ftWarioFindInPool(Level& pool, Predicate& checker, const soIntToType<Tag>&) {
    return pool.getSub().find(checker);
}

template <class Activator, class W, class Level, int S, int L>
inline soArticle* ftWarioSlotGenerate(Level& pool, soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    soArticleDeactivateChecker checker;
    W* weapon = ftWarioFindInPool<W>(pool, checker, soIntToType<0>());
    if (weapon == NULL) {
        weapon = static_cast<W*>(checker.getCandidate());
        if (weapon == NULL) {
            return wnGetNullArticle();
        }
        weapon->deactivateArticle();
    }
    if (Activator::activeArticle(weapon, acc) == true) {
        return &static_cast<soArticle&>(*weapon);
    }
    return wnGetNullArticle();
}

struct ftWarioBikeActivator {
    static bool activeArticle(wnWarioBike* weapon, soModuleAccesser* acc) {
        return g_ftWarioCommon.activeArticle(weapon, acc);
    }
};

#define FT_WARIO_POOL_BIKE static_cast<ftWarioBikePool&>(m_pools).getSub()
#define FT_WARIO_POOL_GARLIC static_cast<ftWarioGarlicPool&>(m_pools).getSub()

#define FT_WARIO_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) WN_ARTICLE_UNUSED_SLOT(4, value) \
    WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) \
    WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) \
    WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) \
    WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

template <class PoolList, class Founder>
void soArticleMediatorImpl<PoolList, Founder>::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnWarioBike* weapon = FT_WARIO_POOL_BIKE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnWarioGarlic* weapon = FT_WARIO_POOL_GARLIC.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

template <class PoolList, class Founder>
s32 soArticleMediatorImpl<PoolList, Founder>::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    FT_WARIO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

template <class PoolList, class Founder>
s32 soArticleMediatorImpl<PoolList, Founder>::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return ftWarioSlotActiveNum<ftWarioBikePool, wnWarioBike, 1>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return ftWarioSlotActiveNum<ftWarioGarlicPool, wnWarioGarlic, 1>(this, WN_ARTICLE_SLOT_TAGS);
    FT_WARIO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

template <class PoolList, class Founder>
bool soArticleMediatorImpl<PoolList, Founder>::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return ftWarioSlotIsGeneratable<ftWarioBikePool, 1>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return ftWarioSlotIsGeneratable<ftWarioGarlicPool, 1>(this, WN_ARTICLE_SLOT_TAGS);
    FT_WARIO_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class PoolList, class Founder>
soArticle* soArticleMediatorImpl<PoolList, Founder>::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return ftWarioSlotGenerate<ftWarioBikeActivator, wnWarioBike>(static_cast<ftWarioBikePool&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return ftWarioSlotGenerate<Founder, wnWarioGarlic>(static_cast<ftWarioGarlicPool&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_WARIO_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

template <class PoolList, class Founder>
bool soArticleMediatorImpl<PoolList, Founder>::shoot(soModuleAccesser*, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnWarioBike>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotShoot<wnWarioGarlic>(article, WN_ARTICLE_SLOT_TAGS);
    FT_WARIO_UNUSED_SLOTS(true)
    default: return false;
    }
}

template class soArticleMediatorImpl<ftWarioArticleList, ftWario>;

// Placeholder translation-unit functions still named fn_110_* in symbols.txt.
extern "C" {

// Empty virtual overrides and default observers.
void fn_110_9464() {}
void fn_110_9470() {}
void fn_110_94B0() {}
void fn_110_94B4() {}
void fn_110_9544() {}
void fn_110_9570() {}
void fn_110_9574() {}
void fn_110_9578() {}
void fn_110_957C() {}
void fn_110_9580() {}
void fn_110_9584() {}
void fn_110_9588() {}
void fn_110_958C() {}
void fn_110_9590() {}
void fn_110_9594() {}
void fn_110_9598() {}
void fn_110_959C() {}
void fn_110_95A0() {}
void fn_110_95A4() {}
void fn_110_95A8() {}
void fn_110_95B4() {}
void fn_110_95B8() {}
void fn_110_95BC() {}
void fn_110_95C0() {}
void fn_110_95EC() {}
void fn_110_95F0() {}
void fn_110_95F4() {}
void fn_110_95F8() {}
void fn_110_95FC() {}
void fn_110_9620() {}
void fn_110_9624() {}
void fn_110_9628() {}
void fn_110_9634() {}
void fn_110_9638() {}
void fn_110_963C() {}
void fn_110_9648() {}
void fn_110_9654() {}
void fn_110_9658() {}
void fn_110_B824() {}
void fn_110_BF34() {}

// Work-array sizes and field reads.
u8 fn_110_9468(u8* p) { return *(u8*)(p + 0x44); }
int fn_110_97CC(u8* p) { return *(int*)(p + 0x20); }
int fn_110_98A8(u8* p) { return *(int*)(p + 0x18); }
int fn_110_9940(u8* p) { return *(int*)(p + 0x10); }
void fn_110_9920(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_110_9930(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }
void fn_110_99E0(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }
int fn_110_99F0(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); return a[idx]; }

// Sub-object accessors.
u8* fn_110_971C(u8* p) { return p + 0x458; }
u8* fn_110_9724(u8* p) { return p + 0x3C8; }
u8* fn_110_972C(u8* p) { return p + 0x8; }
u8* fn_110_9734(u8* p) { return p + 0x84; }
u8* fn_110_973C(u8* p) { return p + 0x70; }
u8* fn_110_9744(u8* p) { return p + 0x5C; }
u8* fn_110_974C(u8* p) { return p + 0x48; }
u8* fn_110_9754(u8* p) { return p + 0x34; }
u8* fn_110_975C(u8* p) { return p + 0x20; }
u8* fn_110_9764(u8* p) { return p + 0x8; }
u8* fn_110_965C(u8* p, u32 idx) { return p + 4 + (idx << 3); }

// Field reads and constant returns.
int fn_110_9600(u8* p) { return *(int*)(p + 0x110); }
int fn_110_9618(u8* p) { return 0; }
int fn_110_91B8(u8* p) { return 0; }
bool fn_110_9A00(u8* p) { return false; }

// Cancel / pool sub-object accessors on the fighter (+0x10000 base adjusted).
u8* fn_110_A9CC(u8* p) { return p + 0xCD4C; }
u8* fn_110_A9EC(u8* p) { return p + 0xCD88; }
u8* fn_110_A9F8(u8* p) { return p + 0xD240; }

// Ground friction flag at +0x31.
void fn_110_B90C(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_110_B918(u8* p) { *(u8*)(p + 0x31) = 1; }

// Destructor wrappers still named fn_110_* in symbols.txt.
#pragma dont_inline on
void* fn_110_1E6C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_1F08(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_22C4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_2304(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_3A60(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_3D64(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_3E14(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_40EC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_41B4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_451C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_4804(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_4844(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_4FB4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_4FF4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_5034(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_110_5074(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }

void* fn_110_1EAC(void* p, s16 flags) {
    if (p != 0) {
        fn_110_1F08(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_3D08(void* p, s16 flags) {
    if (p != 0) {
        fn_110_3D64(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_408C(void* p, s16 flags) {
    if (p != 0) {
        fn_110_40EC((u8*)p + 0xC, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_6620(void* p, s16 flags) {
    if (p != 0) {
        fn_110_451C(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AAD8(void* p, s16 flags) {
    if (p != 0) {
        fn_110_4FB4(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_ACB8(void* p, s16 flags) {
    if (p != 0) {
        fn_110_4FB4(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B4A0(void* p, s16 flags) {
    if (p != 0) {
        fn_110_4FB4(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B634(void* p, s16 flags) {
    if (p != 0) {
        fn_110_4FB4(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AA78(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_4FB4(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AE44(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_4FB4(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_AFDC(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_4FB4(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B174(void* p, s16 flags) {
    if (p != 0) {
        fn_110_AFDC(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_110_B308(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) { fn_110_4FB4(p, 0); }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
#pragma dont_inline reset

// Field read, constant returns and a global value-variation pointer.
u8 fn_110_6E04(u8* p) { return *(u8*)(p + 0x4); }
int fn_110_962C(u8* p) { return 0; }
int fn_110_9640(u8* p) { return 0x7FAD; }
int fn_110_964C(u8* p) { return 0; }
extern void* g_soValueVariation;
void* fn_110_9CC8() { return g_soValueVariation; }

// Virtual slot tail calls (vtable offset 8+4k for slot k).
struct ftWarioVSlotsB {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
};
struct ftWarioVSlotsI {
    virtual int slot0();
    virtual int slot1();
    virtual int slot2();
    virtual int slot3();
};
void fn_110_AA38(u8* p) { ((ftWarioVSlotsB*)p)->slot20(); }

// Vector zeroing (float triple at the start of the object).
void fn_110_B924(float* p) { p[0] = 0.0f; p[1] = 0.0f; p[2] = 0.0f; }

// Virtual slot 3 result compared against zero.
bool fn_110_AA04(u8* p) { return ((ftWarioVSlotsI*)p)->slot3() == 0; }

// Leaf functions shared with other fighter modules.
u8* fn_110_BFBC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C0A4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C18C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C274(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C35C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C444(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C52C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_110_C614(u8* p, int x) { return x == 0 ? p + 0xC : 0; }

} // extern "C"

// Standard modes use the common fighter dispatcher. Every custom request runs
// preprocessing; only the five recognized modes re-enable outside energy.
void ftWarioKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* acc) {
    if (mode <= 0x63) {
        ftKineticTransactor::changeKinetic(mode, pools, acc);
        return;
    }
    Vec2f speed = ftKineticTransactHelper::preHelpProcess(acc, true, true);
    switch (mode) {
    case 0x64: {
        bool result = false;
        changeKineticSub(&result, pools, &speed, acc);
        break;
    }
    case 0x65: ftKineticTransactor::changeKineticHook(soIntToType<0x65>(), pools, acc); break;
    case 0x66: ftKineticTransactor::changeKineticHook(soIntToType<0x66>(), pools, acc); break;
    case 0x67: ftKineticTransactor::changeKineticHook(soIntToType<0x67>(), pools, acc); break;
    case 0x68: ftKineticTransactor::changeKineticHook(soIntToType<0x68>(), pools, acc); break;
    default:
        return;
    }
    ftKineticTransactor::enableOutsideEnergy(acc);
}

// Mode 0x64 preserves vertical velocity in Gravity and horizontal velocity in
// Stop, enables Damage, and installs Wario's own gravity tuning. The gameplay
// name of this mode is unresolved; its selection may come from motion scripts.
void ftWarioKineticTransactor::changeKineticSub(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    Vec2f initialSpeed = *speed;
    Vec2f gravitySpeed;
    float verticalSpeed = initialSpeed.m_y;
    gravitySpeed.m_x = 0.0f;
    gravitySpeed.m_y = verticalSpeed;
    soKineticUtility::resetEnableEnergy(1, acc, 0, &gravitySpeed, &Vec3f(0.0f, 0.0f, 0.0f));
    Vec2f stopSpeed;
    stopSpeed.m_x = initialSpeed.m_x;
    stopSpeed.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(3, acc, 6, &stopSpeed, &Vec3f(0.0f, 0.0f, 0.0f));
    acc->getKineticModule().getEnergy(4)->enable();
    ftWario& fighter = dynamic_cast<ftWario&>(acc->getStageObject());
    ftWarioBikeRiderParam* param = fighter.getExtendParam()->bikeRider;
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(
        *acc->getKineticModule().getEnergy(1));
    gravity.m_gravity = -param->gravity;
    gravity.unk1C = param->unk4;
}
