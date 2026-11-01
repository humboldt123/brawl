#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wario/wn_wario_bike_status_uniq_process.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <math.h>

#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE

void wnWarioBikeStatusUniqProcessAppeal::execFixPosCounter(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soCollisionAttackModule& attack = a->getCollisionAttackModule();
    soKineticModule& kinetic = a->getKineticModule();
    wnWarioBikeParam* param = dynamic_cast<wnWarioBike&>(a->getStageObject()).m_param;
    float speed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1)).length();
    if (speed <= param->unk34) {
        if (attack.isAttack(0, false)) {
            attack.clear(0);
            work.onFlag(0x2200000a);
        }
    } else {
        float power = param->damageGrowth * ((speed - param->unk34) / (param->groundTopSpeed - param->unk34));
        if (attack.isAttack(0, false)) attack.setPowerMul(power);
        else if (work.isFlag(0x2200000a)) {
            attack.set(0, 0);
            attack.setPowerMul(power);
            work.offFlag(0x2200000a);
        }
    }
}

void wnWarioBikeStatusUniqProcessAppeal::execFixPos(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soSituationModule& situation = a->getSituationModule();
    soGroundModule& ground = a->getGroundModule();
    soKineticModule& kinetic = a->getKineticModule();
    soMotionModule& motion = a->getMotionModule();
    wnWarioBikeParam* param = dynamic_cast<wnWarioBike&>(a->getStageObject()).m_param;

    bool frontContact = work.isFlag(0x22000004);
    bool rearContact = work.isFlag(0x22000005);
    bool groundContact = work.isFlag(0x22000006);
    float phase = work.getFloat(0x21000000);
    float groundPhase = work.getFloat(0x21000002);
    float targetPhase = work.getFloat(0x21000003);
    soKineticEnergyNormal& normal =
        dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(0));
    wnKineticEnergyGravity& gravity =
        dynamic_cast<wnKineticEnergyGravity&>(*kinetic.getEnergy(1));
    bool changeKinetic = false;
    float correctionSpeed = 0.0f;

    if (work.isFlag(0x22000002)) {
        if (frontContact || rearContact) {
            float difference = __fabsf(phase - groundPhase);
            if (difference > param->unk48 && !work.isFlag(0x22000007)) {
                work.onFlag(0x22000007);
                correctionSpeed = (difference / 90.0f) * param->landingBounceHeight;
                changeKinetic = true;
            }
        }
        work.offFlag(0x22000002);
    } else if (groundContact) {
        work.offFlag(0x22000007);
        float difference = groundPhase - targetPhase;
        if (difference > param->unk48) {
            correctionSpeed = (difference / 90.0f) * param->landingBounceHeight;
            changeKinetic = true;
        }
    }

    if (changeKinetic) {
        gravity.m_speedY = correctionSpeed;
        gravity.enable();
        kinetic.changeKinetic(33, a);
        situation.setKind(Situation_Air, false);
        ground.setCorrect(soGroundShapeImpl::Correct_Air, 0);
        work.offFlag(0x22000004);
        work.offFlag(0x22000005);
        work.offFlag(0x22000006);
    } else if (frontContact || rearContact || groundContact) {
        if (groundContact) {
            if (phase > groundPhase) {
                phase -= param->unk64;
                if (phase < groundPhase) phase = groundPhase;
            } else if (phase < groundPhase) {
                phase += param->unk64;
                if (phase > groundPhase) phase = groundPhase;
            }
        } else if (!frontContact && rearContact) {
            phase -= param->unk64;
            if (groundContact) {
                if (phase < groundPhase) phase = groundPhase;
            } else if (phase < -45.0f) {
                phase = -45.0f;
            }
        } else if (!rearContact && frontContact) {
            phase += param->unk64;
            if (groundContact) {
                if (phase > groundPhase) phase = groundPhase;
            } else if (phase > 45.0f) {
                phase = 45.0f;
            }
        }
    }
    work.setFloat(phase, 0x21000000);

    float speed = normal.getSpeed().length();
    float position;
    float range;
    if (frontContact || rearContact) {
        position = speed - param->unk14;
        range = param->groundTopSpeed - param->unk14;
    } else {
        position = speed - param->unk20;
        range = param->airTopSpeed - param->unk20;
    }
    if (position < 0.0f) position = 0.0f;
    else if (position > range) position = range;
    float blend = position / range;
    float rate = param->unk44 + blend * (param->unk40 - param->unk44);
    motion.setRate(rate);
}

wnWarioBikeStatusUniqProcessAppeal g_wnWarioBikeStatusUniqProcessAppeal;
