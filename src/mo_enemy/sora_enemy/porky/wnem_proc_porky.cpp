// MATCH-ONLY: Vec3f assignments stay calls to the weak out-of-line copy, as in the original.
#define MT_VEC3F_ASSIGN_NOINLINE
// MATCH-ONLY: Vec2f copies are the aggregate word copies of a plain base, assignments the scalar float copies.
#define MT_VEC2F_PLAIN_BASE
#include <em/em_scaled_value.h>
#include <em/em_shot.h>
#include <em/em_weapon_manager.h>
#include <em/porky/wnem_proc_porky.h>
#include <em/wnem/wnem_proc_common.h>
#include <ft/ft_external_value_accesser.h>
#include <math.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <types.h>

// The procs of the Porky's weapons. The robot it drops (Robo) falls until it touches the ground (Fall), runs toward the
// nearest fighter (Run) until it hits something (Down: it is knocked back and slows down), and blows up when its life
// runs out (Explosion). The spark (Spark) waits for a while (Lock), then homes in on the nearest fighter (Horming) and
// ends after a set number of frames (Erase).

class Fighter;

extern "C" {
// Map name wnemSimple::getValueFloat: the scaled value for the enemy of the accesser.
float fn_41_33B6C(const emScaledValue* value, soModuleAccesser* acc);
}

// HYPOTHESIS: proc numbers: the Porky's weapons are registered as procs 0x28-0x2E in the order of the map.
static const s32 Proc_RoboRun = 0x29;
static const s32 Proc_RoboDown = 0x2a;
static const s32 Proc_RoboExplosion = 0x2b;
// The distance within which the robot and the spark look for a fighter.
static const float SearchRange_Robo = 300.0f;

// The weapons' parameters (wnemSimple::unk2234); the robot reads the first half, the spark the second.
// HYPOTHESIS: roles from the reads below.
struct wnemPorkyWeaponParam {
    float m_runAccel;          // 0x0, the robot's acceleration while it runs
    float m_runSpeedMax;       // 0x4, the largest speed it runs at
    float m_downSlowdown;      // 0x8, Down multiplies the speed by it every frame
    u32 m_downFrames;          // 0xC, the frames Down lasts
    u32 m_explosionFrames;     // 0x10, the frames Explosion lasts
    s32 m_explosionAnim;       // 0x14, the animation command group started at the explosion (0 for none)
    s32 m_runStartAnim;        // 0x18, the group started when the robot starts to run (0 for none)
    s32 m_runAnim;             // 0x1C, the group started once it faces its direction (0 for none)
    float m_sparkSearchRange;  // 0x20, the distance within which the spark looks for a fighter
    emScaledValue m_sparkSpeedMax; // 0x24, the largest speed of the spark
    emScaledValue m_sparkTurnAngle; // 0x2C, the largest turn toward the target per frame (degrees)
    u8 unk34[0x4];
    u32 m_sparkEraseFrames;    // 0x38, the frames after which the spark ends
    u32 m_sparkLockFrames;     // 0x3C, the frames the spark waits before it homes in
};

void wnemPorkyProc::RoboFall(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemPorkyProc::RoboFall  \n");
        acc->getSituationModule().setKind(Situation_Air, false);
        wn->setProcFnc_TouchGround(Proc_RoboRun);
        wn->setProcFnc_LifeZero(Proc_RoboExplosion);
        wn->setProcFnc_HitShell(Proc_RoboDown);
        wn->changeMotion(1, false);
    }
}

void wnemPorkyProc::RoboRun(wnemSimple* wn, soModuleAccesser* acc) {
    wnemPorkyWeaponParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemPorkyWeaponParam*>(wn->unk2234);
    }
    float accel = param->m_runAccel;
    float speedMax = param->m_runSpeedMax;
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemPorkyProc::RoboRun  \n");
        wn->setProcFnc_LifeZero(Proc_RoboExplosion);
        wn->changeMotion(0, false);
        if (param->m_runStartAnim != 0) {
            wn->executeAnimCmd(param->m_runStartAnim, 4, 1);
        }
        // It runs toward the nearest fighter, or keeps its direction when there is none.
        Vec3f pos = acc->getPostureModule().getPos();
        Fighter* fighter = emWeaponManager::SearchNearestFighterPosition(pos.xy(), SearchRange_Robo);
        if (fighter != NULL) {
            Vec3f target;
            target = ftExternalValueAccesser::getHipPos(fighter);
            wn->m_accel.m_x = wn->m_accel.m_y = wn->m_speed.m_x = wn->m_speed.m_y = 0.0f;
            if (pos.m_x <= target.m_x) {
                wn->m_accel.m_x = accel;
                acc->getPostureModule().setLr(1.0f);
            } else {
                wn->m_accel.m_x = -accel;
                acc->getPostureModule().setLr(-1.0f);
            }
            acc->getPostureModule().updateRotYLr();
        } else {
            wn->m_accel.m_x = wn->m_accel.m_y = wn->m_speed.m_x = wn->m_speed.m_y = 0.0f;
            if (acc->getPostureModule().getLr() == 1.0f) {
                wn->m_accel.m_x = accel;
            } else {
                wn->m_accel.m_x = -accel;
            }
        }
        if (param->m_runAnim != 0) {
            wn->executeAnimCmd(param->m_runAnim, 4, 1);
        }
    }
    if (wn->unk2218) {
        wnemCommonProc::Report("wnemPorkyProc::RoboRun <<< SearchHit >>> go MISSILE_BOMB \n");
        wn->changeProcFnc(Proc_RoboDown, 1);
    } else if (wn->m_speed.m_x >= speedMax) {
        wn->m_speed.m_x = speedMax;
        wn->m_accel.m_x = 0.0f;
    } else if (wn->m_speed.m_x <= -speedMax) {
        wn->m_speed.m_x = -speedMax;
        wn->m_accel.m_x = 0.0f;
    }
}

