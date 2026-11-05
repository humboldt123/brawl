#pragma once

#include <havok/hkProcessCollisionOutput.h>
#include <havok/hkBvAgent.h>

// Body of the symmetric agents' processCollision (map name hkSymmetricAgentLinearCast<T>::processCollision, one
// identical 412-byte copy per agent unit). Runs T's processCollision with the bodies swapped, then turns the new
// contact points back (each point moves along its normal by the distance to the other body's surface and the normal
// flips) and, when the agent recorded a time of impact, its contact normal and GSK cache.
// NONMATCHING: the time-of-impact block is scheduled differently (the target orders the cache byte swaps against the
// normal negation; see havok/PARKED.md), the contact loop matches.
template <class T>
inline void hkSymmetricAgent_processCollision(T* agent, void* a, void* b, void* c, void* d) {
    hkProcessCollisionOutput* output = (hkProcessCollisionOutput*)d;
    hkProcessCdPoint* cp = output->m_firstFreeContactPoint;
    hkReal toiTime = output->m_toi.m_time;
    agent->T::processCollision(b, a, c, d);
    for (; cp < output->m_firstFreeContactPoint; cp++) {
        hkVector4 dist;
        dist.setAll(cp->m_contact.m_normal.w);
        cp->m_contact.m_position.x += cp->m_contact.m_normal.x * dist.x;
        cp->m_contact.m_position.y += cp->m_contact.m_normal.y * dist.y;
        cp->m_contact.m_position.z += cp->m_contact.m_normal.z * dist.z;
        cp->m_contact.m_position.w += cp->m_contact.m_normal.w * dist.w;
        cp->m_contact.m_normal.x = -cp->m_contact.m_normal.x;
        cp->m_contact.m_normal.y = -cp->m_contact.m_normal.y;
        cp->m_contact.m_normal.z = -cp->m_contact.m_normal.z;
    }
    if (toiTime != output->m_toi.m_time) {
        output->m_toi.m_contact.m_normal.x = -output->m_toi.m_contact.m_normal.x;
        output->m_toi.m_contact.m_normal.y = -output->m_toi.m_contact.m_normal.y;
        output->m_toi.m_contact.m_normal.z = -output->m_toi.m_contact.m_normal.z;
        output->m_toi.m_gskCache.flip();
    }
}

template <class T>
void hkSymmetricAgentLinearCast<T>::processCollision(void* a, void* b, void* c, void* d) {
    hkSymmetricAgent_processCollision<T>(this, a, b, c, d);
}
