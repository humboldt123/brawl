#include <em/bombhead/wnem_proc_bombhead.h>
#include <em/em_shot.h>
#include <em/wnem/wnem_proc_common.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/collision/so_collision_search_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module.h>
#include <mt/mt_vector.h>
#include <types.h>

// The procs of the Bombhead's thrown head. In flight (HeadFly) it follows the vertical speed of its shot while it
// belongs to a team, starts its search animation after a number of frames, and explodes (HeadExplosion) when it
// touches something. HeadFly's Report strings were copied from the Robo's missile proc (wnemRoboProc::MissileFly).

// HYPOTHESIS: the head's explosion proc (HeadExplosion): touching the ground or a shell, and hitting the search,
// switch to it. (HeadFly's own proc number is not named here.)
static const s32 Proc_HeadExplosion = 0x30;
// HYPOTHESIS: the team number of an article without a team (as in the Hammer Bro's proc).
static const int TeamNo_None = 10000;

// The head's parameters (wnemSimple::unk2234). HYPOTHESIS: roles from the reads below.
struct wnemBombheadParam {
    u32 m_explosionFrames; // 0x0, how long the explosion lasts (HeadExplosion)
    s32 m_explosionAnimCmd; // 0x4, anim cmd of the explosion (0 for none)
    u32 m_searchFrame;     // 0x8, the frame of the flight on which the search starts (HeadFly)
};

// TODO(layout): so/collision/so_collision_search_part.h declares soCollisionSearchData as 0x18 anonymous bytes. The
// copy HeadFly makes of it uses a float at 0xC and a bitfield at 0x10 (patch in the worker notes).
struct wnemBombheadSearchData {
    Vec3f unk0;   // 0x0, HYPOTHESIS: a Vec3f (the copy is the Vec3f copy); its role is unknown
    float unkC;
    u32 unk10_0 : 16;
    u32 m_kind : 10;
    u32 unk10_26 : 6;
    s32 unk14;
};

void wnemBombheadProc::HeadFly(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemBombheadProc::HeadFly  \n");
        wn->setProcFnc_TouchGround(Proc_HeadExplosion);
        wn->setProcFnc_HitShell(Proc_HeadExplosion);
    }
    wnemBombheadParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemBombheadParam*>(wn->unk2234);
    }
    if (wn->getFrameCounter(1) == param->m_searchFrame) {
        wnemCommonProc::Report("call wnemBombheadProc::HeadFly executeAnimCmd GAME_HEAD_SEARCH \n");
        wn->executeAnimCmd(1u);
    }
    if (acc->getTeamModule().getTeam()->getNo() != TeamNo_None) {
        emShotParam* shot = wn->m_shotWork.m_param;
        if (shot != NULL) {
            wn->m_accel.m_x = 0.0f;
            wn->m_accel.m_y = shot->m_flySpeedY;
        }
        if (acc->getCollisionSearchModule().isSearch(0)) {
            wnemBombheadSearchData data = *reinterpret_cast<wnemBombheadSearchData*>(acc->getCollisionSearchModule().getData(0));
            data.m_kind = 2;
            acc->getCollisionSearchModule().set(0, 0, reinterpret_cast<soCollisionSearchData*>(&data));
            acc->getCollisionAttackModule().clearAll();
        } else {
            wnemCommonProc::Report("[warning!!] wnemRoboProc::MissileFly -- refrect but, isSearch(0) = false \n");
        }
    }
    if (wn->unk2218) {
        wnemCommonProc::Report("call wnemBombheadProc::HeadFly <<< hit search >>> \n");
        acc->getCollisionAttackModule().clearAll();
        wn->changeProcFnc(Proc_HeadExplosion, 1);
    }
}

void wnemBombheadProc::HeadExplosion(wnemSimple* wn, soModuleAccesser* acc) {
    wnemBombheadParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemBombheadParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemBombheadProc::HeadExplosion  \n");
        acc->getCollisionAttackModule().clearAll();
        wn->m_shotWork.unk2C[6] = true;
        wn->unk21B8 = false;
        wn->unk21BB = false;
        wn->unk21B9 = false;
        if (param->m_explosionAnimCmd != 0) {
            wn->executeAnimCmd(param->m_explosionAnimCmd, 4, 1);
        }
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
        wn->setVisibilityWhole(0);
    }
    if (wn->getFrameCounter(1) >= param->m_explosionFrames) {
        acc->getCollisionAttackModule().clearAll();
        wn->RequestDeactive();
    }
}
