#pragma once

#include <so/so_module_accesser.h>
#include <wn/wn_em_simple.h>

// The proc functions of Petey Piranha's wnemSimple weapons (its two arms), registered with the enemy weapon manager by
// the Enemy unit. Names from the map (wnemBosspackunProc::ArmEnter, ArmIdol, ArmDead) and the procs' own Report strings.
class wnemBosspackunProc {
public:
    static void ArmEnter(wnemSimple* wn, soModuleAccesser* acc);
    static void ArmIdol(wnemSimple* wn, soModuleAccesser* acc);
    static void ArmDead(wnemSimple* wn, soModuleAccesser* acc);
};
