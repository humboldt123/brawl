#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC2F_CTOR_NOINLINE
#define WN_EXTEND_PARAM_ACCESSER_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/dedede/wn_dedede_star.h>
#include <wn/dedede/wn_dedede_follower_param_accesser.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <math.h>

wnDededeStar::wnDededeStar(s32 articleId, const wnArticleBuildInfo& info, const wnDededeStarData* data) :
    wnWeaponBuilder<wnDededeStarModuleAccesserBuildConfig>(
        "DededeStar", articleId, info,
        wnModuleAccesserBuildData(0x31, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param) {
    Weapon::deactivate(false);
}

// The star is thrown from the position at the angle (degrees, mirrored when flipped) with the speed of 1.5 scaled by dir.
void wnDededeStar::activate(int founderTaskId, int unused, int team, const Vec3f* position, bool flip, float dir, float angle) {
    float lr;
    if (flip) {
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000000);
        angle = 180.0f - angle;
        lr = -1.0f;
    } else {
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000000);
        lr = 1.0f;
    }
    wnActivateDesc desc;
    s32 life = m_param->unk0;
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
    desc.life = life;
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
    float sine;
    float speed = 1.5f;
    angle = 0.017453292f * angle;
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    sine = sin(angle);
    float cosine = cos(angle);
    Vec2f velocity(dir * (speed * cosine), speed * sine);
    normal.m_speed.m_x = velocity.m_x;
    normal.m_speed.m_y = velocity.m_y;
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

wnDededeFollowerParamAccesser::wnDededeFollowerParamAccesser(const wnParamDededeFollower* param,
                                                             const wnParamDededeFollowerCommon* common) :
    wnExtendParamAccesser<3999, 27, 23999, 16, 0, 0>(0x33), m_common(common, 3) {
    m_floats[0] = &param->unkC;
    m_floats[1] = &param->unk10;
    m_floats[2] = &param->unk14;
    m_floats[3] = &param->unk18;
    m_floats[4] = &param->unk24;
    m_floats[5] = &param->unk28;
    m_floats[6] = &param->unk30;
    m_floats[7] = &param->unk34;
    m_ints[0] = &param->unk0;
    m_ints[1] = &param->unk4;
    m_ints[2] = &param->unk8;
    m_ints[3] = &param->unk1C;
    m_ints[4] = &param->unk20;
    m_ints[5] = &param->unk2C;
}

// MATCH-ONLY: the native queries call the const element accessor of the table.
#define DEDEDE_FOLLOWER_COMMON(variation)     static_cast<const soArrayContractibleTable<const wnParamDededeFollowerCommon>&>(m_common).at(variation)

float wnDededeFollowerParamAccesser::getParamFloat(soModuleAccesser* acc, s32 id, s32 index) {
    float result;
    if (id < 0xFA8) {
        result = wnExtendParamAccesser<3999, 27, 23999, 16, 0, 0>::getParamFloat(acc, id, index);
    } else {
        u32 variation = acc->getWorkManageModule().getInt(0x10000005);
        switch (id) {
        case 0xFA9:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk0;
            break;
        case 0xFAA:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk4;
            break;
        case 0xFAB:
            result = DEDEDE_FOLLOWER_COMMON(variation).unkC;
            break;
        case 0xFAC:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk24;
            break;
        case 0xFAD:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk28;
            break;
        case 0xFAE:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk2C;
            break;
        case 0xFAF:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk30;
            break;
        case 0xFB0:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk34;
            break;
        case 0xFB1:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk38;
            break;
        case 0xFB2:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk3C;
            break;
        case 0xFB3:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk40;
            break;
        case 0xFB4:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk44;
            break;
        case 0xFB5:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk48;
            break;
        case 0xFB6:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk4C;
            break;
        case 0xFB7:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk50;
            break;
        case 0xFB8:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk54;
            break;
        case 0xFB9:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk5C;
            break;
        case 0xFBA:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk60;
            break;
        default:
            result = 0.0f;
            break;
        }
    }
    return result;
}

s32 wnDededeFollowerParamAccesser::getParamInt(soModuleAccesser* acc, s32 id, s32 index) {
    s32 result;
    if (id < 0x5DC6) {
        result = wnExtendParamAccesser<3999, 27, 23999, 16, 0, 0>::getParamInt(acc, id, index);
    } else {
        u32 variation = acc->getWorkManageModule().getInt(0x10000005);
        switch (id) {
        case 0x5DC7:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk8;
            break;
        case 0x5DC8:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk10;
            break;
        case 0x5DC9:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk14;
            break;
        case 0x5DCA:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk18;
            break;
        case 0x5DCB:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk1C;
            break;
        case 0x5DCC:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk20;
            break;
        case 0x5DCD:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk58;
            break;
        case 0x5DCE:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk64;
            break;
        case 0x5DCF:
            result = DEDEDE_FOLLOWER_COMMON(variation).unk68;
            break;
        default:
            result = 0;
            break;
        }
    }
    return result;
}
