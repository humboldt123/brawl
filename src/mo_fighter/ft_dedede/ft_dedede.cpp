// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_NOINLINE_SEARCH_BUILDER
#define FT_DEDEDE_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/dedede/ft_dedede.h>
#include <ft/dedede/ft_dedede_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <ft/dedede/ft_dedede_transactor.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <so/so_value_accesser.h>

#define FT_BC ftDededeBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_88CC;
extern soStatusUniqProcess lbl_117_bss_2AC;
extern soStatusUniqProcess lbl_117_bss_23C;
extern soStatusUniqProcess lbl_117_bss_28C;
extern soStatusUniqProcess lbl_117_bss_2DC;
extern soStatusUniqProcess lbl_27_bss_891C;
extern soStatusUniqProcess lbl_27_bss_890C;
extern soStatusUniqProcess lbl_27_bss_88EC;
extern soStatusUniqProcess lbl_27_bss_88FC;
extern soStatusUniqProcess lbl_117_bss_24C;
}
extern soStatusUniqProcess g_ftDededeStatusUniqProcessSpecialWaddledeeThrowPickup;
extern soStatusUniqProcess g_ftDededeStatusUniqProcessSpecialSuperJumpLanding;
extern soStatusUniqProcess g_ftDededeStatusUniqProcessSpecialSuperJumpHitCeil;
extern soStatusUniqProcess g_ftDededeStatusUniqProcessSpecialSuperJumpTurn;

ftDededeExtendParamAccesser g_ftDededeExtendParamAccesser;

ftClassInfoImpl<Fighter_Dedede, ftDedede> g_ftClassInfoDedede;

ftDedede::ftDedede(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftDededeBuildConfig>(entryId,
                                         Fighter_Dedede,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Dedede)) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[40] = {0};
    processes[0] = &lbl_27_bss_88CC;
    processes[1] = &lbl_117_bss_2AC;
    processes[2] = &lbl_117_bss_23C;
    processes[3] = &lbl_117_bss_28C;
    processes[4] = &lbl_117_bss_2DC;
    processes[5] = &lbl_27_bss_88CC;
    processes[7] = &lbl_27_bss_891C;
    processes[8] = &lbl_27_bss_891C;
    processes[9] = &lbl_27_bss_890C;
    processes[10] = &lbl_27_bss_890C;
    processes[11] = &lbl_27_bss_890C;
    processes[12] = &lbl_27_bss_890C;
    processes[13] = &lbl_27_bss_890C;
    processes[14] = &lbl_27_bss_890C;
    processes[15] = &lbl_27_bss_890C;
    processes[16] = &lbl_27_bss_890C;
    processes[17] = &lbl_27_bss_890C;
    processes[18] = &lbl_27_bss_890C;
    processes[19] = &lbl_27_bss_890C;
    processes[20] = &lbl_27_bss_890C;
    processes[21] = &lbl_27_bss_88EC;
    processes[22] = &lbl_27_bss_88FC;
    processes[23] = &g_ftDededeStatusUniqProcessSpecialWaddledeeThrowPickup;
    processes[24] = &lbl_117_bss_24C;
    processes[25] = &g_ftDededeStatusUniqProcessSpecialSuperJumpLanding;
    processes[26] = &g_ftDededeStatusUniqProcessSpecialSuperJumpHitCeil;
    processes[28] = &g_ftDededeStatusUniqProcessSpecialSuperJumpTurn;
    processes[29] = &lbl_117_bss_28C;
    processes[30] = &lbl_117_bss_28C;
    processes[31] = &lbl_117_bss_28C;
    processes[32] = &lbl_117_bss_28C;
    processes[33] = &lbl_117_bss_28C;
    processes[34] = &lbl_117_bss_28C;
    processes[35] = &lbl_117_bss_28C;
    processes[36] = &lbl_117_bss_28C;
    processes[37] = &lbl_117_bss_28C;
    processes[38] = &lbl_117_bss_2DC;
    processes[39] = &lbl_117_bss_2DC;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 40);
    // HYPOTHESIS: the common data record points at the fighter's area definitions (+0xb8).
    m_moduleAccesser->getAreaModule().addArea(*reinterpret_cast<soSet<soAreaData>**>(reinterpret_cast<u8*>(m_data) + 0xb8));
}

