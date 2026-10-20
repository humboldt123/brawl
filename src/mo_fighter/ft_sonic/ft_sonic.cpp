// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_SONIC_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_NOINLINE_SEARCH_BUILDER
#define MT_VEC2F_FROM_VEC3_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/sonic/ft_sonic.h>
#include <ft/sonic/ft_sonic_extend_param_accesser.h>
#include <ft/sonic/ft_sonic_transactor.h>
#include <ft/ft_owner.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/sonic/ft_sonic_status_uniq_process_final_end.h>
#include <ft/sonic/ft_sonic_status_uniq_process_special_lw_hold.h>
#include <ft/sonic/ft_sonic_status_uniq_process_special_s_dash.h>
#include <ft/sonic/ft_sonic_status_uniq_process_special_s_wall_end.h>
#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/link/so_link_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <ft/ft_physics_iface.h>
#include <so/so_value_accesser.h>
#include <wn/sonic/wn_sonic_gimmick_jump.h>
#include <wn/sonic/wn_sonic_super_sonic.h>
#include <gf/gf_task_scheduler.h>
#include <math.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/so_kinetic_energy_normal.h>

#define FT_BC ftSonicBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftSonicExtendParamAccesser g_ftSonicExtendParamAccesser;

ftClassInfoImpl<Fighter_Sonic, ftSonic> g_ftClassInfoSonic;

ftSonic::ftSonic(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftSonicBuildConfig>(entryId,
                                         Fighter_Sonic,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_commonData(g_ftCommonDataAccesser.getData(Fighter_Sonic)) {
    // Sonic overrides four of the 21 consecutive character action slots.
    soStatusUniqProcess* processes[21] = {0};
    processes[12] = &g_ftSonicStatusUniqProcessSpecialSDash;
    processes[15] = &g_ftSonicStatusUniqProcessSpecialSWallEnd;
    processes[18] = &g_ftSonicStatusUniqProcessSpecialLwHold;
    processes[20] = &g_ftSonicStatusUniqProcessFinalEnd;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 21);
    // HYPOTHESIS: selects the physics preset used by Sonic's movement.
    ftPhysicsModuleCall6C(m_moduleAccesser->m_enumerationStart->m_physicsModule, 0x32);
}

// ---- fighter events ---------------------------------------------------------------------------------------------------
// HYPOTHESIS: work flag 0x1200003d/0x1200003e are the spin dash bookkeeping flags (one follows the Spycloak), 0x22000011 is
// raised by an attack in the spin dash status (0x11e) and consumed by the attack check.

void ftSonic::onStart(int param) {
    Fighter::onStart(param);
    m_moduleAccesser->getCollisionHitModule().setStatusNodeDefault(0x13, 3, 0);
    m_moduleAccesser->getCollisionHitModule().resetStatusAll(0);
    ftPhysicsModuleCall94(m_moduleAccesser->m_enumerationStart->m_physicsModule);
    if (getOwner()->getSpycloak()) {
        m_moduleAccesser->getWorkManageModule().onFlag(0x1200003e);
    } else {
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003e);
    }
}

void ftSonic::processCollision() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x112) {
        ftSonicTransactor::getInstance()->processCollision(m_moduleAccesser);
    }
    StageObject::processCollision();
}

void ftSonic::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser);
    ftSonicTransactor::getInstance()->notifyEventCollisionSearch(collisionLog, moduleAccesser);
}

void ftSonic::analyzeSeal(void* seal) {
    // HYPOTHESIS: the seal record starts with its kind; kind 0x41 carries a float that is halved into the glow time.
    s32* record = reinterpret_cast<s32*>(seal);
    if (record[0] == 0x41) {
        float amount = *reinterpret_cast<float*>(&record[1]);
        int frames = (int)(amount * 0.5f);
        reinterpret_cast<ftSonicGlowView*>(m_moduleAccesser->m_enumerationStart->m_glowModule)->setGlow(0x117, frames);
    }
}

