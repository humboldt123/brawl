#include <ft/fighter.h>
#include <ft/ft_owner.h>
#include <it/it_gen_archive.h>
#include <it/it_manager.h>
#include <it/item.h>
#include <math.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the two unused file-local item sheets the original constructs before the status process.
static itGenSheetKind g_unkItemSheet0(Stages::srStageKind(0xff), 0);
static itGenSheetKind g_unkItemSheet1(Stages::srStageKind(0xff), 1);

class ftDiddyStatusUniqProcessSpecialBanana : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialBanana() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDiddyStatusUniqProcessSpecialBanana::initStatus(soModuleAccesser* moduleAccesser) {
    s32 taskId = moduleAccesser->getStageObject().m_taskId;
    itManager* manager = itManager::getInstance();
    int bananas = manager->getItemNum(itKind(2), 0, taskId, -1);
    if (bananas < soValueAccesser::getConstantInt(moduleAccesser, 0x5dc2, 0)) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000015);
    }
}

void ftDiddyStatusUniqProcessSpecialBanana::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000015) &&
        moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
        s32 taskId = moduleAccesser->getStageObject().m_taskId;
        itManager* manager = itManager::getInstance();
        BaseItem* item = manager->createItem(itKind(2), 0, taskId);
        if (item != NULL) {
            item->have(moduleAccesser->getStageObject().m_taskId, 0x39, -1);
            if (item->isHaved() == true) {
                moduleAccesser->getWorkManageModule().setInt(item->m_instanceId, 0x20000000);
                moduleAccesser->getWorkManageModule().onFlag(0x22000014);
                dynamic_cast<Fighter&>(moduleAccesser->getStageObject()).getOwner()->addLogInt(1, 0x1d, 0);
            }
        }
        moduleAccesser->getWorkManageModule().offFlag(0x22000013);
    }
}

void ftDiddyStatusUniqProcessSpecialBanana::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000014)) {
            int instanceId = moduleAccesser->getWorkManageModule().getInt(0x20000000);
            BaseItem* item = itManager::getInstance()->getItemFromInstanceId(instanceId);
            float angle = 0.017453292f * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb9, 0);
            float speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfba, 0);
            float lr = moduleAccesser->getPostureModule().getLr();
            if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
                speed *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfbb, 0);
            }
            Vec3f velocity(lr * (speed * cosf(angle)), speed * sinf(angle), moduleAccesser->getGroundModule().getZ());
            if (item != NULL) {
                item->throwAttack(lr, &velocity, 1.0f);
            }
            moduleAccesser->getWorkManageModule().offFlag(0x22000014);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x22000012);
    }
}

void ftDiddyStatusUniqProcessSpecialBanana::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000014)) {
        int instanceId = moduleAccesser->getWorkManageModule().getInt(0x20000000);
        BaseItem* item = itManager::getInstance()->getItemFromInstanceId(instanceId);
        itManager::getInstance()->removeItem(item);
    }
}

ftDiddyStatusUniqProcessSpecialBanana g_ftDiddyStatusUniqProcessSpecialBanana;
