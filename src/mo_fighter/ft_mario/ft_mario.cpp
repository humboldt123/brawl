// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_MARIO_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_reflector_group.h>
#include <ft/ft_class_info_impl.h>
#include <ft/mario/ft_mario.h>
#include <ft/mario/ft_mario_extend_param_accesser.h>
#include <ft/mario/ft_mario_transactor.h>
#include <ft/ft_owner.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/article/so_article_mediator.h>
#include <so/article/so_generate_article_manage_module.h>

#define FT_BC ftMarioBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
// Parameter-table variation getter (soValueAccesser::getValueVariation) under its map address.
extern "C" int fn_91_ABE8() { return g_soValueVariation; }

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_91_bss_1E4;
extern soStatusUniqProcess lbl_91_bss_204;
extern soStatusUniqProcess lbl_27_bss_734C;
extern soStatusUniqProcess lbl_91_bss_1F4;
}

ftMarioExtendParamAccesser g_ftMarioExtendParamAccesser;
ftMarioDExtendParamAccesser g_ftMarioDExtendParamAccesser;

ftClassInfoImpl<Fighter_Mario, ftMario> g_ftClassInfoMario;
ftClassInfoImpl<Fighter_MarioD, ftMarioD> g_ftClassInfoMarioD;

ftMario::ftMario(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftMarioBuildConfig>(entryId,
                                         Fighter_Mario,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Mario)),
    unk2DBD4(0) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[7] = {0};
    processes[0] = &lbl_91_bss_1E4;
    processes[1] = &lbl_91_bss_204;
    processes[2] = &lbl_27_bss_734C;
    processes[6] = &lbl_91_bss_1F4;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 7);
    // The cape: build the reflector group from the common data record (+0x8c) and install it as group 2.
    void* reflectorRecord = *reinterpret_cast<void**>(reinterpret_cast<u8*>(m_data) + 0x8c);
    u8 shape[0x20];
    ftReflectorGroupDraft group;
    FT_REFLECTOR_GROUP_CLEAR(group);
    fn_27_195B34(reflectorRecord, shape, &group);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&group), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
}

// ---- the fighter overrides ------------------------------------------------------------------------------------------
// Work 0x10000040 / 0x10000041 hold the charge of the Pump and the frames the Pump article is kept after its owner lost it.
// HYPOTHESIS: meaning of the works.
void ftMario::onStart(int startKind) {
    // HYPOTHESIS: the flag 0x1200003d marks a pending fighter change, cleared on every start.
    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
    m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000041);
    Fighter::onStart(startKind);
    // HYPOTHESIS: the common effect 0x1a is the Pump spray.
    m_moduleAccesser->getEffectModule().removeCommon(0x1a);
}

void ftMario::onDeadEnd() {
    m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000041);
    m_moduleAccesser->getGenerateArticleManageModule().removeExist(2, 0);
    Fighter::onDeadEnd();
}

// While the Pump article exists its countdown runs down; at the last frame the Pump changes to its end status.
void ftMario::processUpdate() {
    Fighter::processUpdate();
    if (m_moduleAccesser->getGenerateArticleManageModule().isExist(2) == true) {
        int count = m_moduleAccesser->getWorkManageModule().getInt(0x10000041);
        if (count > 0) {
            if (count - 1 == 0) {
                m_moduleAccesser->getGenerateArticleManageModule().changeStatus(2, 2, 0);
            }
            m_moduleAccesser->getWorkManageModule().setInt(count - 1, 0x10000041);
        }
    }
    unk2DBD4 = 0;
}

void ftMario::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
    if (kind != 2) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    }
}

void ftMario::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0x3c:
        if (stageObj->soGetKind() == 2) {
            switch (stageObj->soGetSubKind()) {
            case 0x4c:
                endFinal(false, true, false);
                break;
            }
        }
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

// The reflector (the cape) keeps a flag for the frame: the hit was too weak for it.
void ftMario::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) {
    if ((s32)groupIndex == 2) {
        if (power >= m_moduleAccesser->getCollisionReflectorModule().getAttackLimit(groupIndex)) {
            unk2DBD4 = 1;
            return;
        }
    }
    Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY);
}

