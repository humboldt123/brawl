// Havok translation unit hkConvexTranslateShape.o (main.dol 0x802D0A94-0x802D1520).
// Functions in address order:
//   0x802D0A94    44  finishLoadedObjecthkConvexTranslateShape   [map: hkConvexTranslateShape__finishLoadedObjecthkConvexTranslateShape]
//   0x802D0AC0    20  cleanupLoadedObjecthkConvexTranslateShape   [map: hkConvexTranslateShape__cleanupLoadedObjecthkConvexTranslateShape]
//   0x802D0AD4   192  __dt   [map: hkConvexTranslateShape____dt]
//   0x802D0B94    72  getVtablehkConvexTranslateShape   [map: hkConvexTranslateShape__getVtablehkConvexTranslateShape]
//   0x802D0BDC     8  getType   [map: hkConvexTranslateShape__getType]
//   0x802D0BE4   300  getAabb   [map: hkConvexTranslateShape__getAabb]
//   0x802D0D10   112  getMaximumProjection   [map: hkConvexTranslateShape__getMaximumProjection]
//   0x802D0D80   492  castRay   [map: hkConvexTranslateShape__castRay]
//   0x802D0F6C   272  castRayWithCollector   [map: hkConvexTranslateShape__castRayWithCollector]
//   0x802D107C   120  getSupportingVertex   [map: hkConvexTranslateShape__getSupportingVertex]
//   0x802D10F4   408  convertVertexIdsToVertices   [map: hkConvexTranslateShape__convertVertexIdsToVertices]
//   0x802D128C   136  getFirstVertex   [map: hkConvexTranslateShape__getFirstVertex]
//   0x802D1314    68  getCollisionSpheresInfo   [map: hkConvexTranslateShape__getCollisionSpheresInfo]
//   0x802D1358   220  getCollisionSpheres   [map: hkConvexTranslateShape__getCollisionSpheres]
//   0x802D1434   152  calcStatistics   [map: hkConvexTranslateShape__calcStatistics]
//   0x802D14CC    84  __sinit_\hkConvexTranslateShape_cpp   [map: hkConvexTranslateShapecpp____sinit_]

#include <havok/hkConvexTranslateShape.h>
#include <havok/hkShapeType.h>
#include <havok/hkCdBody.h>

// Ray cast input (0x28 bytes), declared here because no header for it exists yet. Layout from the copy code:
// ray start and end at 0x00 and 0x10, then two words copied unchanged (HYPOTHESIS: filter and user data).
// MATCH-ONLY-SCOPE: move to include/havok/hkShapeRayCastInput.h when its owner adds one.
struct hkShapeRayCastInput {
    hkVector4 m_from;  // 0x00
    hkVector4 m_to;    // 0x10
    u32 unk20;         // 0x20 HYPOTHESIS
    u32 unk24;         // 0x24 HYPOTHESIS
};

#pragma fp_contract on

void hkConvexTranslateShape::finishLoadedObjecthkConvexTranslateShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkConvexTranslateShape(flag);
}

void hkConvexTranslateShape::cleanupLoadedObjecthkConvexTranslateShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkConvexTranslateShape::~hkConvexTranslateShape() {
}

const void* hkConvexTranslateShape::getVtablehkConvexTranslateShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x30] __attribute__((aligned(16)));
    hkConvexTranslateShape* p = ::new (buf) hkConvexTranslateShape(flag);
    return *(const void**)p;
}

int hkConvexTranslateShape::getType() const {
    return HK_SHAPE_CONVEX_TRANSLATE;
}

void hkConvexTranslateShape::getAabb(const hkTransform& xf, hkReal expansion, hkAabb& out) const {
    m_childShape.m_childShape->getAabb(xf, expansion, out);
    // Rotated translation: xf.rotation * m_translation (the translation part of xf is not used).
    hkVector4 offset;
    offset.setMul4(xf.m_rotation.m_col1, m_translation.y);
    offset.set(xf.m_rotation.m_col0.x * m_translation.x + offset.x,
               xf.m_rotation.m_col0.y * m_translation.x + offset.y,
               xf.m_rotation.m_col0.z * m_translation.x + offset.z, 0.0f);
    offset.set(xf.m_rotation.m_col2.x * m_translation.z + offset.x,
               xf.m_rotation.m_col2.y * m_translation.z + offset.y,
               xf.m_rotation.m_col2.z * m_translation.z + offset.z, 0.0f);
    out.m_min.setAdd4(out.m_min, offset);
    out.m_max.setAdd4(out.m_max, offset);
}

hkReal hkConvexTranslateShape::getMaximumProjection(const hkVector4& direction) const {
    hkReal childProjection = m_childShape.m_childShape->getMaximumProjection(direction);
    return childProjection + m_translation.dot3(direction);
}

