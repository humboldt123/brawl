#include <so/collision/so_collision_shield_group.h>
#include <so/collision/so_collision_shield_part.h>
#include <types.h>

soCollisionShieldGroup::~soCollisionShieldGroup() { }

void soCollisionShieldGroup::init() {
    m_partStart = -1;
    m_partSize = 0;
    unk0 = 0;
    m_hopAngle = 0.0f;
    m_attackMul = 1.0f;
    m_speedMul = 1.0f;
    m_lifeMul = 0.0f;
    m_isFront = false;
    m_isHop = false;
    m_isTurn = false;
    m_isNoMBall = false;
    m_isNoHop = false;
    unkC = 0.0f;
    unk8 = 0.0f;
}

void soCollisionShieldGroup::set(soArray<soCollisionShieldPart>* parts, u32 index, soCollisionShieldData* data) {
    parts->at(m_partStart + index).setData(data);
}

void soCollisionShieldGroup::setReflectorData(const soCollisionReflectorData* data) {
    m_attackMul = data->m_attackMul;
    m_speedMul = data->m_speedMul;
    m_attackLimit = data->m_attackLimit;
    m_lifeMul = data->m_lifeMul;
    m_isNoMBall = data->m_isNoMBall;
}

bool soCollisionShieldGroup::isShield(soArray<soCollisionShieldPart>* parts, u32 index) const {
    return parts->at(m_partStart + index).m_isActive != 0;
}

void soCollisionShieldGroup::setStatus(soArray<soCollisionShieldPart>* parts, u32 index, u32 status) {
    parts->at(m_partStart + index).m_isActive = status;
}

void soCollisionShieldGroup::clearAll(soArray<soCollisionShieldPart>* parts) {
    for (int i = 0; i < m_partSize; i++) {
        parts->at(m_partStart + i).m_isActive = 0;
    }
}

void soCollisionShieldGroup::setTargetProperty(soArray<soCollisionShieldPart>* parts, u32 property) {
    for (int i = 0; i < m_partSize; i++) {
        parts->at(m_partStart + i).setTargetProperty(property);
    }
}
