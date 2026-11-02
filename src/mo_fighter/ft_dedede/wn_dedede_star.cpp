#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC2F_CTOR_NOINLINE
#define WN_EXTEND_PARAM_ACCESSER_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/dedede/wn_dedede_star.h>
#include <wn/dedede/wn_dedede_follower_param_accesser.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_transactor.h>
#include <wn/dedede/wn_dedede_follower_kinetic_transactor.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_value_accesser.h>
#include <so/so_kinetic_utility.h>
#include <so/ground/so_ground_module_impl.h>
#include <ft/ft_kinetic_energy.h>
#include <math.h>

wnDededeStar::wnDededeStar(s32 articleId, const wnArticleBuildInfo& info, const wnDededeStarData* data) :
    wnWeaponBuilder<wnDededeStarModuleAccesserBuildConfig>(
        "DededeStar", articleId, info,
        wnModuleAccesserBuildData(0x31, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param) {
    Weapon::deactivate(false);
}

// The star is thrown from the position at the angle (degrees, mirrored when flipped) with the speed of 1.5 scaled by dir.
void wnDededeStar::activate(int founderTaskId, int unused, int team, const Vec3f* position, bool flip, float dir, float angle) {
    float lr;
    if (flip) {
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000000);
        angle = 180.0f - angle;
        lr = -1.0f;
    } else {
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000000);
        lr = 1.0f;
    }
    wnActivateDesc desc;
    s32 life = m_param->unk0;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = *position;
    desc.lr = lr;
    desc.team = team;
    desc.life = life;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    float sine;
    float speed = 1.5f;
    angle = 0.017453292f * angle;
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    sine = sin(angle);
    float cosine = cos(angle);
    Vec2f velocity(dir * (speed * cosine), speed * sine);
    normal.m_speed.m_x = velocity.m_x;
    normal.m_speed.m_y = velocity.m_y;
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

extern const float lbl_117_rodata_C8;

// HYPOTHESIS: the follower's kinetic energy pools (normal, gravity, normal, ground movement, wind): the transactor reaches them
// through an overload selected by an empty soIntToType<pool level> tag (as in the Diddy Kong barrel transactor); the
// value-initialized one-byte tag temporary is the zero byte stored before each lookup.
static inline soKineticEnergyNormal* getEnergy(void* pools, const soIntToType<0>&) {
    return reinterpret_cast<soKineticEnergyNormal*>(static_cast<u8*>(pools) + 0x10);
}
static inline wnKineticEnergyGravity* getEnergy(void* pools, const soIntToType<1>&) {
    return reinterpret_cast<wnKineticEnergyGravity*>(static_cast<u8*>(pools) + 0x50);
}

// HYPOTHESIS: the unit's out-of-line vector sum (unnamed in the map): writes a + b to out.
void addVec2(const Vec2f* a, const Vec2f* b, Vec2f* out) __attribute__((never_inline));
void addVec2(const Vec2f* a, const Vec2f* b, Vec2f* out) {
    out->m_x = a->m_x + b->m_x;
    out->m_y = a->m_y + b->m_y;
}

static inline Vec2f addVec2f(const Vec2f& a, const Vec2f& b) {
    Vec2f sum;
    addVec2(&a, &b, &sum);
    return sum;
}

// MATCH-ONLY: this unit's other users call the out-of-line Vec2f constructor; the transactor builds its temporaries inline.
struct InlineVec2f : Vec2f {
    InlineVec2f(float x, float y) {
        m_x = x;
        m_y = y;
    }
};
static inline soKineticEnergyNormal* getEnergy(void* pools, const soIntToType<2>&) {
    return reinterpret_cast<soKineticEnergyNormal*>(static_cast<u8*>(pools) + 0x70);
}
static inline soKineticEnergyGroundMovement* getEnergy(void* pools, const soIntToType<3>&) {
    return reinterpret_cast<soKineticEnergyGroundMovement*>(static_cast<u8*>(pools) + 0xB0);
}
static inline soKineticEnergyWindNormal* getEnergy(void* pools, const soIntToType<4>&) {
    return reinterpret_cast<soKineticEnergyWindNormal*>(static_cast<u8*>(pools) + 0xEC);
}

