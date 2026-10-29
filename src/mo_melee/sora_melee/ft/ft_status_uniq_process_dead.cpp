#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <gf/gf_camera.h>
#include <mt/mt_matrix.h>
#include <so/damage/so_damage_attacker_info.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

extern "C" int fn_27_112D44(ftOwner* owner);

// HYPOTHESIS: views of the two indefinite parameter tables read by exitStatus.
struct ftDeadParamA {
    u8 unk0[0x8];
    int m_8;
};
struct ftDeadParamB {
    u8 unk0[0x10];
    int m_10;
};

class ftStatusUniqProcessDead : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessDead() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixCamera(soModuleAccesser* moduleAccesser);
    int decCoin(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessDead g_ftStatusUniqProcessDead;

void ftStatusUniqProcessDead::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessDead::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessDead::execFixCamera(soModuleAccesser* moduleAccesser) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
        return;
    }
    gfCamera* camera = gfCameraManager::getManager() ? gfCameraManager::getManager()->getCamera(0) : NULL;
    if (camera == NULL) {
        return;
    }
    Matrix view(reinterpret_cast<Matrix*>(camera->m_view));
    Matrix inverseView(true);
    view.inverse(&inverseView);
    Vec3f pos;
    Vec3f rot;
    Vec3f screenPos(moduleAccesser->getWorkManageModule().getFloat(0x21000008), moduleAccesser->getWorkManageModule().getFloat(0x21000009), moduleAccesser->getWorkManageModule().getFloat(0x2100000a));
    inverseView.mulPos(&screenPos, &pos);
    moduleAccesser->getPostureModule().setPos(&pos);
    inverseView.getRotate(&rot);
    if (moduleAccesser->m_stageObject->soGetSubKind() != 0x12) {
        rot.m_y += 90.0f;
    }
    moduleAccesser->getPostureModule().setRot(&rot, 0);
    moduleAccesser->m_stageObject->updateNodeSRT();
}

void ftStatusUniqProcessDead::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    int handle = moduleAccesser->getWorkManageModule().getInt(0x20000003);
    if (handle != 0) {
        moduleAccesser->getEffectModule().remove(handle);
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000003);
    }
    int frame;
    switch (moduleAccesser->getWorkManageModule().getInt(0x10000029)) {
    case 0:
        frame = soValueAccesser::getConstantInt(moduleAccesser, 0x5a3a, 0);
        break;
    case 1:
        frame = static_cast<ftDeadParamA*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xa812, 0))->m_8;
        break;
    case 2:
        frame = static_cast<ftDeadParamB*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xa813, 0))->m_10;
        break;
    }
    moduleAccesser->getWorkManageModule().setInt(frame, 0x1000002a);
    moduleAccesser->getWorkManageModule().onFlag(0x12000000);
}

int ftStatusUniqProcessDead::decCoin(soModuleAccesser* moduleAccesser) {
    if (g_ftManager->m_gameRule == 2) {
        int entryId = moduleAccesser->getWorkManageModule().getInt(0x10000000);
        ftOwner* owner = g_ftManager->getOwner(entryId);
        if (!moduleAccesser->getWorkManageModule().isFlag(0x1200002e) && fn_27_112D44(owner) == 0) {
            int coin = g_ftManager->getCoin(entryId) / 2;
            int max = soValueAccesser::getConstantInt(moduleAccesser, 0x5a4b, 0);
            if (coin > max) {
                coin = max;
            }
            g_ftManager->lostCoin(entryId, coin, true);
            soDamageAttackerInfo info;
            moduleAccesser->getDamageModule().getAttackerInfo(&info);
            if (info.m_indirectSoKind == 0 && g_ftManager->isValidEntryId(info.m_indirectEntryId) == true && entryId != info.m_indirectEntryId) {
                g_ftManager->generateCoin(info.m_indirectEntryId, coin);
            }
            return coin;
        }
    }
    return 0;
}
