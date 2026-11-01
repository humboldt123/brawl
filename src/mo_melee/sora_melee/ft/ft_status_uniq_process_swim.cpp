#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_util.h>
#include <so/area/so_area_enviroment_element_checker_impl.h>
#include <so/area/so_area_module_impl.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_value_accesser.h>
#include <st/st_area_manager.h>
#include <gf/gf_heap_manager.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftStatusUniqProcessSwim : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSwim() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessSwim g_ftStatusUniqProcessSwim;

// HYPOTHESIS: value stored in work int 0x10000027 when the fighter leaves the water into status 0 or 0xb.
static int s_swimExitWorkValue = 0x1e;

// HYPOTHESIS: the area manager singleton (created on first use) and its water record query.
extern stAreaManager* lbl_27_bss_5780;
extern "C" {
int fn_27_22B3B8(stAreaManager* manager, u32 areaId, stWaterData* water);
}

static inline stAreaManager* getAreaManager() {
    stAreaManager* manager = lbl_27_bss_5780;
    if (manager == NULL) {
        manager = new (Heaps::System) stAreaManager();
        lbl_27_bss_5780 = manager;
    }
    return lbl_27_bss_5780;
}

// HYPOTHESIS: entering the water takes the id of the water area (work 0x20000002) and the time to stay under (0x10000014: the
// constant 0xd1e scales the damage of the fighter into frames).
static void setWaterEntryState(soModuleAccesser* moduleAccesser) {
    int areaId = moduleAccesser->getAreaModule().getEnviromentElementChecker()->getWaterAreaId();
    moduleAccesser->getWorkManageModule().setInt(areaId, 0x20000002);
    int frames = moduleAccesser->getWorkManageModule().getInt(0x10000014);
    if (frames == 0) {
        float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xd1e, 0);
        float damage = moduleAccesser->getDamageModule().getDamage(0);
        moduleAccesser->getWorkManageModule().setInt(frames + static_cast<int>(scale * damage), 0x10000014);
    }
}

