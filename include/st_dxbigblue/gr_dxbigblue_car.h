#pragma once

#include <mt/mt_vector.h>
#include <st_dxbigblue/gr_dxbigblue.h>

// The record of a car on the course (the stage keeps 30; the ground that shows the car reads and moves it).
struct stDxBigBlueCarData {
    u8 m_state;     // 0x00 4 = comes in, 5 = stays, 6 = goes out of the stay, 7 = leaves, 8/9 = gone, 10 = free
    char _01[3];
    float unk04;    // 0x04 where the car is (x)
    float unk08;    // 0x08 (y)
    float unk0C;    // 0x0C (z)
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;    // 0x24 the angle the car is turned to
    float unk28;
    float unk2C;
    u8 unk30;       // 0x30 1 = the car drives to the right
    char _31[3];
    float unk34;    // 0x34 frames the car is counted as ridden
    u8 unk38;       // 0x38 which way the car is driving (1 = right)
    char _39[3];
};
static_assert(sizeof(stDxBigBlueCarData) == 0x3C, "stDxBigBlueCarData layout");

// A car of the F-Zero course that drives along the bottom of Big Blue. A fighter can land on its roof.
class grDxBigBlueCar : public grDxBigBlue {
protected:
    float* m_posLimitWork;          // 0x158 the limits of the camera
    Vec3f* m_posBgWork;             // 0x15C the position of the background
    float m_speed;                  // 0x160 how fast the car drives (negative to the left)
    u8 unk164;                      // 0x164 1 = a fighter has landed on the car
    char _165[3];
    float unk168;                   // 0x168 how far the car sinks
    u8 m_index;                     // 0x16C which of the 30 places the car has in the line
    char _16D[3];
    u8* m_indexTopWork;             // 0x170 the place of the first car in the line
    u8* m_tblWork;                  // 0x174 the table that says which record belongs to each place
    stDxBigBlueCarData* m_carDataWork; // 0x178 the records of the cars

public:
    grDxBigBlueCar(const char* taskName);
    static grDxBigBlueCar* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grDxBigBlueCar();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void updateActive(float deltaFrame);
    virtual void updateActiveIn(float deltaFrame);
    virtual void updateActiveStay(float deltaFrame);
    virtual void updateActiveOut(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual bool isActive(int index);
    virtual void setPosLimitWork(float* limit) { m_posLimitWork = limit; }
    virtual void setPosBgWork(Vec3f* pos) { m_posBgWork = pos; }
    virtual void setIndex(int index) { m_index = index; }
    virtual void setIndexTopWork(u8* index) { m_indexTopWork = index; }
    virtual void setTblWork(u8* tbl) { m_tblWork = tbl; }
    virtual void setCarDataWork(stDxBigBlueCarData* data) { m_carDataWork = data; }
};
static_assert(sizeof(grDxBigBlueCar) == 0x17C, "grDxBigBlueCar layout");
