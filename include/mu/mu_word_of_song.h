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

// The timing of the whole song: "muWordOfSongParam" in the file mu_word_of_song.dat.
struct muWordOfSongParam {
    u8 m_count;                 // 0x00 how many words there are
    u8 m_unk01;
    u16 m_endFrame;             // 0x02 the frame where the screen fades out
    u16 m_startFrame;           // 0x04 the frame where the first word is placed
};

// One word of the song: "muWordModelParam" in the file mu_word_of_song.dat.
struct muWordModelParam {
    char m_name[0x20];          // 0x00 the name of the model in the file word_of_song.brres
    u16 m_start;                // 0x20 the frames after the start of the song where the word appears
    u16 m_length;               // 0x22 the frames that the word stays
    u16 m_fadeIn;               // 0x24 the frames of the fade in
    u16 m_fadeOut;              // 0x26 the frames of the fade out
};
static_assert(sizeof(muWordModelParam) == 0x28, "Class is wrong size!");

// What the task keeps about a word while the song plays.
struct muWordOfSongEntry {
    u8 m_visible;               // 0x00 the model is in the scene
    int m_start;                // 0x04 the frame where it appears
    int m_end;                  // 0x08 the frame where it is gone
    int m_fadeIn;               // 0x0C the frames of the fade in
    int m_fadeOut;              // 0x10 the frames of the fade out
    float m_fadeInStep;         // 0x14 the alpha that is added in a frame of the fade in
    float m_fadeOutStep;        // 0x18 the alpha that is added in a frame of the fade out (below 0)
    float m_alpha;              // 0x1C
};
static_assert(sizeof(muWordOfSongEntry) == 0x20, "Class is wrong size!");

// The words of the song that are shown over the screen (the song of Super Smash Bros. Brawl: a model for each word that
// fades in and out, in the time that the file says).
class muWordOfSongTask : public gfTask {
public:
    enum State {
        State_Idle = 0,
        State_Start = 1,        // the black screen goes away
        State_Play = 2,         // the words are shown
        State_FadeOut = 3,      // the screen fades to black
        State_End = 4,
    };

    muFadeTask* m_fade;                     // 0x40 HYPOTHESIS: the scene that makes the task sets it
    gfFileIOHandle m_handleScene;           // 0x44 DefaultScene.brres (or DefaultSceneWide.brres)
    gfFileIOHandle m_handleFade;            // 0x48 fade.brres
    gfFileIOHandle m_handleWord;            // 0x4C word_of_song.brres
    gfFileIOHandle m_handleParam;           // 0x50 mu_word_of_song.dat
    nw4r::g3d::ResFile m_resScene;          // 0x54
    nw4r::g3d::ResFile m_resFade;           // 0x58
    nw4r::g3d::ResFile m_resWord;           // 0x5C
    MuObject** m_words;                     // 0x60 the models of the words
    MuObject* m_blackOut;                   // 0x64
    nw4r::g3d::ResAnmScn m_anmScn;          // 0x68 the camera and the light of the scene
    const muWordOfSongParam* m_param;       // 0x6C
    const muWordModelParam* m_models;       // 0x70
    int m_count;                            // 0x74
    muWordOfSongEntry* m_entries;           // 0x78
    u8 m_added;                             // 0x7C the animation and the models are in the scene
    int m_state;                            // 0x80
    int m_frame;                            // 0x84

    muWordOfSongTask(const char* name);
    static muWordOfSongTask* create();

    virtual void processDefault();
    virtual ~muWordOfSongTask();

    void initialize();
    void release();
    void createDataRequest();
    bool isLoading();
    void createData();
    void addModel();
    void removeModel();
    void end();
    void skip();
    void setAlpha(int index, float alpha);
};
static_assert(sizeof(muWordOfSongTask) == 0x88, "Class is wrong size!");
