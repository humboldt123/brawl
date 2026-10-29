#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <wn/fox/wn_fox_land_master.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/motion/so_motion_change_param.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/stageobject.h>
#include <mt/mt_vector.h>
#include <types.h>

// Class shapes from the module map.
class wnFoxLandMasterStatusUniqProcessCommon : public soStatusUniqProcess {
public:
    virtual ~wnFoxLandMasterStatusUniqProcessCommon() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execMapCorrection(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    void controllCaterpillar(soModuleAccesser* moduleAccesser);
    void slideZFromSpeed(soModuleAccesser* moduleAccesser);
    void vibrationYFromSpeed(soModuleAccesser* moduleAccesser);
    void vibrationYByLanding(soModuleAccesser* moduleAccesser);
};

wnFoxLandMasterStatusUniqProcessCommon g_wnFoxLandMasterStatusUniqProcessCommon;

void wnFoxLandMasterStatusUniqProcessCommon::initStatus(soModuleAccesser* moduleAccesser) {
    const wnFoxLandMasterParam* param = dynamic_cast<wnFoxLandMaster&>(moduleAccesser->getStageObject()).getParam();
    if (moduleAccesser->getStatusModule().getStatusKind() == 5) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        float speed = __fabsf(work.getFloat(0x11000003));
        work.setFloat(speed * param->unkC0, 0x21000008);
        work.setFloat(0.0f, 0x21000001);
        int motionKind;
        if (speed >= param->unk48) {
            motionKind = 10;
        } else if (speed >= param->unk4C) {
            motionKind = 9;
        } else {
            motionKind = 8;
        }
        float frame = work.getFloat(0x21000006);
        soMotionChangeParam motionParam;
        motionParam.m_kind = motionKind;
        motionParam.m_frame = frame;
        motionParam.m_rate = 0.0f;
        motionParam._12 = 0;
        motionParam._13 = 0;
        motionParam._14 = 0;
        motionParam._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&motionParam);
    }
    moduleAccesser->getWorkManageModule().setFloat(param->unk44, 0x11000008);
}

void wnFoxLandMasterStatusUniqProcessCommon::execStatus(soModuleAccesser* moduleAccesser) { }
void wnFoxLandMasterStatusUniqProcessCommon::execMapCorrection(soModuleAccesser* moduleAccesser) { }
void wnFoxLandMasterStatusUniqProcessCommon::execFixPos(soModuleAccesser* moduleAccesser) { }

void wnFoxLandMasterStatusUniqProcessCommon::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 2) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        float rotX = work.getFloat(0x11000004);
        work.setFloat(-rotX, 0x11000004);
        work.setFloat(-work.getFloat(0x11000005), 0x11000005);
        work.setFloat(-work.getFloat(0x11000006), 0x11000006);
        work.setFloat(-work.getFloat(0x11000007), 0x11000007);
        soPostureModule& posture = moduleAccesser->getPostureModule();
        posture.reverseLr();
        Vec3f rot(rotX, 0.0f, 0.0f);
        posture.setRot(&rot, 0);
        posture.updateRotYLr();
        moduleAccesser->m_stageObject->updateNodeSRT();
    }
}

void wnFoxLandMasterStatusUniqProcessCommon::controllCaterpillar(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soMotionModule& motion = moduleAccesser->getMotionModule();
    float speed = moduleAccesser->getPostureModule().getLr();
    speed *= kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL)).m_x;
    float frame = motion.getFrame() + 1.9212328f * speed;
    while (frame >= 100.0f) {
        frame -= 100.0f;
    }
    while (frame < 0.0f) {
        frame += 100.0f;
    }
    motion.setFrame(frame);
    moduleAccesser->getWorkManageModule().setFloat(frame, 0x21000006);
}

void wnFoxLandMasterStatusUniqProcessCommon::slideZFromSpeed(soModuleAccesser* moduleAccesser) {
    const wnFoxLandMasterParam* param = dynamic_cast<wnFoxLandMaster&>(moduleAccesser->getStageObject()).getParam();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    float lr = moduleAccesser->getPostureModule().getLr();
    float slide = work.getFloat(0x21000000);
    float unk5 = work.getFloat(0x21000005);
    float prevSpeed = work.getFloat(0x11000002);
    float accel = lr * (kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL)).m_x - prevSpeed);
    if (0.0f == accel) {
        if (slide > 0.0f) {
            slide -= param->unkB0;
            if (slide < 0.0f) {
                slide = 0.0f;
            }
        } else if (slide < 0.0f) {
            slide += param->unkB0;
            if (slide > 0.0f) {
                slide = 0.0f;
            }
        }
    } else {
        slide -= accel * param->unkB4;
        if (slide > param->unkAC) {
            slide = param->unkAC;
        } else if (slide < -param->unkAC) {
            slide = -param->unkAC;
        }
    }
    float rate = param->unkB8 * (slide / param->unkAC);
    work.setFloat(slide, 0x21000000);
    work.setFloat(rate, 0x21000002);
    work.setFloat(unk5, 0x21000005);
}

void wnFoxLandMasterStatusUniqProcessCommon::vibrationYFromSpeed(soModuleAccesser* moduleAccesser) {
    const wnFoxLandMasterParam* param = dynamic_cast<wnFoxLandMaster&>(moduleAccesser->getStageObject()).getParam();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    float offset = work.getFloat(0x21000001);
    float direction = work.getFloat(0x21000004);
    float speed = __fabsf(kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL)).m_x);
    if (speed >= param->unk9C && speed < param->unkA0) {
        float step = param->unkA8 * (1.0f - (speed - param->unk9C) / (param->unkA0 - param->unk9C));
        if (0.0f == direction) {
            direction = 1.0f;
        }
        offset += step * direction;
        if (offset > param->unkA4) {
            offset = param->unkA4 - (offset - param->unkA4);
            direction = -1.0f;
        } else if (offset < -param->unkA4) {
            offset = -param->unkA4 - (offset + param->unkA4);
            direction = 1.0f;
        }
    }
    work.setFloat(offset, 0x21000001);
    work.setFloat(direction, 0x21000004);
}

void wnFoxLandMasterStatusUniqProcessCommon::vibrationYByLanding(soModuleAccesser* moduleAccesser) {
    const wnFoxLandMasterParam* param = dynamic_cast<wnFoxLandMaster&>(moduleAccesser->getStageObject()).getParam();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float offset = work.getFloat(0x21000001);
    float direction = work.getFloat(0x21000004);
    if (0.0f == direction) {
        direction = -1.0f;
    }
    float rate = 0.0f;
    float amplitude = work.getFloat(0x21000008);
    if (amplitude > 0.0f) {
        offset += direction * (4.0f * amplitude / param->unkBC);
        if (offset > amplitude) {
            offset = amplitude - (offset - amplitude);
            direction = -1.0f;
        } else if (offset < -amplitude) {
            offset = -amplitude - (offset + amplitude);
            direction = 1.0f;
        }
        rate = param->unkC4 * (offset / amplitude);
    }
    work.setFloat(offset, 0x21000001);
    work.setFloat(rate, 0x21000003);
    work.setFloat(direction, 0x21000004);
}
