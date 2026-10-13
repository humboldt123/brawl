#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <mt/mt_vector.h>
#include <nw4r/ut/ut_LinkList.h>
#include <snd/snd_3d_generator.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st_dxbigblue/gr_dxbigblue_car.h>
#include <st_dxbigblue/gr_dxbigblue_fly.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::DxBigBlue, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::DxBigBlue, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::DxBigBlue, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// Where a course of the F-Zero track starts and ends (the track of a course is drawn between them).
struct stDxBigBlueCourseTbl {
    Vec3f m_start;  // 0x00
    Vec3f m_end;    // 0x0C
};

// HYPOTHESIS: the names and roles of the members are read off how the stage uses them (the original names none of them).
// Big Blue: the fighters fight on platforms that fly above the F-Zero track. The track scrolls by: the stage keeps the
// order of its courses (a linked list of the course ids) and adds a new course at the end whenever there is room. The cars
// drive on the track and the platforms (and Captain Falcon's Blue Falcon, the middle platform) fly in the foreground.
class stDxBigBlue : public stMelee {
public:
    // A car engine sound: the handle of the sound that plays and the car (its place) the sound follows.
    struct STRUCT_SE_CAR {
        snd3DGenerator m_sound; // 0x00
        s32 m_seHandle;         // 0x08 -1 = no sound is playing
        u8 m_carPlace;          // 0x0C
    };

    // HYPOTHESIS: the name of the node of the list is unknown (the symbols only tell that it has 9 characters)
    struct SListNode : public nw4r::ut::LinkListNode {
        u8 m_index;
    };

private:
    u8 m_bgState;                           // 0x1D8 0 = not started, 1 = running
    float m_bgTimer;                        // 0x1DC
    Vec3f m_posLimit[2];                    // 0x1E0 the limits of the camera (left/bottom and right/top)
    u8 m_loopEnable;                        // 0x1F8 1 = a loop course may come
    Vec3f m_posBg;                          // 0x1FC where the background is
    Vec3f m_rotBg;                          // 0x208 (z = the angle of the loop course)
    float unk214;
    u8 unk218;
    Vec3f m_posBgPrev;                      // 0x21C where the background was
    u8 m_courseState;                       // 0x228 0 = starts, 1 = waits, 2 = off, 3 = runs
    float m_courseTimer;                    // 0x22C
    stDxBigBlueCourseTbl m_course[12];      // 0x230 where every course starts and ends
    u8 m_courseIdTbl[12];                   // 0x350 the course after which each course came (0xC = none)
    u8 m_courseIdTop;                       // 0x35C the course at the beginning of the list
    char _35D[3];
    nw4r::ut::LinkList<SListNode, 0> m_list; // 0x360 the courses on the track (the first one is the oldest)
    u8 m_carState;                          // 0x36C 0 = waits for the fighters, 1/3 = running
    char _36D[3];
    float m_carTimer;                       // 0x370 frames until the next car joins
    stDxBigBlueCarData m_carData[30];       // 0x374
    u8 m_carTbl[30];                        // 0xA7C the place of the cars in the line (shuffled)
    u8 m_carTop;                            // 0xA9A the first car
    u8 m_carBottom;                         // 0xA9B the last car
    u8 m_carNum;                            // 0xA9C how many cars are on the track
    u8 m_carDir;                            // 0xA9D 1 = cars come from the right
    char _A9E[2];
    STRUCT_SE_CAR m_seCar[5];               // 0xAA0
    u8 m_flyReady;                          // 0xAF0 1 = the platforms may begin
    u8 m_flyState;                          // 0xAF1 0 = waits, 2 = waits for a platform
    char _AF2[2];
    float m_flyTimer;                       // 0xAF4
    s32 m_flyAshibaNum;                     // 0xAF8 the ashiba (platforms) that may still come
    s32 m_flyTyukeiNum;                     // 0xAFC the Tyukeisha (relay) that may still come
    stDxBigBlueFlyData m_flyData[6];        // 0xB00 the three platforms, the trainer's, the Falcon and the relay
    u8 m_eventEnd;                          // 0xC38 1 = the event match goes on
    char _C39[3];

public:
    stDxBigBlue();
    static stDxBigBlue* create();

    virtual ~stDxBigBlue();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjAshiba(int index);
    virtual void createObjAshibaTrainer(int index);
    virtual void createObjFalcon(int index);
    virtual void createObjTyukei(int index);
    virtual void createObjCourse(int index);
    virtual void createObjCar();
    virtual void createObjCar1(int index);
    virtual void updateLimit();
    virtual void updateCourse(float deltaFrame);
    virtual void updateCourseClear(float deltaFrame);
    virtual void updateCourseAdd(float deltaFrame);
    virtual void updateCourseLoop(float deltaFrame);
    virtual void updateBg(float deltaFrame);
    virtual void updateCar(float deltaFrame);
    virtual void updateCarPos(float deltaFrame);
    virtual void updateCarSE(float deltaFrame);
    virtual void updateCarClear(float deltaFrame);
    virtual void updateCarAdd(float deltaFrame);
    virtual void updateCarAddInit(float deltaFrame);
    virtual void updateFly(float deltaFrame);
    virtual void updateFlyPos(float deltaFrame);
    virtual void updateAshibaPos(float deltaFrame, int index);
    virtual void updateFalconPos(float deltaFrame);
    virtual void updateTyukeiPos(float deltaFrame);
    virtual bool carRayCheck(Vec3f* hitPos, Vec3f* normal, u8 index);
    virtual bool isCarEnableOut(u32 place);
    virtual bool isCar(float* pos);
    virtual bool isFly(float* height, float* pos, bool below);
    virtual bool isFlyFalcon(float* height, float* pos, bool below);
    virtual void addCarSE(u8 place);
    virtual void addList(u8 index, nw4r::ut::LinkList<SListNode, 0>* list);
    virtual void addListTop(u8 index, nw4r::ut::LinkList<SListNode, 0>* list);
    virtual void clearList(u8 index, nw4r::ut::LinkList<SListNode, 0>* list);
    virtual void clearListAll(nw4r::ut::LinkList<SListNode, 0>* list);
    virtual SListNode* getList(u8 index, nw4r::ut::LinkList<SListNode, 0>* list);
    virtual SListNode* getListTop(nw4r::ut::LinkList<SListNode, 0>* list);
    virtual SListNode* getListEnd(nw4r::ut::LinkList<SListNode, 0>* list);

    bool isEventEnd();

    static stClassInfoImpl<Stages::DxBigBlue, stDxBigBlue> bss_loc_14;
};
static_assert(sizeof(stDxBigBlue) == 0xC3C, "Class is wrong size!");
