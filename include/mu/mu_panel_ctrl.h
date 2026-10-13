#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <mu/mu_msg.h>
#include <types.h>

// The balloon of the scene that shows the text of the help of the entry the cursor is on. It is a task of the scene, so the
// functions that do nothing here are the ones that the other balloons of the scene use.
// HYPOTHESIS: the class has more fields (the size is a guess from the constructor).
class muPanelCtrlTask : public gfTask {
public:
    int m_unk40;            // 0x40
    int m_unk44;            // 0x44 cleared by the constructor
    MuMsg* m_msg;           // 0x48

    muPanelCtrlTask();
    virtual void processAnim();
    virtual ~muPanelCtrlTask();
    virtual void setMsg(int msgNo, int unk1, int unk2);
    virtual void clearMsg() { m_msg->printf(0, ""); }
    virtual void setMsgData(void* data) { m_msg->setMsgData(data); }
    virtual void* getMsgData() { return *reinterpret_cast<void**>(reinterpret_cast<char*>(m_msg) + 4); }
    virtual void changeProcess1(int unk1, int unk2);
    virtual void changeProcess(int unk1, int unk2);
    virtual void frameCancel();
    virtual void startupSpecialMelee();
};
static_assert(sizeof(muPanelCtrlTask) == 0x4C, "Class is wrong size!");
