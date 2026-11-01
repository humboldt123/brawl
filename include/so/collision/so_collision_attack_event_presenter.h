#pragma once
// SHADOW of BrawlHeaders/so/collision/so_collision_attack_event_presenter.h: addObserver is inline in the fighter RELs (FT_MODULE_BUILDER)

#include <StaticAssert.h>
#include <so/event/so_event_presenter.h>
#include <so/collision/so_collision_log.h>
#include <so/so_instance_unit.h>
#include <types.h>

class soModuleAccesser;

class soCollisionAttackEventObserver : public soEventObserver<soCollisionAttackEventObserver> {
public:
    typedef u16 AttributeMask;
    static const AttributeMask ATTRIBUTE_MASK_NONE = 0;
    struct AttributeFlag {
        union {
            struct {
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
            };
            AttributeMask m_mask;
        };
        inline AttributeFlag() : m_mask(ATTRIBUTE_MASK_NONE) {}
        inline AttributeFlag(AttributeMask bits) : m_mask(bits) {}
        inline ~AttributeFlag() {}

        AttributeFlag(soAttributeFlag f) : m_mask(f.m_mask) { }
        operator soAttributeFlag() { return soAttributeFlag(m_mask); }
    };

    soCollisionAttackEventObserver(short unitID) : soEventObserver<soCollisionAttackEventObserver>(unitID) {};
    // (manageId, p2) constructor of the generic article base (observer unit 1)
    soCollisionAttackEventObserver(short manageId, s8 p2) : soEventObserver<soCollisionAttackEventObserver>(0x1) { initialize(manageId, p2); }

#if defined(FT_MODULE_BUILDER) || defined(YK_STAGE_FULL) || defined(SO_EVENT_OBSERVER_INLINE)
    virtual void addObserver(short param1, s8 param2) { addObserverSub(param1, this, param2); } // BYTES: the owning unit's weak copy; the #else units call the sora_melee instance
#else
    virtual void addObserver(short param1, s8 param2);
#endif
#ifdef SO_EVENT_OBSERVER_DEFAULTS_INLINE
    // Opt-in per unit (the Enemy): its observer vtable has a null slot for the handler and a weak copy of the default
    // check, so the original header declared the one pure and defined the other inline.
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) = 0;
    virtual bool notifyEventCollisionAttackCheck(u32 flags) { return false; }
#else
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
#endif
};
static_assert(sizeof(soCollisionAttackEventObserver) == 12, "Class is wrong size!");
