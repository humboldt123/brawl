#include <types.h>
#include <sr/sr_common.h>
#include <gf/gf_archive.h>
#include <gf/gf_camera.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_slow_manager.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_model.h>
#include <cm/cm_camera_controller.h>
#include <gm/gm_global.h>
#include <it/it_manager.h>
#include <if/if_figure_loader.h>
#include <if/if_minigame_coin_shooter.h>
#include <snd/snd_system.h>
#include <havok/hkArray.h>
#include <havok/hkWorld.h>
#include <havok/hkWorldCinfo.h>
#include <havok/hkGroupFilter.h>
#include <ph/ph_world.h>
#include <ph/ph_container.h>

extern "C" void fn_802CB484(hkCollisionDispatcher*);
extern "C" void fn_800151FC(gfArchive* archive, const char* path, Heaps::HeapType heap, int offset, int size, u32 id);
extern "C" void fn_8009DEE8();
extern IfCoinShooter* lbl_805A0310;

class csElementInterface {
public:
    virtual ~csElementInterface() { }
};

class csElement : public csElementInterface {
public:
    u8 m_isActive; // 0x04

    csElement();
    csElement(int group);
    virtual ~csElement();
    virtual void beginOperation() = 0;
    virtual void afterOperation() = 0;
    virtual void debugDraw() = 0;
};

class csWorld : public csElement, public phWorld {
public:
    void createPhysicsWorld(const hkWorldCinfo& cinfo);
};

class csArticle : public csElement {
public:
    u32 m_unk08[6];
    u8 m_unk20; // 0x20
};

