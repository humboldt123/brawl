#pragma once

#include <StaticAssert.h>
#include <mu/mu_msg.h>
#include <mu/mu_proc_main.h>
#include <mu/mu_select_stage_task.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The My Music option of the Options screen: the player first picks a stage on the stage selection screen
// (muSelectStageTask), then gets the list of the songs of that stage (four lines, scrolled with up and down). Left and
// right change how often the song under the cursor plays (0 ... 100), A plays it or stops it, Start stops it, and B goes
// back to the stage selection.
// HYPOTHESIS: the names of the fields and of the steps follow what the functions do.
class muProcOptSong : public muProcMenu {
public:
    enum Step {
        Step_SelectStage = 0,       // waits for the stage selection
        Step_Come = 1,              // the list comes in
        Step_Select = 2,
        Step_Leave = 3,             // the list goes out, then back to the stage selection
        Step_Exit = 4,              // the stage selection was left with B: waits for it to close
    };

    enum Entry {
        RowCount = 4,
        MaxSongs = 15,
        Category_MyMusic = 4,       // the songs of a stage that has two categories (4 and 5) in sndBgmRateSystem
        Category_MyMusic2 = 5,
        Category_Custom = 0x2A,     // the songs of the custom stages
        StageCount = 0x42,
        Obj_Cursor0 = 0x58,         // the cursors of the four lines
        Obj_Row0 = 0x5C,            // the four lines: the title, the gauge of the rate and its icon
        Obj_Scroll = 0x60,          // the scroll arrows and the icon of the pad (play/stop)
        Mdl_Row0 = 5,               // the models of the four lines
    };

    MuMsg* m_msg;                   // 0x66C the titles of the four lines
    int m_category;                 // 0x670 the category of the songs in sndBgmRateSystem
    bool m_isPlaying;               // 0x674 a song of the list plays
    u16 m_scroll;                   // 0x676 the first song of the list
    u16 m_row;                      // 0x678 the line the cursor is on
    int m_padType;                  // 0x67C the type of the pad (a gfPadType::PadType); the icon shows its buttons
    u32 m_playingBgmId;             // 0x680 the song that plays (0: none)
    u32 m_categoryCount;            // 0x684 the songs of the category (of both for My Music)
    u32 m_myMusicCount;             // 0x688 the songs of category 4 that can be used (they come first in the list)
    u32 m_myMusicCount2;            // 0x68C the songs of category 5 that can be used
    void* m_selStageMsgData;        // 0x690 given to the stage selection
    muSelectStageTask* m_selStage;  // 0x694
    u32 m_bgmIds[MaxSongs];         // 0x698 the songs of the list (cursor = index)
    u32 m_bgmCount;                 // 0x6D4

    muProcOptSong(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* selStageMsgData, void* msgData);
    virtual ~muProcOptSong() { delete m_msg; }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual bool selectProc(int dir);
    virtual void playSE_CursorMove();
    void setGageList(u32 count, u32 start, int come);
    u32 countUsableSongs(int category);
    void addUsableSongs(int category);
};
static_assert(sizeof(muProcOptSong) == 0x6D8, "Class is wrong size!");
