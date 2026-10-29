#pragma once

// SHADOW of BrawlHeaders em/galfire/em_galfire.h (-I include comes first): the full layout of the Galfire (enemy kind
// Enemy_Gal_Fire).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <types.h>

// (soArrayVector<soInterpolation<Vec3f>, 24> and <soAreaInstance, 7> are classes of sora_melee shared with other
// enemies: em/em_module_arrays.h declares them.)

class emGalfire : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 24> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 7> m_areaInstances;           // 0x579C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0xe0];                                        // 0x58A4
    emTargetSearchUnit m_targetSearchUnits[1];                  // 0x5984
    emAIUnit m_aiUnits[21];                                     // 0x599C, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;         // 0x5A44, camera module

public:
    emGalfire(u32 p1, emCreate* create);
    virtual ~emGalfire();
    virtual void processUpdate();
};
static_assert(sizeof(emGalfire) == 0x5a94, "Class is wrong size!");
