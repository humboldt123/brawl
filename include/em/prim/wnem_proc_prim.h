#pragma once

#include <so/so_module_accesser.h>
#include <wn/wn_em_simple.h>

// The proc functions of the Primid's wnemSimple weapon (the boomerang of the boomerang Primid), registered with the
// enemy weapon manager by the Enemy unit. Names from the map and the procs' own Report strings.
class wnemPrimProc {
public:
    static void BoomerangHave(wnemSimple* wn, soModuleAccesser* acc);
    static void BoomerangFly(wnemSimple* wn, soModuleAccesser* acc);
    static void BoomerangTurn(wnemSimple* wn, soModuleAccesser* acc);
    static void BoomerangErase(wnemSimple* wn, soModuleAccesser* acc);
};
