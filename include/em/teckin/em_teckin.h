#pragma once

// SHADOW of BrawlHeaders em/teckin/em_teckin.h (-I include comes first): the full layout of the Teckin (enemy kind
// Enemy_Teckin).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 33> and <soAreaInstance, 9> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emTeckin : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 33> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 9> m_areaInstances;           // 0x594C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x120];                                       // 0x5A9C
    emTargetSearchUnit m_targetSearchUnits[3];                  // 0x5BBC
    emAIUnit m_aiUnits[28];                                     // 0x5C04, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x5CE4, camera module

public:
    emTeckin(u32 p1, emCreate* create);
    virtual ~emTeckin();
};
static_assert(sizeof(emTeckin) == 0x5d34, "Class is wrong size!");
