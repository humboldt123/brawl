#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PIKMIN_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/fighter.h>
#include <ft/pikmin/ft_pikmin.h>
#include <ft/pikmin/ft_pikmin_status_uniq_process.h>
#include <ft/pikmin/ft_pikmin_utility.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

// Olimar's aerial attacks: a Pikmin of the line is taken into his hand, hangs on him (model constraint 7, the Pikmin motion
// depends on the motion of the attack) and is let go again when the flag 0x22000011 is set or the status ends.
// HYPOTHESIS: the work flags (0x22000010 take the Pikmin, 0x22000011 release it), the motion numbers 0x64..0x66 (the five aerials'
// attack motions) and the Pikmin motions/statuses 0x1e..0x21, 0xe and 8.
void ftPikminStatusUniqProcessAttackAir::initStatus(soModuleAccesser*) { }

void ftPikminStatusUniqProcessAttackAir::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000010);
        m_troops->holdPikmin(1);
        if (m_troops->getHoldPikminNum() > 0) {
            int taskId = m_troops->getHoldPikminTop()->m_taskId;
            if (moduleAccesser->getLinkModule().link(7, taskId)) {
                moduleAccesser->getLinkModule().setAttribute(7, static_cast<soLinkConnection::Attribute>(0), true);
                int motion = 0x1e;
                if (moduleAccesser->getMotionModule().getKind() == 0x64) {
                    motion = 0x1f;
                }
                if (moduleAccesser->getMotionModule().getKind() == 0x65) {
                    motion = 0x20;
                }
                if (moduleAccesser->getMotionModule().getKind() == 0x66) {
                    motion = 0x21;
                }
                m_troops->getHoldPikminTop()->changeMotion(motion, false);
                m_troops->getHoldPikminTop()->changeStatus(0xe);
                wnPikminPikminTroopsManager* troops = m_troops;
                troops->syncLr(troops->getHoldPikminTop(), moduleAccesser->getPostureModule().getLr());
            }
        }
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        detachAttackAirPikmin(moduleAccesser);
        moduleAccesser->getWorkManageModule().offFlag(0x22000011);
    }
}

void ftPikminStatusUniqProcessAttackAir::exitStatus(soModuleAccesser* moduleAccesser, int) {
    detachAttackAirPikmin(moduleAccesser);
    if (moduleAccesser->getLinkModule().isLink(7)) {
        moduleAccesser->getLinkModule().unlink(7);
    }
}

// The hold Pikmin is placed at the node 0x29 while the aerial runs.
void ftPikminStatusUniqProcessAttackAir::updateHoldPikmin(soModuleAccesser* moduleAccesser) {
    if (m_troops->getHoldPikminNum() > 0) {
        soArrayVector<Vec2f, 1> spots;
        spots.push(Vec2f(moduleAccesser->getModelModule().getNodeGlobalPosition(0x29, false)));
        m_troops->updateHoldPikmin(reinterpret_cast<const soArrayFixed<wnPikminPikminOwnerInfo>*>(&spots));
    }
}

void ftPikminStatusUniqProcessAttackAir::detachAttackAirPikmin(soModuleAccesser* moduleAccesser) {
    if (m_troops->getHoldPikminNum() > 0) {
        wnPikminPikminTroopsManager* troops = m_troops;
        if (troops->isConstraint(troops->getHoldPikminTop(), -1) == true) {
            troops = m_troops;
            troops->detachConstraint(troops->getHoldPikminTop());
        }
        m_troops->getHoldPikminTop()->changeStatus(8);
        ftPikminUtility::reducePikminAllHelper(&moduleAccesser->getStageObject());
    }
}
