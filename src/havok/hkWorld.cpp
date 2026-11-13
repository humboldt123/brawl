// Havok translation unit hkWorld.o (main.dol 0x802EA88C-0x802F004C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802EA88C  2140  __dt   [map: hkWorld____dt]
//   0x802EB0E8    92  __dt   [map: hkEntityEntityBroadPhaseListener____dt]
//   0x802EB144    92  __dt   [map: hkBroadPhaseBorderListener____dt]
//   0x802EB1A0   512  hkWorld_setupContactMgrFactories   [map: hkWorld__hkWorld_setupContactMgrFactories]
//   0x802EB3A0    92  __dt   [map: hkNullContactMgr____dt]
//   0x802EB3FC  1044  updateCollisionFilterOnWorld   [map: hkWorld__updateCollisionFilterOnWorld]
//   0x802EB810   364  hkWorld_updateFilterOnSinglePhantom   [map: hkPhantom__hkWorld_updateFilterOnSinglePhantom]
//   0x802EB97C   820  updateCollisionFilterOnPhantom   [map: hkWorld__updateCollisionFilterOnPhantom]
//   0x802EBCB0  1296  updateCollisionFilterOnEntity   [map: hkWorld__updateCollisionFilterOnEntity]
//   0x802EC1C0   608  addEntity   [map: hkWorld__addEntity]
//   0x802EC420  1952  addEntityBatch   [map: hkWorld__addEntityBatch]
//   0x802ECBC0   552  removeEntity   [map: hkWorld__removeEntity]
//   0x802ECDE8  1232  removeEntityBatch   [map: hkWorld__removeEntityBatch]
//   0x802ED2B8   308  activateRegion   [map: hkWorld__activateRegion]
//   0x802ED3EC   356  addConstraint   [map: hkWorld__addConstraint]
//   0x802ED550   224  removeConstraint   [map: hkWorld__removeConstraint]
//   0x802ED630   492  addAction   [map: hkWorld__addAction]
//   0x802ED81C    68  removeAction   [map: hkWorld__removeAction]
//   0x802ED860   616  removeActionImmediately   [map: hkWorld__removeActionImmediately]
//   0x802EDAC8   296  addPhantom   [map: hkWorld__addPhantom]
//   0x802EDBF0   972  addPhantomBatch   [map: hkWorld__addPhantomBatch]
//   0x802EDFBC   316  removePhantom   [map: hkWorld__removePhantom]
//   0x802EE0F8   812  removePhantomBatch   [map: hkWorld__removePhantomBatch]
//   0x802EE424   268  addPhysicsSystem   [map: hkWorld__addPhysicsSystem]
//   0x802EE530   264  removePhysicsSystem   [map: hkWorld__removePhysicsSystem]
//   0x802EE638    36  setGravity   [map: hkWorld__setGravity]
//   0x802EE65C   108  addWorldDeletionListener   [map: hkWorld__addWorldDeletionListener]
//   0x802EE6C8    80  removeWorldDeletionListener   [map: hkWorld__removeWorldDeletionListener]
//   0x802EE718   276  stepDeltaTime   [map: hkWorld__stepDeltaTime]
//   0x802EE82C  1772  calcStatistics   [map: hkWorld__calcStatistics]
//   0x802EEF18  3620  __ct   [map: hkWorld____ct]
//   0x802EFD3C   308  setCollisionFilter   [map: hkWorld__setCollisionFilter]
//   0x802EFE70   188  internal_executePendingOperations   [map: hkWorld__internal_executePendingOperations]
//   0x802EFF2C     8  queueOperation   [map: hkWorld__queueOperation]
//   0x802EFF34   108  getMemUsageForIntegration   [map: hkWorld__getMemUsageForIntegration]
//   0x802EFFA0     4  lock   [map: hkWorld__lock]
//   0x802EFFA4     4  unlock   [map: hkWorld__unlock]
//   0x802EFFA8     4  lockIslandForConstraintUpdate   [map: hkWorld__lockIslandForConstraintUpdate]
//   0x802EFFAC     4  unlockIslandForConstraintUpdate   [map: hkWorld__unlockIslandForConstraintUpdate]
//   0x802EFFB0     8  createContactMgr   [map: hkNullContactMgrFactory__createContactMgr]
//   0x802EFFB8     4  cleanup   [map: hkNullContactMgr__cleanup]
//   0x802EFFBC     8  reserveContactPoints   [map: hkNullContactMgr__reserveContactPoints]
//   0x802EFFC4     4  processToi   [map: hkNullContactMgr__processToi]
//   0x802EFFC8     4  removeToi   [map: hkNullContactMgr__removeToi]
//   0x802EFFCC     8  addToi   [map: hkNullContactMgr__addToi]
//   0x802EFFD4     4  processContact   [map: hkNullContactMgr__processContact]
//   0x802EFFD8     4  removeContactPoint   [map: hkNullContactMgr__removeContactPoint]
//   0x802EFFDC     8  addContactPoint   [map: hkNullContactMgr__addContactPoint]
//   0x802EFFE4    92  __dt   [map: hkNullContactMgrFactory____dt]
//   0x802F0040    12  __sinit_\hkWorld_cpp   [map: hkWorldcpp____sinit_]

