#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <mu/mu_collection_viewer.h>
#include <ut/ut_list.h>

// The scene of the replay browser (module sora_scene). HYPOTHESIS: the word at 0x360 is the position of the cursor.
struct scReplay : scCollectionScene {
    int m_cursor;                   // 0x360
};

// The information that a replay holds about the match it recorded (only what the browser reads is declared).
// HYPOTHESIS: the names follow how the browser uses the fields.
struct muReplayInfo {
    u8 m_unk00[0x54];
    u8 m_gameMode;                  // 0x54 the kind of match (8: stock/time, 9 and 0x12: ..., 10 and 0x10: ...)
    u8 m_unk55;
    u8 m_charKind[4];               // 0x56 the character of each player
    u8 m_slotKind[4];               // 0x5A what plays each player (0: human, 1: computer ...)
    u8 m_unk5E[0x92 - 0x5E];
    u8 m_ruleKind;                  // 0x92
    u8 m_unk93;
    u16 m_stageKind;                // 0x94
    u8 m_unk96[2];
    u16 m_name[16];                 // 0x98 the name of a custom stage (UTF-16)
};

// What the collection hands out for a replay file: the information and the buffer it was read into.
// HYPOTHESIS: the names follow how the browser uses the fields.
struct muReplaySummary {
    u8 m_unk00[0xC];
    muReplayInfo* m_info;           // 0x0C
    u8 m_unk10[4];
    u8 m_isValid;                   // 0x14
};

// The replay data that is read from a save section. HYPOTHESIS: the object is deleted through the virtual destructor at 0x08.
class muReplayData {
public:
    int m_unk00;
    int m_unk04;
    virtual ~muReplayData();
    void* m_data;                   // 0x0C
    int m_size;                     // 0x10
};

namespace utility {

// A node of a list that knows the list it is on: it puts itself at the end of the list when it is made.
// HYPOTHESIS: the name; the layout follows from how the menu constructs and destroys the file below.
class ListedNode : public utListNode {
public:
    utList* m_list;                 // 0x08 the list that the node is on

    ListedNode(utList* list);
    ~ListedNode();
};

// A file that reads in the background and is on a list of the files that are open.
// HYPOTHESIS: the name is from the map of the module.
class AutoFile : public ListedNode {
public:
    virtual ~AutoFile();
    gfFileIOHandle m_handle;        // 0x10
    bool m_flag;                    // 0x14

    AutoFile();
};

} // namespace utility
static_assert(sizeof(utility::AutoFile) == 0x18, "Class is wrong size!");

// The browser of the saved replays of the collection: shows the thumbnail of the stage or the mode and the characters of
// the replay that the cursor is on, and starts the replay that the player decides on.
class muReplayTask : public muCollectionViewer {
public:
    class Work : public muCollectionViewer::Work {
    public:
        Work();
        virtual ~Work();

        scReplay* m_scene;                  // 0x1D0C the scene the menu belongs to
        int m_exitMode;                     // 0x1D10 what the menu was left with (0: replay, 1: back)
        int m_exitWait;                     // 0x1D14 frames to wait until the menu is left
        bool m_preloaded;                   // 0x1D18 the characters of the replay are being loaded
        utility::AutoFile m_stageNames;     // 0x1D1C the file with the names of the stages
    };

    muReplayTask(const char* name) : muCollectionViewer(name) { }
    static muReplayTask* create(scReplay* scene);

    virtual ~muReplayTask();
    virtual bool preLoad();
    virtual void loadResource();
    virtual void createModel();
    virtual void initMsg();
    virtual void getMineLabel(muCollectionMsgBuf* buf);
    virtual void getNewLabel();
    virtual u32 getCurFileID();
    virtual muCollectionFileName getFileHandle(int index);
    virtual void addObjCore(void* obj);
    virtual void removeObjCore(void* obj);
    virtual int getNumPic(int media);
    virtual int getNumPanel(int media);
    virtual int dispThumbnail(MuObject* object, void* summary);
    virtual void dispPreviewCore(void* info);
    virtual void erasePreviewCore();
    virtual void* getScnRoot();
    virtual void setPanelStateCore(int state, int unk);
    virtual void cancelAndExit();
    virtual void onDecided();
    virtual void controlExit();
    virtual void onPreCheckFinished();
    virtual void controlLoadStart();
    virtual void controlLoading();
    virtual void controlLoadFinish();
    virtual void onLoadFailed();
    virtual void controlBrowsing();
    virtual void controlBrowsingLoading();
    virtual void controlPopUpStart();
    virtual void controlPopUp();
    virtual void controlPopUpCheckStart();
    virtual void controlPopUpChecking();
    virtual void controlNewDataStart();
    virtual void controlNewDataChecking();
    virtual void controlNewData();
    virtual void controlNewDataExit();

    void setMelee();
    void dispPreviewStage(muReplayInfo* info);
    static int getGameMode(const muReplayInfo* info);

    Work* getWork() const { return static_cast<Work*>(m_work); }
};
static_assert(sizeof(muReplayTask) == 0x1B0, "Class is wrong size!");
static_assert(sizeof(muReplayTask::Work) == 0x1D34, "Class is wrong size!");
