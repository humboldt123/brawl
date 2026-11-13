#pragma once
#include <ft/ft_common_data_accesser.h>
#include <types.h>

// HYPOTHESIS: the Whirling Fortress (Special Hi) extend-parameter group of Bowser and Giga Bowser: the air rise speed,
// gravity and fall limit, the ground/air drift speed and acceleration multipliers, and four frame counts read when the
// air status ends.
struct ftKoopaSpecialHiParam {
    float unk0;
    float m_airSpeedY;
    float m_gravity;
    float m_fallSpeedMax;
    float m_groundSpeedX;
    float m_airSpeedX;
    float m_groundAccelMul;
    float m_airAccelMul;
    int unk20;
    int unk24;
    int unk28;
    int unk2C;
};

// Bowser keeps the group at index 1, Giga Bowser at index 2.
static inline ftKoopaSpecialHiParam* getKoopaSpecialHiParam(ftKind kind, int index) {
    return static_cast<ftKoopaSpecialHiParam*>(g_ftCommonDataAccesser.getData(kind)->extendParam[index]);
}
