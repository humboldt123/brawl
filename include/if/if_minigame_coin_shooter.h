#pragma once

// HYPOTHESIS: layout of IfCoinShooter, the HUD task of the Coin Shooter minigame (if_minigame_coin_shooter.o), from the
// constructor (0x80101C0C, which is also the only place that sets every member), the destructor (0x80101F88), loadResource,
// createModel, update/onUpdate, setCoinCount, updateBonusPoint, showFigure(Core), userProc, and the three label classes
// that live in an anonymous namespace of the source file. Names without a proven role are m_unkNNN (NNN = hex byte offset).
//
// Base classes (vtable lbl_8045CD90): utility::StandardMenu (vptr 0x3C; entry 22 is ~IfCoinShooter, 25 loadResource,
// 28 createModel, 29 update; the rest is StandardMenu's/gfTask's) and gfCallBack at 0x1AC (m_next 0x1AC, vptr 0x1B0 =
// vtable + 0x7C, offset word 0xFFFFFE54). The object is allocated with 0x334 bytes (IfCoinShooter::create).
//
// The label classes (IfCoinShooterLabelSimple / Light / Counter) are local to the source file (RTTI names
// "@unnamed@if_minigame_coin_shooter_cpp@::..."), so they are defined there; IfCoinShooterLabelImpl, their base, is global.

#include <StaticAssert.h>
#include <stddef.h>
#include <gf/gf_callback.h>
#include <if/if_figure_loader.h>
#include <if/if_utility.h>
#include <mu/mu_object.h>
#include <mu/mu_vc_trial.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <std/auto_ptr.h>
#include <types.h>

class IfRewardListTask;
class IfCoinShooterLabelImpl;
class MuMsg;
class Vec3f;
class tySealListDataManager;

class IfCoinShooterLabel {
public:
    // HYPOTHESIS: names. The numbers index the model table lbl_8045C8C4 (InfShooter0000 for 0 and 1, 0001, 0002, 0003,
    // 0004, 0005, and 0006 for 7 and 8). The scene (sq_coin_shooter) creates 0 to 6; 7 and 8 are made only by the
    // destructor of a Counter label (a coin / a seal that flies to the totals).
    enum Type {
        Type_0 = 0,  // IfCoinShooterLabelSimple, InfShooter0000, frame 0 of the visible animation
        Type_1 = 1,  // IfCoinShooterLabelSimple, InfShooter0000, frame 1
        Type_2 = 2,  // IfCoinShooterLabelSimple, InfShooter0001
        Type_3 = 3,  // IfCoinShooterLabelSimple, InfShooter0002
        Type_4 = 4,  // IfCoinShooterLabelCounter, InfShooter0003: a number of coins; ends as a Type_7 Light
        Type_5 = 5,  // IfCoinShooterLabelCounter, InfShooter0004: a number plus a seal; ends as a Type_8 Light
        Type_6 = 6,  // IfCoinShooterLabelSimple, InfShooter0005; the shooter's flag 0x1C4 is set while it lives
        Type_7 = 7,  // IfCoinShooterLabelLight: adds its value to the coin total (m_coinNumber)
        Type_8 = 8,  // IfCoinShooterLabelLight: adds its value to the seal total (m_sealNumber)
    };

    IfCoinShooterLabelImpl* unk0;
    IfCoinShooterLabel();
    ~IfCoinShooterLabel();
    // HYPOTHESIS: parameter names. type 4 and 5 pass the number to show and (type 5) the id of the seal that
    // tySealListDataManager::getSealDataId resolves; the other types ignore both. The callers in sq_coin_shooter
    // pass (type, count, 0) or (5, 1, sealId).
    void create(Type type, int value, u32 sealId);
    void reset();
    void setPos(const Vec3f* position);
};
static_assert(sizeof(IfCoinShooterLabel) == 0x4, "Class is the wrong size!");

