#include <st_greenhill/gr_greenhill.h>

// MATCH-ONLY: keep the stage's original constant pool and node name strings.
extern const float g_greenhillCheckConstants[]; // 0.0f, 20.0f, ... 5.0f at [9], 1.0f at [11]
extern const char g_greenhillCheckBallNode[];   // "ball"

// The ball hurts fighters it touches: one hitbox centred on the "ball" node (z mirrored), enabled once.
void grGreenhillCheck::setAttack() {
    const float* constants = g_greenhillCheckConstants;
    if (m_attackEnabled == 1) {
        return;
    }

    soCollisionAttackData attack(constants[11]);
    u32 nodeIndex = getNodeIndex(0, g_greenhillCheckBallNode);
    Vec3f offset;
    getNodePosition(&offset, 0, nodeIndex);
    offset = Vec3f(constants[0], constants[9], -offset.m_z);

    setAttackGimmickDetails(&attack, constants[9], constants[11], constants[11], constants[11],
        10, &offset, 361, 70, 0, 70, nodeIndex,
        0x3FF, 7, false, 15,
        soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Medium,
        soCollisionAttackData::Sound_Attribute_Punch,
        false, false, false, true, false, false, 0, 60,
        false, false, false, soCollisionAttackData::Lr_Check_Forward,
        false, false, false, false, false, soCollisionAttackData::Region_None, true);
    m_yakumono->setAttack(0, 0, &attack);
    m_attackEnabled = 1;
}
