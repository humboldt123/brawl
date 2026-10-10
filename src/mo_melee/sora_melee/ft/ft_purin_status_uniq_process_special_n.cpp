#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/purin/ft_purin_status_uniq_process_special_n.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/fighter.h>
#include <ft/ft_log_transactor.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <mt/mt_prng.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/so_value_accesser.h>

void ftKineticEnergyDisableAndClear(int, soModuleAccesser*);

// Shared native deformation tables. Their enclosing source owner is unresolved.
extern float lbl_27_data_A2010[4];
extern float lbl_27_data_A2020[4];

void ftPurinStatusUniqProcessSpecialNStart::initStatus(soModuleAccesser* acc) {
    float direction = acc->getPostureModule().getLr();
    acc->getWorkManageModule().setFloat(direction, 0x2100000a);
    int turnWindow = soValueAccesser::getConstantInt(acc, 0x5a03, 0);
    acc->getWorkManageModule().setInt(turnWindow, 0x2000000d);
    acc->getWorkManageModule().setFloat(direction, 0x21000011);
    ftPurinStatusUniqProcessSpecialNUtility::ftPurinSpecialNInit(acc);
    if (acc->getSituationModule().getKind() == Situation_Ground) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        // A tiny facing-dependent seed preserves direction while charging.
        float seed = acc->getWorkManageModule().getFloat(0x21000011) * 0.0001f;
        Vec3f groundResetRotation(0.0f, 0.0f, 0.0f);
        Vec2f initialSpeed = Vec2f(seed, 0.0f);
        stop.resetEnergy(0x16, &initialSpeed, &groundResetRotation, acc);
        Vec2f targetSpeed = Vec2f(seed, 0.0f);
        stop.m_speedTarget = targetSpeed;
        stop.onConsiderGroundFriction();
        stop.enable();
        acc->getWorkManageModule().setFloat(acc->getConstantFloatKirby(0xfa2), 0x21000008);
        ftKineticEnergyDisableAndClear(1, acc);
    } else {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
        Vec2f speed;
        Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        Vec3f airResetRotation(0.0f, 0.0f, 0.0f);
        stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &airResetRotation, acc);
        stop.enable();
        Vec3f gravityResetRotation(0.0f, 0.0f, 0.0f);
        gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &gravityResetRotation, acc);
        gravity.m_gravity = -acc->getConstantFloatKirby(0xfa0);
        gravity.unk1C = acc->getConstantFloatKirby(0xfa1);
        gravity.m_fallSpeedMax = acc->getConstantFloatKirby(0xfa1);
        gravity.enable();
    }
    acc->getWorkManageModule().setInt(acc->getSituationModule().getKind(), 0x20000008);
    ftKineticEnergyDisableAndClear(2, acc);
    ftKineticEnergyDisableAndClear(0, acc);
}

void ftPurinStatusUniqProcessSpecialNStart::execStatus(soModuleAccesser* acc) {
    SituationKind situation = acc->getSituationModule().getKind();
    int previous = acc->getWorkManageModule().getInt(0x20000008);
    acc->getWorkManageModule().setFloat(0.0f, 0x21000009);
    checkStartTurn(acc);
    // Restore the appropriate kinetic setup when startup crosses ground/air.
    if (situation != previous) {
        if (situation == Situation_Ground) {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
            float seed = acc->getWorkManageModule().getFloat(0x21000011) * 0.0001f;
            Vec3f groundResetRotation(0.0f, 0.0f, 0.0f);
            Vec2f initialSpeed = Vec2f(seed, 0.0f);
            stop.resetEnergy(0x16, &initialSpeed, &groundResetRotation, acc);
            Vec2f targetSpeed = Vec2f(seed, 0.0f);
            stop.m_speedTarget = targetSpeed;
            stop.onConsiderGroundFriction();
            stop.enable();
            acc->getWorkManageModule().setFloat(acc->getConstantFloatKirby(0xfa2), 0x21000008);
            ftKineticEnergyDisableAndClear(1, acc);
        } else {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
            Vec2f speed;
            Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
            Vec3f airResetRotation(0.0f, 0.0f, 0.0f);
            stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &airResetRotation, acc);
            stop.enable();
            Vec3f gravityResetRotation(0.0f, 0.0f, 0.0f);
            gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &gravityResetRotation, acc);
            gravity.m_gravity = -acc->getConstantFloatKirby(0xfa0);
            gravity.unk1C = acc->getConstantFloatKirby(0xfa1);
            gravity.m_fallSpeedMax = acc->getConstantFloatKirby(0xfa1);
            gravity.enable();
        }
    }
    acc->getWorkManageModule().setInt(situation, 0x20000008);
}

// The turn window is counted down even when the stick remains in its dead zone.
// On its final frame, choose a direction and request the startup turn if needed.
void ftPurinStatusUniqProcessSpecialNStart::checkStartTurn(soModuleAccesser* acc) {
    int remaining = acc->getWorkManageModule().getInt(0x2000000d);
    if (remaining > 0) {
        if (remaining - 1 <= 0) {
            float direction = acc->getWorkManageModule().getFloat(0x21000011);
            float stick = acc->getControllerModule().getStickX();
            if (stick > soValueAccesser::getConstantFloat(acc, 0xc70, 0)) {
                direction = 1.0f;
            } else if (stick < -soValueAccesser::getConstantFloat(acc, 0xc70, 0)) {
                direction = -1.0f;
            }
            if (direction != acc->getPostureModule().getLr()) {
                acc->getWorkManageModule().onFlag(0x22000014);
                acc->getWorkManageModule().setFloat(direction, 0x21000011);
            }
        }
        acc->getWorkManageModule().setInt(remaining - 1, 0x2000000d);
    }
}

void ftPurinStatusUniqProcessSpecialNStart::exitStatus(soModuleAccesser*, int) { }

void ftPurinStatusUniqProcessSpecialNHold::initStatus(soModuleAccesser* acc) {
    // Apply a direction selected during the startup turn window.
    if (acc->getWorkManageModule().isFlag(0x22000014)) {
        float direction = acc->getWorkManageModule().getFloat(0x21000011);
        acc->getPostureModule().setLr(direction);
        acc->getPostureModule().updateRotYLr();
        acc->getStageObject().updateNodeSRT();
        acc->getWorkManageModule().setFloat(direction, 0x2100000a);
    }
    if (acc->getSituationModule().getKind() == Situation_Ground) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        // A tiny facing-dependent seed preserves direction while charging.
        float seed = acc->getWorkManageModule().getFloat(0x21000011) * 0.0001f;
        Vec3f groundResetRotation(0.0f, 0.0f, 0.0f);
        Vec2f initialSpeed = Vec2f(seed, 0.0f);
        stop.resetEnergy(0x16, &initialSpeed, &groundResetRotation, acc);
        Vec2f targetSpeed = Vec2f(seed, 0.0f);
        stop.m_speedTarget = targetSpeed;
        stop.onConsiderGroundFriction();
        stop.enable();
        acc->getWorkManageModule().setFloat(acc->getConstantFloatKirby(0xfa2), 0x21000008);
        ftKineticEnergyDisableAndClear(1, acc);
    } else {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
        Vec2f speed;
        Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        Vec3f airResetRotation(0.0f, 0.0f, 0.0f);
        stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &airResetRotation, acc);
        stop.enable();
        Vec3f gravityResetRotation(0.0f, 0.0f, 0.0f);
        gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &gravityResetRotation, acc);
        gravity.m_gravity = -acc->getConstantFloatKirby(0xfa0);
        gravity.unk1C = acc->getConstantFloatKirby(0xfa1);
        gravity.m_fallSpeedMax = acc->getConstantFloatKirby(0xfa1);
        gravity.enable();
    }
    acc->getWorkManageModule().setInt(acc->getSituationModule().getKind(), 0x20000008);
    int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f facing = acc->getModelModule().getNodeRotate(node);
    if (-1.0f == acc->getPostureModule().getLr()) {
        facing.m_y = 180.0f;
    } else {
        facing.m_y = 0.0f;
    }
    acc->getModelModule().setNodeRotate(node, &facing);
    ftKineticEnergyDisableAndClear(2, acc);
    ftKineticEnergyDisableAndClear(0, acc);
    acc->getControllerModule().setRumble(9, 0, true, -1);
    if (acc->getStageObject().soGetSubKind() == 5) {
        acc->getSoundModule().playStatusSE(static_cast<SndID>(0x1b17), true, 0);
    } else {
        acc->getSoundModule().playStatusSE(static_cast<SndID>(0x17a0), true, 0);
    }
}

void ftPurinStatusUniqProcessSpecialNHold::execStatus(soModuleAccesser* acc) {
    SituationKind situation = acc->getSituationModule().getKind();
    int previous = acc->getWorkManageModule().getInt(0x20000008);
    acc->getWorkManageModule().setFloat(0.0f, 0x21000009);
    acc->getWorkManageModule().addFloat(acc->getConstantFloatKirby(0xfb9), 0x21000005);
    soWorkManageModule& work = acc->getWorkManageModule();
    float maximum = acc->getConstantFloatKirby(0xfb8);
    if (maximum <= work.getFloat(0x21000005)) {
        acc->getWorkManageModule().setFloat(acc->getConstantFloatKirby(0xfb8), 0x21000005);
        acc->getWorkManageModule().onFlag(0x22000010);
        if (acc->getStageObject().soGetSubKind() == 5) {
            acc->getStatusModule().changeStatusRequest(0x186, acc);
        } else {
            acc->getStatusModule().changeStatusRequest(0x118, acc);
        }
    }
    float angle = acc->getWorkManageModule().getFloat(0x21000006);
    float charge = acc->getWorkManageModule().getFloat(0x21000005);
    float increment = acc->getConstantFloatKirby(0xfba) * 0.017453292f;
    float direction = acc->getWorkManageModule().getFloat(0x2100000a);
    angle += (charge * increment) * direction;
    // The phase is stored in radians, while model-node rotation uses degrees.
    while (angle < 0.0f) {
        angle += 6.2831855f;
    }
    while (angle > 6.2831855f) {
        angle -= 6.2831855f;
    }
    acc->getWorkManageModule().setFloat(angle, 0x21000006);
    angle = acc->getWorkManageModule().getFloat(0x21000006);
    int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f rotation = acc->getModelModule().getNodeRotate(node);
    rotation.m_x = 57.29578f * angle;
    acc->getModelModule().setNodeRotate(node, &rotation);

    node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f facing = acc->getModelModule().getNodeRotate(node);
    if (-1.0f == acc->getPostureModule().getLr()) {
        facing.m_y = 180.0f;
    } else {
        facing.m_y = 0.0f;
    }
    acc->getModelModule().setNodeRotate(node, &facing);
    if (situation != previous) {
        if (situation == Situation_Ground) {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
            float seed = acc->getWorkManageModule().getFloat(0x21000011) * 0.0001f;
            Vec3f groundResetRotation(0.0f, 0.0f, 0.0f);
            Vec2f initialSpeed = Vec2f(seed, 0.0f);
            stop.resetEnergy(0x16, &initialSpeed, &groundResetRotation, acc);
            Vec2f targetSpeed = Vec2f(seed, 0.0f);
            stop.m_speedTarget = targetSpeed;
            stop.onConsiderGroundFriction();
            stop.enable();
            acc->getWorkManageModule().setFloat(acc->getConstantFloatKirby(0xfa2), 0x21000008);
            ftKineticEnergyDisableAndClear(1, acc);
        } else {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
            Vec2f speed;
            Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
            Vec3f airResetRotation(0.0f, 0.0f, 0.0f);
            stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &airResetRotation, acc);
            stop.enable();
            Vec3f gravityResetRotation(0.0f, 0.0f, 0.0f);
            gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &gravityResetRotation, acc);
            gravity.m_gravity = -acc->getConstantFloatKirby(0xfa0);
            gravity.unk1C = acc->getConstantFloatKirby(0xfa1);
            gravity.m_fallSpeedMax = acc->getConstantFloatKirby(0xfa1);
            gravity.enable();
        }
    }
    acc->getWorkManageModule().setInt(situation, 0x20000008);
}

