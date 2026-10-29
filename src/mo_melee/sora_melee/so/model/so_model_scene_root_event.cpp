#include <so/model/so_model_module_impl.h>
#include <so/model/so_model_scene_root_event.h>
#include <types.h>

soModelSceneRootEvent::~soModelSceneRootEvent() { }

void soModelSceneRootEvent::notify(s32 p1) {
    if (p1 == 1) {
        m_modelModule->preShadowCalcView();
    }
}
