#define MT_VEC2F_PLAIN_BASE
#define FT_PIKMIN_OWNER_INFO_DTOR
#define MT_VEC2F_FROM_VEC3_CTOR_NOINLINE
#define MT_VEC2F_SUB_NOINLINE
#define MT_VEC2F_LENGTH_SQ_OUT_OF_LINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PIKMIN_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <so/so_array.h>
#include <so/templates/so_array_value_wnPikminPikminOwnerInfo.h>
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/fighter.h>
#include <ft/ft_owner.h>
#include <so/so_slow.h>
#include <ft/ft_manager.h>
#include <cm/cm_camera_controller.h>
#include <st/stage.h>
#include <so/so_external_value_accesser.h>
#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/ground/so_ground_util.h>
#include <so/stageobject.h>
#include <ft/pikmin/ft_pikmin.h>
#include <ft/pikmin/ft_pikmin_utility.h>
#include <ft/pikmin/ft_pikmin_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <so/sound/so_sound_module_impl.h>

// HYPOTHESIS: soValueAccesser's variation value is a plain global here (no header declaration yet)
extern s32 g_soValueVariation;

#define FT_BC ftPikminBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// MATCH-ONLY: unlike the other fighters, ftPikmin calls the ftFighterBuilder constructor out of line (it has a large
// constructor body of its own that MWCC's inliner no longer fits it into).
#pragma dont_inline on
template ftFighterBuilder<ftPikminBuildConfig>::ftFighterBuilder(s32, Heaps::HeapType, Heaps::HeapType, Heaps::HeapType, s32);
#pragma dont_inline off

// MATCH-ONLY: the target reaches the common data accesser through an out-of-line getter in this unit.
extern "C" void* fn_113_62F4();

ftPikminExtendParamAccesser g_ftPikminExtendParamAccesser;

ftClassInfoImpl<Fighter_Pikmin, ftPikmin> g_ftClassInfoPikmin;

