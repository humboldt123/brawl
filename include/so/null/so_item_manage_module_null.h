#pragma once

class BaseItem;

#include <so/item/so_item_manage_module_impl.h>

class soItemManageModuleNull : public soItemManageModule {
public:
    virtual ~soItemManageModuleNull();
    virtual void activate();
    virtual void deactivate();
    virtual void begin();
    virtual void check();
    virtual bool haveItem(itKind kind, u32 variation, u32 index, bool);
    virtual bool createThrowItem(itKind kind, u32 variation, u32 nodeIndex, float angle, float speed, float power);
    virtual bool haveItem(BaseItem* item, u32 index, bool, bool);
    virtual bool haveItem(u32 nodeIndex, BaseItem* item, u32 index, bool, bool);
    virtual bool useItem(BaseItem* item);
    virtual bool attachItem(BaseItem* item, bool);
    virtual bool pickupItem(u8, u32, u32, u32 index);
    virtual bool isSuccessPickupItem();
    virtual bool isHaveItem(u32 index) const;
    virtual itParam::SizeKind getHaveItemSize(u32 index) const;
    virtual itKind getHaveItemKind(u32 index) const;
    virtual itParam::TraitFlag getHaveItemTrait(u32 index) const;
    virtual itParam::HaveKind getHaveItemHoldKind(u32 index);
    virtual int getHaveItemParamInt(u32, u32 index) const;
    virtual float getHaveItemParamFloat(u32, u32 index) const;
    virtual void getHaveItemInfo(soItemInfo*, u32 index) const;
    virtual itParam::SizeKind getPickableItemSize();
    virtual itKind getPickableItemKind();
    virtual itParam::TraitFlag getPickableItemTrait();
    virtual void getPickableItemInfo(soItemInfo*) const;
    virtual bool removeItem(BaseItem*);
    virtual void removeItem(u32 index);
    virtual void removeAll();
    virtual void throwItem(float angle, float speed, float power, u32 index);
    virtual void throwItem(float angle, float speed, float power, Vec2f* pos, u32 index);
    virtual void dropItem(float, float, u32 index);
    virtual void useItem(u32 index);
    virtual void warpItem(Vec3f* pos, u32 index);
    virtual void bornItem(u32 index);
    virtual void shootItemBullet(u32, float, u32 index);
    virtual void shootItemBulletBlanks(u32, u32 index);
    virtual u32 getShootItemBullet(u32 index);
    virtual void updateHaveItemActionInfo(u32 index);
    virtual void setHaveItemScaleAnim(float, u32, u32 index);
    virtual void setHaveItemAction(u32, float, u32 index);
    virtual void setHaveItemVisibility(bool, int index);
    virtual void setHaveItemHoldAnim(bool, u32 index);
    virtual void setHaveItemConstraintNode(u32, u32 index);
    virtual void resetHaveItemConstraintNode(u32 index);
    virtual bool attachItem(itKind, u32 variation, bool);
    virtual bool isAttachItem(itKind);
    virtual u32 getAttachItemCount();
    virtual void getAttachItemInfo(soItemInfo*, u32 index) const;
    virtual void setAttachItemVisibility(bool, itParam::AttachGroup);
    virtual u8 getAttachItemVisibility();
    virtual void ejectHaveItem(u32 index, bool, bool);
    virtual void ejectAttachItem(u32 index, bool, bool);
    virtual void eject(int taskId);
    virtual void ejectAttach(itKind, bool, bool);
    virtual void dropAttach(itKind, float, float);
    virtual void dropAttachGroup(itParam::AttachGroup, float, float);
    virtual void scaleAttach(itKind, float);
    virtual void scaleItem(BaseItem*, float);
    virtual void setHaveItemTeam(int, u32 index);
};
