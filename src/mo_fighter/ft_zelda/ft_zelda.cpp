#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_NOINLINE_SEARCH_BUILDER
#define FT_ZELDA_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_reflector_group.h>
#include <ft/ft_physics_iface.h>
#include <ft/ft_class_info_impl.h>
#include <ft/zelda/ft_zelda.h>
#include <ft/zelda/ft_zelda_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <ft/ft_owner.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/so_value_accesser.h>

#define FT_BC ftZeldaBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_7DD4;
extern soStatusUniqProcess lbl_104_bss_194;
extern soStatusUniqProcess lbl_104_bss_1A4;
extern soStatusUniqProcess lbl_104_bss_1D4;
extern soStatusUniqProcess lbl_104_bss_1E4;
extern soStatusUniqProcess lbl_104_bss_1B4;
}
extern soStatusUniqProcess g_ftZeldaStatusUniqProcessSpecialHi3;

ftZeldaExtendParamAccesser g_ftZeldaExtendParamAccesser;

ftClassInfoImpl<Fighter_Zelda, ftZelda> g_ftClassInfoZelda;

ftZelda::ftZelda(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftZeldaBuildConfig>(entryId,
                                         Fighter_Zelda,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Zelda)),
    unk18674(0),
    unk18675(0) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[10] = {0};
    processes[0] = &lbl_27_bss_7DD4;
    processes[1] = &lbl_104_bss_194;
    processes[2] = &lbl_104_bss_1A4;
    processes[3] = &lbl_104_bss_1D4;
    processes[4] = &lbl_104_bss_1E4;
    processes[7] = &lbl_104_bss_1B4;
    processes[8] = &g_ftZeldaStatusUniqProcessSpecialHi3;
    processes[9] = &lbl_104_bss_1D4;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 10);
    // The reflector (Nayru's Love): build the reflector group from the common data record (+0x90) and install it as group 2.
    void* reflectorRecord = *reinterpret_cast<void**>(reinterpret_cast<u8*>(m_data) + 0x90);
    u8 shape[0x20];
    ftReflectorGroupDraft group;
    FT_REFLECTOR_GROUP_CLEAR(group);
    fn_27_195B34(reflectorRecord, shape, &group);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&group), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    ftPhysicsModuleCall6C(m_moduleAccesser->m_enumerationStart->m_physicsModule, 0x4B);
}

// HYPOTHESIS: the record at +0x80 of the common data holds the offsets (in the facing direction and up) of the fireball from
// the node of the head.
struct ftZeldaDeinOffset {
    u8 unk0[0xC];
    float m_x;
    float m_y;
};

// Din's Fire starts at the head node of the fighter, moved by the offset of the data record.
bool ftZelda::activeArticle(wnZeldaDein* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soModelModule& model = acc->getModelModule();
    Vec3f pos = model.getNodeGlobalPosition(model.getNodeId("HeadN"), false);
    const ftZeldaDeinOffset* offset = *reinterpret_cast<const ftZeldaDeinOffset* const*>(reinterpret_cast<const u8*>(g_ftCommonDataAccesser.getData(Fighter_Zelda)) + 0x80);
    pos.m_x += offset->m_x * acc->getPostureModule().getLr();
    pos.m_y += offset->m_y;
    pos.m_z = 0.0f;
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, fighter.getOwner()->getTeam(), &pos, acc->getPostureModule().getLr());
    return true;
}

// The light bow of the Final Smash starts at the stage object of the fighter with its team.
bool ftZelda::activeArticle1(wnZeldaLightingBow* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(), acc->getTeamModule().getTeam()->getNo(), 0x45);
    return true;
}

