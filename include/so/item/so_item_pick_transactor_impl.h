// SHADOW of BrawlHeaders/so/item/so_item_pick_transactor_impl.h: SO_ITEM_PICK_TRANSACTOR_INLINE_DTORS gives the inline
// destructors of a fighter REL's own pick transactor (the soItemPickTransactor one out of line).
#pragma once

#include <StaticAssert.h>
#include <so/so_null.h>
#include <it/it_archive.h>
#include <so/item/so_item_search_impl.h>
#include <types.h>

class soModuleAccesser;

struct soItemInfo {
    itKind m_kind;
    BaseItem* m_item;
    itParam::TraitFlag m_trait;
    itParam::SizeKind m_size;
    bool m_isVisible : 1;
    bool m_isHoldAnim : 1;
#ifdef SO_ITEM_INFO_BYTE_ATTACH_GROUP
    // BYTES: written in one byte together with the two flags (an enum bit-field is accessed word-wise).
    u8 m_attachGroup : 6;
#else
    itParam::AttachGroup m_attachGroup : 6;
#endif
    char _0x11[3];
};
static_assert(sizeof(soItemInfo) == 0x14, "Class is wrong size!");

class soItemPickTransactor : public soNull, public soNullable {
public:
#ifdef SO_ITEM_PICK_TRANSACTOR_INLINE_DTORS
    virtual ~soItemPickTransactor() __attribute__((never_inline)) { }
#else
    virtual ~soItemPickTransactor();
#endif
    virtual bool check(soItemSearch*, soItemInfo*, soModuleAccesser*);
    virtual bool pickup(soItemSearch*, soModuleAccesser*, u8, itParam::TraitMask, u32, u32 index);
};
static_assert(sizeof(soItemPickTransactor) == 12, "Class is wrong size!");

class soItemPickTransactorImpl : public soItemPickTransactor {
public:
#ifdef SO_ITEM_PICK_TRANSACTOR_INLINE_DTORS
    virtual ~soItemPickTransactorImpl() { }
#else
    virtual ~soItemPickTransactorImpl();
#endif
    virtual bool check(soItemSearch*, soItemInfo*, soModuleAccesser*);
    virtual bool pickup(soItemSearch*, soModuleAccesser*, u8, itParam::TraitMask, u32, u32 index);
};
static_assert(sizeof(soItemPickTransactorImpl) == 12, "Class is wrong size!");