#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_FOX_ARTICLE_POOLS
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_FROM_VEC3_CTOR_NOINLINE
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/fox/ft_fox.h>
#include <ft/fox/ft_fox_extend_param_accesser.h>
#include <ft/fox/ft_fox_transactor.h>
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

#define FT_BC ftFoxBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

ftFoxExtendParamAccesser g_ftFoxExtendParamAccesser;

ftClassInfoImpl<Fighter_Fox, ftFox> g_ftClassInfoFox;

// Fox's reflector process: the shared one with Fox's own turn (the reflector turns to the stick like Wolf's).
class ftFoxStatusUniqProcessReflectorImpl : public ftFoxStatusUniqProcessReflector {
public:
    ftFoxStatusUniqProcessReflectorImpl();
    virtual ~ftFoxStatusUniqProcessReflectorImpl();
    virtual void setTurn(soModuleAccesser* moduleAccesser);
};
ftFoxStatusUniqProcessReflectorImpl::ftFoxStatusUniqProcessReflectorImpl() { }
#pragma dont_inline on
ftFoxStatusUniqProcessReflectorImpl::~ftFoxStatusUniqProcessReflectorImpl() { }
#pragma dont_inline off
ftFoxStatusUniqProcessReflectorImpl g_ftFoxStatusUniqProcessReflector;

void ftFoxStatusUniqProcessReflectorImpl::setTurn(soModuleAccesser* moduleAccesser) {
    ftFoxData* data = dynamic_cast<ftFox&>(moduleAccesser->getStageObject()).m_commonData;
    const soModuleEnumeration& modules = *moduleAccesser->m_enumerationStart;
    soTurnModule& turn = *static_cast<soTurnModule*>(modules.m_turnModule);
    turn.set(&data->reflectorTurnData, true, false, modules.m_postureModule->getLr());
}

// MATCH-ONLY: the trail's constructor is called out of line (the copy sora_melee owns).
#pragma dont_inline on
template soArrayVector<ftFox::PostureInfo, 4>::soArrayVector(s32, const ftFox::PostureInfo&, s32);
#pragma dont_inline off

