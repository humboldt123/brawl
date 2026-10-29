#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <mt/mt_prng.h>
#include <mt/mt_vector.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_module_hyp.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the Blue Falcon final smash link message to a caught fighter; after the message id come the placement
// (position, facing, Y rotation) and an optional ground-safe position.
struct ftLinkEventCaptainFinalMessage : soLinkEventArgs {
    int m_message; // +0x8
    Vec3f unkC;
    float unk18;
    float unk1C;
    bool unk20;
    Vec3f unk24;
};

// HYPOTHESIS: Captain Falcon's extend parameter group 6 (ftData extendParam[5]), read by the target statuses.
struct ftCaptainFinalTargetParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
};

class ftStatusUniqProcessCaptainFinalTarget : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalTarget() { }
    static void linkMessageProcedure(soLinkEventArgs* args, soModuleAccesser* moduleAccesser);
};

class ftStatusUniqProcessCaptainFinalStart : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftStatusUniqProcessCaptainFinalFuraFura : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalFuraFura() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

class ftStatusUniqProcessCaptainFinalClash : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalClash() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftStatusUniqProcessCaptainFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessCaptainFinalTarget g_ftStatusUniqProcessCaptainFinalTarget;
ftStatusUniqProcessCaptainFinalStart g_ftStatusUniqProcessCaptainFinalStart;
ftStatusUniqProcessCaptainFinalFuraFura g_ftStatusUniqProcessCaptainFinalFuraFura;
ftStatusUniqProcessCaptainFinalClash g_ftStatusUniqProcessCaptainFinalClash;
ftStatusUniqProcessCaptainFinalEnd g_ftStatusUniqProcessCaptainFinalEnd;

