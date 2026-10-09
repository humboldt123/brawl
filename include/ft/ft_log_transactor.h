#pragma once

class ftOwner;
class soModuleAccesser;

// Declaration-only interface. Rollout calls this with the Fighter owner;
// the native routine updates the owner's action log and attack power multiplier.
class ftLogTransactor {
public:
    // HYPOTHESIS: the first three integer arguments encode action kind, ID,
    // and forced restart; their meanings follow the routine's branches.
    static void resetLogActionInfo(int, int, int, ftOwner*, soModuleAccesser*);
};
