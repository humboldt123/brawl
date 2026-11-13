#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/gamewatch/ft_gamewatch_status_uniq_process_final_swim.h>
#include <so/area/so_area_enviroment_element_checker_impl.h>
#include <so/area/so_area_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <st/st_area_manager.h>
#include <types.h>

// Mr. Game & Watch's Final Smash (Octopus) in water: 0x12d sinks to the water surface, 0x12e rises to it and 0x12f..0x136
// follow it. The water is tested at model node 0xe of the fighter.

bool ftGameWatchStatusUniqProcessFinalSwim::getWaterAreaInfo(soModuleAccesser* moduleAccesser, stWaterData* water) {
    Vec3f pos = moduleAccesser->getModelModule().getNodeGlobalPosition(0xe, false);
    float surfaceY;
    return moduleAccesser->getAreaModule().getEnviromentElementChecker()->testWater(reinterpret_cast<Vec2f*>(&pos), &surfaceY);
}

// The height the fighter is held at: the water surface plus the motion's TransN offset, 20 units down.
float ftGameWatchStatusUniqProcessFinalSwim::getWaterSurfaceOffsetPos(soModuleAccesser* moduleAccesser, stWaterData* water) {
    Vec3f pos = moduleAccesser->getModelModule().getNodeGlobalPosition(0xe, false);
    float surfaceY;
    if (moduleAccesser->getAreaModule().getEnviromentElementChecker()->testWater(reinterpret_cast<Vec2f*>(&pos), &surfaceY)) {
        Vec3f trans = moduleAccesser->getMotionModule().getTransNTranslate();
        return surfaceY + trans.m_y - 20.0f;
    }
    return 0.0f;
}

bool ftGameWatchStatusUniqProcessFinalSwim::calcWaterSurfacePos(soModuleAccesser* moduleAccesser, stWaterData* water,
                                                                Vec2f* surface) {
    Vec3f pos = moduleAccesser->getModelModule().getNodeGlobalPosition(0xe, false);
    float surfaceY;
    if (moduleAccesser->getAreaModule().getEnviromentElementChecker()->testWater(reinterpret_cast<Vec2f*>(&pos), &surfaceY)) {
        float y = surfaceY;
        surface->m_x = pos.m_x;
        surface->m_y = y;
        return true;
    }
    return false;
}

// Entering (0x12d) stores the water area id (work 0x20000002) and plays the dive splash (effect 8) at the surface.
void ftGameWatchStatusUniqProcessFinalSwim::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x12e:
        break;
    case 0x12f:
        moduleAccesser->getStatusModule().getPrevStatusKind(0);
        break;
    case 0x12d: {
        int areaId = moduleAccesser->getAreaModule().getEnviromentElementChecker()->getWaterAreaId();
        moduleAccesser->getWorkManageModule().setInt(areaId, 0x20000002);
        stWaterData water;
        if (getWaterAreaInfo(moduleAccesser, &water) == true) {
            Vec2f surface;
            if (calcWaterSurfacePos(moduleAccesser, &water, &surface) == true) {
                Vec3f pos;
                Vec3f rot;
                rot.m_x = 0.0f;
                rot.m_y = 0.0f;
                rot.m_z = 0.0f;
                pos.m_x = surface.m_x;
                pos.m_y = surface.m_y;
                pos.m_z = 0.0f;
                moduleAccesser->getEffectModule().req(static_cast<EfID>(8), &pos, &rot, 1.0f, 0, -1);
            }
        }
        moduleAccesser->getAreaModule().setAreaShapeType(3, static_cast<soAreaInstance::ShapeType>(2));
        break;
    }
    }
}

// While swimming the fighter is pulled a quarter of the way to the held height each frame.
void ftGameWatchStatusUniqProcessFinalSwim::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x12f:
    case 0x130:
    case 0x131:
    case 0x132:
    case 0x133:
    case 0x134:
    case 0x135:
    case 0x136: {
        stWaterData water;
        if (getWaterAreaInfo(moduleAccesser, &water) == true) {
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            pos.m_y = pos.m_y + 0.25f * (getWaterSurfaceOffsetPos(moduleAccesser, &water) - pos.m_y);
            moduleAccesser->getPostureModule().setPos(&pos);
        }
        break;
    }
    }
}

// 0x12d goes to 0x12e once it sinks below the held height, 0x12e to 0x12f once it rises above it.
void ftGameWatchStatusUniqProcessFinalSwim::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x12d: {
        stWaterData water;
        if (getWaterAreaInfo(moduleAccesser, &water) == true) {
            float height = getWaterSurfaceOffsetPos(moduleAccesser, &water);
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            if (pos.m_y < height) {
                moduleAccesser->getStatusModule().changeStatusRequest(0x12e, moduleAccesser);
            }
        }
        break;
    }
    case 0x12e: {
        stWaterData water;
        if (getWaterAreaInfo(moduleAccesser, &water) == true) {
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            if (pos.m_y > getWaterSurfaceOffsetPos(moduleAccesser, &water)) {
                moduleAccesser->getStatusModule().changeStatusRequest(0x12f, moduleAccesser);
            }
        }
        break;
    }
    }
}

// Leaving the Final Smash statuses (0x12d..0x137) clears its work flag 0x12000041.
void ftGameWatchStatusUniqProcessFinalSwim::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x12d:
    case 0x12e:
    case 0x12f:
    case 0x130:
    case 0x131:
    case 0x132:
    case 0x133:
    case 0x134:
    case 0x135:
    case 0x136:
    case 0x137:
        break;
    default:
        moduleAccesser->getWorkManageModule().offFlag(0x12000041);
        break;
    }
}

ftGameWatchStatusUniqProcessFinalSwim g_ftGameWatchStatusUniqProcessFinalSwim;
