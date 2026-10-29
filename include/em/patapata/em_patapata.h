#pragma once

// SHADOW of BrawlHeaders em/patapata/em_patapata.h (-I include comes first): the full layout of the Paratroopa (enemy
// kind Enemy_Patapata).

#include <StaticAssert.h>
#include <em/aroaros/em_aroaros.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <em/kuribo/em_kuribo.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>

// A class of sora_melee: the Paratroopa only calls its constructor and destructor. (Its area instance and camera
// subject arrays are declared in em/em_module_arrays.h.)
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 34);

class emPatapata : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 34> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 11> m_areaInstances;          // 0x597C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x160];                                        // 0x5B14
    emTargetSearchUnit m_targetSearchUnits[3];                   // 0x5C74
    emAIUnit m_aiUnits[33];                                      // 0x5CBC, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5DC4, camera module

public:
    emPatapata(u32 p1, emCreate* create);
    virtual ~emPatapata();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
    virtual bool emShootItem(itKind kind, int variation, void* param, float lr, float);
    virtual bool emItemInformationReturnEnemy(int instanceId);
};
static_assert(sizeof(emPatapata) == 0x5e14, "Class is wrong size!");