// MATCH-ONLY: the native constructor and destructor call the member constructors, the accessors and the status process
// destructors out of line.
#pragma dont_inline on
ftPikmin::ftPikmin(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPikminBuildConfig>(entryId,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap,
                                         -1),
    m_commonData(static_cast<ftCommonDataAccesser*>(fn_113_62F4())->getData(Fighter_Pikmin)),
    m_troops(),
    m_owners(0),
    m_specialS(&m_troops),
    m_attackAir(&m_troops),
    m_attackS4Start(&m_troops),
    m_attackS4Hold(&m_troops),
    m_attackS4(&m_troops),
    m_attackHi4Start(&m_troops),
    m_attackHi4Hold(&m_troops),
    m_attackHi4(&m_troops),
    m_specialHi(&m_troops),
    m_catch(&m_troops),
    m_catchWait(&m_troops) {
    // The status processes of the fighter's own moves: the throws and smashes, the aerials, the grab; the side and up special are
    // the processes at the special move status range.
    getModuleAccesser()->getStatusModule().setUniqProc(0x3c, ftPikminStatusUniqProcessThrow::getInstance());
    getModuleAccesser()->getStatusModule().setUniqProc(0x2a, &m_attackS4Start);
    getModuleAccesser()->getStatusModule().setUniqProc(0x2b, &m_attackS4Hold);
    getModuleAccesser()->getStatusModule().setUniqProc(0x2c, &m_attackS4);
    getModuleAccesser()->getStatusModule().setUniqProc(0x30, &m_attackHi4Start);
    getModuleAccesser()->getStatusModule().setUniqProc(0x31, &m_attackHi4Hold);
    getModuleAccesser()->getStatusModule().setUniqProc(0x32, &m_attackHi4);
    getModuleAccesser()->getStatusModule().setUniqProc(0x2f, ftPikminStatusUniqProcessSmashAttack::getInstance());
    getModuleAccesser()->getStatusModule().setUniqProc(0x33, &m_attackAir);
    getModuleAccesser()->getStatusModule().setUniqProc(0x34, &m_catch);
    getModuleAccesser()->getStatusModule().setUniqProc(0x36, &m_catch);
    getModuleAccesser()->getStatusModule().setUniqProc(0x38, &m_catch);
    getModuleAccesser()->getStatusModule().setUniqProc(0x39, &m_catchWait);
    getModuleAccesser()->getStatusModule().setUniqProc(0x3a, &m_catchWait);
    soStatusUniqProcess* special[10] = {0};
    special[1] = &m_specialS;
    special[2] = &m_specialHi;
    getModuleAccesser()->getStatusModule().addRangeUniqProc(special, 10);
}

ftPikmin::~ftPikmin() { }
#pragma dont_inline off

// The activation of the articles lives in the fighter's article helpers (still unnamed).

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Pikmin, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftPikminArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftPikminArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftPikminArticleKind : wnArticleKindInfo {
    ftPikminArticleKind() {
        kind = 0;
        subKind = Fighter_Pikmin;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftPikminArticleBuildInfo : wnArticleBuildInfo {
    ftPikminArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_148B80(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148B94(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148BA8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftPikminArticleTraits;
#define FT_PIKMIN_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftPikminArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftPikminArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Pikmin, &tag.m_resourceGroup)); \
        } \
    };

FT_PIKMIN_TRAITS(wnPikminPikmin, wnPikminPikminData, 0, fn_27_148B80)
FT_PIKMIN_TRAITS(wnPikminDolfin, wnPikminDolfinData, 1, fn_27_148B94)
FT_PIKMIN_TRAITS(wnPikminChain, wnPikminChainData, 2, fn_27_148BA8)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftPikminArticleTraits<W>::ArticleId, ftPikminArticleBuildInfo(ftPikminArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftPikminArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnPikminPikmin>::~wnArticleHolder();
template wnArticleHolder<wnPikminDolfin>::~wnArticleHolder();
template wnArticleHolder<wnPikminChain>::~wnArticleHolder();
template wnArticleHolder<wnPikminPikmin>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftPikminArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftPikminArticleMediator, 10>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftPikminArticleMediator, 10>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftPikminArticleMediator::~ftPikminArticleMediator() { }
#pragma dont_inline off

s32 ftPikminArticleMediator::getMediateNum() { return 3; }
void ftPikminArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Pikmin, 1 Dolfin, 2 Chain (the rest are unused).

#define FT_PIKMIN_POOL_PIKMIN_BASE static_cast<wnArticlePool<wnPikminPikmin, 6, ftPikminDolfinPool>&>(m_pools)
#define FT_PIKMIN_POOL_PIKMIN FT_PIKMIN_POOL_PIKMIN_BASE.getSub()
#define FT_PIKMIN_POOL_DOLFIN_BASE static_cast<wnArticlePool<wnPikminDolfin, 1, ftPikminChainPool>&>(m_pools)
#define FT_PIKMIN_POOL_DOLFIN FT_PIKMIN_POOL_DOLFIN_BASE.getSub()
#define FT_PIKMIN_POOL_CHAIN_BASE static_cast<wnArticlePool<wnPikminChain, 1, soInstancePoolRoot>&>(m_pools)
#define FT_PIKMIN_POOL_CHAIN FT_PIKMIN_POOL_CHAIN_BASE.getSub()

#define FT_PIKMIN_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(3, value) WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) \
    WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) \
    WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) \
    WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) \
    WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftPikminArticleMediator::deactivate() {
    for (s32 i = 0; i < 6; ++i) {
        wnPikminPikmin* weapon = FT_PIKMIN_POOL_PIKMIN.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnPikminDolfin* weapon = FT_PIKMIN_POOL_DOLFIN.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnPikminChain* weapon = FT_PIKMIN_POOL_CHAIN.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftPikminArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 6; }
    case 1: { wnArticleSlotTag tag; return 1; }
    case 2: { wnArticleSlotTag tag; return 1; }
    FT_PIKMIN_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftPikminActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_PIKMIN_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftPikminActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftPikminActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftPikminArticleMediator::getActiveNum(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: FT_PIKMIN_COUNT_ACTIVE(FT_PIKMIN_POOL_PIKMIN, 6)
    case 1: FT_PIKMIN_COUNT_ACTIVE(FT_PIKMIN_POOL_DOLFIN, 1)
    case 2: FT_PIKMIN_COUNT_ACTIVE(FT_PIKMIN_POOL_CHAIN, 1)
    FT_PIKMIN_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_PIKMIN_CAN_GENERATE(POOL, N, ID) \
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

bool ftPikminArticleMediator::isGeneratable(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: FT_PIKMIN_CAN_GENERATE(FT_PIKMIN_POOL_PIKMIN, 6, 0)
    case 1: FT_PIKMIN_CAN_GENERATE(FT_PIKMIN_POOL_DOLFIN, 1, 1)
    case 2: FT_PIKMIN_CAN_GENERATE(FT_PIKMIN_POOL_CHAIN, 1, 2)
    FT_PIKMIN_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftPikminArticleActivator;
template <> struct ftPikminArticleActivator<wnPikminPikmin> {
    static bool activate(wnPikminPikmin* weapon, soModuleAccesser* acc) { return ftPikminUtility::activeArticle(weapon, acc); }
};
template <> struct ftPikminArticleActivator<wnPikminDolfin> {
    static bool activate(wnPikminDolfin* weapon, soModuleAccesser* acc) { return ftPikminUtility::activeArticle1(weapon, acc); }
};
template <> struct ftPikminArticleActivator<wnPikminChain> {
    static bool activate(wnPikminChain* weapon, soModuleAccesser* acc) { return ftPikminUtility::activeArticle2(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_PIKMIN_GENERATE(W, POOL) \
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
        if (ftPikminArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftPikminArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_PIKMIN_GENERATE(wnPikminPikmin, FT_PIKMIN_POOL_PIKMIN)
    case 1: FT_PIKMIN_GENERATE(wnPikminDolfin, FT_PIKMIN_POOL_DOLFIN)
    case 2: FT_PIKMIN_GENERATE(wnPikminChain, FT_PIKMIN_POOL_CHAIN)
    FT_PIKMIN_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftPikminArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; (void)dynamic_cast<wnPikminPikmin&>(*article); return true; }
    case 1: { wnArticleSlotTag tag; (void)dynamic_cast<wnPikminDolfin&>(*article); return true; }
    case 2: { wnArticleSlotTag tag; (void)dynamic_cast<wnPikminChain&>(*article); return true; }
    FT_PIKMIN_UNUSED_SLOTS(true)
    default: return false;
    }
}


// ---- the fighter overrides ------------------------------------------------------------------------------------------
// Work 0x10000040 / 0x10000041 are reset to -1 when Olimar becomes active.
// HYPOTHESIS: meaning of the works.
void ftPikmin::onActivate() {
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000041);
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000040);
}

// The line of Pikmin is dropped and, unless the owner wears the spycloak, the common effect 0x1a000e (HYPOTHESIS: the start
// glow) follows the model, tinted by the costume.
void ftPikmin::onStart(int startKind) {
    m_moduleAccesser->getGenerateArticleManageModule().deactivate(true);
    m_troops.reset();
    Fighter::onStart(startKind);
    if (g_ftManager->getOwner(m_moduleAccesser->getWorkManageModule().getInt(0x10000000))->getSpycloak() == false) {
        int variation = soValueAccesser::getValueInt(m_moduleAccesser, 0x10000036, 0);
        if (variation == 12) {
            variation = 0;
        }
        m_moduleAccesser->getEffectModule().reqFollow(static_cast<EfID>(0x1a000e), 0x1e, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, true, 0x10, variation, -1);
    }
}

// On the start kinds 1 to 3 the three Pikmin of the first article slot are generated and entered at once.
void ftPikmin::postStart(int startKind) {
    switch (startKind) {
    case 0:
        break;
    case 1:
    case 2:
    case 3:
        for (int i = 0; i < 3; i++) {
            soArticle* article = m_moduleAccesser->getGenerateArticleManageModule().generate(0, false, false);
            if (article->isNull() == false) {
                m_moduleAccesser->getGenerateArticleManageModule().entry(article);
            }
        }
        break;
    }
}

void ftPikmin::processUpdate() {
    bool update = false;
    if (soSlow::getInstance()->isEstimate() == true) {
        if (m_moduleAccesser->getStopModule().isStop() == 0 && m_moduleAccesser->getSlowModule().isSkip() == 0) {
            update = true;
        }
    }
    if (update == true) {
        updatePikmin();
        // HYPOTHESIS: the flag 0x22000010 asks to sort the line again while the status 0x115 runs.
        if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x115 &&
            m_moduleAccesser->getWorkManageModule().isFlag(0x22000010) == true) {
            m_troops.sortPikmin();
            m_moduleAccesser->getWorkManageModule().offFlag(0x22000010);
        }
    }
    Fighter::processUpdate();
    if (update == true) {
        updateHoldPikmin();
    }
}

// While the status 0x119 holds the flag 0x12000001, falling into the dead area clears it.
void ftPikmin::processFixCamera() {
    Fighter::processFixCamera();
    if (soSlow::getInstance()->isAdjust() != 0) {
        if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x119 &&
            m_moduleAccesser->getWorkManageModule().isFlag(0x12000001) == true) {
            Vec3f pos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(m_moduleAccesser->getModelModule().getCorrectNodeId(0x12d), false);
            if (soGroundUtil::checkDeadArea(&pos, 0.0f, 0.0f) == -1) {
                m_moduleAccesser->getWorkManageModule().offFlag(0x12000001);
            }
        }
    }
}

// A Pikmin article (slot 0) that leaves the pool leaves the line too.
void ftPikmin::notifyArticleEventEject(int unk1, int unk2, int* unk3) {
    if (unk1 == 0) {
        int taskId = reinterpret_cast<soModuleAccesser*>(unk3)->m_stageObject->m_taskId;
        m_troops.erasePikmin(taskId);
        m_troops.updateJostlePriority();
    }
    StageObject::notifyArticleEventEject(unk1, unk2, unk3);
}

void ftPikmin::notifyArticleEventRemove(int unk1, int* unk2) {
    if (unk1 == 0) {
        int taskId = reinterpret_cast<soModuleAccesser*>(unk2)->m_stageObject->m_taskId;
        m_troops.erasePikmin(taskId);
        m_troops.updateJostlePriority();
    }
    StageObject::notifyArticleEventRemove(unk1, unk2);
}

// The capture statuses 0x39, 0x3a and 0x3c do not take part in the common handling.
// HYPOTHESIS: the status numbers.
bool ftPikmin::notifyEventCaptureStatus(soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) {
    bool result;
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x39:
    case 0x3a:
    case 0x3c:
        result = false;
        break;
    default:
        result = Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3);
        break;
    }
    return result;
}

