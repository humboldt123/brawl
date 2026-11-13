// Havok translation unit hkRigidBody.o (main.dol 0x802E22C4-0x802E3604).
// Functions in address order (method names from the Havok TU map):
//   0x802E22C4    76  finishLoadedObjecthkRigidBody   [map: hkRigidBody__finishLoadedObjecthkRigidBody]
//   0x802E2310    20  cleanupLoadedObjecthkRigidBody   [map: hkRigidBody__cleanupLoadedObjecthkRigidBody]
//   0x802E2324    84  getVtablehkRigidBody   [map: hkRigidBody__getVtablehkRigidBody]
//   0x802E2378    84  estimateAllowedPenetrationDepth   [map: hkCollidable__estimateAllowedPenetrationDepth]
//   0x802E23CC   476  updateCachedShapeInfo   [map: hkRigidBody__updateCachedShapeInfo]
//   0x802E25A8   664  createDynamicRigidMotion   [map: hkRigidBody__createDynamicRigidMotion]
//   0x802E2840   604  __ct   [map: hkRigidBody____ct]
//   0x802E2A9C   112  __dt   [map: hkRigidBody____dt]
//   0x802E2B0C   412  getCinfo   [map: hkRigidBody__getCinfo]
//   0x802E2CA8     8  getMotionState   [map: hkRigidBody__getMotionState]
//   0x802E2CB0     4  setDeactivator   [map: hkRigidBody__setDeactivator]
//   0x802E2CB4   836  clone   [map: hkRigidBody__clone]
//   0x802E2FF8    96  setMotionType   [map: hkRigidBody__setMotionType]
//   0x802E3058   464  setShape   [map: hkRigidBody__setShape]
//   0x802E3228    92  setCenterOfMassLocal   [map: hkRigidBody__setCenterOfMassLocal]
//   0x802E3284   268  setDeactivator   [map: hkRigidBody__setDeactivator1]
//   0x802E3390   328  updateBroadphaseAndResetCollisionInformationOfWarpedBody   [map: hkRigidBody__updateBroadphaseAndResetCollisionInformationOfWarpedBody]
//   0x802E34D8    68  setPosition   [map: hkRigidBody__setPosition]
//   0x802E351C    68  setPositionAndRotation   [map: hkRigidBody__setPositionAndRotation]
//   0x802E3560    68  setTransform   [map: hkRigidBody__setTransform]
//   0x802E35A4    16  setMass   [map: hkRigidBody__setMass]
//   0x802E35B4    80  __sinit_\hkRigidBody_cpp   [map: hkRigidBodycpp____sinit_]

#include <new>
#include <havok/hkRigidBody.h>
#include <havok/hkRegistry.h>
#include <havok/hkWorldOperationUtil.h>

void hkRigidBody::finishLoadedObjecthkRigidBody(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    new (p) hkRigidBody(flag);
}

void hkRigidBody::cleanupLoadedObjecthkRigidBody(void* p) {
    ((hkRigidBody*)p)->~hkRigidBody();
}

#pragma push
#pragma auto_inline off
const void* hkRigidBody::getVtablehkRigidBody() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    hkVector4 buf[31]; // 0x1F0 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkRigidBody(flag);
    return *(const void**)buf;
}
#pragma pop

hkMotionState* hkRigidBody::getMotionState() {
    return &getMotion()->m_motionState;
}

#pragma push
#pragma auto_inline off // MATCH-ONLY: kept out of line for fn_802E3284, which calls it
void hkRigidBody::setDeactivator(hkEntityDeactivator* deactivator) {
    hkEntity::setDeactivator(deactivator);
}
#pragma pop

void hkRigidBody::setPosition(const hkVector4& position) {
    getMotion()->setPosition(position);
    updateBroadphaseAndResetCollisionInformationOfWarpedBody();
}

void hkRigidBody::setPositionAndRotation(const hkVector4& position, const hkQuaternion& rotation) {
    getMotion()->setPositionAndRotation(position, rotation);
    updateBroadphaseAndResetCollisionInformationOfWarpedBody();
}

