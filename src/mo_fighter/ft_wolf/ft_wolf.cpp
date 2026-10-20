// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_WOLF_ARTICLE_POOLS
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_FROM_VEC3_CTOR_NOINLINE
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/wolf/ft_wolf_builder.h>
#include <ft/wolf/ft_wolf_extend_param_accesser.h>
#include <ft/wolf/ft_wolf_transactor.h>
#include <ft/ft_physics_iface.h>
#include <ft/wolf/ft_wolf_status_uniq_process_reflector.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/ft_reflector_group.h>
#include <ft/fox/ft_fox_status_uniq_process.h>
#include <so/article/so_article_mediator.h>
#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <so/team/so_team.h>
#include <so/param/so_param_customize_module_impl.h>

#define FT_BC ftWolfBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

ftWolfExtendParamAccesser g_ftWolfExtendParamAccesser;

ftClassInfoImpl<Fighter_Wolf, ftWolf> g_ftClassInfoWolf;

// Wolf's reflector process: the shared one with Wolf's own turn (setTurn is in ft_wolf_status_uniq_process_reflector.cpp).
ftWolfStatusUniqProcessReflectorImpl::ftWolfStatusUniqProcessReflectorImpl() { }
#pragma dont_inline on
ftWolfStatusUniqProcessReflectorImpl::~ftWolfStatusUniqProcessReflectorImpl() { }
#pragma dont_inline off
ftWolfStatusUniqProcessReflectorImpl g_ftWolfStatusUniqProcessReflector;

// MATCH-ONLY: the trail's constructor is called out of line (the copy sora_melee owns).
#pragma dont_inline on
template soArrayVector<ftWolf::PostureInfo, 4>::soArrayVector(s32, const ftWolf::PostureInfo&, s32);
#pragma dont_inline off

ftWolf::ftWolf(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftWolfBuildConfig>(entryId,
                                         Fighter_Wolf,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_commonData(static_cast<ftWolfData*>(g_ftCommonDataAccesser.getData(Fighter_Wolf))),
    unk1CC88(0),
    unk1CC89(0),
    m_postureTrail(4, PostureInfo()) {
    // Wolf owns the special move processes (blaster, Phantasm, Fire Bird and the Landmaster); the reflector is set up
    // by its own unit.
    soStatusUniqProcess* processes[14] = {0};
    processes[0] = &g_ftFoxStatusUniqProcessBlaster;
    processes[1] = &g_ftFoxStatusUniqProcessIllusion;
    processes[2] = &g_ftFoxStatusUniqProcessFire;
    processes[3] = &g_ftWolfStatusUniqProcessReflector;
    processes[5] = &g_ftFoxStatusUniqProcessFire;
    processes[6] = &g_ftFoxStatusUniqProcessFire;
    processes[7] = &g_ftFoxStatusUniqProcessFire;
    processes[8] = &g_ftWolfStatusUniqProcessReflector;
    processes[9] = &g_ftWolfStatusUniqProcessReflector;
    processes[10] = &g_ftWolfStatusUniqProcessReflector;
    processes[11] = &g_ftFoxStatusUniqProcessFinal;
    processes[12] = &g_ftFoxStatusUniqProcessFinal;
    processes[13] = &g_ftFoxStatusUniqProcessFinal;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 14);

    // The reflector (the shine) is built from the record at +0x90 of the common data.
    u8 reflectorShape[0x20];
    ftReflectorGroupDraft reflectorGroup;
    void* reflectorRecord = *reinterpret_cast<void**>(reinterpret_cast<u8*>(m_commonData) + 0x90);
    FT_REFLECTOR_GROUP_CLEAR(reflectorGroup);
    fn_27_195B34(reflectorRecord, reflectorShape, &reflectorGroup);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&reflectorGroup), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    ftPhysicsModuleCall54(m_moduleAccesser->m_enumerationStart->m_physicsModule, 0);
    ftPhysicsModuleCall6C(m_moduleAccesser->m_enumerationStart->m_physicsModule, 0x14);
}

ftWolf::~ftWolf() { }

