// MATCH-ONLY: Vec2f copies are the aggregate word copies of a plain base, assignments out-of-line calls, and the
// Vec2f::length and operator*= copies calls to the weak copies another unit of the REL carries.
#define MT_VEC2F_PLAIN_BASE
#define MT_VEC2F_LENGTH_OUT_OF_LINE
#define MT_VEC2F_MUL_ASSIGN_DIRECT
#define MT_VEC2F_ASSIGN_NOINLINE
#include <em/duon/wnem_proc_duon.h>
#include <em/em_scaled_value.h>
#include <em/em_weapon_manager.h>
#include <em/wnem/wnem_proc_common.h>
#include <ft/ft_external_value_accesser.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_slow.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module.h>
#include <types.h>

// The procs of the Duon's weapons: bombs that fall and explode, missiles that home in on the nearest fighter, and
// lasers. Each explosion (and the laser's end) stops the article, hides it and removes it after a set number of frames.

// HYPOTHESIS: proc numbers: the weapons' procs follow each other from 0x12 (BombFly) to 0x18 (LaserDead); the numbers
// below are those the procs hand to setProcFnc_* / changeProcFnc.
static const s32 Proc_BombExplosion = 0x13;
static const s32 Proc_MissileExplosion = 0x15;
static const s32 Proc_LaserFly = 0x17;
static const s32 Proc_LaserDead = 0x18;
// The team number of an article without a team.
static const int TeamNo_None = 10000;

extern "C" {
// Map name wnemSimple::getValueInt / wnemSimple::getValueFloat.
// The value of a scaled parameter (base + rate * the level scale of the weapon's owner).
int fn_41_33AD0(emScaledValue* value, soModuleAccesser* acc);
float fn_41_33B6C(emScaledValue* value, soModuleAccesser* acc);
}

// The weapons' parameters (wnemSimple::unk2234). HYPOTHESIS: roles from the reads below.
struct wnemDuonParam {
    u32 m_bombLife;          // 0x0, the frames a bomb's explosion lasts
    float m_searchRange;     // 0x4, the missile looks for a fighter within this range
    emScaledValue m_maxSpeed; // 0x8, the missile's top speed
    emScaledValue m_startTurn; // 0x10, the missile's turn value at its start
    u32 m_maxTurnCount;      // 0x18, the missile turns this many frames
    u32 m_missileLife;       // 0x1C, the frames a missile's explosion lasts
    u32 m_homingStartFrame;  // 0x20, the frame the missile starts homing in
    u32 m_laserEnterLife;    // 0x24, the frames before LaserEnter turns into LaserFly
    u32 m_laserDeadLife;     // 0x28, the frames LaserDead lasts
    s32 m_bombAnimCmd;       // 0x2C, the animation command group of the bomb's explosion (0 for none)
    s32 m_missileAnimCmd;    // 0x30, the same for the missile
    s32 m_laserAnimCmd;      // 0x34, the same for the laser
    emScaledValue m_homingFrames; // 0x38, int: the frames the missile homes in for
    emScaledValue m_turnStep; // 0x40
    emScaledValue m_accel;   // 0x48, the missile's acceleration
    emScaledValue m_minSpeed; // 0x50
    float m_turnLimitLow;    // 0x58
    float m_turnLimitHigh;   // 0x5C
    float m_accelScale;      // 0x60
    float m_turnStepB;       // 0x64
};

void wnemDuonProc::BombFly(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wn->setProcFnc_TouchGround(Proc_BombExplosion);
        wn->setProcFnc_LifeZero(Proc_BombExplosion);
        acc->getSituationModule().setKind(Situation_Air, true);
        wn->executeAnimCmd(5u);
        wn->executeAnimCmd(6u);
    }
    if (wn->unk2218) {
        wn->changeProcFnc(Proc_BombExplosion, 1);
    }
}

void wnemDuonProc::BombExplosion(wnemSimple* wn, soModuleAccesser* acc) {
    wnemDuonParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemDuonParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemDuonProc::BombExplosion  \n");
        wn->SetDeadMode();
        wn->executeAnimCmd(0u);
        wn->executeAnimCmd(3u);
        if (param->m_bombAnimCmd != 0) {
            wn->executeAnimCmd(param->m_bombAnimCmd, 4, 1);
        }
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
        wn->setVisibilityWhole(0);
    }
    if (wn->getFrameCounter(1) >= param->m_bombLife) {
        acc->getCollisionAttackModule().clearAll();
        wn->RequestDeactive();
    }
}

