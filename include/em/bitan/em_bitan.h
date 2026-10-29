#pragma once

// SHADOW of BrawlHeaders em/bitan/em_bitan.h (-I include comes first): the full layout of the Bitan (enemy kind
// Enemy_Bitan).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 12> and <soAreaInstance, 8> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emBitan : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 12> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 8> m_areaInstances;           // 0x555C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x100];                                       // 0x5688
    emTargetSearchUnit m_targetSearchUnits[2];                  // 0x5788
    emAIUnit m_aiUnits[37];                                     // 0x57B8, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x58E0, camera module

public:
    emBitan(u32 p1, emCreate* create);
    virtual ~emBitan();
    virtual float getScore();
};
static_assert(sizeof(emBitan) == 0x5930, "Class is wrong size!");
