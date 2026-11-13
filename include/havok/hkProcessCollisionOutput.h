#pragma once

#include <havok/hkContactPoint.h>
#include <havok/hkGskCache.h>
#include <havok/hkCdPoint.h>

// Contact point written by an agent's processCollision (0x30 bytes, the stride of the symmetric wrappers' loop).
// HYPOTHESIS: the name; only m_contact is touched by the code written so far.
struct hkProcessCdPoint {
    hkContactPoint m_contact; // 0x00 (distance in m_contact.m_normal.w)
    u16 m_contactPointId;     // 0x20 HYPOTHESIS: the name (copied as a halfword when a point is moved)
    u8 unk22[0x0E];           // 0x22 not identified (copied bytewise)
};

// Output of processCollision. Layout from the symmetric processCollision wrappers: the agent appends contact points
// at m_firstFreeContactPoint (0x00) into the inline array (0x10..0x300F), and may record a time of impact in m_toi
// (0x3010). HYPOTHESIS: the array length (256 = (0x3010 - 0x10) / 0x30) and the member names.
// The data members sit in the base hkProcessCollisionData (map name hkProcessCollisionData::getNumContactPoints).
struct hkProcessCollisionData {
    // Time-of-impact record (0x3010). A wrapper that ran the agent with the bodies swapped turns it back when the agent
    // changed m_time: it negates the contact normal and flips m_gskCache.
    struct ToiInfo {
        hkContactPoint m_contact; // 0x3010
        hkReal m_time;            // 0x3030
        hkReal unk24;             // 0x3034 not identified
        hkGskCache m_gskCache;    // 0x3038
    };

    // Potential contacts (0x3050). HYPOTHESIS: the names; the agent3 process functions append the first of the
    // points they just added to the representative list at 0x04.
    struct PotentialInfo {
        hkProcessCdPoint* m_firstFreePotentialContact;       // 0x00
        hkProcessCdPoint** m_firstFreeRepresentativeContact; // 0x04
    };

    hkProcessCdPoint* m_firstFreeContactPoint; // 0x0000
    hkProcessCdPoint m_contactPoints[256];     // 0x0010
    ToiInfo m_toi;                             // 0x3010
    PotentialInfo* m_potentialContacts;        // 0x3050 cleared by reset

    // Empties the output before an agent runs: no contact points, no time of impact.
    // HYPOTHESIS: the names of reset and isEmpty (hkSimulation::processAgentCollideDiscrete inlines both).
    void reset() {
        m_firstFreeContactPoint = m_contactPoints;
        m_toi.m_time = HK_REAL_MAX;
        m_potentialContacts = 0;
    }
    hkBool isEmpty() const { return m_firstFreeContactPoint == m_contactPoints; }
    // Out-of-line copy in hkBoxBoxCollisionDetection.o.
    int getNumContactPoints() const { return m_firstFreeContactPoint - m_contactPoints; }
};

struct hkProcessCollisionOutput : hkProcessCollisionData {
    // Out of line (processAgentsOfEntities calls it): only the time of impact is initialised, to the largest float.
    hkProcessCollisionOutput();
};
