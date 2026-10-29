#pragma once

#include <so/so_module_accesser.h>
#include <wn/wn_em_simple.h>

// The proc functions of the Gyraan's wnemSimple weapons (its axes), registered with the enemy weapon manager by the
// Enemy unit. Names from the map (wnemGyraanProc::AxeIdol, AxeSwing, AxeStick) and the procs' own Report strings.
class wnemGyraanProc {
public:
    static void AxeIdol(wnemSimple* wn, soModuleAccesser* acc);
    static void AxeSwing(wnemSimple* wn, soModuleAccesser* acc);
    static void AxeStick(wnemSimple* wn, soModuleAccesser* acc);
};
