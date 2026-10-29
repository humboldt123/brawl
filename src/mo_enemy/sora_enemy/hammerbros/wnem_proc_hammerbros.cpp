#include <em/em_shot.h>
#include <em/hammerbros/wnem_proc_hammerbros.h>
#include <so/model/so_model_module_simple.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_array.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module.h>
#include <types.h>

// The hammer's proc in flight: it spins the hammer about its y axis each frame, and keeps the article's speed on the
// shot's own value while the hammer belongs to a team.

// HYPOTHESIS: proc 0x32 is the hammer's end-of-life proc; 10000 is the team number of an article without a team.
static const s32 Proc_HammerLifeZero = 0x32;
static const int TeamNo_None = 10000;

void wnemHammerbrosProc::HammerFly(wnemSimple* wn, soModuleAccesser* acc) {
    // MATCH-ONLY: the original .rodata has 0.0f before 90.0f; MWCC pools literals by first use, so mention it first.
    (void)0.0f;
    Vec3f rot = acc->getPostureModule().getRot(1);
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wn->setProcFnc_LifeZero(Proc_HammerLifeZero);
        // The hammer starts tilted a quarter turn.
        rot.m_z -= 90.0f;
    }
    // HYPOTHESIS: int parameter 0 is the model node that spins, float parameter 1 the spin per frame.
    s32 spinNode = wn->m_intParams.at(0);
    rot.m_y += wn->m_floatParams.at(1);
    acc->getPostureModule().setRot(&rot, 1);
    acc->getModelModule().setNodeRotate(spinNode, &rot);
    if (acc->getTeamModule().getTeam()->getNo() != TeamNo_None) {
        emShotParam* param = wn->m_shotWork.m_param;
        if (param != nullptr) {
            // HYPOTHESIS: the hammer's vertical speed (m_flySpeedY) goes into the article's acceleration.
            wn->m_accel.m_x = 0.0f;
            wn->m_accel.m_y = param->m_flySpeedY;
        }
    }
}
