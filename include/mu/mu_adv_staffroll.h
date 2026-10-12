#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <mt/mt_vector.h>
#include <mu/menu.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The root of a group of models that the menu draws. HYPOTHESIS: the name; it is the same class as the roots of the other
// menus (the shared headers call it MenuRoot but lack the type of the group).
class muAdvStaffRoot : public gfTask {
public:
    char m_unk40[8];
    void* scene;                    // 0x48 the group of models
};

// The header of the staff roll data (the excel data of the staff roll in the pack). HYPOTHESIS: the names follow how the
// task uses the fields.
struct muAdvStaffInit {
    u16 m_textNum;                  // 0x00 how many texts there are
    u8 m_rootVisibleNum;            // 0x02 how many entries the list of visibility changes has
    u8 m_rootMoveNum;               // 0x03 how many entries the list of camera moves has
    float m_length;                 // 0x04 how far the roll has to go
    u16 m_frameNum;                 // 0x08 how long the roll takes (frames)
};

// One change of the visibility of the movie: from which frame on it is shown or hidden.
struct muAdvStaffRootVisible {
    float m_frame;                  // 0x00
    u8 m_pending;                   // 0x04 the change has not been done yet
    u8 m_unk05;
    u8 m_show;                      // 0x06 1: show the movie, otherwise hide it
    u8 m_unk07;
};

// One move of a camera: from which frame on it goes where and for how long.
struct muAdvStaffRootMove {
    float m_frame;                  // 0x00
    u8 m_pending;                   // 0x04
    u8 m_target;                    // 0x05 which of the two cameras moves
    u8 m_unk06[2];
    float m_toX;                    // 0x08
    float m_toY;                    // 0x0C
    int m_frameNum;                 // 0x10 how long the move takes
};

// One text of the staff roll. HYPOTHESIS: the names follow how setupExcelParam reads the fields.
struct muAdvStaffText {
    u16 m_msgIndex;                 // 0x00
    u8 m_lineAdd;                   // 0x02
    u8 m_unk03;
    int m_x;                        // 0x04
    int m_y;                        // 0x08
    u8 m_scaleIndex;                // 0x0C
    u8 m_align;                     // 0x0D
    u8 m_colorIndex;                // 0x0E
    u8 m_unk0F;
};

// A move of a camera from one place to another over a number of frames.
struct muAdvStaffCamera {
    Vec3f m_cur;                    // 0x00 where the camera is
    bool m_active;                  // 0x0C the camera is moving
    Vec3f m_from;                   // 0x10
    Vec3f m_to;                     // 0x1C
    int m_total;                    // 0x28 how many frames the move takes
    int m_count;                    // 0x2C how many frames it took already
};

// The staff roll of the adventure mode: the movie plays in the background while the names of the staff scroll up the
// screen, and the logos and the end text fade in and out.
class muAdvStaffrollTask : public gfTask {
public:
    enum State {
        State_Start = 0,
        State_Count = 0x19,
    };

    gfFileIOHandle m_handleModels;        // 0x40 the models
    gfFileIOHandle m_handleTexts;         // 0x44 the texts
    gfFileIOHandle m_handleExcel;         // 0x48 the excel data
    nw4r::g3d::ResFile m_resFile;           // 0x4C
    muAdvStaffRoot* m_objectRoot;                 // 0x50
    muAdvStaffRoot* m_movieRoot;                  // 0x54
    muAdvStaffRoot* m_textRoot;                   // 0x58
    MuObject* m_movie;                      // 0x5C
    MuObject* m_logos[2];                   // 0x60 the logos of the engine makers
    MuObject* m_allRights;                  // 0x68
    MuObject* m_theEnd;                     // 0x6C
    MuObject** m_texts;                     // 0x70
    MuMsg* m_msg;                           // 0x74
    void* m_player;                         // 0x78 the player of the movie
    bool m_movieShown;                      // 0x7C
    bool m_movieDraw;                       // 0x7D
    int m_movieFrames;                      // 0x80
    int m_unk84;
    muAdvStaffInit* m_init;                 // 0x88
    muAdvStaffInit* m_end;                  // 0x8C
    muAdvStaffRootVisible* m_rootVisible;   // 0x90
    muAdvStaffRootMove* m_rootMove;         // 0x94
    float* m_modelParam;                    // 0x98
    float* m_scaleParam;                    // 0x9C
    u8* m_colorParam;                       // 0xA0
    muAdvStaffText* m_textParam;            // 0xA4
    int m_textNum;                          // 0xA8
    int m_rootVisibleNum;                   // 0xAC
    int m_rootMoveNum;                      // 0xB0
    float m_scrollY;                        // 0xB4
    float m_scrollSpeed;                    // 0xB8
    int m_frame;                            // 0xBC
    float* m_textY;                         // 0xC0
    float m_fade;                           // 0xC4
    bool m_done;                            // 0xC8
    muAdvStaffCamera m_cameras[2];          // 0xCC
    int m_state;                            // 0x12C
    int m_stateFrame;                       // 0x130

    muAdvStaffrollTask();
    static muAdvStaffrollTask* create();

    virtual void processDefault();
    virtual void renderOpa();
    virtual ~muAdvStaffrollTask();

    void initialize();
    void release();
    void requestFiles();
    void openMovie();
    int isLoadingFiles();
    bool isLoadingMovie();
    void createData();
    bool isEnableSkip();
    void skip();
    void createExcelData();
    void setupExcelParam();
};
static_assert(sizeof(muAdvStaffrollTask) == 0x134, "Class is wrong size!");
