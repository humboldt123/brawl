#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PIT_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#define WN_WEAPON_BUILDER // the fighter owns the complete bow and arrow types (their weak teardowns are emitted here)
#define WN_PIT_BOW_FULL_BUILDER
#define WN_PIT_IKARUS_FULL_BUILDER // the fighter owns the complete Ikarus type (its holder teardown is emitted here)
#define WN_KINETIC_GENERIC_IN_FIGHTER_TU
#define WN_KINETIC_GENERIC_BUILDER
#define WN_KINETIC_GRAVITY_INLINE_DTOR // BYTES: the Ikarus holder teardown expands the gravity energy teardown
#include <ft/builder/ft_dol_array_list.h>
// The arrow and Ikarus status vectors and the Ikarus hit-part vector: their teardowns are sora_melee's copies.
FT_DOL_ARRAY_VECTOR(soStatusUniqProcess*, 3);
FT_DOL_ARRAY_VECTOR(soStatusUniqProcess*, 5);
FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 1);
FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 11); // the hit module builder calls sora_melee's constructor out of line
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 1);
FT_DOL_ARRAY_VECTOR(soCollisionGroup, 4); // the reflector builder calls sora_melee's constructor (fn_27_199694) out of line
// BYTES: StageObject's weapon-builder inlines are defined here and after the globals instead of in the class
// body: the native .bss places getInput's local static after this unit's globals, so its body was parsed after them.
#undef WN_WEAPON_BUILDER
#define SO_INPUT_NULL_CTOR
#include <so/stageobject.h>
#include <ip/ip_null.h>
#define WN_WEAPON_BUILDER
inline bool StageObject::checkTransitionStatus(u32) { return true; }
inline bool StageObject::isTreadPassive() { return false; }
#include <ft/ft_reflector_group.h>
#include <ft/ft_class_info_impl.h>
#include <ft/pit/ft_pit.h>
#include <ft/pit/ft_pit_extend_param_accesser.h>
#include <ft/pit/ft_pit_transactor.h>
#include <ft/ft_owner.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/article/so_article_mediator.h>
#include <so/so_slow.h>
#include <ft/ft_util.h>

#define FT_BC ftPitBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// The Ikarus holder teardown expands the observer teardown here and calls soEventObserver<soDamageEventObserver>'s
// directly (as Nikita's unit does).
inline soDamageEventObserver::~soDamageEventObserver() { }
// The arrow teardown expands the controller link teardown (as Mario's and Snake's units do).
inline soControllerModuleLinkRef::~soControllerModuleLinkRef() { }
// The articles' sound builder teardown inlines this empty destructor (same form as ft_mario.cpp and ft_snake.cpp).
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_112_bss_1AC;
extern soStatusUniqProcess lbl_112_bss_1BC;
extern soStatusUniqProcess lbl_112_bss_1CC;
}

ftPitExtendParamAccesser g_ftPitExtendParamAccesser;

ftClassInfoImpl<Fighter_Pit, ftPit> g_ftClassInfoPit;

// StageObject's null input (declared early, see the top of the file).
inline Input* StageObject::getInput() {
    static IpNull sNullInput;
    return &sNullInput;
}

ftPit::ftPit(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPitBuildConfig>(entryId,
                                         Fighter_Pit,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Pit)),
    unk297B4(0),
    unk297B5(0) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[15] = {0};
    processes[9] = &lbl_112_bss_1AC;
    processes[11] = &lbl_112_bss_1BC;
    processes[13] = &lbl_112_bss_1CC;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 15);
    // HYPOTHESIS: the common data record holds the fighter's shield shape group at +0x8c (the Mirror Shield), group 1.
    m_moduleAccesser->getCollisionShieldModule().add(reinterpret_cast<soCollisionShieldGroupData*>(reinterpret_cast<u8*>(m_data) + 0x8c), 1);
    // The reflector group 3 comes straight from the record at +0x98.
    m_moduleAccesser->getCollisionReflectorModule().add(*reinterpret_cast<soCollisionReflectorGroupData**>(reinterpret_cast<u8*>(m_data) + 0x98), 3);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 3);
    // Reflector group 2 (the Mirror Shield's rebound) is built from the record at +0x9c.
    u8 shape[0x20];
    ftReflectorGroupDraft group;
    FT_REFLECTOR_GROUP_CLEAR(group);
    fn_27_195B34(*reinterpret_cast<void**>(reinterpret_cast<u8*>(m_data) + 0x9c), shape, &group);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&group), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
}

