#include <wn/samus/wn_samus_laser.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <memory.h>

// HYPOTHESIS: the laser has no etc resource of its own (0xFFFF) and starts in status 0 with a fresh work int 0x10000006.
void wnSamusLaser::activate(int founderTaskId, int etcResourceId, int team) {
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
    m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000006);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
}

// HYPOTHESIS: the laser leaves the model origin, drops its model constraint and enters status 1.
void wnSamusLaser::toShoot(float lr) {
    Vec3f offset(0.0f, 0.0f, 0.0f);
    Vec3f pos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(0, &offset, false, false);
    pos.m_z = m_moduleAccesser->getGroundModule().getZ();
    m_moduleAccesser->getPostureModule().init(&pos, lr);
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
    updateNodeSRT();
}

// HYPOTHESIS: the laser passes through everything while it is out (statuses 0 and 1).
bool wnSamusLaser::notifyEventCollisionAttackCheck(u32 flags) {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0:
    case 1:
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}
