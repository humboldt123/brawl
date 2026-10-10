#pragma once

#include <StaticAssert.h>
#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_yakumono.h>
#include <math.h>
#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <so/collision/so_collision_hit_part.h>
#include <st/se_util.h>
#include <types.h>
#include <yk/yakumono.h>

#include <st_village/st_village.h>

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);

// The grounds make their own model animation (the file comes from the stage) instead of the shared ones: it is built word by
// word and freed by the ground. HYPOTHESIS: layout of gfModelAnimation as in gf/gf_model.h (the first byte after the
// resource tells that the animation owns the resource).
static inline gfModelAnimation* grVillageNewModelAnim(nw4r::g3d::ResFile* resFile) {
    // MATCH-ONLY: the original allocates the 32 bytes and fills them by hand
    gfModelAnimation* anim = reinterpret_cast<gfModelAnimation*>(new (Heaps::StageInstance) u8[sizeof(gfModelAnimation)]);
    if (anim != NULL) {
        anim->m_resFile = *resFile;
        anim->_spacer[0] = 1;
        anim->m_anmObjVisRes = NULL;
        anim->m_anmObjChrRes = NULL;
        anim->m_anmObjTexPatRes = NULL;
        anim->m_anmObjTexSrtRes = NULL;
        anim->m_anmObjMatClrRes = NULL;
        anim->m_anmObjShpRes = NULL;
    }
    return anim;
}

static inline void grVillageFreeModelAnim(gfModelAnimation* anim) {
    if (anim != NULL) {
        if (anim->m_anmObjChrRes != NULL) {
            anim->m_anmObjChrRes->Destroy();
        }
        if (anim->m_anmObjVisRes != NULL) {
            anim->m_anmObjVisRes->Destroy();
        }
        if (anim->m_anmObjTexPatRes != NULL) {
            anim->m_anmObjTexPatRes->Destroy();
        }
        if (anim->m_anmObjTexSrtRes != NULL) {
            anim->m_anmObjTexSrtRes->Destroy();
        }
        if (anim->m_anmObjMatClrRes != NULL) {
            anim->m_anmObjMatClrRes->Destroy();
        }
        if (anim->m_anmObjShpRes != NULL) {
            anim->m_anmObjShpRes->Destroy();
        }
        void** resPtr = reinterpret_cast<void**>(&anim->m_resFile);
        if (*resPtr != NULL && (u8)anim->_spacer[0] != 0) {
            gfHeapManager::free(*resPtr);
            *resPtr = NULL;
        }
        anim->m_anmObjChrRes = NULL;
        anim->m_anmObjVisRes = NULL;
        anim->m_anmObjTexPatRes = NULL;
        anim->m_anmObjTexSrtRes = NULL;
        anim->m_anmObjMatClrRes = NULL;
        anim->m_anmObjShpRes = NULL;
        delete anim;
    }
}

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct grVillageHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

// The base ground of the stage. The scene (time of day), the state of the stage and the places of the guests are shared
// with the stage by pointer.
class grVillage : public grYakumono {
protected:
    u8 m_state;                 // 0x150
    char _151[3];
    float m_timer;              // 0x154
    u8* m_sceneWork;            // 0x158 the time of day (from the stage)
    u8 m_sceneBit;              // 0x15C the grounds are only shown in the times of day whose bit is set
    char _15d[3];
    u8* m_stateWork;            // 0x160 (from the stage)
    Vec3f* m_posGuestWork;      // 0x164 the places of the guests (from the stage)

public:
    grVillage(const char* taskName);
    virtual ~grVillage();
    virtual void update(float deltaFrame);
    virtual void updateVisible(float deltaFrame);
    virtual void setSceneWork(u8* sceneWork) { m_sceneWork = sceneWork; }
    virtual void setSceneBit(u8 sceneBit) { m_sceneBit = sceneBit; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setPosGuestWork(Vec3f* posGuestWork) { m_posGuestWork = posGuestWork; }
    virtual bool isSceneBit();

    static grVillage* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillage) == 0x168, "Class is wrong size!");

// The main stage: the guests that sit and stand at its places follow the nodes of the stage model.
class grVillageStage : public grVillage {
public:
    grVillageStage(const char* taskName) : grVillage(taskName) { }
    virtual ~grVillageStage();
    virtual void processAnim();

    static grVillageStage* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageStage) == 0x168, "Class is wrong size!");

// The floating platform (it has an animation of its own).
class grVillageAshiba : public grVillage {
protected:
    u8 m_motion;                // 0x168
    char _169[3];
    float unk16C;               // 0x16C

public:
    grVillageAshiba(const char* taskName) : grVillage(taskName) {
        m_motion = 1;
        unk16C = 0.0f;
    }
    virtual ~grVillageAshiba();
    virtual void update(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);

