#include <wn/samus/wn_samus_laser2.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <memory.h>

void wnSamusLaser2::activate(int founderTaskId, int etcResourceId, int team, float height) {
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
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, &pos, sizeof(Vec3f));
#else
    desc.pos = pos;
#endif
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagBit7 = 1;
    desc.flagBit6 = 1;
    desc.flagsMidPair = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.setFloat(height, 0x11000001);
    work.onFlag(0x12000000);
}

// HYPOTHESIS: the second laser leaves the model origin raised by its height; a negative height starts it in status 1.
void wnSamusLaser2::toShoot(float lr) {
    soModuleAccesser* acc = m_moduleAccesser;
    Vec3f offset(0.0f, 0.0f, 0.0f);
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0, &offset, false, false);
    float height = acc->getWorkManageModule().getFloat(0x11000001);
    pos.m_z = acc->getGroundModule().getZ();
    pos.m_y += height;
    acc->getPostureModule().init(&pos, lr);
    acc->getLinkModule().removeModelConstraint(true);
    acc->getStatusModule().changeStatusRequest(height < 0.0f ? 1 : 0, acc);
    updateNodeSRT();
}

// HYPOTHESIS: the laser passes through everything while it is out (statuses 0 and 1).
bool wnSamusLaser2::notifyEventCollisionAttackCheck(u32 flags) {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0:
    case 1:
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}
