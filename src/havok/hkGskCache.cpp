// Havok translation unit hkGskCache.o (main.dol 0x8031C8CC-0x8031FEFC).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x8031C8CC  1176  initTriangle   [map: hkGskCache__initTriangle]
//   0x8031CD64   496  init   [map: hkGskCache__init]
//   0x8031CF54   436  findBestProjection   [map: hkConvexPenetrationUtil__findBestProjection]
//   0x8031D108   424  case0To1   [map: hkConvexPenetrationUtil__case0To1]
//   0x8031D2B0   364  case1To2   [map: hkConvexPenetrationUtil__case1To2]
//   0x8031D41C   676  case2To3   [map: hkConvexPenetrationUtil__case2To3]
//   0x8031D6C0   112  case3To4   [map: hkConvexPenetrationUtil__case3To4]
//   0x8031D730   460  getPointForSimplex   [map: hkConvexPenetrationUtil__getPointForSimplex]
//   0x8031D8FC   176  addPointToSimplex   [map: hkConvexPenetrationUtil__addPointToSimplex]
//   0x8031D9AC   556  tryToExpandPolytope   [map: hkConvexPenetrationUtil__tryToExpandPolytope]
//   0x8031DBD8   776  convertResultToFeature   [map: hkConvexPenetrationUtil__convertResultToFeature]
//   0x8031DEE0  2076  builtTetrahedron   [map: hkConvexPenetrationUtil__builtTetrahedron]
//   0x8031E6FC   836  calculatePenetrationDepth   [map: hkConvexPenetrationUtil__calculatePenetrationDepth]
//   0x8031EA40    76  rand01   [map: hkConvexPenetrationUtil__rand01]
//   0x8031EA8C   156  randomVector   [map: hkConvexPenetrationUtil__randomVector]
//   0x8031EB28   576  hkCalculatePenetrationDepth   [map: hkConvexShape__hkCalculatePenetrationDepth]
//   0x8031ED68  1632  hkCalcMultiPenetrationDepth   [map: hkTransform__hkCalcMultiPenetrationDepth]
//   0x8031F458   516  hkCvxCvxDistByHeuristicSamplingPerformSample   [map: hkConvexShape__hkCvxCvxDistByHeuristicSamplingPerformSample]
//   0x8031F65C  2208  hkCvxCvxDistByHeuristicSampling   [map: hkCvxCvxDistByHeuristicSampling__hkCvxCvxDistByHeuristicSampling]

// fn_8031D6C0: body from https://github.com/jj-link/brawl src/havok/fn_8031D6C0.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_8031D6C0 {
struct Obj {
    char pad[0x40];
    float threshold;
    char pad2[0x24];
    void* c;
};
struct Matrix { float m[4][4]; } __attribute__((aligned(16)));
extern "C" void fn_8031CF54(Obj*, void*, void*, Matrix*, float*);
extern "C" void fn_8031D8FC(Obj*, const Matrix*);
// TODO: cleanup (shape from jj-link/brawl fn_8031D6C0)
extern "C" int fn_8031D6C0(Obj* self, void* arg) {
    Matrix matrix;
    float value;
    fn_8031CF54(self, arg, self->c, &matrix, &value);
    if (value < self->threshold) return 1;
    fn_8031D8FC(self, &matrix);
    return 0;
}
} // namespace jj_fn_8031D6C0

// fn_8031EA8C: body from https://github.com/jj-link/brawl src/havok/fn_8031EA8C.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_8031EA8C {
extern "C" float fn_8031EA40(int*);

// TODO: cleanup (shape from jj-link/brawl fn_8031EA8C)
extern "C" void fn_8031EA8C(int arg0, float* arg1, float farg0, float farg1) {
    int sp8 = arg0;
    arg1[0] = (farg1 - farg0) * (farg0 + fn_8031EA40(&sp8));
    arg1[1] = (farg1 - farg0) * (farg0 + fn_8031EA40(&sp8));
    arg1[2] = (farg1 - farg0) * (farg0 + fn_8031EA40(&sp8));
}
} // namespace jj_fn_8031EA8C

// fn_8031D8FC: body from https://github.com/jj-link/brawl src/havok/fn_8031D8FC.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_8031D8FC {
struct Vec4 {
    float x, y, z, w;
};

