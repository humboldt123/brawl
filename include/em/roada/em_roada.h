#pragma once

// SHADOW of BrawlHeaders em/roada/em_roada.h (-I include comes first): the full layout of the Roada (enemy kind
// Enemy_Roada).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 17> and <soAreaInstance, 9> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emRoada : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 17> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 9> m_areaInstances;           // 0x564C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x120];                                       // 0x579C
    emTargetSearchUnit m_targetSearchUnits[3];                  // 0x58BC
    emAIUnit m_aiUnits[39];                                     // 0x5904, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x5A3C, camera module

public:
    emRoada(u32 p1, emCreate* create);
    virtual ~emRoada();
};
static_assert(sizeof(emRoada) == 0x5a8c, "Class is wrong size!");