void ftWolf::processUpdate() {
    Fighter::processUpdate();
    unk1CC89 = 0;
    unk1CC88 = 0;
}

// The trail of positions is emptied when a stock starts.
void ftWolf::onStart(int param) {
    m_postureTrail.getArray()->clear();
    Fighter::onStart(param);
}

// While the Phantasm is running (status 0x113) the fighter records its position and rotation every frame (not during a hit pause
// or slow motion); the afterimage articles ask for the samples with link event 0x455.
void ftWolf::processFixPosition() {
    Fighter::processFixPosition();
    if (soSlow::getInstance()->isEstimate() == true && m_moduleAccesser->getStopModule().isStop() == 0
        && m_moduleAccesser->getStatusModule().getStatusKind() == 0x113) {
        PostureInfo sample;
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        sample.m_pos = pos;
        sample.m_rotX = m_moduleAccesser->getPostureModule().getRot(0).m_x;
        if (m_postureTrail.getArray()->isFull() == true) {
            m_postureTrail.getArray()->pop();
        }
        m_postureTrail.getArray()->unshift(sample);
    }
}

// HYPOTHESIS: the view of the link events this fighter answers: the kind, a flag, and for the trail query (0x455) the index of
// the sample followed by the position and the rotation to fill in; for the rumble event (0x83a) the strength class.
struct ftWolfLinkEventView {
    int kind;
    u8 flag;
    u8 pad[3];
    u32 value;
    Vec3f pos;
    float rotX;
};

// The part of the event that the rumble case copies (the base arguments and the strength class).
struct ftWolfLinkEventRumble : soLinkEventArgs {
    u32 value;
};

void ftWolf::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftWolfLinkEventView* event = reinterpret_cast<ftWolfLinkEventView*>(eventInfo);
    // Events of the Landmaster (an article of subkind 0x21): its start, its end and the hits it takes.
    if (stageObj->soGetKind() == 2 && stageObj->soGetSubKind() == 0x21) {
        switch (event->kind) {
        case 0x838:
            moduleAccesser->getControllerModule().resetFlickY();
            return;
        case 0x839:
        case 0x3c: {
            int status = moduleAccesser->getStatusModule().getStatusKind();
            switch (status) {
            case 0x116:
            case 0x11d:
            case 0x11e:
                moduleAccesser->getStatusModule().changeStatusRequest(0x11f, moduleAccesser);
                moduleAccesser->getLinkModule().unlink(6);
                break;
            }
            if (event->kind == 0x3c) {
                endFinal(true, true, false);
                break;
            }
            return;
        }
        case 0x83a: {
            moduleAccesser->getControllerModule().stopRumble(true);
            ftWolfLinkEventRumble copy = *reinterpret_cast<ftWolfLinkEventRumble*>(event);
            switch (copy.value) {
            case 0:
            case 9:
                break;
            case 1:
                moduleAccesser->getControllerModule().setRumble(2, 0, true, -1);
                break;
            case 5:
                moduleAccesser->getControllerModule().setRumble(0xf, 0, true, -1);
                break;
            case 6:
                moduleAccesser->getControllerModule().setRumble(8, 0, true, -1);
                break;
            case 7:
                moduleAccesser->getControllerModule().setRumble(5, 0, true, -1);
                break;
            case 8:
                moduleAccesser->getControllerModule().setRumble(0xe, 0, true, -1);
                break;
            }
            return;
        }
        }
    }
    switch (event->kind) {
    case 0x455:
        event->pos = m_postureTrail.getArray()->at(event->value).m_pos;
        event->rotX = m_postureTrail.getArray()->at(event->value).m_rotX;
        break;
    default:
        Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
        break;
    }
}

// The reflector (the shine) keeps two flags for the frame: the hit was too weak for it (unk1CC88) or it stopped it (unk1CC89).
void ftWolf::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) {
    if ((s32)groupIndex == 2) {
        if (power >= m_moduleAccesser->getCollisionReflectorModule().getAttackLimit(groupIndex)) {
            unk1CC89 = 1;
            return;
        }
        unk1CC88 = 1;
    }
    Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY);
}

