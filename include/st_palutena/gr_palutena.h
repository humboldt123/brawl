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
#include <types.h>
#include <yk/yakumono.h>

#include <st_palutena/st_palutena.h>

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);

// ecMgr::setEffect(EfID, const Vec3f*) (unnamed in the symbols; the code is the same as setEffect(EfID, Vec3f*))
extern "C" u32 fn_8005F800(ecMgr* mgr, EfID effectID, Vec3f* pos);

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct grPalutenaHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

// MATCH-ONLY: layout of soSet<T> (its members are private)
struct grPalutenaSetView {
    void* m_elements;
    u32 m_size;
};

// MATCH-ONLY: the bytes of the match settings the stages read (their names are unknown): byte 0 holds the game mode in its
// upper six bits and byte 8 the event.
struct grPalutenaMeleeBytes {
    unsigned char m_mode : 6;
    unsigned char m_rest : 2;
    u8 _pad[7];
    u8 m_event;
};

// The grounds make their own model animation (the file comes from the stage) instead of the shared ones: it is built word by
// word and freed by the ground. HYPOTHESIS: layout of gfModelAnimation as in gf/gf_model.h (the first byte after the
// resource tells that the animation owns the resource).
static inline gfModelAnimation* grPalutenaNewModelAnim(nw4r::g3d::ResFile* resFile) {
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

static inline void grPalutenaFreeModelAnim(gfModelAnimation* anim) {
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
        if (*resPtr != NULL && anim->_spacer[0] != 0) {
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

static inline grPalutenaMeleeBytes* palutenaMeleeBytes() {
    return reinterpret_cast<grPalutenaMeleeBytes*>(&g_GameGlobal->m_modeMelee->m_meleeInitData);
}

// The base ground of the stage.
class grPalutena : public grYakumono {
protected:
    u8 m_state;     // 0x150
    char _151[3];
    float unk154;   // 0x154

public:
    grPalutena(const char* taskName);
    virtual ~grPalutena();
    static grPalutena* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPalutena) == 0x158, "Class is wrong size!");

// A platform of the temple. It is hit by the fighters (its life is kept by the stage), shakes, breaks in three steps (the
// models of the steps are the three levels of the platform) and builds itself up again.
class grPalutenaAshiba : public grPalutena {
protected:
    u8 unk158;                                  // 0x158
    char _159[3];
    float* m_hpWork;                            // 0x15C the life of the platform (from the stage)
    float m_hpPrev;                             // 0x160 the life of the last frame
    u8 unk164;                                  // 0x164
    char _165[3];
    float unk168;                               // 0x168
    u8 m_lastLevel;                             // 0x16C the break level of the last frame
    char _16d[3];
    float m_shakeTimer;                         // 0x170
    Vec3f m_shakePos;                           // 0x174 how far the platform is shaken
    Vec3f m_offset;                             // 0x180 where the platform is relative to its place
    Vec3f m_scale;                              // 0x18C
    u8 m_type;                                  // 0x198 which platform (0 - 8)
    u8 m_level;                                 // 0x199 which model of the platform (0 = whole, 1 / 2 = broken)
    char _19a[2];
    float m_effectTimer;                        // 0x19C
    u8 m_effectDone;                            // 0x1A0
    char _1a1[3];
    gfModelAnimation* m_modelAnim;              // 0x1A4
    u8 m_animState;                             // 0x1A8
    u8 m_motion;                                // 0x1A9
    char _1aa[2];
    float unk1AC;                               // 0x1AC
    u8 m_motionSet;                             // 0x1B0
    u8 m_hasYakumono;                           // 0x1B1
    char _1b2[2];
    soCollisionHitData* m_hitData;              // 0x1B4
    soCollisionHitData::Simple* m_hitSimple;    // 0x1B8
    soSet<soCollisionHitData::Simple>* m_hitSet; // 0x1BC
    ykDataGroup* m_dataGroup;                   // 0x1C0
    ykData* m_data;                             // 0x1C4
    snd3DGenerator m_snd;                       // 0x1C8
    u8 m_event;                                 // 0x1D0 1 = the stage is run by the event mode


public:
    grPalutenaAshiba(const char* taskName);
    virtual ~grPalutenaAshiba();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void updateYakumono(float deltaFrame);
    virtual void updatePreBuild(float deltaFrame);
    virtual void updateToBuild(float deltaFrame);
    virtual void updateShake(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual u8 getBreakLevel();
    virtual void requestBuild();
    virtual void requestBreak();
    virtual void getPos_Shake(Vec3f* out);
    virtual void setType(u8 type) { m_type = type; }
    virtual void setLevel(u8 level) { m_level = level; }
    virtual void setHPWork(float* hpWork) { m_hpWork = hpWork; }
    virtual void setResCommon(nw4r::g3d::ResFile* resFile);
    virtual void setMotionCommon(u32 animId, bool loop, bool force, float* frameCount);
    static grPalutenaAshiba* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPalutenaAshiba) == 0x1D4, "Class is wrong size!");

// The platform with a landing dip: when a fighter lands on it, it is pushed down and springs back. The two stages of
// breaking of this platform follow its movement (the translation is published for the break grounds).
class grPalutenaAshibaD01 : public grPalutenaAshiba {
    u8 unk1D1;             // 0x1D1
    u8 m_landState;        // 0x1D2 0 = rests, 1 = pushed down, 2 = springs back
    char _1d3;
    float m_landTimer;     // 0x1D4
    float m_landTime;      // 0x1D8
    Vec3f m_translate;     // 0x1DC
    Vec3f m_landing;       // 0x1E8 how far the platform is pushed down by a landing

public:
    grPalutenaAshibaD01(const char* taskName);
    virtual ~grPalutenaAshibaD01();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateLanding(float deltaFrame);
    virtual void getTranslate_Delta(Vec3f* out);
    virtual void setTranslate_Delta(float x, float y, float z);
    virtual void setLanding_Delta(Vec3f* landing) { m_landing = *landing; }
    virtual void getLanding_Delta(Vec3f* out);
    static grPalutenaAshibaD01* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPalutenaAshibaD01) == 0x1F4, "Class is wrong size!");

// The gimmick that moves along a line between two points (the gimmick data holds the points, the speed and the kind of
// the movement). Sora_melee's own class, only used by this stage so far.
class grGimmickMovement : public grGimmick {
protected:
    grGimmickMovementData* m_data; // 0x14C the gimmick data (set at startup)
    u8 m_state;     // 0x150 0 = rests, 2 = moves with a speed, 3 = moves for a time, 4 = arrived
    char _151[3];
    float m_timer;  // 0x154
    float m_dist;   // 0x158 how far the gimmick moved

public:
    grGimmickMovement(const char* taskName);
    virtual ~grGimmickMovement();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void updateMove(float deltaFrame);
    virtual void getPosNode(Vec3f* out, u32 mdlIndex, u32 nodeIndex);
    virtual void startMove();
};
static_assert(sizeof(grGimmickMovement) == 0x15C, "Class is wrong size!");

// The platform that flies across the stage (it moves like grGimmickMovement says, between the edges of the camera).
class grPalutenaAshibaE01 : public grGimmickMovement {
    int m_dir;                      // 0x15C 0 = flies to the right, 1 = to the left
    u8 m_landState;                 // 0x160
    char _161[3];
    float m_landTimer;              // 0x164
    float m_landTime;               // 0x168
    gfModelAnimation* m_modelAnim;  // 0x16C
    u8 m_animState;                 // 0x170
    u8 m_motion;                    // 0x171
    char _172[2];
    float unk174;                   // 0x174
    u8 m_motionSet;                 // 0x178
    char _179[3];

public:
    grPalutenaAshibaE01(const char* taskName);
    virtual ~grPalutenaAshibaE01();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void updateMove(float deltaFrame);
    virtual void updateLanding(float deltaFrame);
    virtual void setResCommon(nw4r::g3d::ResFile* resFile);
    virtual void setMotionCommon(u32 animId, bool loop, bool force, float* frameCount);
    static grPalutenaAshibaE01* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPalutenaAshibaE01) == 0x17C, "Class is wrong size!");

