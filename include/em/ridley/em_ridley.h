#pragma once

// SHADOW of BrawlHeaders em/ridley/em_ridley.h (-I include comes first): the full layout of Ridley (the Subspace
// Emissary boss, enemy kind Enemy_Ridley).

#include <StaticAssert.h>
// TODO: soArrayVector<soAreaWind, 3> is declared by the Boobas header; it belongs in em/em_module_arrays.h together
// with an include of so/templates/so_array_value_soAreaWind.h (proposed shared patch). Until then the pair comes from
// there.
#include <em/boobas/em_boobas.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>

// A class of sora_melee: Ridley only calls its constructor and destructor. (Its area instance, camera subject and wind
// arrays are declared in em/em_module_arrays.h.)
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 115);

class emRidley : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 115> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 11> m_areaInstances;           // 0x68AC, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x160];                                         // 0x6A44
    emTargetSearchUnit m_targetSearchUnits[2];                    // 0x6BA4
    emAIUnit m_aiUnits[18];                                       // 0x6BD4, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;           // 0x6C64, camera module
    // Wind areas of its own (three, like the Boobas), handed to the Enemy constructor where most enemies pass the
    // shared null object of sora_melee.
    soArrayVector<soAreaWind, 3> m_areaWinds;                     // 0x6CB4, area module

public:
    emRidley(u32 p1, emCreate* create);
    virtual ~emRidley();
};
static_assert(sizeof(emRidley) == 0x6ccc, "Class is wrong size!");
