#pragma once

// SHADOW of BrawlHeaders em/tautau/em_tautau.h (-I include comes first): the full layout of the Tautau (enemy kind
// Enemy_Tautau).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 32> and <soAreaInstance, 11> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emTautau : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 32> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 11> m_areaInstances;          // 0x591C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x160];                                       // 0x5AB4
    emTargetSearchUnit m_targetSearchUnits[5];                  // 0x5C14
    emAIUnit m_aiUnits[40];                                     // 0x5C8C, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x5DCC, camera module

public:
    emTautau(u32 p1, emCreate* create);
    virtual ~emTautau();
};
static_assert(sizeof(emTautau) == 0x5e1c, "Class is wrong size!");
