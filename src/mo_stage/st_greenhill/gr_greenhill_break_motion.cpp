#include <st_greenhill/gr_greenhill.h>
#include <st_greenhill/gr_greenhill_anim.h>

// Only animation 0 exists on the breakable pieces.
void grGreenhillBreak::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
    GR_GREENHILL_SET_MOTION(m_animId, 1)
}