// HYPOTHESIS: the follower takes outside movement (moving ground, wind, hits) into its ground movement, wind and normal energies.
void wnDededeFollowerKineticTransactor::addSpeedOutside(int type, void* speedData, void* pools, soModuleAccesser* moduleAccesser) {
    Vec2f* speed = static_cast<Vec2f*>(speedData);
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    switch (type) {
    case soKineticEnergy::Outside_Movement_Ground: {
        Vec2f outside;
        outside.m_x = 0.0f;
        outside.m_y = 0.0f;
        switch (moduleAccesser->getSituationModule().getKind()) {
        case Situation_Ground: {
            Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(8, 0);
            outside = soKineticUtility::projectionNormalFollow(&InlineVec2f(speed->m_x, 0.0f), &normal);
            break;
        }
        }
        if (0.0f != outside.m_x || 0.0f != outside.m_y) {
            soKineticEnergyGroundMovement* ground = getEnergy(pools, soIntToType<3>());
            addVec2(&ground->m_outsideSpeed, &outside, &ground->m_outsideSpeed);
            // MATCH-ONLY: the original adds a zero it loads from the pool (a rotation input the follower never sets).
            ground->m_rotation = ground->m_rotation + lbl_117_rodata_C8;
            ground->enable();
        }
        break;
    }
    case soKineticEnergy::Outside_Wind: {
        Vec2f delta;
        delta.m_x = 0.0f;
        delta.m_y = 0.0f;
        Vec2f brake;
        brake.m_x = 0.0f;
        brake.m_y = 0.0f;
        switch (moduleAccesser->getSituationModule().getKind()) {
        case Situation_Ground:
            delta.m_x = speed->m_x;
            brake.m_x = paramAccesser->getParamFloat(moduleAccesser, 0xFAE, 0);
            break;
        case Situation_Air:
        case Situation_Water:
            delta.m_x = speed->m_x;
            delta.m_y += speed->m_y;
            brake.m_x = paramAccesser->getParamFloat(moduleAccesser, 0xFAE, 0);
            break;
        }
        if (0.0f != delta.m_x || 0.0f != delta.m_y) {
            soKineticEnergyWindNormal* wind = getEnergy(pools, soIntToType<4>());
            addVec2(&wind->m_windSpeed, &delta, &wind->m_windSpeed);
            wind->m_responseRetention = paramAccesser->getParamFloat(moduleAccesser, 0xFB5, 0);
            wind->m_brake = brake;
            wind->enable();
        }
        break;
    }
    case soKineticEnergy::Outside_Attack: {
        Vec2f push;
        push.m_x = 0.0f;
        push.m_y = 0.0f;
        switch (moduleAccesser->getSituationModule().getKind()) {
        case Situation_Ground:
            push.m_x = speed->m_x;
            break;
        case Situation_Air:
        case Situation_Water:
            push.m_x = speed->m_x;
            push.m_y = speed->m_y;
            break;
        }
        if (0.0f != push.m_x) {
            soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<2>());
            normal->m_speed = addVec2f(normal->getSpeed(), push);
            normal->m_speedTarget = InlineVec2f(0.0f, 0.0f);
            if (moduleAccesser->getSituationModule().getKind() == Situation_Air) {
                normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAF, 0), 0.0f);
            } else {
                normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAE, 0), 0.0f);
            }
            normal->enable();
        }
        if (0.0f != push.m_y) {
            wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
            gravity->m_speedY = push.m_y + gravity->getSpeed().m_y;
            if (moduleAccesser->getKineticModule().getEnergy(1)->isEnable()) {
                gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFB1, 0);
                gravity->m_speedLimit = paramAccesser->getParamFloat(moduleAccesser, 0xFB2, 0);
                gravity->enable();
            }
        }
        break;
    }
    }
}

void wnDededeFollowerKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser) {
    if (mode <= 0x1F) {
        wnKineticTransactor::changeKinetic(mode, pools, moduleAccesser);
        return;
    }
    switch (mode) {
    case 0x20: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub(pools, moduleAccesser);
        break;
    }
    case 0x21: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub1(pools, moduleAccesser);
        break;
    }
    case 0x22: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub2(pools, moduleAccesser);
        break;
    }
    case 0x23: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub3(pools, moduleAccesser);
        break;
    }
    case 0x24: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub4(pools, moduleAccesser);
        break;
    }
    case 0x25: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub5(pools, moduleAccesser);
        break;
    }
    case 0x26: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub6(pools, moduleAccesser);
        break;
    }
    case 0x27: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub7(pools, moduleAccesser);
        break;
    }
    case 0x28: {
        wnDededeFollowerKineticTransactor empty = {};
        break;
    }
    case 0x29: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub8(pools, moduleAccesser);
        break;
    }
    case 0x2a: {
        wnDededeFollowerKineticTransactor empty = {};
        moduleAccesser->getKineticModule().unableEnergyAll();
        break;
    }
    case 0x2b: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub9(pools, moduleAccesser);
        break;
    }
    case 0x2c: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub10(pools, moduleAccesser);
        break;
    }
    case 0x2d: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub11(pools, moduleAccesser);
        break;
    }
    case 0x2e: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub12(pools, moduleAccesser);
        break;
    }
    case 0x2f: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub13(pools, moduleAccesser);
        break;
    }
    case 0x30: {
        wnDededeFollowerKineticTransactor empty = {};
        empty.changeKineticSub14(pools, moduleAccesser);
        break;
    }
    }
}

// HYPOTHESIS: launches along the angle (work float 0x21000005, scaled to radians) at the speed in work float 0x21000006.
void wnDededeFollowerKineticTransactor::changeKineticSub(void* pools, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    float angle = 0.017453292f * moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    float launchSpeed = moduleAccesser->getWorkManageModule().getFloat(0x21000006);
    float lr = moduleAccesser->getPostureModule().getLr();
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    normal->m_speed = InlineVec2f(lr * (launchSpeed * (float)cos(angle)), 0.0f);
    normal->m_speedTarget = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFB0, 0), 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAF, 0), 0.0f);
    normal->enable();
    wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_speedY = launchSpeed * (float)sin(angle);
    gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFB1, 0);
    gravity->m_speedLimit = paramAccesser->getParamFloat(moduleAccesser, 0xFB2, 0);
    gravity->enable();
}

// HYPOTHESIS: bounces off with the current speed reversed and damped (0xFB3), rising by 0xFB4 on top of it.
void wnDededeFollowerKineticTransactor::changeKineticSub1(void* pools, soModuleAccesser* moduleAccesser) {
    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    normal->m_speed = InlineVec2f(-speed.m_x * paramAccesser->getParamFloat(moduleAccesser, 0xFB3, 0), 0.0f);
    normal->m_speedTarget = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFB0, 0), 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAF, 0), 0.0f);
    normal->enable();
    wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_speedY = speed.m_y + paramAccesser->getParamFloat(moduleAccesser, 0xFB4, 0);
    gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFB1, 0);
    gravity->m_speedLimit = paramAccesser->getParamFloat(moduleAccesser, 0xFB2, 0);
    gravity->enable();
}

// HYPOTHESIS: keeps the horizontal speed it had, but never less than the 0xFA9 limit (towards the facing side).
void wnDededeFollowerKineticTransactor::changeKineticSub2(void* pools, soModuleAccesser* moduleAccesser) {
    Vec2f speed = addVec2f(moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)),
                           moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(2)));
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    float lr = moduleAccesser->getPostureModule().getLr();
    float limit = lr * paramAccesser->getParamFloat(moduleAccesser, 0xFA9, 0);
    if (limit < 0.0f) {
        if (speed.m_x < limit) {
            limit = speed.m_x;
        }
    } else {
        if (speed.m_x > limit) {
            limit = speed.m_x;
        }
    }
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    normal->m_speed = InlineVec2f(limit, 0.0f);
    normal->m_speedTarget = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFA9, 0), 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAE, 0), 0.0f);
    normal->enable();
}

// HYPOTHESIS: stops dead and brakes by 0xFAE.
void wnDededeFollowerKineticTransactor::changeKineticSub3(void* pools, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    normal->m_speed = InlineVec2f(0.0f, 0.0f);
    normal->m_speedTarget = InlineVec2f(0.0f, 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAE, 0), 0.0f);
    normal->enable();
}

