#include <types.h>
#include <havok/hkWorld.h>
#include <havok/hkEntity.h>
#include <ph/ph_world.h>
#include <ph/ph_actor.h>

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

class csCoinshooter {
public:
    u8 m_unk00[0x4C0];
    csModule* m_module; // 0x4C0
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
    virtual void entityAddedCallback(hkEntity* entity) { }
    virtual void entityRemovedCallback(hkEntity* entity) { }
    virtual void entityShapeSetCallback(hkEntity* entity) {
        if (entity->m_world != NULL) {
            entityRemovedCallback(entity);
            entityAddedCallback(entity);
        }
    }
    virtual void entityDeletedCallback(hkEntity* entity) { }
};

class phCollisionEvent : public hkCollisionListener, public hkEntityListener {
public:
    virtual void contactPointAddedCallback(hkContactPointAddedEvent& event) { }
    virtual void contactPointConfirmedCallback(hkContactPointConfirmedEvent& event) { }
    virtual void contactPointRemovedCallback(hkContactPointRemovedEvent& event) { }
    virtual void contactProcessCallback(hkContactProcessEvent& event) { }
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

csArticle::csArticle() {
    m_unk20 = 0;
}

csArticle::~csArticle() {
}

void csArticle::beginOperation() {
    lbl_20_bss_8->m_module->m_world->m_hkWorld->lock();
    lbl_20_bss_8->m_module->m_world->m_hkWorld->unlock();
}

void csArticle::afterOperation() {
    lbl_20_bss_8->m_module->m_world->m_hkWorld->lock();
    lbl_20_bss_8->m_module->m_world->m_hkWorld->unlock();
}

void csArticle::applyDestroy() {
}

void csArticle::debugDraw() {
    draw();
}