void wnemPorkyProc::RoboDown(wnemSimple* wn) {
    wnemPorkyWeaponParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemPorkyWeaponParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemPorkyProc::RoboDown  \n");
        wn->SetDeadMode();
        wn->setProcFnc_LifeZero(Proc_RoboExplosion);
        wn->changeMotion(2, false);
        // It is knocked back at no less than a set speed. These are const locals in the original: MWCC folds their
        // uses into the pooled literals, and still emits each one as an unreferenced object at the end of .rodata
        // (0.7, 1.6, 0.0 after the FLT_MIN literal; functions in reverse order, a function's locals in declaration
        // order). HYPOTHESIS: the names, and the zero's place (a const local of RoboFall or RoboRun gives the same bytes).
        const float minSpeed = 0.7f;
        const float knockbackSpeed = 1.6f;
        const float stopAccel = 0.0f;
        if (wn->m_speed.m_x > 0.0f && wn->m_speed.m_x < minSpeed) {
            wn->m_speed.m_x = knockbackSpeed;
        } else if (wn->m_speed.m_x < 0.0f && wn->m_speed.m_x >= -minSpeed) {
            wn->m_speed.m_x = -knockbackSpeed;
        }
        wn->m_accel.m_x = stopAccel;
    }
    if (wn->getFrameCounter(1) >= param->m_downFrames) {
        wn->changeProcFnc(Proc_RoboExplosion, 1);
    }
    float slowdown = param->m_downSlowdown;
    wn->m_speed.m_x = wn->m_speed.m_x * slowdown;
}

void wnemPorkyProc::RoboExplosion(wnemSimple* wn, soModuleAccesser* acc) {
    wnemPorkyWeaponParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemPorkyWeaponParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemPorkyProc::RoboExplosion  \n");
        wn->SetDeadMode();
        wn->executeAnimCmd(1u);
        wn->executeAnimCmd(0u);
        if (param->m_explosionAnim != 0) {
            wn->executeAnimCmd(param->m_explosionAnim, 4, 1);
        }
        // The explosion stands still.
        wn->m_speed.m_x = wn->m_speed.m_y = wn->m_accel.m_x = wn->m_accel.m_y = 0.0f;
        wn->setVisibilityWhole(0);
    }
    if (wn->getFrameCounter(1) >= param->m_explosionFrames) {
        acc->getCollisionAttackModule().clearAll();
        wn->RequestDeactive();
    }
}

void wnemPorkyProc::SparkLock(wnemSimple* wn) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemPorkyProc::SparkLock  \n");
    }
}

void wnemPorkyProc::SparkHorming(wnemSimple* wn, soModuleAccesser* acc) {
    u32 lockFrames;
    float lengthSq;
    wnemPorkyWeaponParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemPorkyWeaponParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemPorkyProc::SparkHorming  \n");
    }
    lockFrames = param->m_sparkLockFrames;
    Vec3f target;
    Vec3f pos = acc->getPostureModule().getPos();
    if (wn->getFrameCounter(1) >= lockFrames) {
        // It homes in on the nearest fighter, or keeps flying the way it flies when there is none.
        float range = param->m_sparkSearchRange;
        Fighter* fighter = emWeaponManager::SearchNearestFighterPosition(pos.xy(), range);
        if (fighter != NULL) {
            target = ftExternalValueAccesser::getHipPos(fighter);
            target.m_z = 0.0f;
        } else {
            target.m_x = pos.m_x + wn->m_speed.m_x;
            target.m_y = pos.m_y + wn->m_speed.m_y;
        }
        float speedMax = fn_41_33B6C(&param->m_sparkSpeedMax, acc);
        float turnAngle = fn_41_33B6C(&param->m_sparkTurnAngle, acc);
        emShotParam* shotParam = wn->m_shotWork.m_param;
        Vec3f velocity;
        velocity.set(wn->m_speed.m_x, wn->m_speed.m_y, 0.0f);
        Vec2f direction = wnemCommonProc::HormingTarget(target, pos, turnAngle, velocity);
        lengthSq = wn->m_speed.m_x * wn->m_speed.m_x + wn->m_speed.m_y * wn->m_speed.m_y;
        float length;
        if (static_cast<float>(__fabs(lengthSq)) <= 1.17549435e-38f) {
            length = 0.0f;
        } else {
            length = lengthSq * rsqrtf(lengthSq);
        }
        float speed = shotParam->m_accel + length;
        if (speed >= speedMax) {
            speed = speedMax;
        }
        direction.m_x = direction.m_x * speed;
        direction.m_y = direction.m_y * speed;
        wn->m_speed.m_x = direction.m_x;
        wn->m_speed.m_y = direction.m_y;
    }
}

void wnemPorkyProc::SparkErase(wnemSimple* wn) {
    wnemPorkyWeaponParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemPorkyWeaponParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemPorkyProc::SparkErase  \n");
        wn->SetDeadMode();
        wn->m_speed.m_x = wn->m_speed.m_y = wn->m_accel.m_x = wn->m_accel.m_y = 0.0f;
        wn->setVisibilityWhole(0);
    }
    if (wn->getFrameCounter(1) >= param->m_sparkEraseFrames) {
        wn->RequestDeactive();
    }
}
