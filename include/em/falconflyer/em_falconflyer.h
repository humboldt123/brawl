#pragma once

// SHADOW of BrawlHeaders em/falconflyer/em_falconflyer.h (-I include comes first): the full layout of the Falcon Flyer
// (enemy kind Enemy_Falconflyer).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>

// A class of sora_melee: the Falcon Flyer only calls its constructor and destructor. (Its area instance and camera
// subject arrays are declared in em/em_module_arrays.h.)
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 25);

class emFalconflyer : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 25> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 7> m_areaInstances;           // 0x57CC, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0xe0];                                         // 0x58D4
    emTargetSearchUnit m_targetSearchUnits[1];                   // 0x59B4
    emAIUnit m_aiUnits[5];                                       // 0x59CC, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x59F4, camera module

public:
    emFalconflyer(u32 p1, emCreate* create);
    virtual ~emFalconflyer();
};
static_assert(sizeof(emFalconflyer) == 0x5a44, "Class is wrong size!");
