#pragma once

// The input that stage objects without a controller report (every query answers "nothing pressed").

#include <ip/input.h>

class IpNull : public Input {
public:
    IpNull() : Input(true) { }
    virtual ~IpNull() { }
    virtual void update() { }
    virtual Vec2f getStickMain() const { return Vec2f(0.0f, 0.0f); }
    virtual Vec2f getStickSub() const { return Vec2f(0.0f, 0.0f); }
    virtual ipPadButton getButton() const { return ipPadButton(0); }
    virtual ipPadTrigger getTrigger() const {
        ipPadTrigger trigger = { 0, 0 };
        return trigger;
    }
    virtual s32 getContNo() const { return -1; }
    virtual void setRumble(u32, u32, s32, u8) { }
    virtual void stopRumble() { }
    virtual void removeRumble() { }
    virtual void removeRumbleId(s32, s32) { }
    virtual void removeRumbleMask(u32) { }
};
