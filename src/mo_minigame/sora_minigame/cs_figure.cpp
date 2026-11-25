#define CS_FIGURE_RESMDL_GETRESMAT_INT
#include <gf/gf_3d_scene.h>
#include <gf/gf_model.h>
#include <havok/hkWorld.h>
#include <if/if_figure_loader.h>
#include <if/if_minigame_coin_shooter.h>
#include <mt/mt_matrix.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <ph/ph_actor.h>
#include <snd/snd_system.h>
#include <types.h>

extern "C" void fn_8003F074(Matrix* mtx, float x, float y, float z);
extern "C" void fn_801B0A28();
nw4r::g3d::ScnMdl* ScnMdl_Construct(MEMAllocator* allocator, u32* size, nw4r::g3d::ResMdl mdl, u32 bufferOption, int nView, void (*callback)());
extern IfCoinShooter* lbl_805A0310;
void ScnMdl_SetNodeMtx(nw4r::g3d::ScnMdl* scnMdl, u32 node, const Matrix* mtx);

struct csPhysicsWorldInner {
    u8 m_unk0[0xC];
    hkWorld* m_world; // 0x0C
};

struct csPhysicsWorld {
    u8 m_unk0[0x10];
    csPhysicsWorldInner* m_inner; // 0x10
};

class csElement;

namespace utility {
template <class T>
class SafePtr {
public:
    T* m_ptr;

    ~SafePtr() {
        if (m_ptr != NULL) {
            m_ptr->destroy();
        }
        m_ptr = NULL;
    }
};
}

template utility::SafePtr<IfFigureLoader::Entry>::~SafePtr();

struct csCoinshooterView {
    u8 m_unk0[0x46];
    u8 m_unk46;                                  // 0x46
    u8 m_unk47[0x64 - 0x47];
    u8 m_unk64;                                  // 0x64
    u8 m_unk65[0x120 - 0x65];
    u32 m_unk120;                                // 0x120
    u32 m_unk124;                                // 0x124
    u8 m_unk128[0x34C - 0x128];
    utility::SafePtr<IfFigureLoader::Entry> m_figEntries[3]; // 0x34C
    u8 m_unk358[0x374 - 0x358];
    IfCoinShooterLabel m_unk374;                 // 0x374
    u32 m_unk378;                                // 0x378
    u32 m_unk37C;                                // 0x37C
    u32 m_unk380;                                // 0x380
    s32 m_unk384;                                // 0x384
    u8 m_unk388[0x3B0 - 0x388];
    nw4r::g3d::ScnMdl* m_enemyModels[6][6];      // 0x3B0 (size 0x90 -> 0x440)
    u8 m_unk440[0x448 - 0x440];
    gfModelAnimation* m_enemyAnims[5][6];        // 0x448 (size 0x78 -> 0x4C0)
    csPhysicsWorld* m_physicsWorld;              // 0x4C0
};

extern csCoinshooterView* lbl_20_bss_8;

class csElementInterface {
public:
    virtual ~csElementInterface() { }
    virtual void beginOperation() = 0;
    virtual void afterOperation() = 0;
    virtual void debugDraw() = 0;
};

class csElement : public csElementInterface {
public:
    u8 m_isActive; // 0x04

    csElement();
    csElement(int group);
    virtual ~csElement();
};

// HYPOTHESIS: Havok listener interfaces and phCollisionEvent base of csArticle (offsets +0x18 and +0x1C).
class hkCollisionListener {
public:
    virtual void contactPointAdded();
    virtual void contactPointConfirmed();
    virtual void contactPointRemoved();
    virtual void contactProcess();
    virtual ~hkCollisionListener() { }
};

class hkEntityListener {
public:
    virtual ~hkEntityListener() { }
    virtual void entityAddedCallback();
    virtual void entityRemovedCallback();
    virtual void entityShapeSetCallback();
    virtual void entityDeletedCallback();
    virtual void applyDestroy();
};

