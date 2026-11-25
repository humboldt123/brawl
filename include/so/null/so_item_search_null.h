#pragma once

#include <so/item/so_item_search_impl.h>

class soItemSearchNull : public soItemSearch {
public:
    virtual ~soItemSearchNull();
    virtual bool search(int*, u32, soModuleAccesser*);
};