class csCoinshooter {
public:
    u8 m_unk00[0x40];
    u8 m_unk40; // 0x40
    u8 m_unk41; // 0x41
    u8 m_unk42; // 0x42
    u8 m_unk43[2];
    u8 m_unk45; // 0x45
    u8 m_unk46; // 0x46
    u8 m_unk47[0x54 - 0x47];
    s32 m_unk54; // 0x54
    u8 m_unk58[4];
    u8 m_unk5C; // 0x5C
    s32 m_padNo; // 0x60
    s32 m_unk64; // 0x64
    s32 m_coinCount; // 0x68
    s32 m_unk6C; // 0x6C
    s32 m_unk70; // 0x70
    s32 m_unk74; // 0x74
    s32 m_unk78; // 0x78
    s32 m_unk7C; // 0x7C
    float m_unk80; // 0x80
    float m_unk84; // 0x84
    float m_unk88; // 0x88
    s32 m_unk8C; // 0x8C
    s32 m_unk90; // 0x90
    s32 m_unk94; // 0x94
    s32 m_unk98; // 0x98
    s32 m_unk9C; // 0x9C
    s32 m_unkA0; // 0xA0
    s32 m_unkA4; // 0xA4
    u8 m_unkA8[0xBC - 0xA8];
    s32 m_unkBC; // 0xBC
    s32 m_unkC0; // 0xC0
    float m_unkC4; // 0xC4
    float m_unkC8; // 0xC8
    s32 m_unkCC; // 0xCC
    s32 m_unkD0; // 0xD0
    s32 m_unkD4; // 0xD4
    s32 m_unkD8; // 0xD8
    s32 m_unkDC; // 0xDC
    s32 m_unkE0; // 0xE0
    s32 m_unkE4; // 0xE4
    s32 m_unkE8; // 0xE8
    s32 m_unkEC; // 0xEC
    s32 m_unkF0; // 0xF0
    s32 m_unkF4; // 0xF4
    s32 m_unkF8; // 0xF8
    s32 m_unkFC; // 0xFC
    float m_unk100; // 0x100
    float m_unk104; // 0x104
    u8 m_unk108; // 0x108
    u8 m_unk109; // 0x109
    u8 m_unk10A; // 0x10A
    u8 m_unk10B; // 0x10B
    u8 m_unk10C; // 0x10C
    u8 m_unk10D; // 0x10D
    u8 m_unk10E; // 0x10E
    u8 m_unk10F; // 0x10F
    u8 m_unk110; // 0x110
    u8 m_unk111; // 0x111
    u8 m_unk112[2];
    s32 m_unk114; // 0x114
    s32 m_unk118; // 0x118
    s32 m_unk11C; // 0x11C
    s32 m_unk120; // 0x120
    s32 m_unk124; // 0x124
    s32 m_unk128; // 0x128
    u8 m_unk12C; // 0x12C
    u8 m_unk12D; // 0x12D
    u8 m_unk12E; // 0x12E
    u8 m_unk12F; // 0x12F
    u8 m_unk130; // 0x130
    u8 m_unk131; // 0x131
    u8 m_unk132; // 0x132
    u8 m_unk133; // 0x133
    u8 m_unk134; // 0x134
    u8 m_unk135; // 0x135
    u8 m_unk136; // 0x136
    u8 m_unk137; // 0x137
    u8 m_unk138; // 0x138
    u8 m_unk139; // 0x139
    u8 m_unk13A; // 0x13A
    u8 m_unk13B; // 0x13B
    s32 m_unk13C; // 0x13C
    s32 m_unk140; // 0x140
    s32 m_unk144; // 0x144
    s32 m_unk148; // 0x148
    s32 m_unk14C; // 0x14C
    s32 m_unk150; // 0x150
    s32 m_unk154; // 0x154
    s32 m_unk158; // 0x158
    s32 m_unk15C; // 0x15C
    s32 m_unk160; // 0x160
    s32 m_unk164; // 0x164
    s32 m_unk168; // 0x168
    s32 m_unk16C; // 0x16C
    s32 m_unk170; // 0x170
    s32 m_unk174; // 0x174
    s32 m_unk178; // 0x178
    u8 m_unk17C; // 0x17C
    u8 m_unk17D; // 0x17D
    u8 m_unk17E; // 0x17E
    u8 m_unk17F; // 0x17F
    u8 m_unk180; // 0x180
    u8 m_unk181; // 0x181
    u8 m_unk182; // 0x182
    u8 m_unk183; // 0x183
    u8 m_unk184[8];
    u8 m_unk18C; // 0x18C
    u8 m_unk18D; // 0x18D
    u8 m_unk18E; // 0x18E
    u8 m_unk18F;
    CameraController* m_cameraController; // 0x190
    void* m_quake; // 0x194
    u8 m_unk198[8];
    hkGroupFilter m_groupFilter; // 0x1A0
    u8 m_unk23C[8];
    csArticle* m_paths[5][7]; // 0x244
    csArticle* m_path2D0;     // 0x2D0
    u8 m_unk2D4[0x2EC - 0x2D4];
    nw4r::g3d::ScnMdl* m_bgMdl; // 0x2EC
    u8 m_unk2F0[0x308 - 0x2F0];
    s32 m_sndGroup; // 0x308
    u32 m_unk30C;
    gfArchive* m_archive; // 0x310
    u8 m_unk314[8];
    nw4r::g3d::ResFile m_resFile31C; // 0x31C
    u8 m_unk320[0x334 - 0x320];
    nw4r::g3d::ResFile m_resFile334; // 0x334
    u8 m_unk338[0x37C - 0x338];
    s32 m_unk37C; // 0x37C
    u8 m_unk380[0x390 - 0x380];
    float m_unk390; // 0x390
    float m_unk394; // 0x394
    float m_unk398; // 0x398
    u8 m_unk39C[0x4F4 - 0x39C];
    hkArray<csArticle*> m_destroyList; // 0x4F4

    void removeCoinAll();
    void removeFigureAll();
    void removeEnemyAll();
    void removeMissileAll();
};

extern csCoinshooter* lbl_20_bss_8;

struct csElementEntry {
    csElement* m_element;
    int m_group;
};

class csModuleInterface {
public:
    virtual ~csModuleInterface() { }
};

class csModule : public csModuleInterface {
public:
    hkArray<csElementEntry> m_elements; // 0x04
    csWorld* m_world;                   // 0x10

    csModule();
    virtual ~csModule();
    virtual void initLoadParam();
    virtual void removeWork();
};

