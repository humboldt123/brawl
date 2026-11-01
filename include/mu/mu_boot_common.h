#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_task.h>
#include <mv/mv_movie_player.h>
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