ftFox::ftFox(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftFoxBuildConfig>(entryId,
                                         Fighter_Fox,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_commonData(static_cast<ftFoxData*>(g_ftCommonDataAccesser.getData(Fighter_Fox))),
    unk1CD44(0),
    unk1CD45(0),
    m_postureTrail(4, PostureInfo()) {
    // Fox owns the special move processes (blaster, Phantasm, Fire Bird and the Landmaster); the reflector is set up
    // by its own unit.
    soStatusUniqProcess* processes[14] = {0};
    processes[0] = &g_ftFoxStatusUniqProcessBlaster;
    processes[1] = &g_ftFoxStatusUniqProcessIllusion;
    processes[2] = &g_ftFoxStatusUniqProcessFire;
    processes[3] = &g_ftFoxStatusUniqProcessReflector;
    processes[5] = &g_ftFoxStatusUniqProcessFire;
    processes[6] = &g_ftFoxStatusUniqProcessFire;
    processes[7] = &g_ftFoxStatusUniqProcessFire;
    processes[8] = &g_ftFoxStatusUniqProcessReflector;
    processes[9] = &g_ftFoxStatusUniqProcessReflector;
    processes[10] = &g_ftFoxStatusUniqProcessReflector;
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
    ftPhysicsModuleCall6C(m_moduleAccesser->m_enumerationStart->m_physicsModule, 0x15);
}

ftFox::~ftFox() { }

void ftFox::processUpdate() {
    Fighter::processUpdate();
    unk1CD45 = 0;
    unk1CD44 = 0;
}

// The trail of positions is emptied when a stock starts.
void ftFox::onStart(int param) {
    m_postureTrail.getArray()->clear();
    Fighter::onStart(param);
    unk1CD45 = 0;
    unk1CD44 = 0;
}

// While the Phantasm is running (status 0x113) the fighter records its position and rotation every frame (not during a hit pause
// or slow motion); the afterimage articles ask for the samples with link event 0x455.
void ftFox::processFixPosition() {
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
struct ftFoxLinkEventView {
    int kind;
    u8 flag;
    u8 pad[3];
    u32 value;
    Vec3f pos;
    float rotX;
};

// The part of the event that the rumble case copies (the base arguments and the strength class).
struct ftFoxLinkEventRumble : soLinkEventArgs {
    u32 value;
};

void ftFox::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftFoxLinkEventView* event = reinterpret_cast<ftFoxLinkEventView*>(eventInfo);
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
            ftFoxLinkEventRumble copy = *reinterpret_cast<ftFoxLinkEventRumble*>(event);
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

// The reflector (the shine) keeps two flags for the frame: the hit was too weak for it (unk1CD44) or it stopped it (unk1CD45).
void ftFox::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if ((s32)groupIndex == 2) {
        if (power >= m_moduleAccesser->getCollisionReflectorModule().getAttackLimit(groupIndex)) {
            unk1CD45 = 1;
            return;
        }
        unk1CD44 = 1;
    }
    Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

bool ftFox::notifyEventCollisionReflectorCheck() {
    if (unk1CD44 == 1) {
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
    if (unk1CD45 == 1) {
        // The reflector stopped the hit: its effects end and the reflector is rearmed.
        m_moduleAccesser->getEffectModule().killAll(1, true, true);
        m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    }
    return Fighter::notifyEventCollisionReflectorCheck();
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Fox, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftFoxArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftFoxArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftFoxArticleKind : wnArticleKindInfo {
    ftFoxArticleKind() {
        kind = 0;
        subKind = Fighter_Fox;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftFoxArticleBuildInfo : wnArticleBuildInfo {
    ftFoxArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (the numbers of its getters are their
// order in the module; only declared here).
extern "C" void* fn_27_1484BC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148380(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148414(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1484A8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftFoxArticleTraits;
#define FT_FOX_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftFoxArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftFoxArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Fox, &tag.m_resourceGroup)); \
        } \
    };

FT_FOX_TRAITS(wnFoxBlaster, wnFoxBlasterData, 0, fn_27_148380)
FT_FOX_TRAITS(wnFoxBlasterBullet, wnFoxBlasterBulletData, 1, fn_27_148414)
FT_FOX_TRAITS(wnFoxIllusion, wnFoxIllusionData, 2, fn_27_1484A8)
FT_FOX_TRAITS(wnFoxLandMaster, wnFoxLandMasterData, 3, fn_27_1484BC)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftFoxArticleTraits<W>::ArticleId, ftFoxArticleBuildInfo(ftFoxArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftFoxArticleTraits<W>::getData()) { }

// ---- the articles of Fox and their mediator -----------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnFoxBlaster>::~wnArticleHolder();
template wnArticleHolder<wnFoxBlasterBullet>::~wnArticleHolder();
template wnArticleHolder<wnFoxIllusion>::~wnArticleHolder();
template wnArticleHolder<wnFoxLandMaster>::~wnArticleHolder();
template wnArticleHolder<wnFoxBlasterBullet>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnFoxIllusion>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftFoxArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftFoxArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftFoxArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftFoxArticleMediator::~ftFoxArticleMediator() { }
#pragma dont_inline off

s32 ftFoxArticleMediator::getMediateNum() { return 4; }
void ftFoxArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 blaster, 1 blaster bullet, 2 Phantasm afterimage, 3 Landmaster (the rest are unused).

#define FT_FOX_POOL_BLASTER_BASE static_cast<wnArticlePool<wnFoxBlaster, 1, ftFoxBulletPool>&>(m_pools)
#define FT_FOX_POOL_BLASTER FT_FOX_POOL_BLASTER_BASE.getSub()
#define FT_FOX_POOL_BULLET_BASE static_cast<wnArticlePool<wnFoxBlasterBullet, 5, ftFoxIllusionPool>&>(m_pools)
#define FT_FOX_POOL_BULLET FT_FOX_POOL_BULLET_BASE.getSub()
#define FT_FOX_POOL_ILLUSION_BASE static_cast<wnArticlePool<wnFoxIllusion, 2, ftFoxLandMasterPool>&>(m_pools)
#define FT_FOX_POOL_ILLUSION FT_FOX_POOL_ILLUSION_BASE.getSub()
#define FT_FOX_POOL_LAND_MASTER_BASE static_cast<wnArticlePool<wnFoxLandMaster, 1, soInstancePoolRoot>&>(m_pools)
#define FT_FOX_POOL_LAND_MASTER FT_FOX_POOL_LAND_MASTER_BASE.getSub()

#define FT_FOX_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

template <class W> struct ftFoxArticleActivator;
template <> struct ftFoxArticleActivator<wnFoxBlaster> {
    static bool activate(wnFoxBlaster* weapon, soModuleAccesser* acc) { return ftFox::activeArticle(weapon, acc); }
};
template <> struct ftFoxArticleActivator<wnFoxBlasterBullet> {
    static bool activate(wnFoxBlasterBullet* weapon, soModuleAccesser* acc) {
        u32 etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
        return ftFoxTransactor::getInstance()->activeArticle2(weapon, acc, 0x3f, etcResId, 0x7000A);
    }
};
template <> struct ftFoxArticleActivator<wnFoxIllusion> {
    static bool activate(wnFoxIllusion* weapon, soModuleAccesser* acc) { return ftFox::activeArticle1(weapon, acc); }
};
template <> struct ftFoxArticleActivator<wnFoxLandMaster> {
    static bool activate(wnFoxLandMaster* weapon, soModuleAccesser* acc) { return ftFox::activeArticle2(weapon, acc); }
};

void ftFoxArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnFoxBlaster* weapon = FT_FOX_POOL_BLASTER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 5; ++i) {
        wnFoxBlasterBullet* weapon = FT_FOX_POOL_BULLET.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnFoxIllusion* weapon = FT_FOX_POOL_ILLUSION.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnFoxLandMaster* weapon = FT_FOX_POOL_LAND_MASTER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftFoxArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 1; }
    case 1: { wnArticleSlotTag tag; return 5; }
    case 2: { wnArticleSlotTag tag; return 2; }
    case 3: { wnArticleSlotTag tag; return 1; }
    FT_FOX_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftFoxActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_FOX_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftFoxActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftFoxActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftFoxArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_FOX_COUNT_ACTIVE(FT_FOX_POOL_BLASTER, 1)
    case 1: FT_FOX_COUNT_ACTIVE(FT_FOX_POOL_BULLET, 5)
    case 2: FT_FOX_COUNT_ACTIVE(FT_FOX_POOL_ILLUSION, 2)
    case 3: FT_FOX_COUNT_ACTIVE(FT_FOX_POOL_LAND_MASTER, 1)
    FT_FOX_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_FOX_CAN_GENERATE(POOL, N, ID) \
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

bool ftFoxArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_FOX_CAN_GENERATE(FT_FOX_POOL_BLASTER, 1, 0)
    case 1: FT_FOX_CAN_GENERATE(FT_FOX_POOL_BULLET, 5, 1)
    case 2: FT_FOX_CAN_GENERATE(FT_FOX_POOL_ILLUSION, 2, 2)
    case 3: FT_FOX_CAN_GENERATE(FT_FOX_POOL_LAND_MASTER, 1, 3)
    FT_FOX_UNUSED_SLOTS(false)
    default: return false;
    }
}

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_FOX_GENERATE(W, POOL) \
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
        if (ftFoxArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftFoxArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_FOX_GENERATE(wnFoxBlaster, FT_FOX_POOL_BLASTER)
    case 1: FT_FOX_GENERATE(wnFoxBlasterBullet, FT_FOX_POOL_BULLET)
    case 2: FT_FOX_GENERATE(wnFoxIllusion, FT_FOX_POOL_ILLUSION)
    case 3: FT_FOX_GENERATE(wnFoxLandMaster, FT_FOX_POOL_LAND_MASTER)
    FT_FOX_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Shooting only type-checks the article; the unused slots accept anything.
bool ftFoxArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; (void)dynamic_cast<wnFoxBlaster&>(*article); return true; }
    case 1: { wnArticleSlotTag tag; (void)dynamic_cast<wnFoxBlasterBullet&>(*article); return true; }
    case 2: { wnArticleSlotTag tag; (void)dynamic_cast<wnFoxIllusion&>(*article); return true; }
    case 3: { wnArticleSlotTag tag; (void)dynamic_cast<wnFoxLandMaster&>(*article); return true; }
    FT_FOX_UNUSED_SLOTS(true)
    default: return false;
    }
}

// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
#pragma dont_inline on
ftFoxTransactor* ftFoxTransactor::getInstance() {
    static ftFoxTransactor instance;
    return &instance;
}
#pragma dont_inline off
#pragma pool_data reset

// ---- the activators of the articles ---------------------------------------------------------------------------------

// The blaster article starts through the transactor with the model and the texture resource of the fighter.
bool ftFox::activeArticle(wnFoxBlaster* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    u32 texResId = resources.getResourceIdAccesser()->getTexResId();
    u32 mdlResId = resources.getResourceIdAccesser()->getMdlResId();
    return ftFoxTransactor::getInstance()->activeArticle1(weapon, acc, mdlResId, texResId, true, false);
}

// The Phantasm afterimage: it starts with the situation and facing of the fighter and a flag that tells it whether an earlier
// afterimage already ran (the work int 0x20000003 counts them).
bool ftFox::activeArticle1(wnFoxIllusion* weapon, soModuleAccesser* acc) {
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
bool ftFox::activeArticle2(wnFoxLandMaster* weapon, soModuleAccesser* acc) {
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

// FIXME: Test code present only to emit the shared builder functions; delete once ftFox is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftFoxInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftFoxInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtual overrides and default observers.
void fn_97_B000() {}
void fn_97_B004() {}
void fn_97_B008() {}
void fn_97_B00C() {}
void fn_97_B010() {}
void fn_97_B014() {}
void fn_97_B018() {}
void fn_97_B01C() {}
void fn_97_B020() {}
void fn_97_B024() {}
void fn_97_B028() {}
void fn_97_B02C() {}
void fn_97_B038() {}
void fn_97_B03C() {}
void fn_97_B040() {}
void fn_97_B044() {}
void fn_97_B070() {}
void fn_97_B074() {}
void fn_97_B078() {}
void fn_97_B07C() {}
void fn_97_B080() {}
void fn_97_B0A4() {}
void fn_97_B0A8() {}
void fn_97_B0AC() {}
void fn_97_B0B0() {}
void fn_97_B0C4() {}
void fn_97_B0C8() {}
void fn_97_B0CC() {}
void fn_97_B0D0() {}
void fn_97_B0D4() {}
void fn_97_B250() {}
void fn_97_B25C() {}
void fn_97_B268() {}
void fn_97_B26C() {}
void fn_97_B270() {}
void fn_97_B274() {}
void fn_97_B278() {}
void fn_97_D784() {}
void fn_97_DE94() {}
void fn_97_DF18() {}
void fn_97_E000() {}
void fn_97_E0E8() {}
void fn_97_E1D0() {}
void fn_97_E2B8() {}
void fn_97_E3A0() {}
void fn_97_E488() {}
void fn_97_E570() {}

// Constant and field accessors.
int fn_97_B778() { return g_soValueVariation; }
int fn_97_AE20(void*, s8 x) { return x == 12; }
u8 fn_97_8138(u8* p) { return *(u8*)(p + 0x4); }
u8 fn_97_AEEC(u8* p) { return *(u8*)(p + 0x44); }
int fn_97_B084(u8* p) { return *(int*)(p + 0x110); }
u8* fn_97_B27C(u8* p) { return *(u8**)(p + 0x20); }
u8* fn_97_B358(u8* p) { return *(u8**)(p + 0x18); }
u8* fn_97_B198(u8* p) { return p + 0x458; }
u8* fn_97_B1A0(u8* p) { return p + 0x3C8; }
u8* fn_97_B1A8(u8* p) { return p + 0x8; }
u8* fn_97_C904(u8* p) { return p + 0x1C794; }
u8* fn_97_C924(u8* p) { return p + 0x1C7D0; }
u8* fn_97_C930(u8* p) { return p + 0x1CC88; }
void fn_97_C8F8(u8* p, u8 v) { *(u8*)(p + 0x14C14) = v; }
void fn_97_D86C(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_97_D878(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_97_D884(float* p) {
    p[0] = 0.0f;
    p[1] = 0.0f;
    p[2] = 0.0f;
}

// Sub-object pointer selectors by index (null for unknown indices).
u8* fn_97_C998(u8* p, int x) {
    if (x == 1) {
        return p + 0x1DF8;
    }
    if (x == 0) {
        return p + 0x10;
    }
    return 0;
}
u8* fn_97_C9C0(u8* p, int x) {
    if (x == 4) {
        return p + 0x860C;
    }
    if (x == 3) {
        return p + 0x6490;
    }
    if (x == 2) {
        return p + 0x4314;
    }
    if (x == 1) {
        return p + 0x2198;
    }
    if (x == 0) {
        return p + 0x1C;
    }
    return 0;
}
u8* fn_97_E1D4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_97_E2BC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }

// Bit flags in the u32 array at +0x1C.
bool fn_97_B2F8(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_97_B314(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_97_B32C(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_97_B340(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_97_B360(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
void fn_97_B388(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_97_B3A0(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_97_B3B8(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_97_B3D0(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_97_B3E0(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_97_B3F8(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_97_B410(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_97_B428(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_97_B448(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_97_B460(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_97_B478(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_97_B490(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }

// Polymorphic view used only to emit virtual calls; slot k lives at vtable offset 8 + 4 * k.
struct ftFoxVirtualSlots {
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
    virtual void slot21();
    virtual void slot22();
};

void fn_97_C970(void* p) { return ((ftFoxVirtualSlots*)p)->slot20(); }
void fn_97_C910(u8* p) { return ((ftFoxVirtualSlots*)(p + 0x1C794))->slot2(); }

}

#pragma dont_inline on
soParamAccesser::~soParamAccesser() { }
#pragma dont_inline off
#pragma dont_inline on
soTeam::~soTeam() { }
#pragma dont_inline off
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
// Article specialization also declared by the weapon-builder header, which
// cannot coexist with this Fighter-based builder translation unit.
template <>
class soParamCustomizeModuleBuilder<soParamCustomizeModuleBuildConfig<soParamCustomizeModuleImpl> > {
    soParamCustomizeModuleImpl m_paramCustomizeModule;
public:
    soParamCustomizeModuleBuilder(soModuleAccesser*) { }
    ~soParamCustomizeModuleBuilder();
};
soParamCustomizeModuleBuilder<soParamCustomizeModuleBuildConfig<soParamCustomizeModuleImpl> >::~soParamCustomizeModuleBuilder() { }
soArticleMediator::~soArticleMediator() { }