// MATCH-ONLY: the original keeps the one-caller fall step out of line.
static bool fall(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
static void set_clash_flying(soModuleAccesser* moduleAccesser);

// HYPOTHESIS: messages 0 (mark caught), 1 (start), 2 (scatter above the stage and drop in), 3 (clash), 4/7 (restore the
// sound), 5 (hit), 6 (place for the end), 7 also returns the fighter to status 0xFB.
void ftStatusUniqProcessCaptainFinalTarget::linkMessageProcedure(soLinkEventArgs* args, soModuleAccesser* moduleAccesser) {
    ftLinkEventCaptainFinalMessage* message = static_cast<ftLinkEventCaptainFinalMessage*>(args);
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    soMotionModule& motion = moduleAccesser->getMotionModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soGroundModule& ground = moduleAccesser->getGroundModule();
    switch (message->m_message) {
    case 0:
        workManage.onFlag(0x12000001);
        break;
    case 1:
        status.changeStatusRequest(0xFC, moduleAccesser);
        break;
    case 2: {
        status.changeStatusRequest(0xFD, moduleAccesser);
        workManage.setInt(0, 0x20000000);
        workManage.setInt(0, 0x20000001);
        workManage.setInt(0, 0x20000002);
        workManage.offFlag(0x12000019);
        workManage.offFlag(0x1200001B);
        {
            soPostureModule& posture = moduleAccesser->getPostureModule();
            ftData* data = g_ftCommonDataAccesser.getData(Fighter_Captain);
            Vec3f pos(0.0f, 0.0f, 0.0f);
            float radius = static_cast<ftCaptainFinalTargetParam*>(data->extendParam[5])->unk0;
            float radiusSq = radius * radius;
            do {
                pos.m_x = radius * (2.0f * randf() - 1.0f);
                pos.m_z = radius * (2.0f * randf() - 1.0f);
            } while (pos.m_x * pos.m_x + pos.m_y * pos.m_y + pos.m_z * pos.m_z >= radiusSq);
            pos.m_x += radius;
            pos.m_y += 3000.0f;
            randf();
            posture.setPos(&pos);
            posture.setLr(-1.0f);
            posture.setRot(&Vec3f(0.0f, 0.0f, 0.0f), 0);
            moduleAccesser->m_stageObject->updateNodeSRT();
        }
        soMotionChangeParam param;
        param.m_kind = 0x29;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        motion.changeMotionRequest(&param);
        {
            soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
            soPostureModule& posture = moduleAccesser->getPostureModule();
            ftData* data = g_ftCommonDataAccesser.getData(Fighter_Captain);
            Vec3f pos;
            pos = posture.getPos();
            pos.m_y = static_cast<ftCaptainFinalTargetParam*>(data->extendParam[5])->unk4;
            posture.setPos(&pos);
            workManage.setFloat(static_cast<ftCaptainFinalTargetParam*>(data->extendParam[5])->unk8, 0x21000004);
        }
        break;
    }
    case 3:
        status.changeStatusRequest(0xFD, moduleAccesser);
        workManage.setInt(1, 0x20000000);
        workManage.setInt(0, 0x20000001);
        workManage.setInt(0, 0x20000002);
        break;
    case 4: {
        soSoundModule& sound = moduleAccesser->getSoundModule();
        sound.activate(&posture.getPos());
        float pitch = workManage.getFloat(0x21000007);
        if (pitch != 1.0f) {
            sound.setSEPitch(pitch);
        }
        break;
    }
    case 5:
        status.changeStatusRequest(0xFF, moduleAccesser);
        workManage.onFlag(0x12000019);
        workManage.onFlag(0x1200001B);
        kinetic.clearSpeedAll();
        kinetic.mulSpeed(&Vec3f(0.0f, 0.0f, 0.0f), soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL));
        break;
    case 6:
        if (message->unk20 && ground.isNull() != true) {
            Vec3f safePos(message->unk24.m_x, message->unk24.m_y, message->unk24.m_z);
            posture.setPos(&safePos);
            ground.setShapeSafePos(&Vec2f(safePos.m_x, safePos.m_y), 0);
            ground.setCorrect(soGroundShapeImpl::CorrectKind(5), 0);
        }
        Vec3f pos;
        pos = message->unkC;
        posture.setPos(&pos);
        posture.setLr(message->unk18);
        if (message->unk20 && ground.isNull() != true) {
            ground.setCorrect(soGroundShapeImpl::CorrectKind(0), 0);
        }
        Vec3f rot;
        rot.m_x = 0.0f;
        rot.m_y = message->unk1C;
        rot.m_z = 0.0f;
        posture.setRot(&rot, 0);
        moduleAccesser->m_stageObject->updateNodeSRT();
        kinetic.clearSpeedAll();
        kinetic.mulSpeed(&Vec3f(0.0f, 0.0f, 0.0f), soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL));
        break;
    case 7: {
        soSoundModule& sound = moduleAccesser->getSoundModule();
        sound.activate(&posture.getPos());
        float pitch = workManage.getFloat(0x21000007);
        if (pitch != 1.0f) {
            sound.setSEPitch(pitch);
        }
        moduleAccesser->getKineticModule().clearSpeedAll();
        status.changeStatusRequest(0xFB, moduleAccesser);
        workManage.offFlag(0x22000010);
        break;
    }
    }
}

void ftStatusUniqProcessCaptainFinalStart::initStatus(soModuleAccesser* moduleAccesser) {
    soMotionModule& motion = moduleAccesser->getMotionModule();
    soMotionChangeParam param;
    param.m_kind = 0xA4;
    param.m_frame = 0.0f;
    param.m_rate = 1.0f;
    param._12 = 0;
    param._13 = 0;
    param._14 = 0;
    param._15 = 0;
    motion.changeMotionRequest(&param);
    getPhysicsModule(moduleAccesser).unk20();
    moduleAccesser->getWorkManageModule().setFloat(1.0f, 0x21000007);
}
void ftStatusUniqProcessCaptainFinalStart::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessCaptainFinalFuraFura::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    workManage.onFlag(0x12000001);
    workManage.offFlag(0x12000019);
    workManage.offFlag(0x1200001A);
    kinetic.changeKinetic(0xC, moduleAccesser);
    kinetic.clearSpeedAll();
    kinetic.unableEnergyAll();
    getPhysicsModule(moduleAccesser).unk20();
    moduleAccesser->getSlowModule().clear();
}

