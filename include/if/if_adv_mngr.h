#pragma once
// Local BrawlHeaders shadow: the Subspace HUD manager layout and the recovered create, resource, model, melee,
// stock, corps and menu calls (names from the symbol map). Existing members are unchanged.

#include <StaticAssert.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

class gfArchive;
class ifAdvTask;
class IfAdvGauge;
class IfAdvPause;
class IfAdvRadarData;
class IfCenter;
class IfPlayer;
namespace nw4r { namespace g3d { class ScnChoice; class ScnGroup; } }

// HYPOTHESIS: an owning pointer member; its inline destructor deletes the radar data.
struct IfAdvRadarDataOwner {
    IfAdvRadarData* m_ptr;

    IfAdvRadarDataOwner() : m_ptr(NULL) { }
    ~IfAdvRadarDataOwner();
};

class IfAdvMngr {
public:
    s32 main();
    void renderPre();

    // HYPOTHESIS: int parameters (the map has no parameter types; both are passed on to IfAdvPause::create).
    IfAdvMngr(int arg1, int arg2);
    ~IfAdvMngr();
    static void create(int arg1, int arg2);
    static void destroy();
    void loadResource(gfArchive* archive);
    void freeResource();
    bool isLoadFinish();
    void createModel();
    void destroyModel();
    void startMelee();
    void meleeMain();
    u32 getState();
    // HYPOTHESIS: int parameter.
    void setNumPlayer(int numPlayer);
    // HYPOTHESIS: int parameter, passed on to IfAdvZako::erasesZako.
    void erasesZako(int index);
    // The boss slot (0 or 1), passed on to IfAdvBoss::eraseBoss.
    void eraseBoss(int index);
    // HYPOTHESIS: int and bool parameters.
    void setBossDamageDisp(int index);
    void setBossDeathDisp(int index);
    void setBossMoveDispFlag(bool flag);
    bool addStockSmb();
    bool addCorps();
    bool removeCorps();
    // HYPOTHESIS: bool parameters (callers pass a keep-manager flag and a comparison result).
    void startPauseMenu(bool arg1, bool arg2);
    void exitPauseMenu();
    bool isEndPauseMenu();
    void startSavePointMenu();
    void exitSavePointMenu();
    void setCameraResAnm();

    ifAdvTask* m_task;                  // 0x00
    u8 unk4_80 : 1;                     // 0x04
    u8 : 7;
    u8 unk5[3];
    nw4r::g3d::ScnChoice* m_choice;     // 0x08
    s32 unkC;                           // 0x0C
    gfArchive* m_archive;               // 0x10
    nw4r::g3d::ResFile unk14;           // 0x14
    nw4r::g3d::ResFile unk18;           // 0x18
    nw4r::g3d::ResFile unk1C;           // 0x1C
    nw4r::g3d::ResFile unk20;           // 0x20
    nw4r::g3d::ResFile unk24;           // 0x24
    nw4r::g3d::ResFile unk28;           // 0x28
    nw4r::g3d::ResFile unk2C;           // 0x2C
    nw4r::g3d::ResFile unk30;           // 0x30
    nw4r::g3d::ResFile unk34;           // 0x34
    u32 unk38;                          // 0x38
    u32 unk3C;                          // 0x3C
    void* unk40;                        // 0x40
    nw4r::g3d::ScnGroup* m_group;       // 0x44
    u8 unk48_80 : 1;                    // 0x48
    u8 unk48_40 : 1;
    u8 unk48_20 : 1;
    u8 unk48_10 : 1;
    u8 : 4;
    u8 unk49[3];
    s32 unk4C;                          // 0x4C
    u32 unk50;                          // 0x50
    u8 unk54;                           // 0x54
    u8 unk55;                           // 0x55
    u8 unk56;                           // 0x56
    u8 unk57;                           // 0x57
    void* unk58;                        // 0x58
    void* unk5C;                        // 0x5C
    void* m_zako;                       // 0x60 HYPOTHESIS: IfAdvZako (map names of the callees)
    u8 unk64[2];                        // 0x64
    u8 unk66[2];                        // 0x66
    u8 unk68[2];                        // 0x68
    u8 unk6A[2];                        // 0x6A
    void* m_boss;                       // 0x6C HYPOTHESIS: IfAdvBoss (map names of the callees)
    u8 unk70;                           // 0x70
    u8 unk71[3];
    IfAdvPause* m_pause;                // 0x74
    IfAdvRadarDataOwner m_radarData;    // 0x78
    u8 m_stock[99];                     // 0x7C
    u8 unkDF;
    s32 m_stockNum;                     // 0xE0
    s32 m_stockIndex;                   // 0xE4
    IfPlayer* m_players[7];             // 0xE8
    IfAdvGauge* m_gauge;                // 0x104
    IfCenter* m_center;                 // 0x108
    s32 m_corps;                        // 0x10C
    u8 unk110;                          // 0x110
    u8 m_isPause;                       // 0x111
    u8 unk112[2];
    s32 m_numPlayer;                    // 0x114
    u8 unk118;                          // 0x118
    u8 unk119[3];
};
static_assert(sizeof(IfAdvMngr) == 0x11C, "Class is the wrong size!");

extern IfAdvMngr* g_IfAdvMngr;
