#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// Use the native Fighter observer override before transitive Fighter includes;
// this preserves the verified primary vtable slot of getOwner.
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_log_transactor.h>
#include <ft/ft_status_uniq_process_item_shoot.h>
#include <ft/ft_status_uniq_process_shoot.h>
#include <ft/ft_util.h>
#include <math.h>
#include <nw4r/math/math_arithmetic.h>
#include <so/controller/so_controller_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// MATCH-ONLY: the original pool starts with an unreferenced pair (0.3f, 0.23f) before the constants the functions load;
// the source construct that leaves them behind is unknown, so reserve them as an exported array.
extern const float g_ftItemShootCLauncherPoolPad[2] = { 0.3f, 0.23f };

// HYPOTHESIS: inline clamp helper (fsel based).
static inline float clampf(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

void ftStatusUniqProcessItemShoot::initStatus_CLauncher(soModuleAccesser* moduleAccesser) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    int prevStatusKind = moduleAccesser->getStatusModule().getPrevStatusKind(0);
    switch (moduleAccesser->m_stageObject->soGetSubKind()) {
    case 7:
        moduleAccesser->getWorkManageModule().onFlag(0x22000013);
        break;
    default:
        moduleAccesser->getWorkManageModule().offFlag(0x22000013);
        break;
    }
    if (!ftUtil::isItemShootStatus(moduleAccesser, prevStatusKind)) {
        moduleAccesser->getVisibilityModule().setDefaultAll();
        detachHoldAnim(moduleAccesser);
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000013) != true || moduleAccesser->getSituationModule().getKind() != 2) {
            moduleAccesser->getMotionModule().addPartialAnimChr(1, 0x173, 0x135, 0, 0.0f, 1.0f, 0);
        }
        if (moduleAccesser->getMotionModule().getPartialKind(1) != -1) {
            moduleAccesser->getMotionModule().setPartialRate(0.0f, 1);
        } else {
            moduleAccesser->getMotionModule().setRate(0.0f);
        }
        moduleAccesser->getWorkManageModule().setInt(0, 0x2000000C);
        setHoldAngle(moduleAccesser, soValueAccesser::getConstantFloat(moduleAccesser, 0xD79, 0));
        ftLogTransactor::resetLogActionInfo(2, 0x64, 1, dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner(), moduleAccesser);
        moduleAccesser->getControllerModule().resetTrigger();
    }
    int bullets = moduleAccesser->getItemManageModule().getShootItemBullet(0);
    switch (moduleAccesser->getWorkManageModule().getInt(0x2000000C)) {
    case 0:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000013) == true && moduleAccesser->getSituationModule().getKind() == 2) {
            moduleAccesser->getMotionModule().removePartialAnimChr(1);
            soMotionChangeParam param;
            param.m_kind = 0x17A;
            param.m_frame = 0.0f;
            param.m_rate = 0.0f;
            param._12 = 0;
            param._13 = 0;
            param._14 = 0;
            param._15 = 0;
            moduleAccesser->getMotionModule().changeMotionRequest(&param);
            setHoldAngle(moduleAccesser, 0.0f);
        } else {
            if (moduleAccesser->getMotionModule().getPartialKind(1) == -1) {
                soMotionModule& motion = moduleAccesser->getMotionModule();
                motion.addPartialAnimChr(1, 0x173, 0x135, 0, motion.getFrame(), 0.0f, 0);
                setHoldAngle(moduleAccesser, 0.0f);
            }
            ftStatusUniqProcessShoot::initShoot_CommonAirUpper(moduleAccesser, 0x150, 0x179);
        }
        break;
    case 1:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000013) == true && moduleAccesser->getSituationModule().getKind() == 2) {
            moduleAccesser->getMotionModule().removePartialAnimChr(1);
            soMotionChangeParam param;
            param.m_kind = 0x175;
            param.m_frame = 0.0f;
            param.m_rate = 1.0f;
            param._12 = 0;
            param._13 = 0;
            param._14 = 0;
            param._15 = 0;
            moduleAccesser->getMotionModule().changeMotionRequest(&param);
        } else {
            if (moduleAccesser->getMotionModule().getPartialKind(1) == -1) {
                moduleAccesser->getMotionModule().addPartialAnimChr(1, 0x173, 0x135, 0, 0.0f, 1.0f, 0);
            }
            if (statusKind != 0xAE) {
                if (bullets == 0) {
                    ftStatusUniqProcessShoot::initShoot_CommonAirUpper(moduleAccesser, 0x177, 0x178);
                } else {
                    ftStatusUniqProcessShoot::initShoot_CommonAirUpper(moduleAccesser, 0x174, 0x175);
                }
            } else {
                if (moduleAccesser->getMotionModule().getPartialKind(1) != -1) {
                    moduleAccesser->getMotionModule().setPartialRate(0.0f, 1);
                } else {
                    moduleAccesser->getMotionModule().setRate(0.0f);
                }
                moduleAccesser->getWorkManageModule().setInt(0, 0x2000000C);
                break;
            }
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000013) == true && moduleAccesser->getSituationModule().getKind() == 2) {
            ftUtil::setXAngle(moduleAccesser, moduleAccesser->getWorkManageModule().getFloat(0x21000008) - 90.0f);
        } else {
            setHoldAngle(moduleAccesser, 0.0f);
        }
        break;
    }
}