bool ftMario::notifyEventCollisionReflectorCheck() {
    if (unk2DBD4 == 1) {
        m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    }
    return Fighter::notifyEventCollisionReflectorCheck();
}

// The animation command ')' shows the visibility record 1 (HYPOTHESIS: the Pump on the back).
bool ftMario::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* acc, s32 index) {
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

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Mario, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftMarioArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftMarioArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftMarioArticleKind : wnArticleKindInfo {
    ftMarioArticleKind() {
        kind = 0;
        subKind = Fighter_Mario;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftMarioArticleBuildInfo : wnArticleBuildInfo {
    ftMarioArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_147C9C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147D34(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147D4C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147D7C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftMarioArticleTraits;
#define FT_MARIO_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftMarioArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftMarioArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Mario, &tag.m_resourceGroup)); \
        } \
    };

FT_MARIO_TRAITS(wnMarioMantle, wnMarioMantleData, 0, fn_27_147D34)
FT_MARIO_TRAITS(wnMarioFireball, wnMarioFireballData, 1, fn_27_147C9C)
FT_MARIO_TRAITS(wnMarioPump, wnMarioPumpData, 2, fn_27_147D4C)
FT_MARIO_TRAITS(wnMarioHugeFlame, wnMarioHugeFlameData, 3, fn_27_147D7C)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftMarioArticleTraits<W>::ArticleId, ftMarioArticleBuildInfo(ftMarioArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftMarioArticleTraits<W>::getData()) { }

// ---- the articles of Mario and their mediator -----------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnMarioMantle>::~wnArticleHolder();
template wnArticleHolder<wnMarioFireball>::~wnArticleHolder();
template wnArticleHolder<wnMarioPump>::~wnArticleHolder();
template wnArticleHolder<wnMarioHugeFlame>::~wnArticleHolder();
template wnArticleHolder<wnMarioFireball>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftMarioArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftMarioArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftMarioArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftMarioArticleMediator::~ftMarioArticleMediator() { }
#pragma dont_inline off

s32 ftMarioArticleMediator::getMediateNum() { return 4; }
void ftMarioArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 cape, 1 fireball, 2 Pump, 3 Final Smash flame (the rest are unused).

#define FT_MARIO_POOL_MANTLE_BASE static_cast<wnArticlePool<wnMarioMantle, 1, ftMarioFireballPool>&>(m_pools)
#define FT_MARIO_POOL_MANTLE FT_MARIO_POOL_MANTLE_BASE.getSub()
#define FT_MARIO_POOL_FIREBALL_BASE static_cast<wnArticlePool<wnMarioFireball, 5, ftMarioPumpPool>&>(m_pools)
#define FT_MARIO_POOL_FIREBALL FT_MARIO_POOL_FIREBALL_BASE.getSub()
#define FT_MARIO_POOL_PUMP_BASE static_cast<wnArticlePool<wnMarioPump, 1, ftMarioHugeFlamePool>&>(m_pools)
#define FT_MARIO_POOL_PUMP FT_MARIO_POOL_PUMP_BASE.getSub()
#define FT_MARIO_POOL_HUGE_FLAME_BASE static_cast<wnArticlePool<wnMarioHugeFlame, 1, soInstancePoolRoot>&>(m_pools)
#define FT_MARIO_POOL_HUGE_FLAME FT_MARIO_POOL_HUGE_FLAME_BASE.getSub()

#define FT_MARIO_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

// The fireball starts through the transactor with the effect resource of the fighter (the kind 0x1b selects the variant).
// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
bool ftMario::activeArticle(wnMarioFireball* weapon, soModuleAccesser* acc) {
    static ftMarioTransactor instance;
    ftMarioTransactor* transactor = &instance;
    u32 etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
    return transactor->activeArticle1(weapon, acc, etcResId, 0x1b);
}
#pragma pool_data reset

// The cape starts at the stage object of the fighter with its team and the situation.
bool ftMario::activeArticle1(wnMarioMantle* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam(), acc->getSituationModule().getKind());
    return true;
}

// The Pump starts with the model resource of the fighter.
bool ftMario::activeArticle2(wnMarioPump* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    int founderTaskId = acc->getStageObject().m_taskId;
    soResourceModule& resources = acc->getResourceModule();
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getMdlResId(), fighter.getOwner()->getTeam(), resources.getResourceIdAccesser()->getEtcResId());
    return true;
}

// The Final Smash flame starts at the stage object of the fighter with its team and the facing.
bool ftMario::activeArticle3(wnMarioHugeFlame* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(), fighter.getOwner()->getTeam(), acc->getPostureModule().getLr());
    return true;
}

void ftMarioArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnMarioMantle* weapon = FT_MARIO_POOL_MANTLE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 5; ++i) {
        wnMarioFireball* weapon = FT_MARIO_POOL_FIREBALL.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnMarioPump* weapon = FT_MARIO_POOL_PUMP.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnMarioHugeFlame* weapon = FT_MARIO_POOL_HUGE_FLAME.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftMarioArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 1; }
    case 1: { wnArticleSlotTag tag; return 5; }
    case 2: { wnArticleSlotTag tag; return 1; }
    case 3: { wnArticleSlotTag tag; return 1; }
    FT_MARIO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftMarioActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_MARIO_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftMarioActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftMarioActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftMarioArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_MARIO_COUNT_ACTIVE(FT_MARIO_POOL_MANTLE, 1)
    case 1: FT_MARIO_COUNT_ACTIVE(FT_MARIO_POOL_FIREBALL, 5)
    case 2: FT_MARIO_COUNT_ACTIVE(FT_MARIO_POOL_PUMP, 1)
    case 3: FT_MARIO_COUNT_ACTIVE(FT_MARIO_POOL_HUGE_FLAME, 1)
    FT_MARIO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_MARIO_CAN_GENERATE(POOL, N, ID) \
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

bool ftMarioArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_MARIO_CAN_GENERATE(FT_MARIO_POOL_MANTLE, 1, 0)
    case 1: FT_MARIO_CAN_GENERATE(FT_MARIO_POOL_FIREBALL, 5, 1)
    case 2: FT_MARIO_CAN_GENERATE(FT_MARIO_POOL_PUMP, 1, 2)
    case 3: FT_MARIO_CAN_GENERATE(FT_MARIO_POOL_HUGE_FLAME, 1, 3)
    FT_MARIO_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftMarioArticleActivator;
