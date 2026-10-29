#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_CAPTAIN_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Marth.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/captain/ft_captain.h>
#include <ft/captain/ft_captain_extend_param_accesser.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>

#define FT_BC ftCaptainBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

ftCaptainExtendParamAccesser g_ftCaptainExtendParamAccesser;

ftClassInfoImpl<Fighter_Captain, ftCaptain> g_ftClassInfoCaptain;

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt); the first and the other
// entries in module 27 are shared ft status processes.
extern "C" {
extern soStatusUniqProcess lbl_27_bss_7CDC;
extern soStatusUniqProcess lbl_27_bss_7CEC;
extern soStatusUniqProcess lbl_100_bss_194;
extern soStatusUniqProcess lbl_100_bss_1A4;
extern soStatusUniqProcess lbl_100_bss_1B4;
extern soStatusUniqProcess lbl_100_bss_1C4;
extern soStatusUniqProcess lbl_100_bss_1D4;
extern soStatusUniqProcess lbl_100_bss_1E4;
extern soStatusUniqProcess lbl_100_bss_1F4;
extern soStatusUniqProcess lbl_100_bss_204;
extern soStatusUniqProcess lbl_100_bss_214;
extern soStatusUniqProcess lbl_100_bss_224;
extern soStatusUniqProcess lbl_100_bss_234;
extern soStatusUniqProcess lbl_100_bss_244;
}

ftCaptain::ftCaptain(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftCaptainBuildConfig>(entryId,
                                         Fighter_Captain,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Captain)) {
    // Register the status processes in action order; entry 10 has no process.
    soStatusUniqProcess* processes[16] = {0};
    processes[0] = &lbl_27_bss_7CDC;
    processes[1] = &lbl_100_bss_194;
    processes[2] = &lbl_100_bss_1C4;
    processes[3] = &lbl_100_bss_1A4;
    processes[4] = &lbl_100_bss_1F4;
    processes[5] = &lbl_27_bss_7CEC;
    processes[6] = &lbl_100_bss_194;
    processes[7] = &lbl_100_bss_1D4;
    processes[8] = &lbl_100_bss_1E4;
    processes[9] = &lbl_100_bss_1B4;
    processes[11] = &lbl_100_bss_204;
    processes[12] = &lbl_100_bss_214;
    processes[13] = &lbl_100_bss_224;
    processes[14] = &lbl_100_bss_234;
    processes[15] = &lbl_100_bss_244;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 16);
}

ftCaptain::~ftCaptain() { }

// ---- the fighter overrides ------------------------------------------------------------------------------------------
// The status processes whose members the fighter calls (their code is in the other units of the module).
class ftCaptainStatusUniqProcessFalconKnuckle : public soStatusUniqProcess {
public:
    void transitionToFall(soModuleAccesser* moduleAccesser);
    void checkHitValidFlag(soModuleAccesser* moduleAccesser);
};
extern ftCaptainStatusUniqProcessFalconKnuckle g_ftCaptainStatusUniqProcessFalconKnuckle;

// HYPOTHESIS: members of the other special status processes, only declared here (unnamed in the module map).
extern "C" {
void fn_100_F3D0(soModuleAccesser* moduleAccesser);
void fn_100_D880(soStatusUniqProcess* process, soModuleAccesser* moduleAccesser);
void fn_100_DFF0(soStatusUniqProcess* process, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
bool fn_100_E1A0(soStatusUniqProcess* process, u32 flags, soModuleAccesser* moduleAccesser);
}

void ftCaptain::processUpdate() {
    soModuleAccesser* acc = m_moduleAccesser;
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x116:
    case 0x11d: case 0x11e: case 0x11f: case 0x120: case 0x121:
        fn_100_F3D0(acc);
        break;
    }
    Fighter::processUpdate();
}

// Leaving the Falcon Knuckle's end status to the ground sends it to the fall transition.
void ftCaptain::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (prevStatusKind == 0x113 && statusKind == 0xe) {
        g_ftCaptainStatusUniqProcessFalconKnuckle.transitionToFall(moduleAccesser);
    }
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}