    static grVillageAshiba* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageAshiba) == 0x170, "Class is wrong size!");

// A guest of the village. It stands (or sits) at a place that the stage gives it, looks at the fighters and blinks. The model
// is made of the common file of the guests (one model for all), the animations of it are in the file of the stage.
class grVillageGuest : public grVillage {
protected:
    u8 m_eyeState;              // 0x168
    u8 unk169;                  // 0x169
    char _16a[2];
    float m_eyeTimer;           // 0x16C
    float m_mouthTimer;         // 0x170
    Vec3f* m_posWork;           // 0x174 where the guest stands (from the stage)
    Vec3f* m_posGimmickWork;    // 0x178 the places of all the guests (from the stage)
    Vec3f m_offset;             // 0x17C
    Vec3f m_rot;                // 0x188
    Vec3f m_rotHead;            // 0x194
    u8* m_stateSubWork;         // 0x1A0 (from the stage)
    u8* m_guestListData;        // 0x1A4 which motions the guest may do
    u8 unk1A8;                  // 0x1A8
    u8 m_id;                    // 0x1A9 which guest
    char _1aa[2];
    u32 m_nodeHead;             // 0x1AC
    s32 m_target;               // 0x1B0 the fighter the guest looks at (-1 = none)
    u32 unk1B4;                 // 0x1B4
    u32 unk1B8;                 // 0x1B8
    u8 m_motion;                // 0x1BC
    char _1bd[3];
    float m_frameCount;         // 0x1C0
    u8 m_commonBound;           // 0x1C4
    u8 m_eye;                   // 0x1C5
    u8 m_mouth;                 // 0x1C6
    char _1c7;
    gfModelAnimation* m_commonAnim; // 0x1C8

public:
    grVillageGuest(const char* taskName);
    virtual ~grVillageGuest();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void unloadData();
    virtual void updateActive(float deltaFrame);
    virtual void updateActiveDetails(float deltaFrame);
    virtual void updateFace(float deltaFrame);
    virtual void updateFaceDetails(float deltaFrame);
    virtual void updateFaceEye(float deltaFrame);
    virtual void updateFaceMouth(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMotionCommon(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMotionFrameEye(float frame, u32 index);
    virtual float getMotionFrameEye(u32 index);
    virtual void setEye(u32 eye);
    virtual void setMouth(u32 mouth);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setPosGimmickWork(Vec3f* posGimmickWork) { m_posGimmickWork = posGimmickWork; }
    virtual void setStateSubWork(u8* stateSubWork) { m_stateSubWork = stateSubWork; }
    virtual void setRotY(float rotY) { m_rot.m_y = rotY; }
    virtual void setID(u8 id) { m_id = id; }
    virtual void setResCommon(nw4r::g3d::ResFile* resFile);
    virtual void setGuestListData(u8* guestListData) { m_guestListData = guestListData; }
    virtual bool getPlayerPosition(int index, Vec3f* pos);
    virtual bool isRotBody();
    virtual bool isRotHead();

    static grVillageGuest* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageGuest) == 0x1CC, "Class is wrong size!");

class grVillageGuestMaster : public grVillageGuest {
public:
    grVillageGuestMaster(const char* taskName) : grVillageGuest(taskName) { }
    virtual ~grVillageGuestMaster();
    virtual void updateActive(float deltaFrame);
    virtual void updateFace(float deltaFrame) { }

    static grVillageGuestMaster* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageGuestMaster) == 0x1CC, "Class is wrong size!");

class grVillageGuestHatonosu : public grVillageGuest {
public:
    grVillageGuestHatonosu(const char* taskName) : grVillageGuest(taskName) { }
    virtual ~grVillageGuestHatonosu();
    virtual void processAnim();

    static grVillageGuestHatonosu* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageGuestHatonosu) == 0x1CC, "Class is wrong size!");

class grVillageGuestMonban : public grVillageGuest {
public:
    grVillageGuestMonban(const char* taskName) : grVillageGuest(taskName) { }
    virtual ~grVillageGuestMonban();
    virtual void updateActive(float deltaFrame);

    static grVillageGuestMonban* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageGuestMonban) == 0x1CC, "Class is wrong size!");

class grVillageGuestFuta : public grVillageGuest {
public:
    grVillageGuestFuta(const char* taskName) : grVillageGuest(taskName) { }
    virtual ~grVillageGuestFuta();
    virtual void updateActive(float deltaFrame);

    static grVillageGuestFuta* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageGuestFuta) == 0x1CC, "Class is wrong size!");

