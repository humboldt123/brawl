#pragma once
#include <gr/gr_yakumono.h>

// Common layout and virtual entries verified against Pirate Ship ground objects.
class grPirates : public grYakumono {
protected:
    u8 m_state;
    u8 unk151[3];
    float m_timer;
    float* m_ctrlFrame;
    void* unk15C;
public:
    grPirates(const char* taskName);
    virtual ~grPirates();
    virtual void setCtrlFrame(void* frame) { m_ctrlFrame = static_cast<float*>(frame); }
    virtual void setEventTotalFrame(void* frame) { unk15C = frame; }
};
static_assert(sizeof(grPirates) == 0x160, "grPirates layout");