bool ftWolf::notifyEventCollisionReflectorCheck() {
    if (unk1CC88 == 1) {
        // The hit was too weak for the reflector: the shine is restarted once its animation has run for a while.
        if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x11b) {
            if (m_moduleAccesser->getMotionModule().getFrame() >= 10.0f) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x11b, m_moduleAccesser);
            }
        } else {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x11b, m_moduleAccesser);
        }
        return true;
    }
    if (unk1CC89 == 1) {
        // The reflector stopped the hit: its effects end and the reflector is rearmed.
        m_moduleAccesser->getEffectModule().killAll(1, true, true);
        m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    }
    return Fighter::notifyEventCollisionReflectorCheck();
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Wolf, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftWolfArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftWolfArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftWolfArticleKind : wnArticleKindInfo {
    ftWolfArticleKind() {
        kind = 0;
        subKind = Fighter_Wolf;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftWolfArticleBuildInfo : wnArticleBuildInfo {
    ftWolfArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (the numbers of its getters are their
// order in the module; only declared here).
extern "C" void* fn_27_1484BC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1486C8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14875C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1487F0(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftWolfArticleTraits;
#define FT_WOLF_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftWolfArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftWolfArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Wolf, &tag.m_resourceGroup)); \
        } \
    };

FT_WOLF_TRAITS(wnWolfBlaster, wnWolfBlasterData, 0, fn_27_1486C8)
FT_WOLF_TRAITS(wnWolfBlasterBullet, wnWolfBlasterBulletData, 1, fn_27_14875C)
FT_WOLF_TRAITS(wnWolfIllusion, wnWolfIllusionData, 2, fn_27_1487F0)
FT_WOLF_TRAITS(wnFoxLandMaster, wnFoxLandMasterData, 3, fn_27_1484BC)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftWolfArticleTraits<W>::ArticleId, ftWolfArticleBuildInfo(ftWolfArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftWolfArticleTraits<W>::getData()) { }

// ---- the articles of Wolf and their mediator -----------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnWolfBlaster>::~wnArticleHolder();
template wnArticleHolder<wnWolfBlasterBullet>::~wnArticleHolder();
template wnArticleHolder<wnWolfIllusion>::~wnArticleHolder();
template wnArticleHolder<wnFoxLandMaster>::~wnArticleHolder();
template wnArticleHolder<wnWolfBlasterBullet>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnWolfIllusion>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftWolfArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftWolfArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftWolfArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftWolfArticleMediator::~ftWolfArticleMediator() { }
#pragma dont_inline off

s32 ftWolfArticleMediator::getMediateNum() { return 4; }
void ftWolfArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 blaster, 1 blaster bullet, 2 Phantasm afterimage, 3 Landmaster (the rest are unused).

#define FT_WOLF_POOL_BLASTER_BASE static_cast<wnArticlePool<wnWolfBlaster, 1, ftWolfBulletPool>&>(m_pools)
#define FT_WOLF_POOL_BLASTER FT_WOLF_POOL_BLASTER_BASE.getSub()
#define FT_WOLF_POOL_BULLET_BASE static_cast<wnArticlePool<wnWolfBlasterBullet, 5, ftWolfIllusionPool>&>(m_pools)
#define FT_WOLF_POOL_BULLET FT_WOLF_POOL_BULLET_BASE.getSub()
#define FT_WOLF_POOL_ILLUSION_BASE static_cast<wnArticlePool<wnWolfIllusion, 2, ftWolfLandMasterPool>&>(m_pools)
#define FT_WOLF_POOL_ILLUSION FT_WOLF_POOL_ILLUSION_BASE.getSub()
#define FT_WOLF_POOL_LAND_MASTER_BASE static_cast<wnArticlePool<wnFoxLandMaster, 1, soInstancePoolRoot>&>(m_pools)
#define FT_WOLF_POOL_LAND_MASTER FT_WOLF_POOL_LAND_MASTER_BASE.getSub()

#define FT_WOLF_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

template <class W> struct ftWolfArticleActivator;
template <> struct ftWolfArticleActivator<wnWolfBlaster> {
    static bool activate(wnWolfBlaster* weapon, soModuleAccesser* acc) { return ftWolf::activeArticle(weapon, acc); }
};
template <> struct ftWolfArticleActivator<wnWolfBlasterBullet> {
    static bool activate(wnWolfBlasterBullet* weapon, soModuleAccesser* acc) {
        u32 etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
        return ftWolfTransactor::getInstance()->activeArticle2(weapon, acc, 0x3c, etcResId, 0x2D0004);
    }
};
template <> struct ftWolfArticleActivator<wnWolfIllusion> {
    static bool activate(wnWolfIllusion* weapon, soModuleAccesser* acc) { return ftWolf::activeArticle1(weapon, acc); }
};
template <> struct ftWolfArticleActivator<wnFoxLandMaster> {
    static bool activate(wnFoxLandMaster* weapon, soModuleAccesser* acc) { return ftWolf::activeArticle2(weapon, acc); }
};

void ftWolfArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnWolfBlaster* weapon = FT_WOLF_POOL_BLASTER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 5; ++i) {
        wnWolfBlasterBullet* weapon = FT_WOLF_POOL_BULLET.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnWolfIllusion* weapon = FT_WOLF_POOL_ILLUSION.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnFoxLandMaster* weapon = FT_WOLF_POOL_LAND_MASTER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftWolfArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 1; }
    case 1: { wnArticleSlotTag tag; return 5; }
    case 2: { wnArticleSlotTag tag; return 2; }
    case 3: { wnArticleSlotTag tag; return 1; }
    FT_WOLF_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftWolfActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_WOLF_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftWolfActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftWolfActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftWolfArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_WOLF_COUNT_ACTIVE(FT_WOLF_POOL_BLASTER, 1)
    case 1: FT_WOLF_COUNT_ACTIVE(FT_WOLF_POOL_BULLET, 5)
    case 2: FT_WOLF_COUNT_ACTIVE(FT_WOLF_POOL_ILLUSION, 2)
    case 3: FT_WOLF_COUNT_ACTIVE(FT_WOLF_POOL_LAND_MASTER, 1)
    FT_WOLF_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_WOLF_CAN_GENERATE(POOL, N, ID) \
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

bool ftWolfArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_WOLF_CAN_GENERATE(FT_WOLF_POOL_BLASTER, 1, 0)
    case 1: FT_WOLF_CAN_GENERATE(FT_WOLF_POOL_BULLET, 5, 1)
    case 2: FT_WOLF_CAN_GENERATE(FT_WOLF_POOL_ILLUSION, 2, 2)
    case 3: FT_WOLF_CAN_GENERATE(FT_WOLF_POOL_LAND_MASTER, 1, 3)
    FT_WOLF_UNUSED_SLOTS(false)
    default: return false;
    }
}

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_WOLF_GENERATE(W, POOL) \
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
        if (ftWolfArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftWolfArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_WOLF_GENERATE(wnWolfBlaster, FT_WOLF_POOL_BLASTER)
    case 1: FT_WOLF_GENERATE(wnWolfBlasterBullet, FT_WOLF_POOL_BULLET)
    case 2: FT_WOLF_GENERATE(wnWolfIllusion, FT_WOLF_POOL_ILLUSION)
    case 3: FT_WOLF_GENERATE(wnFoxLandMaster, FT_WOLF_POOL_LAND_MASTER)
    FT_WOLF_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Shooting only type-checks the article; the unused slots accept anything.
bool ftWolfArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; (void)dynamic_cast<wnWolfBlaster&>(*article); return true; }
    case 1: { wnArticleSlotTag tag; (void)dynamic_cast<wnWolfBlasterBullet&>(*article); return true; }
    case 2: { wnArticleSlotTag tag; (void)dynamic_cast<wnWolfIllusion&>(*article); return true; }
    case 3: { wnArticleSlotTag tag; (void)dynamic_cast<wnFoxLandMaster&>(*article); return true; }
    FT_WOLF_UNUSED_SLOTS(true)
    default: return false;
    }
}

// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
#pragma dont_inline on
ftWolfTransactor* ftWolfTransactor::getInstance() {
    static ftWolfTransactor instance;
    return &instance;
}
#pragma dont_inline off
#pragma pool_data reset

// ---- the activators of the articles ---------------------------------------------------------------------------------

// The blaster article starts through the transactor with the model and the texture resource of the fighter.
bool ftWolf::activeArticle(wnWolfBlaster* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    u32 texResId = resources.getResourceIdAccesser()->getTexResId();
    u32 mdlResId = resources.getResourceIdAccesser()->getMdlResId();
    return ftWolfTransactor::getInstance()->activeArticle1(weapon, acc, mdlResId, texResId, true, false);
}

// The Phantasm afterimage: it starts with the situation and facing of the fighter and a flag that tells it whether an earlier
// afterimage already ran (the work int 0x20000003 counts them).
bool ftWolf::activeArticle1(wnWolfIllusion* weapon, soModuleAccesser* acc) {
    int count = acc->getWorkManageModule().getInt(0x20000003);
    bool followUp = count != 0;
    soSituationModule& situation = acc->getSituationModule();
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    float lr = acc->getPostureModule().getLr();
    u8 situationKind = situation.getKind();
    int team = acc->getTeamModule().getTeam()->getNo();
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), team, lr, situationKind, followUp);
    acc->getWorkManageModule().setInt(count + 1, 0x20000003);
    return false;
}