// HYPOTHESIS: keeps its horizontal speed and brakes by 0xFAE.
void wnDededeFollowerKineticTransactor::changeKineticSub4(void* pools, soModuleAccesser* moduleAccesser) {
    Vec2f speed = addVec2f(moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)),
                           moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(2)));
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    normal->m_speed = InlineVec2f(speed.m_x, 0.0f);
    normal->m_speedTarget = InlineVec2f(0.0f, 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAE, 0), 0.0f);
    normal->enable();
}

// HYPOTHESIS: keeps both components of the speed (gravity takes the vertical one).
void wnDededeFollowerKineticTransactor::changeKineticSub5(void* pools, soModuleAccesser* moduleAccesser) {
    Vec2f speed = addVec2f(moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)),
                           moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(2)));
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    normal->m_speed = InlineVec2f(speed.m_x, 0.0f);
    normal->m_speedTarget = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFB0, 0), 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAF, 0), 0.0f);
    normal->enable();
    wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_speedY = speed.m_y;
    gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFB1, 0);
    gravity->m_speedLimit = paramAccesser->getParamFloat(moduleAccesser, 0xFB2, 0);
    gravity->enable();
}

// HYPOTHESIS: adds a push of 0xFAC (towards the facing side) to the horizontal speed and launches upwards at 0xFAD.
void wnDededeFollowerKineticTransactor::changeKineticSub6(void* pools, soModuleAccesser* moduleAccesser) {
    Vec2f speed = addVec2f(moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)),
                           moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(2)));
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    float lr = moduleAccesser->getPostureModule().getLr();
    float pushX = lr * paramAccesser->getParamFloat(moduleAccesser, 0xFAC, 0);
    pushX += speed.m_x;
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    normal->m_speed = InlineVec2f(pushX, 0.0f);
    normal->m_speedTarget = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFB0, 0), 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAF, 0), 0.0f);
    normal->enable();
    wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_speedY = paramAccesser->getParamFloat(moduleAccesser, 0xFAD, 0);
    gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFB1, 0);
    gravity->m_speedLimit = -paramAccesser->getParamFloat(moduleAccesser, 0xFB2, 0);
    gravity->enable();
}

// HYPOTHESIS: the same as Sub6 with the 0xFA0/0xFA1 launch parameters.
void wnDededeFollowerKineticTransactor::changeKineticSub7(void* pools, soModuleAccesser* moduleAccesser) {
    Vec2f speed = addVec2f(moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)),
                           moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(2)));
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    float lr = moduleAccesser->getPostureModule().getLr();
    float pushX = lr * paramAccesser->getParamFloat(moduleAccesser, 0xFA0, 0);
    pushX += speed.m_x;
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    normal->m_speed = InlineVec2f(pushX, 0.0f);
    normal->m_speedTarget = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFB0, 0), 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAF, 0), 0.0f);
    normal->enable();
    wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_speedY = paramAccesser->getParamFloat(moduleAccesser, 0xFA1, 0);
    gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFB1, 0);
    gravity->m_speedLimit = -paramAccesser->getParamFloat(moduleAccesser, 0xFB2, 0);
    gravity->enable();
}

// HYPOTHESIS: launches the second normal energy (work floats 0x21000002/3 are its horizontal and the gravity energy's vertical speed).
void wnDededeFollowerKineticTransactor::changeKineticSub8(void* pools, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    Vec2f launchSpeed;
    launchSpeed.m_x = moduleAccesser->getWorkManageModule().getFloat(0x21000002);
    launchSpeed.m_y = moduleAccesser->getWorkManageModule().getFloat(0x21000003);
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<2>());
    normal->m_speed = InlineVec2f(launchSpeed.m_x, 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAF, 0), 0.0f);
    normal->enable();
    wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_speedY = launchSpeed.m_y;
    gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFB1, 0);
    gravity->m_speedLimit = -paramAccesser->getParamFloat(moduleAccesser, 0xFB2, 0);
    gravity->enable();
}

