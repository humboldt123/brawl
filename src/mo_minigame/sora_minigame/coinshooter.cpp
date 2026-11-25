#define SORA_MINIGAME_PH_WORLD_NONVIRTUAL_STEPTIME
#include <types.h>
#include <sr/sr_common.h>
#include <mt/mt_prng.h>
#include <mt/mt_vector.h>
#include <gf/gf_application.h>
#include <gf/gf_callback.h>
#include <gf/gf_task.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_model.h>
#include <if/if_figure_loader.h>
#include <if/if_minigame_coin_shooter.h>
#include <havok/hkArray.h>
#include <havok/hkWorld.h>
#include <havok/hkEntity.h>
#include <havok/hkRigidBody.h>
#include <havok/hkGroupFilter.h>
#include <ph/ph_world.h>
#include <ph/ph_actor.h>

extern "C" u32 gmGetCoinNum();

namespace utility {
template <class T>
class SafePtr {
public:
    T* m_ptr;
    SafePtr() : m_ptr(NULL) { }
    ~SafePtr();
};
}

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

class csModuleInterface {
public:
    virtual ~csModuleInterface() { }
};

struct csElementEntry {
    csElement* m_element;
    int m_group;
};

class csModule : public csModuleInterface {
public:
    hkArray<csElementEntry> m_elements; // 0x04
    csWorld* m_world;                   // 0x10

    virtual ~csModule();
    virtual void initLoadParam();
    virtual void removeWork();

    csElement* getElement(const int& index);
    bool entryElement(csElement* element, const int& group);
    bool removeElement(csElement* element);
};

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
};

class csFigure : public csArticle {
public:
    hkRigidBody* m_rigidBody;       // 0x24
    nw4r::g3d::ScnMdl* m_scnMdl28;  // 0x28
    nw4r::g3d::ScnMdl* m_scnMdl2C;  // 0x2C
    nw4r::g3d::ScnMdlExpand* m_scnMdl30; // 0x30
    u8 m_unk34[0x48 - 0x34];
    s32 m_unk48;                    // 0x48
    u8 m_unk4C[0x68 - 0x4C];
    s32 m_index;                    // 0x68
    s32 m_unk6C;                    // 0x6C

    csFigure();
    virtual ~csFigure();
};

class csEnemy : public csArticle {
public:
    hkRigidBody* m_rigidBody;       // 0x24
    nw4r::g3d::ScnMdl* m_scnMdl28;  // 0x28
    nw4r::g3d::ScnMdl* m_scnMdl2C;  // 0x2C
    u32 m_unk30;
    Vec3f m_pos34;                  // 0x34
    s32 m_unk40;                    // 0x40
    s32 m_state;                    // 0x44
    float m_unk48;                  // 0x48
    s32 m_unk4C;                    // 0x4C
    s32 m_kind;                     // 0x50
    s32 m_pathIdx;                  // 0x54
    s32 m_unk58;                    // 0x58
    float m_unk5C;                  // 0x5C
    float m_unk60;                  // 0x60
    s32 m_unk64;                    // 0x64
    float m_unk68;                  // 0x68
    float m_unk6C;                  // 0x6C
    u8 m_unk70[0x8C - 0x70];

    csEnemy();
    virtual ~csEnemy();
    void setEnemyModel();
    void makeNewEnemy();
};

class csMissile : public csArticle {
public:
    hkRigidBody* m_rigidBody;       // 0x24
    nw4r::g3d::ScnMdl* m_scnMdl28;  // 0x28
    gfModelAnimation* m_modelAnim;  // 0x2C
    u8 m_unk30[0x78 - 0x30];

    csMissile();
    virtual ~csMissile();
    void setMissileFirstPosition(int unk);
    void makeNewMissile();
};

class csMotionPath : public csArticle {
public:
    virtual void update(float deltaFrame);
    virtual void processAnim();
    virtual int getModelCount();
    virtual void startMove();
};

template <class T>
class phSingleton {
public:
    static T* s_instance;
    ~phSingleton() {
        s_instance = NULL;
    }
};

