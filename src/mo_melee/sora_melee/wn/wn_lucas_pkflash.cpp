#define FT_MODULE_BUILDER
#define WN_PARAM_ACCESSER_INLINE_DTOR
#define SO_NULLABLE_INLINE_CTOR
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#define WN_STATUS_PLAIN_NULL_ARRAY
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <math.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/lucas/wn_lucas_pkflash.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <memory.h>

// Registers the Lucas PK Flash article class (kind 77) in the class info table during static initialization.
wnClassInfoImpl<77, wnLucasPKFlash, wnLucasPKFlashData> g_wnLucasPKFlashClassInfo;

extern soStatusUniqProcess g_wnLucasPKFlashStatusUniqProcessMove;
extern soStatusUniqProcess g_wnLucasPKFlashStatusUniqProcessBang;

wnLucasPKFlash::wnLucasPKFlash(s32 articleId, const wnArticleBuildInfo& info, const wnLucasPKFlashData* data) :
    wnWeaponBuilder<wnLucasPKFlashModuleAccesserBuildConfig>(
        "LucasPKFlash", articleId, info,
        wnModuleAccesserBuildData(77, data, info.getHeapModule()->getInstanceHeap(),
            info.getHeapModule()->getNWModelInstanceHeap(), info.getHeapModule()->getNWMotionInstanceHeap(),
            static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param), m_energyGravity() {
    soStatusUniqProcess* uniqProcs[4];
    uniqProcs[0] = &g_wnLucasPKFlashStatusUniqProcessMove;
    uniqProcs[1] = NULL;
    uniqProcs[2] = &g_wnLucasPKFlashStatusUniqProcessBang;
    uniqProcs[3] = NULL;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 4);
    m_moduleAccesser->getKineticModule().addEnergy(&m_energyGravity, 2, soKineticEnergy::AttributeFlag(1), -1);
    Weapon::deactivate(false);
}

void wnLucasPKFlash::activate(int founderTaskId, int team, const Vec3f* pos, float lr) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, pos, sizeof(Vec3f));
#else
    desc.pos = *pos;
#endif
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(4), true);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = m_moduleAccesser->getKineticModule();
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
    wnKineticEnergyGravity& gravity = dynamic_cast<wnKineticEnergyGravity&>(*kinetic.getEnergy(2));
    float angle = 1.5707964f + 0.017453292f * m_param->m_launchAngle * lr;
    soEffectModule& effect = m_moduleAccesser->getEffectModule();
    work.setInt(effect.reqFollow(static_cast<EfID>(0x1B000C), 0, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f,
                                 false, 0, 0, -1),
                0x20000000);
    Vec2f speed;
    speed.m_x = -m_param->m_launchSpeed * cosf(angle);
    speed.m_y = m_param->m_launchSpeed * sinf(angle);
    normal.m_speed = Vec2f(speed.m_x, 0.0f);
    normal.m_accel = Vec2f(0.0f, 0.0f);
    normal.m_brake = Vec2f(0.0f, 0.0f);
    normal.m_speedLimit = Vec2f(m_param->m_speedLimitX, 0.0f);
    normal.m_speedTarget = Vec2f(m_param->m_speedLimitX, 0.0f);
    normal.enable();
    gravity.m_speedY = speed.m_y;
    gravity.m_gravity = -m_param->m_gravity;
    gravity.m_speedLimit = m_param->m_speedLimitY;
    gravity.enable();
}

void wnLucasPKFlash::onDeactivate() {
    if (m_moduleAccesser->getLinkModule().isLink(3) == true) {
        m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(4), false);
    }
}

void wnLucasPKFlash::shoot() {
    m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
}
