#pragma once

#include <havok/hkBase.h>
#include <havok/hkMemory.h>
#include <havok/hkVector4.h>
#include <havok/hkTransform.h>
#include <havok/hkAabb.h>
#include <havok/hkShapeType.h>

struct hkShapeContainer;
struct hkShapeRayCastInput;  // ray input (from/to/filter), hkShapeRayCastInput.h
struct hkShapeRayCastOutput; // ray hit (normal, fraction, shape key path), hkShapeRayCastInput.h
struct hkCdBody;            // collidable body; offset 0x08 is its motion state pointer
struct hkShapeRayCastCollector; // HYPOTHESIS: hit collector; slot 0x08 receives hits

// Base collision shape (0x0C bytes). Layout from hkShapeClass.cpp. Vtable slots (byte offset):
// 0x08 dtor, 0x0C calcStatistics (inherited), 0x10 getType, 0x14 getAabb, 0x18 getMaximumProjection,
// 0x1C castRay, 0x20 castRayWithCollector, 0x24 getContainer.
struct hkShape : hkReferencedObject {
    void* m_userData; // 0x08

    // Shapes are freed as class 0x25 (no shape deleting destructor in the image calls the global delete).
    HK_DECLARE_REF_ALLOCATOR(0x25)

    virtual int getType() const;                                              // 0x10
    virtual void getAabb(const hkTransform& xf, hkReal expansion, hkAabb& out) const; // 0x14
    virtual hkReal getMaximumProjection(const hkVector4& direction) const;   // 0x18
    virtual hkBool castRay(const hkShapeRayCastInput& input, hkShapeRayCastOutput& results) const; // 0x1C
    virtual void castRayWithCollector(const hkShapeRayCastInput& input, const hkCdBody& body,
                                      hkShapeRayCastCollector& collector) const; // 0x20
    virtual hkShapeContainer* getContainer() const;                           // 0x24
};
