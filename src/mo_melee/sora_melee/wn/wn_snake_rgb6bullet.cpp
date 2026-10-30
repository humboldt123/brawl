#include <wn/snake/wn_snake_rgb6_bullet.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/kinetic/so_kinetic_energy_3f.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>

void wnSnakeRgb6Bullet::onDeactivate() {
    m_moduleAccesser->getEffectModule().killAll(0, false, true);
}

void wnSnakeRgb6Bullet::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                                soModuleAccesser* moduleAccesser) {
    Weapon::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (statusKind == 1) {
        Vec3f pos(m_moduleAccesser->getWorkManageModule().getFloat(0x11000001),
                  m_moduleAccesser->getWorkManageModule().getFloat(0x11000002), 0.0f);
        m_moduleAccesser->getPostureModule().setPos(&pos);
        updateNodeSRT();
        soKineticEnergy3f* energy = dynamic_cast<soKineticEnergy3f*>(m_moduleAccesser->getKineticModule().getEnergy(2));
        if (energy != nullptr) {
            energy->clearSpeed();
            energy->disable();
        }
    }
}

bool wnSnakeRgb6Bullet::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}
