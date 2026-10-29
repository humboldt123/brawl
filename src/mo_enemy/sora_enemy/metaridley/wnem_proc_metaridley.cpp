#include <em/metaridley/wnem_proc_metaridley.h>
#include <em/wnem/wnem_proc_common.h>
#include <so/link/so_link_module_impl.h>
#include <so/stageobject.h>
#include <types.h>

// The procs of the Meta Ridley's fireball and normal bullet. A fireball flies until it touches the ground or hits a
// shell, then explodes (or bursts on the ground), sends its owner a link event and is removed after a set time; a
// normal bullet flies and just vanishes.

// HYPOTHESIS: proc numbers: 0x43 is the fireball's touch-ground proc, 0x25 its explosion (hit shell) proc, 0x27 the
// normal bullet's Disappear proc.
static const s32 Proc_FireballTouchGround = 0x43;
static const s32 Proc_FireballExplosion = 0x25;
static const s32 Proc_NormalBulletDisappear = 0x27;
// The link index of the Meta Ridley that fired the fireball.
static const int LinkNo_Owner = 3;

// HYPOTHESIS: event 0x13ff is the link event the fireball sends its owner when it ends (answered in
// emMetaridley::notifyEventLink). It is sent as the header's UnkLinkEvent, as the Arman's ArmDead does: a local
// soLinkEventArgs subclass with the same constructor is not inlined (16 more bytes per proc).

// The fireball's parameters (wnemSimple::unk2234). HYPOTHESIS: roles from the reads below.
struct wnemMetaridleyFireballParam {
    u32 m_endFrame; // 0x0, the frames after exploding until the fireball is removed
    s32 m_endAnim;  // 0x4, the animation command run on exploding (0 for none)
};

void wnemMetaridleyProc::FireballFly(wnemSimple* wn) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wn->setProcFnc_TouchGround(Proc_FireballTouchGround);
        wn->setProcFnc_HitShell(Proc_FireballExplosion);
        wn->executeAnimCmd(4u);
        wn->executeAnimCmd(5u);
    }
    if (wn->unk2218) {
        wn->changeProcFnc(Proc_FireballExplosion, 1);
    }
}

void wnemMetaridleyProc::FireballExplosion(wnemSimple* wn, soModuleAccesser* acc) {
    wnemMetaridleyFireballParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemMetaridleyFireballParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemMetaridleyProc::FireballExplosion  \n");
        wn->SetDeadMode();
        wn->setVisibilityWhole(0);
        if (param->m_endAnim != 0) {
            wn->executeAnimCmd(param->m_endAnim, 4, 1);
        }
        wn->executeAnimCmd(0u);
        wn->executeAnimCmd(2u);
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
        UnkLinkEvent args(0x13ff, LinkNo_Owner);
        acc->getLinkModule().sendEventParents(LinkNo_Owner, args);
    }
    if (wn->getFrameCounter(1) >= param->m_endFrame) {
        wn->RequestDeactive();
    }
}

void wnemMetaridleyProc::FireballTouchGround(wnemSimple* wn, soModuleAccesser* acc) {
    wnemMetaridleyFireballParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemMetaridleyFireballParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemMetaridleyProc::FireballTouchGround  \n");
        wn->SetDeadMode();
        wn->setVisibilityWhole(0);
        if (param->m_endAnim != 0) {
            wn->executeAnimCmd(param->m_endAnim, 4, 1);
        }
        wn->executeAnimCmd(7u);
        wn->executeAnimCmd(2u);
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
        UnkLinkEvent args(0x13ff, LinkNo_Owner);
        acc->getLinkModule().sendEventParents(LinkNo_Owner, args);
    }
    if (wn->getFrameCounter(1) >= param->m_endFrame) {
        wn->RequestDeactive();
    }
}

void wnemMetaridleyProc::NormalBulletFly(wnemSimple* wn) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemMetaridleyProc::NormalBulletFly  \n");
        wn->setProcFnc_TouchGround(Proc_NormalBulletDisappear);
        wn->setProcFnc_HitShell(Proc_NormalBulletDisappear);
        wn->executeAnimCmd(6u);
    }
}

void wnemMetaridleyProc::NormalBulletDisappear(wnemSimple* wn) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemMetaridleyProc::NormalBulletDisappear  \n");
        wn->executeAnimCmd(1u);
        wn->executeAnimCmd(3u);
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
        wn->RequestDeactive();
    }
}