class phCollisionEvent : public hkCollisionListener, public hkEntityListener {
public:
    virtual ~phCollisionEvent() { }
};

class csArticle : public csElement, public phActor, public phCollisionEvent {
public:
    u8 m_unk20; // 0x20

    csArticle();
    virtual ~csArticle();
};

class csPathArticle {
public:
    static void checkBonusFire(const Vec3f* pos, bool hit);
};

class csFigure : public csArticle {
public:
    u32 m_unk24;                      // 0x24
    nw4r::g3d::ScnMdl* m_unk28;       // 0x28
    nw4r::g3d::ScnMdl* m_unk2C;       // 0x2C
    nw4r::g3d::ScnMdlExpand* m_unk30; // 0x30
    u8 m_unk34[0x44 - 0x34];
    u32 m_unk44;                      // 0x44
    u8 m_unk48[0x68 - 0x48];
    u32 m_unk68;                      // 0x68
    u32 m_unk6C;                      // 0x6C

    csFigure();
    virtual ~csFigure();
    virtual void beginOperation();
    virtual void afterOperation();
    virtual void debugDraw() { }
    virtual void applyDestroy();
    void setFigureModel();
};

class csEnemy : public csArticle {
public:
    u32 m_unk24;                     // 0x24
    nw4r::g3d::G3dObj* m_unk28;      // 0x28
    nw4r::g3d::ScnMdl* m_unk2C;      // 0x2C
    gfModelAnimation* m_unk30;       // 0x30
    float m_unk34;                   // 0x34
    float m_unk38;                   // 0x38
    float m_unk3C;                   // 0x3C
    u32 m_unk40;                     // 0x40
    u8 m_unk44[0x50 - 0x44];
    s32 m_unk50;                     // 0x50
    u32 m_unk54;                     // 0x54
    s32 m_unk58;                     // 0x58
    u8 m_unk5C[0x70 - 0x5C];
    u32 m_unk70;                     // 0x70

    csEnemy();
    virtual ~csEnemy();
    virtual void beginOperation();
    virtual void afterOperation();
    virtual void debugDraw() { }
    virtual void applyDestroy();
    void setEnemyModel();
};

class csMissile : public csArticle {
public:
    u32 m_unk24;                     // 0x24
    nw4r::g3d::G3dObj* m_unk28;      // 0x28
    u8 m_unk2C[0x58 - 0x2C];
    u32 m_unk58;                     // 0x58
    u8 m_unk5C[0x70 - 0x5C];
    u32 m_unk70;                     // 0x70
    IfCoinShooterLabel m_label;      // 0x74

    csMissile();
    virtual ~csMissile();
    virtual void beginOperation();
    virtual void afterOperation();
    virtual void debugDraw() { }
    virtual void applyDestroy();
};

csFigure::csFigure() : m_unk24(0), m_unk28(NULL), m_unk2C(NULL), m_unk30(NULL), m_unk44(2), m_unk6C(0) { }

csFigure::~csFigure() { }

csEnemy::csEnemy() : m_unk24(0), m_unk28(NULL), m_unk2C(NULL), m_unk40(3), m_unk70(0) { }

csEnemy::~csEnemy() { }

csMissile::csMissile() : m_unk24(0), m_unk28(NULL), m_unk58(3), m_unk70(0) { }

csMissile::~csMissile() { }

void csFigure::afterOperation() {
    lbl_20_bss_8->m_physicsWorld->m_inner->m_world->lock();
    lbl_20_bss_8->m_physicsWorld->m_inner->m_world->unlock();
}


void csFigure::applyDestroy() {
    if (m_unk2C != NULL) {
        m_unk2C->Destroy();
        m_unk2C = NULL;
    }
    if (m_unk30 != NULL) {
        m_unk30->Destroy();
        m_unk30 = NULL;
    }
    if (m_unk28 != NULL) {
        m_unk28->Destroy();
        m_unk28 = NULL;
    }
}

