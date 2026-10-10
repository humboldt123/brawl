#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gr/collision/gr_collision.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

class grFixedPathCollection;
class grGimmickWaterData;
class stTrigger;

template<typename T>
class stClassInfoImpl<Stages::Dolpic, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Dolpic, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Dolpic, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// Delfino Plaza: the plaza moves through a script of scenes (m_state 0..0x20, two entries per scene: the stage waits
// in the even ones and moves the plaza on in the odd ones), sinking and raising the platforms (Ashiba) on the way.
// Some parts (bell, shine, seagulls) follow matrices the plaza model publishes.
class stDolpic : public stMelee {
    u8 m_state;                  // 0x1D8 scene script state
    u8 m_scene;                  // 0x1D9 scene animation that is loaded (0x10 = none)
    float m_sceneTimer;          // 0x1DC frames left of the current scene
    u8 m_sceneVisit[4];          // 0x1E0 which of the scene's two variants played last
    float m_ashibaRate;          // 0x1E4 how far the platforms have sunk/risen
    float m_cameraRate;          // 0x1E8
    int m_cameraData;            // 0x1EC id of the camera position data that is loaded
    int unk1F0;                  // 0x1F0
    u8 m_ashibaLast;             // 0x1F4 platform group that was moving last (5 = none)
    u8 m_ashibaState[5];         // 0x1F5 phase of the five platform groups
    snd3DGenerator m_snd0;       // 0x1FC sound sources (one per sound point)
    snd3DGenerator m_snd1;       // 0x204
    snd3DGenerator m_snd2;       // 0x20C
    snd3DGenerator m_snd3;       // 0x214
    snd3DGenerator m_snd4;       // 0x21C
    snd3DGenerator m_snd5;       // 0x224
    int m_seHandle[7];           // 0x22C handles of the looping sounds (-1 = none)
    float m_seTimer;             // 0x248 frames until the next ambient sound
    float m_seFade;              // 0x24C
    Vec3f m_posGimmick[36];      // 0x250 positions the plaza publishes
    Vec3f m_limit[2];            // 0x400 camera limits: top-left and bottom-right corner
    int m_trainerIndex;          // 0x418 first Pokemon Trainer position of the current group (0x24 = none)
    nw4r::math::MTX34 m_mtxGimmick[15]; // 0x41C matrices the plaza publishes (MATCH-ONLY: plain storage, the constructor
                                        // sets them up with explicit setIdentity calls)
    grCollision* m_collision[10]; // 0x6EC collisions of the scenes
    nw4r::math::MTX34 m_collisionMtx;   // 0x714 matrix the water-line collision is placed with
    u8 m_collisionScene;         // 0x744 scene whose collision is enabled
    u8 m_collisionScenePrev;     // 0x745 scene whose collision is on its way out
    float m_collisionTimer;      // 0x748
    Vec3f m_scale;               // 0x74C scale the plaza gives to the water and the bells
    grFixedPathCollection* m_pathData; // 0x758
    stTrigger* m_trigger;        // 0x75C water trigger
    grGimmickWaterData* m_waterData; // 0x760

public:
    Matrix* mtxGimmick(int index) { return static_cast<Matrix*>(&m_mtxGimmick[index]); }
    Matrix* collisionMtx() { return static_cast<Matrix*>(&m_collisionMtx); }
    stDolpic();
    static stDolpic* create();

    virtual ~stDolpic();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void createObjPathData();
    virtual void createObjMainBg();
    virtual void createObjWater(int index);
    virtual void createObjAshiba(int index);
    virtual void createObjShine();
    virtual void createObjBell(int index);
    virtual void createObjKamome(int index);
    virtual void createObjSwimArea();
    // HYPOTHESIS: the parameter meanings of the update/set functions below (the map gives only their names).
    virtual void updateLimit(float deltaFrame);
    virtual void updateScene(float deltaFrame);
    virtual void updateSceneSelect(float deltaFrame);
    virtual void updateAshiba(float deltaFrame);
    virtual void updateAshiba1(u8 group, u32 sinking, u32 unk);
    virtual void updateSE(float deltaFrame);
    virtual void updateCollision(float deltaFrame);
    virtual void updateCollision1(float deltaFrame, u32 scene, bool enable);
    virtual void updateWater(float deltaFrame);
    virtual void updateCameraCenter(float deltaFrame);
    virtual void updateTrainer(float deltaFrame);
    virtual void setSEVolAshibaSet(float volume, int index);
    virtual void setStateGroundCollision(u32 scene, u32 enable);
    virtual void setScene(u32 scene, int force);
    virtual void renderDebug();

    static stClassInfoImpl<Stages::Dolpic, stDolpic> bss_loc_14;
};
static_assert(sizeof(stDolpic) == 0x764, "Class is wrong size!");