struct Obj {
    char pad[0x60];
    Vec4* a;
    Vec4* b;
    Vec4* c;
    int* index;
};

struct Matrix {
    float m[3][4];
};

// TODO: cleanup (shape from jj-link/brawl fn_8031D8FC)
extern "C" void fn_8031D8FC(Obj* self, const Matrix* src) {
    Vec4* e = &self->c[*self->index];
    e->x = src->m[0][0];
    e->y = src->m[0][1];
    e->z = src->m[0][2];
    e->w = src->m[0][3];

    e = &self->a[*self->index];
    e->x = src->m[1][0];
    e->y = src->m[1][1];
    e->z = src->m[1][2];
    e->w = src->m[1][3];

    e = &self->b[*self->index];
    e->x = src->m[2][0];
    e->y = src->m[2][1];
    e->z = src->m[2][2];
    e->w = src->m[2][3];

    ++*self->index;
}
} // namespace jj_fn_8031D8FC

// fn_8031D2B0: body from https://github.com/jj-link/brawl src/havok/fn_8031D2B0.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_8031D2B0 {
#pragma fp_contract on

struct Vec4 {
    float x, y, z, w;
};

struct Matrix {
    Vec4 row[4];
} __attribute__((aligned(16)));

struct Obj {
    char pad[0x68];
    Vec4* pos;
};

extern "C" void fn_8031D730(Obj* self, const Vec4* in, Matrix* out);
extern "C" void fn_8031D8FC(Obj* self, const Matrix* src);

// TODO: cleanup (shape from jj-link/brawl fn_8031D2B0)
extern "C" void fn_8031D2B0(Obj* self) {
    Matrix a;
    Matrix b;
    Vec4 d;
    Vec4 neg;
    Vec4 r;
    Vec4* m;
    float dp;
    float dq;

    m = self->pos;
    fn_8031D730(self, m, &a);
    d.x = a.row[0].x - self->pos->x;
    d.y = a.row[0].y - self->pos->y;
    d.z = a.row[0].z - self->pos->z;
    d.w = a.row[0].w - self->pos->w;
    dp = d.x * d.x + d.y * d.y + d.z * d.z;
    neg.x = -m->x;
    neg.y = -m->y;
    neg.z = -m->z;
    neg.w = -m->w;
    fn_8031D730(self, &neg, &b);
    r.x = b.row[0].x - self->pos->x;
    r.y = b.row[0].y - self->pos->y;
    r.z = b.row[0].z - self->pos->z;
    r.w = b.row[0].w - self->pos->w;
    dq = r.x * r.x + r.y * r.y + r.z * r.z;
    if (dp >= dq) {
        fn_8031D8FC(self, &a);
    } else {
        fn_8031D8FC(self, &b);
    }
}
} // namespace jj_fn_8031D2B0

// fn_8031D9AC: body from https://github.com/jj-link/brawl src/havok/fn_8031D9AC.cpp (namespaced; callee rebound to our C++ name).
#include <types.h>
#include <havok/hkRotation.h>
namespace jj_fn_8031D9AC {
#pragma fp_contract on
struct Vec4 { float x, y, z, w; };
struct Bounds { unsigned char pad[0x40]; float v[4]; };
struct Shape { unsigned char pad[0x14]; Vec4* first; unsigned char pad2[0x0C]; Vec4* second; unsigned char pad3[0x0C]; Vec4* third; };
struct Obj { unsigned char pad[0x0C]; int a; int b; };
extern "C" int fn_8031FEFC(void*, Shape*, Vec4*, int*, int*);
extern "C" int fn_803201D8(void*, int, int, Vec4*);
// TODO: cleanup (shape from jj-link/brawl fn_8031D9AC)
extern "C" int fn_8031D9AC(Bounds* bounds, Obj* object, Shape* shape, Vec4* point, int* result) {
 float transformed[4] __attribute__((aligned(16)));
 float d[3][4];
 int first;
 int second;
 Vec4* p0 = shape->first;
 d[0][0] = point->x - p0->x;
 d[0][1] = point->y - p0->y;
 d[0][2] = point->z - p0->z;
 d[0][3] = point->w - p0->w;
 Vec4* p1 = shape->second;
 d[1][0] = point->x - p1->x;
 d[1][1] = point->y - p1->y;
 d[1][2] = point->z - p1->z;
 d[1][3] = point->w - p1->w;
 Vec4* p2 = shape->third;
 d[2][0] = point->x - p2->x;
 d[2][1] = point->y - p2->y;
 d[2][2] = point->z - p2->z;
 d[2][3] = point->w - p2->w;
 ((hkVector4*)(transformed))->setRotatedInverseDir(*(const hkRotation*)(d[0]), *(const hkVector4*)(shape));
 if (((transformed[0] < bounds->v[0]) << 3 | (transformed[1] < bounds->v[1]) << 2 |
     (transformed[2] < bounds->v[2]) << 1 | (transformed[3] < bounds->v[3])) & 0xe) {
  *result = 0; return 1;
 }
 first = 0;
 if (fn_8031FEFC(object, shape, point, &first, &second)) { *result = 3; return 1; }
 if (0x37 - object->b + object->a < first) { *result = 2; return 1; }
 if (fn_803201D8(object, first, second, point)) { *result = 3; return 1; }
 return 0;
}
} // namespace jj_fn_8031D9AC