// Status 0x114 (HYPOTHESIS: the loop de loop / spring exit) puts the fighter at the offset of the fighter data from the
// posture; status 0x116 spawns the article of slot 1 (the Sonic spring) and links it to the fighter.
void ftSonic::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (statusKind == 0x114) {
        Vec2f pos(moduleAccesser->getPostureModule().getPos());
        float* params = *reinterpret_cast<float**>(*reinterpret_cast<u8**>(reinterpret_cast<u8*>(m_commonData) + 0x94) + 0x34);
        int side = moduleAccesser->getPostureModule().getLr() < 0.0f ? -1 : 1;
        pos.m_x += params[1] * (float)side;
        pos.m_y += params[2] * moduleAccesser->getPostureModule().getScale();
        moduleAccesser->getWorkManageModule().setFloat(pos.m_x, 0x21000004);
        moduleAccesser->getWorkManageModule().setFloat(pos.m_y, 0x21000005);
    } else if (statusKind == 0x116) {
        m_moduleAccesser->getGenerateArticleManageModule().removeExist(1, 0);
        soArticle* article = m_moduleAccesser->getGenerateArticleManageModule().generate(1, false, false);
        soLinkModule& link = m_moduleAccesser->getLinkModule();
        if (link.link(6, article->getTaskId())) {
            m_moduleAccesser->getGenerateArticleManageModule().entry(article);
        }
    }
}

void ftSonic::processUpdate() {
    Fighter::processUpdate();
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x116 && m_moduleAccesser->getLinkModule().isModelConstraint() == true) {
        soLinkModule& link = m_moduleAccesser->getLinkModule();
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        Vec3f pos = link.getModelConstraintTargetNodePosition();
        posture.setPos(&pos);
    }
}

void ftSonic::onSetMetal(bool metal) {
    if (metal == true) {
        soEffectModule& effect = m_moduleAccesser->getEffectModule();
        int kind = soValueAccesser::getValueInt(m_moduleAccesser, 0x10000036, 0) + 0x30000a;
        effect.killKind(kind, true, true);
    }
}

void ftSonic::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0x455:
        m_moduleAccesser->getStatusModule().changeStatusRequest(0x126, m_moduleAccesser);
        break;
    case 6:
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

void ftSonic::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    int saved = m_moduleAccesser->getWorkManageModule().getInt(0x10000001);
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
    int statusKind = m_moduleAccesser->getStatusModule().getStatusKind();
    if (statusKind == 0x122 || (statusKind >= 0x11d && statusKind < 0x121) || (statusKind >= 0x124 && statusKind < 0x126)) {
        m_moduleAccesser->getWorkManageModule().setInt(saved, 0x10000001);
    }
    if (kind == 3) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    }
}

void ftSonic::notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (collisionLog != NULL) {
        gfTask* task = gfTaskScheduler::getInstance()->getTask(*reinterpret_cast<u32*>(reinterpret_cast<u8*>(collisionLog) + 0xc));
        StageObject* stageObj = dynamic_cast<StageObject*>(task);
        if (stageObj != NULL) {
            if (stageObj->soGetKind() == 3) {
            } else if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x11e) {
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000011);
            }
        }
    }
}

bool ftSonic::notifyEventCollisionAttackCheck(u32 flags) {
    Fighter::notifyEventCollisionAttackCheck(flags);
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x11e && m_moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000011);
        m_moduleAccesser->getWorkManageModule().subInt(soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc9, 0), 0x20000003);
        m_moduleAccesser->getWorkManageModule().subFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfca, 0), 0x21000005);
        if (m_moduleAccesser->getWorkManageModule().getFloat(0x21000005) < 0.0f) {
            m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x21000005);
        }
        m_moduleAccesser->getWorkManageModule().subFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfca, 0), 0x21000004);
        if (m_moduleAccesser->getWorkManageModule().getFloat(0x21000004) < 0.0f) {
            m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x21000004);
        }
        soKineticEnergyNormal* energy;
        if (m_moduleAccesser->getSituationModule().getKind() == 0) {
            energy = dynamic_cast<soKineticEnergyNormal*>(m_moduleAccesser->getKineticModule().getEnergy(3));
        } else {
            energy = dynamic_cast<soKineticEnergyNormal*>(m_moduleAccesser->getKineticModule().getEnergy(2));
        }
        if (energy == NULL) {
            return false;
        }
        Vec2f speed = energy->getSpeed();
        double wideX = fabs(speed.m_x);
        float absX = wideX;
        if (absX <= soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfca, 0)) {
            speed.m_x = 0.0f;
        } else {
            soModuleAccesser* acc = m_moduleAccesser;
            float sign = speed.m_x < 0.0f ? -1 : 1;
            float step = soValueAccesser::getConstantFloat(acc, 0xfca, 0);
            step *= sign;
            speed.m_x -= step;
        }
        energy->m_speed = speed;
    }
    return false;
}

