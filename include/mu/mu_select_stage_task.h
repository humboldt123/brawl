#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The stage selection screen (module sora_menu_sel_stage). Its own code is not decompiled yet; the main menu creates it
// for the My Music option, where the player first picks the stage whose music list is shown.
/// Function names come from the sora_menu_sel_stage map.
// HYPOTHESIS: only what muProcOptSong reads is declared; the size (0x616C) is the one create allocates.
class muSelectStageTask : public gfTask {
public:
    // What create is given. The constructor sets the defaults; the members create copies are named by where they go.
    // HYPOTHESIS: the names follow how create and the callers (muProcOptSong, sora_scene) fill the members.
    struct Param {
        int m_mode;                 // 0x00 copied to m_mode (2: the My Music option)
        void* m_data1;              // 0x04 copied to 0x264
        void* m_data2;              // 0x08 copied to 0x268
        void* m_data4;              // 0x0C copied to 0x270
        void* m_data3;              // 0x10 copied to 0x26C
        nw4r::g3d::ScnGroup* m_group; // 0x14 the scene group the screen draws in
        int m_unk18;                // 0x18
        void* m_msgData;            // 0x1C HYPOTHESIS: a message file of the caller
        int m_unk20;                // 0x20
        int m_padNo;                // 0x24 the pad that uses the screen (0xF0: the pad of the system menu)
        int m_unk28;                // 0x28
        u8 m_unk2C;                 // 0x2C
        void* m_unk30;              // 0x30
        void* m_unk34;              // 0x34
        void* m_unk38;              // 0x38
        void* m_unk3C;              // 0x3C
        int m_unk40;                // 0x40
        void* m_unk44;              // 0x44
        int m_unk48;                // 0x48
        u8 m_unk4C;                 // 0x4C

        Param() {
            m_data1 = NULL;
            m_data2 = NULL;
            m_data4 = NULL;
            m_data3 = NULL;
            m_msgData = NULL;
            m_unk20 = -1;
            m_padNo = 0xF0;
            m_unk28 = 0;
            m_unk2C = 1;
            m_unk30 = NULL;
            m_unk34 = NULL;
            m_unk38 = NULL;
            m_unk3C = NULL;
            m_unk40 = 0;
            m_unk44 = NULL;
            m_unk48 = 0;
            m_unk4C = 0;
        }
    };

    int m_mode;                     // 0x40
    u8 m_unk44[0x224 - 0x44];
    int m_step;                     // 0x224 HYPOTHESIS: 1 when the player backed out, 11 once a stage was chosen
    u8 m_unk228[0x254 - 0x228];
    int m_selKind;                  // 0x254 what was chosen: 0 (HYPOTHESIS: random), 1 a stage (m_selStage), 2 the custom stages
    int m_selStage;                 // 0x258
    u8 m_unk25C[0x260 - 0x25C];
    int m_padNo;                    // 0x260
    u8 m_unk264[0x274 - 0x264];
    int m_decision;                 // 0x274 HYPOTHESIS: 1 when a stage was chosen, 2 when the screen has closed
    u8 m_unk278[0x616C - 0x278];

    static muSelectStageTask* create(Param* param);
    void exit();
    void resumeSelect();

    // HYPOTHESIS: an inline helper (muProcOptSong expands it twice); the decision only counts in mode 2 or once the
    // step is 11.
    int getDecision() const {
        if (m_mode == 2) {
            return m_decision;
        }
        if (m_step != 11) {
            return 0;
        }
        return m_decision;
    }
};
static_assert(sizeof(muSelectStageTask) == 0x616C, "Class is wrong size!");