class csCoinshooter : public gfTask, public phSingleton<csCoinshooter> {
public:
    u8 m_unk40; // 0x40
    u8 m_unk41; // 0x41
    u8 m_unk42; // 0x42
    u8 m_unk43; // 0x43
    u8 m_unk44; // 0x44
    u8 m_unk45; // 0x45
    u8 m_unk46; // 0x46
    u8 m_unk47; // 0x47
    s32 m_unk48; // 0x48
    s32 m_unk4C; // 0x4C
    s32 m_unk50; // 0x50
    s32 m_unk54; // 0x54
    float m_unk58; // 0x58
    u8 m_unk5C; // 0x5C
    s32 m_unk60; // 0x60
    u8 m_unk64; // 0x64
    s32 m_unk68; // 0x68
    s32 m_unk6C; // 0x6C
    s32 m_unk70; // 0x70
    s32 m_unk74; // 0x74
    s32 m_unk78; // 0x78
    u8 m_unk7C; // 0x7C
    float m_unk80; // 0x80
    float m_unk84; // 0x84
    float m_unk88; // 0x88
    s32 m_unk8C; // 0x8C
    s32 m_unk90; // 0x90
    s32 m_unk94; // 0x94
    s32 m_unk98; // 0x98
    s32 m_unk9C; // 0x9C
    s32 m_unkA0; // 0xA0
    s32 m_unkA4; // 0xA4
    u8 m_unkA8; // 0xA8
    u8 m_unkA9; // 0xA9
    s32 m_unkAC; // 0xAC
    s32 m_unkB0; // 0xB0
    s32 m_unkB4; // 0xB4
    u8 m_unkB8; // 0xB8
    s32 m_unkBC; // 0xBC
    s32 m_unkC0; // 0xC0
    float m_unkC4; // 0xC4
    float m_unkC8; // 0xC8
    s32 m_unkCC; // 0xCC
    s32 m_unkD0; // 0xD0
    s32 m_unkD4; // 0xD4
    s32 m_unkD8; // 0xD8
    s32 m_unkDC; // 0xDC
    s32 m_unkE0; // 0xE0
    s32 m_unkE4; // 0xE4
    s32 m_unkE8; // 0xE8
    s32 m_unkEC; // 0xEC
    s32 m_unkF0; // 0xF0
    s32 m_unkF4; // 0xF4
    s32 m_unkF8; // 0xF8
    s32 m_unkFC; // 0xFC
    float m_unk100; // 0x100
    float m_unk104; // 0x104
    u8 m_unk108; // 0x108
    u8 m_unk109; // 0x109
    u8 m_unk10A; // 0x10A
    u8 m_unk10B; // 0x10B
    u8 m_unk10C; // 0x10C
    u8 m_unk10D; // 0x10D
    u8 m_unk10E; // 0x10E
    u8 m_unk10F; // 0x10F
    u8 m_unk110; // 0x110
    u8 m_unk111; // 0x111
    s32 m_unk114; // 0x114
    s32 m_unk118; // 0x118
    s32 m_unk11C; // 0x11C
    s32 m_unk120; // 0x120
    s32 m_unk124; // 0x124
    s32 m_unk128; // 0x128
    u8 m_unk12C; // 0x12C
    u8 m_unk12D; // 0x12D
    u8 m_unk12E; // 0x12E
    u8 m_unk12F; // 0x12F
    u8 m_unk130; // 0x130
    u8 m_unk131; // 0x131
    u8 m_unk132; // 0x132
    u8 m_unk133; // 0x133
    u8 m_unk134; // 0x134
    u8 m_unk135; // 0x135
    u8 m_unk136; // 0x136
    u8 m_unk137; // 0x137
    u8 m_unk138; // 0x138
    u8 m_unk139; // 0x139
    u8 m_unk13A; // 0x13A
    u8 m_unk13B; // 0x13B
    s32 m_unk13C; // 0x13C
    s32 m_unk140; // 0x140
    s32 m_unk144; // 0x144
    s32 m_unk148; // 0x148
    s32 m_unk14C; // 0x14C
    s32 m_unk150; // 0x150
    s32 m_unk154; // 0x154
    s32 m_unk158; // 0x158
    s32 m_unk15C; // 0x15C
    s32 m_unk160; // 0x160
    s32 m_unk164; // 0x164
    s32 m_unk168; // 0x168
    s32 m_unk16C; // 0x16C
    s32 m_unk170; // 0x170
    s32 m_unk174; // 0x174
    s32 m_unk178; // 0x178
    u8 m_unk17C; // 0x17C
    u8 m_unk17D; // 0x17D
    u8 m_unk17E; // 0x17E
    u8 m_unk17F; // 0x17F
    u8 m_unk180; // 0x180
    u8 m_unk181; // 0x181
    u8 m_unk182; // 0x182
    u8 m_unk183; // 0x183
    u8 m_unk184; // 0x184
    float m_unk188; // 0x188
    u8 m_unk18C; // 0x18C
    u8 m_unk18D; // 0x18D
    u8 m_unk18E; // 0x18E
    u8 m_unk18F;
    void* m_unk190; // 0x190
    void* m_unk194; // 0x194
    u32 m_unk198; // 0x198
    u32 m_unk19C; // 0x19C
    hkGroupFilter m_groupFilter; // 0x1A0
    float m_unk23C; // 0x23C
    u32 m_unk240; // 0x240
    csMotionPath* m_paths[5][7]; // 0x244
    csMotionPath* m_path2D0; // 0x2D0
    u8 m_unk2D4; // 0x2D4
    u32 m_unk2D8; // 0x2D8
    u32 m_unk2DC; // 0x2DC
    u32 m_unk2E0; // 0x2E0
    u32 m_unk2E4; // 0x2E4
    u32 m_unk2E8; // 0x2E8
    nw4r::g3d::ScnMdl* m_unk2EC; // 0x2EC
    u32 m_unk2F0; // 0x2F0
    u32 m_unk2F4; // 0x2F4
    u32 m_unk2F8[3]; // 0x2F8
    u32 m_unk304; // 0x304
    u32 m_unk308; // 0x308
    u32 m_unk30C; // 0x30C
    void* m_unk310; // 0x310
    u32 m_unk314; // 0x314
    u32 m_unk318; // 0x318
    u32 m_unk31C; // 0x31C
    u32 m_unk320[5]; // 0x320
    u32 m_unk334; // 0x334
    u32 m_unk338; // 0x338
    u8 m_unk33C[3]; // 0x33C
    u32 m_unk340[3]; // 0x340
    utility::SafePtr<IfFigureLoader::Entry> m_figEntries[3]; // 0x34C
    u8 m_unk358[3]; // 0x358
    u8 m_unk35B; // 0x35B
    u8 m_unk35C; // 0x35C
    u8 m_unk35D; // 0x35D
    u32 m_unk360; // 0x360
    Vec3f m_unk364; // 0x364
    u8 m_unk370; // 0x370
    u8 m_unk371; // 0x371
    IfCoinShooterLabel m_label374; // 0x374
    u32 m_unk378; // 0x378
    s32 m_unk37C; // 0x37C
    s32 m_unk380; // 0x380
    s32 m_unk384; // 0x384
    u8 m_unk388; // 0x388
    float m_unk38C; // 0x38C
    Vec3f m_unk390; // 0x390
    u32 m_unk39C; // 0x39C
    u32 m_unk3A0[4]; // 0x3A0
    nw4r::g3d::ScnMdl* m_enemyMdls[5][6]; // 0x3B0
    u8 m_enemyFlags[5][6]; // 0x428
    gfModelAnimation* m_enemyAnims[5][6]; // 0x448
    csModule* m_module; // 0x4C0
    hkArray<csCoin*> m_coins; // 0x4C4
    hkArray<csFigure*> m_figures; // 0x4D0
    hkArray<csEnemy*> m_enemies; // 0x4DC
    hkArray<csMissile*> m_missiles; // 0x4E8
    hkArray<csArticle*> m_destroyList; // 0x4F4

