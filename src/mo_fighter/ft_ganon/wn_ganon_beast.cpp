#define SO_BUILDER_HELPERS_WEAK
#define IP_INPUT_BUTTON_RAW // BYTES: IpNull::getButton returns the raw button mask (8 bytes, not a struct copy)
#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR // the two-entry status process vector is default-constructed
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
// The status process vector and the attack part vector are built by their sora_melee constructors.
WN_STATUS_PROC_VECTOR_ARG_CTOR(2);
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 4);
#include <wn/wn_activate_desc.h>
#include <so/damage/so_damage.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/visibility/so_visibility_module_impl.h>
#include <ft/ft_common_data_accesser.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>
#include <wn/ganon/wn_ganon_beast.h>

// This article owns the out-of-line copy of the parameter accesser interface's empty teardown.
#pragma push
#pragma dont_inline on
soParamAccesser::~soParamAccesser() { }
#pragma pop

inline soDamageEventObserver::~soDamageEventObserver() { }
inline void soDamageEventObserver::notifyEventOnDamage(soDamage*, bool, soModuleAccesser*) { }
inline void soDamageEventObserver::notifyEventAddDamage(soDamage*, soModuleAccesser*) { }

// Hit-stop constants passed to soStopModuleImpl::calcHitStopFrame (same values and layout as in wn_pit_ikarus.cpp and
// wn_snake_nikita_missile.cpp): MWCC keeps these external-linkage objects at the start of .rodata, ahead of the
// builder constructor's pooled literals.
// HYPOTHESIS: names and the exact original declaration (likely an extern declared in a shared weapon header).
extern const float g_wnHitStopFrameMul = 0.3846154f;
extern const float g_wnHitStopFrameAdd = 5.0f;
extern const float g_wnHitStopFrameMax = 30.0f;

wnGanonBeast::wnGanonBeast(s32 articleId, const wnArticleBuildInfo& info, const wnGanonBeastData* data) :
    wnWeaponBuilder<wnGanonBeastModuleAccesserBuildConfig>(
        "GanonBeast", articleId, info,
        wnModuleAccesserBuildData(0x80, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  reinterpret_cast<s32>(&data->m_attackDataSet), 0, 0, 0, 0, 0, 0)),
    soDamageEventObserver(static_cast<soEventManager&>(getModuleAccesser()->getEventManageModule()).getManageId(), -1),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soModuleAccesser* accesser = getModuleAccesser();
    soStatusUniqProcess* uniqProcs[2] = {};
    uniqProcs[0] = &g_wnGanonBeastStatusUniqProcessStart;
    uniqProcs[1] = &g_wnGanonBeastStatusUniqProcessAttack;
    accesser->getStatusModule().addRangeUniqProc(uniqProcs, 2);
    accesser->getCollisionHitModule().add(const_cast<soSet<soCollisionHitData>*>(&data->m_hitDataSet), 0);
    Weapon::deactivate(false);
}

bool wnGanonBeast::notifyEventCollisionAttackCheck(u32) { return false; }

// HYPOTHESIS: a hit that sends the beast flying while it is not already stopped adds a hit stop and raises its work flag.
void wnGanonBeast::notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser) {
    soStopModule* stop = &moduleAccesser->getStopModule();
    if (damage->m_reaction > 0.0f && !stop->isStop()) {
        int frames = soStopModuleImpl::calcHitStopFrame(damage->m_powerMax, 0.3846154f, 5.0f, 1.0f, 30.0f);
        moduleAccesser->getStopModule().setHitStopFrame(frames, true);
        moduleAccesser->getWorkManageModule().onFlag(0x12000005);
    }
}

void wnGanonBeast::activate(s32 founderTaskId, s32 resourceId, s32 team, const Vec3f* pos, float lr) {
    soModuleAccesser* accesser = m_moduleAccesser;
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, *pos, lr, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    soVisibilityData* visData = static_cast<wnGanonBeastData*>(g_ftCommonDataAccesser.getGanonBeastData())->m_visibilityData;
    accesser->getStatusModule().changeStatusForce(0, accesser);
    soVisibilityModuleImpl& visibility = dynamic_cast<soVisibilityModuleImpl&>(accesser->getVisibilityModule());
    visibility.setData(visData);
    for (int count = 0; count < 2; ++count) {
        visibility.setStatusDefault(0, 0, false);
    }
    visibility.setDefaultAll();
}
