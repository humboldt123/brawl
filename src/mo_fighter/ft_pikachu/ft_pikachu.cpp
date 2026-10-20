// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PIKACHU_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/pikachu/ft_pikachu.h>
#include <ft/pikachu/ft_pikachu_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <ft/pikachu/ft_pikachu_transactor.h>
#include <so/so_value_accesser.h>
#include <so/so_value_accesser.h>
#include <ft/ft_common_data_accesser.h>

// ftManager::setParamPattern selects the shared parameter-table variation (same accessor as ft_marth.cpp).
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

#define FT_BC ftPikachuBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_7C64;
extern soStatusUniqProcess lbl_98_bss_25C;
extern soStatusUniqProcess lbl_98_bss_22C;
extern soStatusUniqProcess lbl_98_bss_2AC;
extern soStatusUniqProcess lbl_98_bss_2CC;
extern soStatusUniqProcess lbl_98_bss_23C;
extern soStatusUniqProcess lbl_98_bss_24C;
extern soStatusUniqProcess lbl_98_bss_26C;
extern soStatusUniqProcess lbl_98_bss_27C;
extern soStatusUniqProcess lbl_98_bss_28C;
extern soStatusUniqProcess lbl_98_bss_29C;
extern soStatusUniqProcess lbl_98_bss_2BC;
extern soStatusUniqProcess lbl_98_bss_2DC;
extern soStatusUniqProcess lbl_98_bss_2EC;
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
    processes[1] = &lbl_98_bss_25C;
    processes[2] = &lbl_98_bss_22C;
    processes[3] = &lbl_98_bss_2AC;
    processes[4] = &lbl_98_bss_2CC;
    processes[5] = &lbl_98_bss_23C;
    processes[6] = &lbl_98_bss_24C;
    processes[7] = &lbl_98_bss_26C;
    processes[8] = &lbl_98_bss_27C;
    processes[9] = &lbl_98_bss_28C;
    processes[10] = &lbl_98_bss_29C;
    processes[11] = &lbl_98_bss_2BC;
    processes[12] = &lbl_98_bss_2DC;
    processes[13] = &lbl_98_bss_2EC;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 14);
}

// The Thunder Jolt starts through the transactor of the fighter with the effect resource of the fighter.
// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
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
extern "C" void* fn_27_148170(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148184(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
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

FT_PIKACHU_TRAITS(wnPikachuThunderJolt, wnPikachuThunderJoltData, 0, fn_27_148050)
FT_PIKACHU_TRAITS(wnPikachuCloud, wnPikachuCloudData, 1, fn_27_148170)
FT_PIKACHU_TRAITS(wnPikachuVortex, wnPikachuVortexData, 2, fn_27_148184)
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
template wnArticleHolder<wnPikachuThunderJolt>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnPikachuCloud>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnPikachuThunder>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftPikachuArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftPikachuArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftPikachuArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftPikachuArticleMediator::~ftPikachuArticleMediator() { }
#pragma dont_inline off

s32 ftPikachuArticleMediator::getMediateNum() { return 4; }
void ftPikachuArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

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
    case 0: { wnArticleSlotTag tag; return 3; }
    case 1: { wnArticleSlotTag tag; return 2; }
    case 2: { wnArticleSlotTag tag; return 1; }
    case 3: { wnArticleSlotTag tag; return 3; }
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

#define FT_PIKACHU_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftPikachuActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftPikachuActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftPikachuArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_PIKACHU_COUNT_ACTIVE(FT_PIKACHU_POOL_THUNDERJOLT, 3)
    case 1: FT_PIKACHU_COUNT_ACTIVE(FT_PIKACHU_POOL_CLOUD, 2)
    case 2: FT_PIKACHU_COUNT_ACTIVE(FT_PIKACHU_POOL_VORTEX, 1)
    case 3: FT_PIKACHU_COUNT_ACTIVE(FT_PIKACHU_POOL_THUNDER, 3)
    FT_PIKACHU_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_PIKACHU_CAN_GENERATE(POOL, N, ID) \
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

bool ftPikachuArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_PIKACHU_CAN_GENERATE(FT_PIKACHU_POOL_THUNDERJOLT, 3, 0)
    case 1: FT_PIKACHU_CAN_GENERATE(FT_PIKACHU_POOL_CLOUD, 2, 1)
    case 2: FT_PIKACHU_CAN_GENERATE(FT_PIKACHU_POOL_VORTEX, 1, 2)
    case 3: FT_PIKACHU_CAN_GENERATE(FT_PIKACHU_POOL_THUNDER, 3, 3)
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

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_PIKACHU_GENERATE(W, POOL) \
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
        if (ftPikachuArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftPikachuArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_PIKACHU_GENERATE(wnPikachuThunderJolt, FT_PIKACHU_POOL_THUNDERJOLT)
    case 1: FT_PIKACHU_GENERATE(wnPikachuCloud, FT_PIKACHU_POOL_CLOUD)
    case 2: FT_PIKACHU_GENERATE(wnPikachuVortex, FT_PIKACHU_POOL_VORTEX)
    case 3: FT_PIKACHU_GENERATE(wnPikachuThunder, FT_PIKACHU_POOL_THUNDER)
    FT_PIKACHU_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftPikachuArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnPikachuThunderJolt&>(*article);
        return true;
    }
    case 1: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnPikachuCloud&>(*article);
        return true;
    }
    case 2: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnPikachuVortex&>(*article);
        return true;
    }
    case 3: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnPikachuThunder&>(*article);
        return true;
    }
    FT_PIKACHU_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftPikachu is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftPikachuInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftPikachuInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_98_744C(u8* p) { return *(u8*)(p + 0x4); }
