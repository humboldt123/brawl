#pragma once

#include <StaticAssert.h>
#include <so/so_array.h>
#include <types.h>

class soCollisionLogGroupShareModule {
public:
    soArray<int>* m_logGroups; // 0x0
    u8 unk4;
    u8 unk5;
    // HYPOTHESIS: the constructor's flag argument.
    bool unk6;

    soCollisionLogGroupShareModule(soArray<int>* logGroups, bool flag);
    virtual ~soCollisionLogGroupShareModule();
    void reset();
};
static_assert(sizeof(soCollisionLogGroupShareModule) == 0xC, "Class is wrong size!");
