#include <types.h>

#include <mu/mu_panel_ctrl.h>

muPanelCtrlTask::muPanelCtrlTask() : gfTask("muPanelCtrl", Category_Menu, 15, 8, true) {
    m_unk44 = 0;
}

muPanelCtrlTask::~muPanelCtrlTask() {
}

void muPanelCtrlTask::processAnim() {
}

void muPanelCtrlTask::setMsg(int msgNo, int unk1, int unk2) {
}

void muPanelCtrlTask::changeProcess(int unk1, int unk2) {
}

void muPanelCtrlTask::changeProcess1(int unk1, int unk2) {
}

void muPanelCtrlTask::frameCancel() {
}

void muPanelCtrlTask::startupSpecialMelee() {
}
