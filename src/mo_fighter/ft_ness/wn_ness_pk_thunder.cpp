#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_STATUS_PLAIN_NULL_ARRAY
#define FT_REL_LINK_EXTERN
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_CTOR_NOINLINE
#include <math.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/ness/wn_ness_pk_thunder.h>
#include <wn/wn_activate_desc.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/fighter.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_event_presenter.h>

// The status unique process of the bolt (wn_ness_pk_thunder_status_uniq_process_move.cpp).
extern soStatusUniqProcess g_wnNessPKThunderStatusUniqProcessMove;

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnNessPKThunderChild>::~wnArticleHolder();
template wnArticleSubPool<wnNessPKThunderChild, 1>::~wnArticleSubPool();
template wnArticleSubPool<wnNessPKThunderChild, 2>::~wnArticleSubPool();
template wnArticleSubPool<wnNessPKThunderChild, 3>::~wnArticleSubPool();
template wnArticleSubPool<wnNessPKThunderChild, 4>::~wnArticleSubPool();
template wnArticleSubPool<wnNessPKThunderChild, 5>::~wnArticleSubPool();
template wnArticleSubPool<wnNessPKThunderChild, 6>::~wnArticleSubPool();
template wnNessPKThunderChild* wnArticleSubPool<wnNessPKThunderChild, 6>::getInstanceAt(s32);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template wnSelectedArticleMediator<wnNessPKThunderMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<wnNessPKThunderMediator, 6>::~wnArticleManageModuleBuilder();
wnNessPKThunderMediator::~wnNessPKThunderMediator() { }
#pragma dont_inline off
soArticleMediator::~soArticleMediator() { }

// StageObject +0xC4 holds the kind / sub kind record of the object (for a fighter the sub kind is the fighter kind).
// HYPOTHESIS: source spelling; the layout follows the holder constructor.
struct wnStageObjectKindView {
    u8 unk0[0xC4];
    wnArticleKindInfo kindInfo;
};

struct wnArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    wnArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

// What the constructor of one of the six bolts is given: the kind record of the owner, the heap module and the data
// block of the owner's fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself; the arguments are rebuilt twice here.
struct wnNessPKThunderChildArgs {
    wnArticleDataTag tag;
    wnArticleBuildInfo info;
    wnNessPKThunderChildArgs(soModuleAccesser* acc) {
        info.kindInfo = &reinterpret_cast<wnStageObjectKindView*>(&dynamic_cast<Fighter&>(*acc->m_stageObject))->kindInfo;
        info.heapModule = static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule);
    }
    const wnNessPKThunderChildData* getData(soModuleAccesser* acc) {
        ftKind kind = *reinterpret_cast<ftKind*>(reinterpret_cast<u8*>(&dynamic_cast<Fighter&>(*acc->m_stageObject)) + 0xC8);
        return static_cast<const wnNessPKThunderChildData*>(g_ftCommonDataAccesser.getWeaponData124(kind, &tag.m_resourceGroup));
    }
};

// Every one of the six bolts is built by this constructor from the data of the owner's fighter kind.
template <>
wnArticleHolder<wnNessPKThunderChild>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(0, wnNessPKThunderChildArgs(acc).info, wnNessPKThunderChildArgs(acc).getData(acc)) { }

s32 wnNessPKThunderMediator::getMediateNum() { return 1; }
void wnNessPKThunderMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

s32 wnNessPKThunderMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 6; }
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}

void wnNessPKThunderMediator::deactivate() {
    for (s32 i = 0; i < 6; ++i) {
        wnNessPKThunderChild* weapon = m_pools.getSub().getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 wnNessPKThunderMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: {
        wnArticleSlotTag tag;
        wnArticleActiveCounter counter;
        for (s32 i = 0; i < 6; ++i) {
            wnArticleActiveCounter* c = &counter;
            (*c)(&static_cast<soArticle&>(*m_pools.getSub().getInstanceAt(i)));
        }
        return counter.m_activeNum;
    }
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}

bool wnNessPKThunderMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: {
        wnArticleSlotTag tag;
        bool allActive;
        s32 i;
        for (i = 0; i < 6; ++i) {
            if (static_cast<soArticle&>(*m_pools.getSub().getInstanceAt(i)).isActiveArticle() == false) {
                allActive = false;
                goto done;
            }
        }
        allActive = true;
    done:
        return allActive == false;
    }
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(false)
    default: return false;
    }
}

