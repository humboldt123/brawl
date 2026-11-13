// Havok translation unit hkTypedBroadPhaseDispatcher.o (main.dol 0x802CAA50-0x802CB41C), carved from an unclaimed gap.
// Unit name is scaffolding: the original file and the owning classes are not identified yet.
// Functions in address order:
//   0x802CAA50   208  fn_802CAA50
//   0x802CAB20   220  fn_802CAB20
//   0x802CABFC    80  fn_802CABFC
//   0x802CAC4C    20  fn_802CAC4C
//   0x802CAC60    32  fn_802CAC60
//   0x802CAC80    24  fn_802CAC80
//   0x802CAC98    24  fn_802CAC98
//   0x802CACB0    24  fn_802CACB0
//   0x802CACC8    24  fn_802CACC8
//   0x802CACE0    24  fn_802CACE0
//   0x802CACF8     4  fn_802CACF8
//   0x802CACFC     4  fn_802CACFC
//   0x802CAD00    92  fn_802CAD00
//   0x802CAD5C   260  fn_802CAD5C
//   0x802CAE60   136  fn_802CAE60
//   0x802CAEE8   216  addPairs__27hkTypedBroadPhaseDispatcherFP22hkBroadPhaseHandlePairiPC28hkCollidableCollidableFilter
//   0x802CAFC0   140  removePairs__27hkTypedBroadPhaseDispatcherFP22hkBroadPhaseHandlePairi
//   0x802CB04C   976  removeDuplicates__27hkTypedBroadPhaseDispatcherFR33hkArray<22hkBroadPhaseHandlePair>R33hkArray<22hkBroadPhaseHandlePair>

#include <types.h>
#include <havok/hkCollisionDispatcher.h>

// fn_802CAA50: body from https://github.com/jj-link/brawl src/havok/fn_802CAA50.cpp (namespaced).
namespace jj_fn_802CAA50 {
typedef void (*F)();
extern "C" {
void fn_802CAB20();
void fn_802CABFC();
void fn_802CAC4C();
void fn_802CAC60();
void fn_802CAC80();
void fn_802CAC98();
void fn_802CACB0();
void fn_802CACC8();
void fn_802CACE0();
void fn_802CC214(void*, F*, int, int);

// TODO: cleanup (shape from jj-link/brawl fn_802CAA50)
extern "C" void fn_802CAA50(void* p) {
    struct S {
        F f[9];
        int a;
        F h;
        char c1;
        char c2;
    } s;

    s.c2 = 0;
    s.f[4] = fn_802CACC8;
    s.f[5] = fn_802CACE0;
    s.f[3] = fn_802CACB0;
    s.f[0] = fn_802CAB20;
    s.h = fn_802CABFC;
    s.a = 0;
    s.f[2] = 0;
    s.f[1] = fn_802CAC4C;
    s.f[6] = fn_802CAC60;
    s.f[7] = fn_802CAC80;
    s.f[8] = fn_802CAC98;
    s.c1 = 1;
    ((hkCollisionDispatcher*)p)->registerAgent3(*(const hkAgent3Funcs*)s.f, -1, -1);
}
}
} // namespace jj_fn_802CAA50

// fn_802CABFC: body from https://github.com/jj-link/brawl src/havok/fn_802CABFC.cpp (namespaced).
namespace jj_fn_802CABFC {
class Inner {
public:
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f7(void* a, void* b, void* c);
};

struct Holder {
    void* vptr;
    Inner* inner;
};

struct Args {
    void* a;
    void* b;
    void* c;
};

// TODO: cleanup (shape from jj-link/brawl fn_802CABFC)
extern "C" int fn_802CABFC(Args arg0, Holder* arg1, int arg2) {
    arg1->inner->f7(arg0.a, arg0.b, arg0.c);
    return arg2;
}
} // namespace jj_fn_802CABFC

// fn_802CAC4C: body from https://github.com/jj-link/brawl src/havok/fn_802CAC4C.cpp (namespaced).
namespace jj_fn_802CAC4C {
struct Inner {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
};

struct Outer {
    char pad[4];
    Inner* m_inner;
};

// TODO: cleanup (shape from jj-link/brawl fn_802CAC4C)
extern "C" void fn_802CAC4C(Outer* p) {
    p->m_inner->v6();
}
} // namespace jj_fn_802CAC4C

