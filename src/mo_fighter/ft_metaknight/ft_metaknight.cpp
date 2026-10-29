#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_METAKNIGHT_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/metaknight/ft_metaknight.h>
#include <ft/metaknight/ft_metaknight_extend_param_accesser.h>
#include <ft/ft_owner.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/article/so_article_mediator.h>

#define FT_BC ftMetaknightBuildConfig
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
extern soStatusUniqProcess lbl_27_bss_80A4;
extern soStatusUniqProcess lbl_111_bss_184;
extern soStatusUniqProcess lbl_111_bss_194;
extern soStatusUniqProcess lbl_111_bss_1C4;
extern soStatusUniqProcess lbl_111_bss_1B4;
extern soStatusUniqProcess lbl_111_bss_1A4;
}

ftMetaknightExtendParamAccesser g_ftMetaknightExtendParamAccesser;

ftClassInfoImpl<Fighter_MetaKnight, ftMetaknight> g_ftClassInfoMetaknight;

ftMetaknight::ftMetaknight(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftMetaknightBuildConfig>(entryId,
                                         Fighter_MetaKnight,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_MetaKnight)) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[16] = {0};
    processes[5] = &lbl_27_bss_80A4;
    processes[7] = &lbl_111_bss_184;
    processes[8] = &lbl_111_bss_194;
    processes[13] = &lbl_111_bss_1C4;
    processes[14] = &lbl_111_bss_1B4;
    processes[15] = &lbl_111_bss_1A4;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 16);
}

ftMetaknight::~ftMetaknight() { }

// ---- the fighter overrides ------------------------------------------------------------------------------------------
void ftMetaknight::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: the flag 0x1200003d marks a pending fighter change, cleared whenever the situation is not the air.
    if (kind != 2) {
        moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    }
    // HYPOTHESIS: status 0x9b hides record 2 on the ground and shows it again in the air.
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x9b:
        if (kind == 0) {
            moduleAccesser->getVisibilityModule().setStatusDefault(2, 0, true);
        } else if (kind == 2) {
            moduleAccesser->getVisibilityModule().setStatusDefault(2, 1, true);
        }
        break;
    }
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
}

// Statuses outside the listed ones reset the visibility record 2 when it is shown.
void ftMetaknight::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    switch (statusKind) {
    case 0x1a: case 0x1b: case 0x1c:
    case 0x1e: case 0x1f: case 0x20:
    case 0x10c: case 0x10e: case 0x10f: case 0x115:
    case 0x11b: case 0x11c: case 0x11f: case 0x121:
        break;
    default:
        if (moduleAccesser->getVisibilityModule().get(2) == 2) {
            moduleAccesser->getVisibilityModule().setStatusDefault(2, 0, true);
        }
        break;
    }
}

// HYPOTHESIS: the Final Smash end status keeps the hit state; its entry points are in the status process unit.
extern soStatusUniqProcess g_ftMetaknightStatusUniqProcessFinalEnd;
extern "C" void fn_111_D548(soStatusUniqProcess* process, soModuleAccesser* moduleAccesser);

