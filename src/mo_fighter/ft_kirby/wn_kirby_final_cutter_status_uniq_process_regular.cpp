#define MT_VEC2F_MEMBER_COPY // MATCH-ONLY: see mt_vector.h.
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/kirby/wn_kirby_final_cutter_status.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_value_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <math.h>

wnKirbyFinalCutterStatusUniqProcessRegular g_wnKirbyFinalCutterStatusUniqProcessRegular;

void wnKirbyFinalCutterStatusUniqProcessRegular::initStatus(soModuleAccesser*) { }

void wnKirbyFinalCutterStatusUniqProcessRegular::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    float angle = work.getFloat(0x11000001);
    float power = work.getFloat(0x11000002);
    float lr = moduleAccesser->getPostureModule().getLr();
    Vec2f velocity;
    float cosine = cos(angle);
    velocity.m_x = lr * (power * cosine);
    float sine = sin(angle);
    velocity.m_y = -power * sine;
    energy.m_speed = velocity;
    energy.enable();
    float next = power - soValueAccesser::getConstantFloat(moduleAccesser, 0xFA2, 0);
    if (next < 0.01f) {
        next = 0.01f;
    }
    work.setFloat(next, 0x11000002);
}

void wnKirbyFinalCutterStatusUniqProcessRegular::execMapCorrection(soModuleAccesser* moduleAccesser) {
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soGroundModule& ground = moduleAccesser->getGroundModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float angle;
    float oldAngle = work.getFloat(0x11000001);
    angle = oldAngle;
    if (situation.getKind() == Situation_Ground && ground.isTouch(static_cast<grCollStatus::TouchMask>(8), 0)) {
        Vec2f normal = ground.getTouchNormal(8, 0);
        float ny = normal.m_y;
        float nx = normal.m_x;
        angle = atan2(nx, ny);
        angle *= posture.getLr();
    }
    if (angle != oldAngle) {
        Vec3f rot(57.29578f * angle, 0.0f, 0.0f);
        posture.setRot(&rot, 0);
        work.setFloat(angle, 0x11000001);
    }
}

void wnKirbyFinalCutterStatusUniqProcessRegular::exitStatus(soModuleAccesser*, int) { }
