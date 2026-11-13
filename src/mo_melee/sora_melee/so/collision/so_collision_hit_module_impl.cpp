#define SO_COLLISION_HIT_MODULE_IMPL_FIELDS
#define SO_GLOW_MODULE_ATTACK_API
#define WN_EM_SIMPLE_GLOW_API
#define SO_GLOW_MODULE_POWER_API
#define SO_EVENT_PRESENTER_UPDATE_CHECK_MANAGE_ID
#define SO_COLLISION_CTOR_DTOR
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/collision/so_collision_hit_event_presenter.h>
#include <so/glow/so_glow_module_simple.h>
#include <so/so_module_accesser.h>
#include <gf/gf_task_scheduler.h>
#include <nw4r/ut/ut_list.h>
#include <so/so_external_value_accesser.h>
#include <so/stageobject.h>
#include <types.h>

extern "C" int fn_27_41C40(soCollisionHitGroup*, soCollision*, soArray<soCollisionHitPart>*, soSet<soCollisionHitData>*, u32, u32);
extern "C" int fn_27_41CF8(soCollisionHitGroup*, soCollision*, soArray<soCollisionHitPart>*, soSet<soCollisionHitData::Simple>*, u32, u32);
extern "C" void fn_27_41F7C(soCollisionHitGroup*, soArray<soCollisionHitPart>*, soSet<soCollisionHitData::Simple>*);
extern "C" u32 fn_27_42348(soCollisionHitGroup*, soArray<soCollisionHitPart>*, u32);
extern "C" bool fn_27_4238C(soCollisionHitGroup*, soCollision*, soArray<soCollisionHitPart>*, soModelModule*, float, float, float, SituationKind);
extern "C" int fn_27_428E0(soCollisionHitGroup*);

// HYPOTHESIS: partial view of soStatusData (the real definition is empty in the headers)
struct soStatusDataView {
    char _0[0xc];
    u32 : 24;
    u32 m_keepHit : 1;
};

soCollisionHitModule::~soCollisionHitModule() { }

soCollisionHitModuleImpl::~soCollisionHitModuleImpl() { }

void soCollisionHitModuleImpl::activate(float posX, float scale, float lr, u32 collisionIndex, u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).reset(&m_collision, m_collisionHitPartArray, posX, scale, lr, (SituationKind)collisionIndex);
    unk66_0 = 0;
    setNoTeam(false);
    m_5c = 0;
    m_prevTotalStatus = 0;
}

void soCollisionHitModuleImpl::activate(float posX, float scale, float lr, int taskId, u32 collisionIndex) {
    m_collision.m_taskId = taskId;
    for (int i = 0; i < (int)getGroupNum(); i++) {
        activate(posX, scale, lr, collisionIndex, i);
    }
}

void soCollisionHitModuleImpl::reset(float posX, float scale, float lr, u32 collisionIndex, u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).reset(&m_collision, m_collisionHitPartArray, posX, scale, lr, (SituationKind)collisionIndex);
}

void soCollisionHitModuleImpl::deactivate() {
    if (unk66_2 == 0) {
        clean();
    } else {
        m_collision.initLogAll();
    }
    m_collision.m_taskId = -1;
}

void soCollisionHitModuleImpl::clean() {
    for (int i = 0; i < m_collisionHitGroupArray->size(); i++) {
        m_collisionHitGroupArray->at(i).init();
    }
    m_hitPartCount = 0;
    m_collision.clean();
}

void soCollisionHitModuleImpl::sleep(bool isSleep) {
    m_isSleep = isSleep;
    if ((int)m_isSleep == 1) {
        m_collision.initLogAll();
    }
}

void soCollisionHitModuleImpl::add(soSet<soCollisionHitData>* set, u32 collisionHitGroupIndex) {
    m_hitPartCount += fn_27_41C40(&m_collisionHitGroupArray->at(collisionHitGroupIndex), &m_collision, m_collisionHitPartArray, set, collisionHitGroupIndex, m_hitPartCount);
}

void soCollisionHitModuleImpl::add(soSet<soCollisionHitData::Simple>* set, u32 collisionHitGroupIndex) {
    m_hitPartCount += fn_27_41CF8(&m_collisionHitGroupArray->at(collisionHitGroupIndex), &m_collision, m_collisionHitPartArray, set, collisionHitGroupIndex, m_hitPartCount);
}

void soCollisionHitModuleImpl::resetStatusAll(u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).resetStatusAll(m_collisionHitPartArray);
}

void soCollisionHitModuleImpl::set(u32 collisionHitPartIndex, soCollisionHitData* hitData, u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).set(m_collisionHitPartArray, collisionHitPartIndex, hitData);
}

void soCollisionHitModuleImpl::set(soSet<soCollisionHitData>* set, u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).set(m_collisionHitPartArray, set);
}

void soCollisionHitModuleImpl::set(soSet<soCollisionHitData::Simple>* set, u32 collisionHitGroupIndex) {
    fn_27_41F7C(&m_collisionHitGroupArray->at(collisionHitGroupIndex), m_collisionHitPartArray, set);
}

