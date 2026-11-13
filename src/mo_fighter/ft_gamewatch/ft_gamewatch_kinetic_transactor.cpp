#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <ft/gamewatch/ft_gamewatch.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <math.h>

// Two blocks of Mr. Game & Watch's extend parameters: brake, gravity and fall limit of two of his own modes.
// HYPOTHESIS: the moves (the ExtendParam groups are not identified yet).
struct ftGameWatchKineticExtendA {
    float unk0;
    float m_brake;    // 0x4
    float unk8;
    float m_gravity;  // 0xC
    float m_unk1C;    // 0x10, copied to the gravity energy's 0x1C
};
struct ftGameWatchKineticExtendB {
    float unk0;
    float m_brake;   // 0x4
    float m_gravity; // 0x8
    float m_unk1C;   // 0xC
};
struct ftGameWatchKineticExtend {
    void* unk0;
    ftGameWatchKineticExtendA* m_a; // 0x4
    void* unk8;
    ftGameWatchKineticExtendB* m_b; // 0xC
};

// The fighter-specific block at 0xB8 of Mr. Game & Watch's fighter data (walk/run/air speeds of his own modes).
// HYPOTHESIS: the meaning of every field; names are offsets.
struct ftGameWatchKineticParam {
    float unk0;   // controller acceleration multiplier (mode 0x68)
    float unk4;   // controller 0x40 (mode 0x68)
    float unk8;   // controller target speed (mode 0x68)
    float unkC;   // brake, times constant 0xc38
    float unk10;  // speed along the facing direction, or the clamp of the entry speed
    float unk14;  // controller acceleration multiplier
    float unk18;  // controller 0x40
    float unk1C;  // controller target speed
    float unk20;
    float unk24;  // speed limit
    u8 unk28[0xC];
    float unk34;  // stick gain (mode 0x6b)
    float unk38;  // vertical speed (modes 0x6b, 0x6c)
    float unk3C;  // entry speed retention (mode 0x6b)
    float unk40;  // horizontal speed limit (mode 0x6b)
    float unk44;  // vertical speed with work flag 0x22000006 (mode 0x6b)
    float unk48;  // vertical speed multiplier (mode 0x6c)
    float unk4C;  // stick gain (mode 0x6c)
    u8 unk50[0x14];
    float unk64;  // gravity
    float unk68;  // fall speed limit
    float unk6C;  // gravity energy 0x18
    float unk70;  // gravity energy 0x1C
    float unk74;  // controller acceleration multiplier
    float unk78;  // controller 0x40
    float unk7C;  // controller target speed
    float unk80;  // controller brake
    float unk84;
    float unk88;  // controller speed limit
    u8 unk8C[0x100];
    float unk18C; // gravity (modes 0x70, 0x71)
    float unk190; // fall speed limit (modes 0x70, 0x71)
    float unk194; // controller acceleration multiplier (modes 0x70, 0x71)
    float unk198;
    float unk19C; // controller brake (modes 0x70, 0x71)
};

struct ftGameWatchData : public ftData {
    u8 unk94[0x24];
    ftGameWatchKineticParam* m_kineticParam; // 0xB8
};

static inline ftGameWatchKineticParam* getKineticParam() {
    return static_cast<ftGameWatchData*>(g_ftCommonDataAccesser.getData(Fighter_GameWatch))->m_kineticParam;
}

static inline ftGameWatchKineticExtend* getKineticExtend(soModuleAccesser* acc) {
    return static_cast<ftGameWatchKineticExtend*>(dynamic_cast<ftGameWatch&>(acc->getStageObject()).getExtendParam());
}

float clampf(float value, float min, float max);

