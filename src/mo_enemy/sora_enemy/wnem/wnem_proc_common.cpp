// The Vec3f subtraction and assignment stay calls to their weak out-of-line copies, as in the original.
#define MT_VEC3F_SUB_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <em/em_scaled_value.h>
#include <em/em_shot.h>
#include <em/wnem/wnem_proc_common.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module.h>
#include <types.h>
#include <wn/wn_em_simple.h>

// The procs that the enemies' weapons share: the homing steering of a weapon (HormingTarget, HormingTarget1) and the
// shell, a shot that flies until it touches the ground (HitGround), a shell or something else (HitEtc) or its life runs
// out (Erase).

// sora_melee: fn_8003DF50 is Vec3f::rot (this = the vector, then the axis, the angle in radians and the result) and
// fn_8003DF9C the angle between two vectors (radians); both keep their address names (not named in the DOL symbols).
extern "C" {
void fn_8003DF50(Vec3f* vec, Vec3f* axis, float angle, Vec3f* out);
float fn_8003DF9C(Vec3f* a, Vec3f* b);
}

// Whether the animation command `index` of the weapon's resource exists (the weapon's animation command table is
// read at m_shotWork.m_param->0x70). Called out of line, as wnemSimple's method keeps its address name.
extern "C" bool fn_41_31960(wnemSimple* wn, u32 index);

// Map name wnemSimple::getValueInt: the scaled value for the enemy of the accesser, as an integer. It keeps its
// address name (wnemSimple's range; the duon procs call it too).
extern "C" s32 fn_41_33AD0(emScaledValue* value, soModuleAccesser* acc);

// The shell's parameters (wnemSimple::unk2234). HYPOTHESIS: the life is how long the shell stays while it ends.
struct wnemShellParam {
    u8 unk0[0x10];
    emScaledValue m_endFrames;
};

// HYPOTHESIS: proc numbers of the shell, from the registrations: 0x32 is Erase, 0x34 HitGround and 0x35 HitEtc.
static const s32 Proc_ShellErase = 0x32;
static const s32 Proc_ShellHitGround = 0x34;
static const s32 Proc_ShellHitEtc = 0x35;
// The team number of an article without a team.
static const int TeamNo_None = 10000;

// The axis the aim is turned around and the default aim (HYPOTHESIS: unit vectors).
static Vec3f g_turnAxis(0.0f, 0.0f, 1.0f);
static Vec3f g_defaultAim(1.0f, 0.0f, 0.0f);

// Whether a float is a finite number (its exponent is not all ones). An inline function of the original: its weak copy
// (mtIsNormal__Ff) comes right after testNormal, its only caller, and the linker keeps this unit's copy.
inline bool mtIsNormal(float value) {
    union {
        float f;
        u32 u;
    } bits;
    bits.f = value;
    bool result = true;
    if (((bits.u >> 23) & 0xff) == 0xff) {
        result = false;
    }
    return result;
}

inline bool Vec3f::testNormal() {
    bool result = false;
    if (mtIsNormal(m_x) && mtIsNormal(m_y) && mtIsNormal(m_z)) {
        result = true;
    }
    return result;
}

inline void wnemSimple::SetDeadMode() {
    m_shotWork.unk2C[6] = true;
    unk21B8 = false;
    unk21BB = false;
    unk21B9 = false;
}

static inline bool isNearZero(float value) {
    if (value < 0.00001f && value > -0.00001f) {
        return true;
    }
    return false;
}

void wnemCommonProc::Report(const char* fmt, ...) { }

Vec2f wnemCommonProc::HormingTarget(Vec3f& to, Vec3f& from, float maxAngle, Vec3f& delta) {
    Vec3f vAim = to - from;
    Vec3f left;
    Vec3f right;
    fn_8003DF50(&delta, &g_turnAxis, 0.017453292f * -maxAngle, &left);
    fn_8003DF50(&delta, &g_turnAxis, 0.017453292f * maxAngle, &right);
    if (!delta.testNormal()) {
        delta = g_defaultAim;
    }
    if (!vAim.testNormal()) {
        vAim = g_defaultAim;
    }
    Vec2f result;
    float deltaToAim = fn_8003DF9C(&delta, &vAim);
    if (deltaToAim <= fn_8003DF9C(&delta, &right)) {
        result.set(vAim.m_x, vAim.m_y);
    } else {
        float leftToAim = fn_8003DF9C(&left, &vAim);
        if (leftToAim <= fn_8003DF9C(&right, &vAim)) {
            result.set(left.m_x, left.m_y);
        } else {
            result.set(right.m_x, right.m_y);
        }
    }
    result.normalize();
    return result;
}