void fn_98_939C() {}
int fn_98_98C0() { return 0x0; }
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
int fn_98_9D6C() { return 0x0; }
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
int fn_98_9DD8() { return 0x0; }
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
int fn_98_CFC0() { return 0x0; }
int fn_98_D25C() { return 0x0; }
int fn_98_D4F4() { return 0x0; }
int fn_98_D790() { return 0x0; }
int fn_98_DA2C() { return 0x0; }
int fn_98_DCC8() { return 0x0; }
int fn_98_DF64() { return 0x0; }
int fn_98_E200() { return 0x0; }

// soArticleMediatorImpl<...wnPikachuThunderJolt...>::setAutoRecycle: stores the flag at this+0x22A2C.
void fn_98_B5A4(u8* p, u8 flag) { *(u8*)(p + 0x22A2C) = flag; }

// ftPikachu::notify_thunder_reflect / adjust_camera_subject: forward this+0x60 to the thunder-hit process handlers.
extern "C" int fn_98_11344(void* p);
extern "C" int fn_98_11B40(void* p);
int fn_98_98B0(u8* p) { return fn_98_11344(*(void**)(p + 0x60)); }
int fn_98_98B8(u8* p) { return fn_98_11B40(*(void**)(p + 0x60)); }

// ftPikachu::getExtendParam: the extend-param block pointer stored at the end of the fighter, offset 0x7C.
void* fn_98_9530(u8* p) { return *(u8**)(p + 0x2AE44) + 0x7C; }

// soInstancePoolSub::getInstanceAt for the wnPikachu* instance holders: instance slots by index, null past the last one.
void* fn_98_B62C(u8* p, int index) {
    if (index == 2) return p + 0x3FA4;
    if (index == 1) return p + 0x1FDC;
    if (index == 0) return p + 0x14;
    return 0;
}
void* fn_98_B664(u8* p, int index) {
    if (index == 0) return p + 0xC;
    return 0;
}
void* fn_98_B67C(u8* p, int index) {
    if (index == 1) return p + 0xA610;
    if (index == 0) return p + 0x10;
    return 0;
}
void* fn_98_B6A8(u8* p, int index) {
    if (index == 2) return p + 0x3F44;
    if (index == 1) return p + 0x1FAC;
    if (index == 0) return p + 0x14;
    return 0;
}

// soKineticMediatorImpl::addSpeed:forwards to the transactor with the pools at this+4.
void fn_98_CCE0(u8* p, void* speed, soModuleAccesser* acc) { ftKineticTransactor::addSpeed(speed, p + 4, acc); }

// ftPikachu::getThunderParam:the common fighter data entry for Pikachu, field at 0x88.
void* fn_98_9540() { return *(void**)((u8*)g_ftCommonDataAccesser.getData(Fighter_Pikachu) + 0x88); }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_98_E424(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_98_E444(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_98_E454(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
bool fn_98_E45C(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_98_E464(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_98_E46C(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_98_E474(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_98_E48C(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_98_E4A4(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_98_E4B4(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_98_E4BC(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
bool fn_98_E4C4(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_98_E4CC(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_98_E4D4(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_98_E4DC(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_98_E4E4(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_98_E4EC(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_98_E4F4(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_98_E4FC(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_98_E504(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_98_E50C(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_98_E514(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_98_E51C(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_98_E524(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_98_E52C(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_98_E534(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_98_E53C(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_98_E544(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_98_E54C(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
bool fn_98_E434(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_98_E47C(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_98_E484(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_98_E494(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_98_E49C(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_98_E4AC(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_98_1DD8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4048(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4480(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_47F8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4B58(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4B98(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4D60(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4DA0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_98_4DE0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"
