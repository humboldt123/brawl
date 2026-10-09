#define FT_FIGHTER_ANIMCMD_LONG
#define FT_KINETIC_MEDIATOR_MODE_ARG
#define FT_MARTH_RUNTIME_HELPERS
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
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x1760, Fighter_Purin> ftPurinGenerateArticleManageModuleBuilder;

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
    virtual void notifyEventChangeSituation(SituationKind, SituationKind, soModuleAccesser*);
    virtual void notifyEventCollisionAttackFighter(soCollisionLog*, soModuleAccesser*);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventOnDamage(soDamage*, bool, soModuleAccesser*);
};
static_assert(sizeof(ftPurin) == 0x9A5C, "Class is the wrong size!");


#define FT_BC ftPurinBuildConfig
#include <ft/builder/ft_builder_noinline.h>

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
    soStatusUniqProcess* processes[14] = {
        &g_ftPurinStatusUniqProcessSpecialNStart,
        &g_ftPurinStatusUniqProcessSpecialS,
        &g_ftPurinStatusUniqProcessSpecialHi,
        &g_ftPurinStatusUniqProcessSpecialLw,
        &g_ftPurinStatusUniqProcessFinal,
        &g_ftPurinStatusUniqProcessSpecialNHold,
        &g_ftPurinStatusUniqProcessSpecialNHoldMax,
        &g_ftPurinStatusUniqProcessSpecialNRoll,
        &g_ftPurinStatusUniqProcessSpecialNRollAir,
        &g_ftPurinStatusUniqProcessSpecialNTurn,
        &g_ftPurinStatusUniqProcessSpecialNEnd,
        &g_ftPurinStatusUniqProcessSpecialNHitEnd,
        &g_ftPurinStatusUniqProcessFinal,
        &g_ftPurinStatusUniqProcessFinal
    };
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 14);
}

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


ftPurinExtendParamAccesser g_ftPurinExtendParamAccesser;

ftClassInfoImpl<Fighter_Jigglypuff, ftPurin> g_ftClassInfoPurin;
