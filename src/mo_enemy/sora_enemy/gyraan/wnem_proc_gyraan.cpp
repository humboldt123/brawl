#include <em/gyraan/wnem_proc_gyraan.h>
#include <em/wnem/wnem_proc_common.h>
#include <gr/collision/gr_collision_status.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <types.h>

// The procs of the Gyraan's axes. The axe is thrown into the air (AxeIdol), flies until it hits the floor or the wall
// it faces (AxeSwing), then sticks there and tells the Gyraan (AxeStick).

// HYPOTHESIS: proc 0x40 is AxeStick (it is entered when AxeSwing sees the axe touch something).
static const s32 Proc_AxeStick = 0x40;
// The Gyraan's link handler reacts to this event (AI flag 0x18, see gyraan/em_gyraan.cpp).
static const int LinkEvent_AxeStuck = 0x1403;

// The axe's parameters (wnemSimple::unk2234).
// HYPOTHESIS: the flag lets the axe pass through platforms (it selects the Air_Passable ground correction).
struct wnemGyraanAxeParam {
    bool m_isPassable; // 0x0
};

void wnemGyraanProc::AxeIdol(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemGyraanProc::AxeIdol  \n");
        acc->getSituationModule().setKind(Situation_Air, true);
        wn->setProcFnc_TouchGround(-1);
    }
}

void wnemGyraanProc::AxeSwing(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("wnemGyraanProc::AxeSwing : OnEnter \n");
        acc->getSituationModule().setKind(Situation_Air, true);
    } else if ((u8)acc->getGroundModule().getCorrect(0) != soGroundShapeImpl::Correct_None) {
        wnemGyraanAxeParam* param = NULL;
        if (wn->unk2234 != NULL) {
            param = reinterpret_cast<wnemGyraanAxeParam*>(wn->unk2234);
        }
        if (param->m_isPassable == true) {
            acc->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air_Passable, 0);
        }
        // The axe sticks when it touches the floor, or the wall on the side it faces.
        bool isTouched = false;
        soGroundModuleImpl& ground = dynamic_cast<soGroundModuleImpl&>(acc->getGroundModule());
        if (ground.isTouch(grCollStatus::TOUCH_MASK_DOWN, 0) == true) {
            isTouched = true;
        } else if (acc->getPostureModule().getLr() == -1.0f) {
            if (ground.isTouch(grCollStatus::TOUCH_MASK_LEFT | grCollStatus::TOUCH_MASK_DOWN_LEFT, 0) == true) {
                isTouched = true;
            }
        } else if (ground.isTouch(grCollStatus::TOUCH_MASK_RIGHT | grCollStatus::TOUCH_MASK_DOWN_RIGHT, 0) == true) {
            isTouched = true;
        }
        if (isTouched) {
            wn->changeProcFnc(Proc_AxeStick, 1);
        }
    }
}

void wnemGyraanProc::AxeStick(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("wnemGyraanProc::AxeStick : OnEnter \n");
        acc->getCollisionAttackModule().clearAll();
        acc->getSituationModule().setKind(Situation_Air, true);
        wnemGyraanAxeParam* param = NULL;
        if (wn->unk2234 != NULL) {
            param = reinterpret_cast<wnemGyraanAxeParam*>(wn->unk2234);
        }
        if ((u8)acc->getGroundModule().getCorrect(0) != soGroundShapeImpl::Correct_None && param->m_isPassable == true) {
            acc->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air_Passable, 0);
        }
        wn->setProcFnc_TouchGround(-1);
        UnkLinkEvent event(LinkEvent_AxeStuck, 3);
        acc->getLinkModule().sendEventParents(3, event);
    }
}
