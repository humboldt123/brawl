#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <it/it_manager.h>
#include <so/area/so_area_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the fields of the area contact record that the inhale reads (as in the Kirby and Wario versions).
struct ftDededeInhaleContact {
    int unk0;
    int areaId;
    int unk8;
    u8 category;
    u8 unkD[11];
};
static_assert(sizeof(ftDededeInhaleContact) == sizeof(soAreaContactLog), "Area contact extent");

// HYPOTHESIS: two (value, flag) pairs initialized in the static initializer before the status instance (as in
// ft_wario_status_uniq_process_special_nopen.cpp; the owner of the pairs is not known).
struct ftDededeStatusUniqProcessSpecialNPair {
    u32 m_0;
    u32 m_4;
    ftDededeStatusUniqProcessSpecialNPair(u32 first, u32 second) : m_0(first), m_4(second) { }
};
static ftDededeStatusUniqProcessSpecialNPair g_ftDededeStatusUniqProcessSpecialNPairA(0xff, 0);
static ftDededeStatusUniqProcessSpecialNPair g_ftDededeStatusUniqProcessSpecialNPairB(0xff, 1);

class ftDededeStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialN() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);

    bool checkItemInhale(soModuleAccesser* moduleAccesser);
};

ftDededeStatusUniqProcessSpecialN g_ftDededeStatusUniqProcessSpecialN;

// HYPOTHESIS: Kirby with Dedede's ability (sub kind 0x20) uses the mouth area 9, Dedede himself area 0xb.
void ftDededeStatusUniqProcessSpecialN::initStatus(soModuleAccesser* moduleAccesser) {
    int area = moduleAccesser->getStageObject().soGetSubKind() == 0x20 ? 9 : 0xb;
    moduleAccesser->getAreaModule().enableArea(area, true, -1);
    moduleAccesser->getWorkManageModule().setInt(area, 0x20000008);
}

void ftDededeStatusUniqProcessSpecialN::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialN::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
        checkItemInhale(moduleAccesser);
    }
}

void ftDededeStatusUniqProcessSpecialN::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, nextStatus, moduleAccesser, true)) {
    case 0x117:
        return;
    default: {
        int i;
        soItemManageModule& items = moduleAccesser->getItemManageModule();
        for (i = 3; i < 7; ++i) {
            if (items.isHaveItem(i)) {
                moduleAccesser->getItemManageModule().removeItem(i);
            }
        }
    }
    case 0x127: {
        int area = moduleAccesser->getWorkManageModule().getInt(0x20000008);
        moduleAccesser->getAreaModule().enableArea(area, false, -1);
        break;
    }
    }
}

// HYPOTHESIS: takes up to four edible items touching the mouth area into the mouth slots 3..6, then changes to the swallow
// status (0x127; 0x1a3 for Kirby with the copied ability, sub kind 5).
bool ftDededeStatusUniqProcessSpecialN::checkItemInhale(soModuleAccesser* moduleAccesser) {
    soItemManageModule& items = moduleAccesser->getItemManageModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    soAreaModule& areas = moduleAccesser->getAreaModule();
    int area = moduleAccesser->getWorkManageModule().getInt(0x20000008);
    int contacts = areas.getAreaContactCount(area);
    int eaten = 0;
    int subKind = moduleAccesser->getStageObject().soGetSubKind();
    int node = 0x196;
    if (subKind == 0x20) {
        node = 6;
    }
    for (int i = 0; i < contacts && eaten < 4; ++i) {
        const ftDededeInhaleContact* contact = reinterpret_cast<const ftDededeInhaleContact*>(areas.getAreaContactLog(area, i));
        if (contact->category == soAreaData::Category_Item) {
            BaseItem* item = itManager::getInstance()->getItemFromAreaId(contact->areaId);
            if (item != NULL && item->isEatable()) {
                switch (item->soGetSubKind()) {
                case 0:
                case 0xD:
                case 0x14:
                case 0x1B:
                case 0x1D:
                    items.haveItem(node, item, eaten + 3, false, false);
                    break;
                default:
                    items.haveItem(node, item, eaten + 3, false, true);
                    break;
                }
                ++eaten;
                if (moduleAccesser->getStatusModule().getStatusKind() != 0x112 &&
                    moduleAccesser->getStatusModule().getStatusKind() != 0x117 &&
                    moduleAccesser->getStatusModule().getStatusKind() != 0x192 &&
                    moduleAccesser->getStatusModule().getStatusKind() != 0x193) {
                    return true;
                }
            }
        }
    }
    if (eaten > 0) {
        if (subKind == 5) {
            status.changeStatusRequest(0x1a3, moduleAccesser);
        } else {
            status.changeStatusRequest(0x127, moduleAccesser);
        }
        return true;
    }
    return false;
}