void hkRigidBody::setTransform(const hkTransform& transform) {
    getMotion()->setTransform(transform);
    updateBroadphaseAndResetCollisionInformationOfWarpedBody();
}

// While the world is locked the change is queued as an operation (kind 4); otherwise it is applied directly.
void hkRigidBody::setMotionType(u8 motionType, u8 a, u8 b) {
    if (m_world != 0 && m_world->m_lockCount != 0) {
        struct {
            u8 kind;            // 0x00 HYPOTHESIS: operation kind 4 (motion type change)
            u8 pad[3];
            hkRigidBody* body;  // 0x04
            u8 motionType;      // 0x08
            u8 a;               // 0x09
            u8 b;               // 0x0A
        } op;
        op.kind = 4;
        op.body = this;
        op.motionType = motionType;
        op.a = a;
        op.b = b;
        m_world->queueOperation(&op);
    } else {
        hkWorldOperationUtil::setRigidBodyMotionType(this, motionType, a, b);
    }
}

void hkRigidBody::setMass(hkReal mass) {
    getMotion()->setMass(mass);
}

void hkRigidBody::setCenterOfMassLocal(const hkVector4& com) {
    getMotion()->setCenterOfMassInLocal(com);
    hkVector4 info;
    updateCachedShapeInfo(m_collidable.m_shape, &info);
}

static hkStaticTypeInfo hkRigidBodyTypeInfo("hkRigidBody", hkRigidBody::finishLoadedObjecthkRigidBody, hkRigidBody::cleanupLoadedObjecthkRigidBody,
                                      hkRigidBody::getVtablehkRigidBody());

// Shape from https://github.com/jj-link/brawl src/havok/fn_802E2378.cpp.
// hkCollidable::estimateAllowedPenetrationDepth (inline copy): 0.2 * the smallest extent below 0.5, else 0.1.
// TODO: cleanup (shape from jj-link/brawl fn_802E2378)
extern "C" void fn_802E2378(char* collidable, const float* extents) {
    float a = extents[1];
    float c = extents[2];
    float b = extents[0];
    b = (b < a) ? b : a;
    b = (b < c) ? b : c;
    if (b < 0.5f) {
        *(float*)(collidable + 0x20) = 0.2f * b;
        return;
    }
    *(float*)(collidable + 0x20) = 0.1f;
}

// TODO: cleanup (shape from jj-link/brawl fn_802E3284)
// fn_802E3284: body from https://github.com/jj-link/brawl src/havok/fn_802E3284.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkRigidBody.h>
#include <havok/hkSpatialRigidBodyDeactivator.h>
namespace jj_fn_802E3284 {
class Class_80487F38 {
public:
    virtual void fn_802E3814(int);
    virtual void fn_80080A64();
    virtual void fn_802E3958();
    virtual void fn_802E3A90();
    virtual int fn_802E3950();
    unsigned short size;
    short refs;
};
class SoHeapAllocator {
public:
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual Class_80487F38* alloc(int, int);
};
struct Owner {
    char pad[0x68];
    Class_80487F38* object;
};
extern "C" char lbl_80532FF0[16];
extern "C" void fn_802E3284(Owner* self, int kind) {
    if (self->object == 0 || kind != self->object->fn_802E3950()) {
        switch (kind) {
        case 1:
            ((hkRigidBody*)(self))->setDeactivator((hkEntityDeactivator*)((Class_80487F38*)lbl_80532FF0));
            break;
        case 2:
            Class_80487F38* p = (Class_80487F38*)new hkSpatialRigidBodyDeactivator();
            ((hkRigidBody*)(self))->setDeactivator((hkEntityDeactivator*)(p));
            if (p->size != 0) {
                if (--p->refs == 0 && p != 0)
                    p->fn_802E3814(1);
            }
            break;
        }
    }
}
} // namespace jj_fn_802E3284
