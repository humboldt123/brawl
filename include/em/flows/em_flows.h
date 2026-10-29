#pragma once

// SHADOW of BrawlHeaders em/flows/em_flows.h (-I include comes first): the full layout of the Flows (enemy kind
// Enemy_Flows).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 31> and <soAreaInstance, 13> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emFlows : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 31> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 13> m_areaInstances;          // 0x58EC, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x1a0];                                       // 0x5ACC
    emTargetSearchUnit m_targetSearchUnits[3];                  // 0x5C6C
    emAIUnit m_aiUnits[16];                                     // 0x5CB4, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x5D34, camera module

public:
    emFlows(u32 p1, emCreate* create);
    virtual ~emFlows();
};
static_assert(sizeof(emFlows) == 0x5d84, "Class is wrong size!");