Vec2f wnemCommonProc::HormingTarget1(Vec3f& to, Vec3f& from, float* angle, Vec3f& delta, bool* reached, float step,
                                     float minAngle, float limit, float stepScale) {
    Vec3f vAim = to - from;
    Vec3f left;
    Vec3f right;
    fn_8003DF50(&delta, &g_turnAxis, 0.017453292f * -*angle, &left);
    fn_8003DF50(&delta, &g_turnAxis, 0.017453292f * *angle, &right);
    // These are const locals in the original: MWCC folds their uses into the pooled literals, and still emits each one
    // as an unreferenced object at the end of .rodata (1.125, 0.65, 0.0 after the 1.0 of the static initializer;
    // functions in reverse order, a function's locals in declaration order). HYPOTHESIS: the names. No code reads a
    // 0.0 here, so the zero is an unused local of this function (or of an earlier one: same bytes).
    const float minTurnAngle = 1.125f;
    const float reachRate = 0.65f;
    const float zero = 0.0f;
    *reached = false;
    Vec2f result;
    if (*angle <= minTurnAngle) {
        *angle = *angle + step * stepScale;
        if (*angle <= limit) {
            *angle = limit;
        }
        result.set(delta.m_x, delta.m_y);
        result.normalize();
        return result;
    }
    if (!delta.testNormal()) {
        wnemCommonProc::Report("wnemCommonProc::HormingTarget : !delta.testNormal() \n");
        delta = g_defaultAim;
    }
    if (!vAim.testNormal()) {
        wnemCommonProc::Report("wnemCommonProc::HormingTarget : !vAim.testNormal()  \n");
        vAim = g_defaultAim;
    }
    float deltaToAim = fn_8003DF9C(&delta, &vAim);
    if (deltaToAim <= fn_8003DF9C(&delta, &right)) {
        result.set(vAim.m_x, vAim.m_y);
        *angle = *angle - step;
        if (*angle <= minAngle) {
            *angle = minAngle;
        }
    } else {
        float leftToAim = fn_8003DF9C(&left, &vAim);
        float rightToAim = fn_8003DF9C(&right, &vAim);
        *angle = *angle + step * stepScale;
        if (*angle <= limit) {
            *angle = limit;
        }
        if (*angle >= reachRate * limit) {
            *reached = true;
        }
        if (leftToAim <= rightToAim) {
            result.set(left.m_x, left.m_y);
        } else {
            result.set(right.m_x, right.m_y);
        }
    }
    result.normalize();
    return result;
}

void wnemCommonProc::ShellFly(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemCommonProc::ShellFly  \n");
        wn->setProcFnc_LifeZero(Proc_ShellErase);
        if (fn_41_31960(wn, 1)) {
            wn->setProcFnc_TouchGround(Proc_ShellHitGround);
        }
        if (fn_41_31960(wn, 7)) {
            wn->setProcFnc_HitShell(Proc_ShellHitEtc);
        }
        if (fn_41_31960(wn, 4)) {
            wn->executeAnimCmd(4u);
            wnemCommonProc::Report("call wnemCommonProc::ShellFly : exec EM_COMMON_WEAPON_ANIMCMD_EFX_SHELL_FLY \n");
        }
        if (fn_41_31960(wn, 5)) {
            wn->executeAnimCmd(5u);
        }
    }
    if (acc->getTeamModule().getTeam()->getNo() != TeamNo_None) {
        emShotParam* shotParam = wn->m_shotWork.m_param;
        if (shotParam != NULL) {
            float flySpeedY = shotParam->m_flySpeedY;
            if (isNearZero(flySpeedY)) {
                wn->m_accel.m_x = 0.0f;
                wn->m_accel.m_y = shotParam->m_flySpeedY;
            }
        }
    }
}