struct csCoinParam {
    s32 m_00;
    s32 m_04;
    s32 m_08;
    s32 m_0C;
    float m_10;
    s32 m_14;
    float m_18;
    float m_1C;
    s32 m_20;
    s32 m_24;
    s32 m_28;
    s32 m_2C;
    s32 m_30;
    s32 m_34;
    s32 m_38;
    s32 m_3C;
    s32 m_40;
    float m_44;
    float m_48;
    s32 m_4C;
    s32 m_50;
    s32 m_54;
    s32 m_58;
    s32 m_5C;
    s32 m_60;
    s32 m_64;
    s32 m_68;
    s32 m_6C;
    s32 m_70;
    s32 m_74;
    s32 m_78;
    s32 m_7C;
    s32 m_80;
    s32 m_84;
    s32 m_88;
    float m_8C;
    float m_90;
    s32 m_94;
    s32 m_98;
    s32 m_9C;
    s32 m_A0;
    s32 m_A4;
    s32 m_A8;
    s32 m_AC;
    s32 m_B0;
    s32 m_B4;
    s32 m_B8;
    s32 m_BC;
    s32 m_C0;
    s32 m_C4;
    s32 m_C8;
    s32 m_CC;
    s32 m_D0;
    s32 m_D4;
    s32 m_D8;
    s32 m_DC;
    s32 m_E0;
    s32 m_E4;
    s32 m_E8;
    s32 m_EC;
    s32 m_F0;
    s32 m_F4;
    s32 m_F8;
    s32 m_FC;
    s32 m_100;
    s32 m_104;
    s32 m_108;
    s32 m_10C;
    s32 m_110;
    s32 m_114;
    s32 m_118;
    s32 m_11C;
    s32 m_120;
    s32 m_124;
    s32 m_128;
    s32 m_12C;
    s32 m_130;
    s32 m_134;
    s32 m_138;
    s32 m_13C;
    s32 m_140;
    s32 m_144;
    s32 m_148;
    s32 m_14C;
    s32 m_150;
    s32 m_154;
    s32 m_158;
    s32 m_15C;
    s32 m_160;
    s32 m_164;
    s32 m_168;
    s32 m_16C;
    s32 m_170;
    float m_174;
    float m_178;
    float m_17C;
};

class csSampleModule : public csModule {
public:
    csCoinshooter* m_shooter;     // 0x14
    hkPhysicsSystem* m_physSys18; // 0x18
    hkPhysicsSystem* m_physSys1C; // 0x1C
    phContainer* m_container20;   // 0x20
    phContainer* m_container24;   // 0x24

    csSampleModule();
    virtual ~csSampleModule();
    virtual void initLoadParam();
    virtual void removeWork();
    void setCoinParam(csCoinParam* param);
};

