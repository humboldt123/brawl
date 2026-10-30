#include <wn/snake/wn_snake_rgb6.h>
#include <mt/mt_matrix.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <wn/wn_extend_param_accesser.h>

void wnSnakeRgb6::notifyEventChangeMotion(int motionKind, int prevMotionKind, void*, soModuleAccesser*) {
    if (motionKind == 1) {
        m_moduleAccesser->getLinkModule().removeModelConstraint(true);
        constraint(-1, 10, 0);
    }
    if (prevMotionKind == 1) {
        m_moduleAccesser->getLinkModule().removeModelConstraint(true);
        constraint(-1, 0x39, 1);
    }
}

void wnSnakeRgb6::processFixCamera() {
    Weapon::processFixCamera();
}

Vec3f wnSnakeRgb6::getShootPos() {
    Matrix mtx(true);
    m_moduleAccesser->getModelModule().getNodeGlobalMtx(4, &mtx, false);
    Vec3f pos(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0),
              soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA1, 0),
              soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA2, 0));
    mtx.mulPos(&pos, &pos);
    return pos;
}

template class wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0>;