void ftPurinStatusUniqProcessSpecialNHold::execStop(soModuleAccesser* acc) {
    float angle = acc->getWorkManageModule().getFloat(0x21000006);
    int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f rotation = acc->getModelModule().getNodeRotate(node);
    rotation.m_x = 57.29578f * angle;
    acc->getModelModule().setNodeRotate(node, &rotation);

    node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f facing = acc->getModelModule().getNodeRotate(node);
    if (-1.0f == acc->getPostureModule().getLr()) {
        facing.m_y = 180.0f;
    } else {
        facing.m_y = 0.0f;
    }
    acc->getModelModule().setNodeRotate(node, &facing);
}
void ftPurinStatusUniqProcessSpecialNHold::exitStatus(soModuleAccesser*, int) { }

void ftPurinStatusUniqProcessSpecialNHoldMax::initStatus(soModuleAccesser* acc) {
    if (acc->getSituationModule().getKind() == Situation_Ground) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        // A tiny facing-dependent seed preserves direction while charging.
        float seed = acc->getWorkManageModule().getFloat(0x21000011) * 0.0001f;
        Vec3f groundResetRotation(0.0f, 0.0f, 0.0f);
        Vec2f initialSpeed = Vec2f(seed, 0.0f);
        stop.resetEnergy(0x16, &initialSpeed, &groundResetRotation, acc);
        Vec2f targetSpeed = Vec2f(seed, 0.0f);
        stop.m_speedTarget = targetSpeed;
        stop.onConsiderGroundFriction();
        stop.enable();
        acc->getWorkManageModule().setFloat(acc->getConstantFloatKirby(0xfa2), 0x21000008);
        ftKineticEnergyDisableAndClear(1, acc);
    } else {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
        Vec2f speed;
        Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        Vec3f airResetRotation(0.0f, 0.0f, 0.0f);
        stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &airResetRotation, acc);
        stop.enable();
        Vec3f gravityResetRotation(0.0f, 0.0f, 0.0f);
        gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &gravityResetRotation, acc);
        gravity.m_gravity = -acc->getConstantFloatKirby(0xfa0);
        gravity.unk1C = acc->getConstantFloatKirby(0xfa1);
        gravity.m_fallSpeedMax = acc->getConstantFloatKirby(0xfa1);
        gravity.enable();
    }
    acc->getWorkManageModule().setInt(acc->getSituationModule().getKind(), 0x20000008);
    int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f facing = acc->getModelModule().getNodeRotate(node);
    if (-1.0f == acc->getPostureModule().getLr()) {
        facing.m_y = 180.0f;
    } else {
        facing.m_y = 0.0f;
    }
    acc->getModelModule().setNodeRotate(node, &facing);
    ftKineticEnergyDisableAndClear(2, acc);
    ftKineticEnergyDisableAndClear(0, acc);
    acc->getWorkManageModule().setInt(0, 0x2000000a);
    acc->getWorkManageModule().setInt(0, 0x2000000b);
    acc->getWorkManageModule().setInt(0, 0x2000000c);
    acc->getControllerModule().stopRumbleKind(9, -1);
    acc->getControllerModule().setRumble(2, 0, true, -1);
    if (!acc->getWorkManageModule().isFlag(0x22000013)) {
        acc->getEffectModule().reqCommon(0x23, 0.0f);
        acc->getWorkManageModule().onFlag(0x22000013);
    }
    if (acc->getStageObject().soGetSubKind() == 5) {
        acc->getSoundModule().playStatusSE(static_cast<SndID>(0x1b18), true, 0);
        acc->getSoundModule().playSE(static_cast<SndID>(0x1b15), true, true, 0);
    } else {
        acc->getSoundModule().playStatusSE(static_cast<SndID>(0x17a1), true, 0);
        acc->getSoundModule().playStatusSE(static_cast<SndID>(0x9b5), true, 0);
    }
}

void ftPurinStatusUniqProcessSpecialNHoldMax::execStatus(soModuleAccesser* acc) {
    SituationKind situation = acc->getSituationModule().getKind();
    int previous = acc->getWorkManageModule().getInt(0x20000008);
    acc->getWorkManageModule().setFloat(0.0f, 0x21000009);
    float angle = acc->getWorkManageModule().getFloat(0x21000006);
    float charge = acc->getWorkManageModule().getFloat(0x21000005);
    float increment = acc->getConstantFloatKirby(0xfba) * 0.017453292f;
    float direction = acc->getWorkManageModule().getFloat(0x2100000a);
    float phaseStep = charge * increment;
    phaseStep *= direction;
    angle += phaseStep;
    // The phase is stored in radians, while model-node rotation uses degrees.
    while (angle < 0.0f) {
        angle += 6.2831855f;
    }
    while (angle > 6.2831855f) {
        angle -= 6.2831855f;
    }
    acc->getWorkManageModule().setFloat(angle, 0x21000006);
    angle = acc->getWorkManageModule().getFloat(0x21000006);
    int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f rotation = acc->getModelModule().getNodeRotate(node);
    rotation.m_x = 57.29578f * angle;
    acc->getModelModule().setNodeRotate(node, &rotation);

    node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f facing = acc->getModelModule().getNodeRotate(node);
    if (-1.0f == acc->getPostureModule().getLr()) {
        facing.m_y = 180.0f;
    } else {
        facing.m_y = 0.0f;
    }
    acc->getModelModule().setNodeRotate(node, &facing);
    if (situation != previous) {
        if (situation == Situation_Ground) {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
            float seed = acc->getWorkManageModule().getFloat(0x21000011) * 0.0001f;
            Vec3f groundResetRotation(0.0f, 0.0f, 0.0f);
            Vec2f initialSpeed = Vec2f(seed, 0.0f);
            stop.resetEnergy(0x16, &initialSpeed, &groundResetRotation, acc);
            Vec2f targetSpeed = Vec2f(seed, 0.0f);
            stop.m_speedTarget = targetSpeed;
            stop.onConsiderGroundFriction();
            stop.enable();
            acc->getWorkManageModule().setFloat(acc->getConstantFloatKirby(0xfa2), 0x21000008);
            ftKineticEnergyDisableAndClear(1, acc);
        } else {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
            Vec2f speed;
            Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
            Vec3f airResetRotation(0.0f, 0.0f, 0.0f);
            stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &airResetRotation, acc);
            stop.enable();
            Vec3f gravityResetRotation(0.0f, 0.0f, 0.0f);
            gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &gravityResetRotation, acc);
            gravity.m_gravity = -acc->getConstantFloatKirby(0xfa0);
            gravity.unk1C = acc->getConstantFloatKirby(0xfa1);
            gravity.m_fallSpeedMax = acc->getConstantFloatKirby(0xfa1);
            gravity.enable();
        }
    }
    acc->getWorkManageModule().setInt(situation, 0x20000008);
}

void ftPurinStatusUniqProcessSpecialNHoldMax::execFixPosCounter(soModuleAccesser* acc) {
    // Fully charged Rollout emits a dust burst and a sparkle every ten frames.
    acc->getWorkManageModule().addInt(1, 0x2000000a);
    if (acc->getWorkManageModule().getInt(0x2000000a) > 9) {
        float slopeAngle = 0.0f;
        if (acc->getSituationModule().getKind() == Situation_Ground) {
            Vec2f normal = acc->getGroundModule().getTouchNormal(grCollStatus::TOUCH_MASK_DOWN, 0);
            slopeAngle = atan2(-normal.m_x, normal.m_y);
        }
        float direction = acc->getPostureModule().getLr();
        Vec3f offset(0.0f, 0.0f, 0.0f);
        offset.m_x = randf() * 5.0f;
        offset.m_z = randf() * 5.0f;
        Vec3f position = acc->getPostureModule().getPos() + offset;
        Vec3f rotation(0.0f, direction == 1.0f ? 1.5707964f : -1.5707964f, slopeAngle);
        acc->getEffectModule().req(static_cast<EfID>(10), &position, &rotation, 1.0f, 0, -1);
        acc->getWorkManageModule().setInt(0, 0x2000000a);
    }
    acc->getWorkManageModule().addInt(1, 0x2000000b);
    if (acc->getWorkManageModule().getInt(0x2000000b) > 9) {
        EfID effect = static_cast<EfID>(acc->getStageObject().soGetSubKind() == 5 ? 0x1260001 : 0x260002);
        int node = acc->getStageObject().soGetSubKind() == 5 ? 0x199 : 6;
        Vec3f rotation(0.0f, 0.0f, 0.0f);
        Vec3f position(0.0f, 0.0f, 0.0f);
        acc->getEffectModule().reqFollow(effect, node, &position, &rotation, 1.0f, false, 0, 0, -1);
        acc->getWorkManageModule().setInt(0, 0x2000000b);
    }
}

void ftPurinStatusUniqProcessSpecialNHoldMax::execStop(soModuleAccesser* acc) {
    float angle = acc->getWorkManageModule().getFloat(0x21000006);
    int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f rotation = acc->getModelModule().getNodeRotate(node);
    rotation.m_x = 57.29578f * angle;
    acc->getModelModule().setNodeRotate(node, &rotation);

    node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f facing = acc->getModelModule().getNodeRotate(node);
    if (-1.0f == acc->getPostureModule().getLr()) {
        facing.m_y = 180.0f;
    } else {
        facing.m_y = 0.0f;
    }
    acc->getModelModule().setNodeRotate(node, &facing);
}
void ftPurinStatusUniqProcessSpecialNHoldMax::exitStatus(soModuleAccesser* acc, int nextStatus) {
    // Keep the rolling effect across the transition into either rolling state.
    switch (nextStatus) {
    case 0x119:
    case 0x11a:
        return;
    default:
        if (acc->getWorkManageModule().isFlag(0x22000013)) {
            acc->getEffectModule().removeCommon(0x23);
            acc->getWorkManageModule().offFlag(0x22000013);
        }
        break;
    }
}

