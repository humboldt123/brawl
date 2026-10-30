#include <wn/samus/wn_samus_supermissile.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <math.h>
#include <memory.h>

// HYPOTHESIS: parameter names; the same activation descriptor as the other projectile articles.
void wnSamusSuperMissile::activate(int founderTaskId, int etcResourceId, int team) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = etcResourceId;
    desc.unk8 = etcResourceId;
    desc.unkC = etcResourceId;
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
    changeMotion(0, false);
}

const wnSamusSuperMissileData* wnSamusSuperMissile::getData() {
    return static_cast<const wnSamusSuperMissileData*>(m_data);
}

// HYPOTHESIS: the super missile leaves the model origin, offset along x by the speed on the facing side, and flies at work angle 0 or pi.
void wnSamusSuperMissile::toShoot(float lr, float speed) {
    soModuleAccesser* acc = m_moduleAccesser;
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0, false);
    pos.m_z = acc->getGroundModule().getZ();
    pos.m_x += speed * lr;
    acc->getPostureModule().init(&pos, lr);
    removeConstraint();
    acc->getWorkManageModule().setFloat(lr > 0.0f ? 0.0f : 3.1415927f, 0x11000001);
    updateNodeSRT();
    acc->getStatusModule().changeStatusForce(0, acc);
}

// HYPOTHESIS: while flying (status 0) the model points along the work angle 0x11000001.
void wnSamusSuperMissile::updateNodeSRT() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0: {
        float scale = m_moduleAccesser->getPostureModule().getScale();
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        m_moduleAccesser->getPostureModule().getLr();
        float angle = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
        Vec3f rot(57.295776f * angle, 90.0f, 0.0f);
        Vec3f scaleVec(scale, scale, scale);
        m_moduleAccesser->getModelModule().setNodeSRT(0, &scaleVec, &rot, &pos);
        break;
    }
    default:
        StageObject::updateNodeSRT();
        break;
    }
}

bool wnSamusSuperMissile::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index) {
    if (Weapon::notifyEventAnimCmd(cmd, moduleAccesser, index)) {
        return true;
    }
    s8 group = cmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (cmd->getType() <= -1 || cmd->getType() >= 11) {
        return false;
    }
    return true;
}

// HYPOTHESIS: while flying, a hit or a clash ends the flight (status 1); a shield ends it too unless it asked for a hop.
bool wnSamusSuperMissile::notifyEventCollisionAttackCheck(u32 flags) {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0:
        if (flags & 0x13) {
            m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
            return false;
        }
        if ((flags & 4) && unkA4Bit7 != 1) {
            m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
            return false;
        }
        break;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}
