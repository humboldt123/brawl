#include <so/collision/so_collision_attack_part.h>

void soCollisionAttackPart::reset() {
    m_status = Status_Inactive;
}

void soCollisionAttackPart::resetStatus() {
    m_status = Status_Set;
}

void soCollisionAttackPart::setTargetCategory(soCollision::CategoryMask categoryMask) {
    m_attackData.m_targetCategory = categoryMask;
    m_clTargetArrayVector.at(1).m_4 = categoryMask;
}
