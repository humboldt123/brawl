#pragma once

#include <StaticAssert.h>
#include <cm/cm_subject.h>
#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_yakumono.h>
#include <math.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_scnobjcallback.h>
#include <nw4r/ut/ut_LinkList.h>
#include <snd/snd_3d_generator.h>
#include <so/collision/so_collision_hit_part.h>
#include <types.h>
#include <yk/yakumono.h>

#include <st_plankton/st_plankton.h>

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);
// the stage that is played (sora_melee .bss)
extern "C" Stage* lbl_27_bss_5668;
// ecMgr::setEffect(EfID, const Vec3f*) (unnamed in the symbols; the code is the same as setEffect(EfID, Vec3f*))
extern "C" u32 fn_8005F800(ecMgr* mgr, EfID effectID, Vec3f* pos);
// Ground::isCollisionStatusOwnerTask (sora_melee): the symbol takes an int pointer (the header declares another type)
extern "C" bool isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(Ground* ground, grCollStatus* collStatus, int* flags);

// MATCH-ONLY: the bytes of the match settings the stages read (their names are unknown): byte 0 holds the game mode in its
// upper six bits, byte 6 the rule in its upper three bits and byte 8 the event.
struct grPlanktonMeleeBytes {
    unsigned char m_mode : 6;
    unsigned char m_rest : 2;
    u8 _pad[5];
    unsigned char m_rule : 3;
    unsigned char m_rest6 : 5;
    u8 _pad7;
    u8 m_event;
};

static inline grPlanktonMeleeBytes* planktonMeleeBytes() {
    return reinterpret_cast<grPlanktonMeleeBytes*>(&g_GameGlobal->m_modeMelee->m_meleeInitData);
}

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct grPlanktonHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

// MATCH-ONLY: layout of soSet<T> (its members are private)
struct grPlanktonSetView {
    void* m_elements;
    u32 m_size;
};

// A fighter that is in the water (the id of its entry) and how many frames it counts down until it may make a ripple again.
class grPlanktonListNode : public nw4r::ut::LinkListNode {
public:
    int m_id;       // 0x08
    float m_count;  // 0x0C
};
static_assert(sizeof(grPlanktonListNode) == 0x10, "Class is wrong size!");
typedef nw4r::ut::LinkList<grPlanktonListNode, 0> grPlanktonList;

// The base ground of the stage.
class grPlankton : public grYakumono {
protected:
    u8 m_state;     // 0x150
    char _151[3];
    float m_timer;  // 0x154

public:
    grPlankton(const char* taskName);
    virtual ~grPlankton();
};
static_assert(sizeof(grPlankton) == 0x158, "Class is wrong size!");

// The water: it follows the camera, and fighters that go in or out of the water make a ripple (hamon).
class grPlanktonBg : public grPlankton {
    Vec3f* m_posLimit;                      // 0x158 left / right limit of the camera (from the stage)
    Vec3f* m_posSuimen;                     // 0x15C the level of the water (from the stage)
    u8* m_stateWater;                       // 0x160 0 = the water is moving (from the stage)
    grPlanktonList m_list;                  // 0x164 the fighters that are in the water

public:
    grPlanktonBg(const char* taskName);
    virtual ~grPlanktonBg();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateArea(float deltaFrame);
    virtual void setPosLimitWork(Vec3f* posLimit) { m_posLimit = posLimit; }
    virtual void setPosSuimenWork(Vec3f* posSuimen) { m_posSuimen = posSuimen; }
    virtual void setStateWater(u8* stateWater) { m_stateWater = stateWater; }
    virtual bool isNodeAlready(grPlanktonList* list, int id);
    virtual void subCountList(float deltaFrame, grPlanktonList* list);
    virtual void clearList(grPlanktonList* list, int id);
    virtual void clearList1(float count, grPlanktonList* list);
    virtual void clearListAll(grPlanktonList* list);
    virtual grPlanktonListNode* getListNode(grPlanktonList* list, int id);
    virtual grPlanktonListNode* getListNode1(grPlanktonList* list, u8 index);
    static grPlanktonBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPlanktonBg) == 0x170, "Class is wrong size!");

// The flower in the middle: it opens (grows from nothing) when its leaves were hit often enough and closes after a while.
class grPlanktonFlower : public grPlankton {
    u8 m_soundState;            // 0x158
    char _159[3];
    float m_soundTimer;         // 0x15C
    float m_effectTimer;        // 0x160
    Vec3f m_scale;              // 0x164
    stPlanktonLeaf* m_leaf;     // 0x170 the six leaves (from the stage)
    cmSubject m_subject;        // 0x174 keeps the camera on the flower while it opens
    snd3DGenerator m_snd;       // 0x1F8

public:
    grPlanktonFlower(const char* taskName);
    virtual ~grPlanktonFlower();
    virtual void update(float deltaFrame);
    virtual void updateScale(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void playSEOpen();
    virtual void setLeafData(stPlanktonLeaf* leaf) { m_leaf = leaf; }
    virtual bool isOpen();
    static grPlanktonFlower* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPlanktonFlower) == 0x200, "Class is wrong size!");

