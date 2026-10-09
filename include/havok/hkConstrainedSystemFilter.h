#pragma once

#include <havok/hkCollisionFilter.h>
#include <havok/hkMemory.h>

// HYPOTHESIS: fifth base of hkConstrainedSystemFilter, a listener for constraint add/remove events. Only the
// vtable shape (virtual destructor plus the two callbacks) is recovered; the parameter types are not.
struct hkConstraintListenerInterface {
    virtual ~hkConstraintListenerInterface() {}
    virtual void constraintAddedCallback(const void* constraint) = 0;
    virtual void constraintRemovedCallback(const void* constraint) = 0;
};

// Collision filter that forwards to a wrapped filter and listens for constraint events (0x20 bytes).
// Layout from the Havok TU map (hkConstrainedSystemFilter__*). Six vptrs: the four hkCollisionFilter
// interfaces, the constraint listener, and the primary vtable at offset 0.
struct hkConstrainedSystemFilter : hkCollisionFilter, hkConstraintListenerInterface {
    HK_DECLARE_REF_ALLOCATOR(0x25)

    hkCollisionFilter* m_childFilter; // 0x1C; HYPOTHESIS: the wrapped filter, reference counted

    hkConstrainedSystemFilter(hkCollisionFilter* childFilter);
    hkConstrainedSystemFilter(hkFinishLoadedObjectFlag flag) {}
    virtual ~hkConstrainedSystemFilter();

    static void finishLoadedObjecthkConstrainedSystemFilter(void* p);
    static void cleanupLoadedObjecthkConstrainedSystemFilter(void* p);
    static const void* getVtablehkConstrainedSystemFilter();

    virtual void constraintAddedCallback(const void* constraint);
    virtual void constraintRemovedCallback(const void* constraint);

    // Forward to the wrapped filter (its virtual slots 0x54, 0x58, 0x5c); true when there is no child.
    hkBool isCollisionEnabled1(const void* a, const void* b) const;
    hkBool isCollisionEnabled2(const void* a, const void* b) const;
    hkBool isCollisionEnabled3(const void* a, const void* b) const;
};