// Mr. Game & Watch's kinetic modes 0x64..0x72; the lower modes go to the common transactor.
// HYPOTHESIS: which statuses use these modes.
// MATCH-ONLY: the accesser copy and the shared flag local order the argument setup (as in the other fighters' transactors).
#pragma push
#pragma opt_propagation off
void ftGameWatchKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* acc) {
    if (mode <= 0x63) {
        ftKineticTransactor::changeKinetic(mode, pools, acc);
        return;
    }
    soModuleAccesser* processAccesser = acc;
    int process = 1;
    Vec2f speed = ftKineticTransactHelper::preHelpProcess(processAccesser, process, process);
    switch (mode) {
#define FT_GAMEWATCH_KINETIC_CASE(Mode, Name) \
    case Mode: {                              \
        bool result = false;                  \
        Name(&result, pools, &speed, acc);    \
        break;                                \
    }
    FT_GAMEWATCH_KINETIC_CASE(0x64, changeKineticSub)
    FT_GAMEWATCH_KINETIC_CASE(0x65, changeKineticSub1)
    FT_GAMEWATCH_KINETIC_CASE(0x66, changeKineticSub2)
    FT_GAMEWATCH_KINETIC_CASE(0x67, changeKineticSub3)
    FT_GAMEWATCH_KINETIC_CASE(0x68, changeKineticSub4)
    FT_GAMEWATCH_KINETIC_CASE(0x69, changeKineticSub5)
    FT_GAMEWATCH_KINETIC_CASE(0x6a, changeKineticSub6)
    FT_GAMEWATCH_KINETIC_CASE(0x6b, changeKineticSub7)
    FT_GAMEWATCH_KINETIC_CASE(0x6c, changeKineticSub8)
    FT_GAMEWATCH_KINETIC_CASE(0x6d, changeKineticSub9)
    FT_GAMEWATCH_KINETIC_CASE(0x6e, changeKineticSub10)
    FT_GAMEWATCH_KINETIC_CASE(0x6f, changeKineticSub11)
    FT_GAMEWATCH_KINETIC_CASE(0x70, changeKineticSub12)
    FT_GAMEWATCH_KINETIC_CASE(0x71, changeKineticSub13)
    FT_GAMEWATCH_KINETIC_CASE(0x72, changeKineticSub14)
#undef FT_GAMEWATCH_KINETIC_CASE
    default:
        return;
    }
    ftKineticTransactor::enableOutsideEnergy(acc);
}
#pragma pop

