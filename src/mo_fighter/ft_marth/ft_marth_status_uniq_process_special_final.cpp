#include <ft/marth/ft_marth_status_uniq_process.h>
#include <ft/marth/ft_marth.h>
#include <ft/ft_kinetic_energy.h>
#include <gf/gf_task.h>
#include <if/if_marth_final.h>
#include <mt/mt_prng.h>
#include <so/so_module_accesser.h>
#include <so/so_photo_call_back.h>
#include <so/so_value_accesser.h>
#include <math.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

void ftMarthStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyDisableAndClear(3, moduleAccesser);
    ftKineticEnergyDisableAndClear(1, moduleAccesser);
    ftKineticEnergyDisableAndClear(2, moduleAccesser);
    ftKineticEnergyDisableAndClear(4, moduleAccesser);
    ftKineticEnergyDisableAndClear(0, moduleAccesser);
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case ftMarthStatus::FinalStart:
        // Six HP-window slots have independent movement and visibility timers.
        for (int i = 0; i < 6; i++) {
            moduleAccesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(moduleAccesser, ftMarthParam::Final_WindowShowFrames, 0), ftMarthWork::Final_WindowShowTimer + i);
            moduleAccesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(moduleAccesser, ftMarthParam::Final_WindowMoveFrames, 0), ftMarthWork::Final_WindowMoveTimer + i);
            moduleAccesser->getWorkManageModule().setFloat(randi(360), ftMarthWork::Final_WindowAngle + i);
        }
        break;
    case ftMarthStatus::FinalDash: {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        float lr = moduleAccesser->getPostureModule().getLr();
        float factor = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::Final_SpeedX, 0);
        factor *= lr;
        float speed = factor;
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            stop.resetEnergy(6, &Vec2f(speed, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        } else {
            stop.resetEnergy(0, &Vec2f(speed, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        }
        stop.m_brake = Vec2f(0.0f, 0.0f);
        stop.m_speedLimit = Vec2f(-1.0f, 0.0f);
        stop.enable();
        moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getSituationModule().getKind(), ftMarthWork::Final_Situation);
        break;
    }
    case ftMarthStatus::FinalHit: {
        ftMarth* marth = dynamic_cast<ftMarth*>(&moduleAccesser->getStageObject());
        if (marth != NULL) {
            marth->addCallback();
        }
        break;
    }
    }
}

// MATCH-ONLY: the original keeps this two-coordinate constructor out of line.
#pragma dont_inline on
static void constructWindowOffset(Vec2f* offset, float x, float y) {
    offset->m_x = x;
    offset->m_y = y;
}
#pragma dont_inline off

void ftMarthStatusUniqProcessFinal::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case ftMarthStatus::FinalDash: {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        float lr = moduleAccesser->getPostureModule().getLr();
        float factor = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::Final_SpeedX, 0);
        factor *= lr;
        float speed = factor;
        // This routine compares against the saved situation without updating that snapshot.
        if (moduleAccesser->getSituationModule().getKind() != moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_Situation)) {
            if (moduleAccesser->getSituationModule().getKind() == 2) {
                stop.resetEnergy(6, &Vec2f(speed, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            } else {
                stop.resetEnergy(0, &Vec2f(speed, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            }
            stop.m_brake = Vec2f(0.0f, 0.0f);
            stop.m_speedLimit = Vec2f(-1.0f, 0.0f);
        } else {
            stop.m_speed = Vec2f(speed, 0.0f);
        }
        break;
    }
    case ftMarthStatus::FinalUnk:
        break;
    case ftMarthStatus::FinalHit:
        updateHpWindow(moduleAccesser);
        break;
    }
}

void ftMarthStatusUniqProcessFinal::execStop(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case ftMarthStatus::FinalHit:
        updateHpWindow(moduleAccesser);
        break;
    }
}

void ftMarthStatusUniqProcessFinal::execFixPos(soModuleAccesser*) {}

void ftMarthStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case ftMarthStatus::FinalHit: {
        ftMarth* marth = dynamic_cast<ftMarth*>(&moduleAccesser->getStageObject());
        if (marth == NULL) return;
        marth->removeCallBack();
        break;
    }
    }
    // Preserve the window tasks between Final Smash phases; delete them on exit.
    switch (status) {
    case ftMarthStatus::FinalDash:
    case ftMarthStatus::FinalUnk:
    case ftMarthStatus::FinalHit:
        return;
    default: {
        int count = moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_WindowCount);
        for (int i = 0; i < count; i++) {
            gfTask::getTask(moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_WindowTaskId + i))->exit();
        }
        break;
    }
    }
}

void ftMarthStatusUniqProcessFinal::updateHpWindow(soModuleAccesser* moduleAccesser) {
    int count = moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_WindowCount);
    if (count > 0) {
        for (int i = 0; i < count; i++) {
            if (moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_WindowMoveTimer + i) > 0) {
                moduleAccesser->getWorkManageModule().subInt(1, ftMarthWork::Final_WindowMoveTimer + i);
                // MATCH-ONLY: reserve trig temporaries before the angle to retain register order.
                float y, x;
                // A fixed per-slot angle produces a repeated linear step while its timer runs.
                float angle = moduleAccesser->getWorkManageModule().getFloat(ftMarthWork::Final_WindowAngle + i);
                float stepSize = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::Final_WindowStep, 0);
                y = sin(angle);
                x = cos(angle);
                Vec2f offset;
                constructWindowOffset(&offset, stepSize * x, stepSize * y);
                IfMarthFinalTask* window = dynamic_cast<IfMarthFinalTask*>(gfTask::getTask(moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_WindowTaskId + i)));
                if (window != NULL && window->isExecutedCallBack() == 1) {
                    Vec3f position = window->getGlobalPos(0);
                    position.m_x += offset.m_x;
                    position.m_y += offset.m_y;
                    window->setPos(&position, 0);
                }
            }
            if (moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_WindowShowTimer + i) > 0) {
                moduleAccesser->getWorkManageModule().subInt(1, ftMarthWork::Final_WindowShowTimer + i);
                if (moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_WindowShowTimer + i) == 0) {
                    IfMarthFinalTask* window = dynamic_cast<IfMarthFinalTask*>(gfTask::getTask(moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_WindowTaskId + i)));
                    if (window != NULL) window->dispOff(0);
                }
            }
        }
    }
}

ftMarthStatusUniqProcessFinal g_ftMarthStatusUniqProcessFinal;
