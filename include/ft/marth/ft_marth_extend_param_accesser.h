#pragma once

#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <ft/ft_extend_param_accesser.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/marth/ft_marth_param.h>
#include <types.h>

// Marth's six ExtendParam groups, one per ftData::extendParam[] slot. The groups are named after the
// status processes that read them; field names marked HYPOTHESIS come from how the values are used.

// Shield Breaker. Field meanings follow the Kirby hat table for Marth's Shield Breaker in BrawlRE.
struct ftMarthExtendParamSpecialN {
    int chargeTime;       // seconds; the loop status runs chargeTime * 30 frames
    int baseDamage;
    int damagePerSecond;  // added once per full 30 frames of charge
    float airSpeedXRatio; // HYPOTHESIS: horizontal momentum kept when started in the air
    float airBrakeX;      // HYPOTHESIS: horizontal deceleration while charging in the air
};

// Dancing Blade.
struct ftMarthExtendParamSpecialS {
    float airSpeedXRatio; // HYPOTHESIS
    float unk4;
    float riseSpeedY;     // HYPOTHESIS: vertical speed on the first grounded-to-air entry
    float unkC;
    float unk10;
};

// Dolphin Slash.
struct ftMarthExtendParamSpecialHi {
    float controlXRatio;  // HYPOTHESIS: scale applied to the controller energy after the launch
    float unk4;
    float unk8;
    float stickThreshold; // HYPOTHESIS
    float angleScale;     // HYPOTHESIS: degrees of steering at full stick
    float airSpeedXRatio; // HYPOTHESIS
    float unk18;
    float gravity;        // HYPOTHESIS: stored negated by the status
    float fallSpeedMax;   // HYPOTHESIS
};

// Counter.
struct ftMarthExtendParamSpecialLw {
    float airSpeedXRatio; // HYPOTHESIS
    float airBrakeX;      // HYPOTHESIS
    float gravity;        // HYPOTHESIS: stored negated by the status
    float fallSpeedMax;   // HYPOTHESIS
    float powerRatio;     // HYPOTHESIS: scales the countered hit's power
    float powerRatioAlt;  // HYPOTHESIS: replaces powerRatio when the ftManager mode is 1
    int hitStopFrame;
    float powerMin;       // HYPOTHESIS
    float powerMax;       // HYPOTHESIS
    float powerMaxAlt;    // HYPOTHESIS: replaces powerMax when the ftManager mode is 1
};

// The Counter shield, passed to soCollisionShieldModule::add by the constructor.
struct ftMarthExtendParamSpecialLwShield {
    int nodeId;
    float offsetX;
    float offsetY;
    float offsetZ;
    float radius;
};

// Final Smash.
struct ftMarthExtendParamFinal {
    float speedX;         // HYPOTHESIS: forward speed (multiplied by lr)
    int unk4;
    float windowStep;     // HYPOTHESIS: distance an HP window moves per frame
    int windowMoveFrames; // HYPOTHESIS
    int windowShowFrames; // HYPOTHESIS
};

// MATCH-ONLY: the first read of each group goes through the non-const ftData (the *Mutable accessors). A
// const read lets MWCC reuse one pointer load for the whole group and the setup() code no longer matches.
// TODO: UBFIX endianness

class ftMarthExtendParamAccesser : public ftExtendParamAccesserEx<3999, 31, 23999, 8> {
    // The setup argument is the fighter's ftData; its extendParam[] slots hold the six parameter groups.
    const ftData* data(const u8* base) { return reinterpret_cast<const ftData*>(base); }
    ftData* dataMutable(const u8* base) { return const_cast<ftData*>(reinterpret_cast<const ftData*>(base)); }

