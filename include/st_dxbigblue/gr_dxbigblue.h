#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <types.h>

// The base of the grounds of Big Blue: a step counter (m_state, 0 = waiting to come in, 1 = ready, 2 = running) and the
// frames the ground has left in that step.
class grDxBigBlue : public grYakumono {
protected:
    u8 m_state;
    float m_timer;
public:
    grDxBigBlue(const char* taskName);
    virtual ~grDxBigBlue();
};
static_assert(sizeof(grDxBigBlue) == 0x158, "Class is the wrong size!");
