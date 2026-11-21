#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/popo/wn_popo_rubber.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/link/so_link_module_impl.h>
#include <so/resource/so_resource_module_impl.h>
#include <wn/wn_lasso_status_view.h>

// HYPOTHESIS: a data word of the REL (the node the rope model is attached with), as in the other lasso weapons.
extern u32 lbl_105_data_99D8;

// The status unique process shared by the four active statuses of the rubber.
extern soStatusUniqProcess g_wnPopoRubberStatusUniqProcessCommon;

wnPopoRubber::wnPopoRubber(s32 articleId, const wnArticleBuildInfo& info, const wnPopoRubberData* data) :
    wnWeaponBuilder<wnPopoRubberModuleAccesserBuildConfig>(
        "wnPopoRubber", articleId, info,
        wnModuleAccesserBuildData(0x97, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[5] = { 0 };
    uniqProcs[1] = &g_wnPopoRubberStatusUniqProcessCommon;
    uniqProcs[2] = &g_wnPopoRubberStatusUniqProcessCommon;
    uniqProcs[3] = &g_wnPopoRubberStatusUniqProcessCommon;
    uniqProcs[4] = &g_wnPopoRubberStatusUniqProcessCommon;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 5);
    Weapon::deactivate(false);
}

// TODO(match): instructions and stack match; three callee-saved registers rotate (this, resourceId and
// parentTaskId get r30/r29/r31 where the original has r29/r31/r30). Tried: int/u32 parameter types, a local
// link-module reference, a local accesser, a named -1, the Vec3f temporary inline, reloading alt for unk60Int.
void wnPopoRubber::activate(int founderTaskId, u32 resourceId, int team, int parentTaskId) {
    Vec3f zero(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, zero, 0.0f, team, 0, 2, 0x80, 0, 0x35f, 0);
    // The two top bits of each flag byte are separate one-bit flags: the original writes them one at a time.
    desc.flagBit7 = 1;
    desc.flagBit6 = 1;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.flag4DBit7 = 0;
    desc.flag4DBit6 = 0;
    Weapon::activate(&desc);
    soResourceModule& resource = m_moduleAccesser->getResourceModule();
    wnLassoAltStatusView* alt = wnLassoGetAltStatus(m_moduleAccesser);
    u32 nodeId = lbl_105_data_99D8;
    int noValue = 0;
    alt->unk18Attach(resource.getResourceIdAccesser()->getBinResId(), noValue, nodeId, nodeId, nodeId, 0, noValue, -1);
    wnLassoGetAltStatus(m_moduleAccesser)->unk60Int(2);
    if (m_moduleAccesser->getLinkModule().link(5, parentTaskId)) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
    }
}
