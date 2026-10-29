#include <em/crazyhand/wnem_proc_crazyhand.h>
#include <em/wnem/wnem_proc_common.h>
#include <types.h>

// The procs of the Crazy Hand's bomb. In flight (BombFly) it plays its animation command and turns into the
// explosion (BombExplosion) when it touches the ground, a shell, or hits the search. The explosion stops it, hides it
// and ends it after a set number of frames.

// HYPOTHESIS: proc 0x38 is the bomb's explosion proc (BombExplosion), registered right after BombFly.
static const s32 Proc_BombExplosion = 0x38;

// The bomb's parameters (wnemSimple::unk2234). HYPOTHESIS: roles from the reads below.
struct wnemCrazyhandBombParam {
    u32 m_explosionFrames; // 0x0, how long the explosion lasts
    s32 m_explosionAnimCmd; // 0x4, anim cmd of the explosion (0 for none)
};

void wnemCrazyhandProc::BombFly(wnemSimple* wn) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wn->setProcFnc_TouchGround(Proc_BombExplosion);
        wn->setProcFnc_HitShell(Proc_BombExplosion);
    }
    wn->executeAnimCmd(2u);
    if (wn->unk2218) {
        wn->changeProcFnc(Proc_BombExplosion, 1);
    }
}

void wnemCrazyhandProc::BombExplosion(wnemSimple* wn) {
    wnemCrazyhandBombParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemCrazyhandBombParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemCrazyhandProc::BombExplosion  \n");
        wn->m_shotWork.unk2C[6] = true;
        wn->unk21B8 = false;
        wn->unk21BB = false;
        wn->unk21B9 = false;
        wn->setVisibilityWhole(0);
        if (param->m_explosionAnimCmd != 0) {
            wn->executeAnimCmd(param->m_explosionAnimCmd, 4, 1);
        }
        wn->executeAnimCmd(1u);
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
    }
    if (wn->getFrameCounter(1) >= param->m_explosionFrames) {
        wn->RequestDeactive();
    }
}
