#pragma once

// The static data of Snake's Cypher article, shared by the article and Snake's Cypher Hang status.

#include <types.h>

// The parameter block of the article (reached through the data block at +0x34).
// HYPOTHESIS: meaning of the fields.
struct wnSnakeCypherParam {
    // Read by Snake's Cypher Hang status (ftSnakeStatusUniqProcessCypherHang::initStatus).
    float m_hangFallSpeedLimit; // +0: the entry fall speed is clamped to -this
    float m_hangGravity;        // +4: ftKineticEnergyGravity::m_gravity
    float m_hangFallSpeedMax;   // +8: ftKineticEnergyGravity::m_fallSpeedMax
    float m_hangGravityUnk1C;   // +0xC: ftKineticEnergyGravity::unk1C
    float m_hangAirAccelMul;    // +0x10: ftKineticEnergyController::m_accelMul
    float m_hangAirSpeedTarget; // +0x14: ftKineticEnergyController::m_speedTarget.x
    float m_hangAirSpeedLimit;  // +0x18: ftKineticEnergyController::m_speedLimit.x
    float unk1C; // life base
    float unk20; // life divisor input
    float unk24; // vertical speed
    float unk28; // vertical acceleration
    float unk2C; // vertical speed target
    s32 unk30;   // value of the shoot work int
    float unk34; // ground collision offset
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnSnakeCypherData {
    s32 m_resGroupNo;
    s32 m_constraintNode; // +4: node synced with the constraint on activation
    u8 unk8[0x2C];
    const wnSnakeCypherParam* m_param; // +0x34
    u8 unk38[1]; // start of the collision hit data set handed to the hit module
};
