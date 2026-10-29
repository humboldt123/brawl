#pragma once

#include <so/so_module_accesser.h>
#include <wn/wn_em_simple.h>

// The proc functions of the Porky's wnemSimple weapons (the Porky's robot and the spark it fires), registered with the
// enemy weapon manager as procs 0x28-0x2E. Names from the map and the procs' own Report strings.
class wnemPorkyProc {
public:
    static void RoboFall(wnemSimple* wn, soModuleAccesser* acc);
    static void RoboRun(wnemSimple* wn, soModuleAccesser* acc);
    static void RoboDown(wnemSimple* wn);
    static void RoboExplosion(wnemSimple* wn, soModuleAccesser* acc);
    static void SparkLock(wnemSimple* wn);
    static void SparkHorming(wnemSimple* wn, soModuleAccesser* acc);
    static void SparkErase(wnemSimple* wn);
};
