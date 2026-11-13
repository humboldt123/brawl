#pragma once

#include <types.h>

class ftOwner;
class soModuleAccesser;
class StageObject;
struct soStatusData;

// Static hooks that keep a fighter owner's battle log (ftLog) up to date: Fighter's event handlers
// and the outside-event observer forward status, situation, attack and hit events here.
// Names are map names (ftLogTransactor::...); the map has no parameter types, so the
// parameter lists below follow the register use of the bodies and their callers.
class ftLogTransactor {
public:
    // HYPOTHESIS: statusData and the parameters named unused are passed by the callers but never read.
    static void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser, ftOwner* owner);
    // HYPOTHESIS: the first three integer arguments encode action kind, ID,
    // and forced restart; their meanings follow the routine's branches.
    static void resetLogActionInfo(int, int, int, ftOwner*, soModuleAccesser*);
    // Starts a new attack in the log and returns its serial number.
    static int intrudeLogAttackInfo(int id, int unused, ftOwner* owner);
    static void processFixPosition(soModuleAccesser* moduleAccesser, ftOwner* owner);
    static void notifyEventCollisionReflector(soModuleAccesser* moduleAccesser, ftOwner* owner);
    static void notifyEventCurrentAttack(ftOwner* owner, StageObject* target);
    static void notifyEventAttack(ftOwner* owner, u32 serial, StageObject* target);
    static void notifyEventHit(ftOwner* owner, int kind, void* unused);
    // HYPOTHESIS: unsigned (cmplwi); kind 1 is Situation_Cliff.
    static void notifyEventChangeSituation(ftOwner* owner, u32 kind);
};
