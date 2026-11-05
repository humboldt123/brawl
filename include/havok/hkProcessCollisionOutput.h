#pragma once

#include <havok/hkContactPoint.h>
#include <havok/hkGskCache.h>

// Contact point written by an agent's processCollision (0x30 bytes, the stride of the symmetric wrappers' loop).
// HYPOTHESIS: the name; only m_contact is touched by the code written so far.
struct hkProcessCdPoint {
    hkContactPoint m_contact; // 0x00 (distance in m_contact.m_normal.w)
    u8 unk20[0x10];           // 0x20 not identified
};

// Output of processCollision. Layout from the symmetric processCollision wrappers: the agent appends contact points
// at m_firstFreeContactPoint (0x00) into the inline array (0x10..0x300F), and may record a time of impact in m_toi
// (0x3010). HYPOTHESIS: the array length (256 = (0x3010 - 0x10) / 0x30) and the member names.
struct hkProcessCollisionOutput {
    // Time-of-impact record (0x3010). A wrapper that ran the agent with the bodies swapped turns it back when the agent
    // changed m_time: it negates the contact normal and flips m_gskCache.
    struct ToiInfo {
        hkContactPoint m_contact; // 0x3010
        hkReal m_time;            // 0x3030
        hkReal unk24;             // 0x3034 not identified
        hkGskCache m_gskCache;    // 0x3038
    };

    hkProcessCdPoint* m_firstFreeContactPoint; // 0x0000
    hkProcessCdPoint m_contactPoints[256];     // 0x0010
    ToiInfo m_toi;                             // 0x3010
};
