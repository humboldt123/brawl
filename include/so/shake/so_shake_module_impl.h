#pragma once

// soShakeModuleImpl (sora_melee so_shake_module_impl.o), from its constructor, its RTTI (soShakeModuleSimple at 0,
// soAnimCmdEventObserver at 8) and the term accessors. Not for units that include ft/builder/ft_dol_types.h, which
// declares an opaque soShakeModuleImpl.

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/shake/so_shake_module_simple.h>
#include <so/so_array.h>
#include <types.h>

class acAnimCmd;
class soModuleAccesser;

// The layout soShakeModuleImpl reads (0x1C bytes, see ft/builder/ft_dol_types.h).
class soShakeTerm {
public:
    u8 m_isActive : 1;     // 0x0
    struct {
        const Vec2f* unk0; // HYPOTHESIS: the offset table, indexed by the term's step
        int unk4;          // the number of steps
        u8 unk8;           // HYPOTHESIS: the priority (a request with a lower one does not replace the running shake)
        u8 unk9;           // the shake advances only when this is set
    } const* m_data;       // 0x4
    int m_frame;           // 0x8
    u8 m_step;             // 0xC
    bool m_flag;           // 0xD
    Vec2f m_offset;        // 0x10
    virtual ~soShakeTerm();
};
static_assert(sizeof(soShakeTerm) == 0x1C, "Class is wrong size!");

class soShakeModuleImpl : public soShakeModuleSimple, public soAnimCmdEventObserver {
public:
    soArray<soShakeTerm>* m_terms; // 0x14
    int m_kind;                    // 0x18: the running shake, -1 for none

    soShakeModuleImpl(soModuleAccesser* moduleAccesser, soArray<soShakeTerm>* terms, void* shakeData);
    virtual ~soShakeModuleImpl();
    virtual void activate();
    virtual void req(int kind, int frame, bool flag, const Vec2f* offset);
    virtual void extend(int kind, int frame);
    virtual void stop();
    virtual void stopKind(int kind);
    virtual void update(bool);
    virtual bool isShake();
    virtual bool getOffset(Vec2f* offset);
    virtual bool isObserv(char group);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
};
static_assert(sizeof(soShakeModuleImpl) == 0x1C, "Class is wrong size!");