// The cloud: a platform that moves around the temple. It holds the positions of the ends of its way (from the stage).
class grPalutenaAshibaKumo : public grPalutena {
protected:
    Vec3f* m_posGimmick; // 0x158 the positions of the clouds' ends (from the stage)

public:
    grPalutenaAshibaKumo(const char* taskName);
    virtual ~grPalutenaAshibaKumo();
    virtual void setPosGimmickWork(Vec3f* posGimmick) { m_posGimmick = posGimmick; }
    static grPalutenaAshibaKumo* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPalutenaAshibaKumo) == 0x15C, "Class is wrong size!");

// The cloud on a chain (its position follows the chain's end).
class grPalutenaAshibaKumoC : public grPalutenaAshibaKumo {
public:
    grPalutenaAshibaKumoC(const char* taskName);
    virtual ~grPalutenaAshibaKumoC();
    virtual void processAnim();
    static grPalutenaAshibaKumoC* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPalutenaAshibaKumoC) == 0x15C, "Class is wrong size!");

// The cloud that flies in from the side, drops towards the temple and lets a fighter land on it.
class grPalutenaAshibaKumoD : public grPalutenaAshibaKumo {
    u8 m_phase;             // 0x15C 0 - 5: waits, starts, flies, drops, leaves
    u8 m_landState;         // 0x15D
    char _15e[2];
    float m_time;           // 0x160
    float m_time2;          // 0x164
    float m_landTime;       // 0x168
    u8 m_landed;            // 0x16C
    u8 m_fromLeft;          // 0x16D
    u8 unk16E;              // 0x16E
    u8 m_flyOut;            // 0x16F
    u8 m_unk170;            // 0x170
    char _171[3];
    float unk174;           // 0x174
    float unk178;           // 0x178
    float unk17C;           // 0x17C
    float m_flyTime;        // 0x180
    float m_flyRate;        // 0x184
    float unk188;           // 0x188
    float m_scale;          // 0x18C
    float m_scaleMul;       // 0x190
    float m_speed;          // 0x194
    float m_angle;          // 0x198
    u32 m_node;             // 0x19C
    float m_landTimer;      // 0x1A0
    Vec3f m_translate;      // 0x1A4
    Vec3f m_landing;        // 0x1B0
    Vec3f unk1BC;           // 0x1BC
    u8 m_ashibaLevel;       // 0x1C8
    char _1c9[3];
    snd3DGenerator m_snd;   // 0x1CC

public:
    grPalutenaAshibaKumoD(const char* taskName);
    virtual ~grPalutenaAshibaKumoD();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual bool setNode();
    virtual void updateMove(float deltaFrame);
    virtual void updateLanding(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void getTranslate_Delta(Vec3f* out);
    virtual void setLanding_Delta(float x, float y, float z) {
        m_landing.m_x = x;
        m_landing.m_y = y;
        m_landing.m_z = z;
    }
    virtual void setAshibaLevel(u8 level) { m_ashibaLevel = level; }
    static grPalutenaAshibaKumoD* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPalutenaAshibaKumoD) == 0x1D4, "Class is wrong size!");