    const ftMarthExtendParamSpecialN* specialN(const u8* base) { return static_cast<const ftMarthExtendParamSpecialN*>(data(base)->extendParam[0]); }
    const ftMarthExtendParamSpecialN* specialNMutable(const u8* base) { return static_cast<const ftMarthExtendParamSpecialN*>(dataMutable(base)->extendParam[0]); }
    const ftMarthExtendParamSpecialS* specialS(const u8* base) { return static_cast<const ftMarthExtendParamSpecialS*>(data(base)->extendParam[1]); }
    const ftMarthExtendParamSpecialS* specialSMutable(const u8* base) { return static_cast<const ftMarthExtendParamSpecialS*>(dataMutable(base)->extendParam[1]); }
    const ftMarthExtendParamSpecialHi* specialHi(const u8* base) { return static_cast<const ftMarthExtendParamSpecialHi*>(data(base)->extendParam[2]); }
    const ftMarthExtendParamSpecialHi* specialHiMutable(const u8* base) { return static_cast<const ftMarthExtendParamSpecialHi*>(dataMutable(base)->extendParam[2]); }
    const ftMarthExtendParamSpecialLw* specialLw(const u8* base) { return static_cast<const ftMarthExtendParamSpecialLw*>(data(base)->extendParam[3]); }
    const ftMarthExtendParamSpecialLw* specialLwMutable(const u8* base) { return static_cast<const ftMarthExtendParamSpecialLw*>(dataMutable(base)->extendParam[3]); }
    const ftMarthExtendParamSpecialLwShield* specialLwShield(const u8* base) { return static_cast<const ftMarthExtendParamSpecialLwShield*>(data(base)->extendParam[4]); }
    const ftMarthExtendParamSpecialLwShield* specialLwShieldMutable(const u8* base) { return static_cast<const ftMarthExtendParamSpecialLwShield*>(dataMutable(base)->extendParam[4]); }
    const ftMarthExtendParamFinal* final(const u8* base) { return static_cast<const ftMarthExtendParamFinal*>(data(base)->extendParam[5]); }
    const ftMarthExtendParamFinal* finalMutable(const u8* base) { return static_cast<const ftMarthExtendParamFinal*>(dataMutable(base)->extendParam[5]); }

public:
    ftMarthExtendParamAccesser() : ftExtendParamAccesserEx(Fighter_Marth) { }
    virtual ~ftMarthExtendParamAccesser() { }

    virtual void setup(const u8* extData) {
        for (s32 i = 0; i < NumVariations; i++) {
            m_floats[i][0] = &specialN(extData)->airSpeedXRatio;
            m_floats[i][1] = &specialN(extData)->airBrakeX;

            m_floats[i][2] = &specialSMutable(extData)->airSpeedXRatio;
            m_floats[i][3] = &specialS(extData)->unk4;
            m_floats[i][4] = &specialS(extData)->riseSpeedY;
            m_floats[i][5] = &specialS(extData)->unkC;
            m_floats[i][6] = &specialS(extData)->unk10;

            m_floats[i][7] = &specialHiMutable(extData)->controlXRatio;
            m_floats[i][8] = &specialHi(extData)->unk4;
            m_floats[i][9] = &specialHi(extData)->unk8;
            m_floats[i][10] = &specialHi(extData)->stickThreshold;
            m_floats[i][11] = &specialHi(extData)->angleScale;
            m_floats[i][12] = &specialHi(extData)->airSpeedXRatio;
            m_floats[i][13] = &specialHi(extData)->unk18;
            m_floats[i][14] = &specialHi(extData)->gravity;
            m_floats[i][15] = &specialHi(extData)->fallSpeedMax;

            m_floats[i][16] = &specialLwMutable(extData)->airSpeedXRatio;
            m_floats[i][17] = &specialLw(extData)->airBrakeX;
            m_floats[i][18] = &specialLw(extData)->gravity;
            m_floats[i][19] = &specialLw(extData)->fallSpeedMax;
            m_floats[i][20] = &specialLw(extData)->powerRatio;
            m_floats[i][21] = &specialLw(extData)->powerRatioAlt;
            m_floats[i][22] = &specialLw(extData)->powerMin;
            m_floats[i][23] = &specialLw(extData)->powerMax;
            m_floats[i][24] = &specialLw(extData)->powerMaxAlt;

            m_floats[i][25] = &specialLwShield(extData)->offsetX;
            m_floats[i][26] = &specialLwShield(extData)->offsetY;
            m_floats[i][27] = &specialLwShield(extData)->offsetZ;
            m_floats[i][28] = &specialLwShield(extData)->radius;

            m_floats[i][29] = &finalMutable(extData)->speedX;
            m_floats[i][30] = &final(extData)->windowStep;

            m_ints[i][0] = &specialNMutable(extData)->chargeTime;
            m_ints[i][1] = &specialN(extData)->baseDamage;
            m_ints[i][2] = &specialN(extData)->damagePerSecond;
            m_ints[i][3] = &specialLw(extData)->hitStopFrame;
            m_ints[i][4] = &specialLwShieldMutable(extData)->nodeId;
            m_ints[i][5] = &final(extData)->unk4;
            m_ints[i][6] = &final(extData)->windowMoveFrames;
            m_ints[i][7] = &final(extData)->windowShowFrames;
        }
    }
};

extern ftMarthExtendParamAccesser g_ftMarthExtendParamAccesser;
