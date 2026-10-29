#pragma once

// soShakeModule / soShakeModuleSimple (sora_melee so_shake_module_simple.o). The vtable order and the method names are
// the module map's; the parameter types come from soShakeModuleImpl, which overrides every slot.

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/so_array.h>
#include <types.h>

class soModuleAccesser;
class soShakeTerm;

class soShakeModule {
public:
    virtual ~soShakeModule() { }
    virtual void activate() = 0;
    // HYPOTHESIS: the third argument is a flag (soShakeModuleImpl stores it as a byte and tests it against 0).
    virtual void req(int kind, int frame, bool flag, const Vec2f* offset) = 0;
    virtual void extend(int kind, int frame) = 0;
    virtual void stop() = 0;
    virtual void stopKind(int kind) = 0;
    // HYPOTHESIS: the bool argument of the other modules' update (soShakeModuleImpl does not read it).
    virtual void update(bool) = 0;
    virtual bool isShake() = 0;
    virtual bool getOffset(Vec2f* offset) = 0;
};
static_assert(sizeof(soShakeModule) == 4, "Class is wrong size!");

class soShakeModuleSimple : public soShakeModule {
public:
    soModuleAccesser* m_moduleAccesser; // 0x4

    soShakeModuleSimple(soModuleAccesser* moduleAccesser, soArray<soShakeTerm>* terms, void* shakeData);
    virtual ~soShakeModuleSimple();
    virtual void activate();
    virtual void req(int kind, int frame, bool flag, const Vec2f* offset);
    virtual void extend(int kind, int frame);
    virtual void stop();
    virtual void stopKind(int kind);
    virtual void update(bool);
    virtual bool isShake();
    virtual bool getOffset(Vec2f* offset);
};
static_assert(sizeof(soShakeModuleSimple) == 8, "Class is wrong size!");
