#pragma once

// SHADOW of BrawlHeaders em/kuribo/em_kuribo.h (-I include comes first): the full layout of the Goomba (enemy kind
// Enemy_Kuribo), and the tread event the Goomba family (Goomba, Giant Goomba, Paratroopa) answers.

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <so/link/so_link_event_presenter.h>
#include <types.h>

extern "C" {
// stTriggerMng (sora_melee st_trigger), address name. HYPOTHESIS: notifies the trigger manager that the enemy with this
// create id (own task id) was trodden on by the task sender (its task id and category).
void fn_27_23313C(u32 createId, u32 senderTaskId, u32 taskId, int senderCategory);
}

// Link event 6: a fighter that jumps on a stage object asks it to take the tread (ftStatusUniqProcessTread::initStatus,
// status Tread_Jump, sends it to the parent it links to). The Goomba family consumes the event without taking the
// fighter: m_handled stays false (no model constraint), the fighter keeps its tread jump count, and the enemy sets the
// jump speed multiplier the fighter stores in Float_Jump_Speed_Mul.
// HYPOTHESIS: field roles from the fighter side (ft_status_uniq_process_tread.cpp, ftTreadLinkEventArgs).
struct emTreadLinkEventArgs : public soLinkEventArgs {
    bool m_handled;            // 0x5, the parent took the fighter (model constraint on m_node)
    bool m_keepTreadJumpCount; // 0x6, false resets the fighter's Int_Tread_Jump_Count
    int m_node;                // 0x8, the parent's node the fighter is constrained to
    Vec3f m_pos;               // 0xC, the fighter's position (x, y) and, in z, its jump speed multiplier
};

// HYPOTHESIS: link event 6 is the tread request (see emTreadLinkEventArgs).
static const int LinkEvent_Tread = 6;

class emKuribo : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 33> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 9> m_areaInstances;           // 0x594C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x120];                                        // 0x5A9C
    emTargetSearchUnit m_targetSearchUnits[2];                   // 0x5BBC
    emAIUnit m_aiUnits[29];                                      // 0x5BEC, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5CD4, camera module

public:
    emKuribo(u32 p1, emCreate* create);
    virtual ~emKuribo();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
};
static_assert(sizeof(emKuribo) == 0x5d24, "Class is wrong size!");
