#pragma once

#include <so/shake/so_shake_module_impl.h>

class soShakeModuleNull : public soShakeModule {
public:
    virtual ~soShakeModuleNull();
    virtual void activate();
    virtual void req(int kind, int frame, bool flag, const Vec2f* offset);
    virtual void extend(int kind, int frame);
    virtual void stop();
    virtual void stopKind(int kind);
    virtual void update(bool);
    virtual bool isShake();
    virtual bool getOffset(Vec2f* offset);
};