void csFigure::setFigureModel() {
    nw4r::g3d::ResFile resFile = lbl_20_bss_8->m_figEntries[m_unk68].m_ptr->m_resFile;
    nw4r::g3d::ResFile::Init(&resFile);
    resFile.Bind();
    gfSceneRoot* sceneRoot = g_gfSceneRoot;
    nw4r::g3d::ResMdl resMdl = resFile.GetResMdl(0);
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    u32 size;
    m_unk28 = ScnMdl_Construct(allocator, &size, resMdl, 0, 1, fn_801B0A28);
    nw4r::g3d::ResMat resMat = resMdl.GetResMat(0);
    resMat.Bind(resFile);
    if (m_unk2C == NULL) {
        resFile = lbl_805A0310->m_standRes;
        nw4r::g3d::ResFile::Init(&resFile);
        resFile.Bind();
        resMdl = resFile.GetResMdl(0);
        u32 standSize;
        m_unk2C = ScnMdl_Construct(allocator, &standSize, resMdl, 0, 1, fn_801B0A28);
        resMat = resMdl.GetResMat(0);
        resMat.Bind(resFile);
    }
    m_unk30 = nw4r::g3d::ScnMdlExpand::Construct(allocator, NULL, 1, m_unk28);
    m_unk30->PushBack(m_unk2C, "pos00");
    sceneRoot->add(gfSceneRoot::Layer_Ground, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_unk30));
}

void csEnemy::afterOperation() {
    lbl_20_bss_8->m_physicsWorld->m_inner->m_world->lock();
    lbl_20_bss_8->m_physicsWorld->m_inner->m_world->unlock();
}


void csEnemy::applyDestroy() {
    if (m_unk28 != NULL) {
        m_unk28->Destroy();
        m_unk28 = NULL;
    }
}

void csEnemy::setEnemyModel() {
    csCoinshooterView* view = lbl_20_bss_8;
    m_unk2C = view->m_enemyModels[m_unk50][m_unk58];
    m_unk30 = view->m_enemyAnims[m_unk50][m_unk58];
    m_unk30->setFrame(0.0f);
    m_unk30->setUpdateRate(1.0f);
    g_gfSceneRoot->add(gfSceneRoot::Layer_Ground, m_unk2C);
    Matrix mtx(true);
    mtx.setIdentity();
    fn_8003F074(&mtx, m_unk34, m_unk38, m_unk3C);
    ScnMdl_SetNodeMtx(m_unk2C, 0, &mtx);
    m_unk2C->UpdateFrame();
}

void csPathArticle::checkBonusFire(const Vec3f* pos, bool hit) {
    csCoinshooterView* view = lbl_20_bss_8;
    if (hit) {
        if (view->m_unk46 != 0) {
            g_sndSystem->playSE((SndID)0x1EC7, -1, 0, 0, -1);
        }
        IfCoinShooterLabel label;
        label.create(IfCoinShooterLabel::Type_3, 0, 0);
        label.setPos(pos);
    }
    if (view->m_unk124 >= view->m_unk120) {
        view->m_unk384 = view->m_unk46 != 0 ? g_sndSystem->playSE((SndID)0x1EC5, -1, 0, 0, -1) : 0;
        view->m_unk64 = 1;
        view->m_unk378 = view->m_unk37C;
        view->m_unk374.create(IfCoinShooterLabel::Type_6, 0, 0);
        view->m_unk124 -= view->m_unk120;
    }
    lbl_805A0310->m_bonusTarget = view->m_unk124;
}

void csMissile::afterOperation() {
    lbl_20_bss_8->m_physicsWorld->m_inner->m_world->lock();
    lbl_20_bss_8->m_physicsWorld->m_inner->m_world->unlock();
}

void csMissile::applyDestroy() {
    if (m_unk28 != NULL) {
        m_unk28->Destroy();
        m_unk28 = NULL;
    }
}
