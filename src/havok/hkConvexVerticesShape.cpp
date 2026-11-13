// Havok translation unit hkConvexVerticesShape.o (main.dol 0x802D1588-0x802D256C).
// Functions in address order:
//   0x802D1588    32  finishLoadedObjecthkConvexVerticesShape   [map: hkConvexVerticesShape__finishLoadedObjecthkConvexVerticesShape]
//   0x802D15A8    20  cleanupLoadedObjecthkConvexVerticesShape   [map: hkConvexVerticesShape__cleanupLoadedObjecthkConvexVerticesShape]
//   0x802D15BC   196  __dt   [map: hkConvexVerticesShape____dt]
//   0x802D1680    60  getVtablehkConvexVerticesShape   [map: hkConvexVerticesShape__getVtablehkConvexVerticesShape]
//   0x802D16BC     8  getType   [map: hkConvexVerticesShape__getType]
//   0x802D16C4    48  getFirstVertex   [map: hkConvexVerticesShape__getFirstVertex]
//   0x802D16F4    20  getCollisionSpheresInfo   [map: hkConvexVerticesShape__getCollisionSpheresInfo]
//   0x802D1708   452  getCollisionSpheres   [map: hkConvexVerticesShape__getCollisionSpheres]
//   0x802D18CC  1004  getAabb   [map: hkConvexVerticesShape__getAabb]
//   0x802D1CB8   800  getSupportingVertex   [map: hkConvexVerticesShape__getSupportingVertex]
//   0x802D1FD8   324  convertVertexIdsToVertices   [map: hkConvexVerticesShape__convertVertexIdsToVertices]
//   0x802D211C   776  castRay   [map: hkConvexVerticesShape__castRay]
//   0x802D2424   244  calcStatistics   [map: hkConvexVerticesShape__calcStatistics]
//   0x802D2518    84  __sinit_\hkConvexVerticesShape_cpp   [map: hkConvexVerticesShapecpp____sinit_]

#include <havok/hkConvexVerticesShape.h>
#include <havok/hkRegistry.h>
#include <havok/hkShapeType.h>

void hkConvexVerticesShape::finishLoadedObjecthkConvexVerticesShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkConvexVerticesShape(flag);
}

void hkConvexVerticesShape::cleanupLoadedObjecthkConvexVerticesShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkConvexVerticesShape::~hkConvexVerticesShape() {
}

#pragma push
#pragma auto_inline off
const void* hkConvexVerticesShape::getVtablehkConvexVerticesShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x50] __attribute__((aligned(16)));
    hkConvexVerticesShape* p = ::new (buf) hkConvexVerticesShape(flag);
    return *(const void**)p;
}
#pragma pop

int hkConvexVerticesShape::getType() const {
    return HK_SHAPE_CONVEX_VERTICES;
}

void hkConvexVerticesShape::getFirstVertex(hkVector4& out) const {
    const hkConvexVerticesShapeFourVectors* v = (const hkConvexVerticesShapeFourVectors*)m_rotatedVertices.m_data;
    out.x = v->m_x.x;
    out.y = v->m_y.x;
    out.z = v->m_z.x;
    out.w = 1.0f;
}

void hkConvexVerticesShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = m_numVertices;
    out->m_flag = 1;
}

// Not yet decompiled in this unit:
//   0x802D1708   452  getCollisionSpheres   [map: hkConvexVerticesShape__getCollisionSpheres]
//   0x802D18CC  1004  getAabb   [map: hkConvexVerticesShape__getAabb]
//   0x802D1CB8   800  getSupportingVertex   [map: hkConvexVerticesShape__getSupportingVertex]
//   0x802D1FD8   324  convertVertexIdsToVertices   [map: hkConvexVerticesShape__convertVertexIdsToVertices]
//   0x802D211C   776  castRay   [map: hkConvexVerticesShape__castRay]
//   0x802D2424   244  calcStatistics   [map: hkConvexVerticesShape__calcStatistics]

static hkStaticTypeInfo hkConvexVerticesShapeTypeInfo("hkConvexVerticesShape", hkConvexVerticesShape::finishLoadedObjecthkConvexVerticesShape, hkConvexVerticesShape::cleanupLoadedObjecthkConvexVerticesShape,
                                      hkConvexVerticesShape::getVtablehkConvexVerticesShape());

// fn_802D2424: body from https://github.com/jj-link/brawl src/havok/fn_802D2424.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802D2424 {
typedef unsigned int u32;

struct Holder {
    char pad[0x30];
    void* data1;
    u32 count1;
    u32 flags1;
    u32 unk3C;
    void* data2;
    u32 count2;
    u32 flags2;
};

struct Visitor {
    virtual void unused();
    virtual void object(const char*, int, void*);
    virtual void array(const char*, int, void*, u32, u32);
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void finish();
};

extern "C" char lbl_80410618[];

// TODO: cleanup (shape from jj-link/brawl fn_802D2424)
extern "C" void fn_802D2424(Holder* self, Visitor* visitor) {
    visitor->object(lbl_80410618 + 0x12, 1, self);

    const char* name1 = lbl_80410618 + 0x1B;
    if (!(self->flags1 & 0x80000000)) {
        visitor->array(name1, 1, self->data1,
                       self->count1 * 0x30,
                       (self->flags1 & 0x3FFFFFFF) * 0x30);
    }

    const char* name2 = lbl_80410618 + 0x1B;
    if (!(self->flags2 & 0x80000000)) {
        visitor->array(name2, 1, self->data2,
                       self->count2 * 0x10,
                       (self->flags2 & 0x3FFFFFFF) * 0x10);
    }

    visitor->finish();
}
} // namespace jj_fn_802D2424