// Hands out the first inactive bolt (newest first), else deactivates the oldest active one and reuses it.
soArticle* wnNessPKThunderMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: {
        wnArticleSlotTag tag;
        soArticleDeactivateChecker checker;
        volatile u8 findTag = 0; // MATCH-ONLY: the native traversal keeps a one-byte marker
        wnNessPKThunderChild* weapon = m_pools.find(checker);
        if (weapon == NULL) {
            weapon = static_cast<wnNessPKThunderChild*>(checker.getCandidate());
            if (weapon == NULL) {
                return wnGetNullArticle();
            }
            weapon->deactivateArticle();
        }
        if (wnNessPKThunder::activeArticle(weapon, acc) == true) {
            return static_cast<soArticle*>(weapon);
        }
        return wnGetNullArticle();
    }
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Shooting only type-checks the article; the unused slots accept anything.
bool wnNessPKThunderMediator::shoot(soModuleAccesser*, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; (void)dynamic_cast<wnNessPKThunderChild&>(*article); return true; }
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(true)
    default: return false;
    }
}

// Sets one trailing bolt up: it follows the previous bolt of the chain (the first one follows the head).
bool wnNessPKThunder::activeArticle(wnNessPKThunderChild* weapon, soModuleAccesser* acc) {
    soGenerateArticleManageModule& articles = acc->getGenerateArticleManageModule();
    s32 count = articles.getActiveNum(0);
    s32 ownerTaskId;
    s32 linkTaskId;
    if (count >= 1) {
        linkTaskId = articles.getArticleFromNo(0, count - 1)->getTaskId();
    } else {
        linkTaskId = acc->m_stageObject->m_taskId;
    }
    ownerTaskId = acc->m_stageObject->m_taskId;
    weapon->activate(ownerTaskId, linkTaskId, acc->getTeamModule().getTeam()->getNo(), count);
    return true;
}

wnNessPKThunder::wnNessPKThunder(s32 articleId, const wnArticleBuildInfo& info, const wnNessPKThunderData* data) :
    wnWeaponBuilder<wnNessPKThunderModuleAccesserBuildConfig>(
        "NessPKThunder", articleId, info,
        wnModuleAccesserBuildData(104, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[1];
    uniqProcs[0] = &g_wnNessPKThunderStatusUniqProcessMove;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 1);
    Weapon::deactivate(false);
}

// HYPOTHESIS: 0x838 tells the articles linked to the head that it was reflected.
struct wnNessPKThunderLinkEvent : soLinkEventArgs {
    wnNessPKThunderLinkEvent(int kind) : soLinkEventArgs(kind) { }
};

// Launches the head of the bolt: it flies off along the angle given by its parameters at the speed of the parameter block.
void wnNessPKThunder::activate(int founderTaskId, int team, Vec3f* pos, float lr) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    __memcpy(&desc.pos, pos, sizeof(Vec3f));
    desc.lr = lr;
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
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(4), true);
    m_moduleAccesser->getWorkManageModule().setFloat(m_param->m_startAngleMul * 0.017453292f, 0x21000000);
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    Vec2f speed;
    speed.m_x = 0.0f;
    speed.m_y = m_param->m_speed;
    normal.m_speed = speed;
    Vec2f accel;
    accel.m_x = 0.0f;
    accel.m_y = 0.0f;
    normal.m_accel = accel;
    Vec2f brake;
    brake.m_x = 0.0f;
    brake.m_y = 0.0f;
    normal.m_brake = brake;
    Vec2f limit(m_param->m_speed, m_param->m_speed);
    normal.m_speedLimit = limit;
    Vec2f target(m_param->m_speed, m_param->m_speed);
    normal.m_speedTarget = target;
    normal.enable();
}

// Bounced back (reflected): the head turns around, lets go of the bolts that follow it and flies off in the opposite direction.
bool wnNessPKThunder::reflect() {
    Vec2f speed;
    wnNessPKThunderLinkEvent event(0x838);
    m_moduleAccesser->getLinkModule().sendEventParents(3, event);
    m_moduleAccesser->getGenerateArticleManageModule().removeExist(0, 0);
    m_moduleAccesser->getWorkManageModule().setInt(0, 0x20000000);
    m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(4), false);
    float angle = m_moduleAccesser->getWorkManageModule().getFloat(0x21000000);
    angle += 3.1415927f;
    while (angle < 0.0f) {
        angle += 6.2831855f;
    }
    while (angle > 6.2831855f) {
        angle -= 6.2831855f;
    }
    m_moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000000);
    const wnNessPKThunderParam* param = dynamic_cast<wnNessPKThunder&>(*m_moduleAccesser->m_stageObject).m_param;
    speed.m_x = param->m_speed * (float)cos(angle);
    speed.m_y = param->m_speed * (float)sin(angle);
    dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0)).m_speed = speed;
    setNotifyArticleEventEject(-1, m_moduleAccesser);
    return false;
}
