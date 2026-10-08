#include <st_greenhill/gr_greenhill.h>
#include <st_greenhill/gr_greenhill_anim.h>
#include <memory.h>

// Switches the ball model to one of its five animations (0 drop, 1 leave, 2 break, 3 roll, 4 idle - HYPOTHESIS names);
// the same sequence of animation binds as the Tengan floor.
void grGreenhillCheck::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
    grGreenhillSetMotion(this, m_animId, 5, animId, shouldLoop, force, frameCount);
}
