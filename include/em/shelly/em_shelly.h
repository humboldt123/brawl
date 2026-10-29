#pragma once

// SHADOW of BrawlHeaders em/shelly/em_shelly.h (-I include comes first): the full layout of the Shelly (enemy kind
// Enemy_Shelly).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 64> and <soAreaInstance, 8> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emShelly : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 64> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 8> m_areaInstances;           // 0x5F1C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x100];                                       // 0x6048
    emTargetSearchUnit m_targetSearchUnits[2];                  // 0x6148
    emAIUnit m_aiUnits[27];                                     // 0x6178, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x6250, camera module

public:
    emShelly(u32 p1, emCreate* create);
    virtual ~emShelly();
};
static_assert(sizeof(emShelly) == 0x62a0, "Class is wrong size!");
