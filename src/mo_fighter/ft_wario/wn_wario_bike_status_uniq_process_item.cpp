#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <wn/wario/wn_wario_bike_status_uniq_process.h>
#include <wn/wario/wn_wario_bike_link_event.h>
#include <it/it_manager.h>
#include <it/item.h>
#include <ft/wario/ft_wario_unk8.h>
#include <so/so_module_accesser.h>

// HYPOTHESIS: shared-header records; see ft_wario_unk8.h. Keep native initialization order.
static ftWarioUnk8 s_unkTag0(0xff, 0);
static ftWarioUnk8 s_unkTag1(0xff, 1);

void wnWarioBikeStatusUniqProcessItem::initStatus(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soKineticModule& kinetic = a->getKineticModule();
    soPostureModule& posture = a->getPostureModule();
    soModelModule& model = a->getModelModule();
    soLinkModule& link = a->getLinkModule();
    if (link.isModelConstraint()) link.removeModelConstraint(true);
    Vec3f spawnPosition = model.getNodeGlobalPosition(9, false);
    float facing = posture.getLr();
    bool grounded = a->getGroundModule().isTouch((grCollStatus::TouchMask)8, 0);
    if (!grounded) spawnPosition.m_y += 0.1f;
    soResourceModule* resource = &a->getResourceModule();
    BaseItem* bike = itManager::getInstance()->createItem(
        &spawnPosition, &spawnPosition, facing, Item_Wario_Bike, 0,
        link.getParentTaskId(3), resource, work.getInt(0x10000005), work.getInt(0x10000006),
        0, 0xffff);
    if (bike) {
        bike->setOwnerScale(a->getPostureModule().getScale());
        Vec3f speed(kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1)), 0.0f);
        Vec3f rotation = model.getNodeGlobalRotation(9, false);
        if (rotation.m_z != 0.0f) {
            rotation.m_x += 180.0f;
            rotation.m_z += 180.0f;
            rotation.m_y += facing == 1.0f ? 90.0f : 270.0f;
        } else if (rotation.m_x != 0.0f) {
            rotation.m_y = 0.0f;
        } else if (rotation.m_x == 0.0f && rotation.m_z == 0.0f) {
            // MATCH-ONLY: retain the native repeated tests and Z/Y/X stores.
            rotation.m_x = rotation.m_y = rotation.m_z = 0.0f;
        }
        bike->resetRotation(&rotation);
        bike->updateNodeSRT();
        bike->addSpeed(&speed, true);
        // HYPOTHESIS: the original reads this work value and discards the result.
        work.getFloat(0x11000001);
        bike->resetDamage();
        ftWarioBikeTaskEvent event(0x45C, bike->m_taskId);
        link.sendEventNodes(7, event, 0);
    }
    work.onFlag(0x2200000B);
}

wnWarioBikeStatusUniqProcessItem g_wnWarioBikeStatusUniqProcessItem;