// BYTES: the native function addresses the local static's guard, object and destructor registration separately
// instead of from one pooled BSS base. The setting is per function: unit-wide it breaks the pooled constructor and
// __sinit, and an in-class inline definition makes the static weak and moves it to the end of .bss.
#pragma pool_data off
ftPitTransactor* ftPitTransactor::getInstance() {
    static ftPitTransactor instance;
    return &instance;
}
#pragma pool_data reset

// The bow starts through the transactor with the model and texture resource of the fighter.
bool ftPit::activeArticle(wnPitBow* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    return ftPitTransactor::getInstance()->activeArticle(weapon, acc, resources.getResourceIdAccesser()->getMdlResId(), resources.getResourceIdAccesser()->getTexResId(), true, false);
}

// The army of the Final Smash starts at the stage object of the fighter with its team and the scale.
bool ftPit::activeArticle1(wnPitParthena* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(), fighter.getOwner()->getTeam(), acc->getPostureModule().getScale());
    return true;
}

// ---- the article holders --------------------------------------------------------------------------------------------
// Articles borrow the owner's kind record and heap while their constructors run.
// The data lookup receives a separate snapshot of the owner's fighter kind.
#include <ft/builder/ft_builder_article.h>

struct ftPitArticleKind : wnArticleKindInfo {
    ftKind getSubKind() const { return static_cast<ftKind>(subKind); }
    ftPitArticleKind() {
        kind = 0;
        subKind = Fighter_Pit;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_149064(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1490FC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1491AC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

// Keep explicit holder bodies where the pool calls them out of line.
FT_DEFINE_ARTICLE_HOLDER(inline, wnPitBow, wnPitBowData, 0, ftPitArticleKind, fn_27_149064)
FT_DEFINE_ARTICLE_HOLDER(inline, wnPitParthena, wnPitParthenaData, 2, ftPitArticleKind, fn_27_1491AC)

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnPitBow>::~wnArticleHolder();
template wnArticleHolder<wnPitBowArrow>::~wnArticleHolder();
template wnArticleHolder<wnPitParthena>::~wnArticleHolder();
template wnArticleHolder<wnPitBowArrow>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftPitArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftPitArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftPitArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftPitArticleMediator::~ftPitArticleMediator() { }
#pragma dont_inline off

s32 ftPitArticleMediator::getMediateNum() { return 3; }
void ftPitArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

FT_DEFINE_ARTICLE_HOLDER(, wnPitBowArrow, wnPitBowArrowData, 1, ftPitArticleKind, fn_27_1490FC)
#ifdef WN_PIT_IKARUS_FULL_BUILDER
// The weak Final Smash teardown is emitted by this unit (native 0x5024, right after the Parthena holder chain).
wnPitParthena::~wnPitParthena() { }
#endif

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Bow, 1 BowArrow, 2 Parthena (the rest are unused).

#define FT_PIT_POOL_BOW_BASE static_cast<wnArticlePool<wnPitBow, 1, ftPitBowArrowPool>&>(m_pools)
#define FT_PIT_POOL_BOW FT_PIT_POOL_BOW_BASE.getSub()
#define FT_PIT_POOL_BOWARROW_BASE static_cast<wnArticlePool<wnPitBowArrow, 4, ftPitParthenaPool>&>(m_pools)
#define FT_PIT_POOL_BOWARROW FT_PIT_POOL_BOWARROW_BASE.getSub()
#define FT_PIT_POOL_PARTHENA_BASE static_cast<wnArticlePool<wnPitParthena, 1, soInstancePoolRoot>&>(m_pools)
#define FT_PIT_POOL_PARTHENA FT_PIT_POOL_PARTHENA_BASE.getSub()

#define FT_PIT_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(3, value) WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) \
    WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) \
    WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) \
    WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) \
    WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftPitArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnPitBow* weapon = FT_PIT_POOL_BOW.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 4; ++i) {
        wnPitBowArrow* weapon = FT_PIT_POOL_BOWARROW.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnPitParthena* weapon = FT_PIT_POOL_PARTHENA.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftPitArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(4, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    FT_PIT_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftPitActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftPitArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnPitBow, 1, ftPitBowArrowPool>, ftPitActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnPitBowArrow, 4, ftPitParthenaPool>, ftPitActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotActiveNum<wnArticlePool<wnPitParthena, 1, soInstancePoolRoot>, ftPitActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_PIT_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftPitArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnPitBow, 1, ftPitBowArrowPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnPitBowArrow, 4, ftPitParthenaPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotIsGeneratable<wnArticlePool<wnPitParthena, 1, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_PIT_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftPitArticleActivator;
template <> struct ftPitArticleActivator<wnPitBow> {
    static bool activate(wnPitBow* weapon, soModuleAccesser* acc) { return ftPit::activeArticle(weapon, acc); }
};
template <> struct ftPitArticleActivator<wnPitBowArrow> {
    static bool activate(wnPitBowArrow* weapon, soModuleAccesser* acc) {
        u32 etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
        return ftPitTransactor::getInstance()->activeArticle1(weapon, acc, etcResId);
    }
};
template <> struct ftPitArticleActivator<wnPitParthena> {
    static bool activate(wnPitParthena* weapon, soModuleAccesser* acc) { return ftPit::activeArticle1(weapon, acc); }
};

soArticle* ftPitArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftPitArticleActivator<wnPitBow> >(static_cast<wnArticlePool<wnPitBow, 1, ftPitBowArrowPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftPitArticleActivator<wnPitBowArrow> >(static_cast<wnArticlePool<wnPitBowArrow, 4, ftPitParthenaPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerate<ftPitArticleActivator<wnPitParthena> >(static_cast<wnArticlePool<wnPitParthena, 1, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_PIT_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

template <int S, int L>
inline bool ftPitShootSlot1(soModuleAccesser* acc, soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    return ftPitTransactor::getInstance()->shootArticle(&dynamic_cast<wnPitBowArrow&>(*article), acc);
}

bool ftPitArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnPitBow>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return ftPitShootSlot1(acc, article, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotShoot<wnPitParthena>(article, WN_ARTICLE_SLOT_TAGS);
    FT_PIT_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftPit is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftPitInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}

// Virtual slot table of the kinetic and checkActivate forwarders: slot k sits at vtable offset 8 + 4k.
// Declared only; the slots are never defined here.
class ftPitVirtualSlots {
public:
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
    virtual void* slot20();
};

// ftPit reads the shared parameter-table variation like Marth does (ftManager::setParamPattern).
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

ftKineticEnergyController::~ftKineticEnergyController() { }

#pragma dont_inline on
ftVirtualNodeMatrixPool::~ftVirtualNodeMatrixPool() { }
soTeam::~soTeam() { }
soParamAccesser::~soParamAccesser() { }
#pragma dont_inline off
soArticleMediator::~soArticleMediator() { }

void ftPit::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
    if (kind != 2) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003D);
    }
}

void ftPit::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (moduleAccesser->getItemManageModule().isHaveItem(0) == true) {
        soVisibilityModule& visibility = moduleAccesser->getVisibilityModule();
        if (visibility.get(1) != 3) {
            visibility.set(1, 2);
        }
    }
}

void ftPit::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0x3C:
        if (stageObj->soGetKind() == 2 && stageObj->soGetSubKind() == 0x7E) {
            moduleAccesser->getEffectModule().clearScreen(0, 0);
            endFinal(true, true, false);
        }
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

void ftPit::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if (groupIndex >= 2) {
        unk297B0 = groupIndex;
        if (power >= m_moduleAccesser->getCollisionReflectorModule().getAttackLimit(groupIndex)) {
            unk297B5 = 1;
            return;
        }
        unk297B4 = 1;
    }
    Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

void ftPit::onStart(int kind) {
    m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantInt(m_moduleAccesser, 0x5DC3, 0), 0x11000013);
    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003D);
    m_moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBB, 0), 0x11000014);
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000015);
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000041);
    Fighter::onStart(kind);
}