void ftPurinStatusUniqProcessSpecialNRoll::initStatus(soModuleAccesser* acc) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    int turnStatus, airStatus;
    if (acc->getStageObject().soGetSubKind() == 5) {
        turnStatus = 0x189;
        airStatus = 0x188;
    } else {
        turnStatus = 0x11b;
        airStatus = 0x11a;
    }
    if (turnStatus == acc->getStatusModule().getPrevStatusKind(0)) {
        float direction = acc->getWorkManageModule().getFloat(0x21000009);
        if (direction != 0.0f) {
            acc->getWorkManageModule().setFloat(direction, 0x2100000a);
            acc->getPostureModule().setLr(direction);
            acc->getPostureModule().updateRotYLr();
            acc->getStageObject().updateNodeSRT();
            acc->getWorkManageModule().setFloat(0.0f, 0x21000009);
            acc->getWorkManageModule().setInt(0, 0x20000003);
        }
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
        Vec2f speed;
        Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        stop.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
        stop.m_accel = Vec2f(0.0f, 0.0f);
        stop.m_speedTarget = Vec2f(0.0f, 0.0f);
        stop.m_brake = Vec2f(0.0f, 0.0f);
        stop.m_speedLimit = Vec2f(-1.0f, -1.0f);
        stop.offConsiderGroundFriction();
        stop.enable();
        ftKineticEnergyDisableAndClear(1, acc);
        // Re-entering Rollout after a turn starts a new action-log entry.
        Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
        ftLogTransactor::resetLogActionInfo(2, 0x13, 1, fighter.getOwner(), acc);
    } else {
        float horizontal;
        if (airStatus == acc->getStatusModule().getPrevStatusKind(0)) {
            soWorkManageModule& work = acc->getWorkManageModule();
            work.setFloat(acc->getConstantFloatKirby(0xfa2), 0x21000008);
            soWorkManageModule& speedWork = acc->getWorkManageModule();
            float direction = speedWork.getFloat(0x2100000a);
            horizontal = speedWork.getFloat(0x21000007) * direction;
            acc->getControllerModule().setRumble(0x17, 0, true, -1);
        } else {
            soWorkManageModule& work = acc->getWorkManageModule();
            float minimum = acc->getConstantFloatKirby(0xfbd);
            float charge = work.getFloat(0x21000005) - minimum;
            charge = acc->getConstantFloatKirby(0xfbf) * charge;
            horizontal = work.getFloat(0x2100000a) * charge;
        }
        Vec2f speed(horizontal, 0.0f);
        stop.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
        stop.m_accel = Vec2f(0.0f, 0.0f);
        stop.m_speedTarget = Vec2f(0.0f, 0.0f);
        stop.m_brake = Vec2f(0.0f, 0.0f);
        stop.m_speedLimit = Vec2f(-1.0f, -1.0f);
        stop.offConsiderGroundFriction();
        stop.enable();
        ftKineticEnergyDisableAndClear(1, acc);
    }
    int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f facing = acc->getModelModule().getNodeRotate(node);
    if (acc->getPostureModule().getLr() == -1.0f) {
        facing.m_y = 180.0f;
    } else {
        facing.m_y = 0.0f;
    }
    acc->getModelModule().setNodeRotate(node, &facing);
    ftKineticEnergyDisableAndClear(2, acc);
    ftKineticEnergyDisableAndClear(0, acc);
}

void ftPurinStatusUniqProcessSpecialNRoll::execStatus(soModuleAccesser* acc) {
    SituationKind situation = acc->getSituationModule().getKind();
    int previous = acc->getWorkManageModule().getInt(0x20000008);
    acc->getWorkManageModule().setFloat(0.0f, 0x21000009);
    Vec3f scale(1.0f, 1.0f, 1.0f);
    int frame = acc->getWorkManageModule().getInt(0x20000002);
    if (frame >= 0 && frame < 4) {
        scale.m_x = 1.0f;
        scale.m_y *= lbl_27_data_A2010[frame];
        scale.m_z *= lbl_27_data_A2020[frame];
        acc->getWorkManageModule().addInt(1, 0x20000002);
    } else {
        scale.m_x = 1.0f;
        scale.m_y = 1.0f;
        scale.m_z = 1.0f;
    }
    int scaleKind = acc->getStageObject().soGetSubKind();
    acc->getModelModule().setNodeScale(scaleKind == 5 ? 0x196 : 3, &scale);
    float oldAngle = acc->getWorkManageModule().getFloat(0x21000006);
    float direction = acc->getWorkManageModule().getFloat(0x2100000a);
    float increment = (acc->getConstantFloatKirby(0xfb6) * 0.2f) * direction;
    if (acc->getSituationModule().getKind() == Situation_Ground) {
        increment *= acc->getWorkManageModule().getFloat(0x21000005) * 0.017453292f;
    } else {
        soWorkManageModule& work = acc->getWorkManageModule();
        float airFactor = acc->getConstantFloatKirby(0xfbe);
        increment *= (work.getFloat(0x21000005) * 0.017453292f) * airFactor;
    }
    float newAngle = oldAngle + increment;
    while (newAngle < 0.0f) {
        newAngle += 6.2831855f;
    }
    while (newAngle > 6.2831855f) {
        newAngle -= 6.2831855f;
    }
    acc->getWorkManageModule().setFloat(newAngle, 0x21000006);
    acc->getWorkManageModule().subInt(1, 0x20000000);
    // Finish only after the timed roll crosses the recovery phase of its rotation.
    if (acc->getWorkManageModule().getInt(0x20000000) <= 0 &&
        newAngle > 1.5707964f && newAngle < 4.712389f) {
        if (increment > 0.0f) {
            if (newAngle > 3.1415927f && oldAngle < 3.1415927f) {
                acc->getWorkManageModule().setInt(0, 0x20000000);
                acc->getWorkManageModule().onFlag(0x22000015);
            }
        } else {
            if (newAngle < 3.1415927f && oldAngle > 3.1415927f) {
                acc->getWorkManageModule().setInt(0, 0x20000000);
                acc->getWorkManageModule().onFlag(0x22000015);
            }
        }
    }
    float stick = acc->getControllerModule().getStickX();
    float stickMagnitude = __fabsf(stick);
    if (stickMagnitude > acc->getConstantFloatKirby(0xfab)) {
        float desiredDirection = stick > 0.0f ? 1.0f : -1.0f;
        if (desiredDirection != acc->getWorkManageModule().getFloat(0x2100000a)) {
            acc->getCollisionAttackModule().clearAll();
            acc->getWorkManageModule().setFloat(desiredDirection, 0x21000009);
            acc->getWorkManageModule().setFloat(-desiredDirection, 0x2100000a);
            if (acc->getStageObject().soGetSubKind() == 5) {
                acc->getStatusModule().changeStatusRequest(0x189, acc);
            } else {
                acc->getStatusModule().changeStatusRequest(0x11b, acc);
            }
        }
    }
    if (situation == Situation_Ground) {
        if (situation != previous) {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
            dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
            Vec2f speed;
            Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
            stop.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
            stop.m_accel = Vec2f(0.0f, 0.0f);
            stop.m_speedTarget = Vec2f(0.0f, 0.0f);
            stop.m_brake = Vec2f(0.0f, 0.0f);
            stop.m_speedLimit = Vec2f(-1.0f, -1.0f);
            stop.offConsiderGroundFriction();
            stop.enable();
            ftKineticEnergyDisableAndClear(1, acc);
        }
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        soWorkManageModule& work = acc->getWorkManageModule();
        float minimum = acc->getConstantFloatKirby(0xfbd);
        float charge = work.getFloat(0x21000005) - minimum;
        charge = acc->getConstantFloatKirby(0xfbf) * charge;
        charge = work.getFloat(0x2100000a) * charge;
        float horizontal = charge;
        Vec2f normal;
        Vec2f::copy(normal, acc->getGroundModule().getTouchNormal(grCollStatus::TOUCH_MASK_DOWN, 0));
        float slopeSpeed = __fabsf(normal.m_x * horizontal);
        float slopeFactor = acc->getConstantFloatKirby(0xfc1);
        float slopeCorrection = slopeSpeed * slopeFactor;
        if (normal.m_x > 0.0f) {
            horizontal += slopeCorrection;
        } else {
            horizontal -= slopeCorrection;
        }
        float magnitudeBeforeFirstLimit = __fabsf(horizontal);
        if (magnitudeBeforeFirstLimit > acc->getConstantFloatKirby(0xfa4)) {
            if (horizontal < 0.0f) {
                horizontal = -acc->getConstantFloatKirby(0xfa4);
            } else {
                horizontal = acc->getConstantFloatKirby(0xfa4);
            }
        }
        float magnitudeBeforeSecondLimit = __fabsf(horizontal);
        if (magnitudeBeforeSecondLimit > acc->getConstantFloatKirby(0xfa5)) {
            if (horizontal < 0.0f) {
                horizontal = -acc->getConstantFloatKirby(0xfa5);
            } else {
                horizontal = acc->getConstantFloatKirby(0xfa5);
            }
        }
        acc->getWorkManageModule().setFloat(__fabsf(horizontal), 0x21000007);
        stop.m_speed = Vec2f(horizontal, 0.0f);
    } else {
        if (situation != previous) {
            setKineticRollAir(acc);
        }
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        soWorkManageModule& work = acc->getWorkManageModule();
        float minimum = acc->getConstantFloatKirby(0xfbd);
        float charge = work.getFloat(0x21000005) - minimum;
        charge = acc->getConstantFloatKirby(0xfbf) * charge;
        charge = work.getFloat(0x2100000a) * charge;
        float horizontal = charge;
        float brake;
        if (horizontal > 0.0f) {
            brake = acc->getConstantFloatKirby(0xfa7);
        } else {
            brake = -acc->getConstantFloatKirby(0xfa7);
        }
        horizontal -= brake;
        float horizontalMagnitude = __fabsf(horizontal);
        if (horizontalMagnitude < acc->getConstantFloatKirby(0xfa8)) {
            float direction = horizontal < 0.0f ? -1.0f : 1.0f;
            horizontal = direction * acc->getConstantFloatKirby(0xfa8);
        }
        stop.m_speed = Vec2f(horizontal, 0.0f);
        acc->getWorkManageModule().setFloat(__fabsf(horizontal), 0x21000007);
    }
    soWorkManageModule& work = acc->getWorkManageModule();
    work.subFloat(acc->getConstantFloatKirby(0xfbc), 0x21000005);
    soWorkManageModule& chargeWork = acc->getWorkManageModule();
    float minimum = acc->getConstantFloatKirby(0xfbd);
    if (chargeWork.getFloat(0x21000005) < minimum) {
        acc->getWorkManageModule().onFlag(0x22000015);
    }
    float angle = acc->getWorkManageModule().getFloat(0x21000006);
    int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f rotation = acc->getModelModule().getNodeRotate(node);
    rotation.m_x = angle * 57.29578f;
    acc->getModelModule().setNodeRotate(node, &rotation);
    node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f facing = acc->getModelModule().getNodeRotate(node);
    if (acc->getPostureModule().getLr() == -1.0f) {
        facing.m_y = 180.0f;
    } else {
        facing.m_y = 0.0f;
    }
    acc->getModelModule().setNodeRotate(node, &facing);
    acc->getWorkManageModule().setInt(situation, 0x20000008);
}

