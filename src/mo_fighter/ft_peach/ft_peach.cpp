// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_NOINLINE_SEARCH_BUILDER
#define FT_MARTH_PHOTO_CALLBACK_NOINLINE // MATCH-ONLY: out-of-line photo base teardown and +0x186F0 photo thunks (shared shadow macro)
#define FT_PEACH_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/peach/ft_peach.h>
#include <ft/peach/ft_peach_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <ft/peach/ft_peach_transactor.h>
#include <ft/peach/ft_peach_status_uniq_process_final.h>
#include <gf/gf_task_scheduler.h>
#include <mt/mt_prng.h>

#define FT_BC ftPeachBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_103_bss_1D4;
extern soStatusUniqProcess lbl_103_bss_1F4;
extern soStatusUniqProcess lbl_103_bss_1C4;
}

ftPeachExtendParamAccesser g_ftPeachExtendParamAccesser;

ftClassInfoImpl<Fighter_Peach, ftPeach> g_ftClassInfoPeach;

ftPeach::ftPeach(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPeachBuildConfig>(entryId,
                                         Fighter_Peach,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Peach)) {
    // HYPOTHESIS: flag 0x10000040 of the work module selects one of three random variations at spawn.
    m_moduleAccesser->getWorkManageModule().setInt(randi(3), 0x10000040);
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[5] = {0};
    processes[3] = &lbl_103_bss_1D4;
    processes[4] = &lbl_103_bss_1F4;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 5);
    m_moduleAccesser->getStatusModule().setUniqProc(0x2A, &lbl_103_bss_1C4);
    // HYPOTHESIS: the common data record holds the fighter's shield shape group at +0x9c (the bubble of the parasol).
    m_moduleAccesser->getCollisionShieldModule().add(reinterpret_cast<soCollisionShieldGroupData*>(reinterpret_cast<u8*>(m_data) + 0x9c), 1);
}

void ftPeach::endFinalRequest() {
    Fighter::endFinal(true, true, false);
    soPhotoCallBack::removeCallBack();
}

ftPeach::~ftPeach() { }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

void ftPeach::onEndFinal() {
    g_ftPeachStatusUniqProcessFinal.destroyInfo(m_moduleAccesser);
}

void ftPeach::onDeactivate() {
    Fighter::endFinal(true, true, false);
    soPhotoCallBack::removeCallBack();
}

// Final Smash blossom windows: the fighter keeps their task ids in work slots.
void ftPeach::photoMoved() {
    int taskId = m_moduleAccesser->getWorkManageModule().getInt(0x10000041);
    if (taskId != 0) {
        IfPeachFinalTask* window = dynamic_cast<IfPeachFinalTask*>(gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Info, taskId));
        if (window != NULL) {
            window->setVisibilityWhole(false);
        }
    }
    int effectId = m_moduleAccesser->getWorkManageModule().getInt(0x10000042);
    if (effectId != 0) {
        m_moduleAccesser->getEffectModule().setVisible(effectId, false);
    }
}

void ftPeach::photoExit() {
    int taskId = m_moduleAccesser->getWorkManageModule().getInt(0x10000041);
    if (taskId != 0) {
        IfPeachFinalTask* window = dynamic_cast<IfPeachFinalTask*>(gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Info, taskId));
        if (window != NULL) {
            window->setVisibilityWhole(true);
        }
    }
    int effectId = m_moduleAccesser->getWorkManageModule().getInt(0x10000042);
    if (effectId != 0) {
        m_moduleAccesser->getEffectModule().setVisible(effectId, true);
    }
}

void** ftPeach::getExtendParam() {
    return m_data->extendParam;
}

// The Toad's help of the transactor runs while the fighter is in status 0x112.
void ftPeach::processUpdate() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x112) {
        ftPeachTransactor::getInstance()->updateStatus112(m_moduleAccesser);
    }
    Fighter::processUpdate();
}

// Leaving status 0x11b resets the two work floats from the constants and raises a flag.
// HYPOTHESIS: the status and the meaning of the work values.
void ftPeach::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (prevStatusKind == 0x11b) {
        soWorkManageModule& work1 = m_moduleAccesser->getWorkManageModule();
        work1.setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBC, 0), 0x11000000);
        soWorkManageModule& work2 = m_moduleAccesser->getWorkManageModule();
        work2.setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBB, 0), 0x11000001);
        m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
    }
}

