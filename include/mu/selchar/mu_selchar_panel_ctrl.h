#pragma once

#include <StaticAssert.h>
#include <mu/mu_object.h>
#include <mu/mu_panel_ctrl.h>
#include <types.h>

// The balloon of the character select screen: it shows the text of the help in the model of the option, like the balloon of
// the tournament, and can take the text off the model again (detachMsg).
// HYPOTHESIS: the names of the fields come from how the balloon of the tournament uses the same base.
class muSelcharPanelCtrlTask : public muPanelCtrlTask {
public:
    MuObject* m_option;         // 0x4C the model that the text is put on
    bool m_attached;            // 0x50 the text is on the model

    muSelcharPanelCtrlTask(MuObject* option, void* msgData);
    void detachMsg();
    virtual void processAnim();
    virtual ~muSelcharPanelCtrlTask();
    virtual void setMsg(int msgNo, int unk1, int unk2);
    virtual void changeProcess1(int unk1, int unk2);
    virtual void changeProcess(int unk1, int unk2);
    virtual void frameCancel();
};
static_assert(sizeof(muSelcharPanelCtrlTask) == 0x54, "Class is wrong size!");
