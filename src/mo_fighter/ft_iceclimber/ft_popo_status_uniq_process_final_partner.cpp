#define FT_FIGHTER_ANIMCMD_LONG
#define FT_FIGHTER_GET_PARTNER_VIRTUAL // MATCH-ONLY: Fighter::getPartner is reached through the vtable
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/fighter.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/model/so_model_module_simple.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: a local shell of ftPopo deriving Fighter directly; this unit only needs its RTTI and getPartner.
class ftPopo : public Fighter {
public:
    virtual Fighter* getPartner();
};

class ftPopoStatusUniqProcessFinalPartner : public soStatusUniqProcess {
    void detachConstraint(soModuleAccesser* moduleAccesser);
    void setSafePosPartner(soModuleAccesser* moduleAccesser);

public:
    virtual ~ftPopoStatusUniqProcessFinalPartner() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftPopoStatusUniqProcessFinalPartner::initStatus(soModuleAccesser* moduleAccesser) {
    ftPopo* self = dynamic_cast<ftPopo*>(&moduleAccesser->getStageObject());
    if (self == 0) {
        return;
    }
    Fighter* partner = self->getPartner();
    if (partner == 0) {
        return;
    }
    if (moduleAccesser->getLinkModule().isLink(6) == 1) {
        moduleAccesser->getLinkModule().setModelConstraintPosOrt(6, 0, 0, 3, true);
    }
    moduleAccesser->getPostureModule().setLr(soExternalValueAccesser::getLr(partner));
}

// HYPOTHESIS: the native soMotionChangeParam constructor takes this flag as a bool (the target passes it
// unmasked); with the u8 declaration the call masks it, so this function stays fn_.
void ftPopoStatusUniqProcessFinalPartner::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000011);
        float frame = 0.0f;
        bool keepFrame = false;
        if (moduleAccesser->getMotionModule().getKind() == 0x1e9) {
            frame = moduleAccesser->getMotionModule().getFrame();
            keepFrame = true;
        }
        soMotionChangeParam param(0x1e5, frame, 1.0f, 0, 0, keepFrame, 0);
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x22000012) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000012);
        float frame = 0.0f;
        bool keepFrame = false;
        if (moduleAccesser->getMotionModule().getKind() == 0x1e5) {
            frame = moduleAccesser->getMotionModule().getFrame();
            keepFrame = true;
        }
        soMotionChangeParam param(0x1e9, frame, 1.0f, 0, 0, keepFrame, 0);
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000010);
        detachConstraint(moduleAccesser);
        bool inAir = true;
        moduleAccesser->getModelModule().setNodeTranslate(2, &Vec3f(0.0f, 0.0f, 0.0f));
        if (moduleAccesser->getMotionModule().getKind() != 0x1e9 &&
            moduleAccesser->getGroundModule().attachGround(0) == true) {
            inAir = false;
            moduleAccesser->getSituationModule().setKind(SituationKind(0), inAir);
            moduleAccesser->getStatusModule().changeStatusRequest(0, moduleAccesser);
        }
        if (inAir == true) {
            setSafePosPartner(moduleAccesser);
            moduleAccesser->getSituationModule().setKind(SituationKind(2), false);
            moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
        }
    }
}

void ftPopoStatusUniqProcessFinalPartner::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (moduleAccesser->getLinkModule().isModelConstraint() == true) {
        detachConstraint(moduleAccesser);
        setSafePosPartner(moduleAccesser);
        moduleAccesser->getModelModule().setNodeTranslate(2, &Vec3f(0.0f, 0.0f, 0.0f));
    }
}

void ftPopoStatusUniqProcessFinalPartner::detachConstraint(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isModelConstraint()) {
        Vec3f pos = moduleAccesser->getModelModule().getNodeGlobalPosition(2, false);
        pos.m_z = moduleAccesser->getGroundModule().getZ();
        moduleAccesser->getLinkModule().removeModelConstraint(true);
        if (moduleAccesser->getLinkModule().isLink(6) == 1) {
            moduleAccesser->getLinkModule().setAttribute(6, soLinkConnection::Attribute(0xd), true);
        }
        moduleAccesser->getPostureModule().setPos(&pos);
        moduleAccesser->getStageObject().updateNodeSRT();
    }
}

void ftPopoStatusUniqProcessFinalPartner::setSafePosPartner(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isLink(6) == 1 && moduleAccesser->getLinkModule().isValidParentShape(6) == true) {
        Vec2f safePos;
        Vec2f::copy(safePos, moduleAccesser->getLinkModule().getParentShapeCenterPos(6));
        moduleAccesser->getGroundModule().setShapeSafePos(&safePos, 0);
    }
}

ftPopoStatusUniqProcessFinalPartner g_ftPopoStatusUniqProcessFinalPartner;
