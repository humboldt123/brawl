#pragma once

#include <types.h>

struct wnSimpleData
#ifdef FT_KIRBY_ARTICLE_POOLS
{
    u8 unk0[0x38];
}
#endif
;
// HYPOTHESIS: source descriptor spelling. Both constructor consumers read a
// kind/subkind pair through the first pointer and the heap module through the second.
struct wnSimpleKindInfo {
    s32 kind, subKind;
    wnSimpleKindInfo() { }
    wnSimpleKindInfo(s32 subKind) : kind(0), subKind(subKind) { }
};
struct wnSimpleConstructionInfo {
    const wnSimpleKindInfo* kindInfo;
    void* heapModule;
    wnSimpleConstructionInfo(const wnSimpleKindInfo& kind, void* heap) :
        kindInfo(&kind), heapModule(heap) { }
};
static_assert(sizeof(wnSimpleConstructionInfo) == 8, "Simple article construction descriptor");
