#include <so/ground/so_ground_shape_operator.h>

soGroundShapeOperatorCliff g_groundShapeOperatorCliff;

void soGroundShapeOperatorCliff::init(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) { }

void soGroundShapeOperatorCliff::exit(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) { }

void soGroundShapeOperatorCliff::setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) {
    shape->setFlag(0, true);
    shape->m_collStatus->m_cliffCheckOn = false;
    shape->m_collStatus->m_cliffStop = false;
    if (shape->m_collStatus->m_setting != grCollStatus::Setting_None) {
        shape->m_collStatus->setStCliff();
    }
}

void soGroundShapeOperatorCliff::update(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser, int state) {
    shape->update(moduleAccesser, state);
    shape->enable();
}
