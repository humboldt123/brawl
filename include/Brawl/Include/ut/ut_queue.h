#pragma once

#include <StaticAssert.h>
#include <types.h>

template <typename T, u32 C>
class utQueue {
    u16 m_size;
    u16 m_count; // number of queued elements
public:
    T m_elements[C];

    utQueue() {
        m_size = C;
        m_count = 0;
        T* p = m_elements;
        do {
            *p = 0;
            p++;
        } while (p < m_elements + C);
    }

    u32 size() const { return C; }
    u16 getCount() const { return m_count; }
    T get(u32 i) const { return m_elements[i]; }
    void push(T value) {
        u16 idx = m_count;
        m_count = idx + 1;
        m_elements[idx] = value;
    }
};