// The base of the three label classes: every one is on AutoList<IfCoinShooterLabelImpl> (the list lbl_8049E560, 0xC bytes
// built by the static initializer fn_801048AC) while it exists. It is 0x18 bytes: the node (next 0, prev 4, list 8),
// the owner, the state word and the vptr (vtable of the derived class: dtor 0x8, model 0xC, update 0x10, detach 0x14).
class IfCoinShooterLabelImpl : public utility::AutoList<IfCoinShooterLabelImpl>::Node {
public:
    // HYPOTHESIS: owner pointer, four-bit state, and two behavior flags.
    // unkC: the IfCoinShooterLabel that owns it (cleared by the owner's reset, and the owner's unk0 by the destructor).
    // unk10 (top nibble, signed): the countdown that userProc runs after the animation ended (set to 2 by userProc);
    //   at 0 the label is flagged unk10_3 and onUpdate deletes it.
    // unk10_3 (0x08): delete me. unk10_2 (0x04): the node animation loops (MuObject::isNodeAnimLoop at the end of the ctor).
    IfCoinShooterLabel* unkC;
    u32 unk10 : 4;
    u8 unk10_3 : 1;
    u8 unk10_2 : 1;
    u8 unk10_0 : 2;
    u8 unk11[3];

    virtual ~IfCoinShooterLabelImpl();
    virtual MuObject* model() const = 0;
    virtual void update();
    virtual void detach() = 0;
};
static_assert(sizeof(IfCoinShooterLabelImpl) == 0x18, "Class is the wrong size!");

class IfCoinShooter : public utility::StandardMenu, public gfCallBack {
public:
    // A counter made of up to four digit models (InfShooter0007, the digit models switch their texture with
    // MuObject::setFrameTex) on the nodes pos00.. of a ScnMdlExpand that sits on a base model. The coin / seal / figure
    // totals of the HUD are three of them in the IfCoinShooter, and every Counter label has one at 0x18.
    class Number {
    public:
        // HYPOTHESIS: the position order. true: digit i (from the least significant) goes to node pos0<i>;
        // false: to pos0<m_digits - 1 - i>. The HUD sets it 1, 1, 0 and the Counter labels set 1.
        bool m_ascending;                       // 0x00
        s32 m_value;                            // 0x04 the shown number (set)
        s32 m_digits;                           // 0x08 how many digit models are used (createModel: 2, 2, 4; Counter: 2)
        std::auto_ptr<MuObject> m_digitObjs[4]; // 0x0C InfShooter0007_TopN; each gets the texture of one digit
        std::auto_ptr<MuObject> m_baseObj;      // 0x1C the model that carries the digits (InfShooter0013 / 0014 / 0011 ...)
        std::auto_ptr<utility::DepthScaler> m_scaler; // 0x20 DepthScaler(0.01, 0.99) on m_baseObj (the 0.5, 0.5 scaler for the figure)
        muG3dObjHolder<nw4r::g3d::ScnMdlExpand> m_expand; // 0x24 the group on m_baseObj that the digit models are pushed into

        // Shows the number: clips it to 0 .. 10^m_digits - 1, pushes the digit models that are needed into m_expand
        // and takes the others out; the digit value goes to the texture frame. fn_80101948.
        void set(int value);
    };
    static_assert(sizeof(Number) == 0x28, "Class is the wrong size!");

    // HYPOTHESIS: a gauge of the bonus (0 .. 100): 0x300 is the value that the gauge shows (updateBonusPoint); the model
    // is InfShooter0015 (frame = value / 10 as the visible animation; the color animation changes at 10 and 100).
    // The shared guard pattern of the destructor (a null test on the address of the struct and one on the member) says
    // that the two form a struct.
    struct BonusGauge {
        s32 m_value;                        // 0x00
        std::auto_ptr<MuObject> m_obj;      // 0x04
    };
    static_assert(sizeof(BonusGauge) == 0x8, "Class is the wrong size!");

