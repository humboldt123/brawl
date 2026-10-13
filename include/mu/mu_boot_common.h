#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_task.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The task that the menu tasks of the boot and title screens make to hold their models in a scene group.
// HYPOTHESIS: only the group is known (main.dol makes the task from the archive; the tasks call exit() on it when they end).
class muModelGroupTask : public gfTask {
public:
    char m_unk40[8];
    nw4r::g3d::ScnGroup* m_group;       // 0x48
};

// main.dol: makes the task that holds the scene group of an archive (the name is the one of the task that asks for it).
extern "C" muModelGroupTask* fn_800AFBFC(const char* name, int unk, gfArchive* archive, int unk2);

namespace nw4r {
namespace g3d {
class ScnMdl;
}
}

// The player of the movies (THP files) that the opening and the how to play screen use.
// HYPOTHESIS: the class and the names of its functions are guessed from what the tasks do with them; main.dol has the code.
class muMovie;
extern "C" muMovie* fn_8007E99C(muMovie* movie);                          // constructor
extern "C" void fn_8007EA78(muMovie* movie, int freeIt);                // destructor
extern "C" void fn_8007EC08(muMovie* movie, const char* path, int unk1, int unk2, int unk3);    // opens the file
extern "C" int fn_8007EF64(muMovie* movie);                             // 1 when the movie is ready to play
extern "C" int fn_8007EF68(muMovie* movie);                             // how many frames the movie has
extern "C" int fn_8007EF70(muMovie* movie, int unk1, int unk2, int unk3, int unk4);   // plays a frame, returns the frame
extern "C" void fn_8007F148(muMovie* movie);                            // starts the movie
extern "C" void fn_8007F2B4(muMovie* movie, nw4r::g3d::ScnMdl* model, int unk);   // sets the model that shows the movie
