#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/builder/ft_builder_kinetic.h>
#include <ft/pikachu/ft_pikachu.h>
#include <ft/pikachu/ft_pikachu_status_uniq_process.h>
#include <cm/cm_camera_controller.h>
#include <gf/gf_task.h>
#include <so/camera/so_camera_module_simple.h>
#include <so/controller/so_controller_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/visibility/so_visibility_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// The event the Volt Tackle sends to its linked articles (HYPOTHESIS: event kind 0x455 is the start of the attack).
class ftPikachuVortexLinkEvent : public soLinkEventArgs {
public:
    ftPikachuVortexLinkEvent(int kind) : soLinkEventArgs(kind) { }
};

float clampf(register float value, register float low, register float high);

void ftPikachuStatusUniqProcessSpecialVortexStart::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftPikachuStatusUniqProcessSpecialVortexAttack::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    moduleAccesser->getVisibilityModule().setWhole(0);
    moduleAccesser->getEffectModule().removeAll(8);
    work.setInt((int)(60.0f * soValueAccesser::getConstantFloat(moduleAccesser, 0xfca, 0)), ftPikachuWork::Vortex_Frames);
    work.setFloat(1.0f, ftPikachuWork::Vortex_Scale);
}

void ftPikachuStatusUniqProcessSpecialVortexAttack::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soControllerModule& controller = moduleAccesser->getControllerModule();
    soGenerateArticleManageModule& articles = moduleAccesser->getGenerateArticleManageModule();
    int cooldown = work.getInt(ftPikachuWork::Vortex_Cooldown);
    work.decInt(ftPikachuWork::Vortex_Cooldown);
    if (cooldown <= 0) {
        u32 mask = soController::getButtonMask(static_cast<soController::PadButton>(0));
        u32 trigger = controller.getTrigger();
        if (trigger & mask) {
            articles.changeStatus(2, 1, 0);
            work.setInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dc7, 0), ftPikachuWork::Vortex_Cooldown);
        }
    }
    gfTask* task = gfTask::getTask(work.getInt(ftPikachuWork::Vortex_FounderTask));
    if (task) {
        StageObject& founder = dynamic_cast<StageObject&>(*task);
        if (soExternalValueAccesser::getWorkFlag(&founder, ftPikachuWork::Vortex_FounderActive)) {
            work.onFlag(ftPikachuWork::Vortex_Free);
        } else {
            work.offFlag(ftPikachuWork::Vortex_Free);
        }
    }
    if (!work.isFlag(ftPikachuWork::Vortex_Linked)) {
        int frames = work.getInt(ftPikachuWork::Vortex_Frames);
        if (frames <= soValueAccesser::getConstantInt(moduleAccesser, 0x5dc6, 0)) {
            ftPikachuVortexLinkEvent event(0x455);
            moduleAccesser->getLinkModule().sendEventNodes(3, event, 0);
            work.setFloat(0.5f, ftPikachuWork::Vortex_Scale);
            work.onFlag(ftPikachuWork::Vortex_Linked);
        }
    }
}

void ftPikachuStatusUniqProcessSpecialVortexAttack::execMapCorrection(soModuleAccesser* moduleAccesser) {
    soGroundModule& ground = moduleAccesser->getGroundModule();
    if (moduleAccesser->getSituationModule().getKind() == 0) {
        if (ground.isTouch(static_cast<grCollStatus::TouchMask>(8), 0)) {
            Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::ATTRIBUTE_MASK_MAIN);
            if (speed.length() > 0.00000011920929f) {
                Vec2f normal = ground.getTouchNormal(8, 0);
                speed.normalize();
                if (normal.m_x * speed.m_x + normal.m_y * speed.m_y > 0.00000011920929f) {
                    moduleAccesser->getSituationModule().setKind(static_cast<SituationKind>(2), false);
                }
            }
        }
    }
}

void ftPikachuStatusUniqProcessSpecialVortexAttack::execFixPos(soModuleAccesser* moduleAccesser) {
    soPostureModule& posture = moduleAccesser->getPostureModule();
    Vec3f position;
    position = posture.getPos();
    float right = CameraController::getInstance()->m_stageCameraBounds.m_right;
    float left = CameraController::getInstance()->m_stageCameraBounds.m_left;
    float x = clampf(position.m_x, left, right);
    float up = CameraController::getInstance()->m_stageCameraBounds.m_up;
    float down = CameraController::getInstance()->m_stageCameraBounds.m_down;
    float y = clampf(position.m_y, down, up);
    if (x != position.m_x || y != position.m_y) {
        position.m_x = x;
        position.m_y = y;
        posture.setPos(&position);
        posture.updateRotYLr();
        moduleAccesser->getStageObject().updateNodeSRT();
    }
}

float clampf(register float value, register float low, register float high) {
    register float difference;
    difference = value - low;
    // MATCH-ONLY: the fighter units are built without inlining of the nw4r FSelect asm helper.
    asm { fsel value, difference, value, low }
    difference = value - high;
    asm { fsel value, difference, high, value }
    return value;
}

void ftPikachuStatusUniqProcessSpecialVortexAttack::adjust_camera_subject(soModuleAccesser* moduleAccesser, soCameraModule* source) {
    soCameraModule& camera = moduleAccesser->getCameraModule();
    Rect2D range;
    source->getCameraRangeGlobalRect(&range, 0);
    camera.setCameraRangeGlobalRect(&range, moduleAccesser, 0);
    camera.setOffsetGlobalRectCenter(&range, moduleAccesser, 0);
}

void ftPikachuStatusUniqProcessSpecialVortexAttack::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getVisibilityModule().setWhole(1);
    moduleAccesser->getCameraModule().setWhole(true);
}

void ftPikachuStatusUniqProcessSpecialVortexEnd::initStatus(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    kinetic.unableEnergyAll();
    kinetic.clearSpeedAll();
}

ftPikachuStatusUniqProcessSpecialVortexStart g_ftPikachuStatusUniqProcessSpecialVortexStart;
ftPikachuStatusUniqProcessSpecialVortexAttack g_ftPikachuStatusUniqProcessSpecialVortexAttack;
ftPikachuStatusUniqProcessSpecialVortexEnd g_ftPikachuStatusUniqProcessSpecialVortexEnd;
