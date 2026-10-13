#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// One fighter of the tournament as the game keeps it (gmTournamentData: the count of the fighters, then these from +8).
// HYPOTHESIS: the names follow how the tournament menus read the fields.
struct muTourEntrant {
    u8 m_kind;                  // 0x00 0: a player's, 1: a CPU, 2: not used
    u8 m_unk01;
    char m_name[0x15];          // 0x02 the name of the fighter
    u8 m_nameTag;               // 0x17 the number of the saved name (0x78: none)
    u8 m_index;                 // 0x18 the number the name was given (Player 1, CPU 1, ...)
    u8 m_named;                 // 0x19 the name was picked by the player
    u8 m_character;             // 0x1A the kind of the fighter in the menu (0x29: none)
    u8 m_pickedCharacter;       // 0x1B the fighter the player picked (0x29: left to chance)
    u8 m_color;                 // 0x1C the costume of the fighter
    u8 m_unk1D;
    u16 m_handicap;             // 0x1E
    int m_slot;                 // 0x20 the place of the fighter in the table (-1: none)
    u8 m_cpuLevel;              // 0x24 the level of the CPU
    u8 m_unk25[3];
};
static_assert(sizeof(muTourEntrant) == 0x28, "Class is wrong size!");

// The data of the tournament in the game (gmTournamentData, 0x510 bytes).
struct muTourData {
    u8 m_count;                 // 0x00 how many fighters take part
    u8 m_playersPerMatch;       // 0x01
    u8 m_unk02;
    u8 m_unk03;                 // 0x03 the players of a match, less one
    u8 m_sizeIndex;             // 0x04
    u8 m_stageMode;             // 0x05
    u8 m_cpuLevel;              // 0x06
    u8 m_unk07;
    muTourEntrant m_entrants[0x20];     // 0x08
    u8 m_unk508;
    u8 m_edited;                // 0x509 the player changed the fighters
    u8 m_stageFlag;             // 0x50A
    u8 m_unk50B;
    int m_ctrlPad;              // 0x50C
};
static_assert(sizeof(muTourData) == 0x510, "Class is wrong size!");

// The shape of the bracket for one kind of tournament (fighters in a match) and one number of fighters.
struct muTourShape {
    u8 m_rounds;                // 0x00 how many rounds there are
    float m_scale;              // 0x04 how small the table is drawn so it fits the screen
    u8 m_matches[5];            // 0x08 how many matches each round has
    u8 m_fighters[5];           // 0x0D how many fighters play each round (a line is drawn for each)
    u8 m_unk12[2];
};
static_assert(sizeof(muTourShape) == 0x14, "Class is wrong size!");

// A line (a fighter of a round) of the table: which of the lines of the file it is, how far it is moved, how many fighters
// pass the node and how the color of the line is told (see setTableData).
// HYPOTHESIS: the names follow how setTableData reads the fields.
struct muTourRow {
    u8 m_line;                  // 0x00
    float m_scale;              // 0x04
    u8 m_fighters;              // 0x08
    u8 m_kind;                  // 0x09 2 ... 5: how many limits there are to tell the color (see the limits)
    u8 m_limit[3];              // 0x0A
};
static_assert(sizeof(muTourRow) == 0x10, "Class is wrong size!");

// How the colors of the last stage of the table are told: the kind (how many limits there are) and the limits.
struct muTourFinal {
    int m_kind;                 // 0x00
    int m_limit[4];             // 0x04
};
static_assert(sizeof(muTourFinal) == 0x14, "Class is wrong size!");

// The way one fighter takes through the table: the lines it follows (pairs: the line it leaves and the line it goes to) and the
// color of each line.
struct muTourPath {
    u8 m_step;                  // 0x00 the pair of lines the fighter is on
    u8 m_line[11];              // 0x01 the lines (the last one says what the last match is, see setTableData)
    u8 m_color[12];             // 0x0C the color (frame of the material animation) of each line
    int m_order;                // 0x18 the place of the fighter in the list of the fighters
};
static_assert(sizeof(muTourPath) == 0x1C, "Class is wrong size!");

