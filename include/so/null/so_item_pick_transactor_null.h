#pragma once

class BaseItem;

#include <so/item/so_item_pick_transactor_impl.h>

class soItemPickTransactorNull : public soItemPickTransactor {
public:
    virtual ~soItemPickTransactorNull();
    virtual bool check(soItemSearch*, soItemInfo*, soModuleAccesser*);
    virtual bool pickup(soItemSearch*, soModuleAccesser*, u8, itParam::TraitMask, u32, u32 index);
};
