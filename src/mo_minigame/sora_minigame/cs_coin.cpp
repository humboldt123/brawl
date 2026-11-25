#include <types.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_model.h>
#include <gf/gf_3d_scene.h>
#include <havok/hkWorld.h>
#include <havok/hkEntity.h>
#include <havok/hkRigidBody.h>
#include <havok/hkGroupFilter.h>
#include <ph/ph_world.h>
#include <ph/ph_actor.h>

nw4r::g3d::ScnMdl* ScnMdl_Construct(MEMAllocator* allocator, u32* size, nw4r::g3d::ResMdl mdl, u32 bufferOption, int nView,
                                    void (*callback)());
extern "C" void fn_801B0A28();

class csElementInterface {
public:
    virtual ~csElementInterface() { }
};

class csElement : public csElementInterface {
public:
    u8 m_isActive; // 0x04

    csElement();
    csElement(int group);
    virtual ~csElement();
    virtual void beginOperation() = 0;
    virtual void afterOperation() = 0;
    virtual void debugDraw() = 0;
};

class csWorld : public csElement, public phWorld {
};

class csModule {
public:
    u8 m_unk00[0x10];
    csWorld* m_world; // 0x10
};

class csCoin;

class csCoinshooter {
public:
    u8 m_unk00[0x198];
    nw4r::g3d::ResMdl m_coinResMdl; // 0x198
    u32 m_unk19C;                   // 0x19C
    hkGroupFilter m_groupFilter;    // 0x1A0
    u8 m_unk23C[0x4C0 - 0x23C];
    csModule* m_module;             // 0x4C0
    hkArray<csCoin*> m_coins;       // 0x4C4
};

extern csCoinshooter* lbl_20_bss_8;

struct hkContactPointAddedEvent;
struct hkContactPointConfirmedEvent;
struct hkContactPointRemovedEvent;
struct hkContactProcessEvent;

class hkCollisionListener {
public:
    virtual void contactPointAddedCallback(hkContactPointAddedEvent& event) = 0;
    virtual void contactPointConfirmedCallback(hkContactPointConfirmedEvent& event) = 0;
    virtual void contactPointRemovedCallback(hkContactPointRemovedEvent& event) = 0;
    virtual void contactProcessCallback(hkContactProcessEvent& event) = 0;
    virtual ~hkCollisionListener() { }
};

class hkEntityListener {
public:
    virtual ~hkEntityListener() { }
    virtual void entityAddedCallback(hkEntity* entity);
    virtual void entityRemovedCallback(hkEntity* entity);
    virtual void entityShapeSetCallback(hkEntity* entity);
    virtual void entityDeletedCallback(hkEntity* entity);
};

class phCollisionEvent : public hkCollisionListener, public hkEntityListener {
public:
    virtual void contactPointAddedCallback(hkContactPointAddedEvent& event);
    virtual void contactPointConfirmedCallback(hkContactPointConfirmedEvent& event);
    virtual void contactPointRemovedCallback(hkContactPointRemovedEvent& event);
    virtual void contactProcessCallback(hkContactProcessEvent& event);
    virtual ~phCollisionEvent() { }
};

class csArticle : public csElement, public phActor, public phCollisionEvent {
public:
    u8 m_unk20; // 0x20

    csArticle();
    virtual ~csArticle();
    virtual void beginOperation();
    virtual void afterOperation();
    virtual void debugDraw();
    virtual void applyDestroy();
};

class csCoin : public csArticle {
public:
    nw4r::g3d::ScnMdl* m_scnMdl; // 0x24
    hkRigidBody* m_rigidBody;    // 0x28
    float m_unk2C;               // 0x2C
    float m_unk30;               // 0x30
    u32 m_unk34;                 // 0x34
    s32 m_unk38;                 // 0x38
    u8 m_unk3C;                  // 0x3C
    u8 m_unk3D;                  // 0x3D
    u8 m_unk3E;                  // 0x3E
    u8 m_unk3F;                  // 0x3F

    csCoin();
    virtual ~csCoin();
    virtual void beginOperation();
    virtual void afterOperation();
    virtual void applyDestroy();
    void makeNewCoin();
};

csCoin::csCoin() {
    m_scnMdl = NULL;
    m_rigidBody = NULL;
    m_unk2C = 0.4f;
    m_unk30 = 0.04f;
    m_unk38 = 30;
    m_unk3E = 0;
    m_unk3F = 0;
}

csCoin::~csCoin() {
}

void csCoin::afterOperation() {
    lbl_20_bss_8->m_module->m_world->m_hkWorld->lock();
    lbl_20_bss_8->m_module->m_world->m_hkWorld->unlock();
}

void csCoin::applyDestroy() {
    m_scnMdl->Destroy();
}
