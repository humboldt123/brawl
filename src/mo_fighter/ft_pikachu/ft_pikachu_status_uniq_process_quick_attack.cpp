// MATCH-ONLY: the inline atan2f of the math header needs scheduling on (the original has its epilogue lwz before the frsp).
#pragma scheduling on
#include <math.h>
#pragma scheduling reset
#define MT_VEC3F_SET_NOINLINE
#define MT_VEC2F_MUL_ASSIGN_DIRECT
#include <ft/builder/ft_dol_array_list.h>
#include <ft/builder/ft_builder_kinetic.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/pikachu/ft_pikachu.h>
#include <ft/pikachu/ft_pikachu_status_uniq_process.h>
#include <math.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

float vec2fAngle(Vec2f*, Vec2f*);

// MATCH-ONLY: the fighter's controller module is only modelled as an opaque class (ft_dol_types.h); the original casts the
// accesser's controller module to it and calls the soControllerModule virtuals on the result.
static inline soControllerModule& fighterController(soModuleAccesser* moduleAccesser) {
    return reinterpret_cast<soControllerModule&>(dynamic_cast<ftControllerModuleImpl&>(moduleAccesser->getControllerModule()));
}

// Quick Attack can be turned into the follow-up (status 0x117) when the stick points away from the warp direction.
static inline bool isStickTurned(soModuleAccesser* moduleAccesser, soWorkManageModule& work, ftPikachuExtendParamQuickAttack* param) {
    if (work.getInt(ftPikachuWork::QuickAttack_Count) < 2) {
        soControllerModule& controller = fighterController(moduleAccesser);
        Vec2f stick(controller.getStickX(), controller.getStickY());
        if (stick.length() >= param->unk30) {
            float directionX = work.getFloat(ftPikachuWork::QuickAttack_DirectionX);
            float directionY = work.getFloat(ftPikachuWork::QuickAttack_DirectionY);
            Vec2f direction;
            direction.m_x = directionX;
            direction.m_y = directionY;
            if (vec2fAngle(&direction, &stick) > 0.017453292f * (float)param->unk4C) {
                return true;
            }
        }
    }
    return false;
}

void ftPikachuStatusUniqProcessSpecialQuickAttackStart::initStatus(soModuleAccesser* moduleAccesser) {
    ftPikachu& pikachu = dynamic_cast<ftPikachu&>(moduleAccesser->getStageObject());
    ftPikachuExtendParamQuickAttack* param = static_cast<ftPikachuExtendParamQuickAttack*>(pikachu.getExtendParam()[2]);
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    kinetic.getEnergy(0)->disable();
    kinetic.getEnergy(1)->disable();
    kinetic.getEnergy(2)->disable();
    kinetic.getEnergy(3)->disable();
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    gravity.m_gravity = param->unk08;
    work.setInt(param->unk00, ftPikachuWork::QuickAttack_StartFrames);
}

void ftPikachuStatusUniqProcessSpecialQuickAttackStart::execStatus(soModuleAccesser* moduleAccesser) {
    ftPikachu& pikachu = dynamic_cast<ftPikachu&>(moduleAccesser->getStageObject());
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int frames = work.getInt(ftPikachuWork::QuickAttack_StartFrames);
    work.setInt(frames - 1, ftPikachuWork::QuickAttack_StartFrames);
    if (moduleAccesser->getSituationModule().getKind() != 0) {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        if (frames - 1 >= 0) {
            gravity.disable();
            gravity.m_speedY = 0.0f;
        } else {
            ftPikachuExtendParamQuickAttack* param = static_cast<ftPikachuExtendParamQuickAttack*>(pikachu.getExtendParam()[2]);
            gravity.enable();
            gravity.m_gravity = param->unk08;
        }
    }
}