void ftStatusUniqProcessItemShoot::execStatus_CLauncher(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessShoot::execShoot_Common(moduleAccesser);
}

void ftStatusUniqProcessItemShoot::execFixPosCounter_CLauncher(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x2000000C)) {
    case 0: {
        Vec2f stick;
        ftUtil::getStickYRate(moduleAccesser, 0.3f, &stick);
        setHoldAngle(moduleAccesser, stick.m_x > 0.0f ? stick.m_x * -stick.m_y * soValueAccesser::getConstantFloat(moduleAccesser, 0xD7C, 0) : 0.0f);
        break;
    }
    case 1: {
        Vec2f stick;
        ftUtil::getStickYRate(moduleAccesser, 0.3f, &stick);
        if (stick.m_x > 0.0f) {
            float add = stick.m_x * -stick.m_y * soValueAccesser::getConstantFloat(moduleAccesser, 0xD7C, 0);
            float velocity = moduleAccesser->getWorkManageModule().getFloat(0x21000009);
            moduleAccesser->getWorkManageModule().setFloat(velocity + add, 0x21000009);
        }
        break;
    }
    }
}

void ftStatusUniqProcessItemShoot::execFixPos_CLauncher(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x2000000C)) {
    case 0: {
        int nextStatus;
        soControllerModule* const controller = &moduleAccesser->getControllerModule();
        if (controller->getTrigger() & soController::getButtonMask(soController::Pad_Button_Attack)) {
            moduleAccesser->getWorkManageModule().setInt(1, 0x2000000C);
            nextStatus = moduleAccesser->getSituationModule().getKind() == 0 ? 0xA4 : 0xAD;
            struct EffectVectors {
                Vec3f pos;
                Vec3f rot;
                Vec3f posRange;
                Vec3f rotRange;
            };
            EffectVectors airEffect;
            EffectVectors groundEffect;
            if (moduleAccesser->getItemManageModule().getShootItemBullet(0) == 0) {
                groundEffect.rotRange.m_x = 0.0f;
                groundEffect.rotRange.m_y = 0.0f;
                groundEffect.rotRange.m_z = 0.0f;
                groundEffect.posRange.m_x = 0.0f;
                groundEffect.posRange.m_y = 0.0f;
                groundEffect.posRange.m_z = 0.0f;
                groundEffect.rot.m_x = 0.0f;
                groundEffect.rot.m_y = 0.0f;
                groundEffect.rot.m_z = 0.0f;
                groundEffect.pos.m_x = 0.0f;
                groundEffect.pos.m_y = 0.0f;
                groundEffect.pos.m_z = 0.0f;
                moduleAccesser->getEffectModule().req(static_cast<EfID>(0x1A), 0, &groundEffect.pos, &groundEffect.rot, 1.0f, &groundEffect.posRange, &groundEffect.rotRange, false, 0);
                moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1F56), false, false, 0);
            } else {
                airEffect.rotRange.m_x = 0.0f;
                airEffect.rotRange.m_y = 0.0f;
                airEffect.rotRange.m_z = 0.0f;
                airEffect.posRange.m_x = 0.0f;
                airEffect.posRange.m_y = 0.0f;
                airEffect.posRange.m_z = 0.0f;
                airEffect.rot.m_x = 0.0f;
                airEffect.rot.m_y = 0.0f;
                airEffect.rot.m_z = 0.0f;
                airEffect.pos.m_x = 0.0f;
                airEffect.pos.m_y = 0.0f;
                airEffect.pos.m_z = 0.0f;
                moduleAccesser->getEffectModule().req(static_cast<EfID>(0x21), 0, &airEffect.pos, &airEffect.rot, 1.0f, &airEffect.posRange, &airEffect.rotRange, false, 0);
            }
            moduleAccesser->getStatusModule().changeStatusForce(nextStatus, moduleAccesser);
        }
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            if (moduleAccesser->getGroundModule().isPassableGround(0) == true) {
                soControllerModule* const stickController = &moduleAccesser->getControllerModule();
                float threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xC73, 0);
                if (stickController->getStickY() <= threshold) {
                    soControllerModule* const flickController = &moduleAccesser->getControllerModule();
                    int flickFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5A65, 0);
                    if ((u8)flickController->getFlickY() < flickFrames) {
                        moduleAccesser->getWorkManageModule().onFlag(0x22000011);
                    }
                }
            }
        }
        break;
    }
    case 1:
        if (moduleAccesser->getMotionModule().isEnd() == true) {
            if (moduleAccesser->getMotionModule().getPartialKind(1) != -1) {
                moduleAccesser->getMotionModule().setPartialRate(0.0f, 1);
            } else {
                moduleAccesser->getMotionModule().setRate(0.0f);
            }
            moduleAccesser->getWorkManageModule().setInt(0, 0x2000000C);
            int nextStatus = moduleAccesser->getSituationModule().getKind() == 0 ? 0xA4 : 0xAD;
            moduleAccesser->getStatusModule().changeStatusForce(nextStatus, moduleAccesser);
        } else {
            if (moduleAccesser->getWorkManageModule().isFlag(0x22000013) == true && moduleAccesser->getSituationModule().getKind() == 2) {
                ftUtil::setXAngle(moduleAccesser, moduleAccesser->getWorkManageModule().getFloat(0x21000008) - 90.0f);
            } else {
                setHoldAngle(moduleAccesser, 0.0f);
            }
        }
        break;
    }
}

