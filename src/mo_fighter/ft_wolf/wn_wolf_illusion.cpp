#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#include <types.h>
#include <ft/wolf/ft_wolf_article_pools.h>
#include <wn/wn_activate_desc.h>
#include <wn/fox/wn_fox_illusion_util.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

void wnWolfIllusion::activate(s32 founderTaskId, s32 unk1, s32 team, float lr, s32 unk2, s32 unk3) {
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
    wnFoxIllusionUtil::activate(m_moduleAccesser, static_cast<u8>(unk2), unk3);
}
