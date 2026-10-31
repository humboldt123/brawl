#pragma once

// SHADOW of BrawlHeaders/ip/ip_human.h: the input members are protected, not private, because aiInput (the CPU controller)
// overrides the getters that return them.

#include <StaticAssert.h>
#include <ip/input.h>
#include <mt/mt_vector.h>
#include <types.h>

class IpHuman : public Input {
protected:
    s32 m_contNo;
    ipPadButton m_button;
    Vec2f m_stickMain;
    Vec2f m_stickSub;
    ipPadTrigger m_trigger;
    bool m_rumbleEnabled;

private:
    bool rumbleCheck();
public:
    IpHuman(u32 contNo);
    virtual ~IpHuman();
    virtual void update();
    virtual Vec2f getStickMain() const;
    virtual Vec2f getStickSub() const;
    virtual ipPadButton getButton() const;
    virtual ipPadTrigger getTrigger() const;
    virtual s32 getContNo() const;
    virtual void setRumble(u32, u32, s32, u8);
    virtual void stopRumble();
    virtual void removeRumble();
    virtual void removeRumbleId(s32, s32);
    virtual void removeRumbleMask(u32);

    bool isWinPoseIndicate() const;
    u32 getWinPose() const;
    void setEnableRumble(bool enable);
};
static_assert(sizeof(IpHuman) == 0x2C, "Class is the wrong size!");
