#pragma once

#include <so/so_module_accesser.h>
#include <wn/wn_em_simple.h>

// The proc functions of the Galleom's wnemSimple weapons (its grenade and its shot), registered with the enemy weapon
// manager. Names from the map (wnemGalleomProc::GrenadeFly, GrenadeExplosion, ShotFly, ShotExplosion).
class wnemGalleomProc {
public:
    static void GrenadeFly(wnemSimple* wn, soModuleAccesser* acc);
    static void GrenadeExplosion(wnemSimple* wn, soModuleAccesser* acc);
    static void ShotFly(wnemSimple* wn, soModuleAccesser* acc);
    static void ShotExplosion(wnemSimple* wn, soModuleAccesser* acc);
};
