#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_STATUS_PLAIN_NULL_ARRAY
#define FT_REL_LINK_EXTERN
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/lucas/wn_lucas_pk_thunder_child.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>

// The status unique process of the bolt (wn_lucas_pk_thunder_child_status_uniq_process_move.cpp).
extern soStatusUniqProcess g_wnLucasPKThunderChildStatusUniqProcessMove;

wnLucasPKThunderChild::wnLucasPKThunderChild(s32 articleId, const wnArticleBuildInfo& info, const wnLucasPKThunderChildData* data) :
    wnWeaponBuilder<wnLucasPKThunderChildModuleAccesserBuildConfig>(
        "LucasPKThunderChild", articleId, info,
        wnModuleAccesserBuildData(80, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[1];
    uniqProcs[0] = &g_wnLucasPKThunderChildStatusUniqProcessMove;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 1);
    Weapon::deactivate(false);
}

void wnLucasPKThunderChild::activate(int founderTaskId, int linkTaskId, int team, int kind) {
    {
        Vec3f origin(0.0f, 0.0f, 0.0f);
        wnActivateDesc desc;
        desc.founderTaskId = founderTaskId;
        desc.resourceId = 0xFFFF;
        desc.unk8 = 0xFFFF;
        desc.unkC = 0xFFFF;
        desc.unk10 = -1;
        desc.unk14 = -1;
        desc.unk18 = 0;
        desc.unk1C = 0;
        __memcpy(&desc.pos, &origin, sizeof(Vec3f));
        desc.lr = 1.0f;
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
        desc.unk4DHigh = 0;
        desc.flagsLow = 0;
        Weapon::activate(&desc);
    }
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    soStatusModule& status = m_moduleAccesser->getStatusModule();
    soLinkModule& link = m_moduleAccesser->getLinkModule();
    if (link.link(5, linkTaskId)) {
        u32 node = 0;
        Vec3f parentPos = link.getParentModelNodeGlobalPosition(5u, node, true);
        posture.setPos(&parentPos);
        work.setFloat(parentPos.m_x, 0x21000000);
        work.setFloat(parentPos.m_y, 0x21000001);
        work.setFloat(parentPos.m_x, 0x21000002);
        work.setFloat(parentPos.m_y, 0x21000003);
        if (kind == 0) {
            m_moduleAccesser->getSoundModule().playStatusSE(static_cast<SndID>(0x144d), true, 0);
        }
        soEffectModule& effect = m_moduleAccesser->getEffectModule();
        EfID effectId = static_cast<EfID>(((kind == 5) ? 1 : 0) + 0x1b0006);
        effect.reqFollow(effectId, 0, &Vec3f(0.0f, 0.0f, 4.0f), &Vec3f(0.0f, 0.0f, 0.0f), 2.0f, false, 0, 0, -1);
        updateNodeSRT();
        link.setAttribute(5, static_cast<soLinkConnection::Attribute>(0xd), true);
    }
    status.changeStatusForce(0, m_moduleAccesser);
}

bool wnLucasPKThunderChild::notifyEventCollisionAttackCheck(u32 flags) {
    return false;
}

void wnLucasPKThunderChild::updateNodeSRT() {
    Vec3f scale;
    Vec3f rot;
    Vec3f pos;
    float s = m_moduleAccesser->getPostureModule().getScale();
    scale.m_z = s;
    scale.m_y = s;
    scale.m_x = s;
    pos = m_moduleAccesser->getPostureModule().getPos();
    rot = m_moduleAccesser->getPostureModule().getRot(0);
    rot.m_y += m_moduleAccesser->getPostureModule().getRotYLr();
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rot, &pos);
}
