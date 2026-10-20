#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <cm/cm_camera_controller.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <ft/fox/ft_fox_status_uniq_process.h>
#include <types.h>

// Local view of the article manage module (void* in the shared enumeration). Slot 0x48 takes two ints.
// P pad slots before a named slot place it at vtable index P + 2 (the destructor pair leads), so 16 pads give 0x48.
class ftFoxFinalArticleManageView {
public:
    virtual ~ftFoxFinalArticleManageView() { }
    virtual void pad0();
    virtual void pad1();
    virtual void pad2();
    virtual void pad3();
    virtual void pad4();
    virtual void pad5();
    virtual void pad6();
    virtual void pad7();
    virtual void pad8();
    virtual void pad9();
    virtual void pad10();
    virtual void pad11();
    virtual void pad12();
    virtual void pad13();
    virtual void pad14();
    virtual void unk48(int, int);
};

// The camera range record as the status reads it: a float in front of the range rectangle (HYPOTHESIS: a depth offset).
struct ftFoxFinalCameraRangeView {
    float m_offsetY;
    Rect2D m_range;
};

// Fox, Falco and Wolf share this process; Falco's statuses are shifted by 3 from status 0x117 on (stage object subkind 0x13).
static inline int ftFoxFinalStatus(soModuleAccesser* moduleAccesser, int status) {
    int result = status;
    if (status <= 0x116) {
        return result;
    }
    switch (moduleAccesser->getStageObject().soGetSubKind()) {
    case 0x6:
        result = status;
        break;
    case 0x13:
        result = status + 3;
        break;
    case 0x2c:
        result = status;
        break;
    }
    return result;
}

ftFoxStatusUniqProcessFinal g_ftFoxStatusUniqProcessFinal;

// Status 0x11d is the Landmaster drop (the fighter falls with the landmaster article), 0x11e the ride.
void ftFoxStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
    int status = ftFoxFinalStatus(moduleAccesser, moduleAccesser->getStatusModule().getStatusKind());
    if (status == 0x11d) {
        float fallSpeed = 0.0f;
        switch (moduleAccesser->getStageObject().soGetSubKind()) {
        case 0x6:
            fallSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbc, 0);
            break;
        case 0x13:
            fallSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfba, 0);
            break;
        case 0x2c:
            fallSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbc, 0);
            break;
        }
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.m_speedY = fallSpeed;
        gravity.m_gravity = 0.0f;
        gravity.unk18 = 0.0f;
        gravity.enable();
    } else if (status == 0x11e) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        work.setFloat(20.0f * moduleAccesser->getPostureModule().getScale(), 0x11000012);
    }
    // The camera range grows to keep the landmaster in view: the rectangle is added relative to the current range.
    ftFoxFinalCameraRangeView* range = reinterpret_cast<ftFoxFinalCameraRangeView*>(moduleAccesser->getCameraModule().getCameraRangeData(0));
    Rect2D rect;
    rect.m_up = 30.0f - range->m_range.m_up;
    rect.m_right = 50.0f - range->m_range.m_left;
    rect.m_left = -50.0f - range->m_range.m_right;
    rect.m_down = -20.0f - range->m_range.m_down;
    moduleAccesser->getCameraModule().addCameraRangeRect(&rect, 0);
    Vec3f offset(0.0f, 20.0f - range->m_offsetY, 0.0f);
    moduleAccesser->getCameraModule().addCameraRangeOffset(&offset, 0);
    moduleAccesser->getCameraModule().setDemoDistScaleMul(4.0f, 0);
}

// Status kinds 0x116 and up are remapped when the stage object reports subkind 0x13 (+3); the article-manage
// view then gets a call unless the status lands on one of the final-kind values.
void ftFoxStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    int curStatus = ftFoxFinalStatus(moduleAccesser, moduleAccesser->getStatusModule().getStatusKind());
    if (curStatus != 0x11f) {
        int nextKind = ftFoxFinalStatus(moduleAccesser, nextStatus);
        if (nextKind != 0x11d && nextKind != 0x11e && nextKind != 0x11f) {
            reinterpret_cast<ftFoxFinalArticleManageView&>(*moduleAccesser->m_enumerationStart->m_generateArticleManageModule)
                .unk48(3, 0);
        }
    }
}

// While the landmaster is dropping (0x11d), reaching the upper camera limit starts the ride; during the ride (0x11e) the
// fighter follows the model node of the landmaster it is linked to.
void ftFoxStatusUniqProcessFinal::execFixPos(soModuleAccesser* moduleAccesser) {
    int status = ftFoxFinalStatus(moduleAccesser, moduleAccesser->getStatusModule().getStatusKind());
    if (status == 0x11d) {
        CameraController* cameraController = CameraController::getInstance();
        Vec3f pos = moduleAccesser->getPostureModule().getPos();
        if (pos.m_y > *reinterpret_cast<float*>(reinterpret_cast<u8*>(cameraController) + 0x150)) {
            int rideStatus;
            switch (moduleAccesser->getStageObject().soGetSubKind()) {
            case 0x6:
                rideStatus = 0x11e;
                break;
            case 0x13:
                rideStatus = 0x11b;
                break;
            case 0x2c:
                rideStatus = 0x11e;
                break;
            default:
                rideStatus = 0x11e;
                break;
            }
            moduleAccesser->getStatusModule().changeStatusRequest(rideStatus, moduleAccesser);
        }
    } else if (status == 0x11e) {
        soLinkModule& link = moduleAccesser->getLinkModule();
        soPostureModule& posture = moduleAccesser->getPostureModule();
        if (link.isLinked(6)) {
            Vec3f nodePos = link.getParentModelNodeGlobalPosition(6, static_cast<u32>(0), false);
            float lr = link.getParentLr(6);
            posture.setPos(&nodePos);
            posture.setLr(lr);
            posture.updateRotYLr();
        }
    }
}
