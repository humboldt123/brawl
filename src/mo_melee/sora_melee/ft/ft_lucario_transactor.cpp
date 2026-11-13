// Use the native Fighter observer override before transitive Fighter includes;
// this preserves the verified primary vtable slots of getOwner and getFtKind.
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <StaticAssert.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_owner.h>
#include <ft/lucario/ft_lucario_transactor.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/lucario/wn_lucario_aura_ball.h>

ftLucarioTransactor::ftLucarioTransactor() { }
ftLucarioTransactor::~ftLucarioTransactor() { }

// The sora_melee copy of the singleton (the fighter REL has its own getInstance).
// BYTES: the guard, the instance and its destructor registration keep separate addresses.
#pragma pool_data off
static inline ftLucarioTransactor* getLucarioTransactor() {
    static ftLucarioTransactor instance;
    return &instance;
}

// Kirby's copied aura ball is article 0xd, Lucario's own is article 0.
static inline int getAuraBallArticleId(soModuleAccesser* moduleAccesser) {
    int articleId = 0;
    if (moduleAccesser->m_stageObject->soGetSubKind() == Fighter_Kirby) {
        articleId = 0xd;
    }
    return articleId;
}

// HYPOTHESIS: 0x11000013/0x11000014 are the aura ball charge scale and its previous value.
void ftLucarioTransactor::init(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.setFloat(1.0f, 0x11000013);
    work.setFloat(1.0f, 0x11000014);
    getLucarioTransactor()->saveAuraBallStatus(moduleAccesser, false, 0, 0);
    moduleAccesser->getGenerateArticleManageModule().removeExist(getAuraBallArticleId(moduleAccesser), 0);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x10000042);
    getLucarioTransactor()->removeAuraBallMaxEffect(moduleAccesser);
}

void ftLucarioTransactor::init_auraball(soModuleAccesser* moduleAccesser) {
    getLucarioTransactor()->saveAuraBallStatus(moduleAccesser, false, 0, 0);
    moduleAccesser->getGenerateArticleManageModule().removeExist(getAuraBallArticleId(moduleAccesser), 0);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x10000042);
    getLucarioTransactor()->removeAuraBallMaxEffect(moduleAccesser);
}

// Leaving the fighter resets the aura ball state as init does.
void ftLucarioTransactor::exit(soModuleAccesser* moduleAccesser) {
    init(moduleAccesser);
}

// The map names a file-local routine that updates the aura ball effects.
void update_auraball_effect(soModuleAccesser* moduleAccesser);

void ftLucarioTransactor::processUpdate(soModuleAccesser* moduleAccesser) {
    if (soSlow::getInstance()->isEstimate() == true) {
        update_auraball_effect(moduleAccesser);
    }
}

// The map names a file-local routine: switches the full-charge effects when the charge level crosses the maximum.
void update_auraball_effect(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int prevLevel = work.getInt(0x10000042);
    int level = -1;
    soGenerateArticleManageModule& generate = moduleAccesser->getGenerateArticleManageModule();
    int articleId = getAuraBallArticleId(moduleAccesser);
    soArticle* article;
    if (generate.isExist(articleId) && (article = generate.getArticle(articleId)) != NULL) {
        level = dynamic_cast<wnLucarioAuraBall&>(*article).getChargeLevel();
    } else {
        int savedLevel;
        if (getLucarioTransactor()->loadAuraBallStatus(moduleAccesser, NULL, &savedLevel)) {
            level = savedLevel;
        }
    }
    if (prevLevel != level) {
        if (level >= moduleAccesser->getConstantIntKirby(0x5dc0)) {
            getLucarioTransactor()->setAuraBallMaxEffect(moduleAccesser);
        } else {
            getLucarioTransactor()->removeAuraBallMaxEffect(moduleAccesser);
        }
    }
    work.setInt(level, 0x10000042);
    work.setInt(level, 0x10000041);
}

bool ftLucarioTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd:
        if (wnLucarioAuraBall* weapon = dynamic_cast<wnLucarioAuraBall*>(article)) {
            return activeArticle1(weapon, acc);
        }
        break;
    }
    return false;
}

// Starts the aura ball with the stored charge (none when no charge was saved).
bool ftLucarioTransactor::activeArticle1(wnLucarioAuraBall* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(*acc->m_stageObject);
    int chargeFrame = 0;
    if (!loadAuraBallStatus(acc, &chargeFrame, NULL)) {
        chargeFrame = 0;
    }
    float chargeScale = acc->getWorkManageModule().getFloat(0x11000013);
    int founderTaskId = fighter.getTaskId();
    int kind = fighter.getFtKind();
    int team = fighter.getOwner()->getTeam();
    soPostureModule& posture = acc->getPostureModule();
    weapon->activate(founderTaskId, team, posture.getLr(), chargeFrame, chargeScale, kind);
    return true;
}

