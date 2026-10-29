#pragma once

#include <so/so_module_accesser.h>
#include <wn/wn_em_simple.h>

// The proc functions of the Duon's wnemSimple weapons (its bombs, missiles and lasers), registered with the enemy weapon
// manager by the Enemy unit. Names from the map (wnemDuonProc::BombFly, ...) and the procs' own Report strings.
class wnemDuonProc {
public:
    static void BombFly(wnemSimple* wn, soModuleAccesser* acc);
    static void BombExplosion(wnemSimple* wn, soModuleAccesser* acc);
    static void MissileFly(wnemSimple* wn, soModuleAccesser* acc);
    static void MissileExplosion(wnemSimple* wn, soModuleAccesser* acc);
    static void LaserEnter(wnemSimple* wn);
    static void LaserFly(wnemSimple* wn);
    static void LaserDead(wnemSimple* wn, soModuleAccesser* acc);
};