void ftPikachuStatusUniqProcessSpecialQuickAttackWarp::initStatus(soModuleAccesser* moduleAccesser) {
    ftPikachu& pikachu = dynamic_cast<ftPikachu&>(moduleAccesser->getStageObject());
    ftPikachuExtendParamQuickAttack* param = static_cast<ftPikachuExtendParamQuickAttack*>(pikachu.getExtendParam()[2]);
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    bool aimed = false;
    soControllerModule& controller = fighterController(moduleAccesser);
    Vec2f stick(controller.getStickX(), controller.getStickY());
    Vec2f direction;
    float length = stick.length();
    if (length > 0.999f) {
        length = 0.999f;
    }
    if (length < param->unk30) {
        aimed = true;
    } else {
        soGroundModule& ground = moduleAccesser->getGroundModule();
        if (ground.isTouch(static_cast<grCollStatus::TouchMask>(8), 0)) {
            Vec2f normal;
            normal = ground.getTouchNormal(8, 0);
            if (vec2fAngle(&normal, &stick) < 1.5707964f) {
                aimed = true;
            }
            if (ground.isPassableGround(0)) {
                aimed = true;
            }
        } else {
            aimed = true;
        }
    }
    if (aimed) {
        work.setInt(soValueAccesser::getConstantInt(moduleAccesser, 0x59db, 0), ftPikachuWork::QuickAttack_WarpTimer);
        float angle;
        float aimLength = stick.length();
        if (aimLength > 0.999f) {
            aimLength = 0.999f;
        }
        if (aimLength > param->unk30) {
            if (__fabsf(stick.m_x) > 0.001f) {
                moduleAccesser->getPostureModule().setStickLR(0.0f);
            }
            angle = atan2f(stick.m_y, stick.m_x);
        } else {
            angle = 1.5707964f;
        }
        direction.m_x = cos(angle);
        direction.m_y = sin(angle);
    } else {
        direction.m_x = stick.m_x;
        direction.m_y = stick.m_y;
        direction.normalize();
    }
    if (direction.m_x >= 0.00000011920929f) {
        moduleAccesser->getPostureModule().setLr(1.0f);
    } else if (direction.m_x <= -0.00000011920929f) {
        moduleAccesser->getPostureModule().setLr(-1.0f);
    }
    moduleAccesser->getPostureModule().updateRotYLr();
    work.setFloat(direction.m_x, ftPikachuWork::QuickAttack_DirectionX);
    work.setFloat(direction.m_y, ftPikachuWork::QuickAttack_DirectionY);
    length *= param->unk34;
    length += param->unk38;
    if (work.getInt(ftPikachuWork::QuickAttack_Count) >= 1) {
        length *= param->unk3C;
    }
    direction *= length;
    kinetic.getEnergy(0)->disable();
    kinetic.getEnergy(1)->disable();
    kinetic.getEnergy(3)->disable();
    soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(2));
    energy.m_speed = direction;
    energy.m_accel = Vec2f(0.0f, 0.0f);
    energy.m_speedTarget = direction;
    energy.m_speedLimit = direction;
    energy.enable();
    soMotionModule& motionModule = moduleAccesser->getMotionModule();
    soMotionChangeParam motion;
    motion.m_kind = 0x1db;
    motion.m_frame = 12.0f;
    motion.m_rate = 0.0f;
    motion._12 = 0;
    motion._13 = 0;
    motion._14 = 0;
    motion._15 = 0;
    motionModule.changeMotionRequest(&motion);
}

