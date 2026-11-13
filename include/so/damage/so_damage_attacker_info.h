#pragma once

#include <StaticAssert.h>
#include <so/so_kind.h>
#include <gf/gf_task.h>
#include <types.h>

class StageObject;

class soDamageAttackerInfo {
public:
    u8 m_directTaskCategory; // gfTask::Category; read as a plain byte (ftLogTransactor::processFixPosition)
    char _spacer[3];
    int m_directTaskId;
    soKind m_directSoKind;
    int m_directSoSubKind;

    u8 m_indirectTaskCategory; // gfTask::Category; read as a plain byte (ftLogTransactor::processFixPosition)
    char _spacer2[3];
    int m_indirectTaskId;
    soKind m_indirectSoKind;
    int m_indirectSoSubKind;
    int m_indirectEntryId;

    int m_clearFrame;

    // NOTE: shadows the BrawlHeaders copy to add the member functions (names from the map).
    void clear();
    void update();
    void copy(soDamageAttackerInfo* dst);
    void set(u32 defenderTaskId, StageObject* attacker);
    void setIndirect(u32 defenderTaskId, StageObject* attacker, StageObject* indirectAttacker);
};
static_assert(sizeof(soDamageAttackerInfo) == 40, "Class is wrong size!");