void ftPurinStatusUniqProcessSpecialNRoll::execFixPosCounter(soModuleAccesser* acc) {
    if (acc->getWorkManageModule().isFlag(0x22000015)) {
        acc->getCollisionAttackModule().clearAll();
        if (acc->getStageObject().soGetSubKind() == 5) {
            acc->getStatusModule().changeStatusRequest(0x18a, acc);
        } else {
            acc->getStatusModule().changeStatusRequest(0x11c, acc);
        }
    } else {
        ftPurinStatusUniqProcessSpecialNUtility::ftSetPowerPurinSpecialN(acc);
        bool hitWall = false;
        float direction = acc->getWorkManageModule().getFloat(0x2100000a);
        if (direction == 1.0f) {
            if (acc->getGroundModule().isTouch(grCollStatus::TOUCH_MASK_RIGHT, 0)) {
                Vec2f normal;
                Vec2f::copy(normal, acc->getGroundModule().getTouchNormal(grCollStatus::TOUCH_MASK_RIGHT, 0));
                ftPurinStatusUniqProcessSpecialNUtility::ftProcHitWallSpecialNPurin(1.0f, acc, &normal);
                hitWall = true;
            }
        } else {
            if (acc->getGroundModule().isTouch(grCollStatus::TOUCH_MASK_LEFT, 0)) {
                Vec2f normal;
                Vec2f::copy(normal, acc->getGroundModule().getTouchNormal(grCollStatus::TOUCH_MASK_LEFT, 0));
                ftPurinStatusUniqProcessSpecialNUtility::ftProcHitWallSpecialNPurin(-1.0f, acc, &normal);
                hitWall = true;
            }
        }
        Vec2f speed;
        Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        if (hitWall) {
            float bounceFactor = acc->getConstantFloatKirby(0xfc4);
            // Charge loss is truncated to an integer before being stored as float.
            int charge = static_cast<int>(bounceFactor * acc->getWorkManageModule().getFloat(0x21000005));
            acc->getWorkManageModule().setFloat(charge, 0x21000005);
            if (acc->getWorkManageModule().getFloat(0x21000005) < 0.0f) {
                acc->getWorkManageModule().setFloat(0.0f, 0x21000005);
            }
            float horizontal = acc->getWorkManageModule().getFloat(0x21000007);
            acc->getWorkManageModule().setFloat(bounceFactor * horizontal, 0x21000007);
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
            Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
            stop.m_speed = Vec2f(-speed.m_x * bounceFactor, 0.0f);
            float direction = stop.m_speed.m_x > 0.0f ? 1.0f : -1.0f;
            acc->getWorkManageModule().setFloat(direction, 0x2100000a);
            acc->getPostureModule().setLr(direction);
            acc->getPostureModule().updateRotYLr();
        }
        if (!acc->getGroundModule().isTouch(grCollStatus::TOUCH_MASK_DOWN, 0)) {
            soWorkManageModule& work = acc->getWorkManageModule();
            work.setFloat(acc->getConstantFloatKirby(0xfa6), 0x21000008);
        }
        double horizontalMagnitude = __fabsf(speed.m_x);
    float effectThreshold = acc->getConstantFloatKirby(0xfc2);
    if (horizontalMagnitude >= effectThreshold) {
            if (acc->getSituationModule().getKind() == Situation_Ground) {
                acc->getWorkManageModule().addInt(1, 0x2000000a);
                if (acc->getWorkManageModule().getInt(0x2000000a) >= 4) {
                    Vec2f normal;
                    Vec2f::copy(normal, acc->getGroundModule().getTouchNormal(grCollStatus::TOUCH_MASK_DOWN, 0));
                    float slopeAngle = atan2(-normal.m_x, normal.m_y);
                    float direction = acc->getPostureModule().getLr();
                    Vec3f offset(0.0f, 0.0f, 0.0f);
                    offset.m_x = randf() * 5.0f;
                    offset.m_z = randf() * 5.0f;
                    Vec3f position = acc->getPostureModule().getPos() + offset;
                    Vec3f rotation(0.0f, direction == 1.0f ? 1.5707964f : -1.5707964f, slopeAngle);
                    acc->getEffectModule().req(static_cast<EfID>(10), &position, &rotation, 1.0f, 0, -1);
                    acc->getWorkManageModule().setInt(0, 0x2000000a);
                }
            }
            int node = acc->getStageObject().soGetSubKind() == 5 ? 0x199 : 6;
            acc->getWorkManageModule().addInt(1, 0x2000000b);
            if (acc->getWorkManageModule().getInt(0x2000000b) >= 8) {
                EfID effect = static_cast<EfID>(acc->getStageObject().soGetSubKind() == 5 ? 0x1260001 : 0x260002);
                Vec3f rotation(0.0f, 0.0f, 0.0f);
                Vec3f position(0.0f, 0.0f, 0.0f);
                acc->getEffectModule().reqFollow(effect, node, &position, &rotation, 1.0f, false, 0, 0, -1);
                acc->getWorkManageModule().setInt(0, 0x2000000b);
            }
            acc->getWorkManageModule().addInt(1, 0x2000000c);
            if (acc->getWorkManageModule().getInt(0x2000000c) >= 4) {
                float direction = acc->getPostureModule().getLr();
                Vec3f rotation(0.0f, direction == 1.0f ? 1.5707964f : -1.5707964f, 0.0f);
                soEffectModule& effect = acc->getEffectModule();
                Vec3f position = acc->getModelModule().getNodeGlobalPosition(node, false);
                effect.req(static_cast<EfID>(0x1d), &position, &rotation, 1.0f, 0, -1);
                acc->getWorkManageModule().setInt(0, 0x2000000c);
            }
            if (!acc->getWorkManageModule().isFlag(0x22000013)) {
                acc->getEffectModule().reqCommon(0x23, 0.0f);
                acc->getWorkManageModule().onFlag(0x22000013);
            }
        } else {
            acc->getWorkManageModule().setInt(0, 0x2000000a);
            acc->getWorkManageModule().setInt(0, 0x2000000b);
            acc->getWorkManageModule().setInt(0, 0x2000000c);
            if (acc->getWorkManageModule().isFlag(0x22000013)) {
                acc->getEffectModule().removeCommon(0x23);
                acc->getWorkManageModule().offFlag(0x22000013);
            }
        }
    }
}

void ftPurinStatusUniqProcessSpecialNRoll::execStop(soModuleAccesser* acc) {
    float angle = acc->getWorkManageModule().getFloat(0x21000006);
    int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f rotation = acc->getModelModule().getNodeRotate(node);
    rotation.m_x = 57.29578f * angle;
    acc->getModelModule().setNodeRotate(node, &rotation);

    node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f facing = acc->getModelModule().getNodeRotate(node);
    if (-1.0f == acc->getPostureModule().getLr()) {
        facing.m_y = 180.0f;
    } else {
        facing.m_y = 0.0f;
    }
    acc->getModelModule().setNodeRotate(node, &facing);
}
void ftPurinStatusUniqProcessSpecialNRoll::exitStatus(soModuleAccesser* acc, int nextStatus) {
    switch (nextStatus) {
    case 0x11a:
        return;
    default:
        if (acc->getWorkManageModule().isFlag(0x22000013)) {
            acc->getEffectModule().removeCommon(0x23);
            acc->getWorkManageModule().offFlag(0x22000013);
        }
        break;
    }
}

void ftPurinStatusUniqProcessSpecialNRoll::setKineticRollAir(soModuleAccesser* acc) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    Vec2f speed;
    Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
    stop.m_accel = Vec2f(0.0f, 0.0f);
    stop.m_speedTarget = Vec2f(0.0f, 0.0f);
    stop.m_brake = Vec2f(0.0f, 0.0f);
    stop.m_speedLimit = Vec2f(-1.0f, -1.0f);
    stop.enable();
    gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), acc);
    gravity.m_gravity = -acc->getConstantFloatKirby(0xfa0);
    gravity.unk1C = acc->getConstantFloatKirby(0xfa1);
    gravity.m_fallSpeedMax = acc->getConstantFloatKirby(0xfa1);
    gravity.enable();
}

void ftPurinStatusUniqProcessSpecialNRollAir::initStatus(soModuleAccesser* acc) {
    g_ftPurinStatusUniqProcessSpecialNRoll.setKineticRollAir(acc);
    int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f facing = acc->getModelModule().getNodeRotate(node);
    if (-1.0f == acc->getPostureModule().getLr()) {
        facing.m_y = 180.0f;
    } else {
        facing.m_y = 0.0f;
    }
    acc->getModelModule().setNodeRotate(node, &facing);
    ftKineticEnergyDisableAndClear(2, acc);
    ftKineticEnergyDisableAndClear(0, acc);
}

