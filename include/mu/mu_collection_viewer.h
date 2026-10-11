#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <if/if_utility.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

// The scenes that show the collection viewers (module sora_scene; only what the menus need is declared).
// HYPOTHESIS: the names follow how the menus use the fields.
struct muCollectionSceneHolder {
    u8 m_unk00[0x48];
    void* m_scnRoot;                // 0x48 the scene root that the models of the menu are added to
    u8 m_unk4C[0x1A4 - 0x4C];
    u8 m_hasScnRoot;                // 0x1A4
};

struct scCollectionScene {
    u8 m_unk000[0x35C];
    muCollectionSceneHolder** m_holder; // 0x35C
};

// A text for the message window: the data and how much of it is used.
struct muCollectionMsgBuf {
    u8 m_data[0x80];
    int m_size;                     // 0x80

    void append(const char* text, u32 size) {
        int used = m_size;
        memcpy(m_data + used, text, size);
        m_size = used + size;
    }
};

// A name of a model or of an animation: it is built by copying and adding to it.
// HYPOTHESIS: the copies follow from the way the compiler copies a 0x41 byte structure around.
struct muCollectionName {
    char m_str[0x41];

    muCollectionName() { }
    muCollectionName(const char* str) { init(str); }
    void init(const char* str) {
        m_str[0] = 0;
        strcat(m_str, str);
    }
    void addNumber(int number) { sprintf(m_str + strlen(m_str), "%d", number); }
    void addSeparator() { strcat(m_str, "__"); }
};

// The name of a file of the collection, as the collection hands it out.
// HYPOTHESIS: the size follows from how the name is copied around.
struct muCollectionFileName {
    char m_name[0x41];
};

// The base of the menus that browse through the pictures of the collection (module sora_menu_collect_viewer). HYPOTHESIS:
// only what the snapshot menu needs is declared; the names of the virtual functions come from the map of that module and
// the unnamed ones keep the offset of their function there.
class muCollectionViewer : public utility::StandardMenu {
public:
    // The files that the menu loads: the first four words of one of them are not known.
    struct MsgFile {
        u8 m_unk00[0x10];
        gfFileIOHandle m_handle;    // 0x10
        u8 m_unk14[4];
    };

    // One medium (the console or the card) that the pictures are on.
    struct Media {
        int m_numPic;               // 0x00
        const char* m_names;        // 0x04 the names of the files, one after the other
        int m_unk08;
        int m_unk0C;
    };

    // The names of the animations of a panel (11 for the left panel and 11 for the right one) and for each state of the
    // panel the one to play. HYPOTHESIS: the sizes follow from how the snapshot menu uses the table.
    struct PanelMotionTable {
        MuAnimNameData m_anim[22];
        u8 m_index[12];             // 0x1B8

        PanelMotionTable(const char* nameLeft, const char* nameRight);
    };

    class Work {
    public:
        int m_unk000;
        u8 m_unk004[0x10];
        MsgFile m_brres;            // 0x014 the models (a loader of the menu: it is reset, then asked to read a file)
        u8 m_brresFlag;             // 0x02C
        u8 m_unk02D[0x9C - 0x2D];
        int m_unk09C;
        u8 m_unk0A0[0x10];
        MsgFile m_wait;             // 0x0B0 the wait window
        u8 m_waitFlag;              // 0x0C8
        u8 m_unk0C9[0x104 - 0xC9];
        MuObject* m_objs[39];       // 0x104 the models of the menu
        int m_curMedia;             // 0x1A0
        int m_unk1A4;
        Media m_media[2];           // 0x1A8
        u8 m_unk1C8[0x1B40 - 0x1C8];
        int m_numFiles;             // 0x1B40
        u8 m_unk1B44[0x1BF0 - 0x1B44];
        int m_panelState;           // 0x1BF0
        int m_exitRequest;          // 0x1BF4
        int m_unk1BF8;
        int m_state;                // 0x1BFC
        u8 m_unk1C00[0x1C7C - 0x1C00];
        MuMsg* m_msg;               // 0x1C7C the texts
        MsgFile m_msgFile[3];       // 0x1C80
        u8 m_unk1CC8[0x1CE4 - 0x1CC8];
        void* m_collection;         // 0x1CE4
        u8 m_unk1CE8[0x1CEE - 0x1CE8];
        u8 m_extraPanel;            // 0x1CEE
        u8 m_unk1CEF[0x1CF0 - 0x1CEF];
        int m_unk1CF0;              // 0x1CF0
        u8 m_unk1CF4[0x1CF8 - 0x1CF4];
        int m_unk1CF8;              // 0x1CF8
        u8 m_unk1CFC[0x1D08 - 0x1CFC];