void ftPikmin::notifyEventOnDamage(soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser);
    if (damage->m_reaction >= soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfad, 0)) {
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000018);
    }
}

// The animation command ')' shows the visibility record 1 (HYPOTHESIS: the Pikmin on the back).
bool ftPikmin::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* acc, s32 index) {
    bool result;
    s8 group = cmd->getGroup();
    if (!isObserv(group)) {
        result = false;
    } else {
        switch (cmd->getType()) {
        case ')':
            acc->getVisibilityModule().set(1, 1);
            result = true;
            break;
        default:
            result = Fighter::notifyEventAnimCmd(cmd, acc, index);
            break;
        }
    }
    return result;
}

// HYPOTHESIS: the camera controller keeps a one-bit flag (set while Olimar runs the Final Smash) in the byte at 0x44.
#define FT_PIKMIN_CAMERA_FLAG_BYTE (*(reinterpret_cast<s8*>(CameraController::getInstance()) + 0x44))

// The Pikmin of the hold list stand hidden at Olimar's position while he runs a status that shows none of them: the owner info
// takes the condition, every hold Pikmin in the waiting status 0x33 follows and is set to `status` (0 means: back to the
// ground if it can attach there, else status 0xa).
// HYPOTHESIS: names of the statuses and of the argument.
void ftPikmin::reduceHideWaitPikmin(int status) {
    refreshOwnerInfoPos();
    const soArrayList<wnPikminPikmin*, 10>* hold = m_troops.getHoldPikminList();
    int num = hold->size();
    if (num > 0) {
        Vec3f position = m_moduleAccesser->getPostureModule().getPos();
        Vec2f pos(position);
        int condition = ftPikminUtility::getPikminOwnerCondition(m_moduleAccesser, 0.0f);
        getOwners().at(0).unkc.bits.condition = condition;
        float lr = m_moduleAccesser->getPostureModule().getLr();
        for (int i = 0; i < num; i++) {
            wnPikminPikmin* pikmin = hold->at(i);
            if (soExternalValueAccesser::getStatusKind(pikmin) == 0x33) {
                m_troops.syncPos(pikmin, &pos);
                m_troops.syncLr(pikmin, lr);
                m_troops.updateNodeSRT(pikmin);
                m_troops.setTargetPos(pikmin, &pos);
                m_troops.setOwnerCondition(pikmin, condition);
                m_troops.onInstanceFlag(pikmin, 0x12000004);
                if (status == 0) {
                    if (m_troops.attachGround(pikmin) == true) {
                        pikmin->changeStatus(status);
                    } else {
                        pikmin->changeStatus(0xa);
                    }
                } else {
                    pikmin->changeStatus(status);
                }
            }
        }
        m_troops.reducePikminAll(&getOwners());
    }
}

