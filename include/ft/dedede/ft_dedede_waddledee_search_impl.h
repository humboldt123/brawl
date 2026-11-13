#pragma once

#include <so/item/so_item_search_impl.h>
#include <types.h>

class soModuleAccesser;

// The item search of King Dedede's item manage module (g_ftDededeWaddledeeSearchImpl): besides the common item search it
// finds the closest Waddle Dee that can be picked up.
class ftDededeWaddledeeSearchImpl : public soItemSearchImpl {
public:
    // HYPOTHESIS: the result record of the search; only the word at 0x4 is written.
    struct Result {
        int unk0;
        int unk4;
    };

    virtual ~ftDededeWaddledeeSearchImpl() { }
    virtual bool search(int* areaId, u32 unk, soModuleAccesser* moduleAccesser);
    bool searchWaddledee(Result* result, soModuleAccesser* moduleAccesser);
};