void ftPeach::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    if (kind != 2 && kind != 1) {
        moduleAccesser->getWorkManageModule().offFlag(0x1200003E);
    }
    ftPeachTransactor::getInstance()->notifyEventChangeSituation(kind, prevKind, moduleAccesser);
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
}

void ftPeach::notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if (collisionLog != NULL) {
        ftPeachTransactor::getInstance()->notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
    }
    Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

bool ftPeach::notifyEventCollisionShieldCheck() {
    if (ftPeachTransactor::getInstance()->notifyEventCollisionShieldCheck(m_moduleAccesser) == true) {
        return true;
    }
    return Fighter::notifyEventCollisionShieldCheck();
}

// The parasol (status 0x116) places two nodes of the model.
// HYPOTHESIS: node 0x69 is the parasol node.
void ftPeach::updateNodeSRT() {
    StageObject::updateNodeSRT();
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x116) {
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        soModelModule& model = m_moduleAccesser->getModelModule();
        Vec3f rotation = posture.getRot(1);
        model.setNodeRotate(0x69, &rotation);
        soModuleAccesser* acc = m_moduleAccesser;
        soModelModule& translateModel = acc->getModelModule();
        translateModel.setNodeTranslate(0x69, &Vec3f(soValueAccesser::getConstantFloat(acc, 0xFC0, 0), soValueAccesser::getConstantFloat(acc, 0xFC1, 0), soValueAccesser::getConstantFloat(acc, 0xFC2, 0)));
    }
}

// HYPOTHESIS: the bytes of the collision log at 0x21-0x24 are its hit kind, category and two flags.
#define FT_PEACH_LOG_KIND(log) (reinterpret_cast<const u8*>(log)[0x21])
#define FT_PEACH_LOG_CATEGORY(log) (reinterpret_cast<const u8*>(log)[0x22])
#define FT_PEACH_LOG_FLAG1(log) (reinterpret_cast<const u8*>(log)[0x23])
#define FT_PEACH_LOG_FLAG2(log) (reinterpret_cast<const u8*>(log)[0x24])

void ftPeach::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventCollisionSearch(collisionLog, m_moduleAccesser);
    if (collisionLog != NULL && m_moduleAccesser->getStatusModule().getStatusKind() == 0x117) {
        if (FT_PEACH_LOG_KIND(collisionLog) == 2 && FT_PEACH_LOG_FLAG1(collisionLog) == 0 && FT_PEACH_LOG_CATEGORY(collisionLog) == 0xA && FT_PEACH_LOG_FLAG2(collisionLog) == 0) {
            return;
        }
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000011);
    }
}

bool ftPeach::notifyEventCollisionSearchCheck() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x117 && m_moduleAccesser->getWorkManageModule().isFlag(0x22000011) == true) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(0x118, m_moduleAccesser);
        return true;
    }
    return Fighter::notifyEventCollisionSearchCheck();
}

// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
#pragma dont_inline on
ftPeachTransactor* ftPeachTransactor::getInstance() {
    static ftPeachTransactor instance;
    return &instance;
}
#pragma dont_inline off
#pragma pool_data reset