void csSampleModule::setCoinParam(csCoinParam* param) {
    m_shooter->m_unk6C = param->m_00;
    m_shooter->m_unk70 = param->m_04;
    m_shooter->m_unk74 = param->m_08;
    m_shooter->m_unk78 = param->m_0C;
    m_shooter->m_unk80 = param->m_10;
    m_shooter->m_unk54 = param->m_14;
    m_shooter->m_unk84 = param->m_18;
    m_shooter->m_unk88 = param->m_1C;
    m_shooter->m_unk8C = param->m_20;
    m_shooter->m_unk90 = param->m_24;
    m_shooter->m_unk94 = param->m_28;
    m_shooter->m_unk98 = param->m_2C;
    m_shooter->m_unk9C = param->m_30;
    m_shooter->m_unkA0 = param->m_34;
    m_shooter->m_unkA4 = param->m_38;
    m_shooter->m_unkBC = param->m_3C;
    m_shooter->m_unkC0 = param->m_40;
    m_shooter->m_unkC4 = param->m_44;
    m_shooter->m_unkC8 = param->m_48;
    m_shooter->m_unkCC = param->m_4C;
    m_shooter->m_unkD0 = param->m_50;
    m_shooter->m_unk18C = param->m_54;
    m_shooter->m_unk18D = param->m_58;
    m_shooter->m_unk18E = param->m_5C;
    m_shooter->m_unkD4 = param->m_60;
    m_shooter->m_unkD8 = param->m_64;
    m_shooter->m_unkDC = param->m_68;
    m_shooter->m_unkE0 = param->m_6C;
    m_shooter->m_unkE4 = param->m_70;
    m_shooter->m_unkE8 = param->m_74;
    m_shooter->m_unkEC = param->m_78;
    m_shooter->m_unkF0 = param->m_7C;
    m_shooter->m_unkF4 = param->m_80;
    m_shooter->m_unkF8 = param->m_84;
    m_shooter->m_unkFC = param->m_88;
    m_shooter->m_unk100 = param->m_8C;
    m_shooter->m_unk104 = param->m_90;
    m_shooter->m_unk108 = param->m_94;
    m_shooter->m_unk109 = param->m_98;
    m_shooter->m_unk10A = param->m_9C;
    m_shooter->m_unk10B = param->m_A0;
    m_shooter->m_unk10C = param->m_A4;
    m_shooter->m_unk10D = param->m_A8;
    m_shooter->m_unk10E = param->m_AC;
    m_shooter->m_unk10F = param->m_B0;
    m_shooter->m_unk110 = param->m_B4;
    m_shooter->m_unk111 = param->m_B8;
    m_shooter->m_unk114 = param->m_BC;
    m_shooter->m_unk118 = param->m_C0;
    m_shooter->m_unk11C = param->m_C4;
    m_shooter->m_unk120 = param->m_C8;
    m_shooter->m_unk128 = param->m_CC;
    m_shooter->m_unk12C = param->m_D0;
    m_shooter->m_unk12D = param->m_D4;
    m_shooter->m_unk12E = param->m_D8;
    m_shooter->m_unk12F = param->m_DC;
    m_shooter->m_unk130 = param->m_E0;
    m_shooter->m_unk131 = param->m_E4;
    m_shooter->m_unk132 = param->m_E8;
    m_shooter->m_unk133 = param->m_EC;
    m_shooter->m_unk134 = param->m_F0;
    m_shooter->m_unk135 = param->m_F4;
    m_shooter->m_unk136 = param->m_F8;
    m_shooter->m_unk137 = param->m_FC;
    m_shooter->m_unk138 = param->m_100;
    m_shooter->m_unk139 = param->m_104;
    m_shooter->m_unk13A = param->m_108;
    m_shooter->m_unk13B = param->m_10C;
    m_shooter->m_unk13C = param->m_110;
    m_shooter->m_unk140 = param->m_114;
    m_shooter->m_unk144 = param->m_118;
    m_shooter->m_unk148 = param->m_11C;
    m_shooter->m_unk14C = param->m_120;
    m_shooter->m_unk150 = param->m_124;
    m_shooter->m_unk154 = param->m_128;
    m_shooter->m_unk158 = param->m_12C;
    m_shooter->m_unk15C = param->m_130;
    m_shooter->m_unk160 = param->m_134;
    m_shooter->m_unk164 = param->m_138;
    m_shooter->m_unk168 = param->m_13C;
    m_shooter->m_unk16C = param->m_140;
    m_shooter->m_unk170 = param->m_144;
    m_shooter->m_unk174 = param->m_148;
    m_shooter->m_unk178 = param->m_14C;
    m_shooter->m_unk17C = param->m_150;
    m_shooter->m_unk17D = param->m_154;
    m_shooter->m_unk17E = param->m_158;
    m_shooter->m_unk17F = param->m_15C;
    m_shooter->m_unk180 = param->m_160;
    m_shooter->m_unk181 = param->m_164;
    m_shooter->m_unk182 = param->m_168;
    m_shooter->m_unk183 = param->m_16C;
    m_shooter->m_unk37C = param->m_170;
    m_shooter->m_unk390 = param->m_174;
    m_shooter->m_unk394 = param->m_178;
    m_shooter->m_unk398 = param->m_17C;
}

csSampleModule::~csSampleModule() {
}
