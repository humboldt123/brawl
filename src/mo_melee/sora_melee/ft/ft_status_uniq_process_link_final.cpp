#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// Use the native Fighter observer override before transitive Fighter includes (same as the shoot scope unit).
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_status_uniq_process_link_final.h>
#include <so/capture/so_capture_module.h>
#include <mt/mt_prng.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

ftStatusUniqProcessLinkFinal g_ftStatusUniqProcessLinkFinal;

// HYPOTHESIS: the eight motion kinds the captured fighter is thrown into (picked at random).
static const int sThrownMotionKinds[8] = { 0x99, 0x9A, 0x9C, 0x9D, 0x9F, 0xA0, 0xA2, 0xA3 };

// Messages from Link's final smash: 1 hit stop, 2 start the capture pose, 3 attach to a node of Link (and pick a random
// thrown motion), 5 / 7 raise flags, 6 report the position of a node.
void ftStatusUniqProcessLinkFinal::linkMessageProcedure(ftLinkFinalMessage* message, soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    work->setInt(message->m_message, 0x20000000);
    switch (message->m_message) {
    case 1:
        moduleAccesser->getStopModule().setOtherStop(message->m_value);
        break;
    case 2: {
        soMotionModule* motion = &moduleAccesser->getMotionModule();
        soMotionChangeParam param;
        param.m_kind = 0x9C;
        param.m_frame = 0.0f;
        param.m_rate = 0.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        motion->changeMotionRequest(&param);
        break;
    }
    case 3: {
        int index = randi(8);
        work->setInt(sThrownMotionKinds[index], 0x20000001);
        int node;
        if (moduleAccesser->m_stageObject->soGetSubKind() == 0x2F) {
            node = moduleAccesser->getModelModule().getCorrectNodeId(0x12E);
        } else {
            node = moduleAccesser->getModelModule().getCorrectNodeId(0x12D);
        }
        moduleAccesser->getLinkModule().setModelConstraintPosOrt(0, node, message->m_value, 3, true);
        work->onFlag(0x22000010);
        break;
    }
    case 5:
        work->onFlag(0x22000012);
        break;
    case 6: {
        soModelModule* model = &moduleAccesser->getModelModule();
        int node = model->getCorrectNodeId(message->m_value);
        Vec3f copy;
        Vec3f pos = model->getNodeGlobalPosition(node, false);
        copy = pos;
        message->m_pos = copy;
        break;
    }
    case 7:
        work->onFlag(0x12000032);
        break;
    }
}

void ftStatusUniqProcessLinkFinal::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    if (moduleAccesser->getStatusModule().getPrevStatusKind(0) != 0xED) {
        work->offFlag(0x22000010);
        work->offFlag(0x22000012);
        work->setInt(0, 0x20000000);
        work->setInt(0xA3, 0x20000000);
    }
}

bool ftLinkFinalIsCaptured(soModuleAccesser* moduleAccesser);

void ftStatusUniqProcessLinkFinal::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isLink(0)) {
        moduleAccesser->getLinkModule().setAttribute(0, (soLinkConnection::Attribute)0, false);
    }
    soModelModule* model = &moduleAccesser->getModelModule();
    soPostureModule* posture = &moduleAccesser->getPostureModule();
    int node = model->getCorrectNodeId(0);
    Vec3f pos = model->getNodeGlobalPosition(node, false);
    posture->setPos(&pos);
    moduleAccesser->m_stageObject->updateNodeSRT();
    moduleAccesser->getSituationModule().setKind((SituationKind)2, true);
    moduleAccesser->getGroundModule().setCorrect((soGroundShapeImpl::CorrectKind)0, 0);
    soKineticModule* kinetic = &moduleAccesser->getKineticModule();
    kinetic->clearSpeedAll();
    kinetic->unableEnergyAll();
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012) || !ftLinkFinalIsCaptured(moduleAccesser)) {
        int status = (moduleAccesser->getSituationModule().getKind() == 0) ? 0x17 : 0xE;
        moduleAccesser->getStatusModule().changeStatusRequest(status, moduleAccesser);
    }
}

bool ftStatusUniqProcessLinkFinal::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    moduleAccesser->getStatusModule().changeStatusRequest(0xed, moduleAccesser);
    return true;
}

void ftStatusUniqProcessLinkFinal::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (nextStatusKind == 0xED) {
        int index = randi(8);
        moduleAccesser->getWorkManageModule().setInt(sThrownMotionKinds[index], 0x20000001);
    }
    switch (nextStatusKind) {
    case 0xED:
    case 0x42:
        break;
    default:
        moduleAccesser->getLinkModule().removeModelConstraint(true);
        static_cast<soCaptureModule*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk54(0);
        static_cast<soCaptureModule*>(moduleAccesser->m_enumerationStart->m_captureModule)->captureCut(false, false);
        break;
    }
}

// Whether Link (or Toon Link) holds the fighter in one of the final smash capture statuses. Called out of line by
// execFixPos: with the inner `default: break;` arms the function is past the auto-inline size.
bool ftLinkFinalIsCaptured(soModuleAccesser* moduleAccesser) {
    StageObject* parent = moduleAccesser->getLinkModule().getParent(0);
    if (parent == NULL) {
        return false;
    }
    if (parent->soGetKind() != StageObject_Fighter) {
        return false;
    }
    int kind = parent->soGetSubKind();
    int status = soExternalValueAccesser::getStatusKind(parent);
    switch (kind) {
    case 2:
        switch (status) {
        case 0x116:
        case 0x11A:
        case 0x11B:
            return true;
        default:
            break;
        }
        break;
    case 0x29:
        switch (status) {
        case 0x116:
        case 0x11A:
        case 0x11B:
            return true;
        default:
            break;
        }
        break;
    }
    return false;
}
