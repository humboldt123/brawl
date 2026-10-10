#define FT_FIGHTER_ANIMCMD_LONG
#define FT_KINETIC_MEDIATOR_MODE_ARG
#define FT_MARTH_RUNTIME_HELPERS
#define MT_VEC3F_CTOR_NOINLINE
#define WN_WEAPON_ANIMCMD_LONG
#define FT_TEAM_TYPED_INTERFACE
#include <so/so_instance_manager.h>
class soAnimCmdControlUnit;
// MATCH-ONLY: the native control-unit manager retains its empty base teardown
// call. Specialize this owner so kinetic managers keep their existing inlining.
template <>
class soInstanceManagerFullProperty<soAnimCmdControlUnit> : public soInstanceManager<soAnimCmdControlUnit>,
                                      public soInstanceManagerPriorityPolicy<soAnimCmdControlUnit>,
                                      public soInstanceManagerAttributePolicy<soAnimCmdControlUnit> {
public:
    // UBFIX: There should have been a virtual dtor in the base class
    ~soInstanceManagerFullProperty() __attribute__((never_inline)) { }
    virtual s32 add(soAnimCmdControlUnit& p1, s32 p2) {
        return add(p1, p2, soAttributeFlag(), -1);
    }

    virtual s32 add(soAnimCmdControlUnit&, s32, soAttributeFlag, s16) = 0;
    virtual u32 capacity() = 0;
    virtual soAnimCmdControlUnit& atIndexFast(s32 index) { return this->at(index); }
    virtual soInstanceUnitFullProperty<soAnimCmdControlUnit>& atUnitIndexFast(s32 index) = 0;
    virtual s32 getIndex(s32 index) const = 0;
};

#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_owner.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/damage/so_damage.h>
#include <ft/purin/ft_purin_status_uniq_process.h>
#include <ft/purin/ft_purin_status_uniq_process_special_n.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/situation/so_situation_event_presenter.h>
#include <so/so_heap_module_impl.h>
#include <so/status/so_status_event_presenter.h>
#include <so/so_null.h>
#include <StaticAssert.h>
#include <so/status/so_status_module_impl.h>
#include <ft/fighter.h>
#include <ft/ft_entry.h>
#include <ft/ft_extend_param_accesser.h>
#include <ft/ft_fighter_build_data.h>
#include <ft/purin/ft_purin_extend_param_accesser.h>
#include <so/so_array.h>
#include <so/event/so_event_manage_module_impl.h>
#include <so/event/so_event_system.h>
#include <so/so_inside_event_manage_module_builder.h>
#include <so/so_instance_unit.h>
#include <so/so_module_accesser.h>
#include <so/template_utils.h>
#include <sr/sr_common.h>
#include <types.h>

#include <ft/ft_cancel_module.h>
#include <ft/ft_param_customize_module_impl.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/ft_status_uniq_process_gimmick.h>
#include <ft/ft_virtual_node_matrix_pool.h>
#include <so/so_module_accesser_builder.h>
#include <ut/ut_uncopyable.h>

#include <ft/ft_fighter_builder.h>
#include <ft/purin/ft_purin_article.h>

////////////////////////////////////////
// Jigglypuff Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftPurinInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftPurinHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftPurinParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftPurinResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftPurinModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<288, 501> ftPurinAnimCmdModuleSubBuildConfig;
typedef soStatusModuleBuildConfig<288, soGeneralWorkBuildConfig<18, 18, 3>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftPurinStatusModuleBuildConfig;

typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftPurinCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftPurinLinkModuleBuildConfig;
typedef soPhysicsModuleBuildConfigCap<0, soPhysicsModuleImpl> ftPurinPhysicsModuleBuildConfig;
typedef ftPurinArticleManageModuleBuilder ftPurinGenerateArticleManageModuleBuilder;

class ftPurinBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftPurinGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
    typedef ftPurinCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftPurinLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftPurinPhysicsModuleBuildConfig PhysicsModuleBuildConfig;
    typedef ftPurinInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftPurinHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftPurinParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftPurinResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftPurinAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftPurinModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftPurinStatusModuleBuildConfig StatusModuleBuildConfig;
};

class ftPurin : public ftFighterBuilder<ftPurinBuildConfig> {

