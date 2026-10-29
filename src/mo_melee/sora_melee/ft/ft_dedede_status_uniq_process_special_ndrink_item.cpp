#define FT_FIGHTER_ANIMCMD_LONG
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/fighter.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: two (value, flag) pairs initialized in the static initializer before the status instance (as in
// ft_wario_status_uniq_process_special_nopen.cpp; the owner of the pairs is not known).
struct ftDededeStatusUniqProcessSpecialNDrinkItemPair {
    u32 m_0;
    u32 m_4;
    ftDededeStatusUniqProcessSpecialNDrinkItemPair(u32 first, u32 second) : m_0(first), m_4(second) { }
};
static ftDededeStatusUniqProcessSpecialNDrinkItemPair g_ftDededeStatusUniqProcessSpecialNDrinkItemPairA(0xff, 0);
static ftDededeStatusUniqProcessSpecialNDrinkItemPair g_ftDededeStatusUniqProcessSpecialNDrinkItemPairB(0xff, 1);

class ftDededeStatusUniqProcessSpecialNDrinkItem : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialNDrinkItem() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftDededeStatusUniqProcessSpecialNDrinkItem g_ftDededeStatusUniqProcessSpecialNDrinkItem;

// HYPOTHESIS: the motions follow what is in the mouth slots 3..6: bombs, heavy items, food, or anything else (the work 0x20000007 gives
// the same choice when the item was already known). Kirby with the copied ability (sub kind 5) has his own motions.
void ftDededeStatusUniqProcessSpecialNDrinkItem::initStatus(soModuleAccesser* moduleAccesser) {
    bool heavy = false;
    bool food = false;
    bool bomb = false;
    if (moduleAccesser->getWorkManageModule().isFlag(0x2200001a)) {
        switch (moduleAccesser->getWorkManageModule().getInt(0x20000007)) {
        case 1:
            heavy = true;
            break;
        case 2:
            bomb = true;
            break;
        }
    } else {
        soItemManageModule& items = moduleAccesser->getItemManageModule();
        for (int i = 3; i < 7; ++i) {
            if (items.isHaveItem(i)) {
                soItemInfo info;
                items.getHaveItemInfo(&info, i);
                if (items.getHaveItemSize(i) == itParam::Size_Heavy) {
                    heavy = true;
                }
                if (info.m_trait.m_mask & itParam::TRAIT_MASK_BOMB) {
                    bomb = true;
                }
                if (info.m_trait.m_mask & itParam::TRAIT_MASK_FOOD) {
                    food = true;
                }
            }
        }
    }
    int subKind = moduleAccesser->getStageObject().soGetSubKind();
    if (bomb) {
        if (subKind == 5) {
            moduleAccesser->getWorkManageModule().setInt(0x2c7, 0x20000000);
            moduleAccesser->getWorkManageModule().setInt(0x2d1, 0x20000001);
        } else {
            moduleAccesser->getWorkManageModule().setInt(0x1d4, 0x20000000);
            moduleAccesser->getWorkManageModule().setInt(0x1de, 0x20000001);
        }
    } else if (heavy) {
        if (subKind == 5) {
            moduleAccesser->getWorkManageModule().setInt(0x2c8, 0x20000000);
            moduleAccesser->getWorkManageModule().setInt(0x2d2, 0x20000001);
        } else {
            moduleAccesser->getWorkManageModule().setInt(0x1d5, 0x20000000);
            moduleAccesser->getWorkManageModule().setInt(0x1df, 0x20000001);
        }
    } else if (food) {
        if (subKind == 5) {
            moduleAccesser->getWorkManageModule().setInt(0x2c6, 0x20000000);
            moduleAccesser->getWorkManageModule().setInt(0x2d0, 0x20000001);
        } else {
            moduleAccesser->getWorkManageModule().setInt(0x1d3, 0x20000000);
            moduleAccesser->getWorkManageModule().setInt(0x1dd, 0x20000001);
        }
    } else {
        if (subKind == 5) {
            moduleAccesser->getWorkManageModule().setInt(0x2c9, 0x20000000);
            moduleAccesser->getWorkManageModule().setInt(0x2d3, 0x20000001);
        } else {
            moduleAccesser->getWorkManageModule().setInt(0x1d6, 0x20000000);
            moduleAccesser->getWorkManageModule().setInt(0x1e0, 0x20000001);
        }
    }
}

void ftDededeStatusUniqProcessSpecialNDrinkItem::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialNDrinkItem::execStop(soModuleAccesser* moduleAccesser) {
}

// HYPOTHESIS: animation flags first hide the mouth items, then eat them once: the fighter hook applies the effects of the item, and the damage
// changes by the amount of the item (a bomb adds damage, other items heal up to a limit).
void ftDededeStatusUniqProcessSpecialNDrinkItem::execFixPos(soModuleAccesser* moduleAccesser) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x2200001a)) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
            soItemManageModuleImpl& items = dynamic_cast<soItemManageModuleImpl&>(moduleAccesser->getItemManageModule());
            for (int i = 3; i < 7; ++i) {
                if (items.isHaveItem(i)) {
                    items.setHaveItemVisibility(0, i);
                }
            }
            moduleAccesser->getWorkManageModule().offFlag(0x22000013);
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000014)) {
            soItemManageModuleImpl& items = dynamic_cast<soItemManageModuleImpl&>(moduleAccesser->getItemManageModule());
            Fighter& fighter = dynamic_cast<Fighter&>(moduleAccesser->getStageObject());
            int node = moduleAccesser->getStageObject().soGetSubKind() == 0x20 ? 6 : 0x196;
            for (int i = 3; i < 7; ++i) {
                if (items.isHaveItem(i)) {
                    soItemInfo info;
                    items.getHaveItemInfo(&info, i);
                    float damage = moduleAccesser->getDamageModule().getDamage(0);
                    float amount = 0.0f;
                    if (info.m_trait.m_mask & (itParam::TRAIT_MASK_FOOD | itParam::TRAIT_MASK_RECOVER | itParam::TRAIT_MASK_QUICK |
                                               itParam::TRAIT_MASK_TOUCH)) {
                        items.useItem(i);
                    } else if (info.m_trait.m_mask & itParam::TRAIT_MASK_BOMB) {
                        amount = moduleAccesser->getConstantFloatKirby(0xfa8);
                    } else if (damage - moduleAccesser->getConstantFloatKirby(0xfa9) < 0.0f) {
                        amount = -damage;
                    } else {
                        amount = -moduleAccesser->getConstantFloatKirby(0xfa9);
                    }
                    Vec3f position = moduleAccesser->getModelModule().getNodeGlobalPosition(node, false);
                    soLinkTouchItemEventArgs event(-1, info.m_item, position, 0.0f, false);
                    fighter.eatItem(&event);
                    moduleAccesser->getDamageModule().addDamage(amount, 0);
                    moduleAccesser->getItemManageModule().removeItem(i);
                }
            }
            moduleAccesser->getWorkManageModule().offFlag(0x22000014);
        }
    }
}

void ftDededeStatusUniqProcessSpecialNDrinkItem::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x2200001a)) {
        for (int i = 3; i < 7; ++i) {
            if (moduleAccesser->getItemManageModule().isHaveItem(i)) {
                moduleAccesser->getItemManageModule().removeItem(i);
            }
        }
    }
}
