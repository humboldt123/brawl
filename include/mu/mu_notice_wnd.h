#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The window with a text and a few buttons that the menus open for a notice (for example "the save data is broken").
// HYPOTHESIS: the fields are the ones that the menus of the start of the game read and write; the rest is not known.
class muNoticeWndTask : public gfTask {
public:
    u8 m_unk40[0xC0];
    u8 m_selected;              // 0x100 the button that was chosen
    u8 m_unk101[3];
    int m_step;                 // 0x104 the step of the window (3: it was answered)
    u8 m_unk108[0x18];
    u8 m_unk120;                // 0x120 set by the menus of the start of the game for every window they open
};

// muNoticeWndTask::create(kind, text file, text number, resource file, scene group, unk, default button, heap, ...)
// HYPOTHESIS: the kind is 0 for a window with one button, 1 for a window with a yes and a no button and 3 for a window
// without a button that has to be closed by the program.
extern "C" muNoticeWndTask* fn_800BEEB8(int kind, void* msgData, int msgId, nw4r::g3d::ResFile* resFile, void* scnRoot, int unk14,
                                       int defaultButton, int heap, int heap2, int unk0, int unk1, int unk2);
// muNoticeWndTask::create1(...): the same with a text that is given (the text and its size) instead of a number
extern "C" muNoticeWndTask* fn_800BF048(int kind, const char* text, int size, nw4r::g3d::ResFile* resFile, void* scnRoot, int unk14,
                                       int defaultButton, int heap, int heap2, int unk0, int unk1, int unk2);
// muNoticeWndTask::setStep(step), deleteWnd(), setYesButtonLabel(text file, text number) and setNoButtonLabel(...)
extern "C" void fn_800BF828(muNoticeWndTask* wnd, int step);
extern "C" void fn_800BF830(muNoticeWndTask* wnd);
extern "C" void fn_800BF97C(muNoticeWndTask* wnd, void* msgData, int msgId);
extern "C" void fn_800BFA04(muNoticeWndTask* wnd, void* msgData, int msgId);
