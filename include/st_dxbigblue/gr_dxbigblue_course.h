#pragma once

#include <gr/gr_calc_world_callback.h>
#include <mt/mt_vector.h>
#include <st_dxbigblue/gr_dxbigblue.h>

// A piece of the F-Zero course that flies past below the fighters (the stage lays them out in a ring of 12). It comes in
// from the front (the courses are listed as a start and an end position), is solid while it is on screen and goes when
// the camera has left it behind.
class grDxBigBlueCourse : public grDxBigBlue {
protected:
    Vec3f m_pos;                // 0x158 where the course is (it moves with the speed of the race)
    Vec3f* m_posWork;           // 0x164 start and end position of every course (two vectors each, start first)
    float* m_posLimitWork;      // 0x168 limits of the camera (the course is gone when it is left of the first one)
    Vec3f* m_posBgWork;         // 0x16C the position of the background
    Vec3f* m_rotWork;           // 0x170 (the turn of the course: the angle is the last value)
    u8 m_courseID;              // 0x174 which of the courses this is
    char _175[3];
    u8* m_courseIDTopWork;      // 0x178 the course in front
    u8* m_attachWork;           // 0x17C the step of this course in the stage's list (0xC = not shown)
    u32 m_nodeStart;            // 0x180 node "StartN"
    u32 m_nodeEnd;              // 0x184 node "EndN"

public:
    grDxBigBlueCourse(const char* taskName);
    static grDxBigBlueCourse* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grDxBigBlueCourse();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updateJoint(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setPosWork(Vec3f* pos) { m_posWork = pos; }
    virtual void setPosLimitWork(float* limit) { m_posLimitWork = limit; }
    virtual void setPosBgWork(Vec3f* pos) { m_posBgWork = pos; }
    virtual void setRotWork(Vec3f* rot) { m_rotWork = rot; }
    virtual void setCourseID(u8 id) { m_courseID = id; }
    virtual void setCourseIDTopWork(u8* id) { m_courseIDTopWork = id; }
    virtual void setAttachWork(u8* attach) { m_attachWork = attach; }
};
static_assert(sizeof(grDxBigBlueCourse) == 0x188, "grDxBigBlueCourse layout");
