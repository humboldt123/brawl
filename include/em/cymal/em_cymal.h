#pragma once

// SHADOW of BrawlHeaders em/cymal/em_cymal.h (-I include comes first): the full layout of the Cymal (enemy kind
// Enemy_Cymal).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 14> and <soAreaInstance, 8> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emCymal : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 14> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 8> m_areaInstances;           // 0x55BC, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x100];                                       // 0x56E8
    emTargetSearchUnit m_targetSearchUnits[1];                  // 0x57E8
    emAIUnit m_aiUnits[13];                                     // 0x5800, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x5868, camera module

public:
    emCymal(u32 p1, emCreate* create);
    virtual ~emCymal();
    virtual void notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionReflectorCheck();
};
static_assert(sizeof(emCymal) == 0x58b8, "Class is wrong size!");