        Work(int numObjs, int unk, const char* name);
        ~Work();
    };

    u8 m_unk40[0x168];
    u32 m_input;                    // 0x1A8
    Work* m_work;                   // 0x1AC

    muCollectionViewer(const char* name);
    virtual ~muCollectionViewer();
    virtual bool checkLoadFinish() const;
    virtual void update();

    virtual void createObjResFile(const char* name);                                    // 0x1260
    virtual void expandPic(int unk);                                                    // 0x1A38
    virtual void changePicAnim(const char* name, int unk, Work* work);                  // 0x1BCC
    virtual void unk_1D58();
    virtual void initMsg();
    virtual void unk_3134();
    virtual void unk_2EC0();
    virtual void getMineLabel(muCollectionMsgBuf* buf);
    virtual void getNewLabel();
    virtual void unk_2FA0();
    virtual u32 getCurFileID();
    virtual muCollectionFileName getFileHandle(int index);
    virtual void unk_2FC8();
    virtual void addObjCore(void* obj);
    virtual void removeObjCore(void* obj);
    virtual int getNumPic(int media);
    virtual int getNumPanel(int media);
    virtual void unk_3784();
    virtual void unk_38F8();
    virtual void dispPreview();                                                         // 0x21BC
    virtual void unk_2528();
    virtual int dispThumbnail(MuObject* object, void* summary);
    virtual void dispPreviewCore(void* info);
    virtual void erasePreviewCore();
    virtual void unk_3194();
    virtual void unk_31D8();
    virtual void* getScnRoot();
    virtual void setPanelStateCore(int state, int unk);
    virtual void unk_3B1C();
    virtual void unk_3B70();
    virtual void cancelAndExit();
    virtual void controlProc();                                                         // 0x5418
    virtual void controlWaitForDialog();                                                // 0x5A1C
    virtual void onDecided();
    virtual void controlExit();
    virtual void controlPreCheck();                                                     // 0x5018
    virtual void onPreCheckFinished();
    virtual void controlScanStart();                                                    // 0x527C
    virtual void controlScanAllStart();                                                 // 0x52E8
    virtual void controlScan();                                                         // 0x533C
    virtual void controlPicSelect();                                                    // 0x5A90
    virtual int slidePic(int unk, int unk2);                                            // 0x5C8C
    virtual void slidePicCore(int unk);                                                 // 0x5D4C
    virtual void controlLoadStart();                                                    // 0x5F24
    virtual void controlLoading();                                                      // 0x5F9C
    virtual void controlLoadFinish();
    virtual void onLoadFailed();
    virtual void controlErase();                                                        // 0x6068
    virtual void openEraseConfirmWnd();                                                 // 0x61FC
    virtual void closeEraseConfirmWnd();                                                // 0x62AC
    virtual void dispEraseConfirmWndYesNo();                                            // 0x6344
    virtual void openEraseReportWnd();                                                  // 0x6450
    virtual void unk_64C8();
    virtual void closeEraseReportWnd();                                                 // 0x6520
    virtual void controlEraseConfirm();                                                 // 0x65A0
    virtual void openWaitWnd();                                                         // 0x67B8
    virtual void closeWaitWnd();                                                        // 0x683C
    virtual void controlEraseReady();                                                   // 0x684C
    virtual void controlErasing();                                                      // 0x6900
    virtual void controlEraseReport();                                                  // 0x69F0
    virtual void endErase();                                                            // 0x6A84
    virtual void unk_6AE8();
    virtual void openMediaMenu();                                                       // 0x6E28
    virtual void beginCopy();                                                           // 0x71E0
    virtual void controlCopyReady();                                                    // 0x72B8
    virtual void controlCopying();                                                      // 0x73B4
    virtual void controlCopyReport();                                                   // 0x74D0
    virtual void endCopy();                                                             // 0x7274
    virtual void dispMediaMenuSelect();                                                 // 0x6E80
    virtual void controlMediaMenu();                                                    // 0x6EF8
    virtual void controlMediaCheck();                                                   // 0x7564
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
    virtual void controlSend();                                                         // 0x7730
    virtual void controlContribute();                                                   // 0x7C90
    virtual void getSummary();                                                          // 0x4148
};
static_assert(sizeof(muCollectionViewer) == 0x1B0, "Class is wrong size!");
static_assert(sizeof(muCollectionViewer::Work) == 0x1D08, "Class is wrong size!");