    csCoinshooter();
    virtual void processUpdate();
    virtual void renderDebug();
    virtual ~csCoinshooter();

    void removeCoinAll();
    void removeFigureAll();
    void removeFigureAll2();
    void removeEnemyAll();
    void removeMissileAll();
    void removeMissileNon0();
    void setActiveModule(csModule* module);
    void removeModule(csModule* module);
    bool entryElement(csElement* element, const int& group);
    bool removeElement(csElement* element);
    void restartFigure();
    void restartEnemy();
    void setEnemy1stParam(csEnemy* enemy);
    void restartMissile();
    void setMissile1stParam(csMissile* missile);
    bool drawReleaseCheck();
    void notifyDrawDone();
    void clr();
};

extern csCoinshooter* lbl_20_bss_8;

class scCoinshooterDrawDownCallback : public gfCallBack {
public:
    virtual void userProc();
    virtual ~scCoinshooterDrawDownCallback() { }
};

struct csHkRigidBody : hkRigidBody {
    virtual hkMotionState* getMotionState();
    virtual void deallocateInternalArrays();
    virtual bool setShape(hkShape* shape);
    virtual hkRigidBody* clone() const;
};

static scCoinshooterDrawDownCallback lbl_20_bss_18;

static const int lbl_20_data_438[5] = { 6, 6, 5, 3, 3 };
static const int lbl_20_data_44C[5] = { 4, 4, 5, 1, 1 };

