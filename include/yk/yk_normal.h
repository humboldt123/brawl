#pragma once

// A stage gimmick's hit object with both a hit module and a damage module (the "Normal" Yakumono): fighters hit it and
// it reports the damage to the gimmick (grYakumono::onDamage). Its attack module is the shared null module.

#include <ip/input.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/damage/so_damage.h>
#include <so/damage/so_damage_transactor_actor.h>
#include <so/damage/so_damage_transactor_null.h>
#include <yk/yk_no_hit_normal.h>
#include <yk/yakumono.h>

// sora_melee's null module singletons (names unknown).
extern u8 lbl_27_bss_384[];
extern u8 lbl_27_bss_5FF4[];

// The null damage transactor (so/damage/so_damage_transactor_null.h) answers "no reaction".

// sora_melee's damage module of the gimmicks (HYPOTHESIS: layout and parameters; the constructor and the destructor are
// sora_melee's own functions).
class ykDamageModuleImpl {
    char _4[0xA8];

public:
    ykDamageModuleImpl(soModuleAccesser* moduleAccesser, soArray<soDamage>* damages, void* unk, soDamageTransactor* transactor,
                       soEventObserverRegistrationDesc* registrationDesc);
    virtual ~ykDamageModuleImpl();
};
static_assert(sizeof(ykDamageModuleImpl) == 0xAC, "Class is wrong size!");

// The attack part of the build config is the null one: the Yakumono gets the shared null attack module.
struct soCollisionAttackModuleBuildConfigNull {
    int m_unused;
};

template <class TAttackConfig, class THitConfig>
class ykNormal;

// The damage half of the gimmick: one damage record per hit part and the damage module that uses them.
template <u32 N>
class ykDamageModuleBuilder {
    // MATCH-ONLY: the extra level makes the inlined destructor test the array's address once more, as the original does
    struct DamageArray {
        soArrayVector<soDamage, N> m_vector;
        DamageArray(s32 a, s32 b) : m_vector(a, b) { }
    };
    DamageArray m_damageArray;
    ykDamageModuleImpl m_damageModule;

public:
    ykDamageModuleBuilder(soModuleAccesser* moduleAccesser, void* unk, soEventObserverRegistrationDesc* registrationDesc)
        : m_damageArray(N, 0), m_damageModule(moduleAccesser, &m_damageArray.m_vector, unk, getNullTransactor(), registrationDesc) { }
    ykDamageModuleImpl* getModule() { return &m_damageModule; }

private:
    static soDamageTransactor* getNullTransactor() {
        static soDamageTransactorNull sNull;
        return &sNull;
    }
};

template <soCollision::Category Cat, u32 P, u32 G, class M, u32 Mask, bool b1>
class ykNormal<soCollisionAttackModuleBuildConfigNull, soCollisionHitModuleBuildConfig<Cat, P, G, M, Mask, b1> > : public Yakumono {
    typedef soCollisionHitModuleBuildConfig<Cat, P, G, M, Mask, b1> HitConfig;

    soCollisionAttackModuleBuildConfigNull m_attackConfig;
    HitConfig m_hitConfig; // the part arrays followed by the hit module (the module is a private member of the config)
    ykDamageModuleBuilder<G> m_damageBuilder;

    // MATCH-ONLY: the hit module sits after the three arrays of the build config
    M* hitModule() {
        return reinterpret_cast<M*>(reinterpret_cast<u8*>(&m_hitConfig) + sizeof(soArrayVector<soCollisionHitPart, P>) +
                                    sizeof(soArrayVector<soCollisionGroup, G>) + sizeof(soArrayVector<soCollisionHitGroup, G>));
    }

public:
    ykNormal(ykInitInfo* info)
        : Yakumono(info, "ykNormal", (soCollisionAttackModule*)lbl_27_bss_384, hitModule(), m_damageBuilder.getModule(),
                   lbl_27_bss_598, lbl_27_bss_444),
          m_hitConfig(&moduleAccesser, m_taskId, (gfTask::Category)(u8)m_taskCategory, lbl_27_data_54C60),
          m_damageBuilder(&moduleAccesser, lbl_27_bss_5FF4, lbl_27_data_54C60) {
        postInitialize();
        activate(info->m_pos, -1.0f, 0.0f);
    }
    virtual ~ykNormal() { }
};

// The same with an attack module of its own (the Yakumono both hits and is hit).
template <class TAttackConfig, soCollision::Category Cat, u32 P, u32 G, class M, u32 Mask, bool b1>
class ykNormal<TAttackConfig, soCollisionHitModuleBuildConfig<Cat, P, G, M, Mask, b1> > : public Yakumono {
    typedef soCollisionHitModuleBuildConfig<Cat, P, G, M, Mask, b1> HitConfig;

    TAttackConfig m_attackConfig;
    HitConfig m_hitConfig;
    ykDamageModuleBuilder<G> m_damageBuilder;

    // MATCH-ONLY: the hit module sits after the three arrays of the build config
    M* hitModule() {
        return reinterpret_cast<M*>(reinterpret_cast<u8*>(&m_hitConfig) + sizeof(soArrayVector<soCollisionHitPart, P>) +
                                    sizeof(soArrayVector<soCollisionGroup, G>) + sizeof(soArrayVector<soCollisionHitGroup, G>));
    }

public:
    ykNormal(ykInitInfo* info)
        : Yakumono(info, "ykNormal", &m_attackConfig.m_attackModule, hitModule(), m_damageBuilder.getModule(),
                   lbl_27_bss_598, lbl_27_bss_444),
          m_attackConfig(&moduleAccesser, m_taskId, (gfTask::Category)(u8)m_taskCategory, lbl_27_data_54C60),
          m_hitConfig(&moduleAccesser, m_taskId, (gfTask::Category)(u8)m_taskCategory, lbl_27_data_54C60),
          m_damageBuilder(&moduleAccesser, lbl_27_bss_5FF4, lbl_27_data_54C60) {
        postInitialize();
        activate(info->m_pos, -1.0f, 0.0f);
    }
    virtual ~ykNormal() { }
};