    // HYPOTHESIS: the trophy that is shown after the player gets one (showFigureCore): the trophy model made from the
    // resource of the loader entry (m_figEntry), the Stand model (DolStand, from /toy/fig/Stand.brres) it stands on
    // and the group that holds them under the tree of the Number for the figures. The destructor has the same guard pattern
    // for them as for the structs above.
    struct FigureView {
        std::auto_ptr<MuObject> m_model;                    // 0x00 made in showFigureCore from the entry's ResFile
        std::auto_ptr<utility::DepthScaler> m_modelScaler;  // 0x04 DepthScaler(0.5, 0.5) on m_model
        std::auto_ptr<MuObject> m_stand;                    // 0x08 DolStand (createModel; the animation is stopped at frame 38)
        std::auto_ptr<utility::DepthScaler> m_standScaler;  // 0x0C DepthScaler(0.5, 0.5) on m_stand
        muG3dObjHolder<nw4r::g3d::ScnMdlExpand> m_expand;   // 0x10 ScnMdlExpand (1 slot) on m_model, pos00 holds m_stand
    };
    static_assert(sizeof(FigureView) == 0x14, "Class is the wrong size!");

    // 0x1AC: gfCallBack { m_next 0x1AC, vptr 0x1B0 }
    u8 m_decisionPad;                           // 0x1B4 gmGetMenuDecisionPad() (the pad that pauses: getSysPadStatus)
    bool m_started;                             // 0x1B5 start() sets it; update does nothing before that
    s32 m_clearMsgTimer;                        // 0x1B8 createModel sets 90; onUpdate counts it down and prints message 2 with an empty string at 0
    s32 m_objPriority;                          // 0x1BC the draw priority that the next MuObject::create gets (createModel starts at 0x10 and ++)
    s32 m_bonusTarget;                          // 0x1C0 the bonus that updateBonusPoint is given (100 while m_unk1C4 is set); only read in this unit
    bool m_unk1C4;                              // 0x1C4 HYPOTHESIS: set while a Type_6 Simple label exists (its ctor sets it, its dtor clears it)
    utility::AutoResFile m_infoRes;             // 0x1C8 /info/info_coin_shooter.brres (loadResource); the HUD models come from it
    utility::AutoResFile m_standRes;            // 0x1FC /toy/fig/Stand.brres (the Stand model for the trophy)
    utility::AutoResFile m_sealRes;             // 0x230 /toy/seal/ToySealThumb.brres (the seal thumbnails: GetResTex(m_sealRes, name))
    utility::AutoFile m_msgFile;                // 0x264 /menu/msg/mini_coin_shooter.msbin
    muG3dObjHolder<nw4r::g3d::ScnGroup> m_scnGroup; // 0x27C ScnGroup (capacity 0x20) added to gfSceneRoot layer 8 (Menu); all HUD models hang on it
    std::auto_ptr<MuObject> m_root;             // 0x280 InfShooter0010_TopN, the main frame (the MuMsg text is attached to it; Light labels fly to it)
    muG3dObjHolder<nw4r::g3d::ScnMdlExpand> m_rootExpand; // 0x284 ScnMdlExpand (3 slots) on m_root: pos00 / pos01 / pos02 (the Number bases and the figure view)
    Number m_sealNumber;                        // 0x288 HYPOTHESIS: seal total (a Type_8 Light adds its value to m_sealNumber.m_value); base InfShooter0013, 2 digits
    Number m_figureNumber;                      // 0x2B0 HYPOTHESIS: figure total (showFigureCore adds 1); base InfShooter0014, 2 digits
    Number m_coinNumber;                        // 0x2D8 coin total (setCoinCount, Type_7 Light); base InfShooter0011, 4 digits
    BonusGauge m_bonusGauge;                    // 0x300
    FigureView m_figureView;                    // 0x308
    std::auto_ptr<MuObject> m_unk31C;           // 0x31C InfShooter0023_TopN, the seal thumbnail model (Type_5 Counter / Type_8 Light retexture it)
    s32 m_figureTimer;                          // 0x320 -1: idle; showFigure sets 2, userProc counts it down while m_figEntry is set, onUpdate shows the figure at 0
    IfFigureLoader::EntryRef m_figEntry;        // 0x324 the loader entry of the trophy that is shown (showFigure takes it over)
    std::auto_ptr<tySealListDataManager> m_sealMgr; // 0x328 made in loadResource (tySealListDataManager(true), 0x3EB4 bytes)
    muTaskHolder<IfRewardListTask> m_rewardTask; // 0x32C the reward notice (pause menu): IfRewardListTask::create in createModel
    std::auto_ptr<MuMsg> m_msg;                 // 0x330 MuMsg (1 line, 0x100 bytes) with /menu/msg/mini_coin_shooter.msbin, printed on m_root's model