// Gravity keeps the vertical speed of the entry and the stop energy (mode 9) its horizontal speed, with the brake of extend
// block A.
void ftGameWatchKineticTransactor::changeKineticSub(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    Vec2f entry = *speed;
    ftGameWatchKineticExtendA* extend = getKineticExtend(acc)->m_a;
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    stop.getSpeed();
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, entry.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, acc, 9, &Vec2f(stop.getSpeed().m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    stop.m_brake = Vec2f(extend->m_brake, 0.0f);
}

// As mode 0x64 from the entry speed, with the gravity of extend block A.
void ftGameWatchKineticTransactor::changeKineticSub1(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    Vec2f entry = *speed;
    ftGameWatchKineticExtendA* extend = getKineticExtend(acc)->m_a;
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, entry.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, acc, 9, &Vec2f(entry.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    gravity.m_gravity = -extend->m_gravity;
    gravity.unk1C = extend->m_unk1C;
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    stop.m_brake = Vec2f(extend->m_brake, 0.0f);
}

// Restarts from the current total speed with every energy off, then as mode 0x65 with extend block B.
void ftGameWatchKineticTransactor::changeKineticSub2(bool*, void*, Vec2f*, soModuleAccesser* acc) {
    Vec2f speed = acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    acc->getKineticModule().unableEnergyAll();
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    stop.getSpeed();
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, acc, 9, &Vec2f(stop.getSpeed().m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    ftGameWatchKineticExtendB* extend = getKineticExtend(acc)->m_b;
    gravity.m_gravity = -extend->m_gravity;
    gravity.unk1C = extend->m_unk1C;
    stop.m_brake = Vec2f(extend->m_brake, 0.0f);
}

// Controller mode 5: the horizontal speed is set along the facing direction (or added when moving against it).
void ftGameWatchKineticTransactor::changeKineticSub3(bool*, void*, Vec2f*, soModuleAccesser* acc) {
    soKineticUtility::resetEnableEnergy(2, acc, 5, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyController& controller =
        dynamic_cast<ftKineticEnergyController&>(*acc->getKineticModule().getEnergy(2));
    ftGameWatchKineticParam* param = getKineticParam();
    float lr = acc->getPostureModule().getLr();
    float add = param->unk10;
    float x = controller.getSpeed().m_x;
    if (x * lr >= 0.0f) {
        x = lr * add;
    } else {
        x += lr * add;
    }
    controller.m_speed = Vec2f(x, 0.0f);
    controller.m_speedTarget = Vec2f(param->unk1C, -1.0f);
    controller.m_speedLimit = Vec2f(param->unk24, 0.0f);
    float brake = param->unkC;
    controller.m_brake = Vec2f(brake * soValueAccesser::getConstantFloat(acc, 0xc38, 0), 0.0f);
    controller.m_accelMul = param->unk14;
    controller.m_unk40 = param->unk18;
}

// Controller mode 0xa from the entry speed.
void ftGameWatchKineticTransactor::changeKineticSub4(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    soKineticUtility::resetEnableEnergy(2, acc, 0xa, speed, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyController& controller =
        dynamic_cast<ftKineticEnergyController&>(*acc->getKineticModule().getEnergy(2));
    ftGameWatchKineticParam* param = getKineticParam();
    controller.m_speedTarget = Vec2f(param->unk8, -1.0f);
    controller.m_speedLimit = Vec2f(param->unk24, 0.0f);
    controller.m_brake = Vec2f(param->unkC * soValueAccesser::getConstantFloat(acc, 0xc38, 0), 0.0f);
    controller.m_accelMul = param->unk0;
    controller.m_unk40 = param->unk4;
}

// As mode 0x67 with controller mode 8 from the entry speed.
void ftGameWatchKineticTransactor::changeKineticSub5(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    soKineticUtility::resetEnableEnergy(2, acc, 8, speed, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyController& controller =
        dynamic_cast<ftKineticEnergyController&>(*acc->getKineticModule().getEnergy(2));
    ftGameWatchKineticParam* param = getKineticParam();
    float lr = acc->getPostureModule().getLr();
    float add = param->unk10;
    float x = controller.getSpeed().m_x;
    if (x * lr >= 0.0f) {
        x = lr * add;
    } else {
        x += lr * add;
    }
    controller.m_speed = Vec2f(x, 0.0f);
    controller.m_speedTarget = Vec2f(param->unk1C, -1.0f);
    controller.m_speedLimit = Vec2f(param->unk24, 0.0f);
    float brake = param->unkC;
    controller.m_brake = Vec2f(brake * soValueAccesser::getConstantFloat(acc, 0xc38, 0), 0.0f);
    controller.m_accelMul = param->unk14;
    controller.m_unk40 = param->unk18;
}

// Stop energy mode 0x12 from the entry speed clamped to the facing speed.
void ftGameWatchKineticTransactor::changeKineticSub6(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    ftGameWatchKineticParam* param = getKineticParam();
    Vec2f entry = *speed;
    float max = param->unk10;
    entry.m_x = clampf(entry.m_x, -max, max);
    soKineticUtility::resetEnableEnergy(3, acc, 0x12, &entry, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    float brake = param->unkC;
    stop.m_brake = Vec2f(brake * soValueAccesser::getConstantFloat(acc, 0xc38, 0), 0.0f);
    stop.m_speedLimit = Vec2f(param->unk24, 0.0f);
}

// MATCH-ONLY: the fighter units are built without inlining of the nw4r FSelect helper.
float clampf(register float value, register float min, register float max) {
    register float t;
    register float result;
    asm {
        fsubs t, value, min
        fsel result, t, value, min
        fsubs t, result, max
        fsel result, t, max, result
    }
    return result;
}

// Air drift from the stick: the entry speed is kept by a factor and capped, the vertical speed depends on work flag
// 0x22000006, then gravity and controller take the parameters of the air modes.
void ftGameWatchKineticTransactor::changeKineticSub7(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    Vec2f entry = *speed;
    ftGameWatchKineticParam* param = getKineticParam();
    float gain = param->unk34;
    entry.m_x = entry.m_x * param->unk3C + gain * acc->getControllerModule().getStickX();
    float max = param->unk40;
    if ((float)fabs(entry.m_x) > max) {
        if (entry.m_x < 0.0f) {
            max = -max;
        }
        entry.m_x = max;
    }
    if (acc->getWorkManageModule().isFlag(0x22000006)) {
        entry.m_y = param->unk44 + entry.m_y * soValueAccesser::getConstantFloat(acc, 0xc46, 0);
    } else {
        entry.m_y = param->unk38 + entry.m_y * soValueAccesser::getConstantFloat(acc, 0xc46, 0);
    }
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, entry.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, acc, 0, &Vec2f(entry.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    gravity.m_gravity = -param->unk64;
    gravity.m_fallSpeedMax = param->unk68;
    gravity.unk18 = param->unk6C;
    gravity.unk1C = param->unk70;
    ftKineticEnergyController& controller =
        dynamic_cast<ftKineticEnergyController&>(*acc->getKineticModule().getEnergy(2));
    controller.m_speedTarget = Vec2f(param->unk7C, -1.0f);
    controller.m_brake = Vec2f(param->unk80, 0.0f);
    controller.m_speedLimit = Vec2f(param->unk88, 0.0f);
    controller.m_accelMul = param->unk74;
    controller.m_unk40 = param->unk78;
}

// As mode 0x6b with the horizontal speed from the stick only and a fixed vertical speed.
void ftGameWatchKineticTransactor::changeKineticSub8(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    Vec2f entry = *speed;
    ftGameWatchKineticParam* param = getKineticParam();
    float gain = param->unk4C;
    entry.m_x = gain * acc->getControllerModule().getStickX();
    float mul = param->unk48;
    entry.m_y = param->unk38 * mul;
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, entry.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, acc, 0, &Vec2f(entry.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    gravity.m_gravity = -param->unk64;
    gravity.m_fallSpeedMax = param->unk68;
    gravity.unk18 = param->unk6C;
    gravity.unk1C = param->unk70;
    ftKineticEnergyController& controller =
        dynamic_cast<ftKineticEnergyController&>(*acc->getKineticModule().getEnergy(2));
    controller.m_speedTarget = Vec2f(param->unk7C, -1.0f);
    controller.m_brake = Vec2f(param->unk80, 0.0f);
    controller.m_speedLimit = Vec2f(param->unk88, 0.0f);
    controller.m_accelMul = param->unk74;
    controller.m_unk40 = param->unk78;
}

// As mode 0x6b from the entry speed as it is.
void ftGameWatchKineticTransactor::changeKineticSub9(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    ftGameWatchKineticParam* param = getKineticParam();
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, acc, 0, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    gravity.m_gravity = -param->unk64;
    gravity.m_fallSpeedMax = param->unk68;
    gravity.unk18 = param->unk6C;
    gravity.unk1C = param->unk70;
    ftKineticEnergyController& controller =
        dynamic_cast<ftKineticEnergyController&>(*acc->getKineticModule().getEnergy(2));
    controller.m_speedTarget = Vec2f(param->unk7C, -1.0f);
    controller.m_brake = Vec2f(param->unk80, 0.0f);
    controller.m_speedLimit = Vec2f(param->unk88, 0.0f);
    controller.m_accelMul = param->unk74;
    controller.m_unk40 = param->unk78;
}

// Motion energy mode 2 from the entry speed with the ground brake.
void ftGameWatchKineticTransactor::changeKineticSub10(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    ftGameWatchKineticParam* param = getKineticParam();
    soKineticUtility::resetEnableEnergy(0, acc, 2, speed, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyMotion& motion = dynamic_cast<ftKineticEnergyMotion&>(*acc->getKineticModule().getEnergy(0));
    motion.m_brake = Vec2f(param->unkC, 0.0f);
}

// Motion energy mode 0 from the entry speed with the ground brake.
void ftGameWatchKineticTransactor::changeKineticSub11(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    ftGameWatchKineticParam* param = getKineticParam();
    soKineticUtility::resetEnableEnergy(0, acc, 0, speed, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyMotion& motion = dynamic_cast<ftKineticEnergyMotion&>(*acc->getKineticModule().getEnergy(0));
    motion.m_brake = Vec2f(param->unkC, 0.0f);
}

// Controller mode 0xf with the entry speed scaled by constant 0xd1b, gravity mode 2.
void ftGameWatchKineticTransactor::changeKineticSub12(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    ftGameWatchKineticParam* param = getKineticParam();
    Vec2f entry = *speed;
    entry.m_x *= soValueAccesser::getConstantFloat(acc, 0xd1b, 0);
    soKineticUtility::resetEnableEnergy(2, acc, 0xf, &Vec2f(entry.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(1, acc, 2, &Vec2f(0.0f, entry.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyController& controller =
        dynamic_cast<ftKineticEnergyController&>(*acc->getKineticModule().getEnergy(2));
    controller.m_brake = Vec2f(param->unk19C, 0.0f);
    controller.m_accelMul = param->unk194;
    controller.m_unk40 = 0.0f;
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    gravity.m_gravity = param->unk18C;
    gravity.m_fallSpeedMax = param->unk190;
    gravity.unk18 = param->unk6C;
    gravity.unk1C = param->unk70;
}

// As mode 0x70 from the entry speed as it is.
void ftGameWatchKineticTransactor::changeKineticSub13(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    ftGameWatchKineticParam* param = getKineticParam();
    soKineticUtility::resetEnableEnergy(2, acc, 0xf, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(1, acc, 2, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyController& controller =
        dynamic_cast<ftKineticEnergyController&>(*acc->getKineticModule().getEnergy(2));
    controller.m_brake = Vec2f(param->unk19C, 0.0f);
    controller.m_accelMul = param->unk194;
    controller.m_unk40 = 0.0f;
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    gravity.m_gravity = param->unk18C;
    gravity.m_fallSpeedMax = param->unk190;
    gravity.unk18 = param->unk6C;
    gravity.unk1C = param->unk70;
}

// As mode 0x6a with stop energy mode 0x10.
void ftGameWatchKineticTransactor::changeKineticSub14(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    ftGameWatchKineticParam* param = getKineticParam();
    Vec2f entry = *speed;
    float max = param->unk10;
    entry.m_x = clampf(entry.m_x, -max, max);
    soKineticUtility::resetEnableEnergy(3, acc, 0x10, &entry, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    float brake = param->unkC;
    stop.m_brake = Vec2f(brake * soValueAccesser::getConstantFloat(acc, 0xc38, 0), 0.0f);
    stop.m_speedLimit = Vec2f(param->unk24, 0.0f);
}
