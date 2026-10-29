#pragma once

#include <so/so_module_accesser.h>
#include <wn/wn_em_simple.h>

// The proc functions of the Cataguard's wnemSimple weapons (its lance and its helmet bullets), registered with the
// enemy weapon manager. Names from the map and the procs' own Report strings.
class wnemCataguardProc {
public:
    static void LanceFly(wnemSimple* wn, soModuleAccesser* acc);
    static void LanceFall(wnemSimple* wn, soModuleAccesser* acc);
    static void LanceStick(wnemSimple* wn, soModuleAccesser* acc);
    static void HelmIdol(wnemSimple* wn, soModuleAccesser* acc);
    static void HelmBullet(wnemSimple* wn, soModuleAccesser* acc);
};
