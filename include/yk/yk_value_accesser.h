#pragma once

#include <StaticAssert.h>
#include <so/so_value_accesser.h>
#include <types.h>

class ykValueAccesser : public soValueAccesser {
public:
    ykValueAccesser();
    virtual soParamAccesser* getExtendParamAccesser();
    virtual ~ykValueAccesser();
};
static_assert(sizeof(ykValueAccesser) == 8, "Class is wrong size!");