template <> struct ftMarioArticleActivator<wnMarioMantle> {
    static bool activate(wnMarioMantle* weapon, soModuleAccesser* acc) { return ftMario::activeArticle1(weapon, acc); }
};
template <> struct ftMarioArticleActivator<wnMarioFireball> {
    static bool activate(wnMarioFireball* weapon, soModuleAccesser* acc) { return ftMario::activeArticle(weapon, acc); }
};
template <> struct ftMarioArticleActivator<wnMarioPump> {
    static bool activate(wnMarioPump* weapon, soModuleAccesser* acc) { return ftMario::activeArticle2(weapon, acc); }
};
template <> struct ftMarioArticleActivator<wnMarioHugeFlame> {
    static bool activate(wnMarioHugeFlame* weapon, soModuleAccesser* acc) { return ftMario::activeArticle3(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_MARIO_GENERATE(W, POOL) \
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
        if (ftMarioArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftMarioArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_MARIO_GENERATE(wnMarioMantle, FT_MARIO_POOL_MANTLE)
    case 1: FT_MARIO_GENERATE(wnMarioFireball, FT_MARIO_POOL_FIREBALL)
    case 2: FT_MARIO_GENERATE(wnMarioPump, FT_MARIO_POOL_PUMP)
    case 3: FT_MARIO_GENERATE(wnMarioHugeFlame, FT_MARIO_POOL_HUGE_FLAME)
    FT_MARIO_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Shooting only type-checks the articles, except the Final Smash flame, which flies off from the position of the fighter.
bool ftMarioArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; (void)dynamic_cast<wnMarioMantle&>(*article); return true; }
    case 1: { wnArticleSlotTag tag; (void)dynamic_cast<wnMarioFireball&>(*article); return true; }
    case 2: { wnArticleSlotTag tag; (void)dynamic_cast<wnMarioPump&>(*article); return true; }
    case 3: {
        wnArticleSlotTag tag;
        wnMarioHugeFlame& flame = dynamic_cast<wnMarioHugeFlame&>(*article);
        Vec3f pos = acc->getPostureModule().getPos();
        flame.toFly(&pos);
        return true;
    }
    FT_MARIO_UNUSED_SLOTS(true)
    default: return false;
    }
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftMario is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftMarioInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftMarioInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Empty virtual overrides of this module (ftEntryEventObserver / Fighter no-ops), under their map addresses.
extern "C" {
void fn_91_A4A4() {}
void fn_91_A4A8() {}
void fn_91_A4AC() {}
void fn_91_A4B0() {}
void fn_91_A4B4() {}
void fn_91_A4B8() {}
void fn_91_A4BC() {}
void fn_91_A4C0() {}
void fn_91_A4C4() {}
void fn_91_A4C8() {}
void fn_91_A4CC() {}
void fn_91_A4D0() {}
void fn_91_A4DC() {}
void fn_91_A4E0() {}
void fn_91_A4E4() {}
void fn_91_A4E8() {}
void fn_91_A514() {}
void fn_91_A518() {}
void fn_91_A51C() {}
void fn_91_A520() {}
void fn_91_A524() {}
void fn_91_A548() {}
void fn_91_A54C() {}
void fn_91_A550() {}
void fn_91_A554() {}
void fn_91_A568() {}
void fn_91_A56C() {}
void fn_91_A570() {}
void fn_91_A574() {}
void fn_91_A578() {}

}

// Field accessors of this module under their map addresses (offsets read from the target code).
extern "C" {

u8 fn_91_22B4(u8* p) { return *(u8*)(p + 0x4); }
u8 fn_91_A0AC(u8* p) { return *(u8*)(p + 0x44); }
int fn_91_A528(u8* p) { return *(int*)(p + 0x110); }
bool fn_91_9D0C(void* self, s8 kind) { return kind == 0xc; }

}

// Weak inline methods of library classes that the REL emits as stand-alone functions. Their names exist in
// sora_melee, so the REL copies cannot be renamed: each shim calls the inline method (as in ft_ike.cpp).
// Virtual-node matrix pool getters (map order: extend, common, hit).
extern "C" {
void* fn_91_A63C(ftVirtualNodeMatrixPoolImpl* p) { return p->ftVirtualNodeMatrixPoolImpl::getExtendMatrix(); }
void* fn_91_A644(ftVirtualNodeMatrixPoolImpl* p) { return p->ftVirtualNodeMatrixPoolImpl::getCommonMatrix(); }
void* fn_91_A64C(ftVirtualNodeMatrixPoolImpl* p) { return p->ftVirtualNodeMatrixPoolImpl::getHitMatrix(); }
}

// soGeneralWorkSimple work-area methods, in map order (flags, floats, ints).
extern "C" {
u32 fn_91_A6EC(soGeneralWorkSimple* p) { return p->soGeneralWorkSimple::getFlagWorkSize(); }
bool fn_91_A768(const soGeneralWorkSimple* p, u32 mask, u32 idx) { return p->soGeneralWorkSimple::isFlag(mask, idx); }
void fn_91_A784(soGeneralWorkSimple* p, u32 mask, u32 idx) { p->soGeneralWorkSimple::offFlag(mask, idx); }
void fn_91_A79C(soGeneralWorkSimple* p, u32 idx) { p->soGeneralWorkSimple::clearFlag(idx); }
void fn_91_A7B0(soGeneralWorkSimple* p, u32 mask, u32 idx) { p->soGeneralWorkSimple::onFlag(mask, idx); }
u32 fn_91_A7C8(soGeneralWorkSimple* p) { return p->soGeneralWorkSimple::getFloatWorkSize(); }
// MATCH-ONLY: divFloatWork is too large for MWCC to inline here (it emits a tail call to a weak copy that the target
// does not have), so its body is written out.
void fn_91_A7D0(soGeneralWorkSimple* p, float v, u32 idx) {
    if (v == 0.0f) {
        return;
    }
    p->m_floatWorks[idx] /= v;
}
void fn_91_A7F8(soGeneralWorkSimple* p, float v, u32 idx) { p->soGeneralWorkSimple::mulFloatWork(v, idx); }
void fn_91_A810(soGeneralWorkSimple* p, float v, u32 idx) { p->soGeneralWorkSimple::subFloatWork(v, idx); }
void fn_91_A828(soGeneralWorkSimple* p, float v, u32 idx) { p->soGeneralWorkSimple::addFloatWork(v, idx); }
void fn_91_A840(soGeneralWorkSimple* p, float v, u32 idx) { p->soGeneralWorkSimple::setFloatWork(v, idx); }
float fn_91_A850(const soGeneralWorkSimple* p, u32 idx) { return p->soGeneralWorkSimple::getFloatWork(idx); }
void fn_91_A868(soGeneralWorkSimple* p, u32 idx) { p->soGeneralWorkSimple::decIntWork(idx); }
void fn_91_A880(soGeneralWorkSimple* p, u32 idx) { p->soGeneralWorkSimple::incIntWork(idx); }
void fn_91_A898(soGeneralWorkSimple* p, s32 v, u32 idx) { p->soGeneralWorkSimple::divIntWork(v, idx); }
void fn_91_A8B8(soGeneralWorkSimple* p, s32 v, u32 idx) { p->soGeneralWorkSimple::mulIntWork(v, idx); }
void fn_91_A8D0(soGeneralWorkSimple* p, s32 v, u32 idx) { p->soGeneralWorkSimple::subIntWork(v, idx); }
void fn_91_A8E8(soGeneralWorkSimple* p, s32 v, u32 idx) { p->soGeneralWorkSimple::addIntWork(v, idx); }
void fn_91_A900(soGeneralWorkSimple* p, s32 v, u32 idx) { p->soGeneralWorkSimple::setIntWork(v, idx); }
}

// Kinetic energy and builder members emitted in this unit.
extern "C" {
void fn_91_CD00(soKineticEnergyNormal* p) { p->soKineticEnergyNormal::offConsiderGroundFriction(); }
void fn_91_CD0C(soKineticEnergyNormal* p) { p->soKineticEnergyNormal::onConsiderGroundFriction(); }
// ftFighterBuilder<ftMarioBuildConfig> virtual getters (map order: cancel module, hit-test query, matrix pool, gimmick pool).
void* fn_91_BD50(ftFighterBuilder<ftMarioBuildConfig>* p) { return p->ftFighterBuilder<ftMarioBuildConfig>::getCancelModule(); }
void* fn_91_BD70(ftFighterBuilder<ftMarioBuildConfig>* p) { return p->ftFighterBuilder<ftMarioBuildConfig>::getVirtualNodeMatrixPool(); }
void* fn_91_BD7C(ftFighterBuilder<ftMarioBuildConfig>* p) { return p->ftFighterBuilder<ftMarioBuildConfig>::getStatusGimmickUniqProcessPool(); }
void fn_91_CC18() {}
void fn_91_D328() {}
void fn_91_D3AC() {}
void fn_91_D494() {}
void fn_91_D57C() {}
void fn_91_D664() {}
void fn_91_D74C() {}
void fn_91_D834() {}
void fn_91_D91C() {}
void fn_91_DA04() {}

}

// Zero-fill of a three-float member (Vec3f-sized).
extern "C" {

void fn_91_CD18(float* p) {
    p[0] = 0.0f;
    p[1] = 0.0f;
    p[2] = 0.0f;
}

}

// Fighter-builder cancel query, forwarding to the cancel module through its virtual interface.
extern "C" bool fn_91_BD5C(ftFighterBuilder<ftMarioBuildConfig>* p) { return p->ftFighterBuilder<ftMarioBuildConfig>::isEnableCancel(); }

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
u8* fn_91_D580(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_91_D668(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_91_D750(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"