void ftPurinStatusUniqProcessSpecialNRollAir::execStatus(soModuleAccesser* acc) {
    SituationKind situation = acc->getSituationModule().getKind();
    int previous = acc->getWorkManageModule().getInt(0x20000008);
    bool keepRotating = true;
    acc->getWorkManageModule().setFloat(0.0f, 0x21000009);
    Vec3f scale(1.0f, 1.0f, 1.0f);
    int frame = acc->getWorkManageModule().getInt(0x20000002);
    if (frame >= 0 && frame < 4) {
        scale.m_x = 1.0f;
        scale.m_y *= lbl_27_data_A2010[frame];
        scale.m_z *= lbl_27_data_A2020[frame];
        acc->getWorkManageModule().addInt(1, 0x20000002);
    } else {
        scale.m_x = 1.0f;
        scale.m_y = 1.0f;
        scale.m_z = 1.0f;
    }
    int scaleKind = acc->getStageObject().soGetSubKind();
    acc->getModelModule().setNodeScale(scaleKind == 5 ? 0x196 : 3, &scale);
    float oldAngle = acc->getWorkManageModule().getFloat(0x21000006);
    float direction = acc->getWorkManageModule().getFloat(0x2100000a);
    float increment = (acc->getConstantFloatKirby(0xfb6) * 0.2f) * direction;
    if (acc->getSituationModule().getKind() == Situation_Ground) {
        increment *= acc->getWorkManageModule().getFloat(0x21000005) * 0.017453292f;
    } else {
        soWorkManageModule& work = acc->getWorkManageModule();
        float airFactor = acc->getConstantFloatKirby(0xfbe);
        increment *= (work.getFloat(0x21000005) * 0.017453292f) * airFactor;
    }
    float newAngle = oldAngle + increment;
    while (newAngle < 0.0f) {
        newAngle += 6.2831855f;
    }
    while (newAngle > 6.2831855f) {
        newAngle -= 6.2831855f;
    }
    acc->getWorkManageModule().setFloat(newAngle, 0x21000006);
    acc->getWorkManageModule().subInt(1, 0x20000000);
    // Finish only after the timed roll crosses the recovery phase of its rotation.
    if (acc->getWorkManageModule().getInt(0x20000000) <= 0 &&
        newAngle > 1.5707964f && newAngle < 4.712389f) {
        if (increment > 0.0f) {
            if (newAngle > 3.1415927f && oldAngle < 3.1415927f) {
                acc->getWorkManageModule().setInt(0, 0x20000000);
                acc->getCollisionAttackModule().clearAll();
                if (acc->getStageObject().soGetSubKind() == 5) {
                    acc->getStatusModule().changeStatusRequest(0x18a, acc);
                } else {
                    acc->getStatusModule().changeStatusRequest(0x11c, acc);
                }
                keepRotating = false;
            }
        } else {
            if (newAngle < 3.1415927f && oldAngle > 3.1415927f) {
                acc->getWorkManageModule().setInt(0, 0x20000000);
                acc->getCollisionAttackModule().clearAll();
                if (acc->getStageObject().soGetSubKind() == 5) {
                    acc->getStatusModule().changeStatusRequest(0x18a, acc);
                } else {
                    acc->getStatusModule().changeStatusRequest(0x11c, acc);
                }
                keepRotating = false;
            }
        }
    }
    float stick = acc->getControllerModule().getStickX();
    float stickMagnitude = __fabsf(stick);
    if (stickMagnitude > acc->getConstantFloatKirby(0xfab)) {
        float desiredDirection = stick > 0.0f ? 1.0f : -1.0f;
        if (desiredDirection != acc->getWorkManageModule().getFloat(0x2100000a)) {
            acc->getCollisionAttackModule().clearAll();
            acc->getWorkManageModule().setFloat(desiredDirection, 0x21000009);
            acc->getWorkManageModule().setFloat(-desiredDirection, 0x2100000a);
            if (acc->getStageObject().soGetSubKind() == 5) {
                acc->getStatusModule().changeStatusRequest(0x189, acc);
            } else {
                acc->getStatusModule().changeStatusRequest(0x11b, acc);
            }
        }
    }
    if (situation == Situation_Ground) {
        if (situation != previous) {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
            dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
            Vec2f speed;
            Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
            stop.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
            stop.m_accel = Vec2f(0.0f, 0.0f);
            stop.m_speedTarget = Vec2f(0.0f, 0.0f);
            stop.m_brake = Vec2f(0.0f, 0.0f);
            stop.m_speedLimit = Vec2f(-1.0f, -1.0f);
            stop.offConsiderGroundFriction();
            stop.enable();
            ftKineticEnergyDisableAndClear(1, acc);
        }
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        soWorkManageModule& work = acc->getWorkManageModule();
        float minimum = acc->getConstantFloatKirby(0xfbd);
        float charge = work.getFloat(0x21000005) - minimum;
        charge = acc->getConstantFloatKirby(0xfbf) * charge;
        charge = work.getFloat(0x2100000a) * charge;
        float horizontal = charge;
        Vec2f normal;
        Vec2f::copy(normal, acc->getGroundModule().getTouchNormal(grCollStatus::TOUCH_MASK_DOWN, 0));
        float slopeSpeed = __fabsf(normal.m_x * horizontal);
        float slopeFactor = acc->getConstantFloatKirby(0xfc1);
        float slopeCorrection = slopeSpeed * slopeFactor;
        if (normal.m_x > 0.0f) {
            horizontal += slopeCorrection;
        } else {
            horizontal -= slopeCorrection;
        }
        float magnitudeBeforeFirstLimit = __fabsf(horizontal);
        if (magnitudeBeforeFirstLimit > acc->getConstantFloatKirby(0xfa4)) {
            if (horizontal < 0.0f) {
                horizontal = -acc->getConstantFloatKirby(0xfa4);
            } else {
                horizontal = acc->getConstantFloatKirby(0xfa4);
            }
        }
        float magnitudeBeforeSecondLimit = __fabsf(horizontal);
        if (magnitudeBeforeSecondLimit > acc->getConstantFloatKirby(0xfa5)) {
            if (horizontal < 0.0f) {
                horizontal = -acc->getConstantFloatKirby(0xfa5);
            } else {
                horizontal = acc->getConstantFloatKirby(0xfa5);
            }
        }
        acc->getWorkManageModule().setFloat(__fabsf(horizontal), 0x21000007);
        stop.m_speed = Vec2f(horizontal, 0.0f);
    } else {
        if (situation != previous) {
            g_ftPurinStatusUniqProcessSpecialNRoll.setKineticRollAir(acc);
        }
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        soWorkManageModule& work = acc->getWorkManageModule();
        float minimum = acc->getConstantFloatKirby(0xfbd);
        float charge = work.getFloat(0x21000005) - minimum;
        charge = acc->getConstantFloatKirby(0xfbf) * charge;
        charge = work.getFloat(0x2100000a) * charge;
        float horizontal = charge;
        float brake;
        if (horizontal > 0.0f) {
            brake = acc->getConstantFloatKirby(0xfa7);
        } else {
            brake = -acc->getConstantFloatKirby(0xfa7);
        }
        horizontal -= brake;
        float horizontalMagnitude = __fabsf(horizontal);
        if (horizontalMagnitude < acc->getConstantFloatKirby(0xfa8)) {
            float direction = horizontal < 0.0f ? -1.0f : 1.0f;
            horizontal = direction * acc->getConstantFloatKirby(0xfa8);
        }
        stop.m_speed = Vec2f(horizontal, 0.0f);
        acc->getWorkManageModule().setFloat(__fabsf(horizontal), 0x21000007);
    }
    soWorkManageModule& work = acc->getWorkManageModule();
    work.subFloat(acc->getConstantFloatKirby(0xfbc), 0x21000005);
    soWorkManageModule& chargeWork = acc->getWorkManageModule();
    float minimum = acc->getConstantFloatKirby(0xfbd);
    if (chargeWork.getFloat(0x21000005) < minimum) {
        acc->getCollisionAttackModule().clearAll();
        if (acc->getStageObject().soGetSubKind() == 5) {
            acc->getStatusModule().changeStatusRequest(0x18a, acc);
        } else {
            acc->getStatusModule().changeStatusRequest(0x11c, acc);
        }
        keepRotating = false;
    }
    if (keepRotating) {
        float angle = acc->getWorkManageModule().getFloat(0x21000006);
        int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
        Vec3f rotation = acc->getModelModule().getNodeRotate(node);
        rotation.m_x = angle * 57.29578f;
        acc->getModelModule().setNodeRotate(node, &rotation);
        node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
        Vec3f facing = acc->getModelModule().getNodeRotate(node);
        if (acc->getPostureModule().getLr() == -1.0f) {
            facing.m_y = 180.0f;
        } else {
            facing.m_y = 0.0f;
        }
        acc->getModelModule().setNodeRotate(node, &facing);
    }
    acc->getWorkManageModule().setInt(situation, 0x20000008);
}

void ftPurinStatusUniqProcessSpecialNRollAir::execFixPosCounter(soModuleAccesser* acc) {
    Vec2f speed;
    bool hitWall = false;
    Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float direction = acc->getWorkManageModule().getFloat(0x2100000a);
    if (direction == 1.0f) {
        if (acc->getGroundModule().isTouch(grCollStatus::TOUCH_MASK_RIGHT, 0)) {
            Vec2f normal;
            Vec2f::copy(normal, acc->getGroundModule().getTouchNormal(grCollStatus::TOUCH_MASK_RIGHT, 0));
            ftPurinStatusUniqProcessSpecialNUtility::ftProcHitWallSpecialNPurin(1.0f, acc, &normal);
            hitWall = true;
        }
    } else {
        if (acc->getGroundModule().isTouch(grCollStatus::TOUCH_MASK_LEFT, 0)) {
            Vec2f normal;
            Vec2f::copy(normal, acc->getGroundModule().getTouchNormal(grCollStatus::TOUCH_MASK_LEFT, 0));
            ftPurinStatusUniqProcessSpecialNUtility::ftProcHitWallSpecialNPurin(-1.0f, acc, &normal);
            hitWall = true;
        }
    }
    if (hitWall) {
        float bounceFactor = acc->getConstantFloatKirby(0xfc4);
        // Charge loss is truncated to an integer before being stored as float.
        int charge = static_cast<int>(bounceFactor * acc->getWorkManageModule().getFloat(0x21000005));
        acc->getWorkManageModule().setFloat(charge, 0x21000005);
        if (acc->getWorkManageModule().getFloat(0x21000005) < 0.0f) {
            acc->getWorkManageModule().setFloat(0.0f, 0x21000005);
        }
        float horizontal = acc->getWorkManageModule().getFloat(0x21000007);
        acc->getWorkManageModule().setFloat(bounceFactor * horizontal, 0x21000007);
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        stop.m_speed = Vec2f(-speed.m_x * bounceFactor, 0.0f);
        float direction = -acc->getWorkManageModule().getFloat(0x2100000a);
        acc->getWorkManageModule().setFloat(direction, 0x2100000a);
        acc->getPostureModule().setLr(direction);
        acc->getPostureModule().updateRotYLr();
    }
    if (acc->getSituationModule().getKind() == Situation_Ground) {
        float verticalMagnitude = __fabsf(speed.m_y);
        float bounceY = verticalMagnitude * acc->getConstantFloatKirby(0xfae);
        float bounceThreshold = acc->getConstantFloatKirby(0xfaf);
        if (bounceY < bounceThreshold) {
            acc->getWorkManageModule().onFlag(0x22000016);
        } else {
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
            gravity.m_speedY = bounceY;
            float stick = acc->getControllerModule().getStickX();
            double stickMagnitude = __fabsf(stick);
            float deadZone = acc->getConstantFloatKirby(0xfab);
            if (stickMagnitude > deadZone) {
                acc->getWorkManageModule().setFloat(stick > 0.0f ? 1.0f : -1.0f, 0x2100000a);
                soWorkManageModule& work = acc->getWorkManageModule();
                float direction = work.getFloat(0x2100000a);
                stop.m_speed = Vec2f(work.getFloat(0x21000007) * direction, 0.0f);
            }
            float angle = acc->getWorkManageModule().getFloat(0x21000006);
            int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
            Vec3f rotation = acc->getModelModule().getNodeRotate(node);
            rotation.m_x = angle * 57.29578f;
            acc->getModelModule().setNodeRotate(node, &rotation);
        }
        acc->getWorkManageModule().setInt(0, 0x20000002);
    }
    double horizontalMagnitude = __fabsf(speed.m_x);
    float effectThreshold = acc->getConstantFloatKirby(0xfc2);
    if (horizontalMagnitude >= effectThreshold) {
        acc->getWorkManageModule().addInt(1, 0x2000000a);
        if (acc->getWorkManageModule().getInt(0x2000000a) >= 4) {
            float direction = acc->getPostureModule().getLr();
            Vec3f offset(0.0f, 0.0f, 0.0f);
            offset.m_x = randf() * 5.0f;
            offset.m_z = randf() * 5.0f;
            Vec3f position = acc->getPostureModule().getPos() + offset;
            Vec3f rotation(0.0f, direction == 1.0f ? 1.5707964f : -1.5707964f, 0.0f);
            acc->getEffectModule().req(static_cast<EfID>(10), &position, &rotation, 1.0f, 0, -1);
            acc->getWorkManageModule().setInt(0, 0x2000000a);
        }
        int node = acc->getStageObject().soGetSubKind() == 5 ? 0x199 : 6;
        acc->getWorkManageModule().addInt(1, 0x2000000b);
        if (acc->getWorkManageModule().getInt(0x2000000b) >= 8) {
            EfID effect = static_cast<EfID>(acc->getStageObject().soGetSubKind() == 5 ? 0x1260001 : 0x260002);
            Vec3f rotation(0.0f, 0.0f, 0.0f);
            Vec3f position(0.0f, 0.0f, 0.0f);
            acc->getEffectModule().reqFollow(effect, node, &position, &rotation, 1.0f, false, 0, 0, -1);
            acc->getWorkManageModule().setInt(0, 0x2000000b);
        }
        acc->getWorkManageModule().addInt(1, 0x2000000c);
        if (acc->getWorkManageModule().getInt(0x2000000c) >= 4) {
            float direction = acc->getPostureModule().getLr();
            Vec3f rotation(0.0f, direction == 1.0f ? 1.5707964f : -1.5707964f, 0.0f);
            soEffectModule& effect = acc->getEffectModule();
            Vec3f position = acc->getModelModule().getNodeGlobalPosition(node, false);
            effect.req(static_cast<EfID>(0x1d), &position, &rotation, 1.0f, 0, -1);
            acc->getWorkManageModule().setInt(0, 0x2000000c);
        }
        if (!acc->getWorkManageModule().isFlag(0x22000013)) {
            acc->getEffectModule().reqCommon(0x23, 0.0f);
            acc->getWorkManageModule().onFlag(0x22000013);
        }
    } else {
        acc->getWorkManageModule().setInt(0, 0x2000000a);
        acc->getWorkManageModule().setInt(0, 0x2000000b);
        acc->getWorkManageModule().setInt(0, 0x2000000c);
        if (acc->getWorkManageModule().isFlag(0x22000013)) {
            acc->getEffectModule().removeCommon(0x23);
            acc->getWorkManageModule().offFlag(0x22000013);
        }
    }
    ftPurinStatusUniqProcessSpecialNUtility::ftSetPowerPurinSpecialN(acc);
}