// Each of the special statuses 0x113 / 0x115 / 0x116 checks the hit it dealt.
void ftCaptain::notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x113:
        g_ftCaptainStatusUniqProcessFalconKnuckle.checkHitValidFlag(moduleAccesser);
        break;
    case 0x115:
        fn_100_D880(&lbl_100_bss_1A4, moduleAccesser);
        break;
    case 0x116:
        fn_100_DFF0(&lbl_100_bss_1F4, power, collisionLog, moduleAccesser);
        break;
    }
}

bool ftCaptain::notifyEventCollisionAttackCheck(u32 flags) {
    soModuleAccesser* acc = m_moduleAccesser;
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x116:
        return fn_100_E1A0(&lbl_100_bss_1F4, flags, acc);
    default:
        return Fighter::notifyEventCollisionAttackCheck(flags);
    }
}

// The Falcon of the Final Smash starts at the position of the fighter (or at the origin in the statuses 0x11f / 0x120).
bool ftCaptain::activeArticle(wnCaptainBlueFalcon* weapon, soModuleAccesser* acc) {
    soPostureModule& posture = acc->getPostureModule();
    soStatusModule& status = acc->getStatusModule();
    ftResourceIdAccesserImpl& ids = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
    Vec3f pos;
    pos = posture.getPos();
    float scale = soValueAccesser::getConstantFloat(acc, 0xbe2, 0);
    float lr = posture.getLr();
    bool attached = false;
    switch (status.getStatusKind()) {
    case 0x11f:
    case 0x120:
        lr = 0.0f;
        pos.m_x = lr;
        pos.m_y = lr;
        pos.m_z = lr;
        attached = true;
        break;
    }
    int founderTaskId = acc->getStageObject().m_taskId;
    int team = acc->getTeamModule().getTeam()->getNo();
    weapon->activate(founderTaskId, ids.getFinalResId(), team, &pos, lr, scale, attached);
    acc->getWorkManageModule().setInt(weapon->m_taskId, 0x20000001);
    return true;
}

// The background of the Final Smash starts with the team of the fighter.
bool ftCaptain::activeArticle1(wnCaptainFinalBg* weapon, soModuleAccesser* acc) {
    int founderTaskId;
    ftResourceIdAccesserImpl& ids = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
    founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, ids.getFinalResId(), acc->getTeamModule().getTeam()->getNo());
    return true;
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Captain, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftCaptainArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftCaptainArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftCaptainArticleKind : wnArticleKindInfo {
    ftCaptainArticleKind() {
        kind = 0;
        subKind = Fighter_Captain;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftCaptainArticleBuildInfo : wnArticleBuildInfo {
    ftCaptainArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_149B30(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_149B44(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftCaptainArticleTraits;
#define FT_CAPTAIN_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftCaptainArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftCaptainArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Captain, &tag.m_resourceGroup)); \
        } \
    };

FT_CAPTAIN_TRAITS(wnCaptainBlueFalcon, wnCaptainBlueFalconData, 0, fn_27_149B30)
FT_CAPTAIN_TRAITS(wnCaptainFinalBg, wnCaptainFinalBgData, 1, fn_27_149B44)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftCaptainArticleTraits<W>::ArticleId, ftCaptainArticleBuildInfo(ftCaptainArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftCaptainArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnCaptainBlueFalcon>::~wnArticleHolder();
template wnArticleHolder<wnCaptainFinalBg>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftCaptainArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftCaptainArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftCaptainArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftCaptainArticleMediator::~ftCaptainArticleMediator() { }
#pragma dont_inline off

s32 ftCaptainArticleMediator::getMediateNum() { return 2; }
void ftCaptainArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 BlueFalcon, 1 FinalBg (the rest are unused).

#define FT_CAPTAIN_POOL_BLUEFALCON_BASE static_cast<wnArticlePool<wnCaptainBlueFalcon, 1, ftCaptainFinalBgPool>&>(m_pools)
#define FT_CAPTAIN_POOL_BLUEFALCON FT_CAPTAIN_POOL_BLUEFALCON_BASE.getSub()
#define FT_CAPTAIN_POOL_FINALBG_BASE static_cast<wnArticlePool<wnCaptainFinalBg, 1, soInstancePoolRoot>&>(m_pools)
#define FT_CAPTAIN_POOL_FINALBG FT_CAPTAIN_POOL_FINALBG_BASE.getSub()

#define FT_CAPTAIN_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) WN_ARTICLE_UNUSED_SLOT(4, value) \
    WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) \
    WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) \
    WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) \
    WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftCaptainArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnCaptainBlueFalcon* weapon = FT_CAPTAIN_POOL_BLUEFALCON.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnCaptainFinalBg* weapon = FT_CAPTAIN_POOL_FINALBG.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftCaptainArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 1; }
    case 1: { wnArticleSlotTag tag; return 1; }
    FT_CAPTAIN_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftCaptainActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_CAPTAIN_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftCaptainActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftCaptainActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftCaptainArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_CAPTAIN_COUNT_ACTIVE(FT_CAPTAIN_POOL_BLUEFALCON, 1)
    case 1: FT_CAPTAIN_COUNT_ACTIVE(FT_CAPTAIN_POOL_FINALBG, 1)
    FT_CAPTAIN_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_CAPTAIN_CAN_GENERATE(POOL, N, ID) \
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