// Slot 13: shoots the aura ball (article 0xd).
bool ftLucarioTransactor::shootArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd:
        if (wnLucarioAuraBall* weapon = dynamic_cast<wnLucarioAuraBall*>(article)) {
            return shootArticle(weapon, acc);
        }
        break;
    }
    return false;
}

// The map names a file-local routine applying the recoil for a charge level.
void set_recoil(soModuleAccesser* acc, int chargeLevel);

bool ftLucarioTransactor::shootArticle(wnLucarioAuraBall* weapon, soModuleAccesser* acc) {
    float lr = acc->getPostureModule().getLr();
    float scale = acc->getPostureModule().getScale();
    weapon->shoot(lr, acc->getWorkManageModule().getFloat(0x11000013), scale);
    set_recoil(acc, weapon->getChargeLevel());
    acc->getWorkManageModule().setInt(7, 0x10000042);
    return false;
}

void ftLucarioTransactor::saveAuraBallStatus(soModuleAccesser* moduleAccesser, bool stored, int chargeFrame, int chargeLevel) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.setFlag(stored, 0x1200003d);
    work.setInt(chargeFrame, 0x10000040);
    work.setInt(chargeLevel, 0x10000041);
}

bool ftLucarioTransactor::loadAuraBallStatus(soModuleAccesser* moduleAccesser, int* chargeFrame, int* chargeLevel) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (chargeFrame != NULL) {
        *chargeFrame = work.getInt(0x10000040);
    }
    if (chargeLevel != NULL) {
        *chargeLevel = work.getInt(0x10000041);
    }
    return work.isFlag(0x1200003d);
}

// HYPOTHESIS: two follow effects mark a full charge (Kirby's copy uses his own effect IDs and nodes).
void ftLucarioTransactor::setAuraBallMaxEffect(soModuleAccesser* moduleAccesser) {
    EfID effect1 = static_cast<EfID>(0x220008);
    EfID effect2 = static_cast<EfID>(0x220009);
    int node1 = 0x1d;
    int node2 = 0x31;
    if (moduleAccesser->m_stageObject->soGetSubKind() == Fighter_Kirby) {
        node1 = 0x1b1;
        effect1 = static_cast<EfID>(0x1230002);
        node2 = 0x1b8;
        effect2 = static_cast<EfID>(0x1230003);
    }
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.getInt(0x10000043) == 0) {
        Vec3f rot(0.0f, 0.0f, 0.0f);
        Vec3f offset(0.0f, 0.0f, 0.0f);
        work.setInt(moduleAccesser->getEffectModule().reqFollow(effect1, node1, &offset, &rot, 1.0f, false, 0, 0, -1), 0x10000043);
    }
    soWorkManageModule& work2 = moduleAccesser->getWorkManageModule();
    if (work2.getInt(0x10000044) == 0) {
        Vec3f rot(0.0f, 0.0f, 0.0f);
        Vec3f offset(0.0f, 0.0f, 0.0f);
        work2.setInt(moduleAccesser->getEffectModule().reqFollow(effect2, node2, &offset, &rot, 1.0f, false, 0, 0, -1), 0x10000044);
    }
}

// Removes one full-charge effect and clears its handle.
static inline void removeAuraBallEffect(soModuleAccesser* moduleAccesser, u32 index) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int handle = work.getInt(index);
    if (handle != 0) {
        moduleAccesser->getEffectModule().remove(handle);
    }
    work.setInt(0, index);
}

void ftLucarioTransactor::removeAuraBallMaxEffect(soModuleAccesser* moduleAccesser) {
    removeAuraBallEffect(moduleAccesser, 0x10000043);
    removeAuraBallEffect(moduleAccesser, 0x10000044);
}

// Pushes Lucario back by the recoil of the charge level: grounded the stop energy slides (mode 0),
// airborne it uses mode 6 and brakes with constant 0xfa2.
void set_recoil(soModuleAccesser* acc, int chargeLevel) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    float lr = acc->getPostureModule().getLr();
    int situation = acc->getSituationModule().getKind();
    Vec2f speed(0.0f, 0.0f);
    int mode;
    float recoil;
    if (situation == 0) {
        mode = 0;
        recoil = acc->getConstantFloatKirby(0xfa0);
    } else {
        mode = 6;
        recoil = acc->getConstantFloatKirby(0xfa1);
    }
    speed.m_x = -lr * (recoil * chargeLevel);
    stop.resetEnergy(mode, &speed, &stop.getRotation(), acc);
    stop.m_speedLimit = speed;
    if (situation != 0) {
        stop.m_brake = Vec2f(acc->getConstantFloatKirby(0xfa2), 0.0f);
    }
}