void ftStatusUniqProcessSwim::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0xb6: {
        switch (moduleAccesser->getStatusModule().getPrevStatusKind(0)) {
        case 0xb3:
        case 0xb4:
        case 0xb5:
        case 0xb7:
        case 0xb8:
        case 0xb9:
        case 0xba:
        case 0xbb:
        case 0xbc:
            break;
        case 0xb6:
        default:
            setWaterEntryState(moduleAccesser);
            moduleAccesser->getAreaModule().setAreaShapeType(3, static_cast<soAreaInstance::ShapeType>(2));
            break;
        }
        if (moduleAccesser->getSituationModule().getPrevKind() == 0) {
            moduleAccesser->getWorkManageModule().setInt(s_swimExitWorkValue, 0x10000027);
        }
        break;
    }
    case 0xb3: {
        setWaterEntryState(moduleAccesser);
        int areaId = moduleAccesser->getWorkManageModule().getInt(0x20000002);
        stAreaManager* manager = getAreaManager();
        stWaterData water;
        bool found = fn_27_22B3B8(manager, areaId, &water) == 1;
        if (found == true) {
            Vec2f surface;
            if (ftUtil::calcWaterSurfacePos(moduleAccesser, reinterpret_cast<Vec3f*>(&water), &surface) == true) {
                ftUtil::setWaterDiveEffect(moduleAccesser, &Vec3f(surface.m_x, surface.m_y, 0.0f));
            }
        }
        moduleAccesser->getAreaModule().setAreaShapeType(3, static_cast<soAreaInstance::ShapeType>(2));
        switch (moduleAccesser->getStatusModule().getPrevStatusKind(0)) {
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
            break;
        default:
            moduleAccesser->getWorkManageModule().offFlag(0x22000010);
            break;
        }
        break;
    }
    case 0xb4:
        moduleAccesser->getMotionModule().get2ndKind();
        break;
    case 0xbb:
        if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
            Vec3f rot = moduleAccesser->getPostureModule().getRot(0);
            rot.m_y = 180.0f;
            moduleAccesser->getPostureModule().setRot(&rot, 0);
        }
        break;
    case 0xbc: {
        Vec3f pos(0.0f, 0.0f, 0.0f);
        Vec3f rot(0.0f, 0.0f, 0.0f);
        soEffectModule& effect = moduleAccesser->getEffectModule();
        float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xbe9, 0);
        effect.reqFollow(static_cast<EfID>(0x5e), 0x12d, &pos, &rot, scale, false, 0, 0, -1);
        break;
    }
    }
}
// HYPOTHESIS: while swimming at the surface the fighter is pulled to the water surface height of its water area (work
// 0x20000002), carried by the current, counts the frames of the splash effect (0x20000003) and plays the surface effect (0x4f when
// diving, 0x50 while resting) every N frames.
void ftStatusUniqProcessSwim::execStatus(soModuleAccesser* moduleAccesser) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    switch (status) {
    case 0xb5:
    case 0xb6:
    case 0xb7:
    case 0xb8:
    case 0xb9:
    case 0xbb: {
        int areaId = moduleAccesser->getWorkManageModule().getInt(0x20000002);
        stAreaManager* manager = getAreaManager();
        stWaterData water;
        bool found = fn_27_22B3B8(manager, areaId, &water) == 1;
        if (found == true) {
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            pos.m_y = pos.m_y + 0.25f * (moduleAccesser->getMotionModule().getTransNTranslate().m_y + water.m_swimHeight - pos.m_y);
            moduleAccesser->getPostureModule().setPos(&pos);
            if (water.m_currentSpeed != 0.0f) {
                moduleAccesser->getKineticModule().addSpeedOutside(soKineticEnergy::Outside_Wind, &Vec3f(water.m_currentSpeed, 0.0f, 0.0f));
            }
            if (water.m_0xc != 0) {
                moduleAccesser->getWorkManageModule().incInt(0x10000014);
            }
            if (status == 0xb7) {
                Vec2f stick;
                ftUtil::getStickXRate(moduleAccesser, soValueAccesser::getConstantFloat(moduleAccesser, 0xc2d, 0), &stick);
                moduleAccesser->getMotionModule().setRate(stick.m_x);
            }
        }
        if (status == 0xbb) {
            int count = moduleAccesser->getWorkManageModule().getInt(0x20000003) + 1;
            if (count > soValueAccesser::getConstantInt(moduleAccesser, 0x5a4a, 0)) {
                float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xbe9, 0);
                Vec3f pos = moduleAccesser->getPostureModule().getPos();
                pos.m_y = water.m_swimHeight;
                moduleAccesser->getEffectModule().req(static_cast<EfID>(0x50), &pos, &Vec3f(0.0f, 0.0f, 0.0f), scale, 0, -1);
                count = 0;
            }
            moduleAccesser->getWorkManageModule().setInt(count, 0x20000003);
        } else {
            int count = moduleAccesser->getWorkManageModule().getInt(0x20000003) + 1;
            if (count > soValueAccesser::getConstantInt(moduleAccesser, 0x5a47, 0)) {
                areaId = moduleAccesser->getWorkManageModule().getInt(0x20000002);
                manager = getAreaManager();
                stWaterData surface;
                bool surfaceFound = fn_27_22B3B8(manager, areaId, &surface) == 1;
                if (surfaceFound == true) {
                    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xbe9, 0);
                    Vec3f pos = moduleAccesser->getPostureModule().getPos();
                    pos.m_y = surface.m_swimHeight;
                    moduleAccesser->getEffectModule().req(static_cast<EfID>(0x4f), &pos, &Vec3f(0.0f, 0.0f, 0.0f), scale, 0, -1);
                }
                count = 0;
            }
            moduleAccesser->getWorkManageModule().setInt(count, 0x20000003);
        }
        if (moduleAccesser->getGroundModule().isTouch(8, 0)) {
            moduleAccesser->getAreaModule().setAreaShapeType(3, static_cast<soAreaInstance::ShapeType>(3));
        } else {
            moduleAccesser->getAreaModule().setAreaShapeType(3, static_cast<soAreaInstance::ShapeType>(2));
        }
        break;
    }
    }
}

void ftStatusUniqProcessSwim::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0xb5:
    case 0xb6:
    case 0xb7:
    case 0xb8:
    case 0xb9:
    case 0xbb: {
        bool inWater = moduleAccesser->getAreaModule().getEnviromentElementChecker()->isWater();
        int count = moduleAccesser->getWorkManageModule().getInt(0x20000004);
        if (inWater == false) {
            count++;
            if (count >= 3) {
                if (moduleAccesser->getGroundModule().isTouch(8, 0) == true) {
                    moduleAccesser->getStatusModule().changeStatusRequest(0, moduleAccesser);
                } else {
                    moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
                }
            }
        } else {
            count = 0;
        }
        moduleAccesser->getWorkManageModule().setInt(count, 0x20000004);
        break;
    }
    }
}

