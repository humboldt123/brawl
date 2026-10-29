#include <em/galleom/wnem_proc_galleom.h>
#include <em/wnem/wnem_proc_common.h>
#include <so/so_slow.h>
#include <types.h>

// The procs of the Galleom's grenade and shot. Both fly until they touch the ground (or, for the shot, hit a shell),
// then change to an explosion that stops them, plays an effect and ends after a set number of frames. Both also move to
// the touch-ground proc as soon as the global slow motion is on.

// HYPOTHESIS: proc 0x3A is the grenade's touch-ground proc (GrenadeExplosion), 0x3C the shot's (ShotExplosion).
static const s32 Proc_GrenadeExplosion = 0x3a;
static const s32 Proc_ShotExplosion = 0x3c;

extern "C" {
// Map name wnemSimple::resetAnimCmd.
// HYPOTHESIS: rewinds the animation command interpreter of the weapon before it runs its first commands.
void fn_41_31708(wnemSimple* wn);
}

// The explosion parameters (wnemSimple::unk2234). HYPOTHESIS: roles from the reads below; the grenade's explosion
// reads the first pair, the shot's the second.
struct wnemGalleomExplosionParam {
    u32 m_grenadeLife;   // 0x0, the frames the grenade's explosion lasts
    s32 m_grenadeAnim;   // 0x4, the animation command group started at the explosion (0 for none)
    u32 m_shotLife;      // 0x8, the frames the shot's explosion lasts
    s32 m_shotAnim;      // 0xC, the animation command group started at the explosion (0 for none)
};

void wnemGalleomProc::GrenadeFly(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemGalleomProc::GrenadeFly  \n");
        wn->setProcFnc_TouchGround(Proc_GrenadeExplosion);
        fn_41_31708(wn);
        wn->executeAnimCmd(1u);
        wn->executeAnimCmd(2u);
    }
    wnemCommonProc::Report("wnemGalleomProc::GrenadeFly (frame:%4d) : call effect \n", wn->getFrameCounter(1));
    if (soSlow::getInstance()->isOn() == 1) {
        wn->changeProcFnc(Proc_GrenadeExplosion, 1);
    }
}

void wnemGalleomProc::GrenadeExplosion(wnemSimple* wn, soModuleAccesser* acc) {
    wnemGalleomExplosionParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemGalleomExplosionParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wn->SetDeadMode();
        wn->setVisibilityWhole(0);
        if (param->m_grenadeAnim != 0) {
            wn->executeAnimCmd(param->m_grenadeAnim, 4, 1);
        }
        wn->executeAnimCmd(0u);
        // The explosion stands still.
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
    }
    if (wn->getFrameCounter(1) >= param->m_grenadeLife) {
        wn->RequestDeactive();
    }
}

void wnemGalleomProc::ShotFly(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemGalleomProc::ShotFly  \n");
        wn->setProcFnc_TouchGround(Proc_ShotExplosion);
        wn->setProcFnc_HitShell(Proc_ShotExplosion);
        wn->executeAnimCmd(3u);
        wn->executeAnimCmd(4u);
    }
    if (wn->unk2218) {
        wn->changeProcFnc(Proc_ShotExplosion, 1);
    } else if (soSlow::getInstance()->isOn() == 1) {
        wn->changeProcFnc(Proc_ShotExplosion, 1);
    }
}

void wnemGalleomProc::ShotExplosion(wnemSimple* wn, soModuleAccesser* acc) {
    wnemGalleomExplosionParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemGalleomExplosionParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemGalleomProc::ShotExplosion  \n");
        wn->SetDeadMode();
        wn->setVisibilityWhole(0);
        if (param->m_shotAnim != 0) {
            wn->executeAnimCmd(param->m_shotAnim, 4, 1);
        }
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
    }
    if (wn->getFrameCounter(1) >= param->m_shotLife) {
        wn->RequestDeactive();
    }
}
