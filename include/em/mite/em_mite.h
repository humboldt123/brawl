#pragma once

// SHADOW of BrawlHeaders em/mite/em_mite.h (-I include comes first): the full layout of the Mite (enemy kind
// Enemy_Mite).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 12> and <soAreaInstance, 13> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emMite : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 12> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 13> m_areaInstances;          // 0x555C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x1a0];                                       // 0x573C
    emTargetSearchUnit m_targetSearchUnits[7];                  // 0x58DC
    emAIUnit m_aiUnits[34];                                     // 0x5984, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x5A94, camera module

public:
    emMite(u32 p1, emCreate* create);
    virtual ~emMite();
};
static_assert(sizeof(emMite) == 0x5ae4, "Class is wrong size!");
