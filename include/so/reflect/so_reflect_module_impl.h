#pragma once

// soReflectModuleImpl (sora_melee so_reflect_module_impl.o), from its constructor, its virtual table and the accessors.
// Not for units that include em/killer/em_killer.h or wn/wn_module_accesser_builder.h, which declare opaque copies.

#include <StaticAssert.h>
#include <so/collision/so_collision_attack_event_presenter.h>
#include <so/collision/so_collision_search_event_presenter.h>
#include <so/reflect/so_reflect_module.h>
#include <types.h>

class soModuleAccesser;

class soReflectModuleImpl : public soReflectModule,
                            public soCollisionAttackEventObserver,
                            public soCollisionSearchEventObserver {
public:
    int m_taskId;      // 0x24: -1 while nothing is reflected
    int m_team;        // 0x28
    float m_attackMul; // 0x2C
    float m_speedMul;  // 0x30
    float m_lifeMul;   // 0x34
    u32 m_count;       // 0x38: HYPOTHESIS: the reflect count (isCountMax compares it with a parameter)
    u8 unk3C;          // 0x3C
    bool m_noSpeedMul; // 0x3D

    soReflectModuleImpl(soModuleAccesser* moduleAccesser, int kind);
    virtual ~soReflectModuleImpl();
    virtual void activate();
    virtual void deactivate();
    virtual void initInfo();
    virtual void resetInfo();
    virtual int getTaskId();
    virtual int getTeam();
    virtual float getAttackMul();
    virtual void setAttackMul(float attackMul);
    virtual float getLifeMul();
    virtual bool isReflect();
    virtual float getSpeedMul();
    virtual bool isCountMax();
    virtual void setNoSpeedMul(bool noSpeedMul);
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(soReflectModuleImpl) == 0x40, "Class is wrong size!");
