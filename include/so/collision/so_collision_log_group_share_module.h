#pragma once

#include <StaticAssert.h>
#include <so/so_array.h>
#include <types.h>

class soCollisionLogGroupShareModule {
public:
    soArray<int>* m_logGroups; // 0x0
    u8 unk4;
    u8 unk5;
    // HYPOTHESIS: the constructor's byte argument (PokeFushigisou passes the 2 log groups of its vector).
    s8 unk6;

    soCollisionLogGroupShareModule(soArray<int>* logGroups, s8 flag);
    virtual ~soCollisionLogGroupShareModule();
    void reset();
};
static_assert(sizeof(soCollisionLogGroupShareModule) == 0xC, "Class is wrong size!");
