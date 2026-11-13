// SHADOW of BrawlHeaders/so/item/so_item_search_impl.h: SO_ITEM_SEARCH_INLINE_DTORS gives the inline destructors of a
// fighter REL's own search (the soItemSearch one out of line, the soItemSearchImpl one expanded).
#pragma once

#include <StaticAssert.h>
#include <so/so_null.h>
#include <types.h>

class soModuleAccesser;

class soItemSearch : public soNull, public soNullable {
public:
#ifdef SO_ITEM_SEARCH_INLINE_DTORS
    virtual ~soItemSearch() __attribute__((never_inline)) { }
#else
    virtual ~soItemSearch();
#endif
    virtual bool search(int*, u32, soModuleAccesser*);

};
static_assert(sizeof(soItemSearch) == 12, "Class is wrong size!");

class soItemSearchImpl : public soItemSearch {
public:
#ifdef SO_ITEM_SEARCH_INLINE_DTORS
    virtual ~soItemSearchImpl() { }
#else
    virtual ~soItemSearchImpl();
#endif
    virtual bool search(int* areaId, u32, soModuleAccesser*);

};
static_assert(sizeof(soItemSearchImpl) == 12, "Class is wrong size!");