void soCollisionHitModuleImpl::setStatus(u32 collisionHitPartIndex, int status, u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).setStatus(m_collisionHitPartArray, collisionHitPartIndex, status);
}

void soCollisionHitModuleImpl::setStatusNode(int nodeIndex, int status, u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).setStatusNode(m_collisionHitPartArray, nodeIndex, status);
}

void soCollisionHitModuleImpl::setStatusNodeDefault(int nodeIndex, int status, u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).setStatusNodeDefault(m_collisionHitPartArray, nodeIndex, status);
}

void soCollisionHitModuleImpl::setStatusAll(int status, u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).setStatusAll(m_collisionHitPartArray, status);
}

void soCollisionHitEventObserver::notifyEventChangeCollisionHit(int index, soModuleAccesser* moduleAccesser) { }

void soCollisionHitModuleImpl::setCheckCatch(bool checkCatch, u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).setCheckCatch(m_collisionHitPartArray, checkCatch);
}

void soCollisionHitModuleImpl::update(float posX, float scale, float lr, SituationKind situationKind, u32 collisionHitGroupIndex) {
    if ((int)m_isSleep == 1) {
        return;
    }
    soModelModule* modelModule = m_moduleAccesser->m_enumerationStart->m_modelModule;
    soCollisionHitGroup& group = m_collisionHitGroupArray->at(collisionHitGroupIndex);
    if (fn_27_4238C(&group, &m_collision, m_collisionHitPartArray, modelModule, posX, scale, lr, situationKind) == true) {
        if (!unk66_0) {
            m_collision.addCheckList();
            unk66_0 = 1;
        }
    }
}

void soCollisionHitModuleImpl::update(float posX, float scale, float lr, SituationKind situationKind) {
    if (!m_isSleep) {
        int num = getGroupNum();
        for (int i = 0; i < num; i++) {
            update(posX, scale, lr, situationKind, i);
        }
    }
}

int soCollisionHitModuleImpl::getStatus(u32 collisionHitPartIndex, u32 collisionHitGroupIndex) {
    int partStatus = m_collisionHitGroupArray->at(collisionHitGroupIndex).getPartStatus(m_collisionHitPartArray, collisionHitPartIndex);
    int totalStatus = fn_27_428E0(&m_collisionHitGroupArray->at(collisionHitGroupIndex));
    if (partStatus > totalStatus) {
        return partStatus;
    }
    return totalStatus;
}

int soCollisionHitModuleImpl::getTotalStatus(u32 collisionHitGroupIndex) {
    return fn_27_428E0(&m_collisionHitGroupArray->at(collisionHitGroupIndex));
}

float soCollisionHitModuleImpl::getDurabilityMulFromAttackData(soCollisionAttackData* attackData, u32 partId) {
    return 1.0f;
}

int soCollisionHitModuleImpl::getPartSize(u32 collisionHitGroupIndex) {
    return m_collisionHitGroupArray->at(collisionHitGroupIndex).m_partSize;
}

u32 soCollisionHitModuleImpl::getPartHeight(u32 collisionHitPartIndex, u32 collisionHitGroupIndex) {
    return fn_27_42348(&m_collisionHitGroupArray->at(collisionHitGroupIndex), m_collisionHitPartArray, collisionHitPartIndex);
}

void soCollisionHitModuleImpl::setNoTeam(bool isNoTeam) {
    m_collision.m_isNoTeam = isNoTeam;
}

void soCollisionHitModuleImpl::setGlobalOffset(u8 offset, u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).m_globalOffset = offset;
}

void soCollisionHitModuleImpl::setMultiSituation(SituationKind situationKind, u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).setMultiSituation(situationKind);
}

void soCollisionHitModuleImpl::initMultiSituation(u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).initMultiSituation();
}

void soCollisionHitModuleImpl::setOpponentCategory(u32 categoryMask, bool enable, u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).setOpponentCategory(m_collisionHitPartArray, categoryMask, enable);
}

void soCollisionHitModuleImpl::setSelfCategory(soCollision::Category category, u32 collisionHitGroupIndex) {
    m_collisionHitGroupArray->at(collisionHitGroupIndex).setSelfCategory(m_collisionHitPartArray, category);
}

void soCollisionHitModuleImpl::event(soCollisionAttackModule* attackModule, void* attackerGlowModule, soCollisionLog* collisionLog, u32 groupIndex) {
    u8 collisionIndex = collisionLog->m_collsionIndex;
    int totalStatus = getTotalStatus(0);
    if (!attackModule->isInvalidInvincible(collisionIndex, collisionLog->m_isAbsolute) && totalStatus == 1) {
        return;
    }
    float power = attackModule->getPower(collisionIndex, collisionLog->m_isAbsolute);
    soCollisionAttackData* attackData = attackModule->getData(collisionIndex, collisionLog->m_isAbsolute);
    if (collisionLog->m_isAbsolute == true && totalStatus == 1) {
        power = 0.0f;
    }
    if (power > 0.0f) {
        power *= getDurabilityMulFromAttackData(attackData, collisionLog->m_damageIndex);
    }
    if (static_cast<soGlowModule*>(m_moduleAccesser->m_enumerationStart->m_glowModule)->isGlow() == true &&
        static_cast<soGlowModule*>(attackerGlowModule)->isGlow() == true) {
        power = static_cast<soGlowModule*>(m_moduleAccesser->m_enumerationStart->m_glowModule)->modifyPower(power, attackData, attackerGlowModule);
    }
    int i;
    soInstanceManagerFullProperty<soCollisionHitEventObserver*>* observerList = soEventPresenter<soCollisionHitEventObserver>::getObserverList();
    for (i = 0; i < (int)observerList->size(); i++) {
        observerList->atIndex(i)->notifyEventCollisionHit(power, attackData, groupIndex, collisionLog->m_damageIndex, m_moduleAccesser, collisionLog);
    }
}

