#pragma once

// SHADOW of BrawlHeaders em/pacci/em_pacci.h (-I include comes first): the full layout of the Pacci (enemy kind
// Enemy_Pacci).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 52> and <soAreaInstance, 10> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emPacci : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 52> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 10> m_areaInstances;          // 0x5CDC, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x140];                                       // 0x5E50
    emTargetSearchUnit m_targetSearchUnits[3];                  // 0x5F90
    emAIUnit m_aiUnits[18];                                     // 0x5FD8, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x6068, camera module

public:
    emPacci(u32 p1, emCreate* create);
    virtual ~emPacci();
};
static_assert(sizeof(emPacci) == 0x60b8, "Class is wrong size!");
