#define FT_MODULE_BUILDER
#include <so/anim/so_anim_cmd_module_impl.h>
#include <types.h>

void soAnimCmdInterpreter::interpretCmd(soModuleAccesser* moduleAccesser, float timeStep) {
    m_moduleAccesser = moduleAccesser;
    unk4C = 0;
    acCmdInterpreter::interpretCmd(timeStep);
}

bool soAnimCmdInterpreter::isSleepStackData(s32 kind) const {
    if (acCmdInterpreter::isSleepStackData(kind) == true) {
        return true;
    }
    return kind == 8 || kind == 9;
}