// Tilts the model's torso node towards the surface normal (grounded) or the flight direction (airborne) and scales it.
static inline void alignModelToMotion(soModuleAccesser* moduleAccesser) {
    ftPikachu& pikachu2 = dynamic_cast<ftPikachu&>(moduleAccesser->getStageObject());
    ftPikachuExtendParamQuickAttack* param2 = static_cast<ftPikachuExtendParamQuickAttack*>(pikachu2.getExtendParam()[2]);
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soModelModule& model = moduleAccesser->getModelModule();
    Vec3f rotation = model.getNodeRotate(3);
    Vec3f scale(1.0f, 1.0f, 1.0f);
    if (situation.getKind() == 0) {
        Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(0xff, 0);
        float lr = moduleAccesser->getPostureModule().getLr();
        float angle = 57.29578f * atan2f(normal.m_x, normal.m_y);
        rotation.m_x = angle * lr;
        rotation.m_x += 57.29578f * param2->unk0C;
        scale.set(param2->unk10, param2->unk14, param2->unk18);
    } else {
        Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::ATTRIBUTE_MASK_MAIN);
        // MATCH-ONLY: the double |x| temporary (assigned inside the call) keeps fabs in f0 and the frsp result in the argument register.
        double wide;
        rotation.m_x = 57.29578f * atan2f((float)(wide = fabs(speed.m_x)), speed.m_y);
        rotation.m_x += 57.29578f * param2->unk1C - 90.0f;
        scale.set(param2->unk20, param2->unk24, param2->unk28);
    }
    model.setNodeRotate(3, &rotation);
    model.setNodeScale(3, &scale);
}

void ftPikachuStatusUniqProcessSpecialQuickAttackWarp::execFixPos(soModuleAccesser* moduleAccesser) {
    ftPikachu& pikachu = dynamic_cast<ftPikachu&>(moduleAccesser->getStageObject());
    ftPikachuExtendParamQuickAttack* param = static_cast<ftPikachuExtendParamQuickAttack*>(pikachu.getExtendParam()[2]);
    alignModelToMotion(moduleAccesser);
    moduleAccesser->getMotionModule().updateFrame(1.0f);
    soGroundModule& ground = moduleAccesser->getGroundModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    Vec2f speedCopy = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::ATTRIBUTE_MASK_MAIN);
    Vec2f normal;
    if (ground.isTouch(static_cast<grCollStatus::TouchMask>(1), 0)) {
        normal = ground.getTouchNormal(1, 0);
        if (vec2fAngle(&speedCopy, &normal) > 0.017453292f * (90.0f + param->unk44)) {
            status.changeStatusRequest(0x118, moduleAccesser);
        }
    }
    if (ground.isTouch(static_cast<grCollStatus::TouchMask>(4), 0)) {
        normal = ground.getTouchNormal(4, 0);
        if (vec2fAngle(&speedCopy, &normal) > 0.017453292f * (90.0f + param->unk44)) {
            status.changeStatusRequest(0x118, moduleAccesser);
        }
    }
    if (ground.isTouch(static_cast<grCollStatus::TouchMask>(2), 0)) {
        normal = ground.getTouchNormal(2, 0);
        if (vec2fAngle(&speedCopy, &normal) > 0.017453292f * (90.0f + param->unk44)) {
            status.changeStatusRequest(0x118, moduleAccesser);
        }
    }
}

void ftPikachuStatusUniqProcessSpecialQuickAttackEnd::initStatus(soModuleAccesser* moduleAccesser) {
    int situationKind = moduleAccesser->getSituationModule().getKind();
    ftPikachu& pikachu = dynamic_cast<ftPikachu&>(moduleAccesser->getStageObject());
    ftPikachuExtendParamQuickAttack* param = static_cast<ftPikachuExtendParamQuickAttack*>(pikachu.getExtendParam()[2]);
    if (situationKind == 0) {
        Vec2f speedCopy = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::ATTRIBUTE_MASK_MAIN);
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        speedCopy *= param->unk48;
        stop.resetEnergy(7, &Vec2f(speedCopy.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, speedCopy.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2)).clearSpeed();
        moduleAccesser->getKineticModule().getEnergy(2)->disable();
    } else {
        Vec2f speed;
        speed.m_x = 0.0f;
        speed.m_y = 0.0f;
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        speed *= param->unk48;
        stop.resetEnergy(7, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2)).clearSpeed();
        moduleAccesser->getKineticModule().getEnergy(2)->disable();
    }
    int count = moduleAccesser->getWorkManageModule().getInt(ftPikachuWork::QuickAttack_Count);
    moduleAccesser->getWorkManageModule().setInt(count + 1, ftPikachuWork::QuickAttack_Count);
    moduleAccesser->getWorkManageModule().setInt(0, ftPikachuWork::QuickAttack_EndState);
}

