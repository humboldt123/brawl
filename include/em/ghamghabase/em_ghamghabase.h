#pragma once

// SHADOW of BrawlHeaders em/ghamghabase/em_ghamghabase.h (-I include comes first): the full layout of the Ghamgha base
// (enemy kinds Enemy_Ghamghabase_01 to _04).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 19> and <soAreaInstance, 7> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emGhamghabase : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 19> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 7> m_areaInstances;           // 0x56AC, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0xe0];                                        // 0x57B4
    emTargetSearchUnit m_targetSearchUnits[1];                  // 0x5894
    emAIUnit m_aiUnits[14];                                     // 0x58AC, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x591C, camera module

public:
    emGhamghabase(u32 p1, emCreate* create);
    virtual ~emGhamghabase();
};
static_assert(sizeof(emGhamghabase) == 0x596c, "Class is wrong size!");
