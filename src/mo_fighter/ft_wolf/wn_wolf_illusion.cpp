#define WN_STATUS_NO_TAIL // the null kinetic builder follows the status builder directly
#define WN_WEAPON_NO_COMMON_ENERGIES // no movement energies
#define IP_INPUT_BUTTON_RAW
#define WN_ARTICLE_ANIMCMD_INT
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/wolf/wn_wolf_illusion.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/fox/wn_fox_illusion_util.h>

wnWolfIllusion::wnWolfIllusion(s32 articleId, const wnArticleBuildInfo& info, const wnWolfIllusionData* data)
    : wnWeaponBuilder<wnWolfIllusionModuleAccesserBuildConfig>(
          "WolfIllusion", articleId, info,
          wnModuleAccesserBuildData(40, data,
              info.getHeapModule()->getInstanceHeap(),
              info.getHeapModule()->getNWModelInstanceHeap(),
              info.getHeapModule()->getNWMotionInstanceHeap(),
              static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser,
              0, 0, 0, 0, 0, 0, 0)),
      m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

void wnWolfIllusion::processUpdate() {
    wnFoxIllusionUtil::processUpdate(m_moduleAccesser);
    Weapon::processUpdate();
}

void wnWolfIllusion::activate(s32 founderTaskId, s32 unk1, s32 team, u8 situationKind, float lr, bool followUp) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = pos;
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
    desc.unk4D = (desc.unk4D & ~0xC0) | 0x80;
    Weapon::activate(&desc);
    wnFoxIllusionUtil::activate(m_moduleAccesser, situationKind, followUp);
}

bool wnWolfIllusion::notifyEventCollisionAttackCheck(u32) { return false; }
