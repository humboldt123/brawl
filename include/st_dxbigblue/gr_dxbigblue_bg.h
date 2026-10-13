#pragma once

#include <st_dxbigblue/gr_dxbigblue.h>

// The sea and the sky of Big Blue: they follow the stage's position and turn around it (the camera of the course).
class grDxBigBlueBg : public grDxBigBlue {
protected:
    float* m_posWork;           // 0x158 the position the background follows (x, y, z)
    float* m_posLimitWork;      // 0x15C the limits of the camera
    float* m_rotWork;           // 0x160 (x, y, angle in degrees)

public:
    grDxBigBlueBg(const char* taskName);
    static grDxBigBlueBg* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grDxBigBlueBg();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setPosWork(float* pos) { m_posWork = pos; }
    virtual void setPosLimitWork(float* limit) { m_posLimitWork = limit; }
    virtual void setRotWork(float* rot) { m_rotWork = rot; }
};
static_assert(sizeof(grDxBigBlueBg) == 0x164, "grDxBigBlueBg layout");
