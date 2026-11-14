#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_GANON_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define WN_PARAM_ACCESSER_INLINE_DTOR
#define FT_TEAM_TYPED_INTERFACE
#define SO_CATCH_MODULE_CLING_SLOTS
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#include <ft/builder/ft_dol_array_list.h>
// The beast's attack-part vector: its teardown is sora_melee's copy.
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 4);
#include <ft/ft_class_info_impl.h>
#include <ft/ganon/ft_ganon.h>
#include <ft/ganon/ft_ganon_extend_param_accesser.h>
#include <ft/ganon/ft_ganon_status_uniq_process_ganon_kick.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_owner.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/article/so_article_mediator.h>
#include <so/catch/so_catch_module.h>
#include <so/so_value_accesser.h>

#undef WN_ARTICLE_NATIVE_BUILDERS
#define FT_BC ftGanonBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// The beast teardown expands the observer teardown here and calls soEventObserver<soDamageEventObserver>'s
// directly (as Pit's and Nikita's units do).
inline soDamageEventObserver::~soDamageEventObserver() { }
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

ftGanonExtendParamAccesser g_ftGanonExtendParamAccesser;

ftClassInfoImpl<Fighter_Ganon, ftGanon> g_ftClassInfoGanon;

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt); the two Warlock Punch
// entries live in module 27 (sora_melee) because Kirby copies the move.
extern "C" {
extern soStatusUniqProcess lbl_27_bss_8004;
extern soStatusUniqProcess lbl_27_bss_8014;
extern soStatusUniqProcess lbl_109_bss_144;
extern soStatusUniqProcess lbl_109_bss_154;
extern soStatusUniqProcess lbl_109_bss_164;
extern soStatusUniqProcess lbl_109_bss_174;
extern soStatusUniqProcess lbl_109_bss_184;
extern soStatusUniqProcess lbl_109_bss_194;
extern soStatusUniqProcess lbl_109_bss_1A4;
extern soStatusUniqProcess lbl_109_bss_1D4;
extern soStatusUniqProcess lbl_109_bss_1E4;
extern soStatusUniqProcess lbl_109_bss_1F4;
}

ftGanon::ftGanon(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftGanonBuildConfig>(entryId,
                                         Fighter_Ganon,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Ganon)) {
    // Register the status processes in action order; entries 2 and 12 have no process.
    soStatusUniqProcess* processes[16] = {0};
    processes[0] = &lbl_27_bss_8004;
    processes[1] = &lbl_109_bss_164;
    processes[3] = &g_ftGanonStatusUniqProcessGanonKick;
    processes[4] = &lbl_109_bss_1D4;
    processes[5] = &lbl_27_bss_8014;
    processes[6] = &lbl_109_bss_174;
    processes[7] = &lbl_109_bss_184;
    processes[8] = &lbl_109_bss_194;
    processes[9] = &lbl_109_bss_1A4;
    processes[10] = &lbl_109_bss_144;
    processes[11] = &lbl_109_bss_154;
    processes[13] = &g_ftGanonStatusUniqProcessGanonKickEnd;
    processes[14] = &lbl_109_bss_1E4;
    processes[15] = &lbl_109_bss_1F4;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 16);
}

ftGanon::~ftGanon() { }

void ftGanon::notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x115:
        g_ftGanonStatusUniqProcessGanonKick.onHit(moduleAccesser);
        break;
    }
}

// The Final Smash status process (ft_ganon_status_uniq_process_final.cpp) ends the transformation on this event; only
// its static hook is needed here.
class ftGanonStatusUniqProcessFinalAttack {
public:
    static void check_final_end(soLinkEventArgs* event, soModuleAccesser* moduleAccesser, StageObject* sender, int unk);
};

// HYPOTHESIS: the catch request (event 8) of a fighter Ganondorf grabs with the Flame Choke (statuses 0x113/0x114): the
// receiver marks it done and, for requests 0xe7/0xe8, answers with the node it is held by.
struct ftGanonLinkEventCatch : soLinkEventArgs {
    int m_nodeId;    // +0x8
    int m_request;   // +0xC
    int unk10;
    int unk14;
    int unk18;
    bool unk1C;
};

// HYPOTHESIS: the cut event (0xe) of the cling; unk5 set means the victim was released without an end status.
struct ftGanonLinkEventCut : soLinkEventArgs {
    bool unk5;
};

void ftGanon::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    switch (eventInfo->m_eventKind) {
    case 4:
        ftGanonStatusUniqProcessFinalAttack::check_final_end(eventInfo, moduleAccesser, stageObj, unk4);
        break;
    case 8: {
        ftGanonLinkEventCatch* event = static_cast<ftGanonLinkEventCatch*>(eventInfo);
        if (event->m_request == 0xe6 && status == 0x114) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x11c, moduleAccesser);
            event->_0x4 = true;
            event->unk1C = false;
            static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->setCatch(stageObj->m_taskId);
            return;
        }
        if (status == 0x113 && event->m_request == 0xe7) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x118, moduleAccesser);
            event->_0x4 = true;
            event->m_nodeId = moduleAccesser->getModelModule().getCorrectNodeId(0x12c);
            return;
        }
        if (status == 0x113 && event->m_request == 0xe8) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x119, moduleAccesser);
            event->_0x4 = true;
            event->m_nodeId = moduleAccesser->getModelModule().getCorrectNodeId(0x12c);
            return;
        }
        break;
    }
    case 0xe:
        switch (status) {
        case 0x11c:
        case 0x11d: {
            soCatchModule* catchModule = static_cast<soCatchModule*>(m_moduleAccesser->m_enumerationStart->m_catchModule);
            catchModule->setSendCutEvent(false);
            static_cast<soCatchModule*>(m_moduleAccesser->m_enumerationStart->m_catchModule)->clingCut(false);
            if (!static_cast<ftGanonLinkEventCut*>(eventInfo)->unk5) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x3b, m_moduleAccesser);
            }
            return;
        }
        }
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

