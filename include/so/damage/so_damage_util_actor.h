#pragma once

#include <StaticAssert.h>
#include <so/so_null.h>
#include <types.h>
#include <so/so_module_accesser.h>
#include <so/damage/so_damage.h>

class soDamageUtilActor : public soNullable {
    // TODO:
public:
    static int getDamageLevel(soModuleAccesser* moduleAccesser, float reaction);
    static bool checkDamageMeteor(soModuleAccesser* moduleAccesser, int attackVector);
    static float getDamageAngle(soModuleAccesser* moduleAccesser, double reaction, double lr, int attackVector, Vec2f *speed);
    static int getDamageFlyStatus(soModuleAccesser* moduleAccesser, float reaction, float angle);
    // The callee multiplies f1-f3 with fmuls and no frsp, so they are floats. The accesser comes first: Enemy, wnWarioBike
    // and wnNessYoyoHead evaluate it before the power argument (r3 before f1).
    static int calcHitStopFrame(soModuleAccesser* moduleAccesser, float power, float mul, float add, soCollisionAttackData*, int);
    // The factor comes before the flag: every caller loads the float argument before the bool.
    static int getDamageHitStopFrame(soModuleAccesser* moduleAccesser, soDamage* damage, float mul, bool unk);
};