    // begin ftPurin fields
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
public:
    ftPurin(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual ~ftPurin();
    virtual void onStart(int);
    static bool activeArticle(wnSimple* article, soModuleAccesser* acc);
    virtual void notifyEventChangeSituation(SituationKind, SituationKind, soModuleAccesser*);
    virtual void notifyEventCollisionAttackFighter(soCollisionLog*, soModuleAccesser*);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventOnDamage(soDamage*, bool, soModuleAccesser*);
};
static_assert(sizeof(ftPurin) == 0x9A5C, "Class is the wrong size!");


// The owned status-change queue uses sora_melee's destructor.
#ifndef FT_REL_LINK_EXTERN
template <> soArrayVector<s32, 8>::~soArrayVector();
#endif

// The matrix pool owns the shared base teardown retained by the REL.
#pragma dont_inline on
ftVirtualNodeMatrixPool::~ftVirtualNodeMatrixPool() { }
soTeam::~soTeam() { }
#pragma dont_inline off

#define FT_BC ftPurinBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftPurinExtendParamAccesser g_ftPurinExtendParamAccesser;

ftClassInfoImpl<Fighter_Jigglypuff, ftPurin> g_ftClassInfoPurin;

ftPurin::ftPurin(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPurinBuildConfig>(entryId,
                                         Fighter_Jigglypuff,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Jigglypuff)) {
    // Register the fourteen character-specific status processes in action order.
    soStatusUniqProcess* processes[14];
    processes[0] = &g_ftPurinStatusUniqProcessSpecialNStart;
    processes[1] = &g_ftPurinStatusUniqProcessSpecialS;
    processes[2] = &g_ftPurinStatusUniqProcessSpecialHi;
    processes[3] = &g_ftPurinStatusUniqProcessSpecialLw;
    processes[4] = &g_ftPurinStatusUniqProcessFinal;
    processes[5] = &g_ftPurinStatusUniqProcessSpecialNHold;
    processes[6] = &g_ftPurinStatusUniqProcessSpecialNHoldMax;
    processes[7] = &g_ftPurinStatusUniqProcessSpecialNRoll;
    processes[8] = &g_ftPurinStatusUniqProcessSpecialNRollAir;
    processes[9] = &g_ftPurinStatusUniqProcessSpecialNTurn;
    processes[10] = &g_ftPurinStatusUniqProcessSpecialNEnd;
    processes[11] = &g_ftPurinStatusUniqProcessSpecialNHitEnd;
    processes[12] = &g_ftPurinStatusUniqProcessFinal;
    processes[13] = &g_ftPurinStatusUniqProcessFinal;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 14);
}

// The status module owns its change-request queue and both observer bases.
soStatusModuleImpl::~soStatusModuleImpl() { }
ftKineticEnergyController::~ftKineticEnergyController() { }

// ftManager selects this shared parameter variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

ftPurin::~ftPurin() { }

