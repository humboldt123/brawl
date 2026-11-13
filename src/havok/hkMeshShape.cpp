// Havok translation unit hkMeshShape.o (main.dol 0x802D51F4-0x802D5CAC).
// Functions in address order:
//   0x802D51F4    52  finishLoadedObjecthkMeshShape   [map: hkMeshShape__finishLoadedObjecthkMeshShape]
//   0x802D5228    20  cleanupLoadedObjecthkMeshShape   [map: hkMeshShape__cleanupLoadedObjecthkMeshShape]
//   0x802D523C    72  getVtablehkMeshShape   [map: hkMeshShape__getVtablehkMeshShape]
//   0x802D5284   112  __ct   [map: hkMeshShape____ct]
//   0x802D52F4     8  getType   [map: hkMeshShape__getType]
//   0x802D52FC   168  getFirstKey   [map: hkMeshShape__getFirstKey]
//   0x802D53A4   252  getNextKey   [map: hkMeshShape__getNextKey]
//   0x802D54A0   508  getChildShape   [map: hkMeshShape__getChildShape]
//   0x802D569C   152  getCollisionFilterInfo   [map: hkMeshShape__getCollisionFilterInfo]
//   0x802D5734   348  addToAabb   [map: hkAabb__addToAabb]
//   0x802D5890   544  getAabb   [map: hkMeshShape__getAabb]
//   0x802D5AB0   244  addSubpart   [map: hkMeshShape__addSubpart]
//   0x802D5BA4   180  calcStatistics   [map: hkMeshShape__calcStatistics]
//   0x802D5C58    84  __sinit_\hkMeshShape_cpp   [map: hkMeshShapecpp____sinit_]

#include <havok/hkMeshShape.h>
#include <havok/hkRegistry.h>
#include <havok/hkShapeType.h>

void hkMeshShape::finishLoadedObjecthkMeshShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkMeshShape(flag);
}

void hkMeshShape::cleanupLoadedObjecthkMeshShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkMeshShape::~hkMeshShape() {
}

#pragma push
#pragma auto_inline off
const void* hkMeshShape::getVtablehkMeshShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x50] __attribute__((aligned(16)));
    hkMeshShape* p = ::new (buf) hkMeshShape(flag);
    return *(const void**)p;
}
#pragma pop

hkMeshShape::hkMeshShape(const hkFinishLoadedObjectFlag& flag) {
    if (flag.m_finishing == 0) {
        return;
    }
    // Subparts loaded without a material index striding type get the default (int8).
    hkMeshShapeSubpart* subparts = (hkMeshShapeSubpart*)m_subparts.m_data;
    for (int i = 0; i < m_subparts.m_size; i++) {
        if (subparts[i].m_materialIndexStridingType == 0) {
            subparts[i].m_materialIndexStridingType = 1;
        }
    }
}


int hkMeshShape::getType() const {
    return HK_SHAPE_TRIANGLE_COLLECTION;
}

// Not yet decompiled in this unit:
//   0x802D5284   112  __ct   [map: hkMeshShape____ct]
//   0x802D52FC   168  getFirstKey   [map: hkMeshShape__getFirstKey]
//   0x802D53A4   252  getNextKey   [map: hkMeshShape__getNextKey]
//   0x802D54A0   508  getChildShape   [map: hkMeshShape__getChildShape]
//   0x802D569C   152  getCollisionFilterInfo   [map: hkMeshShape__getCollisionFilterInfo]
//   0x802D5734   348  addToAabb   [map: hkAabb__addToAabb]
//   0x802D5890   544  getAabb   [map: hkMeshShape__getAabb]
//   0x802D5AB0   244  addSubpart   [map: hkMeshShape__addSubpart]
//   0x802D5BA4   180  calcStatistics   [map: hkMeshShape__calcStatistics]

static hkStaticTypeInfo hkMeshShapeTypeInfo("hkMeshShape", hkMeshShape::finishLoadedObjecthkMeshShape, hkMeshShape::cleanupLoadedObjecthkMeshShape,
                                      hkMeshShape::getVtablehkMeshShape());

// fn_802D5BA4: body from https://github.com/jj-link/brawl src/havok/fn_802D5BA4.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802D5BA4 {
extern "C" char lbl_80410C88[];

struct Obj {
    virtual void v0();
    virtual void v1(const char*, int, void*);
    virtual void v2(const char*, int, int, int, int);
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
};

struct Data {
    char pad[0x34];
    int a;
    int b;
    unsigned c;
};

// TODO: cleanup (shape from jj-link/brawl fn_802D5BA4)
extern "C" void fn_802D5BA4(Data* d, Obj* o) {
    o->v1(lbl_80410C88, 1, d);
    unsigned c = d->c;
    const char* s = lbl_80410C88 + 10;
    if ((c & 0x80000000) == 0) {
        o->v2(s, 1, d->a, d->b * 0x30, (c & 0x3fffffff) * 0x30);
    }
    o->v6();
}
} // namespace jj_fn_802D5BA4

// fn_802D5AB0: body from https://github.com/jj-link/brawl src/havok/fn_802D5AB0.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_802D5AB0 {
struct Entry {
    int a, b, c, d;
    unsigned char e, f;
    int g, h;
    float* p;
    int i;
    float* q;
    int j, k;
};

struct Vec {
    Entry* data;
    int size;
    unsigned int capacity;
};

struct Object {
    unsigned char pad[0x34];
    Vec vec;
};

extern "C" float lbl_80532550[4];

// TODO: cleanup (shape from jj-link/brawl fn_802D5AB0)
extern "C" void fn_802D5AB0(Object* self, const Entry* src) {
    if (self->vec.size == (int)(self->vec.capacity & 0x3FFFFFFF)) {
        hkArrayUtil::_reserveMore((hkArrayBase*)(&self->vec), 0x30);
    }

    Entry* dst = &self->vec.data[self->vec.size++];
    dst->a = src->a;
    dst->b = src->b;
    dst->c = src->c;
    dst->d = src->d;
    dst->e = src->e;
    dst->f = src->f;
    dst->g = src->g;
    dst->h = src->h;
    dst->p = src->p;
    dst->i = src->i;
    dst->q = src->q;
    dst->j = src->j;
    dst->k = src->k;

    if (dst->p == 0) {
        dst->k = 1;
        dst->q = lbl_80532550;
        dst->p = lbl_80532550;
    }
}
} // namespace jj_fn_802D5AB0
