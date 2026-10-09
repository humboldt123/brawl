#pragma once

#include <havok/hkBase.h>

// GJK / separating-axis solver state (map name hkGsk). Only the members touched by the functions recovered so far are
// named; the object size and the rest of the layout are not known yet.
struct hkGsk {
    u8 unk0[0x10]; // 0x00 not identified
    u8 unk10; // 0x10 cleared by the constructor
    int unk14; // 0x14 HYPOTHESIS: feature-change flag, written by setFeatureChange

    hkGsk() __attribute__((never_inline));

    void setFeatureChange(int value);
    // HYPOTHESIS: exports the cache when the feature-change flag is set (tail call to exitAndExportCacheImpl).
    void checkForChangesAndUpdateCache(void* arg);
    // HYPOTHESIS: argument not identified; passed through unchanged by checkForChangesAndUpdateCache.
    void exitAndExportCacheImpl(void* arg);
};