// HYPOTHESIS: swimming status changes: diving (0xb3) turns into 0xb6/0xb4 depending on the height over the surface, 0xb4 into 0xb5
// when above the surface, and the surface statuses go to the wait status when the fighter touches the ground in shallow water.
void ftStatusUniqProcessSwim::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0xb5:
    case 0xb6:
    case 0xb7:
    case 0xb8:
    case 0xb9:
    case 0xbb: {
        bool touching = moduleAccesser->getGroundModule().isTouch(8, 0);
        bool inWater = moduleAccesser->getAreaModule().getEnviromentElementChecker()->isWater();
        if (inWater == true && touching == true) {
            if (moduleAccesser->getWorkManageModule().getInt(0x10000027) <= 0) {
                Vec3f pos = moduleAccesser->getPostureModule().getPos();
                float surfaceY = moduleAccesser->getAreaModule().getEnviromentElementChecker()->getWaterSurfaceY();
                float scale = moduleAccesser->getPostureModule().getScale();
                float depth = soValueAccesser::getConstantFloat(moduleAccesser, 0xbe5, 0);
                if (surfaceY < pos.m_y + 0.5f * depth * scale) {
                    moduleAccesser->getStatusModule().changeStatusRequest(0, moduleAccesser);
                }
            }
        }
        break;
    }
    case 0xb3: {
        if (moduleAccesser->getGroundModule().isTouch(8, 0)) {
            moduleAccesser->getStatusModule().changeStatusRequest(0xb4, moduleAccesser);
        } else {
            soKineticEnergy::AttributeFlag flag(-1);
            Vec2f total = moduleAccesser->getKineticModule().getSumSpeed(flag);
            if (total.m_y > 0.0f) {
                int areaId = moduleAccesser->getWorkManageModule().getInt(0x20000002);
                stAreaManager* manager = getAreaManager();
                stWaterData water;
                bool found = fn_27_22B3B8(manager, areaId, &water) == 1;
                if (found == true) {
                    Vec3f pos = moduleAccesser->getPostureModule().getPos();
                    if (pos.m_y > moduleAccesser->getMotionModule().getTransNTranslate().m_y + water.m_swimHeight) {
                        moduleAccesser->getStatusModule().changeStatusRequest(0xb6, moduleAccesser);
                    } else {
                        moduleAccesser->getStatusModule().changeStatusRequest(0xb4, moduleAccesser);
                    }
                }
            }
        }
        break;
    }
    case 0xb4: {
        int areaId = moduleAccesser->getWorkManageModule().getInt(0x20000002);
        stAreaManager* manager = getAreaManager();
        stWaterData water;
        bool found = fn_27_22B3B8(manager, areaId, &water) == 1;
        if (found == true) {
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            if (pos.m_y > moduleAccesser->getMotionModule().getTransNTranslate().m_y + water.m_swimHeight) {
                moduleAccesser->getStatusModule().changeStatusRequest(0xb5, moduleAccesser);
            }
        }
        break;
    }
    }
}

void ftStatusUniqProcessSwim::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0xb4:
        moduleAccesser->getMotionModule().remove2ndAnimChr();
        break;
    case 0xb5:
    case 0xb6:
    case 0xb7:
    case 0xb8:
    case 0xb9:
    case 0xba:
        break;
    case 0xbb: {
        Vec3f rot = moduleAccesser->getPostureModule().getRot(0);
        rot.m_y = 0.0f;
        moduleAccesser->getPostureModule().setRot(&rot, 0);
        break;
    }
    case 0xbc:
        moduleAccesser->getEffectModule().killAll(4, true, true);
        break;
    }
    switch (nextStatus) {
    case 0xb3:
    case 0xb4:
    case 0xb5:
    case 0xb6:
    case 0xb7:
    case 0xb8:
    case 0xb9:
    case 0xba:
    case 0xbb:
    case 0xbc:
        break;
    default:
        moduleAccesser->getAreaModule().setAreaShapeType(3, static_cast<soAreaInstance::ShapeType>(3));
        switch (nextStatus) {
        case 0:
        case 0xb:
            moduleAccesser->getWorkManageModule().setInt(s_swimExitWorkValue, 0x10000027);
            break;
        }
        break;
    }
}
