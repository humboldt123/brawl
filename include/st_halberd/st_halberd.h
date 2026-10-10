#pragma once

#include <GX/GXTypes.h>
#include <StaticAssert.h>
#include <cm/cm_subject.h>
#include <gf/gf_archive.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

// The matrices are plain storage (the constructors set them to the identity themselves, not through Matrix's own).
struct stHalberdMtx {
    float m[3][4];
};

template<typename T>
class stClassInfoImpl<Stages::Halberd, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Halberd, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Halberd, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// HYPOTHESIS: clears the 3 + 3 bits at the top of the word at cmSubject+8 (the mode of the subject).
static inline void halberdDisableSubject(cmSubject* subject) {
    u32* word = reinterpret_cast<u32*>(reinterpret_cast<u8*>(subject) + 8);
    *word = *word & 0x03FFFFFF;
}

// The destructor of cmSubject takes a hidden delete flag in the main binary (this, flag); the stage passes -1 to destroy
// the member in place. The constructor is the real cmSubject(int kind, int flag) from cm/cm_subject.h.
extern "C" void __dt__9cmSubjectFv(cmSubject* subject, int deleteFlag);

// A camera subject that is built with the stage (the camera follows the flying cannon ball / the player target).
class stHalberdSubject : public cmSubject {
public:
    stHalberdSubject() : cmSubject(0, 1) { }
};

// The parameters of the ship (the stage data file): when the shots come, how fast the arm moves and so on. Nothing here is
// named yet; the fields are the offsets in the file.
struct stHalberdData {
    float unk00;
    float unk04;
    float unk08;
    float unk0C;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30;
    float unk34;
    float unk38;
    float unk3C;
    float unk40;
    float unk44;
    float unk48;
    float unk4C;
    float unk50;
    float unk54;
    float unk58;
    float unk5C;
    float unk60;
    float unk64;
    float unk68;
    float unk6C;
    float unk70;
    float unk74;
    float unk78;
    float unk7C;
    float unk80;
    float unk84;
    float unk88;
    float unk8C;
    float unk90;
    float unk94;
    float unk98;
    float unk9C;
    float unkA0;
    float unkA4;
    float unkA8;
    float unkAC;
};
static_assert(sizeof(stHalberdData) == 0xB0, "Class is wrong size!");

// The Halberd: Meta Knight's ship flies over a sea of clouds. The stage runs the story of the ship (state 0 - 6: the dome
// opens, the laser cannons shoot, the arm grabs, the ship leaves and comes back) and hands pointers to its data (matrices,
// positions, state and frame) to the grounds it makes. The cannons, the laser, the targets and the arm are those grounds.
class stHalberd : public stMelee {
    u8 m_state;                        // 0x1D8 the part of the story (0 - 6)
    char _1d9[3];
    float m_frame;                     // 0x1DC the frame of the stage animation
    float m_frameNext;                 // 0x1E0 when the next part of the story starts
    u8 m_motion;                       // 0x1E4 the motion the grounds play (0x15 = none)
    char _1e5[3];
    stHalberdMtx m_mtx[10];                  // 0x1E8 the matrices of the grounds (from the stage animation)
    u8 m_posState;                     // 0x3C8 which of the position sets is loaded (0 - 2)
    char _3c9[3];
    float m_shotTimer;                 // 0x3CC time until the cannon fires
    u8 m_shotKind;                     // 0x3D0 the shot that is fired now (0x15 = none)
    u8 m_shotQueue[3];                 // 0x3D1 the shots to come
    u8 m_shotSide;                     // 0x3D4
    char _3d5[3];
    Vec3f m_shotPos;                   // 0x3D8
    int m_target;                      // 0x3E4 the number of the player the cannon aims at (-1 = none)
    Vec3f m_targetPos;                 // 0x3E8 where the cannon aims
    stHalberdSubject m_subject;        // 0x3F4 the camera subject of the cannon ball
    u8 m_armState;                     // 0x478 the state of the arm
    char _479[3];
    float m_armTimer;                  // 0x47C
    float m_armTimerMax;               // 0x480
    float m_armWait;                   // 0x484
    Vec3f m_armPos;                    // 0x488 the position of the hand
    Vec3f m_armStart;                  // 0x494 where the move of the hand starts
    Vec3f m_armGoal;                   // 0x4A0 where the move of the hand ends
    int m_armUnk4AC;                   // 0x4AC
    float m_armUnk4B0[3];              // 0x4B0
    Vec3f m_armVel;                    // 0x4BC the speed of the hand when it follows its target
    float m_armSpeed;                  // 0x4C8
    Vec3f m_armOffset;                 // 0x4CC the position of the hand relative to the shoulder
    stHalberdMtx m_mtxBone[13];              // 0x4D8 the matrices of the bones of the arm
    stHalberdMtx m_mtxJoint[12];             // 0x748 the matrices of the joints of the arm
    stHalberdMtx m_mtxHand[1];              // 0x988 the matrix of the hand
    u8 m_armMotion;                    // 0x9B8 the motion of the arm grounds (0x15 = none)
    u8 m_armMiss;                      // 0x9B9 how often the hand missed a player
    char _9ba[2];
    float m_armUnk9BC[3];              // 0x9BC
    u8 m_warning;                      // 0x9C8 the motion of the warning (0x15 = none)
    char _9c9[3];

    Matrix* mtx(stHalberdMtx* mtx) { return reinterpret_cast<Matrix*>(mtx); }

public:
    stHalberd();
    static stHalberd* create();

    virtual ~stHalberd();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjDome(int index);
    virtual void createObjEnkei(int index);
    virtual void createObjStage(int index);
    virtual void createObjCannon(int index);
    virtual void createObjLaser(int index);
    virtual void createObjTarget(int index);
    virtual void createObjHero2Hou(int index);
    virtual void createObjHero2Dan(int index);
    virtual void createObjWarning(int index);
    virtual void createObjArm();
    virtual void createObjArmBone(int index);
    virtual void createObjArmJoint(int index);
    virtual void createObjArmHand(int index);
    virtual void updateActive(float deltaFrame);
    virtual void updateArm(float deltaFrame);
    virtual void updateArmPos();
    virtual void updateHeroHero();
    virtual bool isEventEnd(int param1, int* eventState, int* eventDecision);
    virtual GXColor getFinalTechniqColor();

    static stClassInfoImpl<Stages::Halberd, stHalberd> bss_loc_14;
};
static_assert(sizeof(stHalberd) == 0x9CC, "Class is wrong size!");