void ftPurin::onStart(int kind) {
    // Costume variant 2 replaces article zero when the fighter starts.
    if (static_cast<int>(getOwner()->getFighterColor()) == 2) {
        soGenerateArticleManageModule& articles =
            *static_cast<soGenerateArticleManageModule*>(m_moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
        articles.removeExist(0, 0);
        soArticle* article = static_cast<soGenerateArticleManageModule*>(
            m_moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->generate(0, NULL, NULL);
        if (!article->isNull()) {
            static_cast<soGenerateArticleManageModule*>(
                m_moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->entry(article);
        }
    }
    Fighter::onStart(kind);
}

void ftPurin::notifyEventChangeSituation(SituationKind kind, SituationKind previous, soModuleAccesser* acc) {
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x114:
        // Pound consumes this reset request in its aerial speed update.
        acc->getWorkManageModule().onFlag(0x22000012);
        break;
    }
    Fighter::notifyEventChangeSituation(kind, previous, acc);
}

void ftPurin::notifyEventCollisionAttackFighter(soCollisionLog* log, soModuleAccesser* acc) {
    // HYPOTHESIS: byte 0x21 selects the collision result; only value 1
    // requests the Rollout hit response. Its enum identity is unresolved.
    if (static_cast<u8>(log->_33) == 1) {
        switch (acc->getStatusModule().getStatusKind()) {
        case 0x119:
        case 0x11a:
            acc->getWorkManageModule().onFlag(0x22000012);
            break;
        }
    }
}

bool ftPurin::notifyEventCollisionAttackCheck(u32 flags) {
    soModuleAccesser* acc = m_moduleAccesser;
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x119:
    case 0x11a:
        if (acc->getWorkManageModule().isFlag(0x22000012)) {
            acc->getWorkManageModule().offFlag(0x22000012);
            if (ftPurinStatusUniqProcessSpecialNUtility::ftProcHitSpecialNPurin(acc) == true) {
                return true;
            }
        }
        break;
    }
    return Fighter::notifyEventCollisionAttackCheck(flags);
}

void ftPurin::notifyEventOnDamage(soDamage* damage, bool flag, soModuleAccesser* acc) {
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x117:
    case 0x118:
    case 0x119:
    case 0x11a:
    case 0x11b:
    case 0x11c:
    case 0x11d:
        // Read the five unsigned attribute bits; the SDK enum bitfield is signed.
        if ((*reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(&damage->m_attackData) + 0x30) & 0x1f) == soCollisionAttackData::Attribute_Turn) {
            ftPurinStatusUniqProcessSpecialNUtility::ftProcDamageTurnPurinSpecialN(acc);
        }
        break;
    }
    Fighter::notifyEventOnDamage(damage, flag, acc);
}

// The costume accessory uses the shared simple weapon, with one pooled instance.

ftPurinArticleManageModuleBuilder::ftPurinArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(2, 0), m_mediator(acc),
    m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }

#pragma dont_inline on
ftPurinArticleManageModuleBuilder::~ftPurinArticleManageModuleBuilder() { }
ftPurinSelectedArticleMediator::~ftPurinSelectedArticleMediator() { }
ftPurinArticleMediator::~ftPurinArticleMediator() { }
ftPurinArticleSubPool::~ftPurinArticleSubPool() { }
ftPurinArticleHolder::~ftPurinArticleHolder() { }
#pragma dont_inline off
ftPurinArticlePool::~ftPurinArticlePool() { }
ftPurinArticleHierarchy::~ftPurinArticleHierarchy() { }

wnSimple* ftPurinArticleSubPool::getInstanceAt(s32 index) {
    if (index == 0) { return m_holder.getInstance(); }
    return NULL;
}

bool ftPurin::activeArticle(wnSimple* article, soModuleAccesser* acc) {
    // MATCH-ONLY: declaration order retains the founder ID in its native register.
    s32 founderTaskId;
    ftData* data = g_ftCommonDataAccesser.getData(Fighter_Purin);
    // HYPOTHESIS: source member name. Offset 0x64 contains the simple-article
    // data table; its first element is consumed by this accessory activator.
    wnSimpleData** articleData = *reinterpret_cast<wnSimpleData***>(data->unk58 + 0x0c);
    soResourceModule& resources = acc->getResourceModule();
    founderTaskId = acc->getStageObject().m_taskId;
    article->activate(founderTaskId, resources.getResourceIdAccesser()->getMdlResId(),
        articleData[0], false, true, Vec3f(0.0f, 0.0f, 0.0f), 0xffff, 0xffff,
        0, true, false, -1, 0, true, 2, 0, true);
    return true;
}

// MATCH-ONLY: empty type-list markers survive in each native slot dispatch.
// Their source type is unresolved; inactive slots still own distinct lifetimes.
struct ftPurinArticleSlotTag {
    volatile u8 unk0, unk1;
    ftPurinArticleSlotTag() : unk0(0), unk1(0) { }
    ~ftPurinArticleSlotTag() { }
};
#define FT_PURIN_UNUSED_SLOT(n, value) case n: { ftPurinArticleSlotTag tag; return value; }
#define FT_PURIN_UNUSED_SLOTS(value) \
    FT_PURIN_UNUSED_SLOT(1, value) FT_PURIN_UNUSED_SLOT(2, value) \
    FT_PURIN_UNUSED_SLOT(3, value) FT_PURIN_UNUSED_SLOT(4, value) \
    FT_PURIN_UNUSED_SLOT(5, value) FT_PURIN_UNUSED_SLOT(6, value) \
    FT_PURIN_UNUSED_SLOT(7, value) FT_PURIN_UNUSED_SLOT(8, value) \
    FT_PURIN_UNUSED_SLOT(9, value) FT_PURIN_UNUSED_SLOT(10, value) \
    FT_PURIN_UNUSED_SLOT(11, value) FT_PURIN_UNUSED_SLOT(12, value) \
    FT_PURIN_UNUSED_SLOT(13, value) FT_PURIN_UNUSED_SLOT(14, value) \
    FT_PURIN_UNUSED_SLOT(15, value) FT_PURIN_UNUSED_SLOT(16, value)