void ftStatusUniqProcessItemShoot::exit_CLauncher(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    ftStatusUniqProcessShoot::exitShoot_CommonAirUpper(moduleAccesser, nextStatusKind);
    switch (nextStatusKind) {
    case 0x9B:
    case 0xA4:
    case 0xA5:
    case 0xA6:
    case 0xA7:
    case 0xA8:
    case 0xA9:
    case 0xAA:
    case 0xAB:
    case 0xAC:
    case 0xAD:
    case 0xAE:
        break;
    default:
        moduleAccesser->getItemManageModule().dropItem(0.0f, 0.0f, 0);
        break;
    }
}

// HYPOTHESIS: work float 0x21000008 is the current aim angle of the held launcher (clamped between status constants
// 0xD7A and 0xD7B) and 0x21000009 the angle velocity that decays towards zero; delta is the stick input for this frame.
void ftStatusUniqProcessItemShoot::setHoldAngle(soModuleAccesser* moduleAccesser, float delta) {
    float angle = 0.0f;
    float velocity = moduleAccesser->getWorkManageModule().getFloat(0x21000009);
    if (0.0f != velocity) {
        angle += 0.23f * velocity;
        velocity = velocity - angle;
        if ((float)__fabs(velocity) < 1.1920929e-07f) {
            velocity = 0.0f;
        }
        moduleAccesser->getWorkManageModule().setFloat(velocity, 0x21000009);
    }
    angle += delta;
    float lo = soValueAccesser::getConstantFloat(moduleAccesser, 0xD7A, 0);
    float hi = soValueAccesser::getConstantFloat(moduleAccesser, 0xD7B, 0);
    float value = moduleAccesser->getWorkManageModule().getFloat(0x21000008) + angle;
    value = clampf(value, lo, hi);
    moduleAccesser->getWorkManageModule().setFloat(value, 0x21000008);
    if (moduleAccesser->getMotionModule().getPartialKind(1) != -1) {
        moduleAccesser->getMotionModule().setPartialFrame(value, 1);
    } else {
        moduleAccesser->getMotionModule().setFrame(value);
    }
}