// Olimar's status decides what happens to the Pikmin: entering the hold statuses (0x2d, 0x3a, 0x80) hands them the matching
// Pikmin status, leaving a status that showed them (the second switch) lets them fall back into the line.
// HYPOTHESIS: all status numbers and the Pikmin status numbers 0xa, 0x12, 0x25, 0x2f, 0x30.
void ftPikmin::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    switch (statusKind) {
    case 0x2d: {
        m_troops.holdPikmin(2);
        if (m_troops.getHoldPikminNum() > 0) {
            const soArrayList<wnPikminPikmin*, 10>* hold = m_troops.getHoldPikminList();
            hold->at(0)->changeStatus(0x12);
            soPostureModule& posture = m_moduleAccesser->getPostureModule();
            wnPikminPikmin* const& first = hold->at(0);
            m_troops.syncLr(first, posture.getLr());
            if (hold->size() > 1) {
                hold->at(1)->changeStatus(0x12);
                soPostureModule& posture2 = m_moduleAccesser->getPostureModule();
                wnPikminPikmin* const& second = hold->at(1);
                m_troops.syncLr(second, -posture2.getLr());
            }
        }
        break;
    }
    case 0xbd:
        m_troops.reset();
        break;
    case 0x3a:
        if (m_troops.getHoldPikminNum() > 0) {
            m_troops.getHoldPikminTop()->changeStatus(0x25);
        }
        break;
    case 0x116: {
        m_moduleAccesser->getGenerateArticleManageModule().removeExist(1, 0);
        soArticle* article = m_moduleAccesser->getGenerateArticleManageModule().generate(1, 0, 0);
        if (m_moduleAccesser->getLinkModule().link(6, article->getTaskId()) == false) {
            return;
        }
        m_moduleAccesser->getGenerateArticleManageModule().entry(article);
        FT_PIKMIN_CAMERA_FLAG_BYTE |= 1;
        break;
    }
    case 0x118: {
        m_moduleAccesser->getEffectModule().setSyncScale(false);
        m_moduleAccesser->getEffectModule().req2D(static_cast<EfID>(0xf70001), &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0);
        m_moduleAccesser->getEffectModule().setSyncScale(true);
        Stage* stage = g_Stage;
        if (stage == NULL) {
            return;
        }
        Vec3f center(0.0f, 0.0f, 0.0f);
        stage->getKirifudaPos(&center, 2);
        m_moduleAccesser->getCameraModule().setWhole(true);
        Vec3f focus = center;
        Vec3f focusRest = center;
        Vec3f focusAgain = center;
        m_moduleAccesser->getCameraModule().updateForce(0, &focusAgain, &focusRest);
        m_moduleAccesser->getCameraModule().setEnableUpdatePos(false, -1);
        break;
    }
    case 0x80: {
        const soArrayList<wnPikminPikmin*, 10>* hold = m_troops.getHoldPikminList();
        if (hold->size() > 0) {
            for (int i = 0; i < hold->size(); i++) {
                hold->at(i)->changeStatus(0x2f);
            }
            m_troops.setVisibilityChain(1, -1, 0);
        }
        break;
    }
    case 0x7a:
        refreshOwnerInfoPos();
        break;
    }
    switch (prevStatusKind) {
    case 0x2d:
    case 0x2e:
        if (statusKind != 0x2e && statusKind != 0x2f) {
            if (m_troops.getHoldPikminNum() > 0) {
                const soArrayList<wnPikminPikmin*, 10>* hold = m_troops.getHoldPikminList();
                for (int i = 0; i < hold->size(); i++) {
                    hold->at(i)->changeStatus(0xa);
                }
                m_troops.reducePikminAll(&getOwners());
            }
        }
        break;
    case 0x39:
    case 0x3a:
        if (m_troops.getHoldPikminNum() > 0) {
            if (statusKind == 0x3b) {
                m_troops.reducePikminAll(&getOwners());
            } else if (statusKind != 0x3c && statusKind != 0x3a && statusKind != 0x39) {
                if (soExternalValueAccesser::getStatusKind(m_troops.getHoldPikminTop()) != 0x24) {
                    m_troops.getHoldPikminTop()->changeStatus(0xa);
                }
                m_troops.reducePikminAll(&getOwners());
            }
        }
        break;
    case 0x3c:
        m_troops.reducePikminAll(&getOwners());
        break;
    case 0x80:
        if (statusKind == 0x81) {
            const soArrayList<wnPikminPikmin*, 10>* hold = m_troops.getHoldPikminList();
            for (int i = 0; i < hold->size(); i++) {
                wnPikminPikmin* pikmin = hold->at(i);
                int pikminStatus = soExternalValueAccesser::getStatusKind(pikmin);
                if (pikminStatus == 0x2e || pikminStatus == 0x2f) {
                    pikmin->changeStatus(0x30);
                }
            }
        } else if (ftPikminUtility::isHideOrderOwnerCondition(statusKind) == false) {
            reduceHideWaitPikmin(8);
        }
        ftPikminUtility::unRefLogGroup(moduleAccesser);
        break;
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x95:
        if (ftPikminUtility::isHideOrderOwnerCondition(statusKind) == false) {
            reduceHideWaitPikmin(0);
        }
        break;
    case 0x75:
        if (statusKind != 0x76 && statusKind != 0x77 && statusKind != 0x78 && statusKind != 0x79) {
            if (ftPikminUtility::isHideOrderOwnerCondition(statusKind) == false) {
                reduceHideWaitPikmin(8);
            }
        }
        if (statusKind == 0xe) {
            refreshOwnerInfoPos();
        }
        break;
    case 0x79:
        if (ftPikminUtility::isHideOrderOwnerCondition(statusKind) == false) {
            reduceHideWaitPikmin(8);
        }
        break;
    case 0x94:
        if (statusKind != 0x95) {
            if (ftPikminUtility::isHideOrderOwnerCondition(statusKind) == false) {
                reduceHideWaitPikmin(8);
            }
        }
        break;
    case 0xc0:
    case 0xc2:
        if (ftPikminUtility::isHideOrderOwnerCondition(statusKind) == false) {
            reduceHideWaitPikmin(8);
        }
        break;
    case 0x82:
        if (ftPikminUtility::isHideOrderOwnerCondition(statusKind) == false) {
            reduceHideWaitPikmin(8);
        }
        break;
    case 0x81:
        if (statusKind != 0x82) {
            if (ftPikminUtility::isHideOrderOwnerCondition(statusKind) == false) {
                reduceHideWaitPikmin(8);
            }
        }
        break;
    case 0x116:
    case 0x117:
        if (statusKind >= 0x119 || statusKind < 0x117) {
            FT_PIKMIN_CAMERA_FLAG_BYTE &= ~1;
        }
        break;
    case 0x118:
        m_moduleAccesser->getEffectModule().killKind(0xf70001, true, true);
        FT_PIKMIN_CAMERA_FLAG_BYTE &= ~1;
        break;
    }
}

// The link events of the Pikmin articles and of the other objects Olimar is linked to.
// HYPOTHESIS: all event and status numbers; the sender kind 2 / sub kind 0x90 is the Pikmin article, 0x43 the Pikmin Final Smash
// object.
struct ftPikminLinkArgs : soLinkEventArgs {
    int unk8;
};