void ftPurinStatusUniqProcessSpecialNRollAir::execFixPos(soModuleAccesser* acc) {
    // A weak landing sets this flag rather than immediately bouncing again.
    if (acc->getSituationModule().getKind() == Situation_Ground && acc->getWorkManageModule().isFlag(0x22000016)) {
        if (acc->getStageObject().soGetSubKind() == 5) {
            acc->getStatusModule().changeStatusRequest(0x187, acc);
        } else {
            acc->getStatusModule().changeStatusRequest(0x119, acc);
        }
        acc->getWorkManageModule().offFlag(0x22000016);
        Vec2f normal = acc->getGroundModule().getTouchNormal(grCollStatus::TOUCH_MASK_DOWN, 0);
        float slopeAngle = atan2(-normal.m_x, normal.m_y);
        float direction = acc->getPostureModule().getLr();
        Vec3f rotation(0.0f, direction == 1.0f ? 1.5707964f : -1.5707964f, slopeAngle);
        acc->getEffectModule().req(static_cast<EfID>(10), &acc->getPostureModule().getPos(), &rotation, 1.0f, 0, -1);
    }
}

void ftPurinStatusUniqProcessSpecialNRollAir::execStop(soModuleAccesser*) { }
void ftPurinStatusUniqProcessSpecialNRollAir::exitStatus(soModuleAccesser* acc, int nextStatus) {
    switch (nextStatus) {
    case 0x119:
        return;
    default:
        if (acc->getWorkManageModule().isFlag(0x22000013)) {
            acc->getEffectModule().removeCommon(0x23);
            acc->getWorkManageModule().offFlag(0x22000013);
        }
        break;
    }
}

