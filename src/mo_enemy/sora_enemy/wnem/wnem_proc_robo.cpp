// Vec3f and Vec2f helpers the original calls out of line.
#define MT_VEC3F_CTOR_NOINLINE
// MATCH-ONLY: Vec2f copies are the aggregate word copies of a plain base, assignments out-of-line scalar float copies.
#define MT_VEC2F_PLAIN_BASE
#define MT_VEC2F_ASSIGN_NOINLINE
// MATCH-ONLY: the weak out-of-line operator*= scales the components directly.
#define MT_VEC2F_MUL_ASSIGN_DIRECT
#include <em/em_shot.h>
#include <em/em_weapon_manager.h>
#include <em/wnem/wnem_proc_common.h>
#include <em/wnem/wnem_proc_robo.h>
#include <ft/ft_external_value_accesser.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/collision/so_collision_search_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module.h>
#include <types.h>

// The procs of the homing missile of R.O.B.'s enemy. In flight (Fly) a missile that belongs to a team starts to search
// for what it hits and flies on along its own direction; one without a team homes in on the nearest fighter, after a
// delay turning toward it by at most the parameter's angle each frame. It blows up (Bomb) when it hits something, and
// is taken off the field after its explosion (Erase).

// HYPOTHESIS: proc numbers of the missile's touch-ground / hit-shell (0x1A) and end-of-life (0x1B) procs.
static const s32 Proc_MissileBomb = 0x1a;
static const s32 Proc_MissileErase = 0x1b;
// The team number of an article without a team.
static const int TeamNo_None = 10000;

// The missile's parameters (wnemSimple::unk2234). HYPOTHESIS: roles from the reads below.
struct wnemRoboMissileParam {
    float m_searchRange;   // 0x0, the range within which Fly looks for a fighter
    float m_homingSpeed;   // 0x4, the speed of the missile once it homes in
    float m_maxTurn;       // 0x8, the largest turn per frame (degrees)
    u8 unkC[0x4];
    s32 m_eraseLife;       // 0x10, the life limit once Erase starts
    u32 m_bombFrames;      // 0x14, the frames Bomb lasts
    u32 m_homingFrame;     // 0x18, the frames of Fly after which it homes in
    s32 m_bombAnimCmd;     // 0x1C, the anim cmd of the explosion's attack (0 for none)
};

// The collision search data of a missile (soCollisionSearchData, 0x18 bytes). HYPOTHESIS: only the 10-bit field at
// 0x10 is known; Fly sets it to 2 when it starts to search.
struct wnemRoboSearchHead {
    s32 unk0;
    s32 unk4;
};
struct wnemRoboSearchBits {
    unsigned unk0 : 16;
    unsigned m_kind : 10;
    unsigned unk26 : 6;
};
struct wnemRoboSearchTail {
    wnemRoboSearchBits m_bits;
    s32 unk14;
    wnemRoboSearchTail() { }
};
struct wnemRoboSearchData {
    wnemRoboSearchHead m_head;
    s32 unk8;
    float unkC;
    wnemRoboSearchTail m_tail;
    wnemRoboSearchData() { }
};

