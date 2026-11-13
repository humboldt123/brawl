#define FT_KINETIC_STOP_IMPLEMENTATION // BYTES: the energy constructor and assignment expand inline in the spore transactor
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define WN_ANIMCMD_STACK_PTR
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR
#define WN_PARAM_ACCESSER_INLINE_DTOR
#define MT_VEC3F_IMPLICIT_ASSIGN
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT
#define WN_PEACH_KINOPIO_UNIT
#define SO_COLLISION_GROUP_ALIGNED
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define WN_TRANSITION_CTOR_INLINE
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define WN_KINETIC_GENERIC_BUILDER
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/peach/wn_peach_kinopio.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <mt/mt_prng.h>
#include <math.h>

// HYPOTHESIS: the native speed stores expand this member setter.
inline void soKineticEnergyNormal::setSpeed(Vec2f* speed) {
    m_speed.m_x = speed->m_x;
    m_speed.m_y = speed->m_y;
}

// The modes without behavior of their own (the tags keep their switch arms apart, as in ftPeachKineticTransactor).
template <int N>
static inline void changeKineticHook(const soIntToType<N>&, void*, soModuleAccesser*) { }

// Toad's spore kinetic modes: 0x20 launches the spore's only energy in a random direction, and 0x20..0x24 run that
// energy. The modes below 0x20 are the common weapon ones.
// HYPOTHESIS: the roles of the constants 0xFA0 (base speed), 0xFA1 (speed spread), 0xFA2 (speed scale while the mode
// runs) and 0xFA3 (direction spread in degrees).
static inline void changeKineticSub(const soIntToType<0x20>&, void* pools, soModuleAccesser* acc) {
    Vec2f velocity;
    float speed = soValueAccesser::getConstantFloat(acc, 0xfa0, 0) + soValueAccesser::getConstantFloat(acc, 0xfa1, 0) * randf();
    // The side of the spread is drawn in the same expression (MWCC calls the right operand's randf() first).
    float angle = 0.5f * (0.017453292f * soValueAccesser::getConstantFloat(acc, 0xfa3, 0)) * randf() *
                  (randf() < 0.5f ? 1.0f : -1.0f);
    wnPeachKinopioSporeKineticPools* energies = static_cast<wnPeachKinopioSporeKineticPools*>(pools);
    *energies->getEnergy(soIntToType<0>()) = soKineticEnergyNormal(Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f),
                                                                   Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), false, false);
    // Both components are computed before either is stored.
    float y = speed * (float)sin(angle);
    float x = speed * (float)cos(angle);
    velocity.m_y = y;
    velocity.m_x = x;
    if (acc->getPostureModule().getLr() < 0.0f) {
        velocity.m_x = -velocity.m_x;
    }
    energies->getEnergy(soIntToType<0>())->setSpeed(&velocity);
    energies->getEnergy(soIntToType<0>())->enable();
}

// Mode 0x20 keeps scaling the spore's speed while it runs (0xFA2 is the per-frame factor).
// The helper is not only style: inlined here, the product temporary of operator* takes the stack slot above the copy
// that the assignment makes of it, as in the original; the same statements written inside updateEnergy's case put the
// two the other way round.
static inline void updateEnergySub(const soIntToType<0x20>&, soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    float scale = soValueAccesser::getConstantFloat(acc, 0xfa2, 0);
    energy->m_speed = energy->getSpeed() * scale;
}

void wnPeachKinopioSporeKineticTransactor::changeKinetic(int kineticType, void* pools, soModuleAccesser* acc) {
    if (kineticType <= 0x1f) {
        wnKineticTransactor::changeKinetic(kineticType, pools, acc);
        return;
    }
    switch (kineticType) {
    case 0x20:
        changeKineticSub(soIntToType<0x20>(), pools, acc);
        break;
    case 0x21:
        changeKineticHook(soIntToType<0x21>(), pools, acc);
        break;
    case 0x22:
        changeKineticHook(soIntToType<0x22>(), pools, acc);
        break;
    case 0x23:
        changeKineticHook(soIntToType<0x23>(), pools, acc);
        break;
    default:
        break;
    }
}

template <>
void wnPeachKinopioSporeKineticTransactor::updateEnergy(soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    if (!energy->isEnable() || energy->isSuspend() == true) {
        return;
    }
    switch (acc->getKineticModule().getKineticType()) {
    case 0x20:
        updateEnergySub(soIntToType<0x20>(), energy, acc);
        energy->updateEnergy(acc);
        break;
    case 0x21:
        wnKineticTransactor::updateEnergy(energy, acc);
        break;
    case 0x22:
        wnKineticTransactor::updateEnergy(energy, acc);
        break;
    case 0x23:
        wnKineticTransactor::updateEnergy(energy, acc);
        break;
    case 0x24:
        wnKineticTransactor::updateEnergy(energy, acc);
        break;
    }
}