#include <havok/hkWorld.h>
#include <havok/hkPhysicsSystem.h>
#include <havok/hkPhantom.h>
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkWorldOperationUtil.h>
#include <havok/hkWorldOperationQueue.h>
#include <havok/hkMemory.h>
#include <havok/hkString.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkEntity.h>
#include <havok/hkBroadPhase.h>

// Operation record queued while the world is locked (8 bytes). HYPOTHESIS: field names; the
// type byte selects the deferred operation, the object pointer is its argument.
struct hkWorldOperationRecord {
    u8 m_type;      // 0x00
    void* m_object; // 0x04
};


void hkWorld::lock() {}

void hkWorld::unlock() {}

void hkWorld::lockIslandForConstraintUpdate() {}

void hkWorld::unlockIslandForConstraintUpdate() {}

void hkWorld::setGravity(const hkVector4& gravity) {
    m_gravity = gravity;
}

#pragma dont_inline on
void hkWorld::queueOperation(const void* operation) { // out of line in the original
    m_operationQueue->queueOperation(*static_cast<const hkWorldOperation::BaseOperation*>(operation));
}
#pragma dont_inline reset

void hkWorld::addWorldDeletionListener(hkWorldDeletionListener* listener) {
    m_worldDeletionListeners.pushBack(listener);
}

void hkWorld::removeWorldDeletionListener(hkWorldDeletionListener* listener) {
    // HYPOTHESIS: an entry is cleared in place (no compaction).
    int index = m_worldDeletionListeners.indexOf(listener);
    m_worldDeletionListeners[index] = NULL;
}


// returns NULL when the add got queued because the world was locked
hkPhantom* hkWorld::addPhantom(hkPhantom* phantom) {
    if (m_lockCount + m_unk80 != 0) {
        hkWorldOperationRecord op;
        op.m_type = hkWorldOperation::PHANTOM_ADD;
        op.m_object = phantom;
        queueOperation(&op);
        return NULL;
    }

    m_lockCount++;

    // a phantom that came out of a packfile may not have its collidable pointed at a motion state yet
    if (phantom->m_collidable.m_motion == NULL) {
        phantom->m_collidable.m_motion = phantom->getMotionState();
    }

    phantom->m_world = this;
    phantom->addReference();
    m_phantoms.pushBack(phantom);

    hkWorldOperationUtil::addPhantomBP(this, phantom);

    hkWorldCallbackUtil::firePhantomAdded(this, phantom);
    phantom->firePhantomAdded();

    unlockAndAttemptToExecutePendingOperations();
    return phantom;
}

