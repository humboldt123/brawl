#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <it/it_manager.h>
#include <so/area/so_area_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <types.h>

// Fields of the area's contact record used by Chomp. The remaining bytes are
// intentionally unnamed; the area manager owns the full record.
struct ftWarioChompContact {
    int unk0;
    int areaId;
    int unk8;
    u8 category;
    u8 unkD[11];
};
static_assert(sizeof(ftWarioChompContact) == sizeof(soAreaContactLog), "Area contact extent");

// Weapon::notifyEventLink handles this event by deactivating the projectile.
struct ftWarioConsumeWeaponEvent : soLinkEventArgs {
    ftWarioConsumeWeaponEvent() : soLinkEventArgs(0x835) { }
};

// HYPOTHESIS: two (value, flag) pairs initialized in the static initializer before the status instance (as in
// ft_status_uniq_process_swallowed_thrown.cpp; the owner of the pairs is not known).
struct ftWarioStatusUniqProcessSpecialNOpenPair {
    u32 m_0;
    u32 m_4;
    ftWarioStatusUniqProcessSpecialNOpenPair(u32 first, u32 second) : m_0(first), m_4(second) { }
};
static ftWarioStatusUniqProcessSpecialNOpenPair g_ftWarioStatusUniqProcessSpecialNOpenPairA(0xff, 0);
static ftWarioStatusUniqProcessSpecialNOpenPair g_ftWarioStatusUniqProcessSpecialNOpenPairB(0xff, 1);

class ftWarioStatusUniqProcessSpecialNOpen : public soStatusUniqProcess {
public:
    virtual ~ftWarioStatusUniqProcessSpecialNOpen() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftWarioStatusUniqProcessSpecialNOpen g_ftWarioStatusUniqProcessSpecialNOpen;

// Kirby has a different mouth area and weapon link. His copied status IDs are
// converted only where the native code does so.
void ftWarioStatusUniqProcessSpecialNOpen::initStatus(soModuleAccesser* a) {
    int area = 9 + (a->getStageObject().soGetSubKind() == 5);
    a->getAreaModule().enableArea(area, true, -1);
    a->getWorkManageModule().setInt(area, 0x20000006);
    int status = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(
        0, a->getStatusModule().getStatusKind(), a, true);
    if (status == 0x112) a->getWorkManageModule().setInt(-1, 0x20000007);
}
// Collect up to four edible items into mouth slots 3..6. Search-selected
// projectiles are consumed through a separate link. Item traits select the
// next eating state; a status change during pickup suppresses that request.
void ftWarioStatusUniqProcessSpecialNOpen::execFixPos(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soItemManageModule& items = a->getItemManageModule();
    soStatusModule& status = a->getStatusModule();
    soAreaModule& areas = a->getAreaModule();
    bool heavy = false;
    bool food = false;
    bool bomb = false;
    bool interrupted = false;
    int area = work.getInt(0x20000006);
    int contacts = areas.getAreaContactCount(area);
    int eaten = 0;
    for (int i = 0; i < contacts && eaten < 4; ++i) {
        const ftWarioChompContact* contact = reinterpret_cast<const ftWarioChompContact*>(areas.getAreaContactLog(area, i));
        if (contact->category != soAreaData::Category_Item) continue;
        BaseItem* item = itManager::getInstance()->getItemFromAreaId(contact->areaId);
        if (item == NULL) continue;
        int trait = item->getParam(itValueAccesser::Work_Param_Int_Trait);
        if (!item->isEatable()) continue;
        int node = a->getModelModule().getCorrectNodeId(300);
        // These kinds use the alternate attachment option. The numeric kinds
        // are retained until their item identities are independently verified.
        switch (item->soGetSubKind()) {
        case 0: case 0xD: case 0x14: case 0x1B: case 0x1D:
            items.haveItem(node, item, eaten + 3, false, false);
            break;
        default:
            items.haveItem(node, item, eaten + 3, false, true);
            break;
        }
        ++eaten;
        if (item->getParam(itValueAccesser::Specialized_Param_Int_Size_Kind) == itParam::Size_Heavy) heavy = true;
        if (trait & itParam::TRAIT_MASK_BOMB) bomb = true;
        if (trait & itParam::TRAIT_MASK_FOOD) food = true;
        if (a->getStatusModule().getStatusKind() != 0x112 &&
            a->getStatusModule().getStatusKind() != 0x117 &&
            a->getStatusModule().getStatusKind() != 0x14B &&
            a->getStatusModule().getStatusKind() != 0x14C) {
            interrupted = true;
            break;
        }
    }
    u32 taskId = work.getInt(0x20000007);
    if (taskId != u32(-1) && gfTask::getTask(taskId) != NULL) {
        if (work.getInt(0x20000008) == 1) bomb = true;
        int link = a->getStageObject().soGetSubKind() == 5 ? 7 : 8;
        a->getLinkModule().link(link, taskId);
        ftWarioConsumeWeaponEvent event;
        a->getLinkModule().sendEventParents(link, event);
        ++eaten;
    }
    if (!interrupted && eaten > 0) {
        int next;
        if (bomb) next = 0x11E;
        else if (heavy) next = 0x11F;
        else if (food) next = 0x11D;
        else next = 0x120;
        next = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, next, a, true);
        status.changeStatusRequest(next, a);
    }
}
void ftWarioStatusUniqProcessSpecialNOpen::exitStatus(soModuleAccesser* a, int next) {
    if (next != 0x117 && next != 0x14C) {
        int area = a->getWorkManageModule().getInt(0x20000006);
        a->getAreaModule().enableArea(area, false, -1);
    }
    int original = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, next, a, true);
    switch (original) {
    case 0x117: case 0x11D: case 0x11E: case 0x11F: case 0x120:
        break;
    default: {
        int i;
        soItemManageModule& items = a->getItemManageModule();
        for (i = 3; i < 7; ++i) {
            if (items.isHaveItem(i)) a->getItemManageModule().removeItem(i);
        }
        break;
    }
    }
}
