#pragma once

#include <StaticAssert.h>
#include <mu/menu.h>
#include <mu/mu_event.h>
#include <types.h>

// What the scene keeps of the fighters of the event (the event decides them).
// HYPOTHESIS: the names follow how the menu fills them.
struct scEventFighter {
    u8 m_kind;
    u8 m_costume;
};
struct scEventFighters {
    u32 m_unk00;
    u16 m_unk04;
    u8 m_unk06;
    u8 m_count : 3;             // 0x07 how many fighters
    u8 m_unk07 : 5;
    scEventFighter m_fighters[4];
};

// The scene of the event menu. Only what the menu writes is known.
class scSelEvent : public muMenu {
public:
    u8 m_unk350[0x10];
    muEventSetting m_setting;                   // 0x360 the choice of the player
    scEventFighters m_fighters;                 // 0x384
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
