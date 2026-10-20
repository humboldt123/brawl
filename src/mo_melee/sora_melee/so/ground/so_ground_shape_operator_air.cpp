#include <so/ground/so_ground_shape_operator.h>
#include <so/so_module_accesser.h>

soGroundShapeOperatorAir g_groundShapeOperatorAir;
soGroundShapeOperatorAirPassable g_groundShapeOperatorAirPassable;

void soGroundShapeOperatorAir::init(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) {
    if (shape->m_collStatus->m_setting != grCollStatus::Setting_None) {
        shape->m_collStatus->setStAir();
    }
}

void soGroundShapeOperatorAir::exit(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) { }

void soGroundShapeOperatorAir::setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) {
    if (shape->m_collStatus->m_setting != grCollStatus::Setting_None) {
        shape->m_collStatus->setStAir();
    }
    shape->m_collStatus->m_cliffCheckOn = false;
    shape->m_collStatus->m_cliffStop = false;
    shape->setFlag(0, false);
    u8 cliffCheck = shape->m_cliffCheck;
    switch (cliffCheck) {
    case soGroundShapeImpl::Cliff_Check_Always:
    case soGroundShapeImpl::Cliff_Check_Always_Both_Sides:
        if (cliffCheck == soGroundShapeImpl::Cliff_Check_Always) {
            shape->m_collStatus->m_cliffCheckSides = 0;
        } else {
            if (moduleAccesser->getControllerModule().getStickX() > 0.75f) {
                shape->m_collStatus->m_cliffSide = 1;
            } else if (moduleAccesser->getControllerModule().getStickX() < -0.75f) {
                shape->m_collStatus->m_cliffSide = 0;
            } else {
                shape->m_collStatus->m_cliffSide = 2;
            }
            shape->m_collStatus->m_cliffCheckSides = 2;
        }
        shape->m_collStatus->m_cliffCheckOn = true;
        break;
    case soGroundShapeImpl::Cliff_Check_On_Drop:
    case soGroundShapeImpl::Cliff_Check_On_Drop_Both_Sides: {
        Vec3f pos = moduleAccesser->getPostureModule().getPos();
        Vec3f prevPos = moduleAccesser->getPostureModule().getPrevPos();
        if (pos.m_y < prevPos.m_y) {
            if (cliffCheck == soGroundShapeImpl::Cliff_Check_On_Drop) {
                shape->m_collStatus->m_cliffCheckSides = 0;
            } else {
                if (moduleAccesser->getControllerModule().getStickX() > 0.75f) {
                    shape->m_collStatus->m_cliffSide = 1;
                } else if (moduleAccesser->getControllerModule().getStickX() < -0.75f) {
                    shape->m_collStatus->m_cliffSide = 0;
                } else {
                    shape->m_collStatus->m_cliffSide = 2;
                }
                shape->m_collStatus->m_cliffCheckSides = 2;
            }
            shape->m_collStatus->m_cliffCheckOn = true;
        }
        break;
    }
    }
}

void soGroundShapeOperatorAir::update(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser, int state) {
    shape->update(moduleAccesser, state);
    shape->enable();
}

void soGroundShapeOperatorAirPassable::setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) {
    soGroundShapeOperatorAir::setting(shape, moduleAccesser);
    shape->m_collStatus->setPassableCheck(false);
}
