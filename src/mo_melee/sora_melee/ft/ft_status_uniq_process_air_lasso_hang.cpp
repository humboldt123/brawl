#define MT_VEC3F_SUB_AIR_LASSO // MATCH-ONLY: subtraction register form of the original
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// Use the native Fighter observer override before transitive Fighter includes (same as the shoot scope unit).
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_status_uniq_process_air_lasso_hang.h>
#include <ft/ft_status_uniq_process_views.h>
#include <math.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <types.h>

ftStatusUniqProcessAirLassoHang g_ftStatusUniqProcessAirLassoHang;

// HYPOTHESIS: hanging on the ledge hooks the fighter onto the cliff and logs the lasso action (log int 0x1C).
void ftStatusUniqProcessAirLassoHang::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getGroundModule().isStCliff(0)) {
        moduleAccesser->getGroundModule().entryCliff(0);
        dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner()->addLogInt(1, 0x1C, 0);
    }
}

// HYPOTHESIS: same foot-limited hip follow as the rewind process.
void ftStatusUniqProcessAirLassoHang::execStatus(soModuleAccesser* moduleAccesser) {
    soModelModule& model = moduleAccesser->getModelModule();
    int leftFoot = model.getNodeId("LFootJ");
    int rightFoot = model.getNodeId("RFootJ");
    int hip = model.getCorrectNodeId(0x12D);
    Vec3f leftPos = model.getNodeGlobalPosition(leftFoot, false);
    Vec3f rightPos = model.getNodeGlobalPosition(rightFoot, false);
    Vec3f hipPos = model.getNodeGlobalPosition(hip, false);
    if (leftPos.m_y > rightPos.m_y) {
        if (rightPos.m_y < hipPos.m_y) {
            hipPos.m_y = rightPos.m_y;
        }
    } else if (leftPos.m_y < hipPos.m_y) {
        hipPos.m_y = leftPos.m_y;
    }
    moduleAccesser->getPostureModule().setPos(&hipPos);
}

// HYPOTHESIS: while hanging, the lasso hand (motion kind 0xEB) aims at the ledge like the reach process.
void ftStatusUniqProcessAirLassoHang::execFixPos(soModuleAccesser* moduleAccesser) {
    soGroundModule* ground = &moduleAccesser->getGroundModule();
    if (ground->isStCliff(0)) {
        Vec3f cliffPos = ground->getCliffPos(0);
        cliffPos.m_x = cliffPos.m_x - reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getLr();
        if (moduleAccesser->getMotionModule().getKind() == 0xEB) {
            int node = moduleAccesser->getModelModule().getCorrectNodeId(0x133);
            Vec3f nodePos = moduleAccesser->getModelModule().getNodeGlobalPosition(node, false);
            Vec3f diff = cliffPos - nodePos;
            float diffY = diff.m_y;
            float angle = atan2(diff.m_x * reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getLr(), diffY);
            float frame = 57.29578f * angle;
            moduleAccesser->getMotionModule().setFrame(frame);
        }
        ftStatusUniqProcessAirLassoCommon::execFixPos(moduleAccesser);
    }
}

// Leaving the hang (other than to status 0x82) puts the fighter at the lasso node (ground shape z), unlinks it and leaves the cliff.
void ftStatusUniqProcessAirLassoHang::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (nextStatusKind != 0x82) {
        soModelModule& model = moduleAccesser->getModelModule();
        int node = model.getCorrectNodeId(0x12E);
        Vec3f pos = moduleAccesser->getModelModule().getNodeGlobalPosition(node, false);
        pos.m_z = moduleAccesser->getGroundModule().getZ();
        reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unlinkSlot164(1);
        moduleAccesser->getGroundModule().leaveCliff(0);
    }
}
