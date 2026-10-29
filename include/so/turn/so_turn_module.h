#pragma once

// Turn-module vtable order verified against soTurnModuleImpl, its reflector caller and the emTurnModuleImpl vtable of
// sora_enemy (the slot names are the module map's, the accessors' bodies are the weak copies in the turn unit there).
struct soTurnData {
    const float* angles;
    unsigned int endFrame;
};

class soTurnModule {
public:
    virtual ~soTurnModule();
    virtual void activate();
    virtual void set(const soTurnData* data, bool changeLr, bool isExtern, float lr);
    virtual void end();
    virtual void update(bool active);
    // HYPOTHESIS: the reflector status asks whether a turn is already running before it starts another one.
    virtual bool isTurn();
    virtual bool isExtern();
    virtual float getRY();
    virtual float getRYLr();
    virtual unsigned int getEndFrame();
    virtual void setOmitIntermediate(bool omit);
};
