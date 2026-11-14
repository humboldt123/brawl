#define FT_KINETIC_MEDIATOR_POST_UPDATE // the mediator hands postUpdateEnergy to the transactor
#define SO_KINETIC_ROTATION_FIELDWISE_ZERO
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_CAPTAIN_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define WN_PARAM_ACCESSER_INLINE_DTOR
#define FT_TEAM_TYPED_INTERFACE
#include <ft/builder/ft_dol_array_list.h>
#include <wn/wn_extend_param_accesser.h>

// BYTES: the map names an out-of-line wnExtendParamAccesser<3999,1,23999,0,0,0>::~ (0x37BC) that both
// article destructors call; the generic template has no user-declared destructor, so this unit
// spells out the specialization the Captain articles share (same members as the primary template).
template <>
class wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> : public soParamAccesser {
    s32 m_kind;           // +0xC
    s32 m_floatBase;      // +0x10
    s32 m_intBase;        // +0x14
    s32 m_indefiniteBase; // +0x18
protected:
    const float* m_floats[2]; // +0x1C
    const s32* m_ints[1];
    void* m_indefinites[1];
public:
    wnExtendParamAccesser(s32 kind) : m_kind(kind), m_floatBase(3999), m_intBase(23999), m_indefiniteBase(0) { }
    virtual ~wnExtendParamAccesser() __attribute__((never_inline)) { }
    virtual float getParamFloat(soModuleAccesser*, s32 id, s32 index) {
        return m_floats[id - m_floatBase - 1][index];
    }
    virtual s32 getParamInt(soModuleAccesser*, s32 id, s32 index) {
        return m_ints[id - m_intBase - 1][index];
    }
    virtual void* getParamIndefinite(soModuleAccesser*, s32 id, s32) {
        return m_indefinites[id - m_indefiniteBase - 1];
    }
};

#include <ft/ft_class_info_impl.h>
#include <ft/captain/ft_captain.h>
#include <ft/captain/ft_captain_extend_param_accesser.h>
#include <ft/captain/ft_captain_status_uniq_process_falcon_kick.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <so/catch/so_catch_module.h>
#include <ft/ft_owner.h>

#undef WN_ARTICLE_NATIVE_BUILDERS
#define FT_BC ftCaptainBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// The articles' sound builder teardown inlines this empty destructor (same form as ft_pit.cpp and ft_snake.cpp).
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }

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

ftKineticEnergyController::~ftKineticEnergyController() { }

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

// The Final Smash processes (ft_captain_status_uniq_process_final.cpp); only the members called here are declared.
class ftCaptainStatusUniqProcessFinalStart : public soStatusUniqProcess {
public:
    void notifyEventCollisionAttackFighter(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    bool notifyEventCollisionAttackCheck(u32 flags, soModuleAccesser* moduleAccesser);
};
class ftCaptainStatusUniqProcessFinalMessageHandler : public soStatusUniqProcess {
public:
    static void linkMessageProcedure(soLinkEventArgs* message, soModuleAccesser* moduleAccesser, StageObject* sender, int unk);
    static void update(soModuleAccesser* moduleAccesser);
};

void ftCaptain::processUpdate() {
    soModuleAccesser* acc = m_moduleAccesser;
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x116:
    case 0x11d: case 0x11e: case 0x11f: case 0x120: case 0x121:
        ftCaptainStatusUniqProcessFinalMessageHandler::update(acc);
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
        reinterpret_cast<ftCaptainStatusUniqProcessFalconKick*>(&lbl_100_bss_1A4)->setEventCollisionAttack(moduleAccesser);
        break;
    case 0x116:
        reinterpret_cast<ftCaptainStatusUniqProcessFinalStart*>(&lbl_100_bss_1F4)->notifyEventCollisionAttackFighter(power, collisionLog, moduleAccesser);
        break;
    }
}

bool ftCaptain::notifyEventCollisionAttackCheck(u32 flags) {
    soModuleAccesser* acc = m_moduleAccesser;
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x116:
        return reinterpret_cast<ftCaptainStatusUniqProcessFinalStart*>(&lbl_100_bss_1F4)->notifyEventCollisionAttackCheck(flags, acc);
    default:
        return Fighter::notifyEventCollisionAttackCheck(flags);
    }
}

// The Falcon of the Final Smash starts at the position of the fighter (or at the origin in the statuses 0x11f / 0x120).
// HYPOTHESIS: the catch module (soModuleEnumeration::m_catchModule) has virtual slots past the ones
// declared in so_catch_module.h; these are the ones the link events below call, names unknown.
class ftCaptainCatchModule : public soCatchModule {
public:
    virtual void unk30();
    virtual void unk34();
    virtual void unk38(int);
    virtual bool unk3C(soModuleAccesser* moduleAccesser, void* data);
    virtual void unk40(int);
};

static inline ftCaptainCatchModule* getCatchModule(soModuleAccesser* moduleAccesser) {
    return static_cast<ftCaptainCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule);
}

// HYPOTHESIS: the arguments of the link events ftCaptain answers. Kind 8 asks to catch with the
// request id at +0xC (0xCF is the Raptor Boost/Falcon Dive grab); kind 0xE releases the catch.
struct ftCaptainLinkEventArgs : public soLinkEventArgs {
    bool unk5;
    int unk8;
    int m_request; // +0xC
    int unk10;
    int unk14;
    int unk18;
    bool unk1C;
};

