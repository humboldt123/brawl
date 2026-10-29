#pragma once

// SHADOW of BrawlHeaders em/jdus/em_jdus.h (-I include comes first): the full layout of the Jdus (enemy kind
// Enemy_Jdus).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 33> and <soAreaInstance, 11> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emJdus : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 33> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 11> m_areaInstances;          // 0x594C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x160];                                       // 0x5AE4
    emTargetSearchUnit m_targetSearchUnits[4];                  // 0x5C44
    emAIUnit m_aiUnits[38];                                     // 0x5CA4, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x5DD4, camera module

public:
    emJdus(u32 p1, emCreate* create);
    virtual ~emJdus();
};
static_assert(sizeof(emJdus) == 0x5e24, "Class is wrong size!");