// The light arrow of the Final Smash starts the same way.
bool ftZelda::activeArticle2(wnZeldaLightingBowArrow* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(), acc->getTeamModule().getTeam()->getNo(), 0x63);
    return true;
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Zelda, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftZeldaArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftZeldaArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftZeldaArticleKind : wnArticleKindInfo {
    ftZeldaArticleKind() {
        kind = 0;
        subKind = Fighter_Zelda;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftZeldaArticleBuildInfo : wnArticleBuildInfo {
    ftZeldaArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_148330(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148344(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148358(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14836C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftZeldaArticleTraits;
#define FT_ZELDA_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftZeldaArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftZeldaArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Zelda, &tag.m_resourceGroup)); \
        } \
    };

FT_ZELDA_TRAITS(wnZeldaDein, wnZeldaDeinData, 0, fn_27_148330)
FT_ZELDA_TRAITS(wnZeldaDeinS, wnZeldaDeinSData, 1, fn_27_148344)
FT_ZELDA_TRAITS(wnZeldaLightingBow, wnZeldaLightingBowData, 2, fn_27_148358)
FT_ZELDA_TRAITS(wnZeldaLightingBowArrow, wnZeldaLightingBowArrowData, 3, fn_27_14836C)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftZeldaArticleTraits<W>::ArticleId, ftZeldaArticleBuildInfo(ftZeldaArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftZeldaArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnZeldaDein>::~wnArticleHolder();
template wnArticleHolder<wnZeldaDeinS>::~wnArticleHolder();
template wnArticleHolder<wnZeldaLightingBow>::~wnArticleHolder();
template wnArticleHolder<wnZeldaLightingBowArrow>::~wnArticleHolder();
template wnArticleHolder<wnZeldaDein>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnZeldaDeinS>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftZeldaArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftZeldaArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftZeldaArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftZeldaArticleMediator::~ftZeldaArticleMediator() { }
#pragma dont_inline off

s32 ftZeldaArticleMediator::getMediateNum() { return 4; }
void ftZeldaArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Dein, 1 DeinS, 2 LightingBow, 3 LightingBowArrow (the rest are unused).

#define FT_ZELDA_POOL_DEIN_BASE static_cast<wnArticlePool<wnZeldaDein, 3, ftZeldaDeinSPool>&>(m_pools)
#define FT_ZELDA_POOL_DEIN FT_ZELDA_POOL_DEIN_BASE.getSub()
#define FT_ZELDA_POOL_DEINS_BASE static_cast<wnArticlePool<wnZeldaDeinS, 3, ftZeldaLightingBowPool>&>(m_pools)
#define FT_ZELDA_POOL_DEINS FT_ZELDA_POOL_DEINS_BASE.getSub()
#define FT_ZELDA_POOL_LIGHTINGBOW_BASE static_cast<wnArticlePool<wnZeldaLightingBow, 1, ftZeldaLightingBowArrowPool>&>(m_pools)
#define FT_ZELDA_POOL_LIGHTINGBOW FT_ZELDA_POOL_LIGHTINGBOW_BASE.getSub()
#define FT_ZELDA_POOL_LIGHTINGBOWARROW_BASE static_cast<wnArticlePool<wnZeldaLightingBowArrow, 1, soInstancePoolRoot>&>(m_pools)
#define FT_ZELDA_POOL_LIGHTINGBOWARROW FT_ZELDA_POOL_LIGHTINGBOWARROW_BASE.getSub()

#define FT_ZELDA_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

void ftZeldaArticleMediator::deactivate() {
    for (s32 i = 0; i < 3; ++i) {
        wnZeldaDein* weapon = FT_ZELDA_POOL_DEIN.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 3; ++i) {
        wnZeldaDeinS* weapon = FT_ZELDA_POOL_DEINS.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnZeldaLightingBow* weapon = FT_ZELDA_POOL_LIGHTINGBOW.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnZeldaLightingBowArrow* weapon = FT_ZELDA_POOL_LIGHTINGBOWARROW.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftZeldaArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 3; }
    case 1: { wnArticleSlotTag tag; return 3; }
    case 2: { wnArticleSlotTag tag; return 1; }
    case 3: { wnArticleSlotTag tag; return 1; }
    FT_ZELDA_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftZeldaActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_ZELDA_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftZeldaActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftZeldaActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftZeldaArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_ZELDA_COUNT_ACTIVE(FT_ZELDA_POOL_DEIN, 3)
    case 1: FT_ZELDA_COUNT_ACTIVE(FT_ZELDA_POOL_DEINS, 3)
    case 2: FT_ZELDA_COUNT_ACTIVE(FT_ZELDA_POOL_LIGHTINGBOW, 1)
    case 3: FT_ZELDA_COUNT_ACTIVE(FT_ZELDA_POOL_LIGHTINGBOWARROW, 1)
    FT_ZELDA_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_ZELDA_CAN_GENERATE(POOL, N, ID) \
    { \
        wnArticleSlotTag tag; \
        bool allActive; \
        s32 i; \
        for (i = 0; i < N; ++i) { \
            if (static_cast<soArticle&>(*POOL.getInstanceAt(i)).isActiveArticle() == false) { \
                allActive = false; \
                goto done_##ID; \
            } \
        } \
        allActive = true; \
    done_##ID: \
        return allActive == false; \
    }

bool ftZeldaArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_ZELDA_CAN_GENERATE(FT_ZELDA_POOL_DEIN, 3, 0)
    case 1: FT_ZELDA_CAN_GENERATE(FT_ZELDA_POOL_DEINS, 3, 1)
    case 2: FT_ZELDA_CAN_GENERATE(FT_ZELDA_POOL_LIGHTINGBOW, 1, 2)
    case 3: FT_ZELDA_CAN_GENERATE(FT_ZELDA_POOL_LIGHTINGBOWARROW, 1, 3)
    FT_ZELDA_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftZeldaArticleActivator;
template <> struct ftZeldaArticleActivator<wnZeldaDein> {
    static bool activate(wnZeldaDein* weapon, soModuleAccesser* acc) { return ftZelda::activeArticle(weapon, acc); }
};
template <> struct ftZeldaArticleActivator<wnZeldaDeinS> {
    static bool activate(wnZeldaDeinS* weapon, soModuleAccesser* acc) { return weapon->activeArticle(acc); }
};
template <> struct ftZeldaArticleActivator<wnZeldaLightingBow> {
    static bool activate(wnZeldaLightingBow* weapon, soModuleAccesser* acc) { return ftZelda::activeArticle1(weapon, acc); }
};
template <> struct ftZeldaArticleActivator<wnZeldaLightingBowArrow> {
    static bool activate(wnZeldaLightingBowArrow* weapon, soModuleAccesser* acc) { return ftZelda::activeArticle2(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_ZELDA_GENERATE(W, POOL) \
    { \
        wnArticleSlotTag tag; \
        soArticleDeactivateChecker checker; \
        volatile u8 findTag = 0; /* MATCH-ONLY: the native traversal keeps a one-byte marker */ \
        W* weapon = POOL##_BASE.find(checker); \
        if (weapon == NULL) { \
            weapon = static_cast<W*>(checker.getCandidate()); \
            if (weapon == NULL) { \
                return wnGetNullArticle(); \
            } \
            weapon->deactivateArticle(); \
        } \
        if (ftZeldaArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftZeldaArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_ZELDA_GENERATE(wnZeldaDein, FT_ZELDA_POOL_DEIN)
    case 1: FT_ZELDA_GENERATE(wnZeldaDeinS, FT_ZELDA_POOL_DEINS)
    case 2: FT_ZELDA_GENERATE(wnZeldaLightingBow, FT_ZELDA_POOL_LIGHTINGBOW)
    case 3: FT_ZELDA_GENERATE(wnZeldaLightingBowArrow, FT_ZELDA_POOL_LIGHTINGBOWARROW)
    FT_ZELDA_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftZeldaArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnZeldaDein&>(*article);
        return true;
    }
    case 1: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnZeldaDeinS&>(*article);
        return true;
    }
    case 2: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnZeldaLightingBow&>(*article);
        return true;
    }
    case 3: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnZeldaLightingBowArrow&>(*article);
        return true;
    }
    FT_ZELDA_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftZelda is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftZeldaInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftZeldaInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtuals and no-op helpers.
void fn_104_A024() {}
void fn_104_A470() {}
void fn_104_A5CC() {}
void fn_104_A5D8() {}
void fn_104_A618() {}
void fn_104_A61C() {}
void fn_104_A6AC() {}
void fn_104_A6D8() {}
void fn_104_A6DC() {}
void fn_104_A6E0() {}
void fn_104_A6E4() {}
void fn_104_A6E8() {}
void fn_104_A6EC() {}
void fn_104_A6F0() {}
void fn_104_A6F4() {}
void fn_104_A6F8() {}
void fn_104_A6FC() {}
void fn_104_A700() {}
void fn_104_A704() {}
void fn_104_A708() {}
void fn_104_A70C() {}
void fn_104_A710() {}
void fn_104_A71C() {}
void fn_104_A720() {}
void fn_104_A724() {}
void fn_104_A728() {}
void fn_104_A754() {}
void fn_104_A758() {}
void fn_104_A75C() {}
void fn_104_A760() {}
void fn_104_A764() {}
void fn_104_A788() {}
void fn_104_A78C() {}
void fn_104_A790() {}
void fn_104_A79C() {}
void fn_104_A7A0() {}
void fn_104_A7A4() {}
void fn_104_A7A8() {}
void fn_104_A7AC() {}
void fn_104_A7B8() {}
void fn_104_98C0() {}
void fn_104_CDA8() {}
void fn_104_D4B8() {}
void fn_104_D53C() {}
void fn_104_D624() {}
void fn_104_D70C() {}
void fn_104_D7F4() {}
void fn_104_D8DC() {}
void fn_104_D9C4() {}
void fn_104_DAAC() {}
void fn_104_DB94() {}

// Constant returns.
int fn_104_A304() { return 0; }
int fn_104_A714() { return 0; }
int fn_104_A780() { return 0; }
int fn_104_A794() { return 0; }
int fn_104_A7BC() { return 0; }
int fn_104_AB68() { return 0; }
int fn_104_D5E8() { return 0; }
int fn_104_D6D0() { return 0; }
int fn_104_D7B8() { return 0; }
int fn_104_D8A0() { return 0; }
int fn_104_D988() { return 0; }
int fn_104_DA70() { return 0; }
int fn_104_DB58() { return 0; }
int fn_104_DC40() { return 0; }
int fn_104_A7B0() { return 0x7ffe; }
int fn_104_BF28() { return 4; }
int fn_104_D4F4() { return 8; }

// Field getters.
u8 fn_104_7930(u8* p) { return *(u8*)(p + 0x4); }
u8 fn_104_A5D0(u8* p) { return *(u8*)(p + 0x44); }
int fn_104_A768(u8* p) { return *(int*)(p + 0x110); }
int fn_104_A934(u8* p) { return *(int*)(p + 0x20); }
int fn_104_AA10(u8* p) { return *(int*)(p + 0x18); }
int fn_104_AAA8(u8* p) { return *(int*)(p + 0x10); }

// Sub-object pointers.
u8* fn_104_A884(u8* p) { return p + 0x458; }
u8* fn_104_A88C(u8* p) { return p + 0x3C8; }
u8* fn_104_A894(u8* p) { return p + 0x8; }
u8* fn_104_A89C(u8* p) { return p + 0x84; }
u8* fn_104_A8A4(u8* p) { return p + 0x70; }
u8* fn_104_A8AC(u8* p) { return p + 0x5C; }
u8* fn_104_A8B4(u8* p) { return p + 0x48; }
u8* fn_104_A8BC(u8* p) { return p + 0x34; }
u8* fn_104_A8C4(u8* p) { return p + 0x20; }
u8* fn_104_A8CC(u8* p) { return p + 0x8; }
u8* fn_104_A7C4(u8* p, u32 idx) { return p + 4 + (idx << 3); }

// Byte flags.
void fn_104_CE90(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_104_CE9C(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_104_D4FC(s16* dst, s16* src) { *dst = *src; }

// Bit flags in the u32 array at +0x1C.
bool fn_104_A9B0(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_104_A9CC(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_104_A9E4(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_104_A9F8(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_104_AA18(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
void fn_104_AA40(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_104_AA58(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_104_AA70(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_104_AA88(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_104_AA98(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_104_AAB0(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_104_AAC8(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_104_AAE0(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_104_AB00(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_104_AB18(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_104_AB30(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_104_AB48(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }

// Conditional sub-object pointers selected by an index.
u8* fn_104_BFB8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_104_BFD0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_104_BFE8(u8* p, int x) {
    if (x == 2) {
        return p + 0x3E4C;
    }
    if (x == 1) {
        return p + 0x1F30;
    }
    if (x == 0) {
        return p + 0x14;
    }
    return 0;
}
u8* fn_104_C020(u8* p, int x) {
    if (x == 2) {
        return p + 0x41CC;
    }
    if (x == 1) {
        return p + 0x20F0;
    }
    if (x == 0) {
        return p + 0x14;
    }
    return 0;
}

// Single-store and pointer-chain accessors.
void fn_104_BF30(u8* p, u8 v) { *(u8*)(p + 0x10000 - 0x17C) = v; }
u8* fn_104_9974(u8* p) { return *(u8**)(p + 0x18670) + 0x7C; }

// Virtual destructors: delete the object when the flag is positive, then return it.
void* fn_104_1E70(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3804(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3C90(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3CD0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3D10(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3E0C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_3EE8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_421C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_4618(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_4978(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_49B8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_104_6AD0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }

// Polymorphic view used only to emit virtual calls; the slots are never defined in this unit.
struct ftZeldaFlagByte {
    u8 flag : 1;
    u8 rest : 7;
};
struct ftZeldaVirtualSlots {
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

// Calls the slot-1 virtual of the object when byte 5 bit 7 is set and byte 6 is clear.

// Tail-call virtual slot (offset 0x58) of the object.
void fn_104_BFA8(void* p) { return ((ftZeldaVirtualSlots*)p)->slot20(); }

}
