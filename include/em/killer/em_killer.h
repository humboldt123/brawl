#pragma once

// SHADOW of BrawlHeaders em/killer/em_killer.h (-I include comes first): the full layout of the Killer (Bullet Bill,
// enemy kind Enemy_Killer).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <so/reflect/so_reflect_module.h>
#include <types.h>

// The reflect module implementation of sora_melee (0x40 bytes; the Killer only calls its constructor and destructor).
// The article builders declare an opaque copy of it in wn/wn_module_accesser_builder.h, which enemy units cannot
// include.
class soReflectModuleImpl : public soReflectModule {
    u8 m_data[0x34];

public:
    soReflectModuleImpl(soModuleAccesser* acc, int kind);
    virtual ~soReflectModuleImpl();
    virtual void activate();
    virtual void deactivate();
    virtual void initInfo();
    virtual void resetInfo();
    virtual int getTaskId();
    virtual int getTeam();
    virtual float getAttackMul();
    virtual void setAttackMul(float);
    virtual float getLifeMul();
    virtual bool isReflect();
    virtual float getSpeedMul();
    virtual bool isCountMax();
    virtual void setNoSpeedMul(bool);
};
static_assert(sizeof(soReflectModuleImpl) == 0x40, "Class is wrong size!");

// (soArrayVector<soInterpolation<Vec3f>, 19> and <soAreaInstance, 9> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emKiller : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 19> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 9> m_areaInstances;           // 0x56AC, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x120];                                       // 0x57FC
    emTargetSearchUnit m_targetSearchUnits[2];                  // 0x591C
    emAIUnit m_aiUnits[13];                                     // 0x594C, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x59B4, camera module
    // A reflect module of its own (kind 0), handed to the Enemy constructor; the other small enemies pass the null one.
    soReflectModuleImpl m_reflectModule;                        // 0x5A04

public:
    emKiller(u32 p1, emCreate* create);
    virtual ~emKiller();
};
static_assert(sizeof(emKiller) == 0x5a44, "Class is wrong size!");