// fn_802CAC60: body from https://github.com/jj-link/brawl src/havok/fn_802CAC60.cpp (namespaced).
namespace jj_fn_802CAC60 {
struct acInnerBase {
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual int vf7(int a, int b, int c);
};

struct acOuter {
    int unk0;
    acInnerBase* m_inner;
};

extern "C" {
// TODO: cleanup (shape from jj-link/brawl fn_802CAC60)
extern "C" int fn_802CAC60(acOuter* self, int unused, int a, int b, int c) {
    return self->m_inner->vf7(a, b, c);
}
}
} // namespace jj_fn_802CAC60

// fn_802CAC80: body from https://github.com/jj-link/brawl src/havok/fn_802CAC80.cpp (namespaced).
namespace jj_fn_802CAC80 {
class Inner {
public:
    virtual void v00() = 0;
    virtual void v01() = 0;
    virtual void v02() = 0;
    virtual void v03() = 0;
    virtual void v04() = 0;
    virtual void v05() = 0;
    virtual void v06() = 0;
    virtual void v07() = 0;
    virtual void v08(int x) = 0;
    virtual void v09() = 0;
    virtual void v10() = 0;
};

struct Outer {
    int pad;
    Inner* inner;
};

// TODO: cleanup (shape from jj-link/brawl fn_802CAC80)
extern "C" void fn_802CAC80(Outer* o, int a, int b) {
    o->inner->v08(b);
}
} // namespace jj_fn_802CAC80

// fn_802CAC98: body from https://github.com/jj-link/brawl src/havok/fn_802CAC98.cpp (namespaced).
namespace jj_fn_802CAC98 {
class Inner {
public:
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual int v11(int a);
};

struct Outer {
    int pad;
    Inner* inner;
};

// TODO: cleanup (shape from jj-link/brawl fn_802CAC98)
extern "C" int fn_802CAC98(Outer* o, int a, int b) {
    return o->inner->v11(b);
}
} // namespace jj_fn_802CAC98

// fn_802CACB0: body from https://github.com/jj-link/brawl src/havok/fn_802CACB0.cpp (namespaced).
namespace jj_fn_802CACB0 {
class Inner {
public:
    virtual void v02() = 0;
    virtual void v03() = 0;
    virtual void v04() = 0;
    virtual void v05() = 0;
    virtual void v06() = 0;
    virtual void v07() = 0;
    virtual void v08() = 0;
    virtual void v09() = 0;
    virtual void v10() = 0;
    virtual void v11() = 0;
    virtual int v12(int b) = 0;
};

struct Outer {
    int pad;
    Inner* inner;
};

// TODO: cleanup (shape from jj-link/brawl fn_802CACB0)
extern "C" int fn_802CACB0(Outer* o, int a, int b) {
    return o->inner->v12(b);
}
} // namespace jj_fn_802CACB0

// fn_802CACC8: body from https://github.com/jj-link/brawl src/havok/fn_802CACC8.cpp (namespaced).
namespace jj_fn_802CACC8 {
class Inner {
public:
    virtual void v00() = 0;
    virtual void v01() = 0;
    virtual void v02() = 0;
    virtual void v03() = 0;
    virtual void v04() = 0;
    virtual void v05() = 0;
    virtual void v06() = 0;
    virtual void v07() = 0;
    virtual void v08() = 0;
    virtual void v09() = 0;
    virtual void v10() = 0;
    virtual void v11(int) = 0;
};

struct Outer {
    int pad;
    Inner* inner;
};

// TODO: cleanup (shape from jj-link/brawl fn_802CACC8)
extern "C" void fn_802CACC8(Outer* o, int unused, int arg) {
    o->inner->v11(arg);
}
} // namespace jj_fn_802CACC8