bool ftSonic::activeArticle(wnSonicGimmickJump* weapon, soModuleAccesser* acc) {
    // MATCH-ONLY: declare the saved ID before the other persistent locals.
    int founderTaskId;
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    Vec2f pos(acc->getPostureModule().getPos());
    bool flag = true;
    if (!acc->getWorkManageModule().isFlag(0x22000010)) {
        flag = false;
    }
    soResourceModule& resourceModule = acc->getResourceModule();
    founderTaskId = acc->getStageObject().m_taskId;
    float lr = acc->getPostureModule().getLr();
    weapon->activate(founderTaskId, resourceModule.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam(), &pos, lr, flag);
    return true;
}

bool ftSonic::activeArticle1(wnSonicSuperSonic* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    Vec2f pos(acc->getPostureModule().getPos());
    soSituationModule& situation = acc->getSituationModule();
    soPostureModule& posture = acc->getPostureModule();
    soResourceModule& resourceModule = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    wnSonicSuperSonicEffectList* effects = static_cast<wnSonicSuperSonicEffectList*>(soValueAccesser::getConstantIndefinite(acc, 0xA802, 0));
    SituationKind kind = static_cast<SituationKind>(situation.getKind());
    float lr = posture.getLr();
    int team = fighter.getOwner()->getTeam();
    int resourceId = dynamic_cast<ftResourceIdAccesserImpl&>(*resourceModule.getResourceIdAccesser()).getFinalResId();
    weapon->activate(founderTaskId, resourceId, team, &pos, lr, kind, effects);
    return true;
}

void ftSonic::notifyEventOnDamage(soDamage* damage, bool unk, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventOnDamage(damage, unk, moduleAccesser);
    if (unk == true) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    }
}

void ftSonic::notifyEventChangeMotion(int motionKind, int prevMotionKind, void* motionData, soModuleAccesser* moduleAccesser) {
    if ((u32)(motionKind - 0x14e) <= 1) {
        reinterpret_cast<ftSonicVisibilityView*>(m_moduleAccesser->m_enumerationStart->m_visibilityModule)->notifyEventChangeMotion(motionKind, prevMotionKind, motionData, moduleAccesser);
    }
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Sonic, copied into
// the descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftSonicArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftSonicArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftSonicArticleKind : wnArticleKindInfo {
    ftSonicArticleKind() {
        kind = 0;
        subKind = Fighter_Sonic;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftSonicArticleBuildInfo : wnArticleBuildInfo {
    ftSonicArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

template <class W> struct ftSonicArticleTraits;
#define FT_SONIC_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftSonicArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftSonicArticleDataTag tag; \
            return static_cast<const DATA*>(g_ftCommonDataAccesser.GETTER(Fighter_Sonic, &tag.m_resourceGroup)); \
        } \
    };

FT_SONIC_TRAITS(wnSonicGimmickJump, wnSonicGimmickJumpData, 0, getWeaponData129)
FT_SONIC_TRAITS(wnSonicSuperSonic, wnSonicSuperSonicData, 1, getWeaponData130)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftSonicArticleTraits<W>::ArticleId, ftSonicArticleBuildInfo(ftSonicArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftSonicArticleTraits<W>::getData()) { }

// ---- the articles of Sonic and their mediator ---------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnSonicGimmickJump>::~wnArticleHolder();
template wnArticleHolder<wnSonicSuperSonic>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftSonicArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftSonicArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftSonicArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftSonicArticleMediator::~ftSonicArticleMediator() { }
#pragma dont_inline off
soArticleMediator::~soArticleMediator() { }

s32 ftSonicArticleMediator::getMediateNum() { return 2; }
void ftSonicArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -----------------------------------------------------------------------------------------------
// Slots: 0 spring, 1 Super Sonic (the rest are unused).

#define FT_SONIC_POOL_GIMMICK_JUMP_BASE static_cast<wnArticlePool<wnSonicGimmickJump, 1, ftSonicSuperSonicPool>&>(m_pools)
#define FT_SONIC_POOL_GIMMICK_JUMP FT_SONIC_POOL_GIMMICK_JUMP_BASE.getSub()
#define FT_SONIC_POOL_SUPER_SONIC_BASE static_cast<wnArticlePool<wnSonicSuperSonic, 1, soInstancePoolRoot>&>(m_pools)
#define FT_SONIC_POOL_SUPER_SONIC FT_SONIC_POOL_SUPER_SONIC_BASE.getSub()

#define FT_SONIC_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

template <class W> struct ftSonicArticleActivator;
template <> struct ftSonicArticleActivator<wnSonicGimmickJump> {
    static bool activate(wnSonicGimmickJump* weapon, soModuleAccesser* acc) { return ftSonic::activeArticle(weapon, acc); }
};
template <> struct ftSonicArticleActivator<wnSonicSuperSonic> {
    static bool activate(wnSonicSuperSonic* weapon, soModuleAccesser* acc) { return ftSonic::activeArticle1(weapon, acc); }
};

void ftSonicArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnSonicGimmickJump* weapon = FT_SONIC_POOL_GIMMICK_JUMP.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSonicSuperSonic* weapon = FT_SONIC_POOL_SUPER_SONIC.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftSonicArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 1; }
    case 1: { wnArticleSlotTag tag; return 1; }
    FT_SONIC_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftSonicActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_SONIC_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftSonicActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftSonicActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftSonicArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_SONIC_COUNT_ACTIVE(FT_SONIC_POOL_GIMMICK_JUMP, 1)
    case 1: FT_SONIC_COUNT_ACTIVE(FT_SONIC_POOL_SUPER_SONIC, 1)
    FT_SONIC_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_SONIC_CAN_GENERATE(POOL, N, ID) \
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