void hkWorld::removePhantom(hkPhantom* phantom) {
    if (m_lockCount + m_unk80 != 0) {
        hkWorldOperationRecord op;
        op.m_type = hkWorldOperation::PHANTOM_REMOVE;
        op.m_object = phantom;
        queueOperation(&op);
        return;
    }

    m_lockCount++;

    hkWorldCallbackUtil::firePhantomRemoved(this, phantom);
    phantom->firePhantomRemoved();

    hkWorldOperationUtil::removePhantomBP(this, phantom);

    // take it out of the list: last one gets moved into the hole
    m_phantoms.removeAt(m_phantoms.indexOf(phantom));
    phantom->m_world = NULL;

    // loaded from a packfile (memSize 0) means it might still own empty arrays that need freeing
    if (phantom->m_memSizeAndFlags == 0) {
        phantom->deallocateInternalArrays();
    }
    phantom->removeReference();

    unlockAndAttemptToExecutePendingOperations();
}

// raw field access for the world's members that nobody has laid out yet (offsets are from the asm)
template <typename T>
static inline T& worldField(void* w, int offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<char*>(w) + offset);
}


// wakes up every entity in the box. while the world is locked the box is copied and queued instead
void hkWorld::activateRegion(hkAabb* aabb) {
    if (m_lockCount != 0) {
        hkWorldOperation::SingleObjectOperation op;
        op.m_type = hkWorldOperation::ACTIVATE_REGION;
        op.m_object = hkMemory::getInstance().allocateChunk(0x20, 4);
        hkString::memCpy(op.m_object, aabb, 0x20);
        queueOperation(&op);
        return;
    }

    hkArray<hkBroadPhaseHandlePair> hits;
    // HYPOTHESIS: world +0x54 is the broadphase
    worldField<hkBroadPhase*>(this, 0x54)->querySingleAabb(*aabb, hits);

    for (int i = 0; i < hits.m_size; i++) {
        // handle -> collidable (a signed byte offset stored in the handle) -> owner, only if it is an entity (type 1)
        char* handle = reinterpret_cast<char*>(hits[i].m_b);
        char* collidable = handle + static_cast<s8>(handle[5]);
        hkEntity* entity = NULL;
        if (collidable[0x18] == 1) {
            entity = reinterpret_cast<hkEntity*>(collidable + *reinterpret_cast<int*>(collidable + 0x10));
        }
        if (entity != NULL) {
            entity->activate();
        }
    }
}

// The two virtual calls made by stepDeltaTime. HYPOTHESIS: names from what the calls do (step the
// simulation, ask the watchdog to free memory); only slot 0x10 of each vtable is used.
struct hkWorldStepSimulation : hkReferencedObject {
    virtual int stepDeltaTime(hkReal physicsDeltaTime); // 0x10
};
struct hkWorldFreeMemoryWatchDog : hkReferencedObject {
    int m_memoryLimit; // 0x08
    virtual void freeMemory(hkWorld* world); // 0x10
};

// Runs the simulation for one step. When that succeeded and the memory in use (hkMemory 0x28 + 0x14) is over
// the watchdog's limit, the watchdog is asked to free memory, bracketed by a timer.
int hkWorld::stepDeltaTime(hkReal physicsDeltaTime) {
    int result = ((hkWorldStepSimulation*)m_simulation)->stepDeltaTime(physicsDeltaTime);
    if (result == 0) {
        hkWorldFreeMemoryWatchDog* watchDog = worldField<hkWorldFreeMemoryWatchDog*>(this, 0x50);
        if (watchDog != NULL) {
            hkMemory& memory = hkMemory::getInstance();
            if (worldField<int>(&memory, 0x28) + worldField<int>(&memory, 0x14) > watchDog->m_memoryLimit) {
                HK_TIMER_BEGIN("WatchDog:FreeMem");
                watchDog->freeMemory(this);
                HK_TIMER_END();
            }
        }
    }
    return result;
}

#pragma dont_inline on
void hkWorld::removeAction(hkAction* action) {
    if (m_lockCount != 0) {
        hkWorldOperationRecord op;
        op.m_type = hkWorldOperation::ACTION_REMOVE;
        op.m_object = action;
        queueOperation(&op);
    } else {
        removeActionImmediately(action);
    }
}
#pragma dont_inline reset