// fn_802CACE0: body from https://github.com/jj-link/brawl src/havok/fn_802CACE0.cpp (namespaced).
namespace jj_fn_802CACE0 {
class Inner {
public:
    virtual void v00() = 0;
    virtual void v01() = 0;
    virtual void v02() = 0;
    virtual void v03() = 0;
    virtual void v04() = 0;
    virtual void v05() = 0;
    virtual void v06() = 0;
    virtual void v07() = 0;
    virtual void v08() = 0;
    virtual void v09() = 0;
    virtual void v10() = 0;
    virtual void v11() = 0;
    virtual void v12(int a) = 0;
};

struct Outer {
    int pad;
    Inner* inner;
};

// TODO: cleanup (shape from jj-link/brawl fn_802CACE0)
extern "C" void fn_802CACE0(Outer* o, int a, int b) {
    o->inner->v12(b);
}
} // namespace jj_fn_802CACE0

// fn_802CACF8: body from https://github.com/jj-link/brawl src/havok/fn_802CACF8.cpp (namespaced).
namespace jj_fn_802CACF8 {
// TODO: cleanup (shape from jj-link/brawl fn_802CACF8)
extern "C" void fn_802CACF8(void) {}
} // namespace jj_fn_802CACF8

// fn_802CACFC: body from https://github.com/jj-link/brawl src/havok/fn_802CACFC.cpp (namespaced).
namespace jj_fn_802CACFC {
// TODO: cleanup (shape from jj-link/brawl fn_802CACFC)
extern "C" void fn_802CACFC(void) {}
} // namespace jj_fn_802CACFC

// fn_802CAD00: body from https://github.com/jj-link/brawl src/havok/fn_802CAD00.cpp (namespaced).
#include <havok/hkMemory.h>
namespace jj_fn_802CAD00 {
class SoHeapAllocator {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void free(void* ptr, u16 type, s32 align);
};

struct DtorObj {
    void* vtable;
    u16 type;
};

// TODO: cleanup (shape from jj-link/brawl fn_802CAD00)
extern "C" void* fn_802CAD00(void* arg0, s32 arg1) {
    DtorObj* self = (DtorObj*)arg0;
    if (self != 0 && arg1 > 0) {
        ((SoHeapAllocator*)hkMemory::s_instance)->free(self, self->type, 0x1f);
    }
    return self;
}
} // namespace jj_fn_802CAD00

// fn_802CAD5C: body from https://github.com/jj-link/brawl src/havok/fn_802CAD5C.cpp (namespaced).
#include <havok/hkMemory.h>
namespace jj_fn_802CAD5C {
struct Node {
    void* vtable;
    unsigned short size;
    unsigned short refs;
};

struct SoHeapAllocator {
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual Node* alloc(unsigned long, int);
};
extern "C" int lbl_80487290[6];

struct Object {
    Node* entries[64];
    Node* value;
};

// TODO: cleanup (shape from jj-link/brawl fn_802CAD5C)
extern "C" Object* fn_802CAD5C(Object* self) {
    Node* node = ((SoHeapAllocator*)hkMemory::s_instance)->alloc(8, 0x1F);
    node->size = 8;
    if (node) {
        node->refs = 1;
        node->vtable = &lbl_80487290;
    }
    self->value = node;
    for (int i = 0; i < 64; ++i) {
        self->entries[i] = self->value;
    }
    return self;
}
} // namespace jj_fn_802CAD5C

// fn_802CAE60: body from https://github.com/jj-link/brawl src/havok/fn_802CAE60.cpp (namespaced).
#include <havok/hkMemory.h>
namespace jj_fn_802CAE60 {
class SoHeapAllocator {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void allocate(void* ptr, unsigned short size, int align);
};

class Inner {
public:
    virtual void destroy(int flag);
};

struct Handle {
    char padding[0x100];
    Inner* inner;
};

// TODO: cleanup (shape from jj-link/brawl fn_802CAE60)
extern "C" Handle* fn_802CAE60(Handle* self, int flag) {
    if (self != 0) {
        Inner* obj = self->inner;
        if (obj != 0) {
            obj->destroy(1);
        }
        if (flag > 0) {
            ((SoHeapAllocator*)hkMemory::s_instance)->allocate(self, 0x104, 0x25);
        }
    }
    return self;
}
} // namespace jj_fn_802CAE60
