#define WN_ARTICLE_NATIVE_BUILDERS
#include <mt/mt_vector.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <so/event/so_event_system.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/wn_kinetic_module_impl.h>
#include <types.h>

// Kinetic classes of the generic article (Weapon): the planar energy the shots fly with, the vertical-only energy of
// the falling articles, and the module that owns the two energies and switches between their modes.

wnKineticEnergyNormal::wnKineticEnergyNormal() : unk34(-1) {
}

wnKineticEnergyNormal::~wnKineticEnergyNormal() {
}

void wnKineticEnergyNormal::resetEnergy(int mode, Vec2f* speed, Vec3f*, soModuleAccesser*) {
    float y = speed->m_y;
    float x = speed->m_x;
    m_accel = Vec2f(0.0f, 0.0f);
    m_speedTarget = Vec2f(-1.0f, -1.0f);
    m_brake = Vec2f(0.0f, 0.0f);
    m_speedLimit = Vec2f(-1.0f, -1.0f);
    m_speed.m_x = x;
    m_speed.m_y = y;
    unk34 = mode;
    switch (mode) {
    case 0:
        break;
    case 1:
        m_speedTarget = Vec2f(0.0f, 0.0f);
        break;
    }
}

void wnKineticEnergyNormal::updateEnergy(soModuleAccesser* acc) {
    switch (unk34) {
    case 0:
        break;
    case 1: {
        float speedLen = getSpeed().length();
        if (speedLen <= 0.00001f) {
            m_accel = Vec2f(0.0f, 0.0f);
        } else if (speedLen > 0.00001f) {
            float rest = speedLen - m_brake.length();
            if (rest < 0.0f) {
                m_accel = Vec2f(0.0f, 0.0f);
            } else {
                m_accel = getSpeed() * (rest / speedLen - 1.0f);
            }
        }
        break;
    }
    }
    soKineticEnergyNormal::updateEnergy(acc);
}

wnKineticModuleImpl::wnKineticModuleImpl(soModuleAccesser* acc, soInstanceManagerFullProperty<soKineticEnergy*>* energies, soKineticMediator* mediator) :
    soKineticModuleImpl(acc, energies, mediator),
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
        Vec2f speed;
        Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        Vec3f zero3(0.0f, 0.0f, 0.0f);
        main->resetEnergy(0, &speed, &zero3, acc);
        Vec3f zeroRot(0.0f, 0.0f, 0.0f);
        Vec2f zero(0.0f, 0.0f);
        sub->resetEnergy(-1, &zero, &zeroRot, acc);
    }
    main->enable();
    sub->enable();
}

void wnKineticModuleImpl::initializeStop(int kineticType, soModuleAccesser* acc) {
    soInstanceManagerFullProperty<soKineticEnergy*>* manager = m_kineticEnergyInstanceManager;
    soKineticEnergy* main = manager->at(0);
    soKineticEnergy* sub = manager->at(1);
    if (kineticType == 0) {
        Vec2f speed;
        Vec2f::copy(speed, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        Vec3f zero3(0.0f, 0.0f, 0.0f);
        main->resetEnergy(1, &speed, &zero3, acc);
        Vec3f zeroRot(0.0f, 0.0f, 0.0f);
        Vec2f zero(0.0f, 0.0f);
        sub->resetEnergy(-1, &zero, &zeroRot, acc);
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

void wnKineticEnergyGravity::resetEnergy(int, Vec2f* speed, Vec3f*, soModuleAccesser*) {
    m_speedY = speed->m_y;
}

void wnKineticEnergyGravity::updateEnergy(soModuleAccesser*) {
    m_speedY += m_gravity;
    if (m_speedY < 0.0f && m_speedLimit > 0.0f) {
        double limitAbs = __fabs(m_speedLimit);
        double speedAbs = __fabs(m_speedY);
        if ((float)speedAbs > (float)limitAbs) {
            m_speedY = soKineticUtility::limitSpeed(m_speedY, m_speedLimit);
        }
    }
}

// Reflects v about the normal n (out = v - 2 n (n . v)).
static inline void reflectVec(Vec2f& out, const Vec2f& v, const Vec2f& n) {
    out = v - n * 2.0f * (n.m_x * v.m_x + n.m_y * v.m_y);
}

void wnKineticEnergyGravity::reflectSpeed(Vec3f* normal) {
    Vec2f v(0.0f, m_speedY);
    Vec2f n(*normal);
    reflectVec(v, v, n);
    m_speedY = v.m_y;
}

void wnKineticEnergyGravity::reflectAccel(Vec3f* normal) {
    Vec2f v(0.0f, m_gravity);
    Vec2f n(*normal);
    reflectVec(v, v, n);
    m_gravity = v.m_y;
}

void wnKineticEnergyGravity::mulAccel(Vec3f* mul) {
    m_gravity *= mul->m_y;
}

void wnKineticEnergyGravity::mulSpeed(Vec3f* mul) {
    m_speedY *= mul->m_y;
}

void wnKineticEnergyGravity::clearRotSpeed() {
}

void wnKineticEnergyGravity::clearSpeed() {
    m_speedY = 0.0f;
}

Vec3f wnKineticEnergyGravity::getRotation() {
    return Vec3f(0.0f, 0.0f, 0.0f);
}

Vec2f wnKineticEnergyGravity::getSpeed() {
    return Vec2f(0.0f, m_speedY);
}

wnKineticEnergyGravity::~wnKineticEnergyGravity() {
}
