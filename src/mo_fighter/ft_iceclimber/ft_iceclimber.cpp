// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define FT_FIGHTER_ANIMCMD_LONG // MATCH-ONLY: Fighter::notifyEventAnimCmd(long) overrides the StageObject slot (as in ft_marth.cpp)
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_POPO_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_manager.h>
#include <ft/ft_class_info_impl.h>
#include <ft/iceclimber/ft_iceclimber.h>
#include <ft/iceclimber/ft_iceclimber_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <ft/iceclimber/ft_iceclimber_transactor.h>

#define FT_BC ftPopoBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt); the repeated ones are the null placeholders of the
// unused status ranges.
extern "C" {
extern soStatusUniqProcess lbl_105_bss_28C;
extern soStatusUniqProcess lbl_105_bss_2AC;
extern soStatusUniqProcess lbl_105_bss_2FC;
extern soStatusUniqProcess lbl_105_bss_2DC;
extern soStatusUniqProcess lbl_105_bss_29C;
extern soStatusUniqProcess lbl_105_bss_2BC;
extern soStatusUniqProcess lbl_105_bss_2CC;
extern soStatusUniqProcess lbl_105_bss_2EC;
extern soStatusUniqProcess lbl_105_bss_27C;
extern soStatusUniqProcess lbl_105_bss_30C;
extern soStatusUniqProcess lbl_105_bss_31C;
}

ftPopoExtendParamAccesser g_ftPopoExtendParamAccesser;

ftClassInfoImpl<Fighter_Popo, ftPopo> g_ftClassInfoPopo;

ftPopo::ftPopo(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPopoBuildConfig>(entryId,
                                         Fighter_Popo,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Popo)),
    m_isPartner(false) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[15] = {0};
    processes[1] = &lbl_105_bss_28C;
    processes[2] = &lbl_105_bss_2AC;
    processes[3] = &lbl_105_bss_2FC;
    processes[4] = &lbl_105_bss_2DC;
    processes[5] = &lbl_105_bss_29C;
    processes[8] = &lbl_105_bss_2BC;
    processes[12] = &lbl_105_bss_2CC;
    processes[14] = &lbl_105_bss_2EC;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 15);
    m_moduleAccesser->getStatusModule().setUniqProc(0x74, &lbl_105_bss_27C);
    m_moduleAccesser->getStatusModule().setUniqProc(0x10E, &lbl_105_bss_30C);
    m_moduleAccesser->getStatusModule().setUniqProc(0xBE, &lbl_105_bss_31C);
}

// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
#pragma dont_inline on
ftPopoTransactor* ftPopoTransactor::getInstance() {
    static ftPopoTransactor instance;
    return &instance;
}
#pragma dont_inline off
#pragma pool_data reset

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Popo, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftPopoArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftPopoArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftPopoArticleKind : wnArticleKindInfo {
    ftPopoArticleKind() {
        kind = 0;
        subKind = Fighter_Popo;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftPopoArticleBuildInfo : wnArticleBuildInfo {
    ftPopoArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_1491C4(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1491DC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14925C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_149274(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14928C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftPopoArticleTraits;
#define FT_POPO_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftPopoArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftPopoArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Popo, &tag.m_resourceGroup)); \
        } \
    };

FT_POPO_TRAITS(wnPopoBlizzard, wnPopoBlizzardData, 0, fn_27_1491C4)
FT_POPO_TRAITS(wnPopoIceShot, wnPopoIceShotData, 1, fn_27_1491DC)
FT_POPO_TRAITS(wnPopoIceBerg, wnPopoIceBergData, 2, fn_27_14925C)
FT_POPO_TRAITS(wnPopoRubber, wnPopoRubberData, 3, fn_27_149274)
FT_POPO_TRAITS(wnPopoIceBergWind, wnPopoIceBergWindData, 4, fn_27_14928C)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftPopoArticleTraits<W>::ArticleId, ftPopoArticleBuildInfo(ftPopoArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftPopoArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnPopoBlizzard>::~wnArticleHolder();
template wnArticleHolder<wnPopoIceShot>::~wnArticleHolder();
template wnArticleHolder<wnPopoIceBerg>::~wnArticleHolder();
template wnArticleHolder<wnPopoRubber>::~wnArticleHolder();
template wnArticleHolder<wnPopoIceBergWind>::~wnArticleHolder();
template wnArticleHolder<wnPopoBlizzard>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnPopoIceShot>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftPopoArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftPopoArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftPopoArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftPopoArticleMediator::~ftPopoArticleMediator() { }
#pragma dont_inline off

s32 ftPopoArticleMediator::getMediateNum() { return 5; }
void ftPopoArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Blizzard, 1 IceShot, 2 IceBerg, 3 Rubber, 4 IceBergWind (the rest are unused).

#define FT_POPO_POOL_BLIZZARD_BASE static_cast<wnArticlePool<wnPopoBlizzard, 3, ftPopoIceShotPool>&>(m_pools)
#define FT_POPO_POOL_BLIZZARD FT_POPO_POOL_BLIZZARD_BASE.getSub()
#define FT_POPO_POOL_ICESHOT_BASE static_cast<wnArticlePool<wnPopoIceShot, 5, ftPopoIceBergPool>&>(m_pools)
#define FT_POPO_POOL_ICESHOT FT_POPO_POOL_ICESHOT_BASE.getSub()
#define FT_POPO_POOL_ICEBERG_BASE static_cast<wnArticlePool<wnPopoIceBerg, 1, ftPopoRubberPool>&>(m_pools)
#define FT_POPO_POOL_ICEBERG FT_POPO_POOL_ICEBERG_BASE.getSub()
#define FT_POPO_POOL_RUBBER_BASE static_cast<wnArticlePool<wnPopoRubber, 1, ftPopoIceBergWindPool>&>(m_pools)
#define FT_POPO_POOL_RUBBER FT_POPO_POOL_RUBBER_BASE.getSub()
#define FT_POPO_POOL_ICEBERGWIND_BASE static_cast<wnArticlePool<wnPopoIceBergWind, 1, soInstancePoolRoot>&>(m_pools)
#define FT_POPO_POOL_ICEBERGWIND FT_POPO_POOL_ICEBERGWIND_BASE.getSub()

#define FT_POPO_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) \
    WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) \
    WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) \
    WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftPopoArticleMediator::deactivate() {
    for (s32 i = 0; i < 3; ++i) {
        wnPopoBlizzard* weapon = FT_POPO_POOL_BLIZZARD.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 5; ++i) {
        wnPopoIceShot* weapon = FT_POPO_POOL_ICESHOT.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnPopoIceBerg* weapon = FT_POPO_POOL_ICEBERG.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnPopoRubber* weapon = FT_POPO_POOL_RUBBER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnPopoIceBergWind* weapon = FT_POPO_POOL_ICEBERGWIND.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftPopoArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 3; }
    case 1: { wnArticleSlotTag tag; return 5; }
    case 2: { wnArticleSlotTag tag; return 1; }
    case 3: { wnArticleSlotTag tag; return 1; }
    case 4: { wnArticleSlotTag tag; return 1; }
    FT_POPO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftPopoActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_POPO_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftPopoActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftPopoActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftPopoArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_POPO_COUNT_ACTIVE(FT_POPO_POOL_BLIZZARD, 3)
    case 1: FT_POPO_COUNT_ACTIVE(FT_POPO_POOL_ICESHOT, 5)
    case 2: FT_POPO_COUNT_ACTIVE(FT_POPO_POOL_ICEBERG, 1)
    case 3: FT_POPO_COUNT_ACTIVE(FT_POPO_POOL_RUBBER, 1)
    case 4: FT_POPO_COUNT_ACTIVE(FT_POPO_POOL_ICEBERGWIND, 1)
    FT_POPO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_POPO_CAN_GENERATE(POOL, N, ID) \
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

bool ftPopoArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_POPO_CAN_GENERATE(FT_POPO_POOL_BLIZZARD, 3, 0)
    case 1: FT_POPO_CAN_GENERATE(FT_POPO_POOL_ICESHOT, 5, 1)
    case 2: FT_POPO_CAN_GENERATE(FT_POPO_POOL_ICEBERG, 1, 2)
    case 3: FT_POPO_CAN_GENERATE(FT_POPO_POOL_RUBBER, 1, 3)
    case 4: FT_POPO_CAN_GENERATE(FT_POPO_POOL_ICEBERGWIND, 1, 4)
    FT_POPO_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftPopoArticleActivator;
template <> struct ftPopoArticleActivator<wnPopoBlizzard> {
    static bool activate(wnPopoBlizzard* weapon, soModuleAccesser* acc) { return ftPopo::activeArticle(weapon, acc); }
};
template <> struct ftPopoArticleActivator<wnPopoIceShot> {
    static bool activate(wnPopoIceShot* weapon, soModuleAccesser* acc) {
        u32 etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
        return ftPopoTransactor::getInstance()->activeArticle1(weapon, acc, etcResId);
    }
};
template <> struct ftPopoArticleActivator<wnPopoIceBerg> {
    static bool activate(wnPopoIceBerg* weapon, soModuleAccesser* acc) { return ftPopo::activeArticle1(weapon, acc); }
};
template <> struct ftPopoArticleActivator<wnPopoRubber> {
    static bool activate(wnPopoRubber* weapon, soModuleAccesser* acc) { return ftPopo::activeArticle2(weapon, acc); }
};
template <> struct ftPopoArticleActivator<wnPopoIceBergWind> {
    static bool activate(wnPopoIceBergWind* weapon, soModuleAccesser* acc) { return wnPopoIceBerg::activeArticle(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_POPO_GENERATE(W, POOL) \
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
        if (ftPopoArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftPopoArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_POPO_GENERATE(wnPopoBlizzard, FT_POPO_POOL_BLIZZARD)
    case 1: FT_POPO_GENERATE(wnPopoIceShot, FT_POPO_POOL_ICESHOT)
    case 2: FT_POPO_GENERATE(wnPopoIceBerg, FT_POPO_POOL_ICEBERG)
    case 3: FT_POPO_GENERATE(wnPopoRubber, FT_POPO_POOL_RUBBER)
    case 4: FT_POPO_GENERATE(wnPopoIceBergWind, FT_POPO_POOL_ICEBERGWIND)
    FT_POPO_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftPopoArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnPopoBlizzard&>(*article);
        return true;
    }
    case 1: {
        wnArticleSlotTag tag;
        dynamic_cast<wnPopoIceShot&>(*article).shoot();
        return false;
    }
    case 2: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnPopoIceBerg&>(*article);
        return true;
    }
    case 3: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnPopoRubber&>(*article);
        return true;
    }
    case 4: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnPopoIceBergWind&>(*article);
        return true;
    }
    FT_POPO_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftPopo is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftPopoInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftPopoInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_105_8130(u8* p) { return *(u8*)(p + 0x4); }
void fn_105_A0DC() {}
void fn_105_AAFC() {}
void fn_105_AB14() {}
void fn_105_AB2C() {}
void fn_105_AB44() {}
void fn_105_AF48() {}
u8 fn_105_AF4C(u8* p) { return *(u8*)(p + 0x44); }
void fn_105_BF14() {}
int fn_105_D540() { return 0x0; }
void fn_105_D76C() {}
void fn_105_D8E4() {}
void fn_105_D924() {}
void fn_105_D928() {}
void fn_105_D9E0() {}
void fn_105_D9E4() {}
void fn_105_D9E8() {}
void fn_105_D9EC() {}
void fn_105_D9F0() {}
void fn_105_D9F4() {}
void fn_105_D9F8() {}
void fn_105_D9FC() {}
void fn_105_DA00() {}
void fn_105_DA04() {}
void fn_105_DA08() {}
void fn_105_DA0C() {}
void fn_105_DA10() {}
void fn_105_DA14() {}
void fn_105_DA18() {}
int fn_105_DA1C() { return 0x0; }
void fn_105_DA24() {}
void fn_105_DA28() {}
void fn_105_DA2C() {}
void fn_105_DA30() {}
void fn_105_DA5C() {}
void fn_105_DA60() {}
void fn_105_DA64() {}
void fn_105_DA68() {}
void fn_105_DA6C() {}
int fn_105_DA70(u8* p) { return *(int*)(p + 0x110); }
int fn_105_DA88() { return 0x0; }
void fn_105_DA90() {}
void fn_105_DA94() {}
void fn_105_DA98() {}
int fn_105_DA9C() { return 0x0; }
int fn_105_DAA4() { return 0x0; }
void fn_105_DAAC() {}
void fn_105_DAB0() {}
void fn_105_DAB4() {}
void fn_105_DAB8() {}
void fn_105_DABC() {}
void fn_105_DAC0() {}
void fn_105_DAC4() {}
void fn_105_DAC8() {}
void fn_105_DACC() {}
void fn_105_DAD0() {}
int fn_105_DAD4() { return -0x1; }
int fn_105_DAEC(u8* p) { return *(int*)(p + 0xc); }
int fn_105_DAF4() { return 0x0; }
void fn_105_DAFC() {}
void fn_105_DB00(u8* p, int v) { *(int*)(p + 0xc) = v; }
int fn_105_DC18(u8* p) { return *(int*)(p + 0x20); }
int fn_105_DCF4(u8* p) { return *(int*)(p + 0x18); }
int fn_105_DD8C(u8* p) { return *(int*)(p + 0x10); }
int fn_105_DE4C() { return 0x0; }
int fn_105_F458() { return 0x5; }
void fn_105_1038C() {}
void fn_105_10D70() {}
int fn_105_10DAC() { return 0x8; }
int fn_105_11194() { return 0x0; }
int fn_105_11564() { return 0x0; }
int fn_105_11930() { return 0x0; }
int fn_105_11CF4() { return 0x0; }
int fn_105_120D0() { return 0x0; }
int fn_105_124AC() { return 0x0; }
int fn_105_12888() { return 0x0; }
int fn_105_12C64() { return 0x0; }

}

// "Delete when flag > 0" destructors (identical bodies; one per map class name).
extern "C" {
#pragma dont_inline on
void* fn_105_1E2C(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // ftVirtualNodeMatrixPool____dt
void* fn_105_3674(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soParamAccesser____dt
void* fn_105_3D5C(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soInstanceManagerFullProperty_P15soKineticEnergy_____dt
void* fn_105_41E8(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soInstanceManagerFullProperty_20soAnimCmdControlUnit_____dt
void* fn_105_45E4(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soTeam____dt
void* fn_105_48D8(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soResourceIdAccesser____dt
void* fn_105_4918(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soParamCustomizeModuleBuilder dt
void* fn_105_628C(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soArticleMediator____dt
void* fn_105_62CC(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soArticleOperator____dt
void* fn_105_630C(void* p, s32 flags) { if (p != 0 && (s16)flags > 0) { ::operator delete(p); } return p; } // soArticleGenerator____dt
#pragma dont_inline reset
}

// Flag-free register-offset helpers (leaf getters returning sub-object addresses by index).
extern "C" {
void fn_105_F460(u8* p, u8 v) { *(u8*)(p + 0x17A10) = v; } // soArticleMediatorImpl setAutoRecycle
void* fn_105_F4E8(u8* p, s32 index) { if (index == 0) { return p + 0xc; } return 0; } // soInstancePoolSub getInstanceAt
void* fn_105_F500(u8* p, s32 index) { if (index == 0) { return p + 0xc; } return 0; } // soInstancePoolSub getInstanceAt
void* fn_105_F518(u8* p, s32 index) { if (index == 0) { return p + 0xc; } return 0; } // soInstancePoolSub getInstanceAt
void* fn_105_F58C(u8* p, s32 index) {
    if (index == 2) { return p + 0x3eb4; }
    if (index == 1) { return p + 0x1f64; }
    if (index == 0) { return p + 0x14; }
    return 0;
} // soInstancePoolSub_74...wnPopoBlizzard getInstanceAt
void* fn_105_F530(u8* p, s32 index) {
    if (index == 4) { return p + 0x8f6c; }
    if (index == 3) { return p + 0x6b98; }
    if (index == 2) { return p + 0x47c4; }
    if (index == 1) { return p + 0x23f0; }
    if (index == 0) { return p + 0x1c; }
    return 0;
} // soInstancePoolSub_73...wnPopoIceShot getInstanceAt
}

// Fighter-base this-adjusting thunks: subtract the sub-object offset, then tail call the Fighter method.
extern "C" {
void* fn_105_12E68(void* p, s32 flags) { return fn_105_628C((u8*)p - 0x4, flags); } // Fighter-base dtor thunk for soArticleMediator
void fn_105_12E88(u8* p, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) { ((Fighter*)(p - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObject, unk4); } // Fighter::notifyEventLink
void fn_105_12E90(u8* p, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); } // Fighter::notifyEventChangeStatus
void fn_105_12E98(u8* p, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); } // Fighter::notifyEventChangeSituation
void fn_105_12EA0(u8* p, BaseItem* item, u32 nodeIndex, int* unk) { ((Fighter*)(p - 0xd0))->Fighter::notifyUseItem(item, nodeIndex, unk); } // Fighter::notifyUseItem
#pragma dont_inline on
bool fn_105_12EB8(u8* p, int unk1) { return ((Fighter*)(p - 0x48))->Fighter::isObserv(unk1); } // Fighter::isObserv (header inline body must not be folded in)
#pragma dont_inline reset
void fn_105_12ED8(u8* p, u32 flags) { ((Fighter*)(p - 0x7c))->Fighter::notifyEventCollisionAttackCheck(flags); } // Fighter::notifyEventCollisionAttackCheck
void fn_105_12EE0(u8* p, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x7c))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); } // Fighter::notifyEventCollisionAttack
void fn_105_12EE8(u8* p, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); } // Fighter::notifyEventChangeCollisionHit
void fn_105_12EF0(u8* p) { ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShieldCheck(); } // Fighter::notifyEventCollisionShieldCheck
void fn_105_12EF8(u8* p, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); } // Fighter::notifyEventCollisionShieldSearch
void fn_105_12F00(u8* p, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); } // Fighter::notifyEventCollisionShield
void fn_105_12F08(u8* p) { ((Fighter*)(p - 0xa0))->Fighter::notifyEventCollisionReflectorCheck(); } // Fighter::notifyEventCollisionReflectorCheck
void fn_105_12F10(u8* p, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xa0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); } // Fighter::notifyEventCollisionReflectorSearch
void fn_105_12F18(u8* p, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xa0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); } // Fighter::notifyEventCollisionReflector
void fn_105_12F20(u8* p) { ((Fighter*)(p - 0xac))->Fighter::notifyEventCollisionAbsorberCheck(); } // Fighter::notifyEventCollisionAbsorberCheck
void fn_105_12F28(u8* p, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(p - 0xac))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); } // Fighter::notifyEventCollisionAbsorber
void fn_105_12F30(u8* p) { ((Fighter*)(p - 0xb8))->Fighter::notifyEventCollisionSearchCheck(); } // Fighter::notifyEventCollisionSearchCheck
void fn_105_12F38(u8* p, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xb8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); } // Fighter::notifyEventCollisionSearch
void fn_105_12F40(u8* p, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { ((Fighter*)(p - 0xc4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); } // Fighter::notifyEventCaptureStatus
void fn_105_12F48(u8* p, BaseItem* item, int index, bool unk2, u8 unk3) { ((Fighter*)(p - 0xd0))->Fighter::notifyVisibilityItem(item, index, unk2, unk3); } // Fighter::notifyVisibilityItem
void fn_105_12F50(u8* p, BaseItem* item, u32 index, bool unk2) { ((Fighter*)(p - 0xd0))->Fighter::notifyEjectAttachItem(item, index, unk2); } // Fighter::notifyEjectAttachItem
void fn_105_12F58(u8* p, BaseItem* item, u32 index, bool unk2) { ((Fighter*)(p - 0xd0))->Fighter::notifyEjectItem(item, index, unk2); } // Fighter::notifyEjectItem
void fn_105_12F60(u8* p, BaseItem* item) { ((Fighter*)(p - 0xd0))->Fighter::notifyShootBulletItem(item); } // Fighter::notifyShootBulletItem
void fn_105_12F68(u8* p, BaseItem* item) { ((Fighter*)(p - 0xd0))->Fighter::notifyDropItem(item); } // Fighter::notifyDropItem
void fn_105_12F70(u8* p, BaseItem* item, u32 nodeIndex, int* unk) { ((Fighter*)(p - 0xd0))->Fighter::notifyThrowItem(item, nodeIndex, unk); } // Fighter::notifyThrowItem
void fn_105_12F80(u8* p, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk1, bool unk2) { ((Fighter*)(p - 0xd0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk1, unk2); } // Fighter::notifyAttachItem
void fn_105_12F88(u8* p, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk) { ((Fighter*)(p - 0xd0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk); } // Fighter::notifyHaveItem
void fn_105_12F90(u8* p, BaseItem* item, bool* unk) { ((Fighter*)(p - 0xd0))->Fighter::notifyHaveItemPreCheck(item, unk); } // Fighter::notifyHaveItemPreCheck
void fn_105_12FA0(u8* p, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xe8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); } // Fighter::notifyEventAddDamage
void fn_105_12FA8(u8* p, soDamage* damage, bool unk, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0xe8))->Fighter::notifyEventOnDamage(damage, unk, moduleAccesser); } // Fighter::notifyEventOnDamage
void fn_105_12FB0(u8* p, float power, int unk) { ((Fighter*)(p - 0xf4))->Fighter::notifyEventPikminFinalAttack(power, unk); } // Fighter::notifyEventPikminFinalAttack
void fn_105_12FB8(u8* p) { ((Fighter*)(p - 0xf4))->Fighter::notifyEventChangeAdvUnit(); } // Fighter::notifyEventChangeAdvUnit
void fn_105_12FC0(u8* p) { ((Fighter*)(p - 0xf4))->Fighter::notifyEventBeat(); } // Fighter::notifyEventBeat
void fn_105_12FC8(u8* p, float damage) { ((Fighter*)(p - 0xf4))->Fighter::notifyEventSetDamage(damage); } // Fighter::notifyEventSetDamage
void fn_105_12FD0(u8* p, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(p - 0x100))->Fighter::notifyEventTurn(posX, posY, moduleAccesser); } // Fighter::notifyEventTurn
}