// The original also brackets this with the hkMonitorStream timer commands (HK_TIMER); not reproduced here.
// HYPOTHESIS: the child is tested with the ray moved into its space.
hkBool hkConvexTranslateShape::castRay(const hkShapeRayCastInput& input, hkVector4& normalOut) const {
    hkShapeRayCastInput localInput = input;
    localInput.m_from.setSub4(input.m_from, m_translation);
    localInput.m_to.setSub4(input.m_to, m_translation);
    return m_childShape.m_childShape->castRay(localInput, normalOut);
}

void hkConvexTranslateShape::castRayWithCollector(const hkShapeRayCastInput& input, const hkCdBody& body,
                                                  hkShapeRayCastCollector& collector) const {
    hkShapeRayCastInput localInput = input;
    localInput.m_from.setSub4(input.m_from, m_translation);
    localInput.m_to.setSub4(input.m_to, m_translation);
    hkCdBody localBody;
    localBody.m_shape = m_childShape.m_childShape;
    localBody.m_shapeKey = 0;
    localBody.m_motion = body.m_motion;
    localBody.m_parent = const_cast<hkCdBody*>(&body);
    m_childShape.m_childShape->castRayWithCollector(localInput, localBody, collector);
}

void hkConvexTranslateShape::getSupportingVertex(const hkVector4& direction, hkVector4& out) const {
    static_cast<hkConvexShape*>(m_childShape.m_childShape)->getSupportingVertex(direction, out);
    out.x += m_translation.x;
    out.y += m_translation.y;
    out.z += m_translation.z;
}

void hkConvexTranslateShape::convertVertexIdsToVertices(const void* ids, int numIds, hkVector4* out) const {
    static_cast<hkConvexShape*>(m_childShape.m_childShape)->convertVertexIdsToVertices(ids, numIds, out);
    for (int i = 0; i < numIds; i++) {
        out->x += m_translation.x;
        out->y += m_translation.y;
        out->z += m_translation.z;
        out++;
    }
}

void hkConvexTranslateShape::getFirstVertex(hkVector4& out) const {
    static_cast<hkConvexShape*>(m_childShape.m_childShape)->getFirstVertex(out);
    out.x += m_translation.x;
    out.y += m_translation.y;
    out.z += m_translation.z;
    out.w += m_translation.w;
}

void hkConvexTranslateShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    hkCollisionSpheresInfo* info = out;
    static_cast<hkSphereRepShape*>(m_childShape.m_childShape)->getCollisionSpheresInfo(info);
    info->m_flag = 1;
}

hkVector4* hkConvexTranslateShape::getCollisionSpheres(hkVector4* out) const {
    hkVector4* spheres = static_cast<hkSphereRepShape*>(m_childShape.m_childShape)->getCollisionSpheres(out);
    hkCollisionSpheresInfo info;
    static_cast<hkSphereRepShape*>(m_childShape.m_childShape)->getCollisionSpheresInfo(&info);
    for (int i = 0; i < info.m_numSpheres; i++) {
        out[i].setAdd4(spheres[i], m_translation);
    }
    return out;
}

// Statistics collector slots used by calcStatistics (vtable offsets 0x0C, 0x14 and 0x20).
struct hkStatisticsCollectorIface {
    virtual void unk00();
    virtual void addObject(const char* name, int count, const void* object); // 0x0C
    virtual void unk10();
    virtual void addChildObject(const char* name, int count, const void* object); // 0x14
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();                                                    // 0x20
};

void hkConvexTranslateShape::calcStatistics(hkStatisticsCollector* collector) const {
    ((hkStatisticsCollectorIface*)collector)->addObject("CvxTranslate", 1, this);
    ((hkStatisticsCollectorIface*)collector)->addChildObject("Child", 1, m_childShape.m_childShape);
    ((hkStatisticsCollectorIface*)collector)->unk20();
}

// Not yet decompiled in this unit:
//   0x802D0AD4   192  __dt   [map: hkConvexTranslateShape____dt]
//   0x802D0BE4   300  getAabb   [map: hkConvexTranslateShape__getAabb]
//   0x802D0D10   112  getMaximumProjection   [map: hkConvexTranslateShape__getMaximumProjection]
//   0x802D0D80   492  castRay   [map: hkConvexTranslateShape__castRay]
//   0x802D0F6C   272  castRayWithCollector   [map: hkConvexTranslateShape__castRayWithCollector]
//   0x802D107C   120  getSupportingVertex   [map: hkConvexTranslateShape__getSupportingVertex]
//   0x802D10F4   408  convertVertexIdsToVertices   [map: hkConvexTranslateShape__convertVertexIdsToVertices]
//   0x802D128C   136  getFirstVertex   [map: hkConvexTranslateShape__getFirstVertex]
//   0x802D1314    68  getCollisionSpheresInfo   [map: hkConvexTranslateShape__getCollisionSpheresInfo]
//   0x802D1358   220  getCollisionSpheres   [map: hkConvexTranslateShape__getCollisionSpheres]
//   0x802D1434   152  calcStatistics   [map: hkConvexTranslateShape__calcStatistics]