bool ftSonicArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_SONIC_CAN_GENERATE(FT_SONIC_POOL_GIMMICK_JUMP, 1, 0)
    case 1: FT_SONIC_CAN_GENERATE(FT_SONIC_POOL_SUPER_SONIC, 1, 1)
    FT_SONIC_UNUSED_SLOTS(false)
    default: return false;
    }
}

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_SONIC_GENERATE(W, POOL) \
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
        if (ftSonicArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftSonicArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_SONIC_GENERATE(wnSonicGimmickJump, FT_SONIC_POOL_GIMMICK_JUMP)
    case 1: FT_SONIC_GENERATE(wnSonicSuperSonic, FT_SONIC_POOL_SUPER_SONIC)
    FT_SONIC_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Shooting only type-checks the article (the spring is launched); the unused slots accept anything.
bool ftSonicArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; dynamic_cast<wnSonicGimmickJump&>(*article).shoot(); return true; }
    case 1: { wnArticleSlotTag tag; (void)dynamic_cast<wnSonicSuperSonic&>(*article); return true; }
    FT_SONIC_UNUSED_SLOTS(true)
    default: return false;
    }
}

// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
#pragma dont_inline on
ftSonicTransactor* ftSonicTransactor::getInstance() {
    static ftSonicTransactor instance;
    return &instance;
}
#pragma dont_inline off
#pragma pool_data reset

// FIXME: Test code present only to emit the shared builder functions; delete once ftSonic is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftSonicInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftSonicInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;
#include <so/work/so_general_work_simple.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Targets owned by other units of this module (tail-call thunks).
void __dt__17soArticleMediatorFv(u8* p);
void shoot__22ftSonicArticleMediatorFP16soModuleAccesserP9soArticle(u8* p);
void __dt__22ftSonicArticleMediatorFv(u8* p);
void fn_123_18D8(u8* p);
void __dt__7ftSonicFv(u8* p);
bool fn_123_9F48(u8* p, s32 unk1);
void notifyEventLink__7ftSonicFP15soLinkEventArgsP16soModuleAccesserP11StageObjecti(u8* p);
void notifyEventChangeStatus__7ftSonicFiiP12soStatusDataP16soModuleAccesser(u8* p);
void notifyEventChangeSituation__7ftSonicF13SituationKind13SituationKindP16soModuleAccesser(u8* p);
void notifyEventCollisionAttackCheck__7ftSonicFUl(u8* p);
void notifyEventCollisionSearch__7ftSonicFP14soCollisionLogP16soModuleAccesser(u8* p);
void notifyEventOnDamage__7ftSonicFP8soDamagebP16soModuleAccesser(u8* p);

// Fighter (ftEntryEventObserver / Fighter) empty virtuals.
void fn_123_A184() {}
void fn_123_A188() {}
void fn_123_A18C() {}
void fn_123_A190() {}
void fn_123_A194() {}
void fn_123_A1B8() {}
void fn_123_A1BC() {}
void fn_123_A1D0() {}
void fn_123_A1D4() {}
void fn_123_A1D8() {}
void fn_123_A1DC() {}
void fn_123_A1E0() {}
void fn_123_A1E4() {}
void fn_123_A1E8() {}