void ftPurinStatusUniqProcessSpecialNTurn::initStatus(soModuleAccesser* acc) {
    Vec2f speed;
    Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    acc->getWorkManageModule().setFloat(speed.m_x, 0x21000004);
    acc->getWorkManageModule().setFloat(speed.m_x * -0.05f, 0x21000008);
    (void)dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    if (acc->getSituationModule().getKind() == Situation_Ground) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        (void)dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
        Vec2f currentSpeed;
        Vec2f::copy(currentSpeed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        stop.offConsiderGroundFriction();
        stop.resetEnergy(0, &Vec2f(currentSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
        stop.m_accel = Vec2f(0.0f, 0.0f);
        stop.m_speedTarget = Vec2f(0.0f, 0.0f);
        stop.m_brake = Vec2f(0.0f, 0.0f);
        stop.m_speedLimit = Vec2f(-1.0f, -1.0f);
        stop.enable();
        ftKineticEnergyDisableAndClear(1, acc);
    } else {
        setKineticTurnAir(acc);
    }
    acc->getWorkManageModule().setInt(0, 0x20000004);
    ftKineticEnergyDisableAndClear(2, acc);
    ftKineticEnergyDisableAndClear(0, acc);
    if (acc->getWorkManageModule().isFlag(0x22000013)) {
        acc->getEffectModule().removeCommon(0x23);
        acc->getWorkManageModule().offFlag(0x22000013);
    }
    if (acc->getStageObject().soGetSubKind() == 5) {
        acc->getSoundModule().playStatusSE(static_cast<SndID>(0x1b18), true, 0);
    } else {
        acc->getSoundModule().playStatusSE(static_cast<SndID>(0x179f), true, 0);
    }
    acc->getWorkManageModule().setInt(0, 0x20000004);
}

void ftPurinStatusUniqProcessSpecialNTurn::execStatus(soModuleAccesser* acc) {
    SituationKind situation = acc->getSituationModule().getKind();
    int previous = acc->getWorkManageModule().getInt(0x20000008);
    bool keepRotating = true;
    Vec3f scale(1.0f, 1.0f, 1.0f);
    int frame = acc->getWorkManageModule().getInt(0x20000002);
    if (frame >= 0 && frame < 4) {
        scale.m_x = 1.0f;
        scale.m_y *= lbl_27_data_A2010[frame];
        scale.m_z *= lbl_27_data_A2020[frame];
        acc->getWorkManageModule().addInt(1, 0x20000002);
    } else {
        scale.m_x = 1.0f;
        scale.m_y = 1.0f;
        scale.m_z = 1.0f;
    }
    int scaleKind = acc->getStageObject().soGetSubKind();
    acc->getModelModule().setNodeScale(scaleKind == 5 ? 0x196 : 3, &scale);
    float angle = acc->getWorkManageModule().getFloat(0x21000006);
    float direction = acc->getWorkManageModule().getFloat(0x2100000a);
    angle += (acc->getConstantFloatKirby(0xfac) * 0.2f) * direction;
    while (angle < 0.0f) {
        angle += 6.2831855f;
    }
    while (angle > 6.2831855f) {
        angle -= 6.2831855f;
    }
    acc->getWorkManageModule().setFloat(angle, 0x21000006);
    acc->getWorkManageModule().subInt(1, 0x20000000);
    if (acc->getWorkManageModule().getInt(0x20000000) <= 0) {
        acc->getWorkManageModule().subInt(0, 0x20000000);
        float lr = acc->getWorkManageModule().getFloat(0x2100000a);
        acc->getWorkManageModule().setFloat(-lr, 0x2100000a);
        acc->getCollisionAttackModule().clearAll();
        if (acc->getStageObject().soGetSubKind() == 5) {
            acc->getStatusModule().changeStatusRequest(0x18a, acc);
        } else {
            acc->getStatusModule().changeStatusRequest(0x11c, acc);
        }
        keepRotating = false;
    }
    if (situation == Situation_Ground) {
        if (situation != previous) {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
            (void)dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
            Vec2f currentSpeed;
            Vec2f::copy(currentSpeed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
            stop.offConsiderGroundFriction();
            stop.resetEnergy(0, &Vec2f(currentSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
            stop.m_accel = Vec2f(0.0f, 0.0f);
            stop.m_speedTarget = Vec2f(0.0f, 0.0f);
            stop.m_brake = Vec2f(0.0f, 0.0f);
            stop.m_speedLimit = Vec2f(-1.0f, -1.0f);
            stop.enable();
            ftKineticEnergyDisableAndClear(1, acc);
        }
        updateKineticTurnGround(acc);
        Vec2f speed;
        Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        if (speed.m_x <= acc->getConstantFloatKirby(0xfad)) {
            acc->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground_Cliff_Stop, 0);
        } else {
            acc->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
        }
    } else if (situation != previous) {
        setKineticTurnAir(acc);
    }
    if (keepRotating) {
        int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
        Vec3f facing = acc->getModelModule().getNodeRotate(node);
        if (-1.0f == acc->getPostureModule().getLr()) {
            facing.m_y = 180.0f;
        } else {
            facing.m_y = 0.0f;
        }
        acc->getModelModule().setNodeRotate(node, &facing);
        angle = acc->getWorkManageModule().getFloat(0x21000006);
        node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
        Vec3f rotation = acc->getModelModule().getNodeRotate(node);
        rotation.m_x = 57.29578f * angle;
        acc->getModelModule().setNodeRotate(node, &rotation);
    }
    acc->getWorkManageModule().setInt(situation, 0x20000008);
}

void ftPurinStatusUniqProcessSpecialNTurn::execFixPosCounter(soModuleAccesser* acc) {
    if (acc->getSituationModule().getKind() == Situation_Ground) {
        acc->getWorkManageModule().decInt(0x20000004);
        if (acc->getWorkManageModule().getInt(0x20000004) < 1) {
            Vec2f normal = acc->getGroundModule().getTouchNormal(grCollStatus::TOUCH_MASK_DOWN, 0);
            float slopeAngle = atan2(-normal.m_x, normal.m_y);
            float direction = acc->getPostureModule().getLr();
            Vec3f rotation(0.0f, direction == 1.0f ? -1.5707964f : 1.5707964f, slopeAngle);
            acc->getEffectModule().req(static_cast<EfID>(10), &acc->getPostureModule().getPos(), &rotation, 1.0f, 0, -1);
            acc->getWorkManageModule().setInt(acc->getConstantIntKirby(0x5dc2), 0x20000004);
        }
    }
}

void ftPurinStatusUniqProcessSpecialNTurn::execFixPos(soModuleAccesser* acc) {
    if (acc->getSituationModule().getKind() == Situation_Ground) {
        float current = acc->getWorkManageModule().getFloat(0x21000010);
        float initial = acc->getWorkManageModule().getFloat(0x21000004);
        // Resume rolling after horizontal speed reverses far enough.
        if (initial > 0.0f) {
            if (current < 0.0f) {
                if (__fabsf(initial * acc->getConstantFloatKirby(0xfc3)) <= __fabsf(current)) {
                    if (acc->getStageObject().soGetSubKind() == 5) {
                        acc->getWorkManageModule().setInt(0x189, 0x20000009);
                        acc->getStatusModule().changeStatusRequest(0x187, acc);
                    } else {
                        acc->getWorkManageModule().setInt(0x11b, 0x20000009);
                        acc->getStatusModule().changeStatusRequest(0x119, acc);
                    }
                }
            }
        } else if (current > 0.0f) {
            if (__fabsf(initial * acc->getConstantFloatKirby(0xfc3)) <= __fabsf(current)) {
                if (acc->getStageObject().soGetSubKind() == 5) {
                    acc->getWorkManageModule().setInt(0x189, 0x20000009);
                    acc->getStatusModule().changeStatusRequest(0x187, acc);
                } else {
                    acc->getWorkManageModule().setInt(0x11b, 0x20000009);
                    acc->getStatusModule().changeStatusRequest(0x119, acc);
                }
            }
        }
    }
}

void ftPurinStatusUniqProcessSpecialNTurn::execStop(soModuleAccesser* acc) {
    float angle = acc->getWorkManageModule().getFloat(0x21000006);
    int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f rotation = acc->getModelModule().getNodeRotate(node);
    rotation.m_x = 57.29578f * angle;
    acc->getModelModule().setNodeRotate(node, &rotation);

    node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f facing = acc->getModelModule().getNodeRotate(node);
    if (-1.0f == acc->getPostureModule().getLr()) {
        facing.m_y = 180.0f;
    } else {
        facing.m_y = 0.0f;
    }
    acc->getModelModule().setNodeRotate(node, &facing);
}
void ftPurinStatusUniqProcessSpecialNTurn::exitStatus(soModuleAccesser*, int) { }
void ftPurinStatusUniqProcessSpecialNTurn::updateKineticTurnGround(soModuleAccesser* acc) {
    // MATCH-ONLY: declaration order retains the native floating-point registers.
    float slope;
    float friction;
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    Vec2f speed;
    Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float nextSpeed = speed.m_x;
    soGroundModule& ground = acc->getGroundModule();
    float frictionFactor = acc->getConstantFloatKirby(0xfc0);
    float downFriction = ground.getDownFriction(0);
    friction = downFriction * frictionFactor;
    float acceleration = acc->getWorkManageModule().getFloat(0x21000008);
    Vec2f normal;
    Vec2f::copy(normal, acc->getGroundModule().getTouchNormal(grCollStatus::TOUCH_MASK_DOWN, 0));
    slope = __fabsf(normal.m_x);
    float slopeFactor = acc->getConstantFloatKirby(0xfc1);
    float correction = acceleration * slope;
    float slopeCorrection = correction * slopeFactor;
    // Surface slope adjusts the braking acceleration used while reversing.
    if (normal.m_x > 0.0f) {
        nextSpeed += friction * (acceleration + slopeCorrection);
    } else {
        nextSpeed += friction * (acceleration - slopeCorrection);
    }
    stop.m_speed = Vec2f(nextSpeed, 0.0f);
    acc->getWorkManageModule().setFloat(nextSpeed, 0x21000010);
}

void ftPurinStatusUniqProcessSpecialNTurn::setKineticTurnAir(soModuleAccesser* acc) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    Vec2f speed;
    Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
    stop.m_accel = Vec2f(0.0f, 0.0f);
    stop.m_brake = Vec2f(acc->getConstantFloatKirby(0xfa7), 0.0f);
    stop.m_speedTarget = Vec2f(acc->getConstantFloatKirby(0xfa8), 0.0f);
    stop.m_speedLimit = Vec2f(-1.0f, -1.0f);
    stop.enable();
    gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), acc);
    gravity.m_gravity = -acc->getConstantFloatKirby(0xfa0);
    gravity.unk1C = acc->getConstantFloatKirby(0xfa1);
    gravity.m_fallSpeedMax = acc->getConstantFloatKirby(0xfa1);
    gravity.enable();
}

void ftPurinStatusUniqProcessSpecialNEnd::initStatus(soModuleAccesser* acc) {
    Vec2f speed;
    Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    if (acc->getSituationModule().getKind() == Situation_Ground) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        float horizontal = speed.m_x * acc->getConstantFloatKirby(0xfb4);
        Vec3f groundResetRotation(0.0f, 0.0f, 0.0f);
        stop.resetEnergy(0, &Vec2f(horizontal, 0.0f), &groundResetRotation, acc);
        stop.onConsiderGroundFriction();
        stop.enable();
        ftKineticEnergyDisableAndClear(1, acc);
    } else {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
        float horizontal = speed.m_x * acc->getConstantFloatKirby(0xfb4);
        float vertical = speed.m_y * acc->getConstantFloatKirby(0xfb5);
        Vec3f airResetRotation(0.0f, 0.0f, 0.0f);
        stop.resetEnergy(6, &Vec2f(horizontal, 0.0f), &airResetRotation, acc);
        stop.enable();
        Vec3f gravityResetRotation(0.0f, 0.0f, 0.0f);
        gravity.resetEnergy(0, &Vec2f(0.0f, vertical), &gravityResetRotation, acc);
        gravity.m_gravity = -acc->getConstantFloatKirby(0xfa0);
        gravity.unk1C = acc->getConstantFloatKirby(0xfa1);
        gravity.m_fallSpeedMax = acc->getConstantFloatKirby(0xfa1);
        gravity.enable();
    }
    Vec3f scale(1.0f, 1.0f, 1.0f);
    int kind = acc->getStageObject().soGetSubKind();
    acc->getModelModule().setNodeScale(kind == 5 ? 0x196 : 3, &scale);
    kind = acc->getStageObject().soGetSubKind();
    acc->getModelModule().clearNodeSRT(kind == 5 ? 0x197 : 4);
    kind = acc->getStageObject().soGetSubKind();
    acc->getModelModule().clearNodeSRT(kind == 5 ? 0x196 : 3);
    acc->getWorkManageModule().setFloat(0.0f, 0x21000009);
    ftKineticEnergyDisableAndClear(2, acc);
    ftKineticEnergyDisableAndClear(0, acc);
    if (acc->getWorkManageModule().isFlag(0x22000013)) {
        acc->getEffectModule().removeCommon(0x23);
        acc->getWorkManageModule().offFlag(0x22000013);
    }
}

void ftPurinStatusUniqProcessSpecialNEnd::execStatus(soModuleAccesser* acc) {
    SituationKind situation = acc->getSituationModule().getKind();
    int previous = acc->getWorkManageModule().getInt(0x20000008);
    acc->getWorkManageModule().setFloat(0.0f, 0x21000009);
    Vec3f scale(1.0f, 1.0f, 1.0f);
    int frame = acc->getWorkManageModule().getInt(0x20000002);
    if (frame >= 0 && frame < 4) {
        scale.m_x = 1.0f;
        scale.m_y *= lbl_27_data_A2010[frame];
        scale.m_z *= lbl_27_data_A2020[frame];
        acc->getWorkManageModule().addInt(1, 0x20000002);
    } else {
        scale.m_x = 1.0f;
        scale.m_y = 1.0f;
        scale.m_z = 1.0f;
    }
    int scaleKind = acc->getStageObject().soGetSubKind();
    acc->getModelModule().setNodeScale(scaleKind == 5 ? 0x196 : 3, &scale);
    if (situation != previous) {
        if (situation == Situation_Ground) {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
            Vec2f speed;
            Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
            stop.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
            stop.enable();
            ftKineticEnergyDisableAndClear(1, acc);
        } else {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
            Vec2f speed;
            Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
            stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
            stop.enable();
            gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), acc);
            gravity.m_gravity = -acc->getConstantFloatKirby(0xfa0);
            gravity.unk1C = acc->getConstantFloatKirby(0xfa1);
            gravity.m_fallSpeedMax = acc->getConstantFloatKirby(0xfa1);
            gravity.enable();
        }
    }
    acc->getWorkManageModule().setInt(situation, 0x20000008);
}

void ftPurinStatusUniqProcessSpecialNEnd::exitStatus(soModuleAccesser*, int) { }

void ftPurinStatusUniqProcessSpecialNHitEnd::initStatus(soModuleAccesser* acc) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    Vec2f speed;
    Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float recoilX;
    if (acc->getSituationModule().getKind() == Situation_Ground) {
        recoilX = speed.m_x * acc->getConstantFloatKirby(0xfb2);
    } else {
        recoilX = acc->getConstantFloatKirby(0xfb2);
    }
    stop.resetEnergy(6, &Vec2f(recoilX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
    stop.enable();
    float recoilY = acc->getConstantFloatKirby(0xfb3);
    gravity.resetEnergy(0, &Vec2f(0.0f, recoilY), &Vec3f(0.0f, 0.0f, 0.0f), acc);
    gravity.m_gravity = -acc->getConstantFloatKirby(0xfa0);
    gravity.unk1C = acc->getConstantFloatKirby(0xfa1);
    gravity.m_fallSpeedMax = acc->getConstantFloatKirby(0xfa1);
    gravity.enable();
    ftKineticEnergyDisableAndClear(2, acc);
    ftKineticEnergyDisableAndClear(0, acc);
    if (acc->getWorkManageModule().isFlag(0x22000013)) {
        acc->getEffectModule().removeCommon(0x23);
        acc->getWorkManageModule().offFlag(0x22000013);
    }
}

void ftPurinStatusUniqProcessSpecialNHitEnd::execStatus(soModuleAccesser* acc) {
    SituationKind situation = acc->getSituationModule().getKind();
    acc->getWorkManageModule().getInt(0x20000008);
    // MATCH-ONLY: this declaration retains the native vector temporary order.
    Vec3f resetRotation;
    Vec3f scale(1.0f, 1.0f, 1.0f);
    int frame = acc->getWorkManageModule().getInt(0x20000002);
    if (frame >= 0 && frame < 4) {
        scale.m_x = 1.0f;
        scale.m_y *= lbl_27_data_A2010[frame];
        scale.m_z *= lbl_27_data_A2020[frame];
        acc->getWorkManageModule().addInt(1, 0x20000002);
    } else {
        scale.m_x = 1.0f;
        scale.m_y = 1.0f;
        scale.m_z = 1.0f;
    }
    int scaleKind = acc->getStageObject().soGetSubKind();
    acc->getModelModule().setNodeScale(scaleKind == 5 ? 0x196 : 3, &scale);
    float direction = acc->getWorkManageModule().getFloat(0x2100000a);
    float incrementBase = acc->getConstantFloatKirby(0xfac) * 0.2f;
    float increment = incrementBase * direction;
    float angle = acc->getWorkManageModule().getFloat(0x21000006);
    angle += increment * acc->getConstantFloatKirby(0xfbe);
    while (angle < 0.0f) {
        angle += 6.2831855f;
    }
    while (angle > 6.2831855f) {
        angle -= 6.2831855f;
    }
    acc->getWorkManageModule().setFloat(angle, 0x21000006);
    Vec2f speed;
    Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    if (speed.m_y <= -acc->getConstantFloatKirby(0xfa1)) {
        if (!acc->getKineticModule().getEnergy(2)->isEnable()) {
            ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*acc->getKineticModule().getEnergy(2));
            resetRotation.m_x = 0.0f;
            resetRotation.m_y = 0.0f;
            resetRotation.m_z = 0.0f;
            controller.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &resetRotation, acc);
            controller.enable();
            ftKineticEnergyDisableAndClear(3, acc);
        }
    }
    angle = acc->getWorkManageModule().getFloat(0x21000006);
    int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f rotation = acc->getModelModule().getNodeRotate(node);
    rotation.m_x = 57.29578f * angle;
    acc->getModelModule().setNodeRotate(node, &rotation);

    node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f facing = acc->getModelModule().getNodeRotate(node);
    if (-1.0f == acc->getPostureModule().getLr()) {
        facing.m_y = 180.0f;
    } else {
        facing.m_y = 0.0f;
    }
    acc->getModelModule().setNodeRotate(node, &facing);
    acc->getWorkManageModule().setInt(situation, 0x20000008);
}

