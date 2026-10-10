#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gm/gm_lib.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_ladder.h>
#include <gr/gr_gimmick_spring.h>
#include <st/st_trigger_observe.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_data_container.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::PictChat, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::PictChat, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::PictChat, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The parameters of the stage (the stage data file). Nothing is named by the original, so the fields are the offsets.
struct stPictchatData {
    float unk00;    // 0x00 frames until the first picture
    float unk04;    // 0x04 least frames until the next picture (after the first)
    float unk08;    // 0x08 most frames until the next picture (after the first)
    float unk0C;    // 0x0C least frames until the next picture
    float unk10;    // 0x10 most frames until the next picture
};

// Pictochat: a picture is drawn on the stage, one after the other, and each picture is a gimmick (a platform, a spring, a
// ladder, a bomb, a fire ...). The stage keeps a shuffled list of the 27 pictures and shows the next one when the wait is over;
// the grounds read which picture is shown from the work pointers.
class stPictchat : public stMelee {
    u8 m_state;                         // 0x1D8 the step of the wait (0 = choose, 1 = wait, 2 = the picture is drawn, 3 = it is shown)
    char _1d9[3];
    float m_timer;                      // 0x1DC frames until the next step
    u8 m_first;                         // 0x1E0 no picture was shown yet
    u8 m_pictID;                        // 0x1E1 the picture that is drawn now (0x1D = none, 0x1E = the end)
    u8 m_pictIDPrev;                    // 0x1E2 the picture of the last time
    u8 m_pictID2;                       // 0x1E3
    u8 m_pictCount;                     // 0x1E4 how many pictures were shown (up to 23)
    u8 m_pictList[27];                  // 0x1E5 the shuffled list of the pictures
    u8 m_listIndex;                     // 0x200 where the list is now
    u8 m_attackState[6];                // 0x201 the state of the six attack pictures (5 = off)
    char _207;
    stDataMultiContainer* m_tblPict[27];// 0x208 the tables of the pictures (from the stage data)
    Vec3f m_posGimmick[29];             // 0x274 the places of the gimmicks of the pictures
    grGimmickLadderData* m_hashigoData; // 0x3D0 the data of the two ladders
    Vec3f m_posHashigo[2];              // 0x3D4
    u8 m_hashigoFlag;                   // 0x3EC
    char _3ed[3];
    grGimmickSpringData* m_springData;  // 0x3F0 the data of the two springs
    Vec3f m_posSpring[2];               // 0x3F4
    u8 m_springFlag[2];                 // 0x40C
    u8 m_unk40E;                        // 0x40E
    char _40f;
    Vec3f m_posBomb[2];                 // 0x410
    stTrigger* m_triggerWind;           // 0x428 the area of the wind
    grGimmickWindData* m_windData;      // 0x42C
    s32 m_seHandle;                     // 0x430 the sound of the picture that is drawn (-1 = none)

public:
    stPictchat();
    static stPictchat* create();

    virtual ~stPictchat();
    virtual bool loading();
    virtual void createObj();
    virtual bool isBamperVector() { return true; }
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjSideBar(int index);
    virtual void createObjSideBarLamp(int index);
    virtual void createObjPict(int index);
    virtual void createObjAttack(int index);
    virtual void createObjHashigo();
    virtual void createObjHashigo1(int index);
    virtual void createObjSpring();
    virtual void createObjSpring1(int index);
    virtual void createObjWind();
    virtual void updatePict(float deltaFrame);
    virtual void initStageDataTbl();
    virtual void initPictIDList();

    static stClassInfoImpl<Stages::PictChat, stPictchat> bss_loc_14;
};
static_assert(sizeof(stPictchat) == 0x434, "Class is wrong size!");
