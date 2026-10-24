#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_manager.h>
#include <math.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <st/stage.h>
#include <types.h>
#include <wn/wn_kinetic_energy_normal.h>

static bool isNearZero(float value);

// HYPOTHESIS: the attack of the Ikarus flies at the nearest fighter (or at the camera center when nobody is around).
class wnPitIkarusStatusUniqProcessAttack : public soStatusUniqProcess {
public:
    virtual ~wnPitIkarusStatusUniqProcessAttack() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

wnPitIkarusStatusUniqProcessAttack g_wnPitIkarusStatusUniqProcessAttack;

void wnPitIkarusStatusUniqProcessAttack::initStatus(soModuleAccesser* moduleAccesser) {
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(2));
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    Fighter* target = g_ftManager->searchNearFighter(0.0f, -1.0f, &pos, moduleAccesser->getTeamModule().getTeam()->getNo(), true);
    Vec3f targetPos;
    if (target != NULL) {
        targetPos = soExternalValueAccesser::getPos(target);
    } else {
        targetPos = g_Stage->m_cameraParam1->m_centerPos;
    }
    Vec2f dir(targetPos.m_x - pos.m_x, targetPos.m_y - pos.m_y);
    bool zero = false;
    if (isNearZero(dir.m_x) && isNearZero(dir.m_y)) {
        zero = true;
    }
    if (!zero) {
        dir.normalize();
    } else {
        dir.m_y = 1.0f;
    }
    float speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa3, 0);
    dir.m_x *= speed;
    dir.m_y *= speed;
    energy.m_speed.m_x = dir.m_x;
    energy.m_speed.m_y = dir.m_y;
    if (moduleAccesser->getPostureModule().getLr() * dir.m_x < 0.0f) {
        moduleAccesser->getWorkManageModule().setFlag(true, 0x22000000);
    }
}

static bool isNearZero(float value) {
    return fabs(value) < 1.0e-5f;
}