// HYPOTHESIS: falls straight down from rest.
void wnDededeFollowerKineticTransactor::changeKineticSub9(void* pools, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_speedY = 0.0f;
    gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFB1, 0);
    gravity->m_speedLimit = -paramAccesser->getParamFloat(moduleAccesser, 0xFB2, 0);
    gravity->enable();
}

// HYPOTHESIS: keeps the horizontal speed, but at least the 0xFB6 limit towards the facing side.
void wnDededeFollowerKineticTransactor::changeKineticSub10(void* pools, soModuleAccesser* moduleAccesser) {
    Vec2f speed = addVec2f(moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)),
                           moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(2)));
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    float lr = moduleAccesser->getPostureModule().getLr();
    float limit = lr * paramAccesser->getParamFloat(moduleAccesser, 0xFB6, 0);
    if (limit < 0.0f) {
        if (speed.m_x < limit) {
            limit = speed.m_x;
        }
    } else {
        if (speed.m_x > limit) {
            limit = speed.m_x;
        }
    }
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    normal->m_speed = InlineVec2f(limit, 0.0f);
    normal->m_speedTarget = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFB6, 0), 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAE, 0), 0.0f);
    normal->enable();
}

// HYPOTHESIS: the same with the 0xFB7 limit, braking by 0xFAF, then launching upwards at 0xFAD.
void wnDededeFollowerKineticTransactor::changeKineticSub11(void* pools, soModuleAccesser* moduleAccesser) {
    Vec2f speed = addVec2f(moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)),
                           moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(2)));
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    float lr = moduleAccesser->getPostureModule().getLr();
    float limit = lr * paramAccesser->getParamFloat(moduleAccesser, 0xFB7, 0);
    if (limit < 0.0f) {
        if (speed.m_x < limit) {
            limit = speed.m_x;
        }
    } else {
        if (speed.m_x > limit) {
            limit = speed.m_x;
        }
    }
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    normal->m_speed = InlineVec2f(limit, 0.0f);
    normal->m_speedTarget = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFB7, 0), 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAF, 0), 0.0f);
    normal->enable();
    wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_speedY = paramAccesser->getParamFloat(moduleAccesser, 0xFAD, 0);
    gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFB1, 0);
    gravity->m_speedLimit = -paramAccesser->getParamFloat(moduleAccesser, 0xFB2, 0);
    gravity->enable();
}

// HYPOTHESIS: stops horizontally and falls from rest, braking by 0xFAF.
void wnDededeFollowerKineticTransactor::changeKineticSub12(void* pools, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    normal->m_speed = InlineVec2f(0.0f, 0.0f);
    normal->m_speedTarget = InlineVec2f(0.0f, 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAF, 0), 0.0f);
    normal->enable();
    wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_speedY = 0.0f;
    gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFB1, 0);
    gravity->m_speedLimit = -paramAccesser->getParamFloat(moduleAccesser, 0xFB2, 0);
    gravity->enable();
}

// HYPOTHESIS: like Sub11, keeping the vertical speed instead of launching upwards.
void wnDededeFollowerKineticTransactor::changeKineticSub13(void* pools, soModuleAccesser* moduleAccesser) {
    Vec2f speed = addVec2f(moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)),
                           moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(2)));
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    float lr = moduleAccesser->getPostureModule().getLr();
    float limit = lr * paramAccesser->getParamFloat(moduleAccesser, 0xFB7, 0);
    if (limit < 0.0f) {
        if (speed.m_x < limit) {
            limit = speed.m_x;
        }
    } else {
        if (speed.m_x > limit) {
            limit = speed.m_x;
        }
    }
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    normal->m_speed = InlineVec2f(limit, 0.0f);
    normal->m_speedTarget = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFB7, 0), 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAF, 0), 0.0f);
    normal->enable();
    wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_speedY = speed.m_y;
    gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFB1, 0);
    gravity->m_speedLimit = -paramAccesser->getParamFloat(moduleAccesser, 0xFB2, 0);
    gravity->enable();
}