void soCollisionHitEventObserver::notifyEventCollisionHit(float power, soCollisionAttackData*, u32 index, int, soModuleAccesser* moduleAccesser, soCollisionLog*) { }

void soCollisionHitModuleImpl::event2nd(soCollisionAttackModule* attackModule, void* attackerGlowModule, soCollisionLog* collisionLog, u32 groupIndex) {
    bool flag = true;
    if (attackModule->isInvalidInvincible(collisionLog->m_collsionIndex, collisionLog->m_isAbsolute) != 0 || getTotalStatus(0) != 1) {
        flag = false;
    }
    soInstanceManagerFullProperty<soCollisionHitEventObserver*>* observerList = soEventPresenter<soCollisionHitEventObserver>::getObserverList();
    for (int i = 0; i < (int)observerList->size(); i++) {
        float lr = m_collisionHitGroupArray->at(groupIndex).m_lr;
        soCollisionHitEventObserver*& observer = observerList->atIndex(i);
        float posX = m_collision.getPosX(groupIndex);
        observer->notifyEventCollisionHit2nd(attackModule, collisionLog, groupIndex, posX, lr, m_moduleAccesser, flag);
    }
}

void soCollisionHitModuleImpl::checkLog() {
    typedef void (soCollisionHitModuleImpl::*HitEventFunc)(soCollisionAttackModule*, void*, soCollisionLog*, u32);
    static const HitEventFunc eventFuncs[2] = { &soCollisionHitModuleImpl::event, &soCollisionHitModuleImpl::event2nd };
    int groupNum = m_collisionHitGroupArray->size();
    for (u32 funcIndex = 0; funcIndex < 2; funcIndex++) {
        HitEventFunc func = eventFuncs[funcIndex];
        for (int groupIndex = 0; groupIndex < groupNum; groupIndex++) {
            nw4r::ut::List* logList = (nw4r::ut::List*)m_collision.getLogList(groupIndex);
            soCollisionLog* log = NULL;
            while ((log = (soCollisionLog*)nw4r::ut::List_GetNext(logList, log)) != NULL) {
                if (log->m_kind == 0) {
                    gfTask* task = gfTaskScheduler::getInstance()->getTaskById((gfTask::Category)log->m_taskCategory, log->m_taskId);
                    if (task != NULL) {
                        StageObject& stageObject = dynamic_cast<StageObject&>(*task);
                        soCollisionAttackModule* attackModule = soExternalValueAccesser::getCollisionAttackModule(&stageObject);
                        if (attackModule->isInvalidXlu(log->m_collsionIndex, log->m_isAbsolute) || getTotalStatus(0) != 2) {
                            void* attackerGlowModule = soExternalValueAccesser::getGlowModule(&stageObject);
                            (this->*func)(attackModule, attackerGlowModule, log, groupIndex);
                        }
                    }
                }
            }
        }
    }
}

u32 soCollisionHitModuleImpl::getGroupNum() {
    return m_collisionHitGroupArray->size();
}

void soCollisionHitModuleImpl::setPosX(float posX, u32 collisionGroupIndex) {
    m_collision.setPosX(posX, collisionGroupIndex);
}

float soCollisionHitModuleImpl::getPosX(u32 collisionGroupIndex) {
    return m_collision.getPosX(collisionGroupIndex);
}

Vec3f soCollisionHitModuleImpl::getCenterPos(u16 index, u32 collisionHitGroupIndex) {
    return m_collisionHitGroupArray->at(collisionHitGroupIndex).getCenterPos(&m_collision, index);
}

bool soCollisionHitModuleImpl::isObserv(char unk1) {
    return unk1 == 6;
}

void soCollisionHitModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: bit 31 of the word at +0xc of the status data keeps the hit state across the status change
    if (!((soStatusDataView*)statusData)->m_keepHit) {
        resetStatusAll(0);
        setWhole(0, 0);
        setNoTeam(false);
    }
}

void soCollisionHitModuleImpl::renderDebug() {
}

void soCollisionHitModuleImpl::setReactionFrame(short reactionFrame) {
    m_reactionFrame = reactionFrame;
}

int soCollisionHitModuleImpl::getReactionFrame() {
    return m_reactionFrame;
}

bool soCollisionHitModuleImpl::isReactionFrame() {
    return m_reactionFrame >= 0;
}
