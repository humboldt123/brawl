#define WN_ARTICLE_ANIMCMD_INT
#define WN_DAMAGE_ACTOR_INTERFACE
#define WN_WEAPON_BUILDER
#define FT_REL_LINK_EXTERN
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#define WN_KINETIC_GENERIC_BUILDER
#include <wn/wn_module_accesser_builder.h>
#include <wn/diddy/wn_diddy_barrel.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <mt/mt_prng.h>

static wnDiddyBarrelStatusUniqProcess g_wnDiddyBarrelStatusUniqProcess;

wnDiddyBarrelStatusUniqProcess* wnDiddyBarrelStatusUniqProcess::getInstance() {
    return &g_wnDiddyBarrelStatusUniqProcess;
}

wnDiddyBarrelStatusUniqProcess::~wnDiddyBarrelStatusUniqProcess() { }

void wnDiddyBarrelStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soParamAccesser* params = (soParamAccesser*)work.getParamAccesser();
    int status = moduleAccesser->getStatusModule().getStatusKind();
    if (status == 1) {
        work.onFlag(0x22000000);
        work.setInt(params->getParamInt(moduleAccesser, 0x5DC1, 0), 0x20000002);
    } else if (status == 2) {
        work.setInt(params->getParamInt(moduleAccesser, 0x5DC4, 0), 0x20000000);
    }
}

void wnDiddyBarrelStatusUniqProcess::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soParamAccesser* params = (soParamAccesser*)work.getParamAccesser();
    work.getInt(0x20000000);
    if (moduleAccesser->getStatusModule().getStatusKind() != 1) return;

    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soKineticEnergyNormal& normal = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(0));
    wnKineticEnergyGravity& gravity = dynamic_cast<wnKineticEnergyGravity&>(*kinetic.getEnergy(1));
    float lr = moduleAccesser->getPostureModule().getLr();

    work.addInt(-1, 0x20000001);
    int frame = work.getInt(0x20000001);
    if (frame <= 0 || work.isFlag(0x22000000)) {
        work.setFloat(360.0f * randf(), 0x21000001);
        int delay = params->getParamInt(moduleAccesser, 0x5DC2, 0);
        int randomDelay = params->getParamInt(moduleAccesser, 0x5DC3, 0);
        if (randomDelay > 0) {
            randomDelay = params->getParamInt(moduleAccesser, 0x5DC3, 0);
            delay += randi(randomDelay);
        }
        work.setInt(delay, 0x20000001);
        work.offFlag(0x22000000);
    }

    Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
    Vec2f direction = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
    direction.normalize();
    float angle = atan2f(direction.m_x, direction.m_y) * 57.29578f;
    if (!(angle >= 0.0f)) angle += 360.0f;

    float targetAngle = work.getFloat(0x21000001);
    float delta = targetAngle - angle;
    float distance = fabsf(delta);
    float turnDirection;
    if (!(delta <= 0.0f)) {
        turnDirection = delta - 180.0f >= 0.0f ? -1.0f : 1.0f;
    } else {
        turnDirection = distance - 180.0f >= 0.0f ? 1.0f : -1.0f;
    }

    int stepParam;
    if (lr >= 0.0f) {
        stepParam = turnDirection > 0.0f ? 0xFA3 : 0xFA2;
    } else {
        stepParam = turnDirection > 0.0f ? 0xFA2 : 0xFA3;
    }
    float stepLimit = params->getParamFloat(moduleAccesser, stepParam, 0);
    float step = distance;
    if (!(distance <= stepLimit)) {
        step = params->getParamFloat(moduleAccesser, stepParam, 0);
    }
    if (step == distance) work.onFlag(0x22000000);

    step *= turnDirection;
    angle += step;
    if (!(angle <= 360.0f)) angle -= 360.0f;

    float magnitude = speed.length();
    if (work.getInt(0x20000002) > 0) {
        work.addInt(-1, 0x20000002);
        float minimum = work.getFloat(0x21000002);
        if (!(magnitude >= minimum)) magnitude = work.getFloat(0x21000002);
    }

    Vec2f rotated(0.0f, magnitude);
    rotated.rot(&rotated, (360.0f - angle) * 0.01745329238f);
    normal.m_speed.m_x = rotated.m_x;
    normal.m_speed.m_y = 0.0f;
    gravity.m_speedY = rotated.m_y;

    Vec3f rotation = moduleAccesser->getModelModule().getNodeRotate(3);
    rotation.m_x = angle * lr;
    moduleAccesser->getModelModule().setNodeRotate(3, &rotation);

    float speedMul = params->getParamFloat(moduleAccesser, 0xFA7, 0);
    float baseSpeed = params->getParamFloat(moduleAccesser, 0xFA6, 0);
    moduleAccesser->getCollisionAttackModule().setPowerAddStatus(magnitude / baseSpeed * speedMul);
}

void wnDiddyBarrelStatusUniqProcess::execStop(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
    speed.normalize();
    float angle = atan2f(speed.m_x, speed.m_y) * 57.29578f;
    Vec3f rotation = moduleAccesser->getModelModule().getNodeRotate(3);
    float lr = moduleAccesser->getPostureModule().getLr();
    rotation.m_x = angle * lr;
    moduleAccesser->getModelModule().setNodeRotate(3, &rotation);
}

