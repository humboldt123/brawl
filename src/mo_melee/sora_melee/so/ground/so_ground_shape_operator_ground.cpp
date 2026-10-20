#include <so/ground/so_ground_shape_operator.h>
#include <so/so_module_accesser.h>

soGroundShapeOperatorGround g_groundShapeOperatorGround;
soGroundShapeOperatorGroundCliffStop g_groundShapeOperatorGroundCliffStop;
soGroundShapeOperatorGroundCliffStopFront g_groundShapeOperatorGroundCliffStopFront;
soGroundShapeOperatorGroundOttotto g_groundShapeOperatorGroundOttotto;

// Shared start of every ground operator's setting(): clear the cliff flags and put the status on the ground.
static inline void settingGround(soGroundShapeImpl* shape) {
    grCollStatus* status = shape->m_collStatus;
    status->m_cliffCheckOn = false;
    status->m_cliffStop = false;
    shape->setFlag(0, true);
    if (status->m_setting != grCollStatus::Setting_None) {
        status->setStGround();
    }
}

void soGroundShapeOperatorGround::init(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) { }

void soGroundShapeOperatorGround::exit(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) { }

void soGroundShapeOperatorGround::setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) {
    settingGround(shape);
}

void soGroundShapeOperatorGround::update(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser, int state) {
    shape->update(moduleAccesser, state);
    shape->enable();
}

void soGroundShapeOperatorGroundCliffStop::setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) {
    settingGround(shape);
    grCollStatus* status = shape->m_collStatus;
    if (moduleAccesser->getGroundModule().getConditionChecker()->isConditionNoCliffStop(moduleAccesser) == true) {
        status->m_cliffStop = false;
    } else {
        status->m_cliffStop = true;
    }
    status->m_cliffSide = 2;
}

void soGroundShapeOperatorGroundCliffStopFront::setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) {
    settingGround(shape);
    grCollStatus* status = shape->m_collStatus;
    if (moduleAccesser->getGroundModule().getConditionChecker()->isConditionNoCliffStop(moduleAccesser) == true) {
        status->m_cliffStop = false;
        status->m_cliffSide = 2;
    } else {
        status->m_cliffStop = true;
        float lr = moduleAccesser->getPostureModule().getLr();
        if (lr == 1.0f) {
            status->m_cliffSide = 1;
        } else if (lr == -1.0f) {
            status->m_cliffSide = 0;
        } else {
            status->m_cliffSide = 2;
        }
    }
}

void soGroundShapeOperatorGroundOttotto::setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) {
    settingGround(shape);
    grCollStatus* status = shape->m_collStatus;
    if (moduleAccesser->getGroundModule().getConditionChecker()->isConditionNoCliffStop(moduleAccesser) == true) {
        status->m_cliffStop = false;
        status->m_cliffSide = 2;
    } else {
        status->m_cliffStop = true;
        float stick = moduleAccesser->getControllerModule().getStickX();
        if (stick > 0.75f) {
            status->m_cliffSide = 1;
        } else if (stick < -0.75f) {
            status->m_cliffSide = 0;
        } else {
            status->m_cliffSide = 2;
        }
    }
}
