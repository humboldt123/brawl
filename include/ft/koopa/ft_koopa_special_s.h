#pragma once
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <so/catch/so_catch_module.h>
#include <so/link/so_link_event_presenter.h>
#include <so/so_module_accesser.h>
#include <types.h>

// HYPOTHESIS: Bowser's Flying Slam (Special S): he grabs the opponent on the ground (status 0x11e = start, 0x11f squat, 0x120 jump,
// 0x121 fall, 0x122 landing), rises with the victim, and slams. The statuses share the link event that releases the victim.
struct ftKoopaLinkEventFlyingSlam : soLinkEventArgs {
    int unk8;
    int unkC;
    int unk10;
    int unk14;
    float unk18;
    bool unk1C;
    ftKoopaLinkEventFlyingSlam(int kind, int arg, bool flag);
};

// HYPOTHESIS: the slots of the catch module after catchCut (0x30..0x3C unknown); the exits release the victim with 0x38.
class soCatchModuleKoopaView : public soCatchModule {
public:
    virtual void unk30();
    virtual void unk34();
    virtual void unk38(int);
    virtual bool unk3C();
};

static inline soCatchModuleKoopaView& getCatchView(soModuleAccesser* moduleAccesser) {
    return *static_cast<soCatchModuleKoopaView*>(moduleAccesser->m_enumerationStart->m_catchModule);
}

// HYPOTHESIS: the Special S group of the extend parameters of Bowser (floats).
struct ftKoopaSpecialSParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30;
};

static inline ftKoopaSpecialSParam* getKoopaSpecialSParam(ftKind kind, int index) {
    return reinterpret_cast<ftKoopaSpecialSParam*>(g_ftCommonDataAccesser.getData(kind)->extendParam[index]);
}

