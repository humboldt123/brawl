#pragma once

#include <ft/ft_change_succeed_work.h>
#include <mt/mt_vector.h>
#include <types.h>

// HYPOTHESIS: four (x, y) points of the collision shapes the dummy fighter that takes over the move is given. Samus and
// Zero Suit Samus share the type: its (empty) constructor lives in the Samus unit and the Zero Suit unit calls it there.
struct ftSamusChangeSucceedPoints {
    float point[4][2];
    ftSamusChangeSucceedPoints() __attribute__((never_inline));
};