void ftCaptain::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftCaptainLinkEventArgs* event = static_cast<ftCaptainLinkEventArgs*>(eventInfo);
    switch (eventInfo->m_eventKind) {
    case 8:
        if (event->m_request == 0xcf && m_moduleAccesser->getStatusModule().getStatusKind() == 0x114) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x119, m_moduleAccesser);
            event->_0x4 = true;
            event->unk1C = false;
            getCatchModule(m_moduleAccesser)->setCatch(stageObj->m_taskId);
            return;
        }
        break;
    case 0xe:
        switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
        case 0x119:
        case 0x11a:
            getCatchModule(m_moduleAccesser)->unk40(0);
            getCatchModule(m_moduleAccesser)->unk38(0);
            if (!event->unk5) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x3b, m_moduleAccesser);
            }
            return;
        }
        break;
    case 0x3d:
        switch (moduleAccesser->getStatusModule().getStatusKind()) {
        case 0x116:
        case 0x11d:
        case 0x11e:
        case 0x11f:
        case 0x120:
        case 0x121:
            ftCaptainStatusUniqProcessFinalMessageHandler::linkMessageProcedure(eventInfo, moduleAccesser, stageObj, unk4);
            break;
        }
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

// HYPOTHESIS: the article's task id goes through a small by-value accessor before it is stored in the work area (the
// original reads it ahead of the work module; a direct member read or gfTask::getTaskId() comes out after it).
static inline int getArticleTaskId(Weapon* weapon) {
    int taskId = weapon->m_taskId;
    return taskId;
}

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
    weapon->activate(founderTaskId, ids.getFinalResId(), acc->getTeamModule().getTeam()->getNo(), &pos, lr, attached, scale);
    int articleTaskId = getArticleTaskId(weapon);
    acc->getWorkManageModule().setInt(articleTaskId, 0x20000001);
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

// HYPOTHESIS: the glow module (soModuleEnumeration::m_glowModule) slot 0x50 takes a status kind and a
// frame count (Sonic's analyzeSeal calls the same slot); the earlier slots are not called here.
class ftCaptainGlowModule {
public:
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44();
    virtual void unk48();
    virtual void unk4C();
    virtual void setGlow(int statusKind, int frames);
};

// A sticker (seal) of kind 0x3E powers up the Falcon Punch (status 0x112) and the Raptor Boost (0x117).
void ftCaptain::analyzeSeal(void* seal) {
    ftOwnerSeal* record = static_cast<ftOwnerSeal*>(seal);
    if (record->m_kind == 0x3e) {
        int frames = (int)(record->m_value / 2.0f);
        static_cast<ftCaptainGlowModule*>(m_moduleAccesser->m_enumerationStart->m_glowModule)->setGlow(0x112, frames);
        frames = (int)(record->m_value / 2.0f);
        static_cast<ftCaptainGlowModule*>(m_moduleAccesser->m_enumerationStart->m_glowModule)->setGlow(0x117, frames);
    }
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
    s32 getSubKind() const { return subKind; } // HYPOTHESIS: accessor spelling (the lookup reads the kind through a call, as in ft_snake.cpp)
};

// The descriptor keeps the address of the kind temporary of the caller.
struct ftCaptainArticleBuildInfo : wnArticleBuildInfo {
    ftCaptainArticleBuildInfo(const wnArticleKindInfo& kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};


template <class W> struct ftCaptainArticleTraits;
#define FT_CAPTAIN_TRAITS(W, DATA, ID) \
    template <> struct ftCaptainArticleTraits<W> { \
        enum { ArticleId = ID }; \
        typedef DATA Data; \
    };

FT_CAPTAIN_TRAITS(wnCaptainBlueFalcon, wnCaptainBlueFalconData, 0)
FT_CAPTAIN_TRAITS(wnCaptainFinalBg, wnCaptainFinalBgData, 1)

// Every article of the fighter is built by this constructor from the data of the fighter kind; the data lookup is called
// in the argument list after the descriptor, with a second kind temporary (the holders are spliced into the builder).
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftCaptainArticleTraits<W>::ArticleId, ftCaptainArticleBuildInfo(ftCaptainArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), g_ftCommonDataAccesser.getWeaponData<typename ftCaptainArticleTraits<W>::Data>(ftCaptainArticleKind().getSubKind(), &ftCaptainArticleDataTag().m_resourceGroup)) { }

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
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
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

s32 ftCaptainArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnCaptainBlueFalcon, 1, ftCaptainFinalBgPool>, ftCaptainActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnCaptainFinalBg, 1, soInstancePoolRoot>, ftCaptainActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_CAPTAIN_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftCaptainArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnCaptainBlueFalcon, 1, ftCaptainFinalBgPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnCaptainFinalBg, 1, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
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

soArticle* ftCaptainArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftCaptainArticleActivator<wnCaptainBlueFalcon> >(static_cast<wnArticlePool<wnCaptainBlueFalcon, 1, ftCaptainFinalBgPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftCaptainArticleActivator<wnCaptainFinalBg> >(static_cast<wnArticlePool<wnCaptainFinalBg, 1, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_CAPTAIN_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftCaptainArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnCaptainBlueFalcon>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotShoot<wnCaptainFinalBg>(article, WN_ARTICLE_SLOT_TAGS);
    FT_CAPTAIN_UNUSED_SLOTS(true)
    default: return false;
    }
}



// HYPOTHESIS: extend param group 3 (ftData + 0x88) holds the Falcon Kick parameters.
float* ftCaptain::getFalconKickParam() { return static_cast<float*>(g_ftCommonDataAccesser.getData(Fighter_Captain)->extendParam[3]); }


#pragma dont_inline on
soTeam::~soTeam() { }
#pragma dont_inline off
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
soArticleMediator::~soArticleMediator() { }