void wnDiddyBarrelStatusUniqProcess::exitStatus(soModuleAccesser*, int) { }

wnWeaponBuilder<wnDiddyBarrelModuleAccesserBuildConfig>::wnWeaponBuilder(
    const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd) :
    Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), true), m_modules(this, bd) {
    Weapon::initialize();
}

wnWeaponBuilder<wnDiddyBarrelModuleAccesserBuildConfig>::~wnWeaponBuilder() { }

void wnWeaponBuilder<wnDiddyBarrelModuleAccesserBuildConfig>::deactivateDescendantForce() {
    soSingletonHolder<soArticleMediatorNull>::getInstance()->deactivate();
}

wnDiddyBarrel::wnDiddyBarrel(s32 articleId, const wnArticleBuildInfo& info, const wnDiddyBarrelData* data) :
    wnWeaponBuilder<wnDiddyBarrelModuleAccesserBuildConfig>(
        "DiddyBarrel", articleId, info,
        wnModuleAccesserBuildData(0x37, data, info.getHeapModule()->getInstanceHeap(),
                                  info.getHeapModule()->getNWModelInstanceHeap(), info.getHeapModule()->getNWMotionInstanceHeap(),
                                  data->m_resGroupNo, -1, &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_data(data), m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[3];
    uniqProcs[0] = wnDiddyBarrelStatusUniqProcess::getInstance();
    uniqProcs[1] = wnDiddyBarrelStatusUniqProcess::getInstance();
    uniqProcs[2] = wnDiddyBarrelStatusUniqProcess::getInstance();
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 3);
    m_moduleAccesser->getCollisionHitModule().add((soSet<soCollisionHitData>*)&data->m_hitDataStorage, 0);
    Weapon::deactivate(false);
}

void wnDiddyBarrel::onDeactivate() { }

void wnDiddyBarrel::processUpdate() {
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000001);
    Weapon::processUpdate();
}

bool wnDiddyBarrel::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* acc, int index) {
    return Weapon::notifyEventAnimCmd(cmd, acc, index);
}

void wnDiddyBarrel::notifyEventCollisionAttack(float power, soCollisionLog* collisionLog,
                                                soModuleAccesser* moduleAccesser) {
    float minPower = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    if (!(minPower >= power)) {
        m_moduleAccesser->getWorkManageModule().setFloat(power, 0x11000001);
    }
    Weapon::notifyEventCollisionAttack(power, collisionLog, moduleAccesser);
}

bool wnDiddyBarrel::notifyEventCollisionAttackCheck(u32 flags) {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 1) {
        if ((flags & 3) == 0 && (flags & 0x10) == 0) {
            if (flags & 4) {
                if (unkA4Bit7) return hop();
            } else if (flags & 8) {
                soReflectModule& reflectModule = m_moduleAccesser->getReflectModule();
                if (reflectModule.isCountMax()) {
                    m_moduleAccesser->getPostureModule().reverseLr();
                    m_moduleAccesser->getPostureModule().initRotYLr();
                    int team = reflectModule.getTeam();
                    soTeamModule& teamModule = m_moduleAccesser->getTeamModule();
                    teamModule.setTeam(team, true);
                    teamModule.setHitTeam(team);
                    teamModule.setTeamOwnerId(reflectModule.getTaskId());
                    return reflect();
                }
            }
        }
    }

    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    float hitStop = work.getFloat(0x11000001);
    if (!(hitStop <= 0.0f)) {
        hitStop = work.getFloat(0x11000001);
        setHitStop(hitStop, 1.0f, 0);
    }
    return false;
}

bool wnDiddyBarrel::isCanEat() {
    return m_moduleAccesser->getStatusModule().getStatusKind() == 2;
}

void wnDiddyBarrel::activate(int founderTaskId, int unk1, int team, Vec3f* pos, Vec2f* speed, bool unk5,
                             float lr, float rot) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = desc.unk8 = desc.unkC = unk1;
    desc.unk10 = desc.unk14 = -1;
    desc.unk18 = desc.unk1C = 0;
    desc.pos = *pos;
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

    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.setFloat(rot, 0x21000000);
    work.setFloat((float)m_param->unk0, 0x21000003);
    if (unk5) {
        m_moduleAccesser->getStatusModule().changeStatusForce(2, m_moduleAccesser);
    } else {
        work.setFloat(m_param->unk4 + rot * m_param->unk8, 0x21000002);
        work.setFloat(speed->m_x, 0x21000004);
        work.setFloat(speed->m_y, 0x21000005);
        m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
    }
}

void wnDiddyBarrel::toShoot(Vec3f* pos, float lr, Vec2f* speed, float value, float, bool unk0) {
    m_moduleAccesser->getPostureModule().init(pos, lr);
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.setFloat(value, 0x21000000);
    if (unk0) {
        m_moduleAccesser->getStatusModule().changeStatusForce(2, m_moduleAccesser);
    } else {
        work.setFloat(speed->m_x, 0x21000004);
        work.setFloat(speed->m_y, 0x21000005);
        m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
    }
}
