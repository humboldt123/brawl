#pragma once

// HYPOTHESIS: layout from the constructor, init, the destructor and the step functions; names from the map
// (if_minigame_homerun.o). Parameter types come from the register use of the callee and the IfMngr callers.

#include <StaticAssert.h>
#include <types.h>

class MuObject;
namespace nw4r { namespace g3d { class ResFile; class ScnGroup; class ScnMdlExpand; } }

class IfMinigameHomerun {
public:
    // HYPOTHESIS: one step of the HUD: what runs when the step starts, and what runs every frame (it returns the
    // next step), as in the menu tasks.
    struct StepFunc {
        void (*m_init)(IfMinigameHomerun* homerun);
        int (*m_main)(IfMinigameHomerun* homerun);
    };

    nw4r::g3d::ScnGroup* m_scnGroup;
    MuObject* m_objects[59];
    nw4r::g3d::ScnMdlExpand* m_expands[8];
    int m_step;
    bool unk114; // HYPOTHESIS: set by dispResult; initStepResultAnim then shows object 24
    bool unk115; // HYPOTHESIS: the start animation of object 25 is playing (dispStart sets it, main clears it)
    float m_versusDistances[3]; // HYPOTHESIS: the three distances that dispVersusResult shows

    IfMinigameHomerun();
    void init();
    virtual ~IfMinigameHomerun();
    void createModel(nw4r::g3d::ResFile* resFile);
    // HYPOTHESIS: parameter types as in IfStgedit::createObjResFile
    void createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* mdlSrc, s32 prio);
    void startMelee(nw4r::g3d::ScnGroup* scnGroup);
    // HYPOTHESIS: parameter order of setDistanceWnd, dispHistory and setDistansModel, and the bool of dispResult
    void setDistance(float distance);
    // BYTES: the original calls it out of line from every caller.
    void setDistanceWnd(float distance, int wnd) __attribute__((never_inline));
    void dispHistory(int index, float distance, int color);
    int setDistansModel(nw4r::g3d::ScnMdlExpand* expand, int objectNo, float distance);
    void dispResult(bool arg);
    void dispVersusResult(float distance0, float distance1, float distance2);
    void main();
    void dispStart(int arg1, int arg2);
    static void initStepGame(IfMinigameHomerun* homerun);
    static int mainStepGame(IfMinigameHomerun* homerun);
    static void initStepResultAnim(IfMinigameHomerun* homerun);
    static int mainStepResultAnim(IfMinigameHomerun* homerun);
    static void initStepVersusResultAnim(IfMinigameHomerun* homerun);
    static int mainStepVersusResultAnim(IfMinigameHomerun* homerun);
};
static_assert(sizeof(IfMinigameHomerun) == 0x128, "Class is the wrong size!");
