#include <so/ground/so_ground_shape_operator.h>

soGroundShapeOperatorNone g_groundShapeOperatorNone;

void soGroundShapeOperatorNone::init(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) { shape->resetStatus(); }

void soGroundShapeOperatorNone::exit(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) { shape->resetStatus(); }

void soGroundShapeOperatorNone::setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) { }

void soGroundShapeOperatorNone::update(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser, int state) {
    shape->update(moduleAccesser, state);
    shape->unable();
}
