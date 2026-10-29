#pragma once

#include <wn/wn_em_simple.h>

// The proc functions of the Crazy Hand's bomb (a wnemSimple weapon), registered with the enemy weapon manager as procs
// 0x37 and 0x38. Names from the map (wnemCrazyhandProc::BombFly, BombExplosion) and the proc's own Report string.
class wnemCrazyhandProc {
public:
    static void BombFly(wnemSimple* wn);
    static void BombExplosion(wnemSimple* wn);
};