    IfCoinShooter();                            // 0x80101C0C
    virtual ~IfCoinShooter();                   // 0x80101F88
    static void create();
    static void destroy();
    bool isQuit();                              // 0x80103CAC HYPOTHESIS: the reward task is waiting for the figure loader to be idle
    bool isLoadFinish();                        // 0x80103C44 the menu is loaded and the reward task and the figure loader are, too
    void start();
    // HYPOTHESIS: delta is added to the coin total (clipped to >= 0); with animate the base model plays its pos-1 animation.
    // Called by sq_coin_shooter as (instance, delta, false) and (instance, -n, true).
    void setCoinCount(int delta, bool animate); // 0x80103CF8
    // HYPOTHESIS: the gauge shows min(max(value, 0), 100); the color animation of m_obj changes on every 10.
    void updateBonusPoint(int value);           // 0x80103FA4
    // HYPOTHESIS: takes the loader entry over (the reference is emptied) and starts the 2 frame countdown to showFigureCore.
    void showFigure(IfFigureLoader::EntryRef* entry); // 0x801042B4
    void showFigureCore();                      // 0x801043D0
    void onUpdate();                            // 0x80103A7C

    virtual void loadResource();                // 0x801026F4
    virtual void createModel();                 // 0x80102808
    virtual void update();                      // 0x801039B4
    // gfCallBack (0x1AC): runs every frame from g_gfApplication's callback list (gfCallBackList at +0x114).
    virtual void userProc();                    // 0x80104784
};
static_assert(sizeof(IfCoinShooter) == 0x334, "Class is the wrong size!");
// Offsets proven by the constructor (0x80101C0C) and the destructor (0x80101F88).
static_assert(offsetof(IfCoinShooter, m_decisionPad) == 0x1B4, "Class is the wrong size!");
static_assert(offsetof(IfCoinShooter, m_objPriority) == 0x1BC, "Class is the wrong size!");
static_assert(offsetof(IfCoinShooter, m_infoRes) == 0x1C8, "Class is the wrong size!");
static_assert(offsetof(IfCoinShooter, m_standRes) == 0x1FC, "Class is the wrong size!");
static_assert(offsetof(IfCoinShooter, m_sealRes) == 0x230, "Class is the wrong size!");
static_assert(offsetof(IfCoinShooter, m_msgFile) == 0x264, "Class is the wrong size!");
static_assert(offsetof(IfCoinShooter, m_scnGroup) == 0x27C, "Class is the wrong size!");
static_assert(offsetof(IfCoinShooter, m_sealNumber) == 0x288, "Class is the wrong size!");
static_assert(offsetof(IfCoinShooter, m_coinNumber) == 0x2D8, "Class is the wrong size!");
static_assert(offsetof(IfCoinShooter, m_bonusGauge) == 0x300, "Class is the wrong size!");
static_assert(offsetof(IfCoinShooter, m_figureView) == 0x308, "Class is the wrong size!");
static_assert(offsetof(IfCoinShooter, m_figureTimer) == 0x320, "Class is the wrong size!");
static_assert(offsetof(IfCoinShooter, m_msg) == 0x330, "Class is the wrong size!");
