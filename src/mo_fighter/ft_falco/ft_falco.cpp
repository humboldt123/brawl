#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_FALCO_ARTICLE_POOLS
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_FROM_VEC3_CTOR_NOINLINE
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/falco/ft_falco.h>
#include <ft/falco/ft_falco_extend_param_accesser.h>
#include <ft/falco/ft_falco_transactor.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/ft_reflector_group.h>
#include <ft/fox/ft_fox_status_uniq_process.h>
#include <so/article/so_article_mediator.h>
#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <so/team/so_team.h>
#include <so/param/so_param_customize_module_impl.h>

#define FT_BC ftFalcoBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

ftFalcoExtendParamAccesser g_ftFalcoExtendParamAccesser;

ftClassInfoImpl<Fighter_Falco, ftFalco> g_ftClassInfoFalco;

// MATCH-ONLY: the trail's constructor is called out of line (the copy sora_melee owns).
#pragma dont_inline on
template soArrayVector<ftFalco::PostureInfo, 4>::soArrayVector(s32);
#pragma dont_inline off

ftFalco::ftFalco(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftFalcoBuildConfig>(entryId,
                                         Fighter_Falco,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_commonData(g_ftCommonDataAccesser.getData(Fighter_Falco)),
    unk1D090(0),
    unk1D091(0) {
    // Falco shares Fox's special move processes (blaster, Phantasm, Fire Bird and the Landmaster); the reflector is set up
    // by its own unit.
    soStatusUniqProcess* processes[11] = {0};
    processes[0] = &g_ftFoxStatusUniqProcessBlaster;
    processes[1] = &g_ftFoxStatusUniqProcessIllusion;
    processes[2] = &g_ftFoxStatusUniqProcessFire;
    processes[4] = &g_ftFoxStatusUniqProcessFinal;
    processes[5] = &g_ftFoxStatusUniqProcessFire;
    processes[6] = &g_ftFoxStatusUniqProcessFire;
    processes[7] = &g_ftFoxStatusUniqProcessFire;
    processes[8] = &g_ftFoxStatusUniqProcessFinal;
    processes[9] = &g_ftFoxStatusUniqProcessFinal;
    processes[10] = &g_ftFoxStatusUniqProcessFinal;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 11);

    // The reflector (the shine) is built from the record at +0x90 of the common data.
    u8 reflectorShape[0x20];
    ftReflectorGroupDraft reflectorGroup;
    void* reflectorRecord = *reinterpret_cast<void**>(reinterpret_cast<u8*>(m_commonData) + 0x90);
    FT_REFLECTOR_GROUP_CLEAR(reflectorGroup);
    fn_27_195B34(reflectorRecord, reflectorShape, &reflectorGroup);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&reflectorGroup), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
}

ftFalco::~ftFalco() { }

void ftFalco::processUpdate() {
    Fighter::processUpdate();
    unk1D091 = 0;
    unk1D090 = 0;
}

// The trail of positions is emptied when a stock starts.
void ftFalco::onStart(int param) {
    m_postureTrail.getArray()->clear();
    Fighter::onStart(param);
}

