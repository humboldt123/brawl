#pragma once

// SHADOW of BrawlHeaders em/aroaros/em_aroaros.h (-I include comes first): the full layout of the Auroros (enemy kind
// Enemy_Aroaros).

#include <StaticAssert.h>
#include <em/em_module_arrays.h>
#include <em/em_target_search_unit.h>
#include <em/enemy.h>
#include <ft/builder/ft_dol_instances.h>
#include <types.h>

// The parameter block of an item the Auroros or the Paratroopa creates (anim cmd 26 hands its address to emShootItem):
// the same layout as the Popperam's emPopperamItemShootParam, of which these two read only the node and the offset.
// HYPOTHESIS: one shared block of anim cmd 26 (it could replace both declarations).
struct emItemShootNodeParam {
    u8 unk0[0x18];  // 0x0, the launch speed and angle range (see emPopperamItemShootParam)
    int m_nodeId;   // 0x18, the model node the item is created at
    Vec3f m_offset; // 0x1C, HYPOTHESIS: an offset from that node (the original only tests its address)
};

class emAroaros : public Enemy {
    soArrayVector<soInterpolation<Vec3f>, 37> m_interpolations; // 0x5310, posture module
    soArrayVector<soAreaInstance, 9> m_areaInstances;           // 0x5A0C, area module
    // HYPOTHESIS: one 0x20-byte work entry per area instance, as for the Primid and the Hammer Bro.
    u8 m_areaWork[0x120];                                        // 0x5B5C
    emTargetSearchUnit m_targetSearchUnits[2];                   // 0x5C7C
    emAIUnit m_aiUnits[16];                                      // 0x5CAC, AI module
    soArrayVector<soCameraSubject, 1> m_cameraSubjects;          // 0x5D2C, camera module

public:
    emAroaros(u32 p1, emCreate* create);
    virtual ~emAroaros();
    virtual bool emShootItem(itKind kind, int variation, void* param, float lr, float);
    virtual bool emItemInformationReturnEnemy(int instanceId);
};
static_assert(sizeof(emAroaros) == 0x5d7c, "Class is wrong size!");