// MATCH-ONLY: the original copies the turned direction with the inline scalar copies, while it assigns the article's
// speed and acceleration through the out-of-line operator=.
static inline void copyVec2f(Vec2f& to, const Vec2f& from) {
    to.m_x = from.m_x;
    to.m_y = from.m_y;
}

void wnemDuonProc::MissileFly(wnemSimple* wn, soModuleAccesser* acc) {
    wnemDuonParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemDuonParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemDuonProc::MissileFly  \n");
        wn->setProcFnc_TouchGround(Proc_MissileExplosion);
        wn->setProcFnc_LifeZero(Proc_MissileExplosion);
        acc->getSituationModule().setKind(Situation_Air, true);
        wn->executeAnimCmd(7u);
        wn->executeAnimCmd(8u);
        wn->m_floatParams.at(0) = fn_41_33B6C(&param->m_startTurn, acc);
        wn->m_intParams.at(0) = 0;
        wn->m_intParams.at(0) = 0;
        wn->m_intParams.at(1) = 0;
    }
    if (acc->getTeamModule().getTeam()->getNo() != TeamNo_None) {
        // An article with a team only keeps its direction, at the length of its acceleration.
        float accelLength = wn->m_accel.length();
        Vec2f dir = wn->m_speed;
        dir.normalize();
        dir *= accelLength;
        wn->m_accel = dir;
        return;
    }
    if (wn->unk2218) {
        wn->changeProcFnc(Proc_MissileExplosion, 1);
        return;
    }
    if (soSlow::getInstance()->isOn() == 1) {
        wn->changeProcFnc(Proc_MissileExplosion, 1);
        return;
    }
    u32 homingFrames = fn_41_33AD0(&param->m_homingFrames, acc);
    float turnStep = fn_41_33B6C(&param->m_turnStep, acc);
    float accel = fn_41_33B6C(&param->m_accel, acc);
    float minSpeed = fn_41_33B6C(&param->m_minSpeed, acc);
    float turnLimitLow = param->m_turnLimitLow;
    float turnLimitHigh = param->m_turnLimitHigh;
    float accelScale = param->m_accelScale;
    float turnStepB = param->m_turnStepB;
    float accelScaled = accel * accelScale;
    // MATCH-ONLY: the original also reads the start value here and drops the result.
    fn_41_33B6C(&param->m_startTurn, acc);
    if (wn->getFrameCounter(1) >= homingFrames) {
        // After the homing frames the missile only keeps its direction, at the length of its acceleration.
        float accelLength = wn->m_accel.length();
        Vec2f dir = wn->m_speed;
        dir.normalize();
        dir *= accelLength;
        wn->m_accel = dir;
        return;
    }
    // MATCH-ONLY: declared before the loads so that it takes the original's register; it also carries the flag below.
    s32 turnCount;
    u32 homingStartFrame = param->m_homingStartFrame;
    Vec3f target(0.0f, 0.0f, 0.0f);
    Vec3f pos = acc->getPostureModule().getPos();
    if (wn->getFrameCounter(1) < homingStartFrame) {
        return;
    }
    float searchRange = param->m_searchRange;
    Fighter* fighter = emWeaponManager::SearchNearestFighterPosition(pos.xy(), searchRange);
    if (fighter != NULL) {
        target = ftExternalValueAccesser::getHipPos(fighter);
        target.m_z = 0.0f;
    } else {
        target.m_x = pos.m_x + wn->m_speed.m_x;
        target.m_y = pos.m_y + wn->m_speed.m_y;
    }
    float maxSpeed = fn_41_33B6C(&param->m_maxSpeed, acc);
    float turnAngle = wn->m_floatParams.at(0);
    Vec3f velocity(wn->m_speed.m_x, wn->m_speed.m_y, 0.0f);
    float speed = wn->m_speed.length();
    bool isTurnedOut = false;
    Vec2f dir = wn->m_speed;
    dir.normalize();
    turnCount = wn->m_intParams.at(0);
    if (turnCount < param->m_maxTurnCount) {
        s32& counter = wn->m_intParams.at(0);
        counter = turnCount + 1;
        if (wn->m_intParams.at(1) > 0) {
            isTurnedOut = true;
        }
    } else {
        copyVec2f(dir, wnemCommonProc::HormingTarget1(target, pos, &turnAngle, velocity, &isTurnedOut, turnStep,
                                                      turnLimitLow, turnLimitHigh, turnStepB));
        wn->m_floatParams.at(0) = turnAngle;
        wn->m_intParams.at(0) = 0;
        // HYPOTHESIS: the flag is kept in the second int parameter, as 0 or 1.
        turnCount = isTurnedOut;
        wn->m_intParams.at(1) = turnCount;
    }
    float newSpeed;
    if (!isTurnedOut) {
        newSpeed = speed + accelScaled;
        if (newSpeed >= maxSpeed) {
            newSpeed = maxSpeed;
        }
    } else {
        newSpeed = speed - accel;
        if (newSpeed < minSpeed) {
            newSpeed = minSpeed;
        }
    }
    dir *= newSpeed;
    wn->m_speed = dir;
}