void scCoinshooterDrawDownCallback::userProc() {
    if (lbl_20_bss_8 != NULL) {
        lbl_20_bss_8->notifyDrawDone();
    }
}

csCoinshooter::~csCoinshooter() {
    IfCoinShooter::destroy();
    IfFigureLoader::destroy();
    g_gfApplication->m_114.remove(&lbl_20_bss_18);
}

void csCoinshooter::removeFigureAll() {
    for (int i = m_figures.getSize() - 1; i >= 0; i--) {
        if (m_figures[i]->m_scnMdl30 != NULL) {
            s32 idx = m_figures[i]->m_index;
            if (m_figEntries[idx].m_ptr != NULL) {
                m_figEntries[idx].m_ptr->destroy();
            }
            m_figEntries[idx].m_ptr = NULL;
            if (m_figures[i]->m_scnMdl2C != NULL) {
                m_figures[i]->m_scnMdl30->Remove(m_figures[i]->m_scnMdl2C);
                m_figures[i]->m_unk20 = 1;
            }
            g_gfSceneRoot->remove(reinterpret_cast<nw4r::g3d::ScnMdl*>(m_figures[i]->m_scnMdl30));
            m_figures[i]->m_unk20 = 1;
            if (m_figures[i]->m_scnMdl28 != NULL) {
                m_figures[i]->m_unk20 = 1;
            }
        } else if (m_figures[i]->m_scnMdl2C != NULL) {
            m_figures[i]->m_unk20 = 1;
        }
        m_figures[0]->m_rigidBody->m_world->removeEntity(m_figures[i]->m_rigidBody);
        m_figures[i]->m_rigidBody->removeReference();
        csArticle* fig = m_figures[i];
        m_destroyList.pushBack(fig);
    }
    m_figures.clearAndDeallocate();
}

void csCoinshooter::removeFigureAll2() {
    int start = m_figures.getSize() - 1;
    if (m_unk35C == 1) {
        start = 0;
    }
    for (int i = start; i >= 0; i--) {
        if (m_figures[i]->m_scnMdl30 != NULL) {
            s32 idx = m_figures[i]->m_index;
            if (m_figEntries[idx].m_ptr != NULL) {
                m_figEntries[idx].m_ptr->destroy();
            }
            m_figEntries[idx].m_ptr = NULL;
            if (m_figures[i]->m_scnMdl2C != NULL) {
                m_figures[i]->m_scnMdl30->Remove(m_figures[i]->m_scnMdl2C);
                m_figures[i]->m_unk20 = 1;
            }
            g_gfSceneRoot->remove(reinterpret_cast<nw4r::g3d::ScnMdl*>(m_figures[i]->m_scnMdl30));
            m_figures[i]->m_unk20 = 1;
            if (m_figures[i]->m_scnMdl28 != NULL) {
                m_figures[i]->m_unk20 = 1;
            }
        } else if (m_figures[i]->m_scnMdl2C != NULL) {
            m_figures[i]->m_unk20 = 1;
        }
        csArticle* fig = m_figures[i];
        m_destroyList.pushBack(fig);
        if (i != 0) {
            m_figures[0]->m_rigidBody->m_world->removeEntity(m_figures[i]->m_rigidBody);
            m_figures[i]->m_rigidBody->removeReference();
            m_figures.popBack();
        }
    }
}

