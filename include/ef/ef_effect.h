#pragma once
#include <StaticAssert.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/ut/ut_LinkList.h>
#include <types.h>

namespace nw4r {
namespace g3d {
class ScnObj;
}
} // namespace nw4r

class efEffect {
public:
    nw4r::ut::LinkListNode m_link; // 0x00
    const char* m_name;            // 0x08
    s32 m_state : 8;               // 0x0C
    bool m_pauseRequest : 1;       // 0x0D
    bool unkD_b6 : 1;
    bool unkD_b5 : 1;
    u8 m_group;                    // 0x0E
    u8 m_slowDivisor;              // 0x0F
    float m_frame;                 // 0x10
    float m_nextFrame;             // 0x14
    float m_rate;                  // 0x18
    nw4r::g3d::ScnObj* m_parent;   // 0x1C
    u16 unk20;                     // 0x20
    u16 m_nodeIndex;               // 0x22
    Vec3f m_trans;                 // 0x24
    Vec3f m_rot;                   // 0x30
    Vec3f m_scale;                 // 0x3C

    efEffect(const char* name, u8 group);
    virtual ~efEffect();
    virtual void update(float frames) = 0;
    virtual void updateRootMatrix() = 0;
    virtual void processEffect();
    virtual void render();
    virtual void endCallback();

    s32 getState() const { return m_state; }
    void pause();
    void end();
    void endForce();
    void getWorldMatrix(Matrix* out);
    void getParentWorldMatrix(Matrix* out);
};
static_assert(sizeof(efEffect) == 0x4C, "efEffect size");

// HYPOTHESIS: the frame step shared by every effect, written by efManager::processBegin.
extern float lbl_805A0100;
