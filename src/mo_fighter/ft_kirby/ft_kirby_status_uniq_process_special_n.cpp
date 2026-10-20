#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/fighter.h>
#include <ft/kirby/ft_kirby_unk8.h>
#include <ft/ft_log_transactor.h>
#include <ft/ft_owner.h>
#include <it/it_manager.h>
#include <so/area/so_area_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: maps a copy ability (fighter kind) to the status Kirby enters when he is given that ability by a thrown fighter
// (a table of the main unit); -1 when there is none.
extern "C" int fn_96_1DD84(int kind);

static ftKirbyUnk8 g_ftKirbyUnk8A(0xff, 0);
static ftKirbyUnk8 g_ftKirbyUnk8B(0xff, 1);

// Contact record of the area as read for items (see the Wario version of this status).
struct ftKirbyInhaleContact {
    int unk0;
    int areaId;
    int unk8;
    u8 category;
    u8 unkD[11];
};
static_assert(sizeof(ftKirbyInhaleContact) == sizeof(soAreaContactLog), "Area contact extent");

// HYPOTHESIS: Kirby's Special N (inhale) status process: status kinds 0x112-0x117 are the mouth statuses, 0x126 swallows
// what was caught. Names are unknown.
class ftKirbyStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessSpecialN() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);

    bool inhaleItems(soModuleAccesser* moduleAccesser);
    void releaseItems(soModuleAccesser* moduleAccesser);
};

// The mouth area (9) is enabled; with a copy ability waiting (work 0x10000048) Kirby changes to the status for it, otherwise the
// log notes the start of the inhale.
void ftKirbyStatusUniqProcessSpecialN::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    moduleAccesser->getAreaModule().enableArea(9, true, -1);
    if (work.isFlag(0x12000042)) {
        int kind = work.getInt(0x10000048);
        if (kind != -1) {
            int status = fn_96_1DD84(kind);
            if (status != -1) {
                moduleAccesser->getAreaModule().enableArea(9, false, -1);
                moduleAccesser->getStatusModule().changeStatusRequest(status, moduleAccesser);
            }
        }
    } else {
        Fighter& fighter = dynamic_cast<Fighter&>(moduleAccesser->getStageObject());
        int color = fighter.getOwner()->getFighterColor();
        if (color == 0xc) {
            moduleAccesser->getWorkManageModule().onFlag(0x2200001b);
        }
        ftLogTransactor::resetLogActionInfo(2, 0x13, 0, fighter.getOwner(), moduleAccesser);
    }
}

void ftKirbyStatusUniqProcessSpecialN::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftKirbyStatusUniqProcessSpecialN::execStop(soModuleAccesser* moduleAccesser) {
}

void ftKirbyStatusUniqProcessSpecialN::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
        inhaleItems(moduleAccesser);
    }
}

void ftKirbyStatusUniqProcessSpecialN::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    switch (status) {
    case 0x117:
        return;
    default:
        releaseItems(moduleAccesser);
    case 0x126:
        moduleAccesser->getAreaModule().enableArea(9, false, -1);
        break;
    }
}

// Takes up to four edible items touching the mouth area into the mouth slots 3..6; Kirby swallows (0x126) once any was taken.
bool ftKirbyStatusUniqProcessSpecialN::inhaleItems(soModuleAccesser* moduleAccesser) {
    soItemManageModule& items = moduleAccesser->getItemManageModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    soAreaModule& areas = moduleAccesser->getAreaModule();
    int contacts = areas.getAreaContactCount(9);
    int eaten = 0;
    for (int i = 0; i < contacts && eaten < 4; ++i) {
        const ftKirbyInhaleContact* contact = reinterpret_cast<const ftKirbyInhaleContact*>(areas.getAreaContactLog(9, i));
        if (contact->category == soAreaData::Category_Item) {
            BaseItem* item = itManager::getInstance()->getItemFromAreaId(contact->areaId);
            if (item != NULL && item->isEatable()) {
                // These kinds use the alternate attachment option (as for Wario).
                switch (item->soGetSubKind()) {
                case 0:
                case 0xD:
                case 0x14:
                case 0x1B:
                case 0x1D:
                    items.haveItem(0x199, item, eaten + 3, false, false);
                    break;
                default:
                    items.haveItem(0x199, item, eaten + 3, false, true);
                    break;
                }
                ++eaten;
                if (moduleAccesser->getStatusModule().getStatusKind() != 0x112 &&
                    moduleAccesser->getStatusModule().getStatusKind() != 0x117) {
                    return true;
                }
            }
        }
    }
    if (eaten > 0) {
        status.changeStatusRequest(0x126, moduleAccesser);
        return true;
    }
    return false;
}

void ftKirbyStatusUniqProcessSpecialN::releaseItems(soModuleAccesser* moduleAccesser) {
    soItemManageModule& items = moduleAccesser->getItemManageModule();
    for (int i = 3; i < 7; ++i) {
        if (items.isHaveItem(i)) {
            moduleAccesser->getItemManageModule().removeItem(i);
        }
    }
}

ftKirbyStatusUniqProcessSpecialN g_ftKirbyStatusUniqProcessSpecialN;

ftKirbyUnk8::ftKirbyUnk8(u32 a, u32 b) : unk0(a), unk4(b) { }