void csCoinshooter::removeMissileAll() {
    gfSceneRoot* sceneRoot = g_gfSceneRoot;
    for (int i = m_missiles.getSize() - 1; i >= 0; i--) {
        if (m_missiles[i]->m_scnMdl28 != NULL) {
            gfModelAnimation::unbind(m_missiles[i]->m_scnMdl28);
            gfModelAnimation* modelAnim = m_missiles[i]->m_modelAnim;
            if (modelAnim != NULL) {
                if (modelAnim->m_anmObjChrRes != NULL) {
                    modelAnim->m_anmObjChrRes->Destroy();
                }
                if (modelAnim->m_anmObjVisRes != NULL) {
                    modelAnim->m_anmObjVisRes->Destroy();
                }
                if (modelAnim->m_anmObjTexPatRes != NULL) {
                    modelAnim->m_anmObjTexPatRes->Destroy();
                }
                if (modelAnim->m_anmObjTexSrtRes != NULL) {
                    modelAnim->m_anmObjTexSrtRes->Destroy();
                }
                if (modelAnim->m_anmObjMatClrRes != NULL) {
                    modelAnim->m_anmObjMatClrRes->Destroy();
                }
                if (modelAnim->m_anmObjShpRes != NULL) {
                    modelAnim->m_anmObjShpRes->Destroy();
                }
                if (modelAnim->m_resFile.ptr() != NULL && (u8)modelAnim->_spacer[0] != 0) {
                    gfHeapManager::free(modelAnim->m_resFile.ptr());
                    modelAnim->m_resFile = nw4r::g3d::ResFile();
                }
                modelAnim->m_anmObjChrRes = NULL;
                modelAnim->m_anmObjVisRes = NULL;
                modelAnim->m_anmObjTexPatRes = NULL;
                modelAnim->m_anmObjTexSrtRes = NULL;
                modelAnim->m_anmObjMatClrRes = NULL;
                modelAnim->m_anmObjShpRes = NULL;
                delete modelAnim;
            }
            m_missiles[i]->m_modelAnim = NULL;
            sceneRoot->remove(m_missiles[i]->m_scnMdl28);
            m_missiles[i]->m_unk20 = 1;
        }
        m_missiles[0]->m_rigidBody->m_world->removeEntity(m_missiles[i]->m_rigidBody);
        m_missiles[i]->m_rigidBody->removeReference();
        csArticle* missile = m_missiles[i];
        m_destroyList.pushBack(missile);
    }
    m_missiles.clearAndDeallocate();
}

void csCoinshooter::removeMissileNon0() {
    gfSceneRoot* sceneRoot = g_gfSceneRoot;
    for (int i = m_missiles.getSize() - 1; i >= 1; i--) {
        if (m_missiles[i]->m_scnMdl28 != NULL) {
            gfModelAnimation::unbind(m_missiles[i]->m_scnMdl28);
            gfModelAnimation* modelAnim = m_missiles[i]->m_modelAnim;
            if (modelAnim != NULL) {
                if (modelAnim->m_anmObjChrRes != NULL) {
                    modelAnim->m_anmObjChrRes->Destroy();
                }
                if (modelAnim->m_anmObjVisRes != NULL) {
                    modelAnim->m_anmObjVisRes->Destroy();
                }
                if (modelAnim->m_anmObjTexPatRes != NULL) {
                    modelAnim->m_anmObjTexPatRes->Destroy();
                }
                if (modelAnim->m_anmObjTexSrtRes != NULL) {
                    modelAnim->m_anmObjTexSrtRes->Destroy();
                }
                if (modelAnim->m_anmObjMatClrRes != NULL) {
                    modelAnim->m_anmObjMatClrRes->Destroy();
                }
                if (modelAnim->m_anmObjShpRes != NULL) {
                    modelAnim->m_anmObjShpRes->Destroy();
                }
                if (modelAnim->m_resFile.ptr() != NULL && (u8)modelAnim->_spacer[0] != 0) {
                    gfHeapManager::free(modelAnim->m_resFile.ptr());
                    modelAnim->m_resFile = nw4r::g3d::ResFile();
                }
                modelAnim->m_anmObjChrRes = NULL;
                modelAnim->m_anmObjVisRes = NULL;
                modelAnim->m_anmObjTexPatRes = NULL;
                modelAnim->m_anmObjTexSrtRes = NULL;
                modelAnim->m_anmObjMatClrRes = NULL;
                modelAnim->m_anmObjShpRes = NULL;
                delete modelAnim;
            }
            m_missiles[i]->m_modelAnim = NULL;
            sceneRoot->remove(m_missiles[i]->m_scnMdl28);
            m_missiles[i]->m_unk20 = 1;
        }
        m_missiles[0]->m_rigidBody->m_world->removeEntity(m_missiles[i]->m_rigidBody);
        m_missiles[i]->m_rigidBody->removeReference();
        csArticle* missile = m_missiles[i];
        m_destroyList.pushBack(missile);
        m_missiles.popBack();
    }
}