void ftPurinStatusUniqProcessSpecialNHitEnd::execStop(soModuleAccesser* acc) {
    float angle = acc->getWorkManageModule().getFloat(0x21000006);
    int node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f rotation = acc->getModelModule().getNodeRotate(node);
    rotation.m_x = 57.29578f * angle;
    acc->getModelModule().setNodeRotate(node, &rotation);

    node = acc->getStageObject().soGetSubKind() == 5 ? 0x197 : 4;
    Vec3f facing = acc->getModelModule().getNodeRotate(node);
    if (-1.0f == acc->getPostureModule().getLr()) {
        facing.m_y = 180.0f;
    } else {
        facing.m_y = 0.0f;
    }
    acc->getModelModule().setNodeRotate(node, &facing);
}
void ftPurinStatusUniqProcessSpecialNHitEnd::exitStatus(soModuleAccesser* acc, int nextStatus) {
    // HYPOTHESIS: the landing state receives Rollout's landing-lag parameter.
    switch (nextStatus) {
    case 0x19: {
        float landingLag = acc->getConstantFloatKirby(0xfc5);
        acc->getWorkManageModule().setFloat(landingLag, 0x11000000);
        break;
    }
    }
}

bool ftPurinStatusUniqProcessSpecialNUtility::ftProcHitSpecialNPurin(soModuleAccesser* acc) {
    int status = acc->getStatusModule().getStatusKind();
    int original = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, status, acc, true);
    acc->getWorkManageModule().setInt(0, 0x20000003);
    switch (original) {
    case 0x119:
    case 0x11a:
    case 0x11b: {
        acc->getWorkManageModule().subInt(acc->getConstantIntKirby(0x5dc1), 0x20000000);
        soWorkManageModule& work = acc->getWorkManageModule();
        soPostureModule& posture = acc->getPostureModule();
        posture.setLr(work.getFloat(0x2100000a));
        acc->getPostureModule().updateRotYLr();
        acc->getStageObject().updateNodeSRT();
        acc->getCollisionAttackModule().clearAll();
        if (acc->getStageObject().soGetSubKind() == 5) {
            acc->getStatusModule().changeStatusRequest(0x18b, acc);
        } else {
            acc->getStatusModule().changeStatusRequest(0x11d, acc);
        }
        acc->getWorkManageModule().setFloat(0.0f, 0x21000009);
        return true;
    }
    default:
        return false;
    }
}

void ftPurinStatusUniqProcessSpecialNUtility::ftProcHitWallSpecialNPurin(float direction, soModuleAccesser* acc, Vec2f* normal) {
    float wallAngle = atan2(-normal->m_x, normal->m_y);
    Vec3f position = acc->getPostureModule().getPos();
    float up, down, width;
    acc->getGroundModule().getRhombus(&up, &down, &width, 0);
    if (direction == 1.0f) {
        position.m_x += width;
    } else {
        position.m_x -= width;
    }
    position.m_y += __fabsf(up + down) * 0.5f;
    Vec3f rotation(0.0f, 0.0f, wallAngle);
    Vec3f effectPosition(position.m_x, position.m_y, 0.0f);
    acc->getEffectModule().req(static_cast<EfID>(9), &effectPosition, &rotation, 1.0f, 0, -1);
    acc->getCameraModule().reqQuake(soCameraModule::Quake_L, acc);
    acc->getControllerModule().setRumble(0x0d, 10, false, -1);
    Vec2f speed;
    Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float minimum = acc->getConstantFloatKirby(0xfbd);
    float range = acc->getConstantFloatKirby(0xfb8) - minimum;
    float multiplier = acc->getConstantFloatKirby(0xfbf);
    float threshold = multiplier * range;
    int kind = acc->getStageObject().soGetSubKind();
    if (__fabsf(speed.m_x) >= __fabsf(threshold)) {
        if (kind == 5) {
            acc->getSoundModule().playSE(static_cast<SndID>(0xc2a), false, false, 0);
        } else {
            acc->getSoundModule().playSE(static_cast<SndID>(0x17a3), false, false, 0);
        }
    } else if (kind == 5) {
        acc->getSoundModule().playSE(static_cast<SndID>(0xc29), false, false, 0);
    } else {
        acc->getSoundModule().playSE(static_cast<SndID>(0x17a2), false, false, 0);
    }
}

void ftPurinStatusUniqProcessSpecialNUtility::ftSetPowerPurinSpecialN(soModuleAccesser* acc) {
    Vec2f speed;
    Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float horizontal = __fabsf(speed.m_x);
    float threshold = acc->getConstantFloatKirby(0xfc2);
    bool canAttack;
    if (horizontal < threshold) {
        acc->getCollisionAttackModule().clear(0);
        canAttack = false;
    } else {
        soCollisionAttackModule& attack = acc->getCollisionAttackModule();
        attack.set(0, attack.getGroup(0));
        canAttack = true;
    }
    if (canAttack) {
        float multiplier = acc->getConstantFloatKirby(0xfb1);
        float offset = acc->getConstantFloatKirby(0xfb0);
        float sum = horizontal + offset;
        int power = static_cast<int>(sum * multiplier);
        if (power < 1) {
            power = 1;
        }
        if (acc->getCollisionAttackModule().isAttack(0, false)) {
            acc->getCollisionAttackModule().setPower(0, power, false);
        }
    }
}

void ftPurinStatusUniqProcessSpecialNUtility::ftPurinSpecialNInit(soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    float scale = acc->getPostureModule().getScale();
    work.setFloat(scale, 0x2100000c);
    work.setFloat(scale, 0x2100000d);
    work.setFloat(scale, 0x2100000e);
    work.setInt(acc->getConstantIntKirby(0x5dc0), 0x20000000);
    work.setInt(-1, 0x20000001);
    work.setInt(-1, 0x20000002);
    work.setInt(0, 0x20000003);
    work.setFloat(0.0f, 0x21000006);
    work.setFloat(0.0f, 0x2100000b);
    work.setFloat(0.0f, 0x21000009);
    work.setInt(0, 0x20000004);
    work.setInt(0, 0x20000005);
    work.setFloat(acc->getConstantFloatKirby(0xfb7), 0x21000005);
    work.offFlag(0x22000010);
}

void ftPurinStatusUniqProcessSpecialNUtility::ftProcDamageTurnPurinSpecialN(soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    Vec3f reflection(-1.0f, 1.0f, 1.0f);
    acc->getKineticModule().mulSpeed(&reflection, soKineticEnergy::AttributeFlag(1));
    acc->getKineticModule().mulAccel(&reflection, soKineticEnergy::AttributeFlag(1));
    // Reverse stored Rollout speeds, rotation phase and direction as well.
    work.setFloat(-work.getFloat(0x21000004), 0x21000004);
    work.setFloat(-work.getFloat(0x21000006), 0x21000006);
    work.setFloat(-work.getFloat(0x21000007), 0x21000007);
    work.setFloat(-work.getFloat(0x21000008), 0x21000008);
    work.setFloat(-work.getFloat(0x21000009), 0x21000009);
    work.setFloat(-work.getFloat(0x2100000a), 0x2100000a);
    work.setFloat(-work.getFloat(0x2100000b), 0x2100000b);
}

ftPurinStatusUniqProcessSpecialNHitEnd::~ftPurinStatusUniqProcessSpecialNHitEnd() { }
ftPurinStatusUniqProcessSpecialNEnd::~ftPurinStatusUniqProcessSpecialNEnd() { }
ftPurinStatusUniqProcessSpecialNTurn::~ftPurinStatusUniqProcessSpecialNTurn() { }
ftPurinStatusUniqProcessSpecialNRollAir::~ftPurinStatusUniqProcessSpecialNRollAir() { }
ftPurinStatusUniqProcessSpecialNRoll::~ftPurinStatusUniqProcessSpecialNRoll() { }
ftPurinStatusUniqProcessSpecialNHoldMax::~ftPurinStatusUniqProcessSpecialNHoldMax() { }
ftPurinStatusUniqProcessSpecialNHold::~ftPurinStatusUniqProcessSpecialNHold() { }
ftPurinStatusUniqProcessSpecialNStart::~ftPurinStatusUniqProcessSpecialNStart() { }

ftPurinStatusUniqProcessSpecialNStart g_ftPurinStatusUniqProcessSpecialNStart;
ftPurinStatusUniqProcessSpecialNHold g_ftPurinStatusUniqProcessSpecialNHold;
ftPurinStatusUniqProcessSpecialNHoldMax g_ftPurinStatusUniqProcessSpecialNHoldMax;
ftPurinStatusUniqProcessSpecialNRoll g_ftPurinStatusUniqProcessSpecialNRoll;
ftPurinStatusUniqProcessSpecialNRollAir g_ftPurinStatusUniqProcessSpecialNRollAir;
ftPurinStatusUniqProcessSpecialNTurn g_ftPurinStatusUniqProcessSpecialNTurn;
ftPurinStatusUniqProcessSpecialNEnd g_ftPurinStatusUniqProcessSpecialNEnd;
ftPurinStatusUniqProcessSpecialNHitEnd g_ftPurinStatusUniqProcessSpecialNHitEnd;