// HYPOTHESIS: post-add step of the physics system, not recovered (called with the system's active flag).
extern "C" void fn_802F9098(hkWorld* world, hkBool active);

void hkWorld::addPhysicsSystem(hkPhysicsSystem* system) {
    if (system->m_rigidBodies.m_size > 0) {
        addEntityBatch((hkEntity**)system->m_rigidBodies.m_data, system->m_rigidBodies.m_size, system->m_active);
    }
    if (system->m_phantoms.m_size > 0) {
        addPhantomBatch((hkPhantom**)system->m_phantoms.m_data, system->m_phantoms.m_size);
    }
    for (int i = 0; i < system->m_actions.m_size; i++) {
        hkAction* action = ((hkAction**)system->m_actions.m_data)[i];
        if (action != NULL) {
            addAction(action);
        }
    }
    for (int i = 0; i < system->m_constraints.m_size; i++) {
        addConstraint(((hkConstraintInstance**)system->m_constraints.m_data)[i]);
    }
    if (system->m_rigidBodies.m_size > 0) {
        fn_802F9098(this, system->m_active);
    }
}

void hkWorld::removePhysicsSystem(hkPhysicsSystem* system) {
    for (int i = 0; i < system->m_constraints.m_size; i++) {
        removeConstraint(((hkConstraintInstance**)system->m_constraints.m_data)[i]);
    }
    for (int i = 0; i < system->m_actions.m_size; i++) {
        removeAction(((hkAction**)system->m_actions.m_data)[i]);
    }
    int rigidBodyCount = system->m_rigidBodies.m_size;
    if (rigidBodyCount < 4) {
        for (int i = 0; i < system->m_rigidBodies.m_size; i++) {
            removeEntity(((hkEntity**)system->m_rigidBodies.m_data)[i]);
        }
    } else {
        removeEntityBatch((hkEntity**)system->m_rigidBodies.m_data, system->m_rigidBodies.m_size);
    }
    if (system->m_phantoms.m_size > 0) {
        removePhantomBatch((hkPhantom**)system->m_phantoms.m_data, system->m_phantoms.m_size);
    }
}

// Per-entry record of the 0x28 array (HYPOTHESIS: field names and types, only the read offsets are known).
struct hkWorldMemoryEntry {
    u8 m_unk00[0x0C];  // 0x00
    int m_unk0C;       // 0x0C
    int m_unk10;       // 0x10
    int m_unk14;       // 0x14 (scaled by 4)
    u8 m_unk18[0x24];  // 0x18 .. 0x3B
    int m_unk3C;       // 0x3C (scaled by 128)
};

int hkWorld::getMemUsageForIntegration() {
    int result = 0;
    for (int i = m_unk28.m_size - 1; i >= 0; i--) {
        hkWorldMemoryEntry* entry = ((hkWorldMemoryEntry**)m_unk28.m_data)[i];
        int value = entry->m_unk0C + (entry->m_unk3C << 7) + (entry->m_unk14 << 2) + entry->m_unk10 + 0x9C;
        if (result < value) {
            result = value;
        }
    }
    return result;
}


// Null contact manager and its factory. Declared locally (no header yet). The vtables are lbl_80488348
// (dtor, calcStatistics, then the eight slots in the order below) and lbl_80488330 (dtor, calcStatistics,
// createContactMgr); their deleting destructors free class ids 0x20 and 0x04. Every manager method is a no-op
// returning a fixed value; the factory hands out the manager embedded at +0x08.
// HYPOTHESIS: the real bases are hkContactMgr / hkContactMgrFactory (hkCollisionAgent.h, whose slot parameters
// are not known well enough to override them here).
struct hkNullContactMgr : hkReferencedObject {
    HK_DECLARE_REF_ALLOCATOR(0x20)
    virtual bool addContactPoint();      // 0x10
    virtual bool reserveContactPoints(); // 0x14
    virtual void removeContactPoint();   // 0x18
    virtual void processContact();       // 0x1C
    virtual void cleanup();              // 0x20
    virtual bool addToi();               // 0x24
    virtual void removeToi();            // 0x28
    virtual void processToi();           // 0x2C
};