void ftPikachuStatusUniqProcessSpecialQuickAttackEnd::execStatus(soModuleAccesser* moduleAccesser) {
    ftPikachu& pikachu = dynamic_cast<ftPikachu&>(moduleAccesser->getStageObject());
    ftPikachuExtendParamQuickAttack* param = static_cast<ftPikachuExtendParamQuickAttack*>(pikachu.getExtendParam()[2]);
    int state = moduleAccesser->getWorkManageModule().getInt(ftPikachuWork::QuickAttack_EndState);
    int newState = state;
    if (moduleAccesser->getSituationModule().getKind() == 0) {
        if (state != 0) {
            newState = 2;
            moduleAccesser->getKineticModule().changeKinetic(6, moduleAccesser);
        }
    } else if (state <= 0) {
        // HYPOTHESIS: the original leaves the state at 0 here (no store of 1 in the native code); something else advances it.
        soKineticModule& kinetic = moduleAccesser->getKineticModule();
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        Vec2f speed;
        speed = gravity.getSpeed();
        speed.m_y = speed.m_y - speed.m_y / 9.0f;
        gravity.m_gravity = 0.0f;
        gravity.m_speedY = speed.m_y;
        gravity.enable();
        ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*kinetic.getEnergy(2));
        float brake = soValueAccesser::getConstantFloat(moduleAccesser, 0xbd6, 0);
        controller.m_brake = Vec2f(brake, brake);
        controller.enable();
    } else {
        if (state == 1) {
            bool leave = isStickTurned(moduleAccesser, moduleAccesser->getWorkManageModule(), static_cast<ftPikachuExtendParamQuickAttack*>(dynamic_cast<ftPikachu&>(moduleAccesser->getStageObject()).getExtendParam()[2]));
            if (leave) {
                moduleAccesser->getStatusModule().changeStatusRequest(0x117, moduleAccesser);
                return;
            }
            newState = 2;
        }
        soKineticModule& kinetic = moduleAccesser->getKineticModule();
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, 0xbcf, 0);
        gravity.enable();
        ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*kinetic.getEnergy(2));
        float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xbd5, 0);
        controller.m_speedLimit = Vec2f(limit * param->unk40, 0.0f);
        controller.enable();
    }
    if (newState != state) {
        moduleAccesser->getWorkManageModule().setInt(newState, ftPikachuWork::QuickAttack_EndState);
    }
}

void ftPikachuStatusUniqProcessSpecialQuickAttackEnd::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        ftPikachu& pikachu = dynamic_cast<ftPikachu&>(moduleAccesser->getStageObject());
        ftPikachuExtendParamQuickAttack* param = static_cast<ftPikachuExtendParamQuickAttack*>(pikachu.getExtendParam()[2]);
        moduleAccesser->getWorkManageModule().setFloat(param->unk54, ftPikachuWork::QuickAttack_ExitSpeedY);
        moduleAccesser->getWorkManageModule().setFloat(param->unk50, ftPikachuWork::QuickAttack_ExitSpeedX);
        moduleAccesser->getWorkManageModule().onFlag(ftPikachuWork::QuickAttack_Ended);
    }
}

ftPikachuStatusUniqProcessSpecialQuickAttackStart g_ftPikachuStatusUniqProcessSpecialQuickAttackStart;
ftPikachuStatusUniqProcessSpecialQuickAttackWarp g_ftPikachuStatusUniqProcessSpecialQuickAttackWarp;
ftPikachuStatusUniqProcessSpecialQuickAttackEnd g_ftPikachuStatusUniqProcessSpecialQuickAttackEnd;