// The table (bracket) of the tournament: lines that join the fighters' names, the badges and faces. It moves on the screen to
// show the match that is played, and the fighters of the match walk to the node of the match.
class muTourTable {
public:
    nw4r::g3d::ScnGroup* m_group;           // 0x000 the model the lines and names are put in (a ScnMdlExpand)
    MuObject* m_objs[0x5E];                 // 0x004 the lines (0 ... 0x3E: horizontal ones, 0x3F ...: the others)
    nw4r::g3d::ResFile m_resLineB[0x1F];    // 0x17C a copy of the file for each of the lines 0x3F ...
    nw4r::g3d::ResFile m_resLineA[0x3F];    // 0x1F8 a copy of the file for each of the lines 0 ... 0x3E
    muObjProcessTask* m_objProc;            // 0x2F4
    int m_priority;                         // 0x2F8 the draw priority of the models
    muTourShape m_shape;                    // 0x2FC
    muTourRow m_rows[4][16];                // 0x310 the lines of the rounds after the first one (as setTableData made them)
    u8 m_pairs[0xA0];                       // 0x710 the lines (first, last) of the matches
    Vec3f m_nodePos[5][16];                 // 0x7B0 where the match nodes of each round are
    Vec3f m_basePos;                        // 0xB70 where the table is when no match is shown
    Vec3f m_pos;                            // 0xB7C where the table is now (the table is moved to show a match)
    int m_finalLine;                        // 0xB88 the last line (the one of the winner)
    int m_playersPerMatch;                  // 0xB8C
    int m_moving;                           // 0xB90 how many fighters walk now
    muTourPath m_paths[0x20];               // 0xB94
    int m_slot[4];                          // 0xF14 the fighters that walk now
    u32 m_frame;                            // 0xF24 the frame of the walk
    int m_winner;                           // 0xF28 which of the walking fighters wins the match
    float m_from[4];                        // 0xF2C where each walking fighter starts
    float m_to[4];                          // 0xF3C where each walking fighter ends
    u32 m_frames;                           // 0xF4C how many frames the walk takes
    MuObject* m_plateObjs[0x20];            // 0xF50 the name plate of each fighter
    MuObject* m_faceObjs[0x20];             // 0xFD0 the face of each fighter
    MuObject* m_badgeObjs[0x20];            // 0x1050 the badge of each fighter
    nw4r::g3d::ResFile m_resPlate;          // 0x10D0 the file with the plates
    MuMsg* m_msg;                           // 0x10D4 the names
    nw4r::g3d::ResFile* m_texRes;           // 0x10D8 the file with the faces
    nw4r::g3d::ResFile m_resTex;            // 0x10DC the faces that the table loads itself (when it got none)
    bool m_archiveReady;                    // 0x10E0
    bool m_texReady;                        // 0x10E1
    gfArchive* m_archive;                   // 0x10E4
    gfFileIOHandle m_texFile;               // 0x10E8

    muTourTable(nw4r::g3d::ResFile* texRes, int priority);
    ~muTourTable();

    void setTableData(int playersPerMatch, int sizeIndex);
    void clearTable(int mode);
    bool winnerSet(int slot);
    void setName();
    void moveTable(int round, int node, bool apply);
    void resetTablePos(bool apply);
    bool calcTablePos();
    void entrySet(const int* slots, int count, int frames);
    bool entryCalc();
    void entryEnd();
    void winnerSetIcon(int winner, int frames);
    bool winnerCalc();
    void winnerEnd();
    void finalSet(int frames);
    bool finalCalc();
    void finalEnd();
    bool isLoadEnd();
};
static_assert(sizeof(muTourTable) == 0x10EC, "Class is wrong size!");
