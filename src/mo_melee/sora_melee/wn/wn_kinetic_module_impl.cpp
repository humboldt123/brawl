#define WN_ARTICLE_NATIVE_BUILDERS
#define SO_KINETIC_MODULE_IMPL_CTOR_2
#define WN_ARTICLE_ANIMCMD_INT
#include <mt/mt_vector.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/event/so_event_system.h>
#include <wn/wn_kinetic_module_impl.h>
#include <types.h>

// The kinetic module of the generic article (Weapon): it owns the two energies and switches between their modes.

wnKineticModuleImpl::wnKineticModuleImpl(soModuleAccesser* acc, soInstanceManagerFullProperty<soKineticEnergy*>* energies, soKineticMediator* mediator) :
    soKineticModuleImpl(acc, energies),
    soStatusEventObserver(static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(), -1),
    m_outsideGroundEnergyId(-1) {
}

// The kinetic mode a status asks for sits in a signed byte of the status data.
struct wnStatusKineticData {
    u8 pad[0xC];
    u32 unk0 : 11;
    s32 kineticType : 8;
    u32 unk19 : 13;
};

void wnKineticModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* data, soModuleAccesser* acc) {
    int kineticType = ((wnStatusKineticData*)data)->kineticType;
    if (kineticType > -1) {
        changeKinetic(kineticType, acc);
    }
}

typedef void (wnKineticModuleImpl::*wnKineticInitFunc)(int, soModuleAccesser*);
static const wnKineticInitFunc sKineticInit[3] = {
    &wnKineticModuleImpl::initializeNormal,
    &wnKineticModuleImpl::initializeStop,
    &wnKineticModuleImpl::initializeReset,
};

void wnKineticModuleImpl::changeKinetic(int kineticType, soModuleAccesser* acc) {
    if (kineticType <= -1 || kineticType >= 3) {
        return;
    }
    (this->*sKineticInit[kineticType])(kineticType, acc);
}

void wnKineticModuleImpl::initializeNormal(int kineticType, soModuleAccesser* acc) {
    soInstanceManagerFullProperty<soKineticEnergy*>* manager = m_kineticEnergyInstanceManager;
    soKineticEnergy* main = manager->at(0);
    soKineticEnergy* sub = manager->at(1);
    if (kineticType == 0) {
        Vec2f speed = acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        Vec3f zero3(0.0f, 0.0f, 0.0f);
        main->resetEnergy(0, speed, zero3, acc);
        Vec3f zeroRot(0.0f, 0.0f, 0.0f);
        sub->resetEnergy(-1, Vec2f(0.0f, 0.0f), zeroRot, acc);
    }
    main->enable();
    sub->enable();
}

void wnKineticModuleImpl::initializeStop(int kineticType, soModuleAccesser* acc) {
    soInstanceManagerFullProperty<soKineticEnergy*>* manager = m_kineticEnergyInstanceManager;
    soKineticEnergy* main = manager->at(0);
    soKineticEnergy* sub = manager->at(1);
    if (kineticType == 0) {
        Vec2f speed = acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        Vec3f zero3(0.0f, 0.0f, 0.0f);
        main->resetEnergy(1, speed, zero3, acc);
        Vec3f zeroRot(0.0f, 0.0f, 0.0f);
        sub->resetEnergy(-1, Vec2f(0.0f, 0.0f), zeroRot, acc);
    }
    main->enable();
    sub->enable();
}

void wnKineticModuleImpl::initializeReset(int, soModuleAccesser*) {
    soInstanceManagerFullProperty<soKineticEnergy*>* manager = m_kineticEnergyInstanceManager;
    soKineticEnergy* main = manager->at(0);
    main->clearSpeed();
    main->disable();
    soKineticEnergy* sub = manager->at(1);
    sub->clearSpeed();
    sub->disable();
}

void wnKineticModuleImpl::setOutsideMovementGroundEnergyId(soKineticModule* module, int energyId) {
    wnKineticModuleImpl* impl = dynamic_cast<wnKineticModuleImpl*>(module);
    if (impl != NULL) {
        impl->m_outsideGroundEnergyId = energyId;
    }
}

void wnKineticModuleImpl::addSpeedOutside(soKineticEnergy::OutsideType type, Vec3f* speed) {
    switch (type) {
    case soKineticEnergy::Outside_Movement_Ground:
        if (m_outsideGroundEnergyId != -1) {
            soKineticUtility::addSpeedOutsideMovementGround(getEnergy(m_outsideGroundEnergyId), speed, m_moduleAccesser);
        }
        break;
    }
}

void wnKineticModuleImpl::addSpeed(Vec3f*, soModuleAccesser*) {
}