void ftPikmin::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    int status = m_moduleAccesser->getStatusModule().getStatusKind();
    switch (eventInfo->m_eventKind) {
    case 0x455:
        if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x34 || m_moduleAccesser->getStatusModule().getStatusKind() == 0x36 ||
            m_moduleAccesser->getStatusModule().getStatusKind() == 0x38) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x39, m_moduleAccesser);
        }
        break;
    case 0x456:
        if (status == 0x34 || status == 0x38 || status == 0x36) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
            moduleAccesser->getWorkManageModule().onFlag(0x22000011);
        } else if (status == 0x39) {
            if (m_moduleAccesser->getSituationModule().getKind() == 0) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0, m_moduleAccesser);
            } else {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0xe, m_moduleAccesser);
            }
        }
        break;
    case 0x457:
        if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x34 || m_moduleAccesser->getStatusModule().getStatusKind() == 0x36 ||
            m_moduleAccesser->getStatusModule().getStatusKind() == 0x38 || m_moduleAccesser->getStatusModule().getStatusKind() == 0x39 ||
            m_moduleAccesser->getStatusModule().getStatusKind() == 0x3a) {
            if (m_troops.getHoldPikminNum() > 0) {
                if (m_troops.getHoldPikminTop()->m_taskId != stageObj->m_taskId) {
                    break;
                }
            }
            moduleAccesser->getWorkManageModule().onFlag(0x22000011);
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x3b, m_moduleAccesser);
        }
        break;
    case 0x458:
        m_moduleAccesser->getStatusModule().changeStatusRequest(0x118, m_moduleAccesser);
        break;
    case 0x459:
        m_moduleAccesser->getStatusModule().changeStatusRequest(0x119, m_moduleAccesser);
        break;
    case 0x45a:
        m_moduleAccesser->getStatusModule().changeStatusRequest(0x11a, m_moduleAccesser);
        break;
    case 0x45b:
        if (m_moduleAccesser->getGenerateArticleManageModule().getNum(0) >= 6 && m_troops.isMakeBloodAll() == true) {
            m_outsideEventPresenter.notifyEventPikminMakeBloomAll();
        }
        break;
    case 0x45c:
        if (stageObj->soGetKind() == 2 && stageObj->soGetSubKind() == 0x43) {
            m_troops.holdPikminForce(stageObj->m_taskId);
        }
        break;
    }
    if (stageObj->soGetKind() == 2 && stageObj->soGetSubKind() == 0x90) {
        switch (eventInfo->m_eventKind) {
        case 0x83b:
            if (stageObj->soGetKind() == 2 && stageObj->soGetSubKind() == 0x90) {
                int* value = static_cast<int*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA80E, 0));
                moduleAccesser->getWorkManageModule().setInt(*value, 0x20000001);
                moduleAccesser->getStatusModule().changeStatusRequest(0x81, moduleAccesser);
                if (moduleAccesser->getGenerateArticleManageModule().isExist(2) == true) {
                    if (moduleAccesser->getLinkModule().link(3, moduleAccesser->getGenerateArticleManageModule().getArticle(2)->getTaskId()) == true) {
                        moduleAccesser->getLinkModule().setModelConstraintPosOrt(3, moduleAccesser->getModelModule().getCorrectNodeId(0x12e), static_cast<ftPikminLinkArgs*>(eventInfo)->unk8, 5, false);
                    }
                }
            }
            break;
        case 0x83c:
            if (stageObj->soGetKind() == 2 && stageObj->soGetSubKind() == 0x90) {
                moduleAccesser->getGenerateArticleManageModule().removeExist(2, 0);
                if (moduleAccesser->getGroundModule().isStCliff(0) != 0) {
                    moduleAccesser->getStatusModule().changeStatusRequest(0x75, moduleAccesser);
                    Vec3f position = m_moduleAccesser->getPostureModule().getPos();
                    Vec2f pos(position);
                    const soArrayList<wnPikminPikmin*, 10>* hold = m_troops.getHoldPikminList();
                    for (int i = 0; i < hold->size(); i++) {
                        wnPikminPikmin* pikmin = hold->at(i);
                        m_troops.syncPos(pikmin, &pos);
                        m_troops.updateNodeSRT(pikmin);
                        pikmin->changeStatus(0x33);
                    }
                } else {
                    moduleAccesser->getStatusModule().changeStatusRequest(0x7b, moduleAccesser);
                    const soArrayList<wnPikminPikmin*, 10>* hold = m_troops.getHoldPikminList();
                    for (int i = 0; i < hold->size(); i++) {
                        hold->at(i)->changeStatus(8);
                    }
                    m_troops.reducePikminAll(&getOwners());
                }
                ftPikminUtility::unRefLogGroup(m_moduleAccesser);
            }
            break;
        case 0x83d: {
            const soArrayList<wnPikminPikmin*, 10>* hold = m_troops.getHoldPikminList();
            for (int i = 0; i < hold->size(); i++) {
                int pikminStatus = soExternalValueAccesser::getStatusKind(hold->at(i));
                if (pikminStatus == 0x2e || pikminStatus == 0x2f) {
                    hold->at(i)->changeStatus(0x30);
                }
            }
            break;
        }
        case 0x83e:
            moduleAccesser->getStatusModule().changeStatusRequest(0x83, moduleAccesser);
            return;
        case 0x83f:
            m_troops.setVisibilityChain(1, static_cast<ftPikminLinkArgs*>(eventInfo)->unk8, 1);
            break;
        }
    } else if (stageObj->soGetKind() == 2 && stageObj->soGetSubKind() == 0x43) {
        switch (eventInfo->m_eventKind) {
        case 0x45b:
            if (m_moduleAccesser->getGenerateArticleManageModule().getNum(0) >= 6 && m_troops.isMakeBloodAll() == true) {
                m_outsideEventPresenter.notifyEventPikminMakeBloomAll();
            }
            break;
        case 2:
            m_troops.erasePikmin(stageObj->m_taskId);
            m_troops.updateJostlePriority();
            break;
        }
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

// Stand-ins for the helpers behind the unit split.
extern "C" void fn_113_BF8C(Vec2f* a, Vec2f* b, Vec2f* out); // Vec2f::add, out of line in this unit

// Every frame Olimar leaves a new owner info behind him: his position, the way he faces, the condition for the speed he moves
// with and an option byte. The older infos drift with the movement of the frame (the Pikmin of the line walk to them), the
// oldest one drops out when the array is full.
// HYPOTHESIS: names of the kinetic energy and attribute numbers.
void ftPikmin::updatePikmin() {
    if (getOwnersVector().isFull() == true) {
        getOwnersVector().pop();
    }
    Vec2f speed = m_moduleAccesser->getKineticModule().getEnergy(6)->getSpeed();
    Vec2f shift = m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(7));
    if (speed.lengthSq() != 0.0f) {
        fn_113_BF8C(&shift, &(speed - m_moduleAccesser->getGroundModule().getDownMovementSpeed(0)), &shift);
    }
    int condition = ftPikminUtility::getPikminOwnerCondition(m_moduleAccesser, shift.length());
    u8 option = 0;
    ftPikminUtility::getPikminOwnerOptionInfo(&option, m_moduleAccesser);
    float lr = m_moduleAccesser->getPostureModule().getLr();
    Vec2f pos(m_moduleAccesser->getPostureModule().getPos());
    wnPikminPikminOwnerInfo info;
    info.unk0.words[0] = reinterpret_cast<u32*>(&pos)[0];
    info.unk0.words[1] = reinterpret_cast<u32*>(&pos)[1];
    info.unk8 = lr;
    info.unkc.bits.condition = condition;
    info.unkc.bytes.unkd = option;
    getOwnersVector().unshift(info);
    if (speed.lengthSq() != 0.0f) {
        int num = getOwnersVector().size();
        for (int i = 1; i < num; i++) {
            fn_113_BF8C(reinterpret_cast<Vec2f*>(&getOwnersVector().at(i)), &speed, reinterpret_cast<Vec2f*>(&getOwnersVector().at(i)));
        }
    }
    m_troops.update(&getOwners());
}

