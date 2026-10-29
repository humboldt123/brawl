#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/metaknight/ft_metaknight_status_views.h>
#include <ft/ft_external_value_accesser.h>
#include <mt/mt_prng.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <ft/ft_manager.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>


class ftMetaknightStatusUniqProcessFinalAttackGallery : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessFinalAttackGallery() { }
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    static int getFinalAttackGalleryEntryId(soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: parameter types; index is the attack slot used for the hit.
    static void attackGallery(Fighter* target, int index, soModuleAccesser* moduleAccesser);
};

void ftMetaknightStatusUniqProcessFinalAttackGallery::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: work id 0x20000002 is a countdown that is decremented while it is positive.
    if (moduleAccesser->getWorkManageModule().getInt(0x20000002) > 0) {
        moduleAccesser->getWorkManageModule().decInt(0x20000002);
    }
}

void ftMetaknightStatusUniqProcessFinalAttackGallery::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000002) > 0) {
        return;
    }
    int entryId = getFinalAttackGalleryEntryId(moduleAccesser);
    if (entryId == -1) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000011);
        return;
    }
    moduleAccesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dc5, 0), 0x20000002);
    if (g_ftManager->isValidEntryId(entryId) == true) {
        Fighter* fighter = g_ftManager->getFighter(entryId, -1);
        if (fighter != NULL && fighter->isActive() == true) {
            attackGallery(fighter, 0, moduleAccesser);
        }
        Fighter* subFighter = g_ftManager->getSubFighter(entryId);
        if (subFighter != NULL && subFighter->isActive() == true) {
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
            moduleAccesser->getCollisionAttackModule().setAbsolute(1, 0, &attack);
            attackGallery(subFighter, 1, moduleAccesser);
        }
    }
}

void ftMetaknightStatusUniqProcessFinalAttackGallery::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x120) {
        ftMetaknightStatusUniqProcessFinalUtil::notifyMetaknightFinalEnd(moduleAccesser);
        ftMetaknightStatusUniqProcessFinalUtil::unLinkFinalTargetAll(moduleAccesser);
    }
}

int ftMetaknightStatusUniqProcessFinalAttackGallery::getFinalAttackGalleryEntryId(soModuleAccesser* moduleAccesser) {
    for (int id = 0x20000003; id < 0x2000000a; id++) {
        if (moduleAccesser->getWorkManageModule().getInt(id) != -1) {
            int entryId = moduleAccesser->getWorkManageModule().getInt(id);
            moduleAccesser->getWorkManageModule().setInt(-1, id);
            return entryId;
        }
    }
    return -1;
}

void ftMetaknightStatusUniqProcessFinalAttackGallery::attackGallery(Fighter* target, int index, soModuleAccesser* moduleAccesser) {
    soCollisionHitModule* hit = soExternalValueAccesser::getCollisionHitModule(target);
    bool canHit = true;
    int groupNum = hit->getGroupNum();
    for (int i = 0; i < groupNum; i++) {
        if (hit->getWhole(0) != 0 || hit->getGlobal(0) != 0) {
            canHit = false;
            break;
        }
    }
    if (canHit == true) {
        int hitTarget = ftExternalValueAccesser::getHitTarget(target, 1);
        moduleAccesser->getCollisionAttackModule().hitAbsolute(index, &moduleAccesser->getPostureModule().getPos(), target->m_taskId, 0, hitTarget);
    }
    Vec3f prevPos = soExternalValueAccesser::getPrevRoughPos(target);
    Vec3f rot(0.0f, 0.0f, randf() * 360.0f);
    moduleAccesser->getEffectModule().req((EfID)5, &prevPos, &rot, 1.0f, 0, -1);
}

ftMetaknightStatusUniqProcessFinalAttackGallery g_ftMetaknightStatusUniqProcessFinalAttackGallery;
