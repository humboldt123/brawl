#pragma once

#include <mt/mt_vector.h>
#include <types.h>

class soModuleAccesser;

// Helpers shared by every fighter: status-ID classification and the common parameter multipliers.
// Status IDs are the common fighter action IDs (see the BrawlRE "Actions" list).
class soCollisionLog;
struct soCollisionAttackData;

class ftUtil {
public:
    // Hit by a counter shield (a shield collision whose owner is in a counter-capable status); used by BaseItem.
    static bool isCounterShield(soCollisionLog* collisionLog);

    // HYPOTHESIS: slip chances of a hit (banana-style falls); isSlip handles the ground/air kinds, isSlipDamage the damage variant.
    static bool isSlip(float chance, soModuleAccesser* acc, int kind);
    static bool isSlipDamage(float chance, soModuleAccesser* acc, int kind);
    // Bit 3 of the common status data of the status (HYPOTHESIS: marks the damage statuses).
    static bool isDamageStatus(soModuleAccesser* acc, int status);
    // The point where the fighter's swim segment crosses the water surface line below pos (writes x and the y of pos).
    static bool calcWaterSurfacePos(soModuleAccesser* acc, Vec3f* pos, Vec2f* out);

    // Common statuses 0xA4-0xAE: standing, walking, dashing, jumping, falling, firing and landing while shooting an item.
    static bool isItemShootStatus(soModuleAccesser* acc, int status);

    // Common statuses 0xDF-0xE5: the hammer stand/turn/walk/jump/fall/land states.
    static bool isHammerStatus(soModuleAccesser* acc, int status);
    // HYPOTHESIS: motion IDs 0x141-0x143 are the hammer motions; the status and motion ranges are checked separately.
    static bool isHammerMotion(soModuleAccesser* acc, int motion);

    // Common statuses 0x5C-0x5E: falling asleep, sleeping and waking up.
    static bool isSleepStatus(soModuleAccesser* acc, int status);
    // Common status 0x5F: stunned by a Deku Nut.
    static bool isBindStatus(soModuleAccesser* acc, int status);
    // Common statuses 0xC7-0xC8: buried in the ground.
    static bool isBuryStatus(soModuleAccesser* acc, int status);

    // Speed multipliers. In adventure mode they are scaled up when this fighter is far from player 1's fighter
    // (see calcAdventureMulValue).
    static float getWalkSpeedMul(soModuleAccesser* acc);
    static float getRunSpeedMul(soModuleAccesser* acc);
    static float getJumpSpeedMul(soModuleAccesser* acc);
    // Scales how far a hit moves a fighter by its weight (heavier fighters are knocked back less).
    static float getWeightReactionMul(soModuleAccesser* acc);

    // Linear ramp from 1.0 to a maximum multiplier as the distance to the player 1 fighter grows between a near and a
    // far threshold (three common constant IDs).
    static float calcAdventureMulValue(soModuleAccesser* acc, u32 nearDistanceId, u32 farDistanceId, u32 maxMulId);

    // Motion played while holding an item: -1 when the held item has no special wait motion. Items held by a
    // grip (only King Dedede keeps one) or by a normal/pickup/plate hold use it, except the motion sensor bomb.
    static int getItemWaitMotion(soModuleAccesser* acc);
    // Multiplier used by the item-lift motion rate callback.
    static float getItemLiftMotionRateMul(soModuleAccesser* acc);

    // Whether the fighter is treading on (stomping) the task in its work int; flag 0 is the ground variant.
    static bool checkTread(soModuleAccesser* acc, int flag);

    // Water entry splash (effect, sound and rumble) at the given position.
    static void setWaterDiveEffect(soModuleAccesser* acc, Vec3f* position);

    // Cloud (pass-through platform) handling while airborne; names from the sora_melee map.
    static void setCloudThroughCheck(soModuleAccesser* acc);
    // Touch mask of the fighter's ground shape plus position/normal of the first touched surface.
    static u32 getAirGroundTouchInfo(soModuleAccesser* acc, Vec2f* pos, Vec2f* normal);
    static bool checkCloudThroughOut(soModuleAccesser* acc);
    static int getItemSquatWaitMotion(soModuleAccesser* acc);

    // False while the fighter is moving faster than the common speed limit and its "free speed" flag is off.
    static bool isEnableSpeedOperation(soModuleAccesser* acc);

    // Whether a Zako fighter is allowed to enter the status.
    static bool isValidStatusKindZako(soModuleAccesser* acc, int status);

    // Pushes the fighter out of a wall (parameters as used by fighter statuses, e.g. Sonic's side special wall end).
    // Parameter order is verified from fighter callers and the shared implementation. (Declared by the Sonic status work.)
    static void adjustWall(soModuleAccesser*, float, float, float, int);

    // Ceiling correction restores the supplied facing before adjusting contact.
    // HYPOTHESIS: formal bool/float order is inferred from callers; their register classes are verified.
    static void adjustCeil(soModuleAccesser*, bool, float);

    // Rotates the model node 0x12E around the X axis.
    static void setXAngle(soModuleAccesser* acc, float angle);
    static float setDamageFlyRollAngle(soModuleAccesser* acc);
    static float setAngleWallNormal(soModuleAccesser* acc, float lr);
    static bool isBodyUp(soModuleAccesser* acc);

    // Ice block effect of a frozen fighter; the work int index receives the effect handle.
    static void setIceEffect(soModuleAccesser* acc, u32 workIndex);
    static void setIceExitEffect(soModuleAccesser* acc);

    // Controller rumble. The strength grows with the damage taken / the attack power dealt.
    static void setDamageRumble(soModuleAccesser* acc, float damage);
    static void setAttackRumble(soModuleAccesser* acc, float power);

    // Main stick deflection past a threshold, rescaled to 0..1: out->m_x is the rate and out->m_y the direction
    // (1 or -1; both 0 inside the threshold). HYPOTHESIS: the layout of the result.
    static void getStickXRate(soModuleAccesser* acc, float threshold, Vec2f* out);
    static void getStickYRate(soModuleAccesser* acc, float threshold, Vec2f* out);

    static void setShieldSetoffSpeed(soModuleAccesser* acc, float power, float lr);

    // Scales a value by a factor and exponent (see the definition). Negative exponents invert the scaling.
    static float calcScalingParam(float value, float factor, float exponent);

    // Global multipliers read from the common constants.
    static float getAttackReactionMul(soModuleAccesser* acc);
    // Knockback multiplier for an attack against this fighter: the common constant 0xDB1, damped for fixed-knockback attacks and
    // scaled by the size customizes (param customizes 0 and 1).
    static float getReactionMul(soModuleAccesser* acc, soCollisionAttackData* attackData);
    static float getAttackPowerMul(soModuleAccesser* acc);
    static float getDamageMul(soModuleAccesser* acc);
};