// HYPOTHESIS: stops horizontally and keeps the vertical speed.
void wnDededeFollowerKineticTransactor::changeKineticSub14(void* pools, soModuleAccesser* moduleAccesser) {
    Vec2f speed = addVec2f(moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)),
                           moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(2)));
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(moduleAccesser->getWorkManageModule().getParamAccesser());
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    normal->m_speed = InlineVec2f(0.0f, 0.0f);
    normal->m_speedTarget = InlineVec2f(0.0f, 0.0f);
    normal->m_brake = InlineVec2f(paramAccesser->getParamFloat(moduleAccesser, 0xFAF, 0), 0.0f);
    normal->enable();
    wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_speedY = speed.m_y;
    gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFB1, 0);
    gravity->m_speedLimit = -paramAccesser->getParamFloat(moduleAccesser, 0xFB2, 0);
    gravity->enable();
}

wnDededeFollowerParamAccesser::wnDededeFollowerParamAccesser(const wnParamDededeFollower* param,
                                                             const wnParamDededeFollowerCommon* common) :
    wnExtendParamAccesser<3999, 27, 23999, 16, 0, 0>(0x33), m_common(common, 3) {
    m_floats[0] = &param->unkC;
    m_floats[1] = &param->unk10;
    m_floats[2] = &param->unk14;
    m_floats[3] = &param->unk18;
    m_floats[4] = &param->unk24;
    m_floats[5] = &param->unk28;
    m_floats[6] = &param->unk30;
    m_floats[7] = &param->unk34;
    m_ints[0] = &param->unk0;
    m_ints[1] = &param->unk4;
    m_ints[2] = &param->unk8;
    m_ints[3] = &param->unk1C;
    m_ints[4] = &param->unk20;
    m_ints[5] = &param->unk2C;
}

// MATCH-ONLY: the native queries call the const element accessor of the table.
#define DEDEDE_FOLLOWER_COMMON(variation)     static_cast<const soArrayContractibleTable<const wnParamDededeFollowerCommon>&>(m_common).at(variation)

float wnDededeFollowerParamAccesser::getParamFloat(soModuleAccesser* acc, s32 id, s32 index) {
    float result;
    if (id < 0xFA8) {
        result = wnExtendParamAccesser<3999, 27, 23999, 16, 0, 0>::getParamFloat(acc, id, index);
    } else {
        u32 variation = acc->getWorkManageModule().getInt(0x10000005);
        switch (id) {
        case 0xFA9:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk0;
            break;
        case 0xFAA:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk4;
            break;
        case 0xFAB:
            result = DEDEDE_FOLLOWER_COMMON(variation).unkC;
            break;
        case 0xFAC:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk24;
            break;
        case 0xFAD:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk28;
            break;
        case 0xFAE:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk2C;
            break;
        case 0xFAF:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk30;
            break;
        case 0xFB0:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk34;
            break;
        case 0xFB1:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk38;
            break;
        case 0xFB2:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk3C;
            break;
        case 0xFB3:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk40;
            break;
        case 0xFB4:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk44;
            break;
        case 0xFB5:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk48;
            break;
        case 0xFB6:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk4C;
            break;
        case 0xFB7:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk50;
            break;
        case 0xFB8:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk54;
            break;
        case 0xFB9:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk5C;
            break;
        case 0xFBA:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk60;
            break;
        default:
            result = 0.0f;
            break;
        }
    }
    return result;
}

s32 wnDededeFollowerParamAccesser::getParamInt(soModuleAccesser* acc, s32 id, s32 index) {
    s32 result;
    if (id < 0x5DC6) {
        result = wnExtendParamAccesser<3999, 27, 23999, 16, 0, 0>::getParamInt(acc, id, index);
    } else {
        u32 variation = acc->getWorkManageModule().getInt(0x10000005);
        switch (id) {
        case 0x5DC7:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk8;
            break;
        case 0x5DC8:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk10;
            break;
        case 0x5DC9:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk14;
            break;
        case 0x5DCA:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk18;
            break;
        case 0x5DCB:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk1C;
            break;
        case 0x5DCC:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk20;
            break;
        case 0x5DCD:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk58;
            break;
        case 0x5DCE:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk64;
            break;
        case 0x5DCF:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk68;
            break;
        default:
            result = 0;
            break;
        }
    }
    return result;
}
