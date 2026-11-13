// Havok translation unit hkCapsuleShape.o (main.dol 0x802CEB14-0x802D0144).
// Functions in address order:
//   0x802CEB14    32  finishLoadedObjecthkCapsuleShape   [map: hkCapsuleShape__finishLoadedObjecthkCapsuleShape]
//   0x802CEB34    20  cleanupLoadedObjecthkCapsuleShape   [map: hkCapsuleShape__cleanupLoadedObjecthkCapsuleShape]
//   0x802CEB48    92  __dt   [map: hkCapsuleShape____dt]
//   0x802CEBA4    60  getVtablehkCapsuleShape   [map: hkCapsuleShape__getVtablehkCapsuleShape]
//   0x802CEBE0   136  __ct   [map: hkCapsuleShape____ct]
//   0x802CEC68   200  getSupportingVertex   [map: hkCapsuleShape__getSupportingVertex]
//   0x802CED30   288  convertVertexIdsToVertices   [map: hkCapsuleShape__convertVertexIdsToVertices]
//   0x802CEE50    36  getFirstVertex   [map: hkCapsuleShape__getFirstVertex]
//   0x802CEE74     8  getNumVertices   [map: hkCapsuleShape__getNumVertices]
//   0x802CEE7C    20  getCollisionSpheresInfo   [map: hkCapsuleShape__getCollisionSpheresInfo]
//   0x802CEE90    72  getCollisionSpheres   [map: hkCapsuleShape__getCollisionSpheres]
//   0x802CEED8  1120  getAabb   [map: hkCapsuleShape__getAabb]
//   0x802CF338   668  closestInfLineSegInfLineSeg   [map: hkCapsuleShape__closestInfLineSegInfLineSeg]
//   0x802CF5D4   352  closestPointLineSeg   [map: hkCapsuleShape__closestPointLineSeg]
//   0x802CF734  2384  castRay   [map: hkCapsuleShape__castRay]
//   0x802D0084     8  getType   [map: hkCapsuleShape__getType]
//   0x802D008C   100  calcStatistics   [map: hkCapsuleShape__calcStatistics]
//   0x802D00F0    84  __sinit_\hkCapsuleShape_cpp   [map: hkCapsuleShapecpp____sinit_]

#include <havok/hkCapsuleShape.h>
#include <havok/hkRegistry.h>
#include <havok/hkShapeType.h>

void hkCapsuleShape::finishLoadedObjecthkCapsuleShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkCapsuleShape(flag);
}

void hkCapsuleShape::cleanupLoadedObjecthkCapsuleShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkCapsuleShape::~hkCapsuleShape() {
}

#pragma push
#pragma auto_inline off
const void* hkCapsuleShape::getVtablehkCapsuleShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x30] __attribute__((aligned(16)));
    hkCapsuleShape* p = ::new (buf) hkCapsuleShape(flag);
    return *(const void**)p;
}
#pragma pop

#pragma scheduling off
void hkCapsuleShape::getFirstVertex(hkVector4& out) const {
    out.x = m_vertexB.x;
    out.y = m_vertexB.y;
    out.z = m_vertexB.z;
    out.w = m_vertexB.w;
}
#pragma scheduling reset

int hkCapsuleShape::getNumVertices() const {
    return 2;
}

void hkCapsuleShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = 2;
    out->m_flag = 1;
}

// The two capsule end points, copied in order.
#pragma scheduling off
hkVector4* hkCapsuleShape::getCollisionSpheres(hkVector4* out) const {
    hkVector4* result = out;
    out[0] = m_vertexA;
    out[1] = m_vertexB;
    return result;
}
#pragma scheduling reset

int hkCapsuleShape::getType() const {
    return HK_SHAPE_CAPSULE;
}

// Not yet decompiled in this unit:
//   0x802CEBE0   136  __ct (vertexA, vertexB, radius)   [map: hkCapsuleShape____ct]
//   0x802CEC68   200  getSupportingVertex   [map: hkCapsuleShape__getSupportingVertex]
//   0x802CED30   288  convertVertexIdsToVertices   [map: hkCapsuleShape__convertVertexIdsToVertices]
//   0x802CEE90    72  getCollisionSpheres   [map: hkCapsuleShape__getCollisionSpheres]
//   0x802CEED8  1120  getAabb   [map: hkCapsuleShape__getAabb]
//   0x802CF338   668  closestInfLineSegInfLineSeg   [map: hkCapsuleShape__closestInfLineSegInfLineSeg]
//   0x802CF5D4   352  closestPointLineSeg   [map: hkCapsuleShape__closestPointLineSeg]
//   0x802CF734  2384  castRay   [map: hkCapsuleShape__castRay]

