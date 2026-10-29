#define MT_VEC2F_FROM_VEC3_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PIKMIN_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/fighter.h>
#include <ft/pikmin/ft_pikmin.h>
#include <ft/pikmin/ft_pikmin_status_uniq_process.h>
#include <ft/pikmin/ft_pikmin_utility.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

// Olimar's side special (the Pikmin throw): the status takes the first Pikmin of the line into his hand (Pikmin status 0x14, held
// by the troops manager the status process is given), places it at the hand node while the status runs (Pikmin status 0x15, turned
// by the slope under him) and lets the hold Pikmin go when the status ends.
// HYPOTHESIS: the status numbers and what the constants 0xfa2/0xfa3 describe (the offset of the held Pikmin from the node).
void ftPikminStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getPostureModule().setStickLR(0.0f);
    moduleAccesser->getPostureModule().updateRotYLr();
    m_troops->holdPikmin(1);
    if (m_troops->getHoldPikminNum() > 0) {
        m_troops->getHoldPikminTop()->changeStatus(0x14);
        wnPikminPikminTroopsManager* troops = m_troops;
        troops->syncLr(troops->getHoldPikminTop(), moduleAccesser->getPostureModule().getLr());
        moduleAccesser->getWorkManageModule().onFlag(0x22000012);
    }
}

void ftPikminStatusUniqProcessSpecialS::execStatus(soModuleAccesser*) { }

void ftPikminStatusUniqProcessSpecialS::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000010);
        if (m_troops->getHoldPikminNum() > 0) {
            if (moduleAccesser->getWorkManageModule().isFlag(0x22000011) == true) {
                wnPikminPikminTroopsManager* troops = m_troops;
                troops->onStatusFlag(troops->getHoldPikminTop(), 0x22000000);
            }
            wnPikminPikminTroopsManager* troops = m_troops;
            if (troops->isConstraint(troops->getHoldPikminTop(), -1) == true) {
                wnPikminPikminTroopsManager* constraintTroops = m_troops;
                constraintTroops->detachConstraint(constraintTroops->getHoldPikminTop());
            }
            troops = m_troops;
            troops->syncLr(troops->getHoldPikminTop(), moduleAccesser->getPostureModule().getLr());
            float slope = 0.0f;
            if (moduleAccesser->getSituationModule().getKind() == 0) {
                if (moduleAccesser->getGroundModule().isTouch(static_cast<grCollStatus::TouchMask>(8), 0) == true) {
                    Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(8, 0);
                    float normalY = normal.m_y;
                    float normalX = normal.m_x;
                    slope = (float)atan2(normalX, normalY);
                    if (slope != 0.0f) {
                        troops = m_troops;
                        troops->setStatusFloat(troops->getHoldPikminTop(), slope, 0x21000000);
                    }
                }
            }
            Vec2f nodePos(moduleAccesser->getModelModule().getNodeGlobalPosition(0, false));
            Vec2f offset(soValueAccesser::getConstantFloat(moduleAccesser, 0xfa2, 0),
                         soValueAccesser::getConstantFloat(moduleAccesser, 0xfa3, 0));
            if (moduleAccesser->getPostureModule().getLr() < 0.0f) {
                offset.m_x = -offset.m_x;
            }
            if (slope != 0.0f) {
                offset.rot(-slope, &offset);
            }
            float scale = moduleAccesser->getPostureModule().getScale();
            offset.m_x *= scale;
            offset.m_y *= scale;
            Vec2f spot = nodePos + offset;
            troops = m_troops;
            troops->syncPos(troops->getHoldPikminTop(), &spot);
            m_troops->getHoldPikminTop()->changeStatus(0x15);
            troops = m_troops;
            troops->updateNodeSRT(troops->getHoldPikminTop());
            m_troops->libertyPikminAll();
        }
    }
}

// MATCH-ONLY: the original keeps the task id in a register loaded before the calls.
#pragma opt_propagation off
void ftPikminStatusUniqProcessSpecialS::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (m_troops->getHoldPikminNum() > 0) {
        int taskId = moduleAccesser->getStageObject().m_taskId;
        wnPikminPikminTroopsManager* troops = m_troops;
        if (troops->isConstraint(troops->getHoldPikminTop(), taskId) == true) {
            if (soExternalValueAccesser::getStatusKind(m_troops->getHoldPikminTop()) == 0x15 ||
                soExternalValueAccesser::getStatusKind(m_troops->getHoldPikminTop()) == 0x14) {
                troops = m_troops;
                troops->detachConstraint(troops->getHoldPikminTop());
            }
        }
        if (nextStatusKind != 0xe && nextStatusKind != 0) {
            m_troops->getHoldPikminTop()->changeStatus(0xa);
        }
        ftPikminUtility::reducePikminAllHelper(&moduleAccesser->getStageObject());
    }
}
#pragma opt_propagation reset

// The hold Pikmin is placed at the node 0x29 while the throw statuses run.
void ftPikminStatusUniqProcessSpecialS::updateHoldPikmin(soModuleAccesser* moduleAccesser) {
    if (m_troops->getHoldPikminNum() > 0) {
        soArrayVector<Vec2f, 1> spots;
        spots.push(Vec2f(moduleAccesser->getModelModule().getNodeGlobalPosition(0x29, false)));
        m_troops->updateHoldPikmin(reinterpret_cast<const soArrayFixed<wnPikminPikminOwnerInfo>*>(&spots));
    }
}

