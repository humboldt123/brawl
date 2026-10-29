#pragma once

#include <so/so_null.h>

// The constructor and concrete vtable place soNullable at 0 and this interface's
// vptr at 8, as with soGroundModule. Unused signatures remain unresolved.
class soJostleModule : public soNull, public soNullable {
public:
    virtual ~soJostleModule();
    virtual void unk10(); // activate; signature unresolved
    virtual void setByte47(u8 value); // setStatus
    virtual void setByte46(int value); // setLayer
    virtual void setByte48(bool value); // setFix
    virtual void setByte4B(u8 value); // setIgnoreSpeedX
    virtual void setByte4A(bool value); // sleep
    virtual void unk30(); // update
    virtual float getSpeedX();
    virtual void unk38(); // setRefer
    virtual float getPosZ();
    virtual void unk40(); // setPushSpeedX
    virtual void unk44(); // setPriority
    virtual void unk48(); // setJostleData
    virtual void unk4C(); // setForceGap
};
static_assert(sizeof(soJostleModule) == 0xC, "Jostle module interface extent");