void ftPit::processUpdate() {
    Fighter::processUpdate();
    if (soSlow::getInstance()->isEstimate() == true) {
        // HYPOTHESIS: 0x11000013 is the accumulated mirror-shield power, decayed by a constant while slowdown is estimated.
        float value = m_moduleAccesser->getWorkManageModule().getFloat(0x11000013);
        value += soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFB7, 0);
        int limit = soValueAccesser::getConstantInt(m_moduleAccesser, 0x5DC3, 0);
        if (value > (float)limit) {
            value = (float)limit;
        }
        m_moduleAccesser->getWorkManageModule().setFloat(value, 0x11000013);
        m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000015);
        switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
        case 0x11F:
        case 0x115:
            break;
        default: {
            float timer = m_moduleAccesser->getWorkManageModule().getFloat(0x11000014);
            timer += soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBC, 0);
            if (timer > soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBB, 0)) {
                timer = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBB, 0);
            }
            m_moduleAccesser->getWorkManageModule().setFloat(timer, 0x11000014);
            break;
        }
        }
    }
    unk297B5 = 0;
    unk297B4 = 0;
}

void ftPit::notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if (groupIndex == 1) {
        int collisionIndex = collisionLog->m_collsionIndex;
        soCollisionAttackData* attackData = attackModule->getData(collisionIndex, collisionLog->m_isAbsolute);
        if (power > m_moduleAccesser->getWorkManageModule().getFloat(0x11000015)) {
            m_moduleAccesser->getWorkManageModule().setFloat(power, 0x11000015);
            m_moduleAccesser->getWorkManageModule().setFloat(m_moduleAccesser->getDamageModule().getDamageLr(posX, posY, attackModule, attackData, collisionIndex, collisionLog->m_isAbsolute), 0x11000016);
        }
    }
    Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

