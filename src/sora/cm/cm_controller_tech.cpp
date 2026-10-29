#include <cm/cm_controller_tech.h>
#include <types.h>

cmTechController::cmTechController() : gfCameraController() {
    m_target = NULL;
    unkC = 0;
    unkE = 0;
    unk11_80 = false;
    unk11_40 = false;
    unk14 = 100.0f;
    unk54 = 0.0f;
}

cmTechController::~cmTechController() { }

void cmTechController::resetTarget() {
    m_target = NULL;
}
