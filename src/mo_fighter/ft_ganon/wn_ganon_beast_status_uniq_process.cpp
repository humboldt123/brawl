#define SO_BUILDER_HELPERS_WEAK
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>
#include <wn/ganon/wn_ganon_beast.h>

wnGanonBeastStatusUniqProcessStart g_wnGanonBeastStatusUniqProcessStart;
wnGanonBeastStatusUniqProcessAttack g_wnGanonBeastStatusUniqProcessAttack;

// The beast appears: the camera range is pushed ahead of it.
void wnGanonBeastStatusUniqProcessStart::initStatus(soModuleAccesser* moduleAccesser) {
    soCameraModule* camera = moduleAccesser->m_enumerationStart->m_cameraModule;
    float lr = moduleAccesser->m_enumerationStart->m_postureModule->getLr();
    Vec3f offset;
    offset.m_x = lr * soValueAccesser::getConstantFloat(moduleAccesser, 4000, 0);
    offset.m_y = soValueAccesser::getConstantFloat(moduleAccesser, 4001, 0);
    offset.m_z = 0.0f;
    camera->addCameraRangeOffset(&offset, 0);
}

void wnGanonBeastStatusUniqProcessStart::execStatus(soModuleAccesser*) { }

// The beast charges across the stage at a constant speed.
void wnGanonBeastStatusUniqProcessAttack::execStatus(soModuleAccesser* moduleAccesser) {
    soPostureModule* posture = moduleAccesser->m_enumerationStart->m_postureModule;
    Vec3f current = posture->getPos();
    Vec3f pos;
    pos = current;
    float lr = posture->getLr();
    float speed = soValueAccesser::getConstantFloat(moduleAccesser, 4002, 0);
    pos.m_x += speed * lr;
    posture->setPos(&pos);
}