static hkStaticTypeInfo hkCapsuleShapeTypeInfo("hkCapsuleShape", hkCapsuleShape::finishLoadedObjecthkCapsuleShape, hkCapsuleShape::cleanupLoadedObjecthkCapsuleShape,
                                      hkCapsuleShape::getVtablehkCapsuleShape());

// Statistics collector slots used by calcStatistics (vtable offsets 0x0C and 0x20); see hkCylinderShape.cpp.
// Shape from https://github.com/jj-link/brawl src/havok/fn_802D008C.cpp.
struct hkStatisticsCollectorIface {
    virtual void unk00();
    virtual void addObject(const char* name, int count, const void* object); // 0x0C
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();                                                    // 0x20
};

// TODO: cleanup (shape from jj-link/brawl fn_802D008C)
void hkCapsuleShape::calcStatistics(hkStatisticsCollector* collector) const {
    ((hkStatisticsCollectorIface*)collector)->addObject("CapsuleShape", 1, this);
    ((hkStatisticsCollectorIface*)collector)->unk20();
}

// fn_802CEC68: body from https://github.com/jj-link/brawl src/havok/fn_802CEC68.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802CEC68 {
#pragma fp_contract on

extern "C" {

extern "C" float lbl_805A4068;

struct Vec4 {
    float x, y, z, w;
};

struct Seg {
    char pad[0x10];
    Vec4 a;
    Vec4 b;
};

struct Out {
    float x, y, z;
    union {
        float w;
        int id;
    };
};

// TODO: cleanup (shape from jj-link/brawl fn_802CEC68)
void fn_802CEC68(Seg* s, Vec4* dir, Out* out) {
    __declspec(align(16)) Vec4 d;
    d.x = s->b.x - s->a.x;
    d.y = s->b.y - s->a.y;
    d.z = s->b.z - s->a.z;
    d.w = s->b.w - s->a.w;
    float dot = d.x * dir->x + d.y * dir->y + d.z * dir->z;
    int id;
    if (dot < lbl_805A4068) {
        out->x = s->a.x;
        id = 0;
        out->y = s->a.y;
        out->z = s->a.z;
        out->w = s->a.w;
    } else {
        out->x = s->b.x;
        id = 0x10;
        out->y = s->b.y;
        out->z = s->b.z;
        out->w = s->b.w;
    }
    out->id = id;
}

}
} // namespace jj_fn_802CEC68

// fn_802CF5D4: body from https://github.com/jj-link/brawl src/havok/fn_802CF5D4.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802CF5D4 {
#pragma fp_contract on
extern "C" float lbl_805A4068;
struct V4 { float x, y, z, w; } __attribute__((aligned(16)));

static inline float dot3(const V4& a, const V4& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// TODO: cleanup (shape from jj-link/brawl fn_802CF5D4)
extern "C" void fn_802CF5D4(const V4* a, const V4* b, const V4* c, V4* out) {
    V4 d, e;
    d.x = b->x - a->x; d.y = b->y - a->y; d.z = b->z - a->z; d.w = b->w - a->w;
    e.x = c->x - b->x; e.y = c->y - b->y; e.z = c->z - b->z; e.w = c->w - b->w;
    float p = dot3(e, d);
    p = -p;
    float l = dot3(e, e);
    if (p <= lbl_805A4068) {
        out->x = b->x; out->y = b->y; out->z = b->z; out->w = b->w;
    } else if (p >= l) {
        out->x = c->x; out->y = c->y; out->z = c->z; out->w = c->w;
    } else {
        p /= l;
        out->x = p * e.x + b->x; out->y = p * e.y + b->y;
        out->z = p * e.z + b->z; out->w = p * e.w + b->w;
    }
}
} // namespace jj_fn_802CF5D4