struct hkNullContactMgrFactory : hkReferencedObject {
    hkNullContactMgr m_contactMgr; // 0x08

    HK_DECLARE_REF_ALLOCATOR(0x04)
    virtual hkNullContactMgr* createContactMgr(); // 0x10
};

hkNullContactMgr* hkNullContactMgrFactory::createContactMgr() {
    return &m_contactMgr;
}

void hkNullContactMgr::cleanup() {}

bool hkNullContactMgr::reserveContactPoints() {
    return false;
}

void hkNullContactMgr::processToi() {}

void hkNullContactMgr::removeToi() {}

bool hkNullContactMgr::addToi() {
    return true;
}

void hkNullContactMgr::processContact() {}

void hkNullContactMgr::removeContactPoint() {}

bool hkNullContactMgr::addContactPoint() {
    return false;
}

// Unnamed static flag (lbl_805A0F20); initialised through hkBool(bool) so the TU gets a __sinit.
// HYPOTHESIS: a debug/config switch of the unit; nothing in the written code reads it yet.
static hkBool s_hkWorld_unk(false);

// Deleting destructors of the two broad-phase listeners hkWorld creates (classes not modelled yet; sized delete,
// allocation class 0x1f). Shape from https://github.com/jj-link/brawl src/havok/fn_802EB0E8.cpp / fn_802EB144.cpp.
// hkEntityEntityBroadPhaseListener::~hkEntityEntityBroadPhaseListener
// TODO: cleanup (shape from jj-link/brawl fn_802EB0E8)
extern "C" hkReferencedObject* fn_802EB0E8(hkReferencedObject* self, int deleting) {
    if (self != 0 && deleting > 0) {
        hkMemory::s_instance->deallocateChunk(self, self->m_memSizeAndFlags, 0x1f);
    }
    return self;
}
// hkBroadPhaseBorderListener::~hkBroadPhaseBorderListener
// TODO: cleanup (shape from jj-link/brawl fn_802EB144)
extern "C" hkReferencedObject* fn_802EB144(hkReferencedObject* self, int deleting) {
    if (self != 0 && deleting > 0) {
        hkMemory::s_instance->deallocateChunk(self, self->m_memSizeAndFlags, 0x1f);
    }
    return self;
}

// fn_802EB810: body from https://github.com/jj-link/brawl src/havok/fn_802EB810.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802EB810 {
struct Param {
    char padding0[0x10];
    int offset;
    char padding1[4];
    signed char state;
};

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6(Param*);
    virtual unsigned int v7();
    virtual void v8(Param*);
};

struct Target {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6(void*);
    virtual unsigned int v7(void*);
    virtual void v8(void*);
};

struct ObjB {
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
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual unsigned int v18(void*, Param*);
};

// TODO: cleanup (shape from jj-link/brawl fn_802EB810)
extern "C" void fn_802EB810(Obj* a, Param* p, ObjB* b) {
    unsigned int c1 = a->v7() >> 24;
    void* field = (char*)a + 0x1c;

    if ((signed char)(b->v18(field, p) >> 24) != 0) {
        if ((signed char)c1 == 0) {
            a->v6(p);
        }
        if (p->state == 2) {
            if ((signed char)(((Target*)((char*)p + p->offset))->v7(field) >> 24) == 0) {
                ((Target*)((char*)p + p->offset))->v6(field);
            }
        }
    } else {
        if ((signed char)c1 != 0) {
            a->v8(p);
        }
        if (p->state == 2) {
            if ((signed char)(((Target*)((char*)p + p->offset))->v7(field) >> 24) != 0) {
                ((Target*)((char*)p + p->offset))->v8(field);
            }
        }
    }
}
} // namespace jj_fn_802EB810

