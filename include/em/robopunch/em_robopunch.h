#pragma once

// SHADOW of BrawlHeaders em/robopunch/em_robopunch.h (-I include comes first): the full layout of the Robopunch (enemy
// kind Enemy_Robo_Punch).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 54> and <soAreaInstance, 11> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emRobopunch : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 54> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 11> m_areaInstances;          // 0x5D3C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x160];                                       // 0x5ED4
    emTargetSearchUnit m_targetSearchUnits[4];                  // 0x6034
    emAIUnit m_aiUnits[41];                                     // 0x6094, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x61DC, camera module

public:
    emRobopunch(u32 p1, emCreate* create);
    virtual ~emRobopunch();
};
static_assert(sizeof(emRobopunch) == 0x622c, "Class is wrong size!");