// A leaf platform: hits tilt it (up to a limit), it changes its color for a while and makes a line of the color of the hit.
class grPlanktonAshiba : public grPlankton {
    stPlanktonLeaf* m_leaf;                     // 0x158 (from the stage)
    u32 m_nodeLeaf;                             // 0x15C the node of the leaf
    u32 m_nodeLine;                             // 0x160 the node of the line
    u8 m_colorLevel;                            // 0x164
    char _165[3];
    float m_colorTimer;                         // 0x168
    float m_rotTimer;                           // 0x16C
    float m_lineTimer;                          // 0x170
    float m_landTimer;                          // 0x174
    u8 m_dir;                                   // 0x178
    u8 m_hasYakumono;                           // 0x179
    char _17a[2];
    soCollisionHitData* m_hitData;              // 0x17C
    soCollisionHitData::Simple* m_hitSimple;    // 0x180
    soSet<soCollisionHitData::Simple>* m_hitSet; // 0x184
    ykDataGroup* m_dataGroup;                   // 0x188
    ykData* m_data;                             // 0x18C
    snd3DGenerator m_snd;                       // 0x190
    u8 m_event;                                 // 0x198 1 = the stage is run by the event mode
    char _199[3];

public:
    grPlanktonAshiba(const char* taskName);
    virtual ~grPlanktonAshiba();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateRot(float deltaFrame);
    virtual void updateColor(float deltaFrame);
    virtual void updateLine(float deltaFrame);
    virtual void updateLanding(float deltaFrame);
    virtual void updateHanenbou(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setLeafData(stPlanktonLeaf* leaf) { m_leaf = leaf; }
    virtual bool setNode();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    static grPlanktonAshiba* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPlanktonAshiba) == 0x19C, "Class is wrong size!");

// The leaf on the left side: it is hit and turned like a seesaw.
class grPlanktonAshibaLeft : public grPlankton {
    char m_tgtNodeTop[0x80];                    // 0x158 the name of the node of the top
    u8 unk1D8;                                  // 0x1D8
    u8 unk1D9;                                  // 0x1D9
    char _1da[2];
    float m_rotTimer;                           // 0x1DC
    u8 m_dir;                                   // 0x1E0
    u8 m_hasYakumono;                           // 0x1E1
    char _1e2[2];
    soCollisionHitData* m_hitData;              // 0x1E4
    soCollisionHitData::Simple* m_hitSimple;    // 0x1E8
    soSet<soCollisionHitData::Simple>* m_hitSet; // 0x1EC
    ykDataGroup* m_dataGroup;                   // 0x1F0
    ykData* m_data;                             // 0x1F4
    u32 m_nodeTop;                              // 0x1F8

public:
    grPlanktonAshibaLeft(const char* taskName);
    virtual ~grPlanktonAshibaLeft();
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateRot(float deltaFrame);
    virtual void updateCallback(u32 index);
    virtual bool setNode();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void setTgtNodeTop(const char* name);
    virtual const char* getTgtNodeTop() { return m_tgtNodeTop; }
    virtual void initAshibaData();
    static grPlanktonAshibaLeft* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPlanktonAshibaLeft) == 0x1FC, "Class is wrong size!");

// A leaf that falls from the top and bounces on the leaves and the walls.
class grPlanktonHanenbou : public grPlankton {
    u8 m_flip;                                  // 0x158
    char _159[3];
    Vec3f* m_posLimit;                          // 0x15C hmm: the limits of the camera (from the stage)
    stPlanktonLeaf* m_leaf;                     // 0x160 the six leaves (from the stage)
    u32 m_bounce;                               // 0x164 how often the leaf bounced
    float m_startY;                             // 0x168
    float m_endY;                               // 0x16C
    stPlanktonHanenbou* m_data;                 // 0x170 (from the stage)
    snd3DGenerator m_snd;                       // 0x174

public:
    grPlanktonHanenbou(const char* taskName);
    virtual ~grPlanktonHanenbou();
    virtual void update(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual bool checkRefrect();
    virtual bool checkLeaf();
    virtual bool checkWall();
    virtual bool chackLineCross(float* a, float* b, float* c, float* d, float* out);
    virtual float getRadian(float x1, float y1, float x2, float y2);
    virtual void getRotPos(float angle, float x, float y, float* out);
    virtual void playSELeaf(float* pos, u8 leafIndex);
    virtual void playSEWall(float* pos);
    virtual void setPosLimitWork(Vec3f* posLimit) { m_posLimit = posLimit; }
    virtual void setLeafData(stPlanktonLeaf* leaf) { m_leaf = leaf; }
    virtual void setHanenbouData(stPlanktonHanenbou* data) { m_data = data; }
    static grPlanktonHanenbou* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPlanktonHanenbou) == 0x17C, "Class is wrong size!");

// Sets the two colors of the material of a ripple.
class grPlanktonHamonScnObjCallBack : public nw4r::g3d::IScnObjCallback {
    GXColor* m_color[2]; // 0x04

public:
    grPlanktonHamonScnObjCallBack() {
        m_color[0] = NULL;
        m_color[1] = NULL;
    }
    virtual ~grPlanktonHamonScnObjCallBack() { }
    virtual void ExecCallback_CALC_MAT(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* object, u32 param, void* info);
    virtual void SetCallBackCondition(nw4r::g3d::ScnObj* object);
    virtual void setColor(u8 index, GXColor* color);
};
static_assert(sizeof(grPlanktonHamonScnObjCallBack) == 0xC, "Class is wrong size!");

// A ripple of the water.
class grPlanktonHamon : public grPlankton {
    stPlanktonHamon* m_hamon;                       // 0x158 (from the stage)
    GXColor m_color1;                               // 0x15C
    GXColor m_color2;                               // 0x160
    grPlanktonHamonScnObjCallBack m_scnObjCallback; // 0x164
    u8 m_motion;                                    // 0x170
    char _171[3];
    float m_wait;                                   // 0x174 the length of the animation

public:
    grPlanktonHamon(const char* taskName);
    virtual ~grPlanktonHamon();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setHamonData(stPlanktonHamon* hamon) { m_hamon = hamon; }
    static grPlanktonHamon* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPlanktonHamon) == 0x178, "Class is wrong size!");