// TODO: cleanup (shape from jj-link/brawl fn_802EFD3C)
// fn_802EFD3C: body from https://github.com/jj-link/brawl src/havok/fn_802EFD3C.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkWorld.h>
namespace jj_fn_802EFD3C {
#pragma use_lmw_stmw on
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
class SoHeapAllocator {
public:
 virtual void vf0();
 virtual void vf1();
 virtual void vf2();
 virtual void vf3();
 virtual void* alloc(int, int);
};
extern "C" char lbl_80487320[];
struct Ref {
 virtual void destroy(int);
 u16 active;
 s16 refs;
 void* a;
 void* b;
 void* c;
 void* d;
};
struct Other { void* a; void* b; };
struct Self { u8 pad[0x68]; Other* other; Ref* ref; };
extern "C" void fn_802EFD3C(Self* self, Ref* arg, char* flag, u8 x, u8 y) {
 if (!arg) {
  Ref* p = (Ref*)((SoHeapAllocator*)hkMemory::s_instance)->alloc(0x18, 0x25);
  p->active = 0x18;
  arg = p;
  if (p) {
   p->refs = 1;
   *(void**)p = lbl_80487320;
   p->a = lbl_80487320 + 0x10;
   p->b = lbl_80487320 + 0x20;
   p->c = lbl_80487320 + 0x30;
   p->d = lbl_80487320 + 0x40;
  }
 } else if (arg->active) {
  arg->refs = arg->refs + 1;
 }
 Ref* p = self->ref;
 if (p->active) {
  p->refs = p->refs - 1;
  if (p->refs == 0 && p != 0) p->destroy(1);
 }
 self->ref = arg;
 void* ptr = arg;
 if (arg) ptr = (char*)arg + 0xc;
 char f = *flag;
 self->other->b = ptr;
 if (f) ((hkWorld*)(self))->updateCollisionFilterOnWorld(x, y);
}
} // namespace jj_fn_802EFD3C

// TODO: cleanup (shape from jj-link/brawl fn_802EB1A0)
// fn_802EB1A0: body from https://github.com/jj-link/brawl src/havok/fn_802EB1A0.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkWorld.h>
#include <havok/hkSimpleConstraintContactMgr.h>
#include <havok/hkReportContactMgr.h>
#include <havok/hkMemory.h>
namespace jj_fn_802EB1A0 {
class Alloc {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void* alloc(int size, int align);
};
class Object_vptr_0 {
public:
    virtual void slot0(int);
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    unsigned short size;
    short refs;
    void* unk8;
    short pad12;
    short unkE;
};
extern "C" char lbl_80488330[];
extern "C" char lbl_80488348[];
extern "C" void fn_802CC8DC(void*, void*, int);
extern "C" void fn_802EB1A0(int arg0, void* arg1) {
    Object_vptr_0* p1 = (Object_vptr_0*)new hkSimpleConstraintContactMgr::Factory((hkWorld*)arg0);
    Object_vptr_0* p2 = (Object_vptr_0*)new hkReportContactMgr::Factory((hkWorld*)arg0);
    Object_vptr_0* p3 = (Object_vptr_0*)((Alloc*)hkMemory::s_instance)->alloc(0x10, 4);
    p3->size = 0x10;
    if (p3 != 0) {
        p3->refs = 1;
        *(void**)p3 = lbl_80488330;
        p3->unkE = 1;
        p3->unk8 = lbl_80488348;
    }
    fn_802CC8DC(arg1, p1, 1);
    fn_802CC8DC(arg1, p2, 2);
    fn_802CC8DC(arg1, p3, 3);
    if (p1->size != 0) {
        if (--p1->refs == 0 && p1 != 0) {
            p1->slot0(1);
        }
    }
    if (p2->size != 0) {
        if (--p2->refs == 0 && p2 != 0) {
            p2->slot0(1);
        }
    }
    if (p3->size != 0) {
        if (--p3->refs == 0 && p3 != 0) {
            p3->slot0(1);
        }
    }
}
} // namespace jj_fn_802EB1A0