static soArticle* ftPurinNullArticle() {
    return reinterpret_cast<soArticle*>(g_ftRobotNullArticleStorage);
}

soArticle* ftPurinArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: {
        ftPurinArticleSlotTag tag;
        soArticleDeactivateChecker checker;
        // MATCH-ONLY: native empty traversal marker has its own byte lifetime.
        volatile u8 traversalTag = 0;
        wnSimple* instance;
        if (checker(&static_cast<soArticle&>(*m_pool.getSub().getInstance())) == true) {
            instance = m_pool.getSub().getInstance();
        } else {
            instance = NULL;
        }
        if (instance == NULL) {
            instance = static_cast<wnSimple*>(checker.getCandidate());
            if (instance == NULL) { return ftPurinNullArticle(); }
            instance->deactivateArticle();
        }
        if (ftPurin::activeArticle(instance, acc) == true) {
            return &static_cast<soArticle&>(*instance);
        }
        return ftPurinNullArticle();
    }
    FT_PURIN_UNUSED_SLOTS(ftPurinNullArticle())
    default: return ftPurinNullArticle();
    }
}

static inline bool ftPurinAllArticlesActive(ftPurinArticleHierarchy& pool) {
    for (s32 i = 0; i < 1; ++i) {
        soArticle& article = *pool.getSub().getInstanceAt(i);
        if (article.isActiveArticle() == false) { return false; }
    }
    return true;
}

bool ftPurinArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: {
        ftPurinArticleSlotTag tag;
        return !ftPurinAllArticlesActive(m_pool);
    }
    FT_PURIN_UNUSED_SLOTS(false)
    default: return false;
    }
}

// The generic visitor records both active and inactive counts. The native
// active query uses its first count and retains the indirect predicate call.
struct ftPurinArticleActiveCounter {
    // MATCH-ONLY: retain the native visitor storage and indirect predicate load;
    // its original aliasing through the generic pool visitor is unresolved.
    bool (*volatile isActive)(soArticle*);
    volatile s32 activeCount, inactiveCount;
    static bool checkActivate(soArticle* article);
    ftPurinArticleActiveCounter() : isActive(checkActivate), activeCount(0), inactiveCount(0) { }
    void operator()(soArticle* article) {
        if (isActive(article) == true) { ++activeCount; }
        else { ++inactiveCount; }
    }
};
bool ftPurinArticleActiveCounter::checkActivate(soArticle* article) {
    return article->isActiveArticle();
}

s32 ftPurinArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: {
        ftPurinArticleSlotTag tag;
        ftPurinArticleActiveCounter counter;
        for (s32 i = 0; i < 1; ++i) {
            counter(&static_cast<soArticle&>(*m_pool.getSub().getInstanceAt(i)));
        }
        return counter.activeCount;
    }
    FT_PURIN_UNUSED_SLOTS(0)
    default: return 0;
    }
}

s32 ftPurinArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { ftPurinArticleSlotTag tag; return 1; }
    FT_PURIN_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftPurinArticleMediator::shoot(soModuleAccesser*, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { ftPurinArticleSlotTag tag; (void)dynamic_cast<wnSimple&>(*article); return true; }
    FT_PURIN_UNUSED_SLOTS(true)
    default: return false;
    }
}

void ftPurinArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnSimple* instance = m_pool.getSub().getInstanceAt(i);
        if (!static_cast<soArticle&>(*instance).setDeactivateDescendant()) { return; }
    }
}
s32 ftPurinArticleMediator::getMediateNum() { return 1; }
void ftPurinArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

#undef FT_PURIN_UNUSED_SLOTS
#undef FT_PURIN_UNUSED_SLOT