// The beast of the Final Smash starts where the fighter stands, facing its way, on the team of its owner.
bool ftGanon::activeArticle(wnGanonBeast* weapon, soModuleAccesser* acc) {
    soPostureModule& posture = acc->getPostureModule();
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().getTaskId();
    float lr = posture.getLr();
    Vec3f pos = posture.getPos();
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(),
        fighter.getOwner()->getTeam(), &pos, lr);
    acc->getWorkManageModule().setInt(weapon->getTaskId(), 0x20000000);
    return true;
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Ganon, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftGanonArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftGanonArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftGanonArticleKind : wnArticleKindInfo {
    ftGanonArticleKind() {
        kind = 0;
        subKind = Fighter_Ganon;
    }
    s32 getSubKind() const { return subKind; } // HYPOTHESIS: accessor spelling (the lookup reads the kind through a call, as in ft_captain.cpp)
};

// The descriptor keeps the address of the kind temporary of the caller.
struct ftGanonArticleBuildInfo : wnArticleBuildInfo {
    ftGanonArticleBuildInfo(const wnArticleKindInfo& kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The beast is built from the data of the fighter kind.
template <>
inline wnArticleHolder<wnGanonBeast>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(0, ftGanonArticleBuildInfo(ftGanonArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)),
               g_ftCommonDataAccesser.getWeaponData<wnGanonBeastData>(ftGanonArticleKind().getSubKind(), &ftGanonArticleDataTag().m_resourceGroup)) { }

// The simple article (the shared generic article) is built from the simple data of the fighter kind.
struct ftGanonSimpleKind : wnSimpleKindInfo {
    ftGanonSimpleKind() : wnSimpleKindInfo(Fighter_Ganon) { }
    s32 getSubKind() const { return subKind; } // HYPOTHESIS: accessor spelling, as the article kind's
};
template <>
inline wnArticleHolder<wnSimple>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(1, wnSimpleConstructionInfo(wnSimpleKindInfo(Fighter_Ganon), acc->m_enumerationStart->m_heapModule),
               g_ftCommonDataAccesser.getSimpleData(static_cast<ftKind>(ftGanonSimpleKind().getSubKind()),
                                                    &ftGanonArticleDataTag().m_resourceGroup), false) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnGanonBeast>::~wnArticleHolder();
template wnArticleHolder<wnSimple>::~wnArticleHolder();

template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftGanonArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftGanonArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftGanonArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftGanonArticleMediator::~ftGanonArticleMediator() { }
#pragma dont_inline off

s32 ftGanonArticleMediator::getMediateNum() { return 2; }
void ftGanonArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Beast, 1 Simple (the rest are unused).
#define FT_GANON_POOL_BEAST_BASE static_cast<wnArticlePool<wnGanonBeast, 1, ftGanonSimplePool>&>(m_pools)
#define FT_GANON_POOL_BEAST FT_GANON_POOL_BEAST_BASE.getSub()
#define FT_GANON_POOL_SIMPLE_BASE static_cast<wnArticlePool<wnSimple, 1, soInstancePoolRoot>&>(m_pools)
#define FT_GANON_POOL_SIMPLE FT_GANON_POOL_SIMPLE_BASE.getSub()
#define FT_GANON_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) WN_ARTICLE_UNUSED_SLOT(4, value) \
    WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) \
    WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) \
    WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) \
    WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftGanonArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnGanonBeast* weapon = FT_GANON_POOL_BEAST.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSimple* weapon = FT_GANON_POOL_SIMPLE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftGanonArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    FT_GANON_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftGanonActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftGanonArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnGanonBeast, 1, ftGanonSimplePool>, ftGanonActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnSimple, 1, soInstancePoolRoot>, ftGanonActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_GANON_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftGanonArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnGanonBeast, 1, ftGanonSimplePool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnSimple, 1, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_GANON_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftGanonArticleActivator {
    static bool activate(W* weapon, soModuleAccesser* acc) { return ftGanon::activeArticle(weapon, acc); }
};

soArticle* ftGanonArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftGanonArticleActivator<wnGanonBeast> >(static_cast<wnArticlePool<wnGanonBeast, 1, ftGanonSimplePool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftGanonArticleActivator<wnSimple> >(static_cast<wnArticlePool<wnSimple, 1, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_GANON_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftGanonArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnGanonBeast>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotShoot<wnSimple>(article, WN_ARTICLE_SLOT_TAGS);
    FT_GANON_UNUSED_SLOTS(true)
    default: return false;
    }
}


// HYPOTHESIS: extend param group 3 (ftData + 0x88) holds the Wizard's Foot parameters.
float* ftGanon::getGanonKickParam() { return static_cast<float*>(g_ftCommonDataAccesser.getData(Fighter_Ganon)->extendParam[3]); }

#pragma dont_inline on
soTeam::~soTeam() { }
#pragma dont_inline off
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
soArticleMediator::~soArticleMediator() { }

bool ftGanon::activeArticle(wnSimple* weapon, soModuleAccesser* acc) {
    int founderTaskId;
    soResourceModule& resources = acc->getResourceModule();
    founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(),
        static_cast<const wnSimpleData*>(g_ftCommonDataAccesser.getData(Fighter_Ganon)->extendParam[5]),
        true, false, Vec3f(0.0f, 0.0f, 0.0f), 0xffff, 0xffff, 0, true, false, -1, 0, false, 2, 0, false);
    return true;
}

// The simple article's teardown is a copy in each fighter that pools one (as in ft_purin.cpp and ft_snake.cpp).
wnSimple::~wnSimple() { }
