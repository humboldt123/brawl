// Havok translation unit hkReportContactMgr.o (main.dol 0x802D7708-0x802D7FC8).
// Functions in address order (method names from the Havok TU map). Only the functions listed below are
// written so far; the contact point, TOI and processing functions are not done yet.
//   0x802D7708    60  __ct   [map: hkReportContactMgr____ct]
//   0x802D7744    92  __dt   [map: hkReportContactMgr____dt]
//   0x802D7F20    28  __ct   [map: hkReportContactMgr7FactoryFP7hkWorld____ct]
//   0x802D7F3C   132  createContactMgr   [map: hkReportContactMgr7FactoryFRC12hkCollidableRC12hkCollidableRC16hkCollisionInput__createContactMgr]
//   0x802D7FC0     8  getConstraintInstance   [map: hkDynamicsContactMgr__getConstraintInstance]

#include <havok/hkReportContactMgr.h>

hkReportContactMgr::hkReportContactMgr(hkWorld* world, const void* ownerA, const void* ownerB) {
    // HYPOTHESIS: the limit is the smaller of the two owners' u16 fields at 0x92.
    u16 limitB = *(const u16*)((const char*)ownerB + 0x92);
    u16 limitA = *(const u16*)((const char*)ownerA + 0x92);
    m_maxContacts = limitA < limitB ? limitA : limitB;
    m_world = world;
    m_ownerA = ownerA;
    m_ownerB = ownerB;
}

hkReportContactMgr::~hkReportContactMgr() {}

hkReportContactMgr::Factory::Factory(hkWorld* world) {
    m_factoryWorld = world;
}

hkReportContactMgr* hkReportContactMgr::Factory::createContactMgr(const hkCollidable& a, const hkCollidable& b,
                                                                  const void* input) {
    const void* ownerA = (const char*)&a + a.m_ownerOffset;
    const void* ownerB = (const char*)&b + b.m_ownerOffset;
    hkReportContactMgr* mgr = (hkReportContactMgr*)hkMemory::getInstance().allocateChunk(0x18, 0x20);
    if (mgr != 0) {
        mgr->m_memSizeAndFlags = 0x18;
        ::new (mgr) hkReportContactMgr(m_factoryWorld, ownerA, ownerB);
    }
    return mgr;
}

hkConstraintInstance* hkDynamicsContactMgr::getConstraintInstance() {
    return 0;
}

// fn_802D7BD0: body from https://github.com/jj-link/brawl src/havok/fn_802D7BD0.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkEntityCallbackUtil.h>
namespace jj_fn_802D7BD0 {
struct Ctx {
    int a, b;
    int pad;
    int c, d;
    float e, f;
    int g, h;
};
struct Inner {
    char pad[8];
    int unk8;
    char pad2[0x1B8 - 0xC];
    int unk1B8;
};
struct Arg {
    int unk0;
    Inner* unk4;
    Inner* unk8;
    float unkC;
};

// TODO: cleanup (shape from jj-link/brawl fn_802D7BD0)
extern "C" void fn_802D7BD0(int, Arg* p, float f) {
    Ctx c;
    int b = (int)p->unk8 + 0x1c;
    int a = (int)p->unk4 + 0x1c;
    float u = p->unkC;
    c.a = a;
    c.b = b;
    c.c = (int)p + 0x20;
    c.d = (int)p + 0x10;
    c.e = f;
    c.f = u;
    c.g = 0;
    c.h = 0;
    hkWorldCallbackUtil::fireContactPointConfirmed((hkWorld*)(p->unk4->unk8), (hkWorldCallbackUtil::ContactPointEvent*)(&c));
    if (p->unk4->unk1B8) hkEntityCallbackUtil::fireContactPointConfirmedInternal((hkEntity*)(p->unk4), (hkCollisionEventConfirmed*)(&c));
    if (p->unk8->unk1B8) hkEntityCallbackUtil::fireContactPointConfirmedInternal((hkEntity*)(p->unk8), (hkCollisionEventConfirmed*)(&c));
}
} // namespace jj_fn_802D7BD0

