#pragma once

// SHADOW of BrawlHeaders em/ngagog/em_ngagog.h (-I include comes first): the full layout of the Ngagog (enemy kind
// Enemy_Ngagog).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 37> and <soAreaInstance, 9> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emNgagog : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 37> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 9> m_areaInstances;           // 0x5A0C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x120];                                       // 0x5B5C
    emTargetSearchUnit m_targetSearchUnits[3];                  // 0x5C7C
    emAIUnit m_aiUnits[30];                                     // 0x5CC4, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x5DB4, camera module

public:
    emNgagog(u32 p1, emCreate* create);
    virtual ~emNgagog();
};
static_assert(sizeof(emNgagog) == 0x5e04, "Class is wrong size!");
