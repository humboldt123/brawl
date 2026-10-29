#pragma once

// SHADOW of BrawlHeaders em/faulong/em_faulong.h (-I include comes first): the full layout of the Faulong (enemy kind
// Enemy_Faulong).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 27> and <soAreaInstance, 9> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emFaulong : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 27> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 9> m_areaInstances;           // 0x582C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x120];                                       // 0x597C
    emTargetSearchUnit m_targetSearchUnits[2];                  // 0x5A9C
    emAIUnit m_aiUnits[23];                                     // 0x5ACC, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x5B84, camera module

public:
    emFaulong(u32 p1, emCreate* create);
    virtual ~emFaulong();
};
static_assert(sizeof(emFaulong) == 0x5bd4, "Class is wrong size!");