void csCoinshooter::notifyDrawDone() {
    for (int i = 0; i < m_destroyList.getSize(); i++) {
        csArticle* article = m_destroyList[i];
        article->applyDestroy();
        article->m_unk20 = 0;
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 7; k++) {
                if (m_paths[j][k] == article) {
                    m_paths[j][k] = NULL;
                }
            }
        }
        if (m_path2D0 == article) {
            m_path2D0 = NULL;
        }
        m_destroyList.removeAt(i);
        if (m_unk43 != 0 || m_figures[0] != article) {
            delete article;
        }
    }
    if (m_unk371 != 0) {
        m_unk371 = 0;
    }
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 6; j++) {
            if (m_enemyMdls[i][j] != NULL && m_enemyFlags[i][j] != 0) {
                m_enemyMdls[i][j]->Destroy();
                m_enemyFlags[i][j] = 0;
                m_enemyMdls[i][j] = NULL;
            }
        }
    }
}

void csCoinshooter::setActiveModule(csModule* module) {
    if (m_module != NULL) {
        delete m_module;
    }
    m_module = module;
}

void csCoinshooter::removeModule(csModule* module) {
    if (m_module == module) {
        m_module = NULL;
    }
}

void csCoinshooter::processUpdate() {
    if (m_module != NULL) {
        for (int i = 0; i < m_module->m_elements.getSize(); i++) {
            if (m_module->getElement(i) != NULL && m_module->getElement(i)->m_isActive != 0) {
                m_module->getElement(i)->beginOperation();
            }
        }
        m_module->m_world->stepTime(0.016666668f);
        for (int i = 0; i < m_module->m_elements.getSize(); i++) {
            if (m_module->getElement(i) != NULL && m_module->getElement(i)->m_isActive != 0) {
                m_module->getElement(i)->afterOperation();
            }
        }
    }
}

void csCoinshooter::renderDebug() {
    if (m_module != NULL) {
        for (int i = 0; i < m_module->m_elements.getSize(); i++) {
            if (m_module->getElement(i) != NULL && m_module->getElement(i)->m_isActive != 0) {
                m_module->getElement(i)->debugDraw();
            }
        }
    }
}

bool csCoinshooter::entryElement(csElement* element, const int& group) {
    return m_module->entryElement(element, group);
}

bool csCoinshooter::removeElement(csElement* element) {
    return m_module->removeElement(element);
}

void csCoinshooter::restartMissile() {
    removeMissileNon0();
    setMissile1stParam(m_missiles[0]);
    for (int i = 0; i < m_unk370 - 1; i++) {
        if (m_missiles.getSize() < 20) {
            csMissile* missile = new (Heaps::MenuInstance) csMissile();
            missile->makeNewMissile();
            m_missiles.pushBack(missile);
        }
    }
}

bool csCoinshooter::drawReleaseCheck() {
    if (m_destroyList.getSize() != 0) {
        return false;
    }
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 6; j++) {
            if (m_paths[i][j] != NULL && m_enemyFlags[i][j] != 0) {
                return false;
            }
        }
    }
    return true;
}

void csCoinshooter::clr() {
    m_destroyList.clearAndDeallocate();
}
