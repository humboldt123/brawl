#include <types.h>
#include <havok/hkBase.h>
#include <havok/hkMemory.h>
#include <havok/hkWorld.h>
#include <havok/hkWorldCinfo.h>
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

struct phShape {
    hkEntity* m_entity; // 0x00
};

class csArticle : public csElement, public phActor {
public:
    virtual ~csArticle();
};

class csEnemy {
public:
    u8 m_unk00[0x44];
    s32 m_state; // 0x44
    u8 m_unk48[0x8];
    s32 m_unk50; // 0x50
};

class csCoinshooter {
public:
    u8 m_unk00[0x4DC];
    hkArray<csEnemy*> m_enemies; // 0x4DC
};

extern csCoinshooter* lbl_20_bss_8;

class csWorld : public csElement, public phWorld {
public:
    u32 m_unk10; // 0x10
    u32 m_unk14; // 0x14
    u8 m_unk18;  // 0x18
    u32 m_unk1C; // 0x1C
    u32 m_unk20; // 0x20
    s32 m_unk24; // 0x24
    s32 m_unk28; // 0x28
    u32 m_unk2C; // 0x2C

    virtual ~csWorld();
    void createPhysicsWorld(const hkWorldCinfo& cinfo);
    void addArticle(csArticle* article);
    virtual void beginOperation();
    virtual void afterOperation();
    void enemyGroupBonus();
    int getEnemyKillCount();
    virtual void debugDraw();
};

csWorld::~csWorld() {
    m_hkWorld->removeReference();
    m_hkWorld = NULL;
}

struct csHkWorld : hkWorld {
    csHkWorld(const hkWorldCinfo& cinfo, unsigned int version = 0x00009C40);
};

void csWorld::createPhysicsWorld(const hkWorldCinfo& cinfo) {
    m_hkWorld = new csHkWorld(cinfo);
}

void csWorld::addArticle(csArticle* article) {
    for (int i = 0; i < article->m_count; i++) {
        m_hkWorld->addEntity(article->m_entries[i].m_shape->m_entity, 1);
    }
}

void csWorld::afterOperation() {
}

void csWorld::debugDraw() {
}
