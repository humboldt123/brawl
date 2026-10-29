#pragma once

// SHADOW of BrawlHeaders em/wnem/wnem_proc_common.h (-I include comes first): adds the shared procs that the enemy
// weapon register installs (HormingTarget*, Shell*). Names from the map and the procs' own Report strings.

#include <mt/mt_vector.h>

class soModuleAccesser;
class wnemSimple;

class wnemCommonProc {
public:
    static void Report(const char* fmt, ...);

    // Homing steering of a weapon. HYPOTHESIS: the roles, from the comparisons: the direction from `from` to `to` is
    // turned towards `aim` by at most `maxAngle` degrees (the aim is replaced with its axis default when it is not a
    // finite vector); the result is the unit direction.
    static Vec2f HormingTarget(Vec3f& to, Vec3f& from, float maxAngle, Vec3f& aim);
    // The same with a turn angle kept between calls (`angle`, grown by `step * stepScale` each call until it reaches
    // `limit`) and a flag that is raised when it is close to the limit.
    static Vec2f HormingTarget1(Vec3f& to, Vec3f& from, float* angle, Vec3f& aim, bool* reached, float step,
                                float minAngle, float limit, float stepScale);

    // The procs of an enemy's shell (a shot that flies, then ends when it touches the ground or something else).
    static void ShellFly(wnemSimple* wn, soModuleAccesser* acc);
    static void ShellErase(wnemSimple* wn, soModuleAccesser* acc);
    static void ShellHitEtc(wnemSimple* wn, soModuleAccesser* acc);
    static void ShellHitGround(wnemSimple* wn, soModuleAccesser* acc);
};