// A new Pikmin joins the line behind Olimar.
void ftPikmin::addPikmin(wnPikminPikmin* pikmin) {
    m_troops.addPikmin(pikmin);
    m_troops.updateJostlePriority();
}

// The Pikmin of the line stand at the owner infos again.
void ftPikmin::reducePikminAll() {
    m_troops.reducePikminAll(&getOwners());
}

// Olimar's position is the target of every owner info.
void ftPikmin::refreshOwnerInfoPos() {
    int num = getOwners().size();
    Vec3f position = m_moduleAccesser->getPostureModule().getPos();
    Vec2f pos(position);
    for (int i = 0; i < num; i++) {
        Vec2f* owner = reinterpret_cast<Vec2f*>(&getOwners().at(i));
        owner->m_x = pos.m_x;
        owner->m_y = pos.m_y;
    }
}

// MATCH-ONLY: the original calls the constructor and destructor of the local Vec2f array out of line (they live in the REL).
FT_DOL_ARRAY_VECTOR(Vec2f, 10);

// MATCH-ONLY: the REL carries the out-of-line helper (the declaration is never_inline, see MT_VEC2F_LENGTH_SQ_OUT_OF_LINE).
float Vec2f::lengthSq() {
    float xSq = m_x * m_x;
    float ySq = m_y * m_y;
    return xSq + ySq;
}

// MATCH-ONLY: Vec2f::Vec2f(float, float) is a function of the REL that this unit calls; the class keeps it inline (the kinetic
// energy helpers of the unit need the inline one), so the out-of-line copy is spelled as a function of its own.
extern "C" void __ct__5Vec2fFff(Vec2f* self, float x, float y) {
    self->m_x = x;
    self->m_y = y;
}

static inline Vec2f addVec2f(Vec2f& a, Vec2f& b) {
    Vec2f sum;
    fn_113_BF8C(&a, &b, &sum);
    return sum;
}

// Stand-ins for the status-process classes (at the offsets given) that update their own hold Pikmin.

// Olimar's statuses that carry Pikmin in hand hand the update to their status-process class (members at 0x200D8..0x20128);
// the plain hold statuses 0x2d..0x2f place the hold Pikmin themselves: the two spots they take are the hand node moved by the
// constant offset (turned by the facing and the scale) and the same node moved the other way.
// HYPOTHESIS: status numbers, the meaning of the two spots and of the constants 0xfa8/0xfa9.
void ftPikmin::updateHoldPikmin() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x33:
        m_attackAir.updateHoldPikmin(m_moduleAccesser);
        break;
    case 0x113:
        m_specialS.updateHoldPikmin(m_moduleAccesser);
        break;
    case 0x2a:
    case 0x2b:
    case 0x2c:
        m_attackS4.updateHoldPikmin(m_moduleAccesser);
        break;
    case 0x30:
    case 0x31:
    case 0x32:
        m_attackHi4.updateHoldPikmin(m_moduleAccesser);
        break;
    case 0x2d:
    case 0x2e:
    case 0x2f:
        if (m_troops.getHoldPikminNum() > 0) {
            soArrayVector<Vec2f, 10> spots(0);
            Vec2f handPos(m_moduleAccesser->getModelModule().getNodeGlobalPosition(0, false));
            Vec2f handPosAgain(m_moduleAccesser->getModelModule().getNodeGlobalPosition(0, false));
            soModuleAccesser* acc = m_moduleAccesser;
            float offsetY = soValueAccesser::getConstantFloat(acc, 0xfa9, 0);
            Vec2f offset;
            __ct__5Vec2fFff(&offset, soValueAccesser::getConstantFloat(acc, 0xfa8, 0), offsetY);
            float scale = m_moduleAccesser->getPostureModule().getScale();
            offset.m_x *= scale;
            offset.m_y *= scale;
            if (m_moduleAccesser->getPostureModule().getLr() < 0.0f) {
                offset.m_x = -offset.m_x;
            }
            spots.push(addVec2f(handPos, offset));
            spots.push(handPos - offset);
            m_troops.updateHoldPikmin(reinterpret_cast<const soArrayFixed<wnPikminPikminOwnerInfo>*>(&spots));
        }
        break;
    case 0x114:
        m_specialHi.updateHoldPikmin(m_moduleAccesser);
        break;
    case 0x34:
    case 0x36:
    case 0x38:
        m_catch.updateHoldPikmin(m_moduleAccesser);
        break;
    }
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftPikmin is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftPikminInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftPikminInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
// Vtable view used only to emit the virtual call of soArticle::checkActivate (slot at +0x58, first slot at +0x8).
// Declared but never defined; the slots before it are placeholders.
struct ftPikminArticleSlots {
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
    virtual int checkActivate();
};