// While the Phantasm is running (status 0x113) the fighter records its position and rotation every frame (not during a hit pause
// or slow motion); the afterimage articles ask for the samples with link event 0x455.
void ftFalco::processFixPosition() {
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
struct ftFalcoLinkEventView {
    int kind;
    u8 flag;
    u8 pad[3];
    u32 value;
    Vec3f pos;
    float rotX;
};

// The part of the event that the rumble case copies (the base arguments and the strength class).
struct ftFalcoLinkEventRumble : soLinkEventArgs {
    u32 value;
};

void ftFalco::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftFalcoLinkEventView* event = reinterpret_cast<ftFalcoLinkEventView*>(eventInfo);
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
            case 0x11a:
            case 0x11b:
                moduleAccesser->getStatusModule().changeStatusRequest(0x11c, moduleAccesser);
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
            ftFalcoLinkEventRumble copy = *reinterpret_cast<ftFalcoLinkEventRumble*>(event);
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

// The reflector (the shine) keeps two flags for the frame: the hit was too weak for it (unk1D090) or it stopped it (unk1D091).
void ftFalco::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if ((s32)groupIndex == 2) {
        if (power >= m_moduleAccesser->getCollisionReflectorModule().getAttackLimit(groupIndex)) {
            unk1D091 = 1;
            return;
        }
        unk1D090 = 1;
    }
    Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

bool ftFalco::notifyEventCollisionReflectorCheck() {
    if (unk1D090 == 1) {
        // The hit was too weak for the reflector: the shine keeps a flash on the fighter.
        soEffectModule& effect = m_moduleAccesser->getEffectModule();
        effect.reqFollow(static_cast<EfID>(0x14000C), 0x12, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 0, 0, -1);
        return true;
    }
    if (unk1D091 == 1) {
        // The reflector stopped the hit: its effects end and the reflector is rearmed.
        m_moduleAccesser->getEffectModule().killAll(1, true, true);
        m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    }
    return Fighter::notifyEventCollisionReflectorCheck();
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Falco, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftFalcoArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftFalcoArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftFalcoArticleKind : wnArticleKindInfo {
    ftFalcoArticleKind() {
        kind = 0;
        subKind = Fighter_Falco;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftFalcoArticleBuildInfo : wnArticleBuildInfo {
    ftFalcoArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (the numbers of its getters are their
// order in the module; only declared here).
extern "C" void* fn_27_1484BC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14858C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148620(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1486B4(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftFalcoArticleTraits;
#define FT_FALCO_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftFalcoArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftFalcoArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Falco, &tag.m_resourceGroup)); \
        } \
    };

FT_FALCO_TRAITS(wnFalcoBlaster, wnFalcoBlasterData, 0, fn_27_14858C)
FT_FALCO_TRAITS(wnFalcoBlasterBullet, wnFalcoBlasterBulletData, 1, fn_27_148620)
FT_FALCO_TRAITS(wnFalcoIllusion, wnFalcoIllusionData, 2, fn_27_1486B4)
FT_FALCO_TRAITS(wnFoxLandMaster, wnFoxLandMasterData, 3, fn_27_1484BC)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftFalcoArticleTraits<W>::ArticleId, ftFalcoArticleBuildInfo(ftFalcoArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftFalcoArticleTraits<W>::getData()) { }

// ---- the articles of Falco and their mediator -----------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnFalcoBlaster>::~wnArticleHolder();
template wnArticleHolder<wnFalcoBlasterBullet>::~wnArticleHolder();
template wnArticleHolder<wnFalcoIllusion>::~wnArticleHolder();
template wnArticleHolder<wnFoxLandMaster>::~wnArticleHolder();
template wnArticleHolder<wnFalcoBlasterBullet>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnFalcoIllusion>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftFalcoArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftFalcoArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftFalcoArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftFalcoArticleMediator::~ftFalcoArticleMediator() { }
#pragma dont_inline off

s32 ftFalcoArticleMediator::getMediateNum() { return 4; }
void ftFalcoArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 blaster, 1 blaster bullet, 2 Phantasm afterimage, 3 Landmaster (the rest are unused).

#define FT_FALCO_POOL_BLASTER_BASE static_cast<wnArticlePool<wnFalcoBlaster, 1, ftFalcoBulletPool>&>(m_pools)
#define FT_FALCO_POOL_BLASTER FT_FALCO_POOL_BLASTER_BASE.getSub()
#define FT_FALCO_POOL_BULLET_BASE static_cast<wnArticlePool<wnFalcoBlasterBullet, 5, ftFalcoIllusionPool>&>(m_pools)
#define FT_FALCO_POOL_BULLET FT_FALCO_POOL_BULLET_BASE.getSub()
#define FT_FALCO_POOL_ILLUSION_BASE static_cast<wnArticlePool<wnFalcoIllusion, 2, ftFalcoLandMasterPool>&>(m_pools)
#define FT_FALCO_POOL_ILLUSION FT_FALCO_POOL_ILLUSION_BASE.getSub()
#define FT_FALCO_POOL_LAND_MASTER_BASE static_cast<wnArticlePool<wnFoxLandMaster, 1, soInstancePoolRoot>&>(m_pools)
#define FT_FALCO_POOL_LAND_MASTER FT_FALCO_POOL_LAND_MASTER_BASE.getSub()

#define FT_FALCO_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

template <class W> struct ftFalcoArticleActivator;
template <> struct ftFalcoArticleActivator<wnFalcoBlaster> {
    static bool activate(wnFalcoBlaster* weapon, soModuleAccesser* acc) { return ftFalco::activeArticle(weapon, acc); }
};
template <> struct ftFalcoArticleActivator<wnFalcoBlasterBullet> {
    static bool activate(wnFalcoBlasterBullet* weapon, soModuleAccesser* acc) {
        u32 etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
        return ftFalcoTransactor::getInstance()->activeArticle2(weapon, acc, 0x41, etcResId, 0x14000A);
    }
};
template <> struct ftFalcoArticleActivator<wnFalcoIllusion> {
    static bool activate(wnFalcoIllusion* weapon, soModuleAccesser* acc) { return ftFalco::activeArticle1(weapon, acc); }
};
template <> struct ftFalcoArticleActivator<wnFoxLandMaster> {
    static bool activate(wnFoxLandMaster* weapon, soModuleAccesser* acc) { return ftFalco::activeArticle2(weapon, acc); }
};

void ftFalcoArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnFalcoBlaster* weapon = FT_FALCO_POOL_BLASTER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 5; ++i) {
        wnFalcoBlasterBullet* weapon = FT_FALCO_POOL_BULLET.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnFalcoIllusion* weapon = FT_FALCO_POOL_ILLUSION.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnFoxLandMaster* weapon = FT_FALCO_POOL_LAND_MASTER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftFalcoArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 1; }
    case 1: { wnArticleSlotTag tag; return 5; }
    case 2: { wnArticleSlotTag tag; return 2; }
    case 3: { wnArticleSlotTag tag; return 1; }
    FT_FALCO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftFalcoActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_FALCO_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftFalcoActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftFalcoActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftFalcoArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_FALCO_COUNT_ACTIVE(FT_FALCO_POOL_BLASTER, 1)
    case 1: FT_FALCO_COUNT_ACTIVE(FT_FALCO_POOL_BULLET, 5)
    case 2: FT_FALCO_COUNT_ACTIVE(FT_FALCO_POOL_ILLUSION, 2)
    case 3: FT_FALCO_COUNT_ACTIVE(FT_FALCO_POOL_LAND_MASTER, 1)
    FT_FALCO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_FALCO_CAN_GENERATE(POOL, N, ID) \
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

bool ftFalcoArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_FALCO_CAN_GENERATE(FT_FALCO_POOL_BLASTER, 1, 0)
    case 1: FT_FALCO_CAN_GENERATE(FT_FALCO_POOL_BULLET, 5, 1)
    case 2: FT_FALCO_CAN_GENERATE(FT_FALCO_POOL_ILLUSION, 2, 2)
    case 3: FT_FALCO_CAN_GENERATE(FT_FALCO_POOL_LAND_MASTER, 1, 3)
    FT_FALCO_UNUSED_SLOTS(false)
    default: return false;
    }
}

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_FALCO_GENERATE(W, POOL) \
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
        if (ftFalcoArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftFalcoArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_FALCO_GENERATE(wnFalcoBlaster, FT_FALCO_POOL_BLASTER)
    case 1: FT_FALCO_GENERATE(wnFalcoBlasterBullet, FT_FALCO_POOL_BULLET)
    case 2: FT_FALCO_GENERATE(wnFalcoIllusion, FT_FALCO_POOL_ILLUSION)
    case 3: FT_FALCO_GENERATE(wnFoxLandMaster, FT_FALCO_POOL_LAND_MASTER)
    FT_FALCO_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Shooting only type-checks the article; the unused slots accept anything.
bool ftFalcoArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; (void)dynamic_cast<wnFalcoBlaster&>(*article); return true; }
    case 1: { wnArticleSlotTag tag; (void)dynamic_cast<wnFalcoBlasterBullet&>(*article); return true; }
    case 2: { wnArticleSlotTag tag; (void)dynamic_cast<wnFalcoIllusion&>(*article); return true; }
    case 3: { wnArticleSlotTag tag; (void)dynamic_cast<wnFoxLandMaster&>(*article); return true; }
    FT_FALCO_UNUSED_SLOTS(true)
    default: return false;
    }
}

// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
#pragma dont_inline on
ftFalcoTransactor* ftFalcoTransactor::getInstance() {
    static ftFalcoTransactor instance;
    return &instance;
}
#pragma dont_inline off
#pragma pool_data reset

// ---- the activators of the articles ---------------------------------------------------------------------------------

// The blaster article starts through the transactor with the model and the texture resource of the fighter.
bool ftFalco::activeArticle(wnFalcoBlaster* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    u32 texResId = resources.getResourceIdAccesser()->getTexResId();
    u32 mdlResId = resources.getResourceIdAccesser()->getMdlResId();
    return ftFalcoTransactor::getInstance()->activeArticle1(weapon, acc, mdlResId, texResId, true, false);
}

// The Phantasm afterimage: it starts with the situation and facing of the fighter and a flag that tells it whether an earlier
// afterimage already ran (the work int 0x20000003 counts them).
bool ftFalco::activeArticle1(wnFalcoIllusion* weapon, soModuleAccesser* acc) {
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
bool ftFalco::activeArticle2(wnFoxLandMaster* weapon, soModuleAccesser* acc) {
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

// FIXME: Test code present only to emit the shared builder functions; delete once ftFalco is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftFalcoInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftFalcoInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Small functions of this translation unit (empty virtuals, constant returns, field accessors and work-area
// operations) under their symbol names from the REL map. The bodies are the leaf code the REL contains.
extern int g_soValueVariation;

extern "C" {

// Empty bodies.
void fn_108_AEA4() {}
void fn_108_AEA8() {}
void fn_108_AEAC() {}
void fn_108_AEB0() {}
void fn_108_AEB4() {}
void fn_108_AEB8() {}
void fn_108_AEBC() {}
void fn_108_AEC0() {}
void fn_108_AEC4() {}
void fn_108_AEC8() {}
void fn_108_AECC() {}
void fn_108_AED0() {}
void fn_108_AEDC() {}
void fn_108_AEE0() {}
void fn_108_AEE4() {}
void fn_108_AEE8() {}
void fn_108_AF14() {}
void fn_108_AF18() {}
void fn_108_AF1C() {}
void fn_108_AF20() {}
void fn_108_AF24() {}
void fn_108_AF48() {}
void fn_108_AF4C() {}
void fn_108_AF50() {}
void fn_108_AF54() {}
void fn_108_AF68() {}
void fn_108_AF6C() {}
void fn_108_AF70() {}
void fn_108_AF74() {}
void fn_108_AF78() {}
void fn_108_D5F4() {}
void fn_108_DD00() {}
void fn_108_DD84() {}
void fn_108_DE6C() {}
void fn_108_DF54() {}
void fn_108_E03C() {}
void fn_108_E124() {}
void fn_108_E20C() {}
void fn_108_E2F4() {}
void fn_108_E3DC() {}

// Field accessors.
u8 fn_108_8008(u8* p) { return *(u8*)(p + 0x4); }
u8 fn_108_AD90(u8* p) { return *(u8*)(p + 0x44); }
int fn_108_AF28(u8* p) { return *(int*)(p + 0x110); }
int fn_108_B0EC(u8* p) { return *(int*)(p + 0x20); }
int fn_108_B1C8(u8* p) { return *(int*)(p + 0x18); }
u8* fn_108_B03C(u8* p) { return p + 0x458; }
u8* fn_108_B044(u8* p) { return p + 0x3C8; }
u8* fn_108_B04C(u8* p) { return p + 0x8; }
int fn_108_B5E8() { return g_soValueVariation; }
void fn_108_D6DC(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_108_D6E8(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_108_C768(u8* p, u8 v) { *(u8*)(p + 0x14F84) = v; }
u8* fn_108_C774(u8* p) { return p + 0x1CAE0; }
u8* fn_108_C794(u8* p) { return p + 0x1CB1C; }
u8* fn_108_C7A0(u8* p) { return p + 0x1CFD4; }
u8* fn_108_C808(u8* p, s32 index) {
    if (index == 1) { return p + 0x1FB4; }
    if (index == 0) { return p + 0x10; }
    return NULL;
}
u8* fn_108_E040(u8* p, s32 index) {
    if (index == 0) { return p + 0xC; }
    return NULL;
}
u8* fn_108_E128(u8* p, s32 index) {
    if (index == 0) { return p + 0xC; }
    return NULL;
}

// Flag work area (array at +0x1C).
bool fn_108_B168(u8* p, u32 mask, u32 idx) { return (*(u32**)(p + 0x1C))[idx] & mask; }
void fn_108_B184(u8* p, u32 mask, u32 idx) { u32* flags = *(u32**)(p + 0x1C); flags[idx] &= ~mask; }
void fn_108_B19C(u8* p, u32 idx) { u32* flags = *(u32**)(p + 0x1C); flags[idx] = 0; }
void fn_108_B1B0(u8* p, u32 mask, u32 idx) { u32* flags = *(u32**)(p + 0x1C); flags[idx] |= mask; }

// Int work area (array at +0xC).
s32 fn_108_B268(u8* p, u32 idx) { s32* ints = *(s32**)(p + 0xC); s32 v = ints[idx]; ints[idx] = v - 1; return v; }
s32 fn_108_B280(u8* p, u32 idx) { s32* ints = *(s32**)(p + 0xC); s32 v = ints[idx]; ints[idx] = v + 1; return v; }
void fn_108_B298(u8* p, s32 divisor, u32 idx) { if (divisor == 0) { return; } s32* ints = *(s32**)(p + 0xC); ints[idx] = ints[idx] / divisor; }
void fn_108_B2B8(u8* p, s32 factor, u32 idx) { s32* ints = *(s32**)(p + 0xC); ints[idx] = ints[idx] * factor; }
void fn_108_B2D0(u8* p, s32 value, u32 idx) { s32* ints = *(s32**)(p + 0xC); ints[idx] = ints[idx] - value; }
void fn_108_B2E8(u8* p, s32 value, u32 idx) { s32* ints = *(s32**)(p + 0xC); ints[idx] = ints[idx] + value; }
void fn_108_B300(u8* p, s32 value, u32 idx) { s32* ints = *(s32**)(p + 0xC); ints[idx] = value; }

// Float work area (array at +0x14).
void fn_108_B1F8(u8* p, u32 idx, float factor) { float* floats = *(float**)(p + 0x14); floats[idx] *= factor; }
void fn_108_B210(u8* p, u32 idx, float value) { float* floats = *(float**)(p + 0x14); floats[idx] -= value; }
void fn_108_B228(u8* p, u32 idx, float value) { float* floats = *(float**)(p + 0x14); floats[idx] += value; }
void fn_108_B240(u8* p, u32 idx, float value) { float* floats = *(float**)(p + 0x14); floats[idx] = value; }
float fn_108_B250(u8* p, u32 idx) { float* floats = *(float**)(p + 0x14); return floats[idx]; }

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