// Kassar starts with the effect and texture resources of the fighter and with his team.
bool ftPeach::activeArticle(wnPeachKassar* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), resources.getResourceIdAccesser()->getTexResId(), acc->getTeamModule().getTeam()->getNo());
    return true;
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Peach, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftPeachArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftPeachArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftPeachArticleKind : wnArticleKindInfo {
    ftPeachArticleKind() {
        kind = 0;
        subKind = Fighter_Peach;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftPeachArticleBuildInfo : wnArticleBuildInfo {
    ftPeachArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_148F4C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148F64(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftPeachArticleTraits;
#define FT_PEACH_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftPeachArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftPeachArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Peach, &tag.m_resourceGroup)); \
        } \
    };

FT_PEACH_TRAITS(wnPeachKassar, wnPeachKassarData, 0, fn_27_148F4C)
FT_PEACH_TRAITS(wnPeachKinopio, wnPeachKinopioData, 1, fn_27_148F64)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftPeachArticleTraits<W>::ArticleId, ftPeachArticleBuildInfo(ftPeachArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftPeachArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnPeachKassar>::~wnArticleHolder();
template wnArticleHolder<wnPeachKinopio>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftPeachArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftPeachArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftPeachArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftPeachArticleMediator::~ftPeachArticleMediator() { }
#pragma dont_inline off

s32 ftPeachArticleMediator::getMediateNum() { return 3; }
void ftPeachArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Kassar, 1 Kinopio (the rest are unused).

#define FT_PEACH_POOL_KASSAR_BASE static_cast<wnArticlePool<wnPeachKassar, 1, ftPeachKinopioPool>&>(m_pools)
#define FT_PEACH_POOL_KASSAR FT_PEACH_POOL_KASSAR_BASE.getSub()
#define FT_PEACH_POOL_KINOPIO_BASE static_cast<wnArticlePool<wnPeachKinopio, 1, soInstancePoolRoot>&>(m_pools)
#define FT_PEACH_POOL_KINOPIO FT_PEACH_POOL_KINOPIO_BASE.getSub()

#define FT_PEACH_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(3, value) WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) \
    WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) \
    WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) \
    WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) \
    WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftPeachArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnPeachKassar* weapon = FT_PEACH_POOL_KASSAR.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnPeachKinopio* weapon = FT_PEACH_POOL_KINOPIO.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftPeachArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 1; }
    case 1: { wnArticleSlotTag tag; return 1; }
    WN_ARTICLE_UNUSED_SLOT(2, 1)
    FT_PEACH_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftPeachActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_PEACH_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftPeachActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftPeachActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftPeachArticleMediator::getActiveNum(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: FT_PEACH_COUNT_ACTIVE(FT_PEACH_POOL_KASSAR, 1)
    case 1: FT_PEACH_COUNT_ACTIVE(FT_PEACH_POOL_KINOPIO, 1)
    case 2: { wnArticleSlotTag tag; return 0; }
    FT_PEACH_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_PEACH_CAN_GENERATE(POOL, N, ID) \
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

bool ftPeachArticleMediator::isGeneratable(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: FT_PEACH_CAN_GENERATE(FT_PEACH_POOL_KASSAR, 1, 0)
    case 1: FT_PEACH_CAN_GENERATE(FT_PEACH_POOL_KINOPIO, 1, 1)
    case 2: { wnArticleSlotTag tag; return true; }
    FT_PEACH_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftPeachArticleActivator;
template <> struct ftPeachArticleActivator<wnPeachKassar> {
    static bool activate(wnPeachKassar* weapon, soModuleAccesser* acc) { return ftPeach::activeArticle(weapon, acc); }
};
template <> struct ftPeachArticleActivator<wnPeachKinopio> {
    static bool activate(wnPeachKinopio* weapon, soModuleAccesser* acc) {
        soResourceModule& resources = acc->getResourceModule();
        u32 etcResId = resources.getResourceIdAccesser()->getEtcResId();
        return ftPeachTransactor::getInstance()->activeArticle1(weapon, acc, etcResId);
    }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_PEACH_GENERATE(W, POOL) \
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
        if (ftPeachArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftPeachArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_PEACH_GENERATE(wnPeachKassar, FT_PEACH_POOL_KASSAR)
    case 1: FT_PEACH_GENERATE(wnPeachKinopio, FT_PEACH_POOL_KINOPIO)
    case 2: {
        wnArticleSlotTag tag;
        BaseItem* item = soArticleMediatorHelper::createItem(0x56, 2, acc->getStageObject().m_taskId);
        if (item == NULL) {
            return wnGetNullArticle();
        }
        acc->getItemManageModule().haveItem(item, 0, false, true);
        return wnGetNullArticle();
    }
    FT_PEACH_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftPeachArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; (void)dynamic_cast<wnPeachKassar&>(*article); return true; }
    case 1: { wnArticleSlotTag tag; (void)dynamic_cast<wnPeachKinopio&>(*article); return true; }
    case 2: {
        wnArticleSlotTag tag;
        return true;
    }
    WN_ARTICLE_UNUSED_SLOT(3, true)
    WN_ARTICLE_UNUSED_SLOT(4, true)
    WN_ARTICLE_UNUSED_SLOT(5, true)
    WN_ARTICLE_UNUSED_SLOT(6, true)
    WN_ARTICLE_UNUSED_SLOT(7, true)
    WN_ARTICLE_UNUSED_SLOT(8, true)
    WN_ARTICLE_UNUSED_SLOT(9, true)
    WN_ARTICLE_UNUSED_SLOT(10, true)
    WN_ARTICLE_UNUSED_SLOT(11, true)
    WN_ARTICLE_UNUSED_SLOT(12, true)
    WN_ARTICLE_UNUSED_SLOT(13, true)
    WN_ARTICLE_UNUSED_SLOT(14, true)
    WN_ARTICLE_UNUSED_SLOT(15, true)
    WN_ARTICLE_UNUSED_SLOT(16, true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftPeach is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftPeachInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftPeachInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_103_72A4(u8* p) { return *(u8*)(p + 0x4); }
void fn_103_9058() {}
void fn_103_9A40() {}
int fn_103_9A44() { return 0x0; }
void fn_103_9BC4() {}
void fn_103_9D4C() {}
u8 fn_103_9D50(u8* p) { return *(u8*)(p + 0x44); }
void fn_103_9D58() {}
void fn_103_9D98() {}
void fn_103_9D9C() {}
void fn_103_9E2C() {}
void fn_103_9E58() {}
void fn_103_9E5C() {}
void fn_103_9E60() {}
void fn_103_9E64() {}
void fn_103_9E68() {}
void fn_103_9E6C() {}
void fn_103_9E70() {}
void fn_103_9E74() {}
void fn_103_9E78() {}
void fn_103_9E7C() {}
void fn_103_9E80() {}
void fn_103_9E84() {}
void fn_103_9E88() {}
void fn_103_9E8C() {}
void fn_103_9E90() {}
int fn_103_9E94() { return 0x0; }
void fn_103_9E9C() {}
void fn_103_9EA0() {}
void fn_103_9EA4() {}
void fn_103_9EA8() {}
void fn_103_9ED4() {}
void fn_103_9ED8() {}
void fn_103_9EDC() {}
void fn_103_9EE0() {}
void fn_103_9EE4() {}
int fn_103_9EE8(u8* p) { return *(int*)(p + 0x110); }
int fn_103_9F00() { return 0x0; }
void fn_103_9F08() {}
void fn_103_9F0C() {}
void fn_103_9F10() {}
void fn_103_9F14() {}
int fn_103_9F18() { return 0x0; }
int fn_103_9F20() { return 0x0; }
void fn_103_9F28() {}
void fn_103_9F2C() {}
void fn_103_9F30() {}
void fn_103_9F34() {}
int fn_103_A0A8(u8* p) { return *(int*)(p + 0x20); }
int fn_103_A184(u8* p) { return *(int*)(p + 0x18); }
int fn_103_A21C(u8* p) { return *(int*)(p + 0x10); }
int fn_103_B328() { return 0x3; }
void fn_103_C138() {}
int fn_103_C93C() { return 0x0; }
void fn_103_C978() {}
int fn_103_CA24() { return 0x0; }
int fn_103_CAD4() { return 0x0; }
int fn_103_CB84() { return 0x0; }
void fn_103_CBC0() {}
int fn_103_CC6C() { return 0x0; }
void fn_103_CCA8() {}
int fn_103_CD54() { return 0x0; }
void fn_103_CD90() {}
int fn_103_CE3C() { return 0x0; }
void fn_103_CE78() {}
int fn_103_CF24() { return 0x0; }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_103_D148(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_103_D150(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
bool fn_103_D158(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_103_D168(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_103_D170(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
void fn_103_D190(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
bool fn_103_D1A8(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_103_D1B0(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_103_D1B8(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_103_D1D8(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_103_D1F0(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_103_D210(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_103_D218(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_103_D220(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_103_D228(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_103_D230(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_103_D238(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_103_D240(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_103_D248(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_103_D250(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_103_D258(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_103_D260(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_103_D268(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_103_D270(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_103_D278(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_103_D280(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_103_D288(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_103_D290(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_103_D298(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
void fn_103_D160(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
bool fn_103_D180(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_103_D1C8(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_103_D1E0(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_103_D1E8(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_103_D1F8(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_103_1E28(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_3770(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_3C7C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_400C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_4408(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_4684(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_46C4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_52FC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_533C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_103_537C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"