extern "C" {

int fn_113_686C(u8* p) { return *(int*)(p + 0x60); }
int fn_113_7FA8() { return 0x0; }
int fn_113_7FBC() { return 0x0; }
int fn_113_8094() { return 0x0; }
int fn_113_809C() { return 0x0; }
int fn_113_80A4() { return 0x0; }
int fn_113_80AC() { return 0x0; }
int fn_113_80D8() { return 0x0; }
int fn_113_80E0() { return 0x0; }
u8 fn_113_80F4(u8* p) { return *(u8*)(p + 0x4); }
void fn_113_9F54() {}
int fn_113_C534() { return 0x0; }
void fn_113_C5F4() {}
void fn_113_C810() {}
u8 fn_113_C814(u8* p) { return *(u8*)(p + 0x44); }
void fn_113_C81C() {}
void fn_113_C85C() {}
void fn_113_C860() {}
void fn_113_C8F0() {}
void fn_113_C91C() {}
void fn_113_C920() {}
void fn_113_C924() {}
void fn_113_C928() {}
void fn_113_C92C() {}
void fn_113_C930() {}
void fn_113_C934() {}
void fn_113_C938() {}
void fn_113_C93C() {}
void fn_113_C940() {}
void fn_113_C944() {}
void fn_113_C948() {}
void fn_113_C94C() {}
void fn_113_C950() {}
void fn_113_C954() {}
int fn_113_C958() { return 0x0; }
void fn_113_C960() {}
void fn_113_C964() {}
void fn_113_C968() {}
void fn_113_C96C() {}
void fn_113_C998() {}
void fn_113_C99C() {}
void fn_113_C9A0() {}
void fn_113_C9A4() {}
void fn_113_C9A8() {}
int fn_113_C9AC(u8* p) { return *(int*)(p + 0x110); }
int fn_113_C9C4() { return 0x0; }
void fn_113_C9CC() {}
void fn_113_C9D0() {}
void fn_113_C9D4() {}
void fn_113_C9D8() {}
int fn_113_C9DC() { return 0x0; }
int fn_113_C9E4() { return 0x0; }
void fn_113_C9EC() {}
void fn_113_C9F0() {}
void fn_113_C9F4() {}
void fn_113_C9F8() {}
void fn_113_C9FC() {}
int fn_113_CB70() { return 0x1; }
void fn_113_CB78() {}
int fn_113_CB7C() { return 0x0; }
void fn_113_CB84() {}
int fn_113_CB88() { return 0x0; }
void fn_113_CB90() {}
void fn_113_CB94() {}
void fn_113_CB98() {}
void fn_113_CB9C() {}
void fn_113_CBA0() {}
void fn_113_CBA4() {}
void fn_113_CBA8() {}
void fn_113_CBAC() {}
int fn_113_CBB0(u8* p) { return *(int*)(p + 0x20); }
int fn_113_CC8C(u8* p) { return *(int*)(p + 0x18); }
int fn_113_CD24(u8* p) { return *(int*)(p + 0x10); }
int fn_113_E000() { return 0x3; }
void fn_113_EEFC() {}
void fn_113_F678() {}
int fn_113_F724() { return 0x0; }
void fn_113_F760() {}
int fn_113_F80C() { return 0x0; }
void fn_113_F848() {}
int fn_113_F8F4() { return 0x0; }
void fn_113_F930() {}
int fn_113_F9DC() { return 0x0; }
void fn_113_FA18() {}
int fn_113_FAC4() { return 0x0; }
void fn_113_FB00() {}
int fn_113_FBAC() { return 0x0; }
void fn_113_FBE8() {}
int fn_113_FC94() { return 0x0; }
void fn_113_FCD0() {}
int fn_113_FD7C() { return 0x0; }

// Field, sub-object and vector helpers recovered from the map names (Class__method).
// soArticleMediatorImpl<...>::setAutoRecycle
void fn_113_E008(u8* p, u8 v) { *(u8*)(p + 0x17758) = v; }
// soArticle::checkActivate (virtual slot 0x58)
int fn_113_E080(void* p) { return static_cast<ftPikminArticleSlots*>(p)->checkActivate(); }
// soInstancePoolSub<...>::getInstanceAt (Chain / Dolfin: one slot at +0xC)
void* fn_113_E090(u8* p, s32 idx) {
    if (idx == 0) {
        return p + 0xc;
    }
    return 0;
}
void* fn_113_E0A8(u8* p, s32 idx) {
    if (idx == 0) {
        return p + 0xc;
    }
    return 0;
}
// soInstancePoolSub<...>::getInstanceAt (Pikmin pool, six slots)
void* fn_113_E0C0(u8* p, s32 idx) {
    if (idx == 5) {
        return p + 0x10088;
    }
    if (idx == 4) {
        return p + 0xCD40;
    }
    if (idx == 3) {
        return p + 0x99F8;
    }
    if (idx == 2) {
        return p + 0x66B0;
    }
    if (idx == 1) {
        return p + 0x3368;
    }
    if (idx == 0) {
        return p + 0x20;
    }
    return 0;
}
// soSoundIdExchangerNull singleton address
void* fn_113_8088() { return &g_soSoundIdExchangerNull; }
// ftCommonDataAccesser::getInstance
void* fn_113_62F4() { return &g_ftCommonDataAccesser; }
// soModuleAccesser::getStatusModule
void* fn_113_6860(u8* p) { return *(void**)(*(u8**)(p + 0xd8) + 0x70); }
// soValueAccesser::getValueVariation
s32 fn_113_D0AC() { return g_soValueVariation; }

// The troops manager is the sub-object at this+0x1FD08; the owner info array sits at this+0x1FE8C.
#define PIKMIN_TROOPS(p) reinterpret_cast<wnPikminPikminTroopsManager*>((p) + 0x1FD08)

// ftPikmin::setTeam: base Fighter::setTeam first, then the troops manager
void fn_113_C4E0(u8* p, int team, int teamOwner) {
    reinterpret_cast<Fighter*>(p)->Fighter::setTeam(team, teamOwner);
    PIKMIN_TROOPS(p)->setTeamAll(team, teamOwner, 0);
}

// ftPikmin overrides that forward to StageObject (direct tail calls, not virtual)
void fn_113_AD94(u8* p) { reinterpret_cast<StageObject*>(p)->StageObject::renderDebug(); }
void fn_113_B918(u8* p) { reinterpret_cast<StageObject*>(p)->StageObject::updateNodeSRT(); }

// Fighter virtual overrides reached through the Fighter base sub-object (this - delta): direct tail calls to Fighter::*
void fn_113_FFA8(u8* p, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) {
    reinterpret_cast<Fighter*>(p - 0x54)->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObject, unk4);
}
void fn_113_FFB0(u8* p, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x64)->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}
bool fn_113_FFB8(u8* p, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) {
    return reinterpret_cast<Fighter*>(p - 0xC4)->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3);
}
void fn_113_FFC0(u8* p, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0xE8)->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser);
}
void fn_113_FFF0(u8* p, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x70)->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
}
bool fn_113_FFF8(u8* p, u32 flags) {
    return reinterpret_cast<Fighter*>(p - 0x7C)->Fighter::notifyEventCollisionAttackCheck(flags);
}
void fn_113_10000(u8* p, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x7C)->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser);
}
void fn_113_10008(u8* p, int index, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x88)->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser);
}
bool fn_113_10010(u8* p) {
    return reinterpret_cast<Fighter*>(p - 0x94)->Fighter::notifyEventCollisionShieldCheck();
}
void fn_113_10018(u8* p, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x94)->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser);
}
void fn_113_10020(u8* p, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x94)->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}
bool fn_113_10028(u8* p) {
    return reinterpret_cast<Fighter*>(p - 0xA0)->Fighter::notifyEventCollisionReflectorCheck();
}
void fn_113_10030(u8* p, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0xA0)->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser);
}
void fn_113_10038(u8* p, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float unk7) {
    reinterpret_cast<Fighter*>(p - 0xA0)->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, unk7);
}
bool fn_113_10040(u8* p) {
    return reinterpret_cast<Fighter*>(p - 0xAC)->Fighter::notifyEventCollisionAbsorberCheck();
}
void fn_113_10048(u8* p, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float unk7) {
    reinterpret_cast<Fighter*>(p - 0xAC)->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, unk7);
}
bool fn_113_10050(u8* p) {
    return reinterpret_cast<Fighter*>(p - 0xB8)->Fighter::notifyEventCollisionSearchCheck();
}
void fn_113_10058(u8* p, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0xB8)->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser);
}
void fn_113_10068(u8* p, BaseItem* item, int unk2, bool unk3, u8 unk4) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4);
}
void fn_113_10070(u8* p, BaseItem* item, u32 index, bool unk3) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyEjectAttachItem(item, index, unk3);
}
void fn_113_10078(u8* p, BaseItem* item, u32 index, bool unk3) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyEjectItem(item, index, unk3);
}
void fn_113_10080(u8* p, BaseItem* item) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyShootBulletItem(item);
}
void fn_113_10088(u8* p, BaseItem* item) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyDropItem(item);
}
void fn_113_10090(u8* p, BaseItem* item, u32 nodeIndex, int* unk3) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyThrowItem(item, nodeIndex, unk3);
}
void fn_113_10098(u8* p, BaseItem* item, u32 nodeIndex, int* unk3) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyUseItem(item, nodeIndex, unk3);
}
void fn_113_100A0(u8* p, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) {
    reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5);
}
bool fn_113_100B0(u8* p, BaseItem* item, bool* unk2) {
    return reinterpret_cast<Fighter*>(p - 0xD0)->Fighter::notifyHaveItemPreCheck(item, unk2);
}
void fn_113_100B8(u8* p, soDamage* damage, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0xE8)->Fighter::notifyEventAddDamage(damage, moduleAccesser);
}
void fn_113_100C8(u8* p, float unk1, int unk2) {
    reinterpret_cast<Fighter*>(p - 0xF4)->Fighter::notifyEventPikminFinalAttack(unk1, unk2);
}
void fn_113_100D0(u8* p) {
    reinterpret_cast<Fighter*>(p - 0xF4)->Fighter::notifyEventChangeAdvUnit();
}
void fn_113_100D8(u8* p) {
    reinterpret_cast<Fighter*>(p - 0xF4)->Fighter::notifyEventBeat();
}
void fn_113_100E0(u8* p, float unk1) {
    reinterpret_cast<Fighter*>(p - 0xF4)->Fighter::notifyEventSetDamage(unk1);
}
void fn_113_100E8(u8* p, float unk1, float unk2, soModuleAccesser* moduleAccesser) {
    reinterpret_cast<Fighter*>(p - 0x100)->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser);
}