// The Landmaster of the Final Smash: it starts at the fighter and takes it along (link 6).
bool ftWolf::activeArticle2(wnFoxLandMaster* weapon, soModuleAccesser* acc) {
    soTeamModule& teamModule = acc->getTeamModule();
    soResourceModule& resources = acc->getResourceModule();
    soPostureModule& posture = acc->getPostureModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    float scale = soValueAccesser::getConstantFloat(acc, 0xdb2, 0);
    float lr = posture.getLr();
    Vec3f pos = posture.getPos();
    int team = teamModule.getTeam()->getNo();
    int resourceId = dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId();
    weapon->activate(founderTaskId, resourceId, team, &pos, lr, scale, 0x1f);
    acc->getLinkModule().link(6, weapon->m_taskId);
    return true;
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftWolf is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftWolfInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftWolfInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;


// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtual overrides and default observers.
void fn_125_9F74() {}
void fn_125_DE30() {}
void fn_125_DF18() {}
void fn_125_E000() {}
void fn_125_E0E8() {}
void fn_125_E1D0() {}
void fn_125_E2B8() {}
void fn_125_E3A0() {}
void fn_125_E488() {}

// Constant-zero returns. The parameters are unused; they exist so callers can pass arguments.
int fn_125_DEDC(void*, void*, void*) { return 0; }
int fn_125_DFC4(void*, void*, void*) { return 0; }
int fn_125_E0AC(void*, void*, void*) { return 0; }
int fn_125_E194(void*, void*, void*) { return 0; }
int fn_125_E27C(void*, void*, void*) { return 0; }
int fn_125_E364(void*, void*, void*) { return 0; }
int fn_125_E44C(void*, void*, void*) { return 0; }
int fn_125_E534(void*, void*, void*) { return 0; }

} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_125_1EAC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_3720(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_3F78(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_42F0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_45C8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_4608(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_4CD8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_4E6C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_6130(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_6170(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_125_61B0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
int fn_125_AAEC() { return 0; }
int fn_125_B160() { return 0x1; }
void fn_125_B168() {}
int fn_125_B16C() { return 0x0; }
void fn_125_B174() {}
int fn_125_B178() { return 0x0; }
void fn_125_B180() {}
void fn_125_B184() {}
void fn_125_B188() {}
void fn_125_B18C() {}
void fn_125_B190() {}
void fn_125_DDAC() {}
u8* fn_125_DE34(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_DF1C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_E004(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_E0EC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_E1D4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_E2BC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_E3A4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_125_E48C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"