class grVillageGuestAyasiNeko : public grVillageGuest {
protected:
    u8 m_eyeSet;                // 0x1CC
    char _1cd[3];

public:
    grVillageGuestAyasiNeko(const char* taskName) : grVillageGuest(taskName) { m_eyeSet = 0; }
    virtual ~grVillageGuestAyasiNeko();
    virtual void updateFaceDetails(float deltaFrame);

    static grVillageGuestAyasiNeko* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageGuestAyasiNeko) == 0x1D0, "Class is wrong size!");

class grVillageGuestTotakeke : public grVillageGuest {
public:
    grVillageGuestTotakeke(const char* taskName) : grVillageGuest(taskName) { }
    virtual ~grVillageGuestTotakeke();
    virtual void updateActive(float deltaFrame);

    static grVillageGuestTotakeke* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageGuestTotakeke) == 0x1CC, "Class is wrong size!");

// Something that moves along a path of the stage model while a sound plays (the bird, the taxi, the UFO).
class grVillageGuestPathMove : public grVillage {
protected:
    u8 m_type;                              // 0x168 which one (3 = the UFO)
    u8 m_first;                             // 0x169
    u8 m_motion;                            // 0x16A
    char _16b;
    float m_frame;                          // 0x16C
    float m_frameCount;                     // 0x170
    StSeUtil::SeSeqInstance<1, 1> m_sePlayer; // 0x174
    SndID m_seIds[1];                       // 0x1B0
    StSeUtil::UnkStruct m_seData;           // 0x1B4
    snd3DGenerator m_snd;                   // 0x1C4

public:
    grVillageGuestPathMove(const char* taskName);
    virtual ~grVillageGuestPathMove();
    virtual void update(float deltaFrame);
    virtual void setMotionRatio(float ratio) { m_motionRatio = ratio; }
    virtual void updateYakumono(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setType(u8 type) { m_type = type; }

    static grVillageGuestPathMove* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageGuestPathMove) == 0x1CC, "Class is wrong size!");

// The lamps of the stage are lit at night.
class grVillageLiveDeco : public grVillage {
protected:
    u8 m_light;                 // 0x168
    char _169[3];
    u32 m_node[4];              // 0x16C

public:
    grVillageLiveDeco(const char* taskName) : grVillage(taskName) {
        m_light = 6;
        m_node[0] = 0;
        m_node[1] = 0;
        m_node[2] = 0;
        m_node[3] = 0;
    }
    virtual ~grVillageLiveDeco();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updateLight(float deltaFrame);

    static grVillageLiveDeco* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageLiveDeco) == 0x17C, "Class is wrong size!");

// The balloon that flies by: it can be hit (it drops an item when it is popped).
class grVillageBalloon : public grVillageGuestPathMove {
protected:
    u8 m_balloonMotion;                     // 0x1CC
    char _1cd[3];
    Vec3f m_posBalloon;                     // 0x1D0
    float m_wait;                           // 0x1DC
    u32 m_itemId;                           // 0x1E0
    u32 m_nodeItem;                         // 0x1E4
    u8 m_itemMake;                          // 0x1E8
    u8 m_hasHit;                            // 0x1E9
    u8 unk1EA;                              // 0x1EA
    char _1eb;
    soCollisionHitData* m_hitData;          // 0x1EC the hit sphere
    soCollisionHitData::Simple* m_hitSimple; // 0x1F0 its form for the hit object
    soSet<soCollisionHitData>* m_hitSet;    // 0x1F4
    ykDataGroup* m_dataGroup;               // 0x1F8
    ykData* m_data;                         // 0x1FC
    snd3DGenerator m_balloonSnd;            // 0x200

public:
    grVillageBalloon(const char* taskName);
    virtual ~grVillageBalloon();
    virtual void processFixPosition();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual bool setNode();
    virtual void updateYakumono(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();

    static grVillageBalloon* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageBalloon) == 0x208, "Class is wrong size!");

// The clock tower: the hands show the time of the console.
class grVillageClock : public grVillage {
protected:
    Vec3f m_rotHour;            // 0x168
    Vec3f m_rotMinute;          // 0x174
    u32 m_nodeHour;             // 0x180
    u32 m_nodeMinute;           // 0x184

public:
    grVillageClock(const char* taskName);
    virtual ~grVillageClock();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updateClock(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);

    static grVillageClock* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageClock) == 0x188, "Class is wrong size!");

// The sky changes its colors with the time of day.
class grVillageSky : public grVillage {
protected:
    u8 m_scene;                 // 0x168
    char _169[3];

public:
    grVillageSky(const char* taskName) : grVillage(taskName) { m_scene = 5; }
    virtual ~grVillageSky();
    virtual void update(float deltaFrame);
    virtual void changeColor();

    static grVillageSky* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grVillageSky) == 0x16C, "Class is wrong size!");
