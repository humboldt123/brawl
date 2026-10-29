// The full soTurnModuleImpl layout (see em/turn/em_turn_module_impl.h).
#define EM_TURN_MODULE_IMPL_FULL
#define SO_TURN_MODULE_IMPL_FULL
// The anim cmd argument list destructor is the one of sora_melee.
#define SO_ARRAY_CONTRACTIBLE_TABLE_DTOR_EXTERN
// MATCH-ONLY: like the Enemy unit, the constructor calls the sora_melee copies of the event observer constructor and
// of soEventObserver::initialize.
#define EM_ENEMY_BUILDER
// MATCH-ONLY: the soAnimCmdEventObserver destructor (a weak copy elsewhere in sora_enemy) calls the sora_melee
// instance of the observer template's destructor; an inline template destructor would also emit weak copies of the
// template's vtable and addObserver, which the original unit does not have.
#define SO_EVENT_OBSERVER_DTOR_EXTERN
// (First: the full soTurnModuleImpl must come before the opaque one of ft/builder/ft_dol_types.h, which it disables.)
#include <so/turn/so_turn_module_impl.h>
#include <em/enemy.h>
#include <em/turn/em_turn_module_impl.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/event/so_event_manage_module_impl.h>
#include <so/event/so_event_system.h>
#include <so/so_module_accesser.h>
#include <types.h>

static const u32 WorkFlag_Front = 0x12000003; // the enemy faces the camera (see Enemy::getEmTopNRot)

// The argument at index (a virtual call through the array interface, as in the Enemy unit).
static inline acCmdArg getCmdArg(const soArrayFixed<const acCmdArgConv>& args, s32 index) {
    return acCmdArg(&args.at(index));
}

emTurnModuleImpl::emTurnModuleImpl(soModuleAccesser* acc)
    : soTurnModuleImpl(acc),
      soAnimCmdEventObserver(static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(), static_cast<s8>(-1)) {
    m_energyId = -1;
    m_rotNodeId = 0;
}

emTurnModuleImpl::~emTurnModuleImpl() { }

void emTurnModuleImpl::activate() {
    soTurnModuleImpl::activate();
    m_energyId = -1;
    m_rotNodeId = 0;
}

void emTurnModuleImpl::start(int energyId, u32 rotNodeId, bool changeLr, float lr) {
    if (rotNodeId == -1) {
        return;
    }
    soTurnModuleImpl::set(NULL, changeLr, false, lr);
    m_energyId = energyId;
    m_rotNodeId = rotNodeId;
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    Enemy& enemy = dynamic_cast<Enemy&>(m_moduleAccesser->getStageObject());
    Vec3f rot = enemy.getEmTopNRot(posture.getRotYLr(), m_moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Front));
    m_rotY = rot.m_y;
}

void emTurnModuleImpl::end() {
    if (m_state != 0) {
        Enemy& enemy = dynamic_cast<Enemy&>(m_moduleAccesser->getStageObject());
        enemy.initRot(m_rotNodeId);
    }
    m_state = 0;
    m_rotY = unk30 = 0.0f;
}

void emTurnModuleImpl::updateSubReverseLr(emKineticModuleImpl* kinetic, soPostureModule* posture) {
    posture->reverseLr();
    for (int i = 0; i < 8; i++) {
        if (i == 0) {
            continue;
        }
        emKineticEnergy* energy = kinetic->getEmEnergy(i);
        if (energy->isInvalidEnergy() == 0) {
            energy->reverseLr();
        }
    }
}

void emTurnModuleImpl::update(bool active) {
    if (isTurn() == false) {
        return;
    }
    if (active == false) {
        return;
    }
    emKineticModuleImpl& kinetic = dynamic_cast<emKineticModuleImpl&>(m_moduleAccesser->getKineticModule());
    kinetic.updateEnergyAsId(m_energyId);
    kinetic.updateEnergyPost(m_energyId);
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    Enemy& enemy = dynamic_cast<Enemy&>(m_moduleAccesser->getStageObject());
    Vec3f rot = enemy.getEmTopNRot(posture.getRotYLr(), m_moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Front));
    if (m_state == 1) {
        // The rotation crossed 0: the enemy now faces the other way.
        if ((rot.m_y >= 0.0f && m_rotY < 0.0f) || (rot.m_y <= 0.0f && m_rotY > 0.0f)) {
            if (m_changeLr == true && !posture.isRotYLrDifferentInnerLr()) {
                m_lrReversed = true;
                updateSubReverseLr(&kinetic, &posture);
            }
            m_state = 2;
        }
    }
    m_rotY = rot.m_y;
    if (kinetic.isEnergyEnd(m_energyId) == true) {
        posture.updateRotYLr();
        if (m_changeLr == true && !m_lrReversed) {
            updateSubReverseLr(&kinetic, &posture);
        }
        m_moduleAccesser->getDamageModule().leaveTurnDamage();
        emTurnModuleImpl::end();
        posture.fixRot();
    }
}

bool emTurnModuleImpl::isObserv(char group) {
    bool observ = false;
    if (soTurnModuleImpl::isObserv(group) == true || group == 'l') {
        observ = true;
    }
    return observ;
}

bool emTurnModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    if (acmd->getGroup() != 'l') {
        return soTurnModuleImpl::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
    }
    s8 type = acmd->getType();
    s32 argNum = acmd->getArgNum();
    if (type <= -1 || type >= 2) {
        return soTurnModuleImpl::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
    }
    switch (type) {
    case 0:
    case 1: {
        // Start a turn: (energy id, node id (0) or node name (1)[, change lr]).
        if (argNum < 2 || argNum > 3) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int energyId = getCmdArg(args, 0).getIntData();
        u32 rotNodeId = -1;
        if (type == 0) {
            rotNodeId = getCmdArg(args, 1).getIntData();
        } else if (type == 1) {
            soModelModule& model = moduleAccesser->getModelModule();
            rotNodeId = model.getNodeId(getCmdArg(args, 1).getStringData());
        }
        bool changeLr = true;
        if (argNum == 3) {
            changeLr = getCmdArg(args, 2).getBoolData();
        }
        start(energyId, rotNodeId, changeLr, m_moduleAccesser->getPostureModule().getLr());
        return true;
    }
    }
    return false;
}
