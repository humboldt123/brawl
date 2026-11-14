#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_file_io_handle.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The camera task of the menu that draws the models: it keeps the group of the scene that the models are inserted into.
// HYPOTHESIS: the field names follow how the scene group is reached from the intro.
class muIntroCameraTask : public gfTask {
public:
    u32 m_unk40;
    u32 m_unk44;
    nw4r::g3d::ScnGroup* m_scnRoot;    // 0x48
};
struct muIntroCameraRef {
    muIntroCameraTask* m_task;
};

// The scene of the intro (module sora_scene; only what this menu needs is declared here). The intro shows who fights:
// the players (up to 3) and the enemies (up to 2), each with a character and a kind of entry.
// HYPOTHESIS: the class hierarchy gfScene <- muMenu <- scIntro follows from the run time type information of the module.
class scIntro : public muMenu {
public:
    struct Entry {
        int m_charKind;     // the character (index of the fighter kind)
        int m_type;         // how the character enters (0 .. 3), 4 when there is none
    };

    u8 m_unk350[0xC];
    int m_introNo;                  // 0x35C the number of the intro (the map animation "ItrSimpleMap0000_TopN__%d")
    int m_mode;                     // 0x360 0 and 1 are fights, a larger value is the number of a mini intro (mode - 2)
    int m_playerNum;                // 0x364
    Entry m_players[3];             // 0x368
    int m_enemyNum;                 // 0x380
    Entry m_enemies[2];             // 0x384
    int m_rumbleController;         // 0x394
    muIntroCameraRef* m_camera;     // 0x398
    u8 m_unk39C;
    u8 m_rumbleSetting;             // 0x39D

    scIntro(const char* name) : muMenu(name) { }
};

// The intro of a fight: loads the models of the fighters and the enemies, shows them on the intro map with the sounds
// of the script, and ends the scene when the script is done.
class muIntroTask : public gfTask {
public:
    // One entry of the sound script: the sound and how many frames it takes.
    struct Sound {
        u32 m_id;
        int m_frames;
    };

    static const int ObjNum = 18;
    static const int HandleNum = 8;
    static const int SoundNum = 16;

    nw4r::g3d::ResFile m_resFileCmn;            // 0x40 "cmn.brres" (the map)
    nw4r::g3d::ResFile m_resFileChrCmn;         // 0x44 "chrcmn.brres" (the panels)
    nw4r::g3d::ResFile m_resFileChar[3];        // 0x48 the models of the players
    nw4r::g3d::ResFile m_resFileEnemy[2];       // 0x54 the models of the enemies
    nw4r::g3d::ResFile m_resFileMini;           // 0x5C the file of a mini intro
    MuObject* m_objects[ObjNum];                // 0x60 map, panels, players (4 ...), enemies (16, 17)
    nw4r::g3d::G3dObj* m_scnMdl;                           // 0xA8 the model that the character models are attached to
    int m_introNo;                              // 0xAC copy of the values of the scene
    int m_mode;                                 // 0xB0
    int m_playerNum;                            // 0xB4
    scIntro::Entry m_players[3];                // 0xB8
    int m_enemyNum;                             // 0xD0
    scIntro::Entry m_enemies[2];                // 0xD4
    int m_unkE4;
    muFileIOHandle m_handles[HandleNum];        // 0xE8
    bool m_started;                             // 0x108
    Sound m_sounds[SoundNum];                   // 0x10C
    int m_soundNum;                             // 0x18C
    int m_soundIdx;                             // 0x190
    int m_soundFrame;                           // 0x194

    muIntroTask();
    static muIntroTask* create();

    virtual void processDefault();
    virtual ~muIntroTask();

    void makeSoundScript();
    void getStageSetting();
    void loadCharModel();
    bool isLoadFinished();
    void createCharModel();
    void getEnemyResFileName(char* path, char* anim, char* vis, int charKind, int type);
    void createObjResFile(const struct muIntroModel* models, int count, nw4r::g3d::ResFile* resFile);
    void dispEnemy(int introNo);

    void addSound(u32 id, int frames);
    int getMiniNo() const;
    int getPlayerNum() const;
    int getPlayerType(int index) const;
    int getEnemyNum() const;
    int getEnemyType(int index) const;
    float getVisibleFrame(int index) const;
};
static_assert(sizeof(muIntroTask) == 0x198, "Class is wrong size!");
