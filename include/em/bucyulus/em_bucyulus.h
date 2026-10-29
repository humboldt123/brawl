#pragma once

// SHADOW of BrawlHeaders em/bucyulus/em_bucyulus.h (-I include comes first): the full layout of the Bucyulus (enemy
// kind Enemy_Bucyulus).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <so/link/so_link_event_presenter.h>
#include <types.h>

// The event the Bucyulus sends to the fighter it holds when it enters status 0xC (link event 0x35).
// HYPOTHESIS: m_param carries parameters 0xFA6 and 0xFA7 of the Bucyulus with 1 in between (a scale or speed of the
// held fighter); the receiver is not decompiled.
struct emBucyulusHoldLinkEventArgs : public soLinkEventArgs {
    Vec3f m_param; // 0x8
    emBucyulusHoldLinkEventArgs(int eventKind) : soLinkEventArgs(eventKind), m_param(0.0f, 0.0f, 0.0f) { }
};

class emBucyulus : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 23> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 7> m_areaInstances;           // 0x576C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0xe0];                                         // 0x5874
    emTargetSearchUnit m_targetSearchUnits[1];                   // 0x5954
    emAIUnit m_aiUnits[24];                                      // 0x596C, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5A2C, camera module

public:
    emBucyulus(u32 p1, emCreate* create);
    virtual ~emBucyulus();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emBucyulus) == 0x5a7c, "Class is wrong size!");
