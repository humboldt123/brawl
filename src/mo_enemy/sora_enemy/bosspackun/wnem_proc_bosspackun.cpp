#include <em/bosspackun/wnem_proc_bosspackun.h>
#include <em/wnem/wnem_proc_common.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <types.h>

// The procs of Petey Piranha's arms. An arm enters (ArmEnter) and moves on to idle at once; while idle it reports its
// hit points as a percentage to the boss, and when it dies it tells the boss (ArmDead). Every event to the boss
// carries the arm's id (emBosspackunLinkEventArgs in em/bosspackun/em_bosspackun.h).

// HYPOTHESIS: proc numbers of ArmIdol and of ArmDead (the arm's hit-points-zero proc).
static const s32 Proc_ArmDead = 0x11;
static const s32 Proc_ArmIdol = 0x10;

// HYPOTHESIS: the link events the boss's notifyEventLink handles (em_bosspackun.cpp): the arm reports (1), the AI
// events of the two health thresholds (0x1400, 0x1401) and the arm's death (0x13ff).
static const int LinkEvent_Arm = 1;
static const int LinkEvent_ArmDead = 0x13ff;
static const int LinkEvent_AI0 = 0x1400;
static const int LinkEvent_AI1 = 0x1401;
// The link module slot the events go to (the parents of the arm, that is the boss).
static const s32 LinkSlot_Parents = 3;

// The arm's parameters (wnemSimple::unk2234). HYPOTHESIS: roles from the reads below.
struct wnemBosspackunArmParam {
    u8 unk0[8];
    u32 m_threshold0; // 0x8, health percentage at or below which the arm sends LinkEvent_AI0
    u32 m_threshold1; // 0xC, the same for LinkEvent_AI1
    s32 m_unk10;      // 0x10, copied to wnemSimple 0x21E8
    s32 m_unk14;      // 0x14, copied to wnemSimple 0x21EC
    s32 m_effectValue; // 0x18, stored in the effect module (soEffectModuleImpl::unk50)
};

void wnemBosspackunProc::ArmEnter(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemBosspackunProc::ArmEnter  \n");
        wn->setProcFnc_HpZero(Proc_ArmDead);
        wnemBosspackunArmParam* param = reinterpret_cast<wnemBosspackunArmParam*>(wn->unk2234);
        if (param != NULL) {
            wn->unk21E8 = param->m_unk10;
            wn->unk21EC = param->m_unk14;
            soEffectModuleImpl& effect = dynamic_cast<soEffectModuleImpl&>(acc->getEffectModule());
            effect.unk50 = param->m_effectValue;
        }
        // HYPOTHESIS: the arm with id 0 faces right, the other left.
        if (wn->m_linkId == 0) {
            acc->getPostureModule().setLr(1.0f);
        } else {
            acc->getPostureModule().setLr(-1.0f);
        }
    }
    wn->changeProcFnc(Proc_ArmIdol, 1);
}

void wnemBosspackunProc::ArmIdol(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wn->setProcFnc_HpZero(Proc_ArmDead);
    }
    if (wn->unk2216) {
        wn->executeAnimCmd(0u);
        wn->executeAnimCmd(1u);
        UnkLinkEvent event(LinkEvent_Arm, wn->m_linkId);
        acc->getLinkModule().sendEventParents(LinkSlot_Parents, event);
        wn->unk2216 = false;
        wn->unk2217 = false;
    } else {
        if (wn->unk2217) {
            wn->unk2217 = false;
        }
        wnemBosspackunArmParam* param = NULL;
        if (wn->unk2234 != NULL) {
            param = reinterpret_cast<wnemBosspackunArmParam*>(wn->unk2234);
        }
        s32 mode = wn->m_intParams.at(0);
        // HYPOTHESIS: unk21D4 over unk21D8 are the arm's hit points over its maximum.
        float maxHp = wn->unk21D8;
        if (maxHp <= 0.0f) {
            maxHp = 1.0f;
        }
        float percent = 100.0f * (wn->unk21D4 / maxHp);
        if (mode == 0) {
            if (percent <= static_cast<float>(param->m_threshold0)) {
                UnkLinkEvent event(LinkEvent_AI0, wn->m_linkId);
                acc->getLinkModule().sendEventParents(LinkSlot_Parents, event);
            }
        } else if (mode == 1) {
            if (percent <= static_cast<float>(param->m_threshold1)) {
                UnkLinkEvent event(LinkEvent_AI1, wn->m_linkId);
                acc->getLinkModule().sendEventParents(LinkSlot_Parents, event);
            }
        }
    }
}

void wnemBosspackunProc::ArmDead(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wn->m_shotWork.unk2C[6] = true;
        wn->unk21B8 = false;
        wn->unk21BB = false;
        wn->unk21B9 = false;
        UnkLinkEvent event(LinkEvent_ArmDead, wn->m_linkId);
        acc->getLinkModule().sendEventParents(LinkSlot_Parents, event);
        // HYPOTHESIS: the boss accepts the event by setting the handled flag; then the arm turns translucent.
        if (event._0x4 == true) {
            acc->getCollisionHitModule().setXluGlobal(0);
        }
    }
}
