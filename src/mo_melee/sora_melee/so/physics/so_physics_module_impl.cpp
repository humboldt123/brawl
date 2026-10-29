#define SO_PHYSICS_MODULE_IMPL_FULL
#include <ph/ph_2ndary_controller.h>
#include <so/physics/so_physics_module_impl.h>
#include <so/templates/so_array_value_soPhysicsIKHandle.h>
#include <types.h>

extern "C" void fn_8008F3E0(ph2ndary2Controller* controller, int status);
extern "C" void fn_80095990(ph2ndaryController* controller, const int* winding);

bool soPhysicsModuleImpl::isIK(int ikId) {
    return m_ikHandles->at(ikId).unk34 > 0.0f;
}

bool soPhysicsModuleImpl::is2nd() {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        return controller->unk20 != 0;
    }
    return false;
}

void soPhysicsModuleImpl::set2nd(int on) {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        if (!controller->unk20 && on) {
            controller->unk0E = 1;
        }
        controller->unk20 = on;
    }
}

void soPhysicsModuleImpl::set2ndGravity(float gravity) {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->m_gravity = gravity;
    }
}

void soPhysicsModuleImpl::set2ndType(int type) {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->setWeaponType(type);
    }
}

int soPhysicsModuleImpl::get2ndLineCount() {
    if (m_2ndaryController != NULL) {
        return m_2ndaryController->m_lines.getSize();
    }
    if (m_2ndary2Controller != NULL) {
        return m_2ndary2Controller->m_controller[0]->m_lines.getSize();
    }
    return 0;
}

int soPhysicsModuleImpl::get2ndActiveNodeNum() {
    if (m_2ndaryController != NULL) {
        return m_2ndaryController->getWeaponLineCount();
    }
    ph2ndary2Controller* controller = m_2ndary2Controller;
    if (controller != NULL) {
        if (controller->unk08[0]) {
            return controller->m_controller[1]->getWeaponLineCount();
        }
        return controller->m_controller[0]->getWeaponLineCount();
    }
    return 0;
}

void soPhysicsModuleImpl::set2ndActiveNodeNum(int num) {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->unk42 = num;
        controller->unk43 = num;
        return;
    }
    if (m_2ndary2Controller != NULL) {
        controller = m_2ndary2Controller->m_controller[0];
        controller->unk42 = num;
        controller->unk43 = num;
    }
}

void soPhysicsModuleImpl::set2ndBoth(int on) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    if (controller != NULL) {
        ph2ndaryController* first = controller->m_controller[0];
        if (first != NULL) {
            if (!first->unk20 && on) {
                first->unk0E = 1;
            }
            first->unk20 = on;
        }
        if (controller->m_controller[1] != NULL) {
            controller->m_controller[1]->unk20 = on;
        }
    }
}

void soPhysicsModuleImpl::set2ndBothStatus(int status) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    if (controller != NULL) {
        fn_8008F3E0(controller, status);
    }
}

int soPhysicsModuleImpl::get2ndBothSituation() {
    switch (m_2ndary2Controller->m_controller[0]->unk59 % 10) {
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            return 3;
        case 0:
        default:
            return 0;
    }
}

int soPhysicsModuleImpl::get2ndBothNodeMax() {
    return m_2ndary2Controller->m_controller[0]->unk5A;
}

void soPhysicsModuleImpl::set2ndBothSpeed(const Vec3f* speed, int lineId, u8 flag) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    controller->m_controller[0]->setLastVelocity(speed, lineId, flag);
    if (controller->m_controller[1] != NULL) {
        controller->m_controller[1]->unk5B = flag;
    }
}

void soPhysicsModuleImpl::set2ndBothRewind(int rewind) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    if (!controller->unk08[0]) {
        fn_80095990(controller->m_controller[0], &rewind);
    } else {
        fn_80095990(controller->m_controller[1], &rewind);
    }
}

void soPhysicsModuleImpl::set2ndBothWaver(int waver) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    ph2ndaryController* first = controller->m_controller[0];
    first->m_waver = waver;
    first->m_active = 1;
    ph2ndaryController* second = controller->m_controller[1];
    second->m_waver = waver;
    second->m_active = 1;
}

void soPhysicsModuleImpl::set2ndBothHitStat(int hitStat) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    controller->m_controller[0]->unk5B = hitStat;
    if (controller->m_controller[1] != NULL) {
        controller->m_controller[1]->unk5B = hitStat;
    }
}

void soPhysicsModuleImpl::set2ndBothReverseFlag(int flag) {
    ph2ndary2Controller* controller = m_2ndary2Controller;
    if (controller != NULL) {
        controller->unk08[2] = flag;
    }
}

u32 soPhysicsModuleImpl::get2ndNodeId(int lineId) {
    if (m_2ndaryController != NULL) {
        return m_2ndaryController->getWeaponStartID(lineId);
    }
    if (m_2ndary2Controller != NULL) {
        return m_2ndary2Controller->m_controller[0]->getWeaponStartID(lineId);
    }
    return 0;
}

void soPhysicsModuleImpl::set2ndPosSplit(int split) {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->unkE8 = split;
        return;
    }
    if (m_2ndary2Controller != NULL) {
        m_2ndary2Controller->m_controller[0]->unkE8 = split;
    }
}

bool soPhysicsModuleImpl::is2ndHitCollision() {
    if (m_2ndaryController != NULL) {
        return m_2ndaryController->isHitCollisionLastNode2Pos(7);
    }
    if (m_2ndary2Controller != NULL) {
        return m_2ndary2Controller->isHitCollisionLastNode2Pos();
    }
    return false;
}

void soPhysicsModuleImpl::set2ndWeaponScale(float scale) {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->m_weaponScale = scale;
        return;
    }
    ph2ndary2Controller* controller2 = m_2ndary2Controller;
    if (controller2 != NULL) {
        controller2->m_controller[0]->m_weaponScale = scale;
        controller2->m_controller[1]->m_weaponScale = scale;
    }
}

void soPhysicsModuleImpl::set2ndCliffDir(float lr) {
    bool left = lr == -1.0f;
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->unk56 = left;
        return;
    }
    ph2ndary2Controller* controller2 = m_2ndary2Controller;
    if (controller2 != NULL) {
        controller2->m_controller[0]->unk56 = left;
        controller2->m_controller[1]->unk56 = left;
    }
}

void soPhysicsModuleImpl::sync2ndSavedPosition() {
    ph2ndaryController* controller = m_2ndaryController;
    if (controller != NULL) {
        controller->unk0E = 1;
    }
}

bool soPhysicsModuleImpl::isIK() {
    if (unk30 != 0) {
        for (int i = 0; i < m_ikHandles->size(); i++) {
            if (isIK(i) == true) {
                return true;
            }
        }
    }
    return false;
}

bool soPhysicsModuleImpl::isObserv(char group) {
    return group == 0x17;
}

void soPhysicsModuleImpl::set2ndSleep(bool sleep) {
    m_2ndSleep = sleep;
}

void soPhysicsModuleImpl::set2ndPreCollisionNode(int nodeId) {
    m_2ndPreCollisionNode = nodeId;
}

void soPhysicsModuleImpl::set2ndDefault() {
    set2nd(m_2ndDefault);
}

void soPhysicsModuleImpl::set2ndDefault(int on) {
    m_2ndDefault = on;
}
