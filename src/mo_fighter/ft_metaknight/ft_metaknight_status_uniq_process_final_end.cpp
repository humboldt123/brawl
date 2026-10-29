#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/metaknight/ft_metaknight_status_views.h>
#include <ft/ft_external_value_accesser.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/collision/so_collision_log.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>


class ftMetaknightStatusUniqProcessFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessFinalEnd() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);

    // HYPOTHESIS: parameter types; hits the linked target with attack slot index.
    void attackFinal(Fighter* target, int index, soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: the power argument mirrors Fighter::notifyEventCollisionAttack, which forwards it here.
    void notifyEventCollisionAttackFighter(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    // Finds the first link slot in 6..11 that is not linked; -1 when all are in use.
    int getFreeFinalLinkWork(soModuleAccesser* moduleAccesser);
};

void ftMetaknightStatusUniqProcessFinalEnd::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000011);
        soCollisionAttackData* data = moduleAccesser->getCollisionAttackModule().getData(0, true);
        soCollisionAttackAbsoluteData attack;
        attack.m_slipChance = 1.0f;
        attack.m_hitStopFrame = 1.0f;
        attack.m_hitStopDelay = 1.0f;
        attack.unk20_17 = 0;
        attack.m_power = data->m_power;
        attack.m_vector = data->m_vector;
        attack.m_reactionEffect = data->m_reactionEffect;
        attack.m_reactionFix = data->m_reactionFix;
        attack.m_reactionAdd = data->m_reactionAdd;
        // HYPOTHESIS: the original reads these flags as unsigned fields; the enum fields here sign-extend them.
        attack.m_attribute = data->m_attribute;
        attack.m_soundLevel = data->m_soundLevel;
        attack.m_soundAttribute = data->m_soundAttribute;
        attack.m_lrCheck = data->m_lrCheck;
        attack.m_noHitStop = data->m_noHitStop;
        attack.m_noEffect = data->m_noEffect;
        for (int i = 6; i < 12; i++) {
            if (moduleAccesser->getLinkModule().isLink(i) == 1) {
                int index = i - 6;
                Fighter* target = dynamic_cast<Fighter*>(moduleAccesser->getLinkModule().getParent(i));
                if (target != NULL && target->isActive() == true) {
                    moduleAccesser->getCollisionAttackModule().setAbsolute(index, 0, &attack);
                    attackFinal(target, index, moduleAccesser);
                }
            }
        }
        ftMetaknightStatusUniqProcessFinalUtil::notifyMetaknightFinalEnd(moduleAccesser);
        ftMetaknightStatusUniqProcessFinalUtil::unLinkFinalTargetAll(moduleAccesser);
    }
}

void ftMetaknightStatusUniqProcessFinalEnd::exitStatus(soModuleAccesser* moduleAccesser, int) {
    ftMetaknightStatusUniqProcessFinalUtil::notifyMetaknightFinalEnd(moduleAccesser);
    ftMetaknightStatusUniqProcessFinalUtil::unLinkFinalTargetAll(moduleAccesser);
}

void ftMetaknightStatusUniqProcessFinalEnd::attackFinal(Fighter* target, int index, soModuleAccesser* moduleAccesser) {
    Vec3f pos = soExternalValueAccesser::getPrevRoughPos(target);
    int hitTarget = ftExternalValueAccesser::getHitTarget(target, 1);
    moduleAccesser->getCollisionAttackModule().hitAbsolute(index, &pos, target->m_taskId, 0, hitTarget);
}

void ftMetaknightStatusUniqProcessFinalEnd::notifyEventCollisionAttackFighter(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (collisionLog != NULL) {
        int linkId = getFreeFinalLinkWork(moduleAccesser);
        if (linkId >= 0) {
            // MATCH-ONLY: the original tests the link result without using it.
            if (moduleAccesser->getLinkModule().link(linkId, collisionLog->m_taskId) == false) {
                return;
            }
        }
    }
}

int ftMetaknightStatusUniqProcessFinalEnd::getFreeFinalLinkWork(soModuleAccesser* moduleAccesser) {
    for (int i = 6; i < 12; i++) {
        if (!moduleAccesser->getLinkModule().isLink(i)) {
            return i;
        }
    }
    return -1;
}

ftMetaknightStatusUniqProcessFinalEnd g_ftMetaknightStatusUniqProcessFinalEnd;
