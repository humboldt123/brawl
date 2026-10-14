#pragma once

#include <types.h>

// HYPOTHESIS: ftUtil::convertReflectorData(record, shapeOut, groupOut) in sora_melee turns the reflector record of a fighter's common
// data into a one-shape reflector group (the shape is written to shapeOut, the group descriptor to groupOut).
extern "C" void fn_27_195B34(const void* record, void* shapeOut, void* groupOut);

// HYPOTHESIS: reflector group descriptor as filled by convertReflectorData (shape array pointer, shape count, four floats, high flag
// bit). The fighter constructors clear it member by member before the conversion.
struct ftReflectorGroupDraft {
    void* m_shapes;
    s32 m_count;
    float m_f0, m_f1, m_f2, m_f3;
    u32 m_flag : 1;
    u32 m_rest : 31;
};

#define FT_REFLECTOR_GROUP_CLEAR(group) \
    do {                                \
        (group).m_shapes = 0;           \
        (group).m_count = 0;            \
        (group).m_f0 = 0.0f;            \
        (group).m_f1 = 0.0f;            \
        (group).m_f2 = 0.0f;            \
        (group).m_f3 = 0.0f;            \
        (group).m_flag = 0;             \
    } while (0)
