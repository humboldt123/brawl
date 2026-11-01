#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#include <types.h>
#include <ft/pit/ft_pit_article_pools.h>
#include <st/stage.h>
#include <so/so_module_accesser.h>
#include <wn/wn_activate_desc.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

// HYPOTHESIS: the army appears at kirifuda point 6 of the stage, scaled by the kirifuda scale of that point.
void wnPitParthena::activate(int founderTaskId, int resourceId, int team, float scale) {
    Vec3f position;
    g_Stage->getKirifudaPos(&position, 6);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = position;
    desc.lr = 1.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.unk4DHigh = 0;
    desc.flagsLowNibble = 0;
    desc.unk38 = 0;
    desc.unk3C = 0xFF;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setFloat(scale, 0x11000001);
    Vec3f kirifudaScale;
    g_Stage->getKirifudaScale(&kirifudaScale, 6);
    m_moduleAccesser->getPostureModule().setScale(kirifudaScale.m_x);
    if (g_Stage->getKirifudaModelType(6)) {
        m_moduleAccesser->getVisibilityModule().setStatusDefault(0, 1, true);
    }
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}
