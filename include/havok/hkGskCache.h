#pragma once

#include <havok/hkBase.h>

// GSK cache entry (12 bytes, map name hkGskCache). From the symmetric processCollision wrappers, which flip a cache to
// the swapped body order: m_dimA + m_dimB vertex ids are stored A first, so the flip swaps the two counts, swaps
// unkA/unkB and reverses the first m_dimA + m_dimB ids. HYPOTHESIS: the names; the counts are signed bytes (extsb).
struct hkGskCache {
    u16 m_vertices[4]; // 0x00
    s8 m_dimA;         // 0x08
    s8 m_dimB;         // 0x09
    u8 unkA;           // 0x0A
    u8 unkB;           // 0x0B

    // Turns the cache to the swapped body order (inlined into the symmetric processCollision wrappers).
    void flip() {
        u8 t = unkB;
        unkB = unkA;
        unkA = t;
        s8 dim = m_dimA;
        m_dimA = m_dimB;
        m_dimB = dim;
        int last = m_dimA + m_dimB - 1;
        u16 id = m_vertices[0];
        m_vertices[0] = m_vertices[last];
        m_vertices[last] = id;
        if ((m_dimA & m_dimB) == 2) {
            id = m_vertices[1];
            m_vertices[1] = m_vertices[2];
            m_vertices[2] = id;
        }
    }

    // Copies the 12 bytes of other (map name hkGskCache____as).
    void operator=(const hkGskCache& other);
};
