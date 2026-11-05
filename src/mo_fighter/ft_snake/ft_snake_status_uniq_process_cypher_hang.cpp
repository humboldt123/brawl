// Snake's Cypher Hang status: hang under the Cypher with the gravity and air control of the Cypher's parameters.
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/fighter.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <wn/snake/wn_snake_cypher_data.h>

class ftSnakeStatusUniqProcessCypherHang : public soStatusUniqProcess {
public:
    virtual ~ftSnakeStatusUniqProcessCypherHang() {}
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

// The data lookup's out-parameter pair, as in ft_snake.cpp: the getter receives the second flag.
struct ftSnakeArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftSnakeArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

// BYTES: sora_melee ftCommonDataAccesser::getWeaponData20 (map name; the symbol is still unnamed in sora_melee's
// symbols.txt): the Cypher's article data, also used by ft_snake.cpp's Cypher holder.
extern "C" void* fn_27_147F88(ftCommonDataAccesser* accesser, int kind, bool* resourceGroup);

// The Cypher data of the fighter kind that owns this status.
static inline const wnSnakeCypherData* getCypherData(soModuleAccesser* moduleAccesser) {
    ftSnakeArticleDataTag tag;
    int ownerKind = moduleAccesser->getStageObject().soGetSubKind();
    return static_cast<const wnSnakeCypherData*>(fn_27_147F88(&g_ftCommonDataAccesser, ownerKind, &tag.m_resourceGroup));
}

void ftSnakeStatusUniqProcessCypherHang::initStatus(soModuleAccesser* moduleAccesser) {
    const wnSnakeCypherParam* param = getCypherData(moduleAccesser)->m_param;

    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));

    ftKineticEnergyGravity& gravity =
        dynamic_cast<ftKineticEnergyGravity&>(
            *moduleAccesser->getKineticModule().getEnergy(1));
    gravity.enable();
    if (speed.m_y < -param->m_hangFallSpeedLimit)
        speed.m_y = -param->m_hangFallSpeedLimit;
    gravity.resetEnergy(1, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    gravity.m_gravity = param->m_hangGravity;
    gravity.m_fallSpeedMax = param->m_hangFallSpeedMax;
    gravity.unk1C = param->m_hangGravityUnk1C;
    gravity.enable();

    ftKineticEnergyController& controller =
        dynamic_cast<ftKineticEnergyController&>(
            *moduleAccesser->getKineticModule().getEnergy(2));
    controller.enable();
    controller.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    controller.m_accelMul = param->m_hangAirAccelMul;
    controller.m_speedTarget = Vec2f(param->m_hangAirSpeedTarget, 0.0f);
    controller.m_speedLimit = Vec2f(param->m_hangAirSpeedLimit, 0.0f);

    moduleAccesser->getKineticModule().getEnergy(0)->disable();
    moduleAccesser->getKineticModule().getEnergy(3)->disable();
}

void ftSnakeStatusUniqProcessCypherHang::execStatus(
    soModuleAccesser* moduleAccesser) {
    bool shouldClamp = false;
    if (moduleAccesser->getGroundModule().isTouch(8, 0)) {
        soKineticEnergy* energy = moduleAccesser->getKineticModule().getEnergy(1);
        if (energy->getSpeed().m_y <= 0.0f)
            shouldClamp = true;
    }

    if (shouldClamp) {
        ftKineticEnergyGravity& gravity =
            dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.m_speedY = 0.0f;
    }
}

void ftSnakeStatusUniqProcessCypherHang::exitStatus(
    soModuleAccesser*, int) {
}

ftSnakeStatusUniqProcessCypherHang g_ftSnakeStatusUniqProcessCypherHang;
