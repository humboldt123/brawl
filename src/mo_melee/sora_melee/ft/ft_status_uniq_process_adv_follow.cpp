#define MT_VEC3F_SUB_FRESH_REGS
#define MT_VEC3F_MUL_INLINE // paired-single Vec3f scale, as in the original code
#include <ft/fighter.h>
#include <ft/ft_external_value_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftStatusUniqProcessAdvFollow : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessAdvFollow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessAdvFollow g_ftStatusUniqProcessAdvFollow;

// HYPOTHESIS: the follower status (adventure mode) starts its looping effect 0x1030016 at the fighter and a status sound
// (0x2287); the handles are kept in work ints 0x20000000/1.
void ftStatusUniqProcessAdvFollow::initStatus(soModuleAccesser* moduleAccesser) {
    float scale = moduleAccesser->getPostureModule().getScale();
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    int effectHandle = moduleAccesser->getEffectModule().req(static_cast<EfID>(0x1030016), &Vec3f(pos.m_x, pos.m_y, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), scale, 0, -1);
    moduleAccesser->getWorkManageModule().setInt(effectHandle, 0x20000000);
    int soundHandle = moduleAccesser->getSoundModule().playStatusSE(static_cast<SndID>(0x2287), false, 0);
    moduleAccesser->getWorkManageModule().setInt(soundHandle, 0x20000001);
}

void ftStatusUniqProcessAdvFollow::execStatus(soModuleAccesser* moduleAccesser) { }

// Each frame the follower moves towards the hip of the main fighter at the speed in work float 0x21000004 (which grows by
// 0.008 per frame); within 5 units it ends the status (kind 0xe). When the main fighter is not accessible the speed resets.
void ftStatusUniqProcessAdvFollow::execFixPos(soModuleAccesser* moduleAccesser) {
    int entryId = g_ftManager->getEntryId(0);
    if (g_ftManager->isValidEntryId(entryId) == true) {
        if (g_ftManager->isFighterPositionAccessable(entryId, -1) == true) {
            Fighter* fighter = g_ftManager->getFighter(entryId, -1);
            Vec3f hip = ftExternalValueAccesser::getHipPos(fighter);
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            float speed = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
            Vec3f diff = hip - pos;
            float lengthSq = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
            if (lengthSq < 25.0f) {
                moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
                return;
            }
            pos = pos + diff * speed;
            float nextSpeed = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
            nextSpeed += 0.008f;
            moduleAccesser->getWorkManageModule().setFloat(nextSpeed, 0x21000004);
            moduleAccesser->getPostureModule().setPos(&pos);
            soEffectModule& effect = moduleAccesser->getEffectModule();
            int effectHandle = moduleAccesser->getWorkManageModule().getInt(0x20000000);
            effect.setPos(effectHandle, &pos);
        } else {
            moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x21000004);
        }
    }
}

// Leaving the follower status snaps the fighter to the hip of the main fighter (entry of player 0), plays the end effect
// 0x1030017 and sound 0x2288, and removes the looping effect.
void ftStatusUniqProcessAdvFollow::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    int entryId = g_ftManager->getEntryId(0);
    if (g_ftManager->isValidEntryId(entryId) == true) {
        if (g_ftManager->isFighterPositionAccessable(entryId, -1) == true) {
            Fighter* fighter = g_ftManager->getFighter(entryId, -1);
            Vec3f hip = ftExternalValueAccesser::getHipPos(fighter);
            hip.m_z = 0.0f;
            moduleAccesser->getPostureModule().initPos(&hip);
            reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).setLr(soExternalValueAccesser::getLr(fighter));
            Vec2f safePos(hip.m_x, hip.m_y);
            moduleAccesser->getGroundModule().setShapeSafePos(&safePos, 0);
            float scale = moduleAccesser->getPostureModule().getScale();
            moduleAccesser->getEffectModule().req(static_cast<EfID>(0x1030017), &Vec3f(hip.m_x, hip.m_y, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), scale, 0, -1);
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x2288), false, false, 0);
        }
    }
    int effectHandle = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    if (effectHandle != 0) {
        moduleAccesser->getEffectModule().remove(effectHandle);
    }
}
