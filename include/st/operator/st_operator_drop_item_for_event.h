#pragma once

#include <StaticAssert.h>
#include <st/operator/st_operator_drop_item.h>
#include <types.h>

class stOperatorDropItemEvent : public stOperatorDropItemMelee {
public:
    float unk70;
    float unk74;
    u8 unk78;

    stOperatorDropItemEvent();
    virtual ~stOperatorDropItemEvent();
    virtual void processBegin();
    virtual void startOperator();
};
static_assert(sizeof(stOperatorDropItemEvent) == 0x7C, "Class is wrong size!");