bool ftPit::notifyEventCollisionShieldCheck() {
    float power = m_moduleAccesser->getWorkManageModule().getFloat(0x11000015);
    if (power > 0.0f) {
        soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBD, 0);
        m_moduleAccesser->getStopModule().setHitStop((float)(int)power, 0);
        soModuleAccesser* accesser = m_moduleAccesser;
        ftUtil::setShieldSetoffSpeed(accesser, power, accesser->getWorkManageModule().getFloat(0x11000016));
        float remain = m_moduleAccesser->getWorkManageModule().getFloat(0x11000014) - power;
        if (remain < 0.0f) {
            m_moduleAccesser->getCollisionShieldModule().setStatus(0, 0, 1);
            remain = 0.0f;
        }
        m_moduleAccesser->getWorkManageModule().setFloat(remain, 0x11000014);
        return false;
    }
    return Fighter::notifyEventCollisionShieldCheck();
}

bool ftPit::notifyEventCollisionReflectorCheck() {
    if (unk297B4 == 1) {
        if (unk297B0 == 3) {
            m_moduleAccesser->getEffectModule().reqFollow(static_cast<EfID>(0x180010), 0x2C, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 1, 0, -1);
            int soundHandle = m_moduleAccesser->getWorkManageModule().getInt(0x10000041);
            if (soundHandle == -1 || !m_moduleAccesser->getSoundModule().isPlay(soundHandle)) {
                int handle = m_moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x2C8), false, false, 0);
                m_moduleAccesser->getWorkManageModule().setInt(handle, 0x10000041);
            }
        }
    } else if (unk297B5 == 1) {
        m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, unk297B0);
    }
    return Fighter::notifyEventCollisionReflectorCheck();
}
