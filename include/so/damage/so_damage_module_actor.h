#pragma once

// soDamageModuleActor (sora_melee so_damage_module_actor.o), the soDamageModuleImpl of fighters and articles that keeps
// its own damage log. From its constructor, its virtual table and the log accessors. Not for units that include
// ft/builder/ft_dol_types.h, which declares an opaque soDamageModuleActor.

#include <StaticAssert.h>
#include <so/damage/so_damage.h>
#include <so/damage/so_damage_module_impl.h>
#include <types.h>

class soDamageModuleActor : public soDamageModuleImpl {
public:
    int unkAC;               // 0xAC: the constructor stores 0x18 here and at 0xB4
    int unkB0;               // 0xB0
    int unkB4;               // 0xB4
    int unkB8;               // 0xB8
    soDamageLog m_damageLog; // 0xBC

    soDamageModuleActor(soModuleAccesser* moduleAccesser, soArray<soDamage>* damageArray, void* effector, void* transactor, soEventObserverRegistrationDesc* registrationDesc);
    void toDamageFlyRollStatus(float angle, float speed);

    virtual ~soDamageModuleActor();
    virtual void activate(float damage);
    virtual soDamageLog* getDamageLog();
    virtual void clearDamageLog();
    virtual bool isCheckGroundDamage();
};
static_assert(sizeof(soDamageModuleActor) == 0x104, "Class is wrong size!");