// Thunks to functions not reconstructed yet (this - delta, then a direct tail call with the same arguments)
void fn_113_4770(u8* p, int a1, int a2, int a3);
void fn_113_DD20(u8* p, int a1, int a2, int a3);
void fn_113_1EB4(u8* p, int a1, int a2, int a3);
void fn_113_C6B4(u8* p, int a1, int a2, int a3);
void fn_113_FF80(u8* p, int a1, int a2, int a3) { fn_113_4770(p - 0x4, a1, a2, a3); }
void fn_113_FF88(u8* p, int a1, int a2, int a3) { fn_113_DD20(p - 0x4, a1, a2, a3); }
void fn_113_FF90(u8* p, int a1, int a2, int a3) { fn_113_1EB4(p - 0x4, a1, a2, a3); }
void fn_113_FFC8(u8* p, int a1, int a2, int a3) { fn_113_C6B4(p - 0x40, a1, a2, a3); }

// Vec2f helpers (stored as two floats)

void fn_113_BF8C(Vec2f* a, Vec2f* b, Vec2f* out) {
    out->m_x = a->m_x + b->m_x;
    out->m_y = a->m_y + b->m_y;
}

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_113_100A8(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_113_FFA0(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_113_9B4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_113_2908(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_113_2B1C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_113_2E34(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_113_3208(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_113_35B8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_113_35F8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_113_47B0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_113_47F0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"
