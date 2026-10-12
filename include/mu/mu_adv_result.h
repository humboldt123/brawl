#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <mu/mu_resource_loader.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// A list of ids that the scene gives to the result screen (the items, the stickers and the music that the player got).
// HYPOTHESIS: the names follow how the screen walks over a list (rewind, count, take the id, go to the next).
class muAdvResultIdList {
public:
    virtual int getCount();             // 0x08
    virtual void rewind();              // 0x0C
    virtual void vf10();
    virtual void vf14();
    virtual u32 getId();                // 0x18
    virtual void vf1C();
    virtual void next();                // 0x20
};

// What the scene gives to the result screen.
struct muAdvResultTaskParam {
    int m_pad;                          // 0x00 the pad that decides
    int m_coins;                        // 0x04 the coins that the player got (they rain down)
    int m_value0;                       // 0x08 shown as number (HYPOTHESIS: the stickers)
    int m_value1;                       // 0x0C shown as number (HYPOTHESIS: the trophies)
    muAdvResultIdList* m_list0;         // 0x10
    muAdvResultIdList* m_list1;         // 0x14
    muAdvResultIdList* m_list2;         // 0x18
};

// The list of names that the screen shows (a task of the menu, created by a function of main.dol).
class muAdvResultListDisp : public gfTask {
public:
    u8 m_unk40[0x228];
    MuObject* m_object;                 // 0x268 the model that the texts are put on
};

// One coin that falls down the screen: the model it uses, its speed and its spin; the coins are in two lists (the ones
// that fall and the ones that wait).
struct muAdvResultCoin {
    int m_objectNo;                     // 0x00
    float m_speedX;                     // 0x04
    float m_speedY;                     // 0x08
    float m_spinX;                      // 0x0C
    float m_spinY;                      // 0x10
    float m_spinZ;                      // 0x14
    muAdvResultCoin* m_next;            // 0x18
    muAdvResultCoin* m_prev;            // 0x1C
};

// The result screen of the adventure mode: the numbers of coins, stickers and trophies, the lists of what was got,
// and coins that rain down while a song plays; a button leaves.
class muAdvResultTask : public gfTask {
public:
    enum Step {
        Step_Load = 0,
        Step_Load2 = 1,
        Step_FadeIn = 2,
        Step_Coin = 3,
        Step_Count = 6,
    };

    // One step of the screen: what runs when the step starts, and what runs every frame (it returns the next step).
    struct StepFunc {
        void (*m_init)(muAdvResultTask* task);
        int (*m_main)(muAdvResultTask* task);
    };

    static const int ObjNum = 32;
    static const int CoinNum = 30;

    int m_result;                       // 0x40 set when the screen is over
    int m_pad;                          // 0x44
    int m_inputWait;                    // 0x48 frames in which the pads are not read
    // The data of a muMenuController. HYPOTHESIS: the original does not run its constructor or destructor in this class.
    u8 m_controller[0xA8];              // 0x4C
    u8 m_unkF4[8];
    int m_coins;                        // 0xFC
    int m_value0;                       // 0x100
    int m_value1;                       // 0x104
    muAdvResultIdList* m_lists[3];      // 0x108
    void* m_figMng;                     // 0x114 the manager of the trophy list
    int m_unk118;                       // 0x118 the texts
    void* m_sealMng;                    // 0x11C the manager of the sticker list
    int m_unk120;                       // 0x120 the texts
    u8 m_unk124[4];
    int m_unk128;
    nw4r::g3d::ResFile m_resFileCamera; // 0x12C
    nw4r::g3d::ResFile m_resFileMenu;   // 0x130
    nw4r::g3d::ResFile m_resFileCoin;   // 0x134
    MuObject* m_objects[ObjNum];        // 0x138
    MuMsg* m_msg;                       // 0x1B8
    int m_step;                         // 0x1BC
    int m_stepPrev;                     // 0x1C0
    int m_unk1C4;
    u8 m_unk1C8[4];
    muAdvResultCoin m_coinPool[CoinNum];// 0x1CC
    muAdvResultCoin* m_coinFree;        // 0x58C
    muAdvResultCoin* m_coinActive;      // 0x590
    int m_coinsLeft;                    // 0x594
    int m_frame;                        // 0x598
    muAdvResultListDisp* m_listDisp;    // 0x59C
    u32 m_seHandle;                     // 0x5A0 the sound of the coins
    bool m_isSecond;                    // 0x5A4
    u8 m_unk5A5[3];
    gfArchive* m_archive;               // 0x5A8
    scResourceLoader* m_loader;         // 0x5AC

    muAdvResultTask();
    static muAdvResultTask* create(const muAdvResultTaskParam* param);

    virtual void processDefault();
    virtual ~muAdvResultTask();

    void initMsg();
    void listDisp();
    void controllProc();
    void createObjResFile(const struct muAdvResultModel* models, int count, nw4r::g3d::ResFile* resFile);
    void initDropCoinList();
    void dropCoinMain();
    void dropCoinStartOne(muAdvResultCoin* coin);
    bool dropCoinMainOne(muAdvResultCoin* coin);

    static void mainStepLoadInit(muAdvResultTask* task);
    static int mainStepLoadMain(muAdvResultTask* task);
    static void mainStepLoadInit2(muAdvResultTask* task);
    static int mainStepLoadMain2(muAdvResultTask* task);
    static void stepFadeInInit(muAdvResultTask* task);
    static int stepFadeInMain(muAdvResultTask* task);
    static void stepCoinInit(muAdvResultTask* task);
    static int stepCoinMain(muAdvResultTask* task);
};
static_assert(sizeof(muAdvResultTask) == 0x5B0, "Class is wrong size!");
