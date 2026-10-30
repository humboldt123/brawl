#pragma once

// Local BrawlHeaders shadow: names the size field that soCollisionSearchModuleImpl::getSize/setSize read and write.

#include <StaticAssert.h>
#include <types.h>

struct soCollisionSearchData {
    char _0[0xC];
    float m_size;
    char _10[0x8];
};
static_assert(sizeof(soCollisionSearchData) == 0x18, "Class is wrong size!");
