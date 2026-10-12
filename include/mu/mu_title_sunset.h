#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_fade.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_anmscn.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// How long the three parts of the sunset last (frames).
struct muTitleSunsetParam {
    u16 m_lengthB;              // 0x00 the second part (the sunset scene)
    u16 m_lengthC;              // 0x02 the third part (the sunset scene, until the end)
    u16 m_lengthA;              // 0x04 the first part (the characters that are carried away by the river)
};

// A fade of the screen: it starts when the part is about to end, or at the start of the next part.
struct muTitleSunsetFade {
    u8 m_enabled;               // 0x00
    float m_time;               // 0x04 how long the fade takes (frames), and how long before the end of the part it starts
    u8 m_from[4];               // 0x08 the color that the fade starts with (red, green, blue, alpha)
    u8 m_to[4];                 // 0x0C the color that the fade ends with
};
static_assert(sizeof(muTitleSunsetFade) == 0x10, "Class is wrong size!");

// The title screen at the sunset (the sunset of Super Smash Bros. Brawl's opening): a scene where the characters of the
// game are carried away on a river ("nagashi"), then the sunset scene with the characters in the places that the
// scene gives. The scene is a file that is made by the title scene; the task moves the scene animation and fades the
// screen between the parts.
class muTitleSunsetTask : public gfTask {
public:
    enum { CharCount = 35 };    // the characters that are in the game

    enum State {
        State_Start = 1,
        State_ShowNagashi = 2,  // puts the characters of the river in the scene
        State_PlayNagashi = 3,  // the first part
        State_EndNagashi = 4,
        State_ShowSunsetB = 5,  // puts the characters of the sunset in the scene
        State_PlayB = 6,        // the second part
        State_EndB = 7,
        State_ShowSunsetC = 8,
        State_PlayC = 9,        // the third part
        State_EndC = 10,
        State_Hold = 11,        // the last frame of the scene stays
        State_End = 12,
    };

    muFadeTask* m_fade;                             // 0x40 HYPOTHESIS: the title scene sets it
    gfFileIOHandle m_handle;                        // 0x44
    nw4r::g3d::ResFile m_resFile;                   // 0x48
    MuObject* m_nagashiScene;                       // 0x4C "StgOpeningSunset_nagashi"
    MuObject* m_nagashi;                            // 0x50 "gr_nagashi"
    MuObject* m_nagashiPos;                         // 0x54 "nagashi_posi", the places of the characters of the river
    MuObject* m_charNagashi[CharCount];             // 0x58 the characters of the river
    MuObject* m_sunsetScene;                        // 0xE4 "StgOpeningSunset"
    MuObject* m_gake;                               // 0xE8 "gake"
    MuObject* m_x1;                                 // 0xEC "x1"
    MuObject* m_backPosi;                           // 0xF0 "back_posi", the places of the characters of the sunset
    MuObject* m_charBack[CharCount];                // 0xF4 the characters of the sunset
    const muTitleSunsetParam* m_param;              // 0x180
    const muTitleSunsetFade* m_fadeParams;          // 0x184
    int m_charCount;                                // 0x188 how many characters are shown
    int m_charList[CharCount];                      // 0x18C the characters of the river
    int m_charOrder[CharCount];                     // 0x218 the characters of the sunset
    float m_scrollA;                                // 0x2A4 HYPOTHESIS: the frame of the scene in the first part
    float m_speedA;                                 // 0x2A8
    float m_frameA;                                 // 0x2AC how many frames the first part ran
    float m_startFrame;                             // 0x2B0 the frame where the scene starts
    float m_scrollB;                                // 0x2B4
    float m_speedB;                                 // 0x2B8
    float m_frameB;                                 // 0x2BC
    float m_scrollC;                                // 0x2C0
    float m_speedC;                                 // 0x2C4
    float m_frameC;                                 // 0x2C8
    char m_nagashiName[CharCount][0x20];            // 0x2CC "nagashi_posi1" ...
    char m_backName[CharCount][0x20];               // 0x72C "back_posi1" ...
    char m_charNagashiName[CharCount][0x20];        // 0xB8C "bil_mario" ...
    char m_charBackName[CharCount][0x20];           // 0xFEC "bk_mario" ...
    u8 m_skipFirst;                                 // 0x144C HYPOTHESIS: the scene starts at the sunset
    u8 m_nagashiLoaded;                             // 0x144D the river is in the scene
    u8 m_sunsetLoaded;                              // 0x144E the sunset is in the scene
    nw4r::g3d::ResAnmScn m_anmScn;                  // 0x1450
    int m_state;                                    // 0x1454
    int m_unk1458;                                  // 0x1458

    muTitleSunsetTask();
    static muTitleSunsetTask* create();

    virtual void processDefault();
    virtual ~muTitleSunsetTask();

    void initialize();
    void release();
    void createData(nw4r::g3d::ResFile* resFile);
    void createCharaList();
    void setSunsetParam(const muTitleSunsetParam* param);
    void setFadeParam(const muTitleSunsetFade* fadeParams);
    void skip();
    void removeNagashiModel();
    void addSunsetModel();
    void removeSunsetModel();
    void setCharaModelPosFromListSunset();
    void setFadeFromParamID(int id);
    void createEnableUseCharaList(int* list);
    void printEnableUseCharaList();
};
static_assert(sizeof(muTitleSunsetTask) == 0x145C, "Class is wrong size!");
