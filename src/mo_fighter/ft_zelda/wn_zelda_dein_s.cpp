#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <wn/wn_module_accesser_builder.h>
#include <wn/zelda/wn_zelda_dein_s.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

// The status unique process of the fire (wn_zelda_dein_s_status_uniq_process_bang.cpp).
extern soStatusUniqProcess g_wnZeldaDeinSStatusUniqProcessBang;

wnZeldaDeinS::wnZeldaDeinS(s32 articleId, const wnArticleBuildInfo& info, const wnZeldaDeinSData* data) :
    wnWeaponBuilder<wnZeldaDeinSModuleAccesserBuildConfig>(
        "wnZeldaDeinS", articleId, info,
        wnModuleAccesserBuildData(0x2A, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param) {
    soStatusUniqProcess* uniqProcs[1];
    uniqProcs[0] = &g_wnZeldaDeinSStatusUniqProcessBang;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 1);
    Weapon::deactivate(false);
}

void wnZeldaDeinS::activate(int founderTaskId, int team, const Vec3f* position, float lr, float unk) {
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
    __memcpy(&desc.pos, position, sizeof(Vec3f));
#else
    desc.pos = *position;
#endif
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    soWorkManageModule& workManage = m_moduleAccesser->getWorkManageModule();
    workManage.setFloat(60.0f, 0x21000000);
    workManage.setFloat(0.0f, 0x21000002);
    workManage.setFloat(unk, 0x21000001);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}
