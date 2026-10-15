#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/pikmin/wn_pikmin_chain.h>
#include <mt/mt_vector.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <so/status/so_status_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_lasso_status_view.h>

// HYPOTHESIS: a data word of the REL (the node or color the chain model is attached with).
extern u32 lbl_113_data_BAA8;

// The status unique processes of the chain: the lasso ones live in sora_melee (wn_lasso_status_uniq_process_*.cpp), the two
// that extend them in this REL.
extern soStatusUniqProcess g_wnPikminChainStatusUniqProcessShoot;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHang;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHangRewind;
extern soStatusUniqProcess g_wnPikminChainStatusUniqProcessReach;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessFailure;

wnPikminChain::wnPikminChain(s32 articleId, const wnArticleBuildInfo& info, const wnPikminChainData* data) :
    wnWeaponBuilder<wnPikminChainModuleAccesserBuildConfig>(
        "PikminChain", articleId, info,
        wnModuleAccesserBuildData(0x90, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    uniqProcs[1] = &g_wnPikminChainStatusUniqProcessShoot;
    uniqProcs[3] = &g_wnLassoStatusUniqProcessRewind;
    uniqProcs[5] = &g_wnLassoStatusUniqProcessHang;
    uniqProcs[6] = &g_wnLassoStatusUniqProcessHangRewind;
    uniqProcs[7] = &g_wnPikminChainStatusUniqProcessReach;
    uniqProcs[8] = &g_wnLassoStatusUniqProcessFailure;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 9);
    Weapon::deactivate(false);
}

void wnPikminChain::activate(float lr, int founderTaskId, int kind, int team, bool unk, int pikminNum) {
    Vec3f zero(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = kind;
    desc.unk8 = kind;
    desc.unkC = kind;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    __memcpy(&desc.pos, &zero, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    wnLassoGetLink(m_moduleAccesser)->unk148Int(2);
    wnLassoGetLink(m_moduleAccesser)->unk194Int(3, 8, false);
    wnLassoGetLink(m_moduleAccesser)->unk194Int(3, 8, false);
    soResourceModule& resource = m_moduleAccesser->getResourceModule();
    wnLassoAltStatusView* alt = wnLassoGetAltStatus(m_moduleAccesser);
    u32 nodeId = lbl_113_data_BAA8;
    int noValue = 0;
    alt->unk18Attach(resource.getResourceIdAccesser()->getBinResId(), noValue, nodeId, nodeId, true, 0, noValue, -1);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getWorkManageModule().setFlag(unk, 0x22000000);
    m_moduleAccesser->getWorkManageModule().setInt(pikminNum, 0x10000007);
    m_moduleAccesser->getWorkManageModule().setInt(0xb, 0x10000005);
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    alt = wnLassoGetAltStatus(m_moduleAccesser);
    alt->unkDCFloat(posture.getScale());
    wnLassoGetAltStatus(m_moduleAccesser)->unkA4Int(6);
}

void wnPikminChain::onDeactivate() {
    m_moduleAccesser->getLinkModule().unlinkAll();
    m_moduleAccesser->getLinkModule().unlinkNodeAll();
}

void wnPikminChain::updateNodeSRT() {
    soModuleAccesser* acc = m_moduleAccesser;
    float scaleValue = acc->getPostureModule().getScale();
    Vec3f scale;
    scale.m_z = scaleValue;
    scale.m_y = scaleValue;
    scale.m_x = scaleValue;
    Vec3f rot;
    Vec3f pos;
    pos = acc->getPostureModule().getPos();
    rot = acc->getPostureModule().getRot(0);
    rot.m_y += acc->getPostureModule().getRotYLr();
    acc->getModelModule().setNodeSRT(1, &scale, &rot, &pos);
    acc->getModelModule().setNodeSRT(0, &scale, &rot, &pos);
}

void wnPikminChain::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Weapon::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (statusKind == 7) {
        // Reaching the end of the chain: remember how many Pikmin it carried.
        soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
        work.setInt(work.getInt(0x10000007), 0x20000002);
    }
}

void wnPikminChain::processFixCamera() {
    Weapon::processFixCamera();
    if (soSlow::getInstance()->isEstimate() == true && m_moduleAccesser->getStopModule().isStop() == false) {
        int status = m_moduleAccesser->getStatusModule().getStatusKind();
        if (status < 6) {
            if (status >= 4) {
                return;
            }
            if (status < 1) {
                return;
            }
        } else if (status >= 8) {
            return;
        }
        wnPikminChainUtility::updatePikminVisibility(m_moduleAccesser);
    }
}

void wnPikminChain::renderDebug() {
    StageObject::renderDebug();
}