// fn_8031CD64: body from https://github.com/jj-link/brawl src/havok/fn_8031CD64.cpp (namespaced; callee rebound to our C++ name).
#include <types.h>
#include <havok/hkRotation.h>
#include <havok/hkTransform.h>
namespace jj_fn_8031CD64 {
struct V4 {
    float x;
    float y;
    float z;
    float w;
};

struct Out {
    short a;
    short b;
    unsigned short c;
    unsigned short d;
    unsigned char e;
    unsigned char f;
    unsigned char g;
    unsigned char h;
};

struct Iface {
    virtual void s0() = 0;
    virtual void s1() = 0;
    virtual void s2() = 0;
    virtual void s3() = 0;
    virtual void s4() = 0;
    virtual void s5() = 0;
    virtual void s6() = 0;
    virtual void s7() = 0;
    virtual void s8() = 0;
    virtual void s9() = 0;
    virtual void get2(V4* a, V4* b) = 0;
    virtual void s11() = 0;
    virtual void get1(V4* a) = 0;
    virtual int get0() = 0;
};

struct Frame {
    V4 v10;
    V4 v20;
    V4 v30;
    V4 v40;
    V4 v50;
    V4 v60;
    V4 v70;
} __attribute__((aligned(16)));


// TODO: cleanup (shape from jj-link/brawl fn_8031CD64)
extern "C" void fn_8031CD64(Out* arg0, Iface* arg1, Iface* arg2, int arg3) {
    Frame f;

    arg1->get1(&f.v70);
    arg2->get1(&f.v60);
    ((hkVector4*)(&f.v50))->setTransformedPos(*(const hkTransform*)(arg3), *(const hkVector4*)(&f.v60));

    f.v40.x = f.v50.x - f.v70.x;
    f.v40.y = f.v50.y - f.v70.y;
    f.v40.z = f.v50.z - f.v70.z;
    f.v40.w = f.v50.w - f.v70.w;

    arg1->get2(&f.v40, &f.v30);
    arg0->a = (short)*(int*)&f.v30.w;

    f.v40.x = f.v50.x - f.v30.x;
    f.v40.y = f.v50.y - f.v30.y;
    f.v40.z = f.v50.z - f.v30.z;
    f.v40.w = f.v50.w - f.v30.w;
    ((hkVector4*)(&f.v20))->setRotatedInverseDir(*(const hkRotation*)(arg3), *(const hkVector4*)(&f.v40));

    f.v20.x = -f.v20.x;
    f.v20.y = -f.v20.y;
    f.v20.z = -f.v20.z;
    f.v20.w = -f.v20.w;

    arg2->get2(&f.v20, &f.v10);
    arg0->b = (short)*(int*)&f.v10.w;
    arg0->c = 0xFFFF;
    arg0->d = 0xFFFF;
    arg0->e = 1;
    arg0->f = 1;
    arg0->g = (unsigned char)arg1->get0();
    arg0->h = (unsigned char)arg2->get0();
}
} // namespace jj_fn_8031CD64