void ftStatusUniqProcessCaptainFinalFuraFura::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soMotionModule& motion = moduleAccesser->getMotionModule();
    int phase = workManage.getInt(0x20000000);
    int step = workManage.getInt(0x20000001);
    int frame = workManage.getInt(0x20000002);
    workManage.incInt(0x20000002);
    switch (phase) {
    case 0:
        switch (step) {
        case 0:
            if (fall(moduleAccesser)) {
                soMotionChangeParam param;
                param.m_kind = 0xAA;
                param.m_frame = 0.0f;
                param.m_rate = 1.0f;
                param._12 = 0;
                param._13 = 0;
                param._14 = 0;
                param._15 = 0;
                motion.changeMotionRequest(&param);
                workManage.incInt(0x20000001);
                workManage.setInt(0x15, 0x20000002);
            }
            break;
        case 1:
            if (frame >= 0x30) {
                soMotionChangeParam param;
                param.m_kind = 0xC6;
                param.m_frame = 0.0f;
                param.m_rate = 1.0f;
                param._12 = 0;
                param._13 = 0;
                param._14 = 0;
                param._15 = 0;
                motion.changeMotionRequest(&param);
                workManage.incInt(0x20000001);
            }
            break;
        }
        break;
    case 1:
        switch (step) {
        case 0:
            if (frame >= 0x27) {
                soMotionChangeParam param;
                param.m_kind = 0xC5;
                param.m_frame = 0.0f;
                param.m_rate = 1.0f;
                param._12 = 0;
                param._13 = 0;
                param._14 = 0;
                param._15 = 0;
                motion.changeMotionRequest(&param);
                workManage.incInt(0x20000001);
                soSoundModule& sound = moduleAccesser->getSoundModule();
                workManage.setFloat(sound.getSEPitch(), 0x21000007);
                sound.deactivate();
            }
            break;
        }
        break;
    }
}

// HYPOTHESIS: the fall speed grows by the parameter acceleration each frame and the drop ends at height 3000.
static bool fall(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    float accel = static_cast<ftCaptainFinalTargetParam*>(g_ftCommonDataAccesser.getData(Fighter_Captain)->extendParam[5])->unkC;
    bool landed = false;
    float speed = workManage.getFloat(0x21000004);
    speed += accel;
    workManage.setFloat(speed, 0x21000004);
    Vec3f pos;
    pos = posture.getPos();
    pos.m_y += speed;
    if (pos.m_y <= 3000.0f) {
        pos.m_y = 3000.0f;
        landed = true;
    }
    posture.setPos(&pos);
    return landed;
}

bool ftStatusUniqProcessCaptainFinalFuraFura::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    soStatusModule& status = moduleAccesser->getStatusModule();
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    status.changeStatusRequest(0xFE, moduleAccesser);
    workManage.setInt(2, 0x20000000);
    workManage.setInt(0, 0x20000001);
    workManage.setInt(0, 0x20000002);
    return true;
}

void ftStatusUniqProcessCaptainFinalClash::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    workManage.onFlag(0x12000001);
    workManage.offFlag(0x12000019);
    workManage.offFlag(0x1200001A);
    getPhysicsModule(moduleAccesser).unk20();
}

void ftStatusUniqProcessCaptainFinalClash::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    int phase = workManage.getInt(0x20000000);
    int step = workManage.getInt(0x20000001);
    workManage.getInt(0x20000002);
    workManage.incInt(0x20000002);
    switch (phase) {
    case 2:
        switch (step) {
        case 0:
            set_clash_flying(moduleAccesser);
            break;
        }
        break;
    }
}

static void set_clash_flying(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    ftKineticEnergyDamage& damage = dynamic_cast<ftKineticEnergyDamage&>(*kinetic.getEnergy(4));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    ftCaptainFinalTargetParam* param = static_cast<ftCaptainFinalTargetParam*>(g_ftCommonDataAccesser.getData(Fighter_Captain)->extendParam[5]);
    float speedX = param->unk10;
    float speedY = param->unk14;
    float brakeX = param->unk18;
    float gravityAccel = param->unk1C;
    kinetic.changeKinetic(0x1F, moduleAccesser);
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, speedY), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(4, moduleAccesser, 2, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    damage.m_speedLimit = Vec2f(speedX, 0.0f);
    damage.m_brake = Vec2f(brakeX, 0.0f);
    gravity.m_gravity = gravityAccel;
    gravity.unk1C = speedY;
    gravity.m_fallSpeedMax = speedY;
    gravity.unk18 = 0.0f;
}

void ftStatusUniqProcessCaptainFinalEnd::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().clearSpeedAll();
    getPhysicsModule(moduleAccesser).unk20();
}
void ftStatusUniqProcessCaptainFinalEnd::execStatus(soModuleAccesser* moduleAccesser) { }