bool ftMetaknight::notifyEventCollisionAttackCheck(u32 flags) {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x116) {
        fn_111_D548(&g_ftMetaknightStatusUniqProcessFinalEnd, m_moduleAccesser);
    }
    return Fighter::notifyEventCollisionAttackCheck(flags);
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of the article is given: the kind record of the owner (kind 0, sub kind Metaknight, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftMetaknightArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftMetaknightArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftMetaknightArticleKind : wnArticleKindInfo {
    ftMetaknightArticleKind() {
        kind = 0;
        subKind = Fighter_MetaKnight;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftMetaknightArticleBuildInfo : wnArticleBuildInfo {
    ftMetaknightArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data block of the article comes from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_1496A8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftMetaknightArticleTraits;
#define FT_METAKNIGHT_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftMetaknightArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftMetaknightArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_MetaKnight, &tag.m_resourceGroup)); \
        } \
    };

FT_METAKNIGHT_TRAITS(wnMetaknightMantle, wnMetaknightMantleData, 0, fn_27_1496A8)

// The article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftMetaknightArticleTraits<W>::ArticleId, ftMetaknightArticleBuildInfo(ftMetaknightArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftMetaknightArticleTraits<W>::getData()) { }

// ---- the article of Meta Knight and its mediator --------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnMetaknightMantle>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftMetaknightArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftMetaknightArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftMetaknightArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftMetaknightArticleMediator::~ftMetaknightArticleMediator() { }
#pragma dont_inline off

s32 ftMetaknightArticleMediator::getMediateNum() { return 1; }
void ftMetaknightArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Mantle (the rest are unused).

#define FT_METAKNIGHT_POOL_MANTLE_BASE static_cast<wnArticlePool<wnMetaknightMantle, 1, soInstancePoolRoot>&>(m_pools)
#define FT_METAKNIGHT_POOL_MANTLE FT_METAKNIGHT_POOL_MANTLE_BASE.getSub()

#define FT_METAKNIGHT_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(1, value) WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

// The Mantle starts at the stage object of the fighter with its team and the model and texture resources.
bool ftMetaknight::activeArticle(wnMetaknightMantle* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    int founderTaskId = acc->getStageObject().m_taskId;
    soResourceModule& resources = acc->getResourceModule();
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getMdlResId(), resources.getResourceIdAccesser()->getTexResId(), fighter.getOwner()->getTeam());
    return true;
}

void ftMetaknightArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnMetaknightMantle* weapon = FT_METAKNIGHT_POOL_MANTLE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftMetaknightArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 1; }
    FT_METAKNIGHT_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftMetaknightActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_METAKNIGHT_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftMetaknightActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftMetaknightActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftMetaknightArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_METAKNIGHT_COUNT_ACTIVE(FT_METAKNIGHT_POOL_MANTLE, 1)
    FT_METAKNIGHT_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_METAKNIGHT_CAN_GENERATE(POOL, N, ID) \
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

bool ftMetaknightArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_METAKNIGHT_CAN_GENERATE(FT_METAKNIGHT_POOL_MANTLE, 1, 0)
    FT_METAKNIGHT_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftMetaknightArticleActivator;
template <> struct ftMetaknightArticleActivator<wnMetaknightMantle> {
    static bool activate(wnMetaknightMantle* weapon, soModuleAccesser* acc) { return ftMetaknight::activeArticle(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_METAKNIGHT_GENERATE(W, POOL) \
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
        if (ftMetaknightArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftMetaknightArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_METAKNIGHT_GENERATE(wnMetaknightMantle, FT_METAKNIGHT_POOL_MANTLE)
    FT_METAKNIGHT_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Shooting only type-checks the article.
bool ftMetaknightArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; (void)dynamic_cast<wnMetaknightMantle&>(*article); return true; }
    FT_METAKNIGHT_UNUSED_SLOTS(true)
    default: return false;
    }
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftMetaknight is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftMetaknightInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftMetaknightInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Virtual-slot view used only to emit virtual tail calls; the slots are never defined in this unit.
struct ftMetaknightVirtualSlots {
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
};

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

// soValueAccesser::getValueVariation (same global as ft_marth.cpp)
extern int g_soValueVariation;
int fn_111_8CC8() { return g_soValueVariation; }

// soArticle::checkActivate: virtual slot 20 tail call.
void fn_111_9874(void* p) { return ((ftMetaknightVirtualSlots*)p)->slot20(); }

u8 fn_111_5F38(u8* p) { return *(u8*)(p + 0x4); }
void fn_111_8650() {}
void fn_111_8654() {}
void fn_111_8658() {}
int fn_111_87CC(u8* p) { return *(int*)(p + 0x20); }
int fn_111_88A8(u8* p) { return *(int*)(p + 0x18); }
void fn_111_A5EC() {}
void fn_111_AE98() {}

}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
u8* fn_111_BB28(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"