bool ftCaptainArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_CAPTAIN_CAN_GENERATE(FT_CAPTAIN_POOL_BLUEFALCON, 1, 0)
    case 1: FT_CAPTAIN_CAN_GENERATE(FT_CAPTAIN_POOL_FINALBG, 1, 1)
    FT_CAPTAIN_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftCaptainArticleActivator;
template <> struct ftCaptainArticleActivator<wnCaptainBlueFalcon> {
    static bool activate(wnCaptainBlueFalcon* weapon, soModuleAccesser* acc) { return ftCaptain::activeArticle(weapon, acc); }
};
template <> struct ftCaptainArticleActivator<wnCaptainFinalBg> {
    static bool activate(wnCaptainFinalBg* weapon, soModuleAccesser* acc) { return ftCaptain::activeArticle1(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_CAPTAIN_GENERATE(W, POOL) \
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
        if (ftCaptainArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftCaptainArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_CAPTAIN_GENERATE(wnCaptainBlueFalcon, FT_CAPTAIN_POOL_BLUEFALCON)
    case 1: FT_CAPTAIN_GENERATE(wnCaptainFinalBg, FT_CAPTAIN_POOL_FINALBG)
    FT_CAPTAIN_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftCaptainArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; (void)dynamic_cast<wnCaptainBlueFalcon&>(*article); return true; }
    case 1: { wnArticleSlotTag tag; (void)dynamic_cast<wnCaptainFinalBg&>(*article); return true; }
    FT_CAPTAIN_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftCaptain is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftCaptainInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftCaptainInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

// HYPOTHESIS: extend param group 3 (ftData + 0x88) holds the Falcon Kick parameters.
void* fn_100_8C4C() { return g_ftCommonDataAccesser.getData(Fighter_Captain)->extendParam[3]; }

u8 fn_100_6C60(u8* p) { return *(u8*)(p + 0x4); }
void fn_100_9670() {}
void fn_100_9674() {}
int fn_100_97E8(u8* p) { return *(int*)(p + 0x20); }
int fn_100_98C4(u8* p) { return *(int*)(p + 0x18); }
void fn_100_B7DC() {}
void fn_100_BF74() {}
void fn_100_C05C() {}
void fn_100_C1F4() {}
void fn_100_C2DC() {}
void fn_100_C3C4() {}
void fn_100_C4AC() {}
void fn_100_C594() {}

}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
u8* fn_100_C1F8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_100_C2E0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"
