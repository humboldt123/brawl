#pragma once

#include <so/so_module_accesser.h>
#include <wn/wn_em_simple.h>

// The proc functions of the Botron's wnemSimple weapon (its iron ball), registered with the enemy weapon manager by
// the Enemy unit. Names from the map (wnemBotronProc::IronIdol, IronTouchGround) and the procs' own Report strings.
class wnemBotronProc {
public:
    static void IronIdol(wnemSimple* wn, soModuleAccesser* acc);
    static void IronTouchGround(wnemSimple* wn, soModuleAccesser* acc);
};
