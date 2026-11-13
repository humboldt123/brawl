// Havok translation unit hkMoppAabbCastVirtualMachine.o (main.dol 0x8031FEFC-0x803227F4).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x8031FEFC   268  findVisibilityBoundaryEdges   [map: hkWingedEdge__findVisibilityBoundaryEdges]
//   0x80320008   464  findVisibleTriangles   [map: hkWingedEdge__findVisibleTriangles]
//   0x803201D8   176  patchInBoundaryToPointOfViewExpansion   [map: hkWingedEdge__patchInBoundaryToPointOfViewExpansion]
//   0x80320288   632  allocateTriangles   [map: hkWingedEdge__allocateTriangles]
//   0x80320500   832  initializePolytopeExpansion   [map: hkWingedEdge__initializePolytopeExpansion]
//   0x80320840   632  addTetrahedron   [map: hkWingedEdge__addTetrahedron]
//   0x80320AB8   396  calculateTriangleData   [map: hkWingedEdge8TriangleFv__calculateTriangleData]
//   0x80320C44   568  findTriangleNearestOrigin   [map: hkWingedEdge__findTriangleNearestOrigin]
//   0x80320E7C   768  find2ClosestMultiTriangles   [map: hkWingedEdge__find2ClosestMultiTriangles]
//   0x8032117C  5140  queryRayOnTree   [map: hkMoppAabbCastVirtualMachine__queryRayOnTree]
//   0x80322590   600  aabbCast   [map: hkMoppAabbCastVirtualMachine__aabbCast]
//   0x803227E8    12  __sinit_\hkMoppAabbCastVirtualMachine_cpp   [map: hkMoppAabbCastVirtualMachinecpp____sinit_]

#include <havok/hkBase.h>

// Unnamed static flag (lbl_805A0F28); initialised through hkBool(bool) so the TU gets a __sinit.
// HYPOTHESIS: a debug/config switch of the unit; nothing in the written code reads it yet.
static hkBool s_hkMoppAabbCastVirtualMachine_unk(false);

// fn_80320AB8: body from https://github.com/jj-link/brawl src/havok/fn_80320AB8.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_80320AB8 {
#pragma fp_contract on

extern "C" float lbl_805A4750;
extern "C" float lbl_805A4754;
extern "C" float lbl_805A4758;

struct V4 { float x, y, z, w; } __attribute__((aligned(16)));

struct Plane {
    float x, y, z, w;
    float d;
    V4* a;
    char pad18[0xC];
    V4* b;
    char pad28[0xC];
    V4* c;
};

// TODO: cleanup (shape from jj-link/brawl fn_80320AB8)
extern "C" void fn_80320AB8(Plane* p) {
    V4* a = p->a;
    V4* b = p->b;
    V4* c = p->c;
    V4 e1, e2;
    e1.x = a->x - b->x; e1.y = a->y - b->y; e1.z = a->z - b->z; e1.w = a->w - b->w;
    e2.x = b->x - c->x; e2.y = b->y - c->y; e2.z = b->z - c->z; e2.w = b->w - c->w;
    float nz, ny, nx;
    nx = e1.y * e2.z - e1.z * e2.y;
    ny = e1.z * e2.x - e1.x * e2.z;
    nz = e1.x * e2.y - e1.y * e2.x;
    float l = nx * nx + ny * ny + nz * nz;
    float r = lbl_805A4750;
    p->x = nx;
    p->y = ny;
    p->z = nz;
    p->w = r;
    if (l != r) {
        if (l <= r) {
            union { int i; float f; } u;
            u.i = 0x7f800000;
            r = u.f;
        } else {
            double d = __frsqrte(l);
            float e = (float)d;
            r = lbl_805A4754 * e * (lbl_805A4758 - e * (l * e));
        }
    }
    p->x *= r;
    p->y *= r;
    p->z *= r;
    p->w *= r;
    V4* q = p->a;
    p->d = p->x * q->x + p->y * q->y + p->z * q->z;
}
} // namespace jj_fn_80320AB8