// Constant returns.
s32 fn_123_A1B0(u8* p) { return 0; }
bool fn_123_A1C0(u8* p) { return false; }
u32 fn_123_A1C8(u8* p) { return 0; }
s32 fn_123_A590(u8* p) { return 0; }
s32 fn_123_9BC4(u8* p) { return 0; }
s32 fn_123_A144() { return 0; }

// Field getters.
s32 fn_123_A198(u8* p) { return *(s32*)(p + 0x110); }
u8 fn_123_A004(u8* p) { return *(u8*)(p + 0x44); }
u8 fn_123_6E08(u8* p) { return *(u8*)(p + 0x4); }
void* fn_123_A1EC(u8* p, u32 index) { return p + index * 8 + 4; }
void* fn_123_A2AC(u8* p) { return p + 0x458; }
void* fn_123_A2B4(u8* p) { return p + 0x3c8; }
void* fn_123_A2BC(u8* p) { return p + 0x8; }
void* fn_123_A2C4(u8* p) { return p + 0x84; }
void* fn_123_A2CC(u8* p) { return p + 0x70; }
void* fn_123_A2D4(u8* p) { return p + 0x5c; }
void* fn_123_A2DC(u8* p) { return p + 0x48; }
void* fn_123_A2E4(u8* p) { return p + 0x34; }
void* fn_123_A2EC(u8* p) { return p + 0x20; }
void* fn_123_A2F4(u8* p) { return p + 0x8; }

