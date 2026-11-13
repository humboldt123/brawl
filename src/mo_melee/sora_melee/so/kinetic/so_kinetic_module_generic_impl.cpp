#define SO_KINETIC_MODULE_IMPL_CTOR_2
#include <ft/builder/ft_builder_kinetic.h>
#include <so/event/so_event_system.h>
#include <so/so_instance_manager.h>
#include <so/so_module_accesser.h>
#include <types.h>

// The kinetic module of the fighters: soKineticModuleImpl plus a mediator that owns the fixed fighter energies (the
// first getMediateNum() entries of the energy manager) and is told about every kinetic change first.

soKineticModuleGenericImpl::soKineticModuleGenericImpl(soModuleAccesser* acc, ftKineticEnergyManager* manager,
                                                       void* mediator) :
    soKineticModuleImpl(acc, manager),
    soStatusEventObserver(static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(), -1) {
    m_mediator = static_cast<soKineticMediator*>(mediator);
}

void soKineticModuleGenericImpl::changeKinetic(int kineticType, soModuleAccesser* moduleAccesser) {
    m_mediator->changeKinetic(kineticType, moduleAccesser);
    soKineticModuleImpl::changeKinetic(kineticType, moduleAccesser);
}

void soKineticModuleGenericImpl::updateEnergy(soModuleAccesser* moduleAccesser, soKineticEnergy::AttributeFlag flag) {
    if (flag.m_mask == -1) {
        m_mediator->updateEnergy(moduleAccesser);
    } else {
        m_mediator->updateEnergy1(moduleAccesser, soKineticAttributeMask(flag.m_mask));
    }
    soInstanceManagerFullProperty<soKineticEnergy*>* manager = m_kineticEnergyInstanceManager;
    int size = manager->size();
    int mediateNum = m_mediator->getMediateNum();
    if (size > mediateNum) {
        for (int i = 0; i < size; i++) {
            if (manager->getId(i) >= mediateNum) {
                soKineticEnergy* energy = manager->atIndex(i);
                if (energy->isEnable() && energy->isSuspend() != true) {
                    energy->updateEnergy(moduleAccesser);
                }
            }
        }
    }
    // The original passes the accesser here too (r4); the interface declares no argument.
    m_mediator->postUpdateEnergy();
}

// The kinetic mode a status asks for sits in a signed byte of the status data.
struct soStatusKineticData {
    u8 pad[0xC];
    u32 unk0 : 11;
    s32 kineticType : 8;
    u32 unk19 : 13;
};

void soKineticModuleGenericImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                                         soModuleAccesser* moduleAccesser) {
    soStatusKineticData* kineticData = (soStatusKineticData*)statusData;
    if (kineticData->kineticType != -1) {
        m_mediator->notifyEventChangeStatus((void*)statusKind, (void*)prevStatusKind, statusData, moduleAccesser);
        changeKinetic(kineticData->kineticType, moduleAccesser);
    }
}

void soKineticModuleGenericImpl::eraseEnergy(int index) {
    if (index >= m_mediator->getMediateNum()) {
        soKineticModuleImpl::eraseEnergy(index);
    }
}

void soKineticModuleGenericImpl::eraseEnergyAll() {
}

void soKineticModuleGenericImpl::addSpeed(Vec3f* speed, soModuleAccesser* moduleAccesser) {
    m_mediator->addSpeed(speed, moduleAccesser);
}

void soKineticModuleGenericImpl::addSpeedOutside(soKineticEnergy::OutsideType type, Vec3f* speed) {
    m_mediator->addSpeedOutside(type, speed, m_moduleAccesser);
}