// fn_80320008: body from https://github.com/jj-link/brawl src/havok/fn_80320008.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_80320008 {
#pragma fp_contract on
extern "C" float lbl_805A4748;
extern "C" float lbl_805A474C;
struct Vec4 { float x, y, z, w; } __attribute__((aligned(16)));
struct Node;
struct Link;
struct Owner { char pad[0x30]; Link* link; };
struct Link { Owner* owner; char pad[8]; Node* node; };
struct Node {
    float x, y, z;
    char pad0[4];
    float distance;
    Vec4* point;
    char pad1[0x2c];
    int state;
};
struct Context {
    float min, max;
    char pad[4];
    int count;
    char pad2[0xdd0];
    Node* nodes[1];
};
// TODO: cleanup (shape from jj-link/brawl fn_80320008)
extern "C" int fn_80320008(Context* ctx, Node* node, Vec4* point, int* count, Link** output) {
    float epsilon = lbl_805A4748;
    int result = 0;
    int i = 0;

    for (; i < 3; ++i) {
        Link* link = *(Link**)((char*)node + (i << 4) + 0x1c);
        Node* child = link->node;
        if (child->state == 0) {
            Vec4 delta;
            Vec4* p = child->point;
            int state = 1;
            delta.x = point->x - p->x;
            delta.y = point->y - p->y;
            delta.z = point->z - p->z;
            delta.w = point->w - p->w;
            float d = child->z * delta.z + (child->x * delta.x + child->y * delta.y);
            if (d > ctx->min) {
                if (d < ctx->max) ctx->max = d;
            } else {
                if (epsilon + d > ctx->min) ctx->min = epsilon + d;
                state = 2;
            }
            if (ctx->min >= ctx->max) state = 4;
            child->state = state;
            if (state == 1) {
                result = fn_80320008(ctx, child, point, count, output);
                if (result == 2) break;
            } else if (child->state == 4) {
                result = 2;
                break;
            }
        }
        if (child->state == 2) {
            link->owner->link = link;
            ++*count;
            *output = link;
        }
    }
    node->distance = lbl_805A474C;
    ctx->nodes[ctx->count++] = node;
    return result;
}
} // namespace jj_fn_80320008

// fn_803201D8: body from https://github.com/jj-link/brawl src/havok/fn_803201D8.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_803201D8 {
extern "C" {
void fn_80320288(int, void*, int, int);
}

struct V {
    void* p; int n; unsigned int c;
    bool isOwned() const { return (c & 0x80000000) == 0; }
    void dealloc() { hkThreadMemory::s_instance->deallocateChunk(p, c * 4, 0x15); }
};
extern "C" int fn_80320500(int, V*, int, int, int*);

// TODO: cleanup (shape from jj-link/brawl fn_803201D8)
extern "C" int fn_803201D8(int a0, int a1, int a2, int a3) {
    int sp8 = a3;
    char buf[0x190];
    V v;
    v.p = buf; v.n = 0; v.c = 0x80000064;
    fn_80320288(a0, &v, a1, a3);
    int r = fn_80320500(a0, &v, a1, a2, &sp8);
    if (v.isOwned()) v.dealloc();
    return r;
}
} // namespace jj_fn_803201D8

// TODO: cleanup (shape from jj-link/brawl fn_80320288)
// fn_80320288: body from https://github.com/jj-link/brawl src/havok/fn_80320288.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
namespace jj_fn_80320288 {
struct Item { char data[0x50]; };
struct Pool {
    char pad[0xc];
    int freeCount;
    int usedCount;
    char pad2[0xdcc];
    Item* freeItems[100];
    Item items[1];
};
struct Array { Item** data; int size; };
extern "C" void fn_80320288(Pool* pool, Array* out, int count) {
    int i = pool->freeCount - 1;
    for (; count != 0 && i >= 0; --count) {
        out->data[out->size++] = pool->freeItems[i];
        --i;
        --pool->freeCount;
    }
    for (int i = 0; i < count; ++i) {
        out->data[out->size++] = &pool->items[pool->usedCount++];
    }
}
} // namespace jj_fn_80320288
