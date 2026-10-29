#pragma once
#include <StaticAssert.h>
#include <ef/ef_effect.h>
#include <gf/gf_callback.h>
#include <gf/gf_task.h>
#include <nw4r/ef.h>
#include <nw4r/ut/ut_LinkList.h>
#include <types.h>

class efManager : public gfTask {
public:
    // HYPOTHESIS: one member object, since its fields are set before m_effectLists is constructed.
    struct Unk40 {
        bool m_isJustGameFrame : 1; // 0x40
        bool unk40_b6 : 1;
        bool unk40_b5 : 1;
        bool unk40_b4 : 1;
        char _41[0x44 - 0x41];
        u32 unk44;                              // 0x44
        u32 unk48;                              // 0x48
        float m_gameFrame;                      // 0x4C
        float m_gameFrameDelta;                 // 0x50
        float unk54;                            // 0x54
        char _58[0x5C - 0x58];
        nw4r::ef::EffectSystem* m_effectSystem; // 0x5C
        nw4r::ef::MemoryManager* m_memoryManager; // 0x60
        void* unk64;                            // 0x64
        void* unk68;                            // 0x68
        void* unk6C;                            // 0x6C
        void* unk70;                            // 0x70
        void* unk74;                            // 0x74
        void* unk78;                            // 0x78
        void* unk7C;                            // 0x7C
        void* unk80;                            // 0x80
        nw4r::math::MTX34 unk84;                // 0x84
        nw4r::math::MTX34 unkB4;                // 0xB4
        u8 unkE4;                               // 0xE4
        u32 unkE8;                              // 0xE8
        u8 unkEC;                               // 0xEC
        u32 unkF0;                              // 0xF0
        float unkF4;                            // 0xF4
        float unkF8;                            // 0xF8
        float unkFC;                            // 0xFC
        float unk100;                           // 0x100
        char _104[0x108 - 0x104];

        Unk40() {
            m_isJustGameFrame = false;
            unk40_b6 = true;
            unk40_b4 = false;
            unk44 = 0;
            unk54 = -1.0f;
            PSMTXIdentity(unk84);
            PSMTXIdentity(unkB4);
            unkE4 = 0;
            unkE8 = 0;
            unkEC = 1;
            unkF0 = 0;
            unkF4 = 0.0f;
            unkF8 = 1.0f;
            unkFC = 0.0f;
            unk100 = 1.0f;
        }
    } unk40; // 0x40
    nw4r::ut::LinkList<efEffect, 0> m_effectLists[3]; // 0x108
    void* unk12C;                                     // 0x12C
    void* unk130;                                     // 0x130
    void* unk134;                                     // 0x134
    s32 unk138;                                       // 0x138

    efManager();
    virtual ~efManager();
    virtual void processBegin();
    virtual void processEffect();
    virtual void renderPre();
    virtual void processDebug();
    void render();

    static void create();
    void addEffect(efEffect* effect);
    void removeEffect(efEffect* effect);

    static efManager* getInstance();
};
static_assert(sizeof(efManager) == 0x13C, "efManager size");

// HYPOTHESIS: the gfApplication hook whose methods are 0x8005C5B0/0x8005C5F0 (map: efPostRenderCallback).
class efPostRenderCallback : public gfCallBack {
public:
    virtual void userProc();
    virtual ~efPostRenderCallback() { }
};

extern efPostRenderCallback lbl_805A0108;

extern efManager* lbl_805A0110;

inline efManager* efManager::getInstance() {
    if (lbl_805A0110 == NULL) {
        create();
    }
    return lbl_805A0110;
}
