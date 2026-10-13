#pragma once

#include <st_dxbigblue/gr_dxbigblue_course.h>

// The part of the F-Zero course with the loop: it works like a course, but turns on the way (the angle in the rotation work
// goes from 0 to 360) and has two collisions that are on only for part of the turn (the way up and the way down).
class grDxBigBlueCourseLoop : public grDxBigBlueCourse {
protected:
    Vec3f m_len;                    // 0x188 the vector from the start to the end of the course
    float unk194;                   // 0x194 how far the loop has turned (0 to 1)
    float unk198;                   // 0x198
    float unk19C;                   // 0x19C
    float unk1A0;                   // 0x1A0
    u8 unk1A4;                      // 0x1A4 1 = the loop is turned all the way
    char _1A5[3];
    grCollisionJoint* m_jointStart; // 0x1A8 collision of the way up ("LoopStartN")
    grCollisionJoint* m_jointEnd;   // 0x1AC collision of the way down ("LoopEndN")
    u32 m_nodeCenter;               // 0x1B0 node "LoopCenterN"

public:
    inline grDxBigBlueCourseLoop(const char* taskName) : grDxBigBlueCourse(taskName) {
        m_len.m_x = 0.0f;
        m_len.m_y = 0.0f;
        m_len.m_z = 0.0f;
        unk194 = 0.0f;
        unk198 = 0.0f;
        unk19C = 0.0f;
        unk1A0 = 0.0f;
        unk1A4 = 0;
        m_jointStart = NULL;
        m_jointEnd = NULL;
        m_nodeCenter = 0;
    }
    static grDxBigBlueCourseLoop* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grDxBigBlueCourseLoop();
    virtual void processAnim();
    virtual bool setNode();
    virtual void updateJoint(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
};
static_assert(sizeof(grDxBigBlueCourseLoop) == 0x1B4, "grDxBigBlueCourseLoop layout");