void wnemDuonProc::MissileExplosion(wnemSimple* wn, soModuleAccesser* acc) {
    wnemDuonParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemDuonParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemDuonProc::MissileExplosion  \n");
        wn->SetDeadMode();
        wn->executeAnimCmd(2u);
        if (param->m_missileAnimCmd != 0) {
            wn->executeAnimCmd(param->m_missileAnimCmd, 4, 1);
        }
        wn->executeAnimCmd(3u);
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
        wn->setVisibilityWhole(0);
    }
    if (wn->getFrameCounter(1) >= param->m_missileLife) {
        acc->getCollisionAttackModule().clearAll();
        wn->RequestDeactive();
    }
}

void wnemDuonProc::LaserEnter(wnemSimple* wn) {
    wnemDuonParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemDuonParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemDuonProc::LaserEnter  \n");
        wn->setProcFnc_LifeZero(Proc_LaserDead);
        wn->setProcFnc_HitShell(Proc_LaserDead);
        wn->changeMotion(0, true);
    }
    if (param->m_laserAnimCmd != 0) {
        wn->executeAnimCmd(param->m_laserAnimCmd, 2, 1);
    }
    if (wn->getFrameCounter(1) >= param->m_laserEnterLife) {
        wn->changeProcFnc(Proc_LaserFly, 1);
    }
}

void wnemDuonProc::LaserFly(wnemSimple* wn) {
    wnemDuonParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemDuonParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemDuonProc::LaserFly  \n");
        wn->setProcFnc_HitShell(Proc_LaserDead);
        if (wn->setProcFnc_LifeZero(Proc_LaserDead) == true) {
            wnemCommonProc::Report("call wnemDuonProc::LaserFly ------- setProcFnc_LifeZero EM_WEAPON_PROC_FUNC_ID_DUON_LASER_DEAD is successful \n");
        }
        wn->changeMotion(1, true);
        wn->executeAnimCmd(9u);
    }
    if (param->m_laserAnimCmd != 0) {
        wn->executeAnimCmd(param->m_laserAnimCmd, 2, 1);
    }
}

void wnemDuonProc::LaserDead(wnemSimple* wn, soModuleAccesser* acc) {
    wnemDuonParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemDuonParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemDuonProc::LaserDead  \n");
        wn->SetDeadMode();
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
        wn->setVisibilityWhole(0);
        acc->getCollisionAttackModule().clearAll();
        wnemCommonProc::Report("call wnemDuonProc::LaserDead ------- executeAnimCmd Erase Effect \n");
        wn->executeAnimCmd(1u);
        wn->executeAnimCmd(4u);
    }
    u32 life = param->m_laserDeadLife;
    if (wn->getFrameCounter(1) >= life) {
        wnemCommonProc::Report("call wnemDuonProc::LaserDead -- RequestDeactive -- \n");
        wn->RequestDeactive();
    }
}
