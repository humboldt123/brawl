#pragma once

// SHADOW of BrawlHeaders em/jyakeel/em_jyakeel.h (-I include comes first): the full layout of the Jyakeel (enemy kind
// Enemy_Jyakeel).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 9> and <soAreaInstance, 7> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emJyakeel : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 9> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 7> m_areaInstances;          // 0x54CC, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0xe0];                                       // 0x55D4
    emTargetSearchUnit m_targetSearchUnits[1];                 // 0x56B4
    emAIUnit m_aiUnits[3];                                     // 0x56CC, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;        // 0x56E4, camera module

public:
    emJyakeel(u32 p1, emCreate* create);
    virtual ~emJyakeel();
};
static_assert(sizeof(emJyakeel) == 0x5734, "Class is wrong size!");
