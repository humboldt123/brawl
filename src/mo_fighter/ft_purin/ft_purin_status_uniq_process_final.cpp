#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/purin/ft_purin_status_uniq_process.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/so_value_accesser.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

// HYPOTHESIS names: status 0x116 grows, 0x11e holds, and 0x11f shrinks.
// Work float 0x21000004 counts phase frames, 05 preserves scale during
// hitstop, and 06 records the adjusted animation rate. Flag 11 starts the
// growth clock; flag 12 requests the one-time animation-rate adjustment.
void ftPurinStatusUniqProcessFinal::initStatus(soModuleAccesser* acc) {
    ftKineticEnergyDisableAndClear(3, acc);
    ftKineticEnergyDisableAndClear(1, acc);
    ftKineticEnergyDisableAndClear(2, acc);
    ftKineticEnergyDisableAndClear(4, acc);
    ftKineticEnergyDisableAndClear(0, acc);
    acc->getWorkManageModule().offFlag(0x22000011);
    acc->getWorkManageModule().setFloat(0.0f, 0x21000004);
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x116:
        acc->getWorkManageModule().setFloat(1.0f, 0x21000006);
        acc->getWorkManageModule().onFlag(0x22000012);
        break;
    case 0x11e:
        setModelTurn(acc);
        break;
    }
    acc->getGroundModule().setShapeFlag(soGroundShapeImpl::ShapeFlagId(3), true, 0);
}

void ftPurinStatusUniqProcessFinal::execStatus(soModuleAccesser* acc) {
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x116: {
        if (acc->getWorkManageModule().isFlag(0x22000011)) {
            acc->getWorkManageModule().addFloat(1.0f, 0x21000004);
            if (acc->getWorkManageModule().isFlag(0x22000012)) {
                soMotionModule& motion = acc->getMotionModule();
                float frame = motion.getFrame();
                float remaining = motion.getEndFrame(0x1ec) - frame;
                float rate = remaining / soValueAccesser::getConstantFloat(acc, 0xfce, 0);
                acc->getMotionModule().setRate(rate);
                acc->getWorkManageModule().setFloat(rate, 0x21000006);
                acc->getWorkManageModule().offFlag(0x22000012);
            }
        }
        soWorkManageModule& work = acc->getWorkManageModule();
        float duration = soValueAccesser::getConstantFloat(acc, 0xfce, 0);
        float progress = work.getFloat(0x21000004) / duration;
        float maximumScale = soValueAccesser::getConstantFloat(acc, 0xfcd, 0);
        float scale = 1.0f + progress * (maximumScale - 1.0f);
        acc->getPostureModule().setOwnerScale(scale);
        acc->getWorkManageModule().setFloat(scale, 0x21000005);
        break;
    }
    case 0x11e:
        acc->getWorkManageModule().addFloat(1.0f, 0x21000004);
        {
            float scale = soValueAccesser::getConstantFloat(acc, 0xfcd, 0);
            acc->getPostureModule().setOwnerScale(scale);
        }
        break;
    case 0x11f: {
        soWorkManageModule& work = acc->getWorkManageModule();
        float duration = soValueAccesser::getConstantFloat(acc, 0xfd0, 0);
        if (work.getFloat(0x21000004) < duration) {
            acc->getWorkManageModule().addFloat(1.0f, 0x21000004);
        }
        soWorkManageModule& scaleWork = acc->getWorkManageModule();
        duration = soValueAccesser::getConstantFloat(acc, 0xfd0, 0);
        float progress = scaleWork.getFloat(0x21000004) / duration;
        float maximumScale = soValueAccesser::getConstantFloat(acc, 0xfcd, 0);
        float scale = maximumScale + progress * (1.0f - maximumScale);
        acc->getPostureModule().setOwnerScale(scale);
        acc->getWorkManageModule().setFloat(scale, 0x21000005);
        break;
    }
    }
}

void ftPurinStatusUniqProcessFinal::execStop(soModuleAccesser* acc) {
    float scale = acc->getWorkManageModule().getFloat(0x21000005);
    acc->getPostureModule().setOwnerScale(scale);
}

void ftPurinStatusUniqProcessFinal::execFixPos(soModuleAccesser* acc) {
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x116: {
        soWorkManageModule& work = acc->getWorkManageModule();
        float duration = soValueAccesser::getConstantFloat(acc, 0xfce, 0);
        if (work.getFloat(0x21000004) >= duration) {
            acc->getStatusModule().changeStatusRequest(0x11e, acc);
        }
        break;
    }
    case 0x11e: {
        soWorkManageModule& work = acc->getWorkManageModule();
        float duration = soValueAccesser::getConstantFloat(acc, 0xfcf, 0);
        if (work.getFloat(0x21000004) >= duration) {
            acc->getStatusModule().changeStatusRequest(0x11f, acc);
        }
        break;
    }
    }
}

void ftPurinStatusUniqProcessFinal::exitStatus(soModuleAccesser*, int nextStatus) {
    // MATCH-ONLY: retain the native empty phase dispatch.
    switch (nextStatus) {
    case 0x116:
    case 0x11e:
    case 0x11f:
        return;
    }
}

void ftPurinStatusUniqProcessFinal::setModelTurn(soModuleAccesser* acc) {
    if (-1.0f == acc->getPostureModule().getLr()) {
        acc->getPostureModule().reverseRotYLr();
        acc->getStageObject().updateNodeSRT();
    }
}

ftPurinStatusUniqProcessFinal g_ftPurinStatusUniqProcessFinal;