// The shell stops and disappears: it plays its end animation commands (which exist only for some shells) and, when it
// has no end frames (or they have passed), is taken off the field.
// The end-frames flag is set through a named local (here and in HitEtc / HitGround): MWCC turns
// `param != NULL && value != 0`, and the same test as nested ifs without the local, into one && expression whose
// temporary takes r31 and pushes the parameter block pointer to r30; the original keeps the pointer in r31.
void wnemCommonProc::ShellErase(wnemSimple* wn, soModuleAccesser* acc) {
    wnemShellParam* param = reinterpret_cast<wnemShellParam*>(wn->unk2234);
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemCommonProc::ShellErase  \n");
        wn->SetDeadMode();
        acc->getCollisionAttackModule().clearAll();
        wn->executeAnimCmd(0u);
        if (fn_41_31960(wn, 2)) {
            wn->executeAnimCmd(2u);
        }
        if (fn_41_31960(wn, 6)) {
            wn->executeAnimCmd(6u);
        }
        if (fn_41_31960(wn, 3)) {
            wn->executeAnimCmd(3u);
        }
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
        wn->setVisibilityWhole(0);
    }
    bool hasEndFrames = false;
    if (param != NULL) {
        s32 frames = fn_41_33AD0(&param->m_endFrames, acc);
        if (frames != 0) {
            hasEndFrames = true;
        }
    }
    if (hasEndFrames) {
        s32 endFrames = fn_41_33AD0(&param->m_endFrames, acc);
        if (wn->getFrameCounter(1) >= endFrames) {
            acc->getCollisionAttackModule().clearAll();
            wn->RequestDeactive();
        }
    } else {
        acc->getCollisionAttackModule().clearAll();
        wn->RequestDeactive();
    }
}

void wnemCommonProc::ShellHitEtc(wnemSimple* wn, soModuleAccesser* acc) {
    wnemShellParam* param = reinterpret_cast<wnemShellParam*>(wn->unk2234);
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemCommonProc::ShellHitEtc  \n");
        wn->SetDeadMode();
        acc->getCollisionAttackModule().clearAll();
        if (fn_41_31960(wn, 7)) {
            wn->executeAnimCmd(7u);
        }
        if (fn_41_31960(wn, 8)) {
            wn->executeAnimCmd(8u);
        }
        if (fn_41_31960(wn, 6)) {
            wn->executeAnimCmd(6u);
        }
        if (fn_41_31960(wn, 3)) {
            wn->executeAnimCmd(3u);
        }
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
        wn->setVisibilityWhole(0);
    }
    bool hasEndFrames = false;
    if (param != NULL) {
        s32 frames = fn_41_33AD0(&param->m_endFrames, acc);
        if (frames != 0) {
            hasEndFrames = true;
        }
    }
    if (hasEndFrames) {
        s32 endFrames = fn_41_33AD0(&param->m_endFrames, acc);
        if (wn->getFrameCounter(1) >= endFrames) {
            acc->getCollisionAttackModule().clearAll();
            wn->RequestDeactive();
        }
    } else {
        acc->getCollisionAttackModule().clearAll();
        wn->RequestDeactive();
    }
}

void wnemCommonProc::ShellHitGround(wnemSimple* wn, soModuleAccesser* acc) {
    wnemShellParam* param = reinterpret_cast<wnemShellParam*>(wn->unk2234);
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemCommonProc::ShellHitGround  \n");
        wn->SetDeadMode();
        acc->getCollisionAttackModule().clearAll();
        if (fn_41_31960(wn, 1)) {
            wn->executeAnimCmd(1u);
        }
        if (fn_41_31960(wn, 8)) {
            wn->executeAnimCmd(8u);
        }
        if (fn_41_31960(wn, 3)) {
            wn->executeAnimCmd(3u);
        }
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
        wn->setVisibilityWhole(0);
    }
    bool hasEndFrames = false;
    if (param != NULL) {
        s32 frames = fn_41_33AD0(&param->m_endFrames, acc);
        if (frames != 0) {
            hasEndFrames = true;
        }
    }
    if (hasEndFrames) {
        s32 endFrames = fn_41_33AD0(&param->m_endFrames, acc);
        if (wn->getFrameCounter(1) >= endFrames) {
            acc->getCollisionAttackModule().clearAll();
            wn->RequestDeactive();
        }
    } else {
        acc->getCollisionAttackModule().clearAll();
        wn->RequestDeactive();
    }
}
