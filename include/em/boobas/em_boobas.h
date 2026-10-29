#pragma once

// SHADOW of BrawlHeaders em/boobas/em_boobas.h (-I include comes first): the full layout of the Boobas.

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>

// A class of sora_melee: the Boobas only calls its constructor and destructor. (Its interpolation and area instance
// arrays are declared in em/em_module_arrays.h.)
FT_DOL_ARRAY_VECTOR(soAreaWind, 3);

class emBoobas : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 26> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 13> m_areaInstances;          // 0x57FC, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x1a0];                                        // 0x59DC
    emTargetSearchUnit m_targetSearchUnits[4];                   // 0x5B7C
    emAIUnit m_aiUnits[28];                                      // 0x5BDC, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5CBC, camera module
    // The only enemy so far with wind areas of its own: three, handed to the Enemy constructor where the others pass
    // the shared null object of sora_melee.
    soArrayVector<soAreaWind, 3> m_areaWinds;                    // 0x5D0C, area module

public:
    emBoobas(u32 p1, emCreate* create);
    virtual ~emBoobas();
};
static_assert(sizeof(emBoobas) == 0x5d24, "Class is wrong size!");
