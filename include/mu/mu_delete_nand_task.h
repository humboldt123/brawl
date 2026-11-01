#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <types.h>

// The task that deletes save data from the Wii system memory for the Delete Data option (muProcOptDel). Its own code is
// still in the uncarved part of sora_menu_main (0x3CC18 ...); the function names come from the sora_menu_main map.
// HYPOTHESIS: only what muProcOptDel uses is declared; the size (0x40) is the one create allocates.
class muDeleteNandTask : public gfTask {
public:
    enum Step {
        Step_Done = 8,              // HYPOTHESIS: the data is deleted (muProcOptDel waits for it)
    };

    // Makes the task (once) and starts deleting the data of the given kind (the entry of muProcOptDel).
    static muDeleteNandTask* create(HeapType heap, int kind);
    // HYPOTHESIS: not static: muProcOptDel loads the task before calling it (the step itself is a static of the unit)
    int getStep();
};
static_assert(sizeof(muDeleteNandTask) == 0x40, "Class is wrong size!");