// Sub-object and collision-state helpers.
void fn_123_C3CC(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_123_C3D8(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_123_C2F8() {}

// soGeneralWorkSimple accessors (flags at +0x1C, ints at +0x0C, floats at +0x14).
bool fn_123_A3D8(const soGeneralWorkSimple* p, u32 flag, u32 index) { return p->soGeneralWorkSimple::isFlag(flag, index); }
void fn_123_A3F4(soGeneralWorkSimple* p, u32 flag, u32 index) { p->soGeneralWorkSimple::offFlag(flag, index); }
void fn_123_A420(soGeneralWorkSimple* p, u32 flag, u32 index) { p->soGeneralWorkSimple::onFlag(flag, index); }
u32 fn_123_A35C(soGeneralWorkSimple* p) { return p->soGeneralWorkSimple::getFlagWorkSize(); }
u32 fn_123_A438(soGeneralWorkSimple* p) { return p->soGeneralWorkSimple::getFloatWorkSize(); }
u32 fn_123_A4D0(soGeneralWorkSimple* p) { return p->soGeneralWorkSimple::getIntWorkSize(); }
void fn_123_A468(soGeneralWorkSimple* p, float value, u32 index) { p->soGeneralWorkSimple::mulFloatWork(value, index); }
void fn_123_A480(soGeneralWorkSimple* p, float value, u32 index) { p->soGeneralWorkSimple::subFloatWork(value, index); }
void fn_123_A498(soGeneralWorkSimple* p, float value, u32 index) { p->soGeneralWorkSimple::addFloatWork(value, index); }
void fn_123_A4B0(soGeneralWorkSimple* p, float value, u32 index) { p->soGeneralWorkSimple::setFloatWork(value, index); }
float fn_123_A4C0(const soGeneralWorkSimple* p, u32 index) { return p->soGeneralWorkSimple::getFloatWork(index); }
void fn_123_A4D8(soGeneralWorkSimple* p, u32 index) { p->soGeneralWorkSimple::decIntWork(index); }
void fn_123_A4F0(soGeneralWorkSimple* p, u32 index) { p->soGeneralWorkSimple::incIntWork(index); }
void fn_123_A528(soGeneralWorkSimple* p, s32 value, u32 index) { p->soGeneralWorkSimple::mulIntWork(value, index); }
void fn_123_A540(soGeneralWorkSimple* p, s32 value, u32 index) { p->soGeneralWorkSimple::subIntWork(value, index); }
void fn_123_A558(soGeneralWorkSimple* p, s32 value, u32 index) { p->soGeneralWorkSimple::addIntWork(value, index); }
void fn_123_A570(soGeneralWorkSimple* p, s32 value, u32 index) { p->soGeneralWorkSimple::setIntWork(value, index); }
s32 fn_123_A580(const soGeneralWorkSimple* p, u32 index) { return p->soGeneralWorkSimple::getIntWork(index); }

// Fighter notifyEvent thunks: subtract the sub-object offset, then tail-call the Fighter method.
void fn_123_EF30(u8* p) { return __dt__17soArticleMediatorFv(p - 0x4); }
void fn_123_EF38(u8* p) { return shoot__22ftSonicArticleMediatorFP16soModuleAccesserP9soArticle(p - 0x4); }
void fn_123_EF40(u8* p) { return __dt__22ftSonicArticleMediatorFv(p - 0x4); }
void fn_123_EF48(u8* p) { return fn_123_18D8(p - 0x40); }
void fn_123_EF50(u8* p, soLinkEventArgs* a0, soModuleAccesser* a1, StageObject* a2, int a3) { return ((Fighter*)(p - 0x54))->Fighter::notifyEventLink(a0, a1, a2, a3); }
void fn_123_EF58(u8* p, int a0, int a1, soStatusData* a2, soModuleAccesser* a3) { return ((Fighter*)(p - 0x64))->Fighter::notifyEventChangeStatus(a0, a1, a2, a3); }
void fn_123_EF60(u8* p, SituationKind a0, SituationKind a1, soModuleAccesser* a2) { return ((Fighter*)(p - 0x70))->Fighter::notifyEventChangeSituation(a0, a1, a2); }
bool fn_123_EF68(u8* p, u32 a0) { return ((Fighter*)(p - 0x7C))->Fighter::notifyEventCollisionAttackCheck(a0); }
void fn_123_EF70(u8* p, soCollisionLog* a0, soModuleAccesser* a1) { return ((Fighter*)(p - 0xB8))->Fighter::notifyEventCollisionSearch(a0, a1); }
void fn_123_EF78(u8* p, soDamage* a0, bool a1, soModuleAccesser* a2) { return ((Fighter*)(p - 0xE8))->Fighter::notifyEventOnDamage(a0, a1, a2); }
void fn_123_EF80(u8* p) { return __dt__7ftSonicFv(p - 0x40); }
bool fn_123_EF88(u8* p, acAnimCmd* a0, soModuleAccesser* a1, int a2) { return ((Fighter*)(p - 0x48))->Fighter::notifyEventAnimCmd(a0, a1, a2); }
bool fn_123_EF90(u8* p, s32 unk1) { return fn_123_9F48(p - 0x48, unk1); }
void fn_123_EF98(u8* p) { return notifyEventLink__7ftSonicFP15soLinkEventArgsP16soModuleAccesserP11StageObjecti(p - 0x54); }
void fn_123_EFA0(u8* p) { return notifyEventChangeStatus__7ftSonicFiiP12soStatusDataP16soModuleAccesser(p - 0x64); }
void fn_123_EFA8(u8* p) { return notifyEventChangeSituation__7ftSonicF13SituationKind13SituationKindP16soModuleAccesser(p - 0x70); }
void fn_123_EFB0(u8* p) { return notifyEventCollisionAttackCheck__7ftSonicFUl(p - 0x7C); }
void fn_123_EFB8(u8* p, float a0, soCollisionLog* a1, soModuleAccesser* a2) { return ((Fighter*)(p - 0x7C))->Fighter::notifyEventCollisionAttack(a0, a1, a2); }
void fn_123_EFC0(u8* p, int a0, soModuleAccesser* a1) { return ((Fighter*)(p - 0x88))->Fighter::notifyEventChangeCollisionHit(a0, a1); }
bool fn_123_EFC8(u8* p) { return ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
void fn_123_EFD0(u8* p, soCollisionSearchModule* a0, soCollisionLog* a1, u32 a2, soModuleAccesser* a3) { return ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShieldSearch(a0, a1, a2, a3); }
void fn_123_EFD8(u8* p, soCollisionAttackModule* a0, float a1, soCollisionLog* a2, int a3, float a4, float a5, soModuleAccesser* a6) { return ((Fighter*)(p - 0x94))->Fighter::notifyEventCollisionShield(a0, a1, a2, a3, a4, a5, a6); }
bool fn_123_EFE0(u8* p) { return ((Fighter*)(p - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
void fn_123_EFE8(u8* p, soCollisionSearchModule* a0, soCollisionLog* a1, u32 a2, soModuleAccesser* a3) { return ((Fighter*)(p - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(a0, a1, a2, a3); }
void fn_123_EFF0(u8* p, soCollisionAttackModule* a0, soCollisionLog* a1, u32 a2, soModuleAccesser* a3, float a4, float a5, float a6) { return ((Fighter*)(p - 0xA0))->Fighter::notifyEventCollisionReflector(a0, a1, a2, a3, a4, a5, a6); }
bool fn_123_EFF8(u8* p) { return ((Fighter*)(p - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
void fn_123_F000(u8* p, soCollisionAttackModule* a0, soCollisionLog* a1, u32 a2, soModuleAccesser* a3, float a4, float a5, float a6) { return ((Fighter*)(p - 0xAC))->Fighter::notifyEventCollisionAbsorber(a0, a1, a2, a3, a4, a5, a6); }
bool fn_123_F008(u8* p) { return ((Fighter*)(p - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_123_F010(u8* p) { return notifyEventCollisionSearch__7ftSonicFP14soCollisionLogP16soModuleAccesser(p - 0xB8); }
bool fn_123_F018(u8* p, soModuleAccesser* a0, int a1, int a2, int a3) { return ((Fighter*)(p - 0xC4))->Fighter::notifyEventCaptureStatus(a0, a1, a2, a3); }
void fn_123_F020(u8* p, BaseItem* a0, int a1, bool a2, u8 a3) { return ((Fighter*)(p - 0xD0))->Fighter::notifyVisibilityItem(a0, a1, a2, a3); }
void fn_123_F028(u8* p, BaseItem* a0, u32 a1, bool a2) { return ((Fighter*)(p - 0xD0))->Fighter::notifyEjectAttachItem(a0, a1, a2); }
void fn_123_F030(u8* p, BaseItem* a0, u32 a1, bool a2) { return ((Fighter*)(p - 0xD0))->Fighter::notifyEjectItem(a0, a1, a2); }
void fn_123_F038(u8* p, BaseItem* a0) { return ((Fighter*)(p - 0xD0))->Fighter::notifyShootBulletItem(a0); }
void fn_123_F040(u8* p, BaseItem* a0) { return ((Fighter*)(p - 0xD0))->Fighter::notifyDropItem(a0); }
void fn_123_F048(u8* p, BaseItem* a0, u32 a1, int* a2) { return ((Fighter*)(p - 0xD0))->Fighter::notifyThrowItem(a0, a1, a2); }
bool fn_123_F050(u8* p, BaseItem* a0, u32 a1, int* a2) { return ((Fighter*)(p - 0xD0))->Fighter::notifyUseItem(a0, a1, a2); }
void fn_123_F058(u8* p, BaseItem* a0, u32 a1, u32 a2, bool a3, bool a4) { return ((Fighter*)(p - 0xD0))->Fighter::notifyAttachItem(a0, a1, a2, a3, a4); }
void fn_123_F060(u8* p, itParam::SizeKind a0, BaseItem* a1, u32 a2, u32 a3, bool a4) { return ((Fighter*)(p - 0xD0))->Fighter::notifyHaveItem(a0, a1, a2, a3, a4); }
bool fn_123_F068(u8* p, BaseItem* a0, bool* a1) { return ((Fighter*)(p - 0xD0))->Fighter::notifyHaveItemPreCheck(a0, a1); }
void notifyEventChangeMotion__7ftSonicFiiPvP16soModuleAccesser(u8* p, s32 kind);
void fn_123_F070(u8* p, s32 kind) { return notifyEventChangeMotion__7ftSonicFiiPvP16soModuleAccesser(p - 0xDC, kind); }
void fn_123_F078(u8* p, soDamage* a0, soModuleAccesser* a1) { return ((Fighter*)(p - 0xE8))->Fighter::notifyEventAddDamage(a0, a1); }
void fn_123_F080(u8* p) { return notifyEventOnDamage__7ftSonicFP8soDamagebP16soModuleAccesser(p - 0xE8); }
void fn_123_F088(u8* p, float a0, int a1) { return ((Fighter*)(p - 0xF4))->Fighter::notifyEventPikminFinalAttack(a0, a1); }
void fn_123_F090(u8* p) { return ((Fighter*)(p - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_123_F098(u8* p) { return ((Fighter*)(p - 0xF4))->Fighter::notifyEventBeat(); }
void fn_123_F0A0(u8* p, float a0) { return ((Fighter*)(p - 0xF4))->Fighter::notifyEventSetDamage(a0); }
void fn_123_F0A8(u8* p, float a0, float a1, soModuleAccesser* a2) { return ((Fighter*)(p - 0x100))->Fighter::notifyEventTurn(a0, a1, a2); }

// Empty notifyEvent observers of ftEntryEventObserver.
void fn_123_A108() {}
void fn_123_A10C() {}
void fn_123_A110() {}
void fn_123_A114() {}
void fn_123_A118() {}
void fn_123_A11C() {}
void fn_123_A120() {}
void fn_123_A124() {}
void fn_123_A128() {}
void fn_123_A12C() {}
void fn_123_A130() {}
void fn_123_A134() {}
void fn_123_A138() {}
void fn_123_A13C() {}
void fn_123_A140() {}
void fn_123_A14C() {}
void fn_123_A150() {}
void fn_123_A154() {}
void fn_123_A158() {}
void fn_123_A000() {}
void fn_123_A00C() {}
void fn_123_A04C() {}
void fn_123_A050() {}

// Sub-object getters and instance-pool lookups (fixed offsets into the unit's object).
void* fn_123_B558(u8* p) { return p + 0xD028; }
void* fn_123_B578(u8* p) { return p + 0xD064; }
void* fn_123_B584(u8* p) { return p + 0xD51C; }
void* fn_123_D08C(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_D488(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_D884(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_DC74(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_E078(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_E47C(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_E880(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_EC84(u8* p, s32 index) { if (index == 0) return p + 0xC; return 0; }
void* fn_123_D134(u8* p) { return 0; }
void* fn_123_D530(u8* p) { return 0; }
void* fn_123_D92C(u8* p) { return 0; }
void* fn_123_DD1C(u8* p) { return 0; }
void* fn_123_E120(u8* p) { return 0; }
void* fn_123_E524(u8* p) { return 0; }
void* fn_123_E928(u8* p) { return 0; }
void* fn_123_ED2C(u8* p) { return 0; }

// Kinetic transactor/mediator thunks (tail calls into ftKineticTransactor members).
extern "C" void addSpeed__19ftKineticTransactorFPvPvP16soModuleAccesser(void* a, void* b, void* c);
extern "C" void addSpeedOutside__19ftKineticTransactorFiPvPvP16soModuleAccesser(void* a, s32 b, void* c, void* d);
extern "C" void notifyEventChangeStatus__19ftKineticTransactorFPvPvPvPv(void* a, void* b, void* c, void* d);
void fn_123_CCEC(u8* p, void* a, void* b) { return addSpeed__19ftKineticTransactorFPvPvP16soModuleAccesser(a, p + 4, b); }
void fn_123_CCFC(u8* p, void* a, s32 b, void* c) { return addSpeedOutside__19ftKineticTransactorFiPvPvP16soModuleAccesser(a, b, p + 4, c); }
void fn_123_CD10(u8* p, void* a, void* b, void* c, void* d) { return notifyEventChangeStatus__19ftKineticTransactorFPvPvPvPv(a, b, c, d); }
s32 fn_123_CD24() { return 8; }
void fn_123_CCE8() {}

// Fixed-offset virtual slot view (slot k at vtable offset 8 + 4k); only used to emit virtual tail calls.
struct ftSonicVirtualSlots {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual s32 slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6(s32 kind);
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

void fn_123_B564(u8* p) { return ((ftSonicVirtualSlots*)(p + 0xD028))->slot2(); }
bool fn_123_B590(u8* p) { return ((ftSonicVirtualSlots*)p)->slot3() == 0; }
void fn_123_9BC0() {}
void fn_123_9E88() {}
void fn_123_9BBC() {}

// soGeneralWorkSimple divide helpers (zero-divisor guard is in the header).
void fn_123_A440(soGeneralWorkSimple* p, float value, u32 index) { p->soGeneralWorkSimple::divFloatWork(value, index); }
void fn_123_A508(soGeneralWorkSimple* p, s32 value, u32 index) { p->soGeneralWorkSimple::divIntWork(value, index); }

// Two-word value read from the object (returned in r3:r4).
struct ftSonicWordPair {
    u32 first;
    u32 second;
};
ftSonicWordPair fn_123_9564(u8* p) { return *(ftSonicWordPair*)(p + 0x8); }

extern void* g_soValueVariation;
void* fn_123_A858() { return g_soValueVariation; }

}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_123_1D70(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_123_1E0C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_123_396C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_123_39AC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_123_3C98(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_123_4028(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_123_4424(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_123_470C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_123_474C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_123_50D4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_123_5114(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"