// fn_802D7A54: body from https://github.com/jj-link/brawl src/havok/fn_802D7A54.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkEntityCallbackUtil.h>
namespace jj_fn_802D7A54 {
struct Inner { char pad[0x1b8]; int v; };
struct Obj { char pad[8]; void* a; Inner* b; Inner* c; };
struct Out { char pad[4]; short s; char c6; char c7; };
struct Ctx {
    int a, b, c, pad;
    int d, e;
    Out* out;
    float f;
    int g;
    Obj* self;
    float h;
};
// hkEntityCallbackUtil::fireContactPointAddedInternal

// TODO: cleanup (shape from jj-link/brawl fn_802D7A54)
extern "C" int fn_802D7A54(Obj* self, int a, int b, int, int d, int e, float* p, Out* out, float h) {
    Ctx x;
    out->s = 0;
    out->c6 = 0;
    out->c7 = 1;
    float t = *p;
    x.a = a;
    x.b = b;
    x.c = 0;
    x.d = d;
    x.e = e;
    x.out = out;
    x.f = t;
    x.self = self;
    x.g = 0;
    x.h = h;
    hkWorldCallbackUtil::fireContactPointAdded((hkWorld*)(self->a), (hkWorldCallbackUtil::ContactPointEvent*)(&x));
    if (self->b->v != 0) hkEntityCallbackUtil::fireContactPointAddedInternal((hkEntity*)self->b, (hkContactPointAddedEvent*)&x);
    if (self->c->v != 0) hkEntityCallbackUtil::fireContactPointAddedInternal((hkEntity*)self->c, (hkContactPointAddedEvent*)&x);
    *p = x.f;
    return x.g;
}
} // namespace jj_fn_802D7A54

// fn_802D7C78: body from https://github.com/jj-link/brawl src/havok/fn_802D7C78.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkEntityCallbackUtil.h>
namespace jj_fn_802D7C78 {
typedef unsigned short u16;

struct Root {
    unsigned char pad[0x1B8];
    int count;
};

struct Manager;

struct Context {
    u16 flag;
    unsigned char pad2[2];
    int value;
    Root* root1;
    Root* root2;
    int pad14;
    void* owner;
};

class Self {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual int v10();

    unsigned char pad[4];
    Manager* manager;
    Root* root1;
    Root* root2;
};


// TODO: cleanup (shape from jj-link/brawl fn_802D7C78)
extern "C" void fn_802D7C78(Self* self, u16 arg1) {
    Root* root1 = self->root1;
    Root* root2 = self->root2;
    Context context;

    context.flag = arg1;
    context.value = 0;
    context.root1 = root1;
    context.root2 = root2;
    context.owner = self;

    if (arg1 != 0xFFFF) {
        int value = self->v10();
        if (value != 0) {
            value += 8;
        }
        context.value = value;
    }

    hkWorldCallbackUtil::fireContactPointRemoved((hkWorld*)(self->manager), (hkWorldCallbackUtil::ContactPointEvent*)(&context));

    if (root1->count != 0) {
        hkEntityCallbackUtil::fireContactPointRemovedInternal((hkEntity*)(root1), (hkCollisionEventRemoved*)(&context));
    }

    if (root2->count != 0) {
        hkEntityCallbackUtil::fireContactPointRemovedInternal((hkEntity*)(root2), (hkCollisionEventRemoved*)(&context));
    }
}
} // namespace jj_fn_802D7C78

// fn_802D7B10: body from https://github.com/jj-link/brawl src/havok/fn_802D7B10.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkEntityCallbackUtil.h>
namespace jj_fn_802D7B10 {
typedef unsigned short u16;
struct Manager;
struct Root { char pad0[8]; Manager* manager; char padC[0x1AC]; int count; };
struct Input { char pad0[0x10]; int offset; char pad14[4]; unsigned char type; };
struct Context { u16 flag; char pad2[2]; int value; Root* root1; Root* root2; int pad14; void* owner; };
// TODO: cleanup (shape from jj-link/brawl fn_802D7B10)
extern "C" void fn_802D7B10(void* owner, Input* a, Input* b, int value) {
    Root* root1;
    Root* root2;
    if ((int)a->type == 1) root1 = (Root*)((char*)a + a->offset);
    else root1 = 0;
    if ((int)b->type == 1) root2 = (Root*)((char*)b + b->offset);
    else root2 = 0;
    Manager* manager = root1->manager;
    Context context;
    context.flag = 0xFFFF;
    context.value = value;
    context.root1 = root1;
    context.root2 = root2;
    context.owner = owner;
    hkWorldCallbackUtil::fireContactPointRemoved((hkWorld*)(manager), (hkWorldCallbackUtil::ContactPointEvent*)(&context));
    if (root1->count != 0) hkEntityCallbackUtil::fireContactPointRemovedInternal((hkEntity*)(root1), (hkCollisionEventRemoved*)(&context));
    if (root2->count != 0) hkEntityCallbackUtil::fireContactPointRemovedInternal((hkEntity*)(root2), (hkCollisionEventRemoved*)(&context));
}
} // namespace jj_fn_802D7B10