// The Jet Hammer starts with the model resource of the fighter and the facing.
bool ftDedede::activeArticle1(wnDededeJethammer* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    float lr = acc->getPostureModule().getLr();
    int modelResId = resources.getResourceIdAccesser()->getMdlResId();
    weapon->activate(founderTaskId, modelResId, lr);
    return true;
}

// HYPOTHESIS: the work flags 0x22000013/0x22000014 mark the two throw directions of the Star Spit; the star starts at the
// model node "TopN" moved by the parameter offsets (the second one follows the facing), and the used flag is cleared.
bool ftDedede::activeArticle(wnDededeStar* weapon, soModuleAccesser* acc) {
    float lr = acc->getPostureModule().getLr();
    bool flip;
    Vec3f pos;
    Vec3f offset;
    if (acc->getWorkManageModule().isFlag(0x22000013)) {
        offset.m_x = 0.0f;
        offset.m_y = soValueAccesser::getConstantFloat(acc, 0xfbd, 0);
        offset.m_z = lr * soValueAccesser::getConstantFloat(acc, 0xfbc, 0);
        const char* nodeName = "TopN";
        soModelModule* model = &acc->getModelModule();
        pos = model->getNodeGlobalPosition(model->getNodeId(nodeName), &offset, false, false);
        flip = false;
        acc->getWorkManageModule().offFlag(0x22000013);
    }
    if (acc->getWorkManageModule().isFlag(0x22000014)) {
        offset.m_x = 0.0f;
        offset.m_y = soValueAccesser::getConstantFloat(acc, 0xfbf, 0);
        offset.m_z = lr * soValueAccesser::getConstantFloat(acc, 0xfbe, 0);
        const char* nodeName = "TopN";
        soModelModule* model = &acc->getModelModule();
        pos = model->getNodeGlobalPosition(model->getNodeId(nodeName), &offset, false, false);
        flip = true;
        acc->getWorkManageModule().offFlag(0x22000014);
    }
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    int team = acc->getTeamModule().getTeam()->getNo();
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), team, &pos, flip, 1.0f, 20.0f);
    return false;
}

// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
#pragma dont_inline on
ftDededeTransactor* ftDededeTransactor::getInstance() {
    static ftDededeTransactor instance;
    return &instance;
}
#pragma dont_inline off
#pragma pool_data reset

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Dedede, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftDededeArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftDededeArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftDededeArticleKind : wnArticleKindInfo {
    ftDededeArticleKind() {
        kind = 0;
        subKind = Fighter_Dedede;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftDededeArticleBuildInfo : wnArticleBuildInfo {
    ftDededeArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_1488D4(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1488E8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148910(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1488FC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1489A4(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftDededeArticleTraits;
#define FT_DEDEDE_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftDededeArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftDededeArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Dedede, &tag.m_resourceGroup)); \
        } \
    };

FT_DEDEDE_TRAITS(wnDededeStar, wnDededeStarData, 0, fn_27_1488D4)
FT_DEDEDE_TRAITS(wnDededeJethammer, wnDededeJethammerData, 1, fn_27_1488E8)
FT_DEDEDE_TRAITS(wnDededeStarmissile, wnDededeStarmissileData, 2, fn_27_148910)
FT_DEDEDE_TRAITS(wnDededeWaddledooBeam, wnDededeWaddledooBeamData, 3, fn_27_1488FC)
FT_DEDEDE_TRAITS(wnDededeFollower, wnDededeFollowerData, 4, fn_27_1489A4)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftDededeArticleTraits<W>::ArticleId, ftDededeArticleBuildInfo(ftDededeArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftDededeArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnDededeStar>::~wnArticleHolder();
template wnArticleHolder<wnDededeJethammer>::~wnArticleHolder();
template wnArticleHolder<wnDededeStarmissile>::~wnArticleHolder();
template wnArticleHolder<wnDededeWaddledooBeam>::~wnArticleHolder();
template wnArticleHolder<wnDededeFollower>::~wnArticleHolder();
template wnArticleHolder<wnDededeStar>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnDededeWaddledooBeam>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnDededeFollower>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftDededeArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftDededeArticleMediator, 25>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftDededeArticleMediator, 25>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftDededeArticleMediator::~ftDededeArticleMediator() { }
#pragma dont_inline off

s32 ftDededeArticleMediator::getMediateNum() { return 5; }
void ftDededeArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Star, 1 Jethammer, 2 Starmissile, 3 WaddledooBeam, 4 Follower (the rest are unused).

#define FT_DEDEDE_POOL_STAR_BASE static_cast<wnArticlePool<wnDededeStar, 2, ftDededeJethammerPool>&>(m_pools)
#define FT_DEDEDE_POOL_STAR FT_DEDEDE_POOL_STAR_BASE.getSub()
#define FT_DEDEDE_POOL_JETHAMMER_BASE static_cast<wnArticlePool<wnDededeJethammer, 1, ftDededeStarmissilePool>&>(m_pools)
#define FT_DEDEDE_POOL_JETHAMMER FT_DEDEDE_POOL_JETHAMMER_BASE.getSub()
#define FT_DEDEDE_POOL_STARMISSILE_BASE static_cast<wnArticlePool<wnDededeStarmissile, 1, ftDededeWaddledooBeamPool>&>(m_pools)
#define FT_DEDEDE_POOL_STARMISSILE FT_DEDEDE_POOL_STARMISSILE_BASE.getSub()
#define FT_DEDEDE_POOL_WADDLEDOOBEAM_BASE static_cast<wnArticlePool<wnDededeWaddledooBeam, 4, ftDededeFollowerPool>&>(m_pools)
#define FT_DEDEDE_POOL_WADDLEDOOBEAM FT_DEDEDE_POOL_WADDLEDOOBEAM_BASE.getSub()
#define FT_DEDEDE_POOL_FOLLOWER_BASE static_cast<wnArticlePool<wnDededeFollower, 19, soInstancePoolRoot>&>(m_pools)
#define FT_DEDEDE_POOL_FOLLOWER FT_DEDEDE_POOL_FOLLOWER_BASE.getSub()

#define FT_DEDEDE_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) \
    WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) \
    WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) \
    WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftDededeArticleMediator::deactivate() {
    for (s32 i = 0; i < 2; ++i) {
        wnDededeStar* weapon = FT_DEDEDE_POOL_STAR.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnDededeJethammer* weapon = FT_DEDEDE_POOL_JETHAMMER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnDededeStarmissile* weapon = FT_DEDEDE_POOL_STARMISSILE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 4; ++i) {
        wnDededeWaddledooBeam* weapon = FT_DEDEDE_POOL_WADDLEDOOBEAM.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 19; ++i) {
        wnDededeFollower* weapon = FT_DEDEDE_POOL_FOLLOWER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftDededeArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 2; }
    case 1: { wnArticleSlotTag tag; return 1; }
    case 2: { wnArticleSlotTag tag; return 1; }
    case 3: { wnArticleSlotTag tag; return 4; }
    case 4: { wnArticleSlotTag tag; return 19; }
    FT_DEDEDE_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftDededeActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_DEDEDE_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftDededeActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftDededeActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftDededeArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_DEDEDE_COUNT_ACTIVE(FT_DEDEDE_POOL_STAR, 2)
    case 1: FT_DEDEDE_COUNT_ACTIVE(FT_DEDEDE_POOL_JETHAMMER, 1)
    case 2: FT_DEDEDE_COUNT_ACTIVE(FT_DEDEDE_POOL_STARMISSILE, 1)
    case 3: FT_DEDEDE_COUNT_ACTIVE(FT_DEDEDE_POOL_WADDLEDOOBEAM, 4)
    case 4: FT_DEDEDE_COUNT_ACTIVE(FT_DEDEDE_POOL_FOLLOWER, 19)
    FT_DEDEDE_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_DEDEDE_CAN_GENERATE(POOL, N, ID) \
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

bool ftDededeArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_DEDEDE_CAN_GENERATE(FT_DEDEDE_POOL_STAR, 2, 0)
    case 1: FT_DEDEDE_CAN_GENERATE(FT_DEDEDE_POOL_JETHAMMER, 1, 1)
    case 2: FT_DEDEDE_CAN_GENERATE(FT_DEDEDE_POOL_STARMISSILE, 1, 2)
    case 3: FT_DEDEDE_CAN_GENERATE(FT_DEDEDE_POOL_WADDLEDOOBEAM, 4, 3)
    case 4: FT_DEDEDE_CAN_GENERATE(FT_DEDEDE_POOL_FOLLOWER, 19, 4)
    FT_DEDEDE_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftDededeArticleActivator;
template <> struct ftDededeArticleActivator<wnDededeStar> {
    static bool activate(wnDededeStar* weapon, soModuleAccesser* acc) { return ftDedede::activeArticle(weapon, acc); }
};
template <> struct ftDededeArticleActivator<wnDededeJethammer> {
    static bool activate(wnDededeJethammer* weapon, soModuleAccesser* acc) { return ftDedede::activeArticle1(weapon, acc); }
};
template <> struct ftDededeArticleActivator<wnDededeStarmissile> {
    static bool activate(wnDededeStarmissile* weapon, soModuleAccesser* acc) {
        u32 etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
        return ftDededeTransactor::getInstance()->activeArticle(weapon, acc, etcResId);
    }
};
template <> struct ftDededeArticleActivator<wnDededeWaddledooBeam> {
    static bool activate(wnDededeWaddledooBeam* weapon, soModuleAccesser* acc) { return ftDedede::activeArticle3(weapon, acc); }
};
template <> struct ftDededeArticleActivator<wnDededeFollower> {
    static bool activate(wnDededeFollower* weapon, soModuleAccesser* acc) { return ftDedede::activeArticle2(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_DEDEDE_GENERATE(W, POOL) \
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
        if (ftDededeArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftDededeArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_DEDEDE_GENERATE(wnDededeStar, FT_DEDEDE_POOL_STAR)
    case 1: FT_DEDEDE_GENERATE(wnDededeJethammer, FT_DEDEDE_POOL_JETHAMMER)
    case 2: FT_DEDEDE_GENERATE(wnDededeStarmissile, FT_DEDEDE_POOL_STARMISSILE)
    case 3: FT_DEDEDE_GENERATE(wnDededeWaddledooBeam, FT_DEDEDE_POOL_WADDLEDOOBEAM)
    case 4: FT_DEDEDE_GENERATE(wnDededeFollower, FT_DEDEDE_POOL_FOLLOWER)
    FT_DEDEDE_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftDededeArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnDededeStar&>(*article);
        return true;
    }
    case 1: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnDededeJethammer&>(*article);
        return false;
    }
    case 2: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnDededeStarmissile&>(*article);
        return true;
    }
    case 3: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnDededeWaddledooBeam&>(*article);
        return true;
    }
    case 4: {
        wnArticleSlotTag tag;
        return ftDedede::shootArticle(&dynamic_cast<wnDededeFollower&>(*article), acc);
    }
    FT_DEDEDE_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftDedede is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftDededeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftDededeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Destructors whose bodies are empty; the compiler emits the deleting variants.
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
ftKineticEnergyController::~ftKineticEnergyController() { }
#pragma dont_inline on
ftVirtualNodeMatrixPool::~ftVirtualNodeMatrixPool() { }
#pragma dont_inline off

// ftDedede::notifyArticleEventRemove forwards to the StageObject version (tail call).
extern "C" void fn_117_99E0(StageObject* self, int unk1, int* unk2) { self->StageObject::notifyArticleEventRemove(unk1, unk2); }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_117_67D4(u8* p) { return *(u8*)(p + 0x4); }
void fn_117_8A60() {}
int fn_117_99D8(u8* p) { return *(int*)(p + 0xb8); }
int fn_117_9F40() { return 0x0; }
void fn_117_A13C() {}
void fn_117_A2B4() {}
u8 fn_117_A2B8(u8* p) { return *(u8*)(p + 0x44); }
void fn_117_A2C0() {}
void fn_117_A300() {}
void fn_117_A304() {}
void fn_117_A394() {}
void fn_117_A3C0() {}
void fn_117_A3C4() {}
void fn_117_A3C8() {}
void fn_117_A3CC() {}
void fn_117_A3D0() {}
void fn_117_A3D4() {}
void fn_117_A3D8() {}
void fn_117_A3DC() {}
void fn_117_A3E0() {}
void fn_117_A3E4() {}
void fn_117_A3E8() {}
void fn_117_A3EC() {}
void fn_117_A3F0() {}
void fn_117_A3F4() {}
void fn_117_A3F8() {}
int fn_117_A3FC() { return 0x0; }
void fn_117_A404() {}
void fn_117_A408() {}
void fn_117_A40C() {}
void fn_117_A410() {}
void fn_117_A43C() {}
void fn_117_A440() {}
void fn_117_A444() {}
void fn_117_A448() {}
void fn_117_A44C() {}
int fn_117_A450(u8* p) { return *(int*)(p + 0x110); }
int fn_117_A468() { return 0x0; }
void fn_117_A470() {}
void fn_117_A474() {}
void fn_117_A478() {}
void fn_117_A47C() {}
int fn_117_A480() { return 0x0; }
int fn_117_A488() { return 0x0; }
void fn_117_A490() {}
void fn_117_A494() {}
void fn_117_A498() {}
void fn_117_A49C() {}
void fn_117_A4A0() {}
int fn_117_A654(u8* p) { return *(int*)(p + 0x20); }
int fn_117_A730(u8* p) { return *(int*)(p + 0x18); }
int fn_117_A7C8(u8* p) { return *(int*)(p + 0x10); }
int fn_117_A888() { return 0x0; }
int fn_117_C104() { return 0x5; }
void fn_117_D0F8() {}
void fn_117_D804() {}
int fn_117_D840() { return 0x8; }
void fn_117_D888() {}
int fn_117_D934() { return 0x0; }
void fn_117_D970() {}
int fn_117_DA1C() { return 0x0; }
void fn_117_DA58() {}
int fn_117_DB04() { return 0x0; }
void fn_117_DB40() {}
int fn_117_DBEC() { return 0x0; }
void fn_117_DC28() {}
int fn_117_DCD4() { return 0x0; }
void fn_117_DD10() {}
int fn_117_DDBC() { return 0x0; }
void fn_117_DDF8() {}
int fn_117_DEA4() { return 0x0; }
void fn_117_DEE0() {}
int fn_117_DF8C() { return 0x0; }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_117_E1B0(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_117_E1B8(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_117_E1C0(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
bool fn_117_E1C8(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_117_E1D0(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
bool fn_117_E208(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_117_E210(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_117_E218(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_117_E220(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_117_E238(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_117_E250(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_117_E270(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_117_E278(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_117_E280(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_117_E288(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_117_E290(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_117_E298(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_117_E2A0(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_117_E2A8(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_117_E2B0(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_117_E2B8(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_117_E2C0(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_117_E2C8(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_117_E2D0(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_117_E2D8(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_117_E2E0(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_117_E2E8(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_117_E2F0(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_117_E2F8(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
bool fn_117_E1E0(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_117_E228(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_117_E230(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_117_E240(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_117_E248(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_117_E258(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_117_43A8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_117_43E8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_117_4428(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"
