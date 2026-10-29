#pragma once

// SHADOW of BrawlHeaders em/dekakuribo/em_dekakuribo.h (-I include comes first): the full layout of the Giant Goomba
// (enemy kind Enemy_Dekakuribo).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <em/kuribo/em_kuribo.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>

class emDekakuribo : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 28> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 10> m_areaInstances;          // 0x585C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x140];                                        // 0x59D0
    emTargetSearchUnit m_targetSearchUnits[3];                   // 0x5B10
    emAIUnit m_aiUnits[34];                                      // 0x5B58, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5C68, camera module

public:
    emDekakuribo(u32 p1, emCreate* create);
    virtual ~emDekakuribo();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
};
static_assert(sizeof(emDekakuribo) == 0x5cb8, "Class is wrong size!");
