#pragma once
#include <ft/ft_common_data_accesser.h>
#include <types.h>

// HYPOTHESIS: the Bowser Bomb (Special Lw) extend-parameter group of Bowser and Giga Bowser: the multipliers applied to
// the speed carried into the air start, the brake, gravity and fall limit of the rise, and the drop speed.
struct ftKoopaSpecialLwParam {
    float m_airSpeedXMul;
    float m_airSpeedYMul;
    float m_brakeX;
    float m_gravity;
    float m_fallSpeedMax;
    float m_dropSpeedY;
};

// Bowser keeps the group at index 2, Giga Bowser at index 3.
static inline ftKoopaSpecialLwParam* getKoopaSpecialLwParam(ftKind kind, int index) {
    return static_cast<ftKoopaSpecialLwParam*>(g_ftCommonDataAccesser.getData(kind)->extendParam[index]);
}
