#pragma once

#include <so/so_module_accesser.h>
#include <wn/wn_em_simple.h>

// The proc functions of the R.O.B. enemy's wnemSimple weapon (its homing missile), registered with the enemy weapon
// manager. Names from the map and the procs' own Report strings.
class wnemRoboProc {
public:
    static void MissileFly(wnemSimple* wn, soModuleAccesser* acc);
    static void MissileBomb(wnemSimple* wn, soModuleAccesser* acc);
    static void MissileErase(wnemSimple* wn, soModuleAccesser* acc);
};