extern "C" {
bool fn_105_12EB0(u8* p, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, long unk3) { return ((Fighter*)(p - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); } // Fighter::notifyEventAnimCmd
}

// Sub-object destructor wrappers: destroy one base sub-object (flag -1 or 0), then delete when flags > 0.
extern "C" {
void* fn_27_C31CC(void* p, s16 flags);
void* fn_27_63EBC(void* p, s16 flags);
void* fn_27_5C674(void* p, s16 flags);
void* fn_27_5AE44(void* p, s16 flags);
void* fn_27_6880C(void* p, s16 flags);
void* fn_27_D3B50(void* p, s16 flags);
void __dt__15soKineticEnergyFv(void* p, s16 flags);
void __dt__16soStopModuleImplFv(void* p, s16 flags);
void __dt__17soSoundModuleImplFv(void* p, s16 flags);
void __dt__19soGeneralWorkSimpleFv(void* p, s16 flags);
void* fn_105_327C(void* p, s16 flags);
void* fn_105_3348(void* p, s16 flags);
void* fn_105_3490(void* p, s16 flags);
void* fn_105_4CF4(void* p, s16 flags);

#pragma dont_inline on
void* fn_105_3220(void* p, s32 flags) { if (p != 0) { fn_105_327C(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soSelectInstanceHolder dt
void* fn_105_32EC(void* p, s32 flags) { if (p != 0) { fn_105_3348(p, 0); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soLineHierarchy dt
void* fn_105_3430(void* p, s16 flags) { if (p != 0) { fn_105_3490((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
void* fn_105_36B4(void* p, s32 flags) { if (p != 0) { __dt__15soKineticEnergyFv(p, 0); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soKineticEnergyRotNormal dt
void* fn_105_3A24(void* p, s32 flags) { if (p != 0) { fn_27_C31CC(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soGlowModuleBuilder dt
void* fn_105_3A80(void* p, s32 flags) { if (p != 0) { fn_27_63EBC(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soReflectModuleBuilder dt
void* fn_105_3ADC(void* p, s32 flags) { if (p != 0) { fn_27_5C674(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soSlowModuleBuilder dt
void* fn_105_3BC4(void* p, s32 flags) { if (p != 0) { __dt__19soGeneralWorkSimpleFv((u8*)p + 0x38, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soGeneralWorkBuilder dt
void* fn_105_4100(void* p, s32 flags) { if (p != 0) { __dt__19soGeneralWorkSimpleFv((u8*)p + 0x14, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soGeneralWorkBuilder dt
void* fn_105_42F0(void* p, s32 flags) { if (p != 0) { fn_27_5AE44(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soVisibilityModuleBuilder dt
void* fn_105_4418(void* p, s32 flags) { if (p != 0) { fn_27_6880C((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soShakeModuleBuilder dt
void* fn_105_43B8(void* p, s32 flags) { if (p != 0) { __dt__17soSoundModuleImplFv((u8*)p + 0xc, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soSoundModuleBuilder dt
void* fn_105_4478(void* p, s32 flags) { if (p != 0) { __dt__16soStopModuleImplFv(p, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soStopModuleBuilder dt
void* fn_105_4C94(void* p, s16 flags) { if (p != 0) { fn_105_4CF4((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
void* fn_105_51C4(void* p, s32 flags) { if (p != 0) { fn_27_D3B50(p, 0); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soDamageEventObserver dt
void* fn_105_78F8(void* p, s32 flags) { if (p != 0) { fn_105_45E4(p, 0); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // ftTeam dt
#pragma dont_inline reset
}

// ---- the fighter hooks -----------------------------------------------------------------------------------------------
// The link events the Ice Climbers send each other (HYPOTHESIS: the field meanings follow the uses below).
struct ftPopoLinkEventArgs : soLinkEventArgs {
    int m_value;
    bool m_flag;
    ftPopoLinkEventArgs(int eventKind) : soLinkEventArgs(eventKind), m_value(0), m_flag(false) { }
    ftPopoLinkEventArgs(int eventKind, int value) : soLinkEventArgs(eventKind), m_value(value), m_flag(false) { }
};

// The kinetic transactor singleton of the pair; its slot 6 takes the situation change (the shared header names the slot after the
// robot's use of it).
struct ftPopoTransactorSlots {
    virtual ~ftPopoTransactorSlots();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
};

void ftPopo::setCursor(bool enable) {
    if (m_isPartner == false) {
        Fighter::setCursor(enable);
    }
}

void ftPopo::setNameCursor(bool enable) {
    if (m_isPartner == false) {
        Fighter::setNameCursor(enable);
    }
}

void ftPopo::setLoupe(bool enable) {
    if (m_isPartner == false) {
        Fighter::setLoupe(enable);
    }
}

void ftPopo::setLoupeDamage(bool enable) {
    if (m_isPartner == false) {
        Fighter::setLoupeDamage(enable);
    }
}

// The second fighter of the pair borrows the sub owner and sub input of the entry.
ftOwner* ftPopo::getOwner() {
    if (m_isPartner == true) {
        return g_ftManager->getSubOwner(m_entryId);
    }
    return g_ftManager->getOwner(m_entryId);
}

Input* ftPopo::getInput() {
    if (m_isPartner == true) {
        return static_cast<Input*>(g_ftManager->getSubInput(m_entryId));
    }
    return static_cast<Input*>(g_ftManager->getInput(m_entryId));
}

// The other fighter of the entry (none when the entry has just one).
Fighter* ftPopo::getPartner() {
    ftManager* manager = g_ftManager;
    if (static_cast<u8>(manager->getFighterCount(m_entryId)) > 1) {
        int index = 1;
        if (m_moduleAccesser->getWorkManageModule().isFlag(0x1200003e)) {
            index = 0;
        }
        return manager->getFighter(m_entryId, index);
    }
    return NULL;
}

void ftPopo::processUpdate() {
    Fighter::processUpdate();
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x117 || m_moduleAccesser->getStatusModule().getStatusKind() == 0x120) {
        soLinkModule& link = m_moduleAccesser->getLinkModule();
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        Vec3f position = link.getModelConstraintTargetNodePosition();
        posture.setPos(&position);
    }
}

bool ftPopo::notifyUseItem(BaseItem* item, u32 nodeIndex, int* unk) {
    if (m_isPartner == true && item->soGetSubKind() == 0x14) {
        return false;
    }
    return Fighter::notifyUseItem(item, nodeIndex, unk);
}

void ftPopo::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftPopoTransactorSlots*>(ftPopoTransactor::getInstance())->notifyEventChangeSituation(kind, prevKind, moduleAccesser);
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
}

// Link event without a payload.
struct ftPopoLinkKindEventArgs : soLinkEventArgs {
    ftPopoLinkKindEventArgs(int eventKind) : soLinkEventArgs(eventKind) { }
};

// HYPOTHESIS: entering a few statuses from the Ice Climbers' own statuses (0x111-0x11d) sets the initial speed of the move from a
// constant (work floats 0x11000000/0x11000001, flag 0x12000003) and tells the partner about it through link events 0x45a/0x45d.
void ftPopo::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    switch (statusKind) {
    case 0x19:
        switch (prevStatusKind) {
        case 0x112:
            m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA1, 0), 0x11000000);
            m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
            break;
        case 0x113:
            m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA9, 0), 0x11000000);
            m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
            break;
        case 0x11C:
            m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBD, 0), 0x11000000);
            m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
            m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBC, 0), 0x11000001);
            break;
        case 0x11B:
            m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFD2, 0), 0x11000000);
            m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
            m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFD1, 0), 0x11000001);
            break;
        }
        break;
    case 0x10:
        if (prevStatusKind == 0x11B && moduleAccesser->getLinkModule().isLink(6) == 1) {
            ftPopoLinkKindEventArgs event(0x45D);
            moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
        }
        break;
    case 0x73:
        if (prevStatusKind == 0x11D && m_moduleAccesser->getWorkManageModule().isFlag(0x1200003E) == true
            && moduleAccesser->getLinkModule().isLink(6) == 1) {
            ftPopoLinkKindEventArgs event(0x45A);
            moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
        }
        break;
    case 0x120:
        startFinalCommon();
        break;
    }
}

// Link event carrying one flag right after the base fields (HYPOTHESIS: kind 0x462 tells the partner whether to stay in the move).
struct ftPopoLinkFlagEventArgs : soLinkEventArgs {
    bool m_flag;
    ftPopoLinkFlagEventArgs(int eventKind, bool flag) : soLinkEventArgs(eventKind), m_flag(flag) { }
};

// HYPOTHESIS: while linked to the partner (and not in the partner's own state) a few motions are mirrored to it: 0x462 for the
// two movement motion groups, 0x456 with the partner's matching motion for the rest.
void ftPopo::notifyEventChangeMotion(int motionKind, int, void*, soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isLink(6) == 1 && m_moduleAccesser->getWorkManageModule().isFlag(0x1200003e) == false) {
        int partnerMotion = -1;
        switch (motionKind) {
        case 0x1C2:
            partnerMotion = 0x1EA;
            break;
        case 0x1C3:
            partnerMotion = 0x1EB;
            break;
        case 0x1C5:
            partnerMotion = 0x1EC;
            break;
        case 0x1C7:
            partnerMotion = 0x1EE;
            break;
        case 0x1C9:
            partnerMotion = 0x1F0;
            break;
        case 0x1D3:
        case 0x1D8:
        case 0x1E4: {
            ftPopoLinkFlagEventArgs event(0x462, true);
            moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
            return;
        }
        case 0x1D5:
        case 0x1D9:
        case 0x1E8: {
            ftPopoLinkFlagEventArgs event(0x462, false);
            moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
            return;
        }
        }
        if (partnerMotion != -1) {
            ftPopoLinkEventArgs event(0x456, partnerMotion);
            moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
        }
    }
}

bool ftPopo::isModelSwapColor(int color) {
    switch (color) {
    case 1:
    case 4:
    case 5:
        return true;
    }
    return false;
}

// HYPOTHESIS: the statuses the partner may be in for a couple technique (the Ice Climbers' synchronized specials check it).
bool ftPopo::isEnableCoupleTech(int status) {
    switch (status) {
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 10:
    case 11:
    case 12:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 30:
    case 31:
    case 32:
    case 33:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
    case 106:
    case 107:
    case 108:
    case 109:
    case 113:
    case 114:
    case 124:
    case 125:
    case 126:
    case 142:
    case 143:
    case 144:
    case 145:
    case 149:
    case 201:
    case 203:
    case 221:
    case 246:
    case 276:
    case 279:
    case 288:
        return true;
    }
    return false;
}

// Shared value variation getter (the Marth translation unit owns the C++ member; this is the REL copy).
extern int g_soValueVariation;
extern "C" {
int fn_105_E114() { return g_soValueVariation; } // soValueAccesser getValueVariation
}

// Forwarders to ftPopo methods that are not written yet (the target's `this` is the adjusted pointer).
extern "C" {
void* fn_105_F0C8(void* p, s32 flags);
void* fn_105_12E70(void* p, s32 flags) { return fn_105_F0C8((u8*)p - 0x4, flags); } // soArticleMediatorImpl shoot thunk
void* fn_105_12E78(void* p, s32 flags) { return fn_105_327C((u8*)p - 0x4, flags); } // soArticleMediatorImpl thunk
void fn_105_B138(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4);
void fn_105_12EC0(u8* p, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) { fn_105_B138(p - 0x54, eventInfo, moduleAccesser, stageObject, unk4); } // ftPopo notifyEventLink thunk
}

// Chained sub-object destructor wrappers (same shape as above): destroy the sub-object, then delete when flags > 0.
extern "C" {
void* fn_105_5C34(void* p, s16 flags);
void* fn_105_56B8(void* p, s16 flags);
void* fn_105_6090(void* p, s16 flags);
void* fn_105_5BD4(void* p, s16 flags);
void* fn_105_5FD0(void* p, s16 flags);
void* fn_105_5658(void* p, s16 flags);
void* fn_105_6030(void* p, s16 flags);

#pragma dont_inline on
void* fn_105_4B4C(void* p, s32 flags) { if (p != 0) { fn_105_3430((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
void* fn_105_5520(void* p, s32 flags) { if (p != 0) { fn_105_4C94((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
void* fn_105_55F8(void* p, s32 flags) { if (p != 0) { fn_105_5658((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
void* fn_105_5658(void* p, s16 flags) { if (p != 0) { fn_105_56B8((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
void* fn_105_5B74(void* p, s32 flags) { if (p != 0) { fn_105_5BD4((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
void* fn_105_5BD4(void* p, s16 flags) { if (p != 0) { fn_105_5C34((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
void* fn_105_5F70(void* p, s32 flags) { if (p != 0) { fn_105_5FD0((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
void* fn_105_5FD0(void* p, s16 flags) { if (p != 0) { fn_105_6030((u8*)p + 0x8, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // soInstancePoolSub dt
void* fn_105_6030(void* p, s16 flags) { if (p != 0) { fn_105_6090((u8*)p + 0x4, -1); if ((s16)flags > 0) { ::operator delete(p); } } return p; } // wnInstanceHolder dt
#pragma dont_inline reset
}

// Virtual slot forwarder: the object's vtable slot at offset 0x58 (slot 20) is tail called with the same arguments.
extern "C" {
struct ftPopoVSlotsArticle {
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
    virtual int slot20();
};
int fn_105_F4D8(ftPopoVSlotsArticle* p) { return p->slot20(); } // soArticle checkActivate (virtual tail call)

}