// The chain between the clouds: 23 links that hang from two points and sag; the model follows them.
class grPalutenaChain : public grPalutena {
    u8 m_chainState;        // 0x158 0 = the links are laid out, 1 = they sag
    char _159[3];
    Vec3f* m_pos;           // 0x15C the position of every link
    float* m_rot;           // 0x160 the angle of every link
    Vec3f* m_posGimmick;    // 0x164 the two points the chain hangs from (from the stage)
    u32* m_node;            // 0x168 the node of every link

public:
    grPalutenaChain(const char* taskName);
    virtual ~grPalutenaChain();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updatePos(float deltaFrame);
    virtual void updateChain(float deltaFrame, float distance, u32 from, u32 to);
    virtual void updateCallBack(float deltaFrame);
    virtual void setPosGimmickWork(Vec3f* posGimmick) { m_posGimmick = posGimmick; }
    static grPalutenaChain* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPalutenaChain) == 0x16C, "Class is wrong size!");

// The model of a platform that falls apart (an animation plays when the platform breaks).
class grPalutenaAshibaBreak : public grPalutena {
    Vec3f m_translate;  // 0x158 where the model is (it follows the platform)
    u8 m_motion;        // 0x164
    char _165[3];
    float m_timer;      // 0x168

public:
    grPalutenaAshibaBreak(const char* taskName);
    virtual ~grPalutenaAshibaBreak();
    virtual void update(float deltaFrame);
    virtual void updateBreak(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void getTranslate_Delta(Vec3f* out);
    virtual void setTranslate_Delta(float x, float y, float z) {
        m_translate.m_x = x;
        m_translate.m_y = y;
        m_translate.m_z = z;
    }
    virtual void requestBuild();
    virtual void requestBreak();
    static grPalutenaAshibaBreak* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPalutenaAshibaBreak) == 0x16C, "Class is wrong size!");
