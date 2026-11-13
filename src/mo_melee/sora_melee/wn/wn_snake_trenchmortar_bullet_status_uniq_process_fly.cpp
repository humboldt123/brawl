#include <mt/mt_vector.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the fly status of Snake's mortar shell: once the shell slows below a speed it starts to tip over,
// turning by a growing step each frame until it has rotated half a turn. Work flags (HYPOTHESIS names):
// 0x22000003 first frame done, 0x22000001 tipping, 0x22000002 tipped over.
// HYPOTHESIS: the unit's .rodata starts with a float no function here loads (likely the pool of a function that was
// stripped at link time).
namespace wn_snake_trenchmortar_bullet_status_uniq_process_fly_link_ref {
    extern const float unused_float = 180.0f;
}

struct wnSnakeTrenchMortarBulletFlyParam {
    u8 _00[0x28];
    float m_tipSpeed;     // 0x28
    float m_tipStep;      // 0x2C
    float m_tipStepAccel; // 0x30
};

class wnSnakeTrenchMortarBulletStatusUniqProcessFly : public soStatusUniqProcess {
public:
    const wnSnakeTrenchMortarBulletFlyParam* m_param; // 0x04
    float m_tipStep;                                  // 0x08

    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void wnSnakeTrenchMortarBulletStatusUniqProcessFly::execStatus(soModuleAccesser* moduleAccesser) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x22000003)) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000003);
        return;
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000001)) {
        Vec3f rot = moduleAccesser->getPostureModule().getRot(1);
        m_tipStep += m_param->m_tipStepAccel;
        rot.m_x += m_tipStep;
        if (rot.m_x > 180.0f) {
            rot.m_x = 180.0f;
            moduleAccesser->getWorkManageModule().onFlag(0x22000002);
        }
        moduleAccesser->getPostureModule().setRot(&rot, 1);
        return;
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000002)) {
        return;
    }
    float speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(-1)).length();
    if (speed < m_param->m_tipSpeed) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000001);
        Vec3f rot = moduleAccesser->getPostureModule().getRot(1);
        Vec3f baseRot = moduleAccesser->getPostureModule().getRot(0);
        rot.m_z = baseRot.m_z;
        moduleAccesser->getPostureModule().setRot(&rot, 1);
        m_tipStep = m_param->m_tipStep;
    }
}