void wnemRoboProc::MissileFly(wnemSimple* wn, soModuleAccesser* acc) {
    wnemRoboMissileParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemRoboMissileParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemRoboProc::MissileFly  \n");
        wn->setProcFnc_HitShell(Proc_MissileBomb);
        wn->setProcFnc_TouchGround(Proc_MissileBomb);
        wn->setProcFnc_LifeZero(Proc_MissileErase);
        wn->executeAnimCmd(0u);
    }
    if (acc->getTeamModule().getTeam()->getNo() != TeamNo_None) {
        // A missile that belongs to a team searches for what it hits, and keeps its direction at a slow acceleration.
        if (acc->getCollisionSearchModule().isSearch(0)) {
            wnemRoboSearchData data = *reinterpret_cast<wnemRoboSearchData*>(acc->getCollisionSearchModule().getData(0));
            data.m_tail.m_bits.m_kind = 2;
            acc->getCollisionSearchModule().set(0, 0, reinterpret_cast<soCollisionSearchData*>(&data));
        } else {
            wnemCommonProc::Report("[warning!!] wnemRoboProc::MissileFly -- refrect but, isSearch(0) = false \n");
        }
        Vec2f& accel = wn->m_accel;
        accel.m_x = accel.m_y = 0.0f;
        accel = wn->m_speed;
        wn->m_accel *= 0.01f;
        acc->getCollisionAttackModule().clearAll();
    }
    if (wn->unk2218) {
        acc->getCollisionAttackModule().clearAll();
        wn->changeProcFnc(Proc_MissileBomb, 1);
    } else if (acc->getTeamModule().getTeam()->getNo() == TeamNo_None) {
        // Without a team it homes in on the nearest fighter, or flies on, once its delay is over.
        u32 homingFrame = param->m_homingFrame;
        Vec3f targetPos(0.0f, 0.0f, 0.0f);
        Vec3f pos = acc->getPostureModule().getPos();
        if (wn->getFrameCounter(1) >= homingFrame) {
            float range = param->m_searchRange;
            Fighter* fighter = emWeaponManager::SearchNearestFighterPosition(pos.xy(), range);
            if (fighter != NULL) {
                targetPos = ftExternalValueAccesser::getHipPos(fighter);
                targetPos.m_z = 0.0f;
            } else {
                targetPos.m_x = pos.m_x + wn->m_speed.m_x;
                targetPos.m_y = pos.m_y + wn->m_speed.m_y;
            }
            float maxTurn = param->m_maxTurn;
            float homingSpeed = param->m_homingSpeed;
            Vec3f heading(wn->m_speed.m_x, wn->m_speed.m_y, 0.0f);
            Vec2f dir = wnemCommonProc::HormingTarget(targetPos, pos, maxTurn, heading);
            dir *= homingSpeed;
            wn->m_speed = dir;
        }
    }
}

void wnemRoboProc::MissileBomb(wnemSimple* wn, soModuleAccesser* acc) {
    wnemRoboMissileParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemRoboMissileParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemRoboProc::MissileBomb  \n");
        wn->SetDeadMode();
        acc->getCollisionAttackModule().clearAll();
        wn->m_shotWork.unk2C[6] = true;
        wn->executeAnimCmd(1u);
        wn->executeAnimCmd(2u);
        if (param->m_bombAnimCmd != 0) {
            wnemCommonProc::Report("call wnemRoboProc::MissileBomb ------- executeAnimCmd Bomb Attack \n");
            wn->executeAnimCmd(param->m_bombAnimCmd, 4, 1);
        }
        wn->m_speed.m_x = wn->m_speed.m_y = wn->m_accel.m_x = wn->m_accel.m_y = 0.0f;
        wn->setVisibilityWhole(0);
    }
    if (wn->getFrameCounter(1) >= param->m_bombFrames) {
        wnemCommonProc::Report("call wnemRoboProc::MissileBomb ------- TimeOut > Deactive \n");
        acc->getCollisionAttackModule().clearAll();
        wn->RequestDeactive();
    }
}

void wnemRoboProc::MissileErase(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemRoboProc::MissileErase  \n");
        wnemRoboMissileParam* param = NULL;
        if (wn->unk2234 != NULL) {
            param = reinterpret_cast<wnemRoboMissileParam*>(wn->unk2234);
        }
        wn->setVisibilityWhole(0);
        wn->SetDeadMode();
        wn->executeAnimCmd(3u);
        wn->m_speed.m_x = wn->m_speed.m_y = wn->m_accel.m_x = wn->m_accel.m_y = 0.0f;
        acc->getCollisionAttackModule().clearAll();
        wn->SetLifeLimit(param->m_eraseLife);
    }
}
