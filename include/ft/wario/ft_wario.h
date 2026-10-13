#pragma once
#include <ft/fighter.h>
#include <so/anim/so_anim_obj.h>
// HYPOTHESIS: grouping follows getExtendParam and rider parameter accesses.
// Named fields: the bike dismount speed and the frames before a new bike can be summoned (offsets and sizes match the
// published Wario bike parameter table; use in special_s / special_s_escape agrees).
struct ftWarioBikeRiderParam {
    u8 unk0[8];
    float unk8;  // HYPOTHESIS: a damage reaction threshold (compared with the damage reaction in checkDamage)
    float unkC;  // HYPOTHESIS: a damage reaction threshold (compared with the damage reaction in checkDamage)
    float escapeSpeedX;
    float escapeSpeedY;
    int remountCooldownFrames;
};
// Corkscrew (up special) parameters. HYPOTHESIS: grouping follows SpecialHiStart/Jump reads of the same record.
struct ftWarioSpecialHiParam {
    float startSpeedXMul;
    float startSpeedYAdd;
    float startGravity;
    float stickThreshold;
    float jumpTiltDegrees;
    float endSpeedX;
    float endSpeedY;
};
// Wario Waft (down special) charge parameters: charge thresholds are in seconds (the status multiplies by 60).
// HYPOTHESIS: the jump speed names follow the interpolation between two charge levels in execStatus.
struct ftWarioSpecialLwParam {
    int chargeSecondsMax;
    int chargeSecondsLevel1;
    int chargeSecondsLevel2;
    float powerMulLevel1;
    float powerMulLevel2;
    float jumpSpeedYLevel2Min;
    float jumpSpeedYLevel2Max;
    float jumpSpeedYLevel3;
};
struct ftWarioExtendParam {
    void* unk0;
    ftWarioBikeRiderParam* bikeRider;
    ftWarioSpecialHiParam* specialHi;
    ftWarioSpecialLwParam* specialLw;
};
// Wario-Man's gauge: HYPOTHESIS: the names follow ftWarioMan::change and notifyEventOnDamage (how long the transformation
// lasts, and what a hit takes off the gauge).
struct ftWarioManParam {
    float durationSeconds; // 0x00 multiplied by 60 into frames
    float unk4;            // 0x04 scales a constant (0xdb2) taken off the duration
    float damageToGauge;   // 0x08 a hit takes this much per percent of damage off the gauge
    float unkC;            // 0x0C the gauge level below which the end warning plays
};
// The parameter accessor of Wario-Man reached through the fighter (its other tables belong to the builder).
struct ftWarioManExtendParamAccesser {
    u8 unk0[0x7C];
    ftWarioExtendParam m_param; // 0x7C
    ftWarioManParam* m_manParam; // 0x8C
};
// Partial fighter declarations: module-builder storage remains unreconstructed.
// Used only for the verified inherited Fighter ABI and getExtendParam calls.
class ftWario : public Fighter {
public:
    ftWarioExtendParam* getExtendParam();
};
class ftWarioMan : public Fighter {
    u8 unk194[0xB24C - sizeof(Fighter)];
public:
    ftWarioManExtendParamAccesser* m_extendParam; // 0xB24C
    soAnimObj m_animObj;                          // 0xB250 the Waft gauge animation

    ftWarioExtendParam* getExtendParam();
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void onStart(int startKind);
    virtual void toDead(Dead::Reason deadReason);
    virtual void onDeadEnd();
    virtual void notifyEventLink(soLinkEventArgs* args, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventOnDamage(soDamage* damage, bool unk1, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual void change(void* unk1, int unk2);
    virtual void setupChangeSucceedWork(void* changeSucceedWork);
    virtual void changeSucceedCore(void* changeSucceedWork, int unk2);
    virtual void processUpdate();
    virtual void processFixPosition();
};
