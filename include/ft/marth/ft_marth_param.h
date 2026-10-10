#pragma once

// Ids accepted by soValueAccesser::getConstantFloat / getConstantInt for Marth: 4000 + index into the
// float table and 24000 + index into the int table built by ftMarthExtendParamAccesser::setup().
struct ftMarthParam {
    enum Float {
        SpecialN_AirSpeedXRatio = 4000,
        SpecialN_AirBrakeX = 4001,
        SpecialS_AirSpeedXRatio = 4002,
        SpecialS_RiseSpeedY = 4004,
        SpecialHi_ControlXRatio = 4007,
        SpecialHi_ExitValue = 4008,
        SpecialHi_StickThreshold = 4010,
        SpecialHi_AngleScale = 4011,
        SpecialHi_AirSpeedXRatio = 4012,
        SpecialHi_MotionUnk40 = 4013,
        SpecialHi_Gravity = 4014,
        SpecialHi_FallSpeedMax = 4015,
        SpecialLw_AirSpeedXRatio = 4016,
        SpecialLw_AirBrakeX = 4017,
        SpecialLw_Gravity = 4018,
        SpecialLw_FallSpeedMax = 4019,
        SpecialLw_PowerRatio = 4020,
        SpecialLw_PowerRatioAlt = 4021,
        SpecialLw_PowerMin = 4022,
        SpecialLw_PowerMax = 4023,
        SpecialLw_PowerMaxAlt = 4024,
        Final_SpeedX = 4029,
        Final_WindowStep = 4030,
    };
    enum Int {
        SpecialN_ChargeTime = 24000,
        SpecialN_BaseDamage = 24001,
        SpecialN_DamagePerSecond = 24002,
        SpecialLw_HitStopFrame = 24003,
        Final_WindowMoveFrames = 24006,
        Final_WindowShowFrames = 24007,
    };
};
