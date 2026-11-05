#define SO_ARRAY_LIST_CTOR_FLAG // MATCH-ONLY: the launcher calls the sora_melee vector constructor with a zero argument
#define FT_POOL_ROOT_NONPOLYMORPHIC // the missile pool's root has no vtable (the first pool level owns it)
#define SO_COLLISION_SEARCH_OBSERVER_INLINE_CTOR // BYTES: the search observer's (manage id, priority) constructor is a weak copy in this unit
#define WN_WEAPON_BUILDER
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_KINETIC_GENERIC_BUILDER
#define WN_KINETIC_NORMAL_EXTERN
#define FT_REL_LINK_EXTERN
#define MT_VEC2F_CTOR_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/snake/wn_snake_nikita.h>
#include <wn/snake/wn_snake_nikita_missile_status_uniq_process_fly.h>

#include <so/kinetic/so_kinetic_module_impl.h>
#include <wn/snake/so_kinetic_energy_local_normal.h>

#include <gf/gf_task_scheduler.h>
#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/link/so_link_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_external_value_accesser.h>


namespace {
soGenerateArticleManageModule& getArticleManage(soModuleAccesser& accesser) {
    // HYPOTHESIS: this builder accessor stores the article manager at the
    // verified module-enumeration slot +0x84.
    return *static_cast<soGenerateArticleManageModule*>(
        accesser.m_enumerationStart->m_generateArticleManageModule);
}

struct SnakeNikitaEndEvent : soLinkEventArgs {
    explicit SnakeNikitaEndEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};
}

// The original built the launcher (0x206A4-0x23B40, ending with its own thunk block) and the missile
// (0x23B40-0x272CC) from two source files. In one file MWCC refuses to inline every module builder the two
// constructors share (-ipa file), so both constructors drop. Until the unit is split, the launcher constructor
// is only compiled with WN_SNAKE_NIKITA_LAUNCHER_TU (it is exact when the missile code is not in the file).
#ifdef WN_SNAKE_NIKITA_LAUNCHER_TU
#pragma push
#pragma pool_data off // MATCH-ONLY: native addresses each module global and singleton guard separately
wnSnakeNikita::wnSnakeNikita(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeNikitaData* data) :
    wnWeaponBuilder<wnSnakeNikitaModuleAccesserBuildConfig>(
        "SnakeNikita", articleId, info,
        wnModuleAccesserBuildData(0x1C, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1,
                                  &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
    m_unkMissileTaskIds(0) {
    Weapon::deactivate(false);
}
#pragma pop

// The owner kind words Weapon construction copies from wnArticleBuildInfo (+0xC4 kind info, +0xC8 fighter kind).
// HYPOTHESIS: view name.
struct wnSnakeNikitaOwnerView {
    u8 unk0[0xC4];
    wnArticleKindInfo kindInfo;
};
// HYPOTHESIS: the lookup flags form this two-byte record; only the second byte's role is known (as for F.L.U.D.D.).
struct wnSnakeNikitaMissileDataArgs {
    bool unk0;
    bool resourceGroup;
    wnSnakeNikitaMissileDataArgs() : unk0(false), resourceGroup(false) { }
};

static inline wnSnakeNikitaOwnerView* ownerView(soModuleAccesser* acc) {
    return reinterpret_cast<wnSnakeNikitaOwnerView*>(&dynamic_cast<Weapon&>(acc->getStageObject()));
}
struct wnSnakeNikitaMissileArgs {
    wnArticleBuildInfo info;
    wnSnakeNikitaMissileArgs(const wnArticleKindInfo& kind, soHeapModule* heap) {
        info.kindInfo = &kind;
        info.heapModule = heap;
    }
};

// One pooled missile (map: wnInstanceHolder<wnSnakeNikitaMissile, soKindInfoGeneric<2, 28>, soIntToType<0> > ctor):
// built with the owner's kind info and heap module and Snake's weapon data 24.
template <>
wnArticleHolder<wnSnakeNikitaMissile>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(0, wnSnakeNikitaMissileArgs(ownerView(acc)->kindInfo, &acc->getHeapModule()).info,
               static_cast<const wnSnakeNikitaMissileData*>(g_ftCommonDataAccesser.getWeaponData24(
                   static_cast<ftKind>(ownerView(acc)->kindInfo.subKind), &wnSnakeNikitaMissileDataArgs().resourceGroup))) { }

#endif // WN_SNAKE_NIKITA_LAUNCHER_TU

void wnSnakeNikitaMissile::setAutonomy() {
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);

    if (m_moduleAccesser->getLinkModule().isLink(3)) {
        // HYPOTHESIS: the enum names attribute 4 as unknown; its meaning is unverified.
        m_moduleAccesser->getLinkModule().setAttribute(
            3, soLinkConnection::Attribute_Reference_Parent_Unknown_4, false);
    }
}

void wnSnakeNikita::notifyArticleEventRemove(int unk1, int* unk2) {
    StageObject::notifyArticleEventRemove(unk1, unk2);
}

void wnSnakeNikita::notifyEventLink(soLinkEventArgs* eventInfo,
                                    soModuleAccesser* moduleAccesser,
                                    StageObject* linkedObject, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0x838:
        notifyEnd(1);
        break;
    case 0x839:
        notifyEnd(0);
        break;
    default:
        break;
    }

    Weapon::notifyEventLink(eventInfo, moduleAccesser, linkedObject, unk4);
}

void wnSnakeNikita::notifyEnd(int isEnd) {
    if (isEnd != 0) {
        SnakeNikitaEndEvent event(0x456);
        m_moduleAccesser->getLinkModule().sendEventParents(3, event);
    } else {
        SnakeNikitaEndEvent event(0x457);
        m_moduleAccesser->getLinkModule().sendEventParents(3, event);
    }
}

void wnSnakeNikita::onDeactivate() {
    soArrayVector<soArticle*, 3> articles;
    getArticleManage(*m_moduleAccesser).getArticleList(&articles, 0, 2);

    if (!articles.isNull()) {
        wnSnakeNikitaMissile* missile =
            dynamic_cast<wnSnakeNikitaMissile*>(articles.at(0));
        if (missile != NULL) {
            missile->setAutonomy();

            const unsigned long taskId = *reinterpret_cast<u32*>(
                reinterpret_cast<u8*>(missile) + 0x28);
            bool alreadyTracked = false;
            for (s32 index = 0; index < m_unkMissileTaskIds.size(); ++index) {
                if (m_unkMissileTaskIds.at(index) == taskId) {
                    alreadyTracked = true;
                }
            }
            if (!alreadyTracked) {
                m_unkMissileTaskIds.push(taskId);
            }
        }
    }
}

void wnSnakeNikita::forceFallMissile() {
    soArrayVector<soArticle*, 3> articles;
    getArticleManage(*m_moduleAccesser).getArticleList(&articles, 0, 2);

    if (!articles.isNull()) {
        wnSnakeNikitaMissile* missile =
            dynamic_cast<wnSnakeNikitaMissile*>(articles.at(0));
        if (missile != NULL && soExternalValueAccesser::getStatusKind(missile) == 0 &&
            !soExternalValueAccesser::getWorkFlag(missile, 0x12000003)) {
            missile->changeStatus(1);
        }
    }
}

inline soKineticEnergyLocalNormal::soKineticEnergyLocalNormal(float angle, const Vec2f& speed, const Vec2f& accel,
                                                              const Vec2f& target, const Vec2f& brake,
                                                              const Vec2f& limit, bool unk30) :
    soKineticEnergyNormal(speed, accel, target, brake, limit, unk30, false, 0), unk34(angle) { }

// HYPOTHESIS: stands in for soParamAccesserNull::getNullInstance() (fn_122_23F48, map name), which the shared
// soParamAccesserNull header does not declare.
inline soParamAccesser* getNullParamAccesser() {
    return &g_soParamAccesserNull;
}
// HYPOTHESIS: stand in for soKineticEnergyRotNormal::setAccel/setBrake (fn_122_245FC/24604, map names), which the
// shared soKineticEnergyRotNormal header does not declare.
inline void setRotAccel(soKineticEnergyRotNormal* energy, const Vec3f& accel) {
    energy->m_accel = accel;
}
inline void setRotBrake(soKineticEnergyRotNormal* energy, const Vec3f& brake) {
    energy->m_brake = brake;
}

#pragma dont_inline on // BYTES: the native missile constructor calls every helper (accessors included) out of line
wnSnakeNikitaMissile::wnSnakeNikitaMissile(s32 articleId, const wnArticleBuildInfo& info,
                                           const wnSnakeNikitaMissileData* data) :
    wnWeaponBuilder<wnSnakeNikitaMissileModuleAccesserBuildConfig>(
        "SnakeNikitaMissile", articleId, info,
        wnModuleAccesserBuildData(0x1D, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1,
                                  getNullParamAccesser(), 0, 0, 0, 0, 0, 0, 0)),
    soDamageEventObserver(static_cast<soEventManager&>(getModuleAccesser()->getEventManageModule()).getManageId(), -1),
    soCollisionSearchEventObserver(static_cast<soEventManager&>(getModuleAccesser()->getEventManageModule()).getManageId(), -1),
    m_param(data->m_param),
    m_energyLocal(0.0f, Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), false),
    m_energyGravity(0.0f, -m_param->m_gravity, m_param->m_speedLimit) {
    getModuleAccesser()->getCollisionHitModule().add(reinterpret_cast<soSet<soCollisionHitData>*>(const_cast<u8*>(data->unk38)), 0);

    soStatusUniqProcess* uniqProcs[3] = { 0 };
    uniqProcs[0] = wnSnakeNikitaMissileStatusUniqProcessFly::getInstance();
    getModuleAccesser()->getStatusModule().addRangeUniqProc(uniqProcs, 3);

    getModuleAccesser()->getKineticModule().addEnergy(&m_energyLocal, wnSnakeNikitaMissile_Energy_LocalNormal,
                                                      soKineticEnergy::AttributeFlag(1), -1);
    getModuleAccesser()->getKineticModule().addEnergy(&m_energyGravity, wnSnakeNikitaMissile_Energy_Gravity,
                                                      soKineticEnergy::AttributeFlag(1), -1);
    soKineticEnergyRotNormal* rot = dynamic_cast<soKineticEnergyRotNormal*>(
        getModuleAccesser()->getKineticModule().getEnergy(wnSnakeNikitaMissile_Energy_Rot));
    if (rot != NULL) {
        setRotAccel(rot, Vec3f(0.0f, 0.0f, m_param->m_rotAccel));
        setRotBrake(rot, Vec3f(0.0f, 0.0f, m_param->m_rotBrake));
    }
    Weapon::deactivate(false);
}
#pragma dont_inline reset

wnWeaponBuilder<wnSnakeNikitaMissileModuleAccesserBuildConfig>::wnWeaponBuilder(
    const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd) :
    Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), true), m_modules(this, bd),
    m_energyNormal(), m_energyRotNormal() {
    Weapon::initialize();
    m_moduleAccesser->getKineticModule().addEnergy(&m_energyNormal, 0, soKineticEnergy::AttributeFlag(1), -1);
    m_moduleAccesser->getKineticModule().addEnergy(&m_energyRotNormal, 1, soKineticEnergy::AttributeFlag(1), -1);
}

void wnSnakeNikitaMissile::processCollision() {
    const int statusKind = m_moduleAccesser->getStatusModule().getStatusKind();
    if (statusKind == 0) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000001);
    } else if (statusKind == 1) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000000);
    }
    StageObject::processCollision();
}

void wnSnakeNikitaMissile::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser*) {
    if (collisionLog != NULL) {
        int statusKind = m_moduleAccesser->getStatusModule().getStatusKind();
        if (statusKind == 0) {
            if (collisionLog->m_kind == 3) {
                if (collisionLog->m_teamNo != m_moduleAccesser->getTeamModule().getTeam()->getNo() &&
                    m_moduleAccesser->getReflectModule().isReflect() == 1) {
                    m_moduleAccesser->getWorkManageModule().onFlag(0x22000002);
                }
            } else {
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000001);
            }
        } else if (statusKind == 1) {
            if (collisionLog->m_kind == 3) {
                if (collisionLog->m_teamNo != m_moduleAccesser->getTeamModule().getTeam()->getNo()) {
                    m_moduleAccesser->getWorkManageModule().onFlag(0x22000001);
                }
            } else {
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000000);
            }
        }
    }
}

bool wnSnakeNikitaMissile::notifyEventCollisionSearchCheck() {
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    const int statusKind = m_moduleAccesser->getStatusModule().getStatusKind();
    bool shouldReflect = false;
    if (statusKind == 0) {
        shouldReflect = work.isFlag(0x22000002);
        if (shouldReflect) {
            work.offFlag(0x22000002);
        }
    } else if (statusKind == 1) {
        shouldReflect = work.isFlag(0x22000001);
        if (shouldReflect) {
            work.offFlag(0x22000001);
        }
    }

    if (shouldReflect) {
        setAutonomy();
        SnakeNikitaEndEvent event(0x839);
        m_moduleAccesser->getLinkModule().sendEventParents(3, event);

        const int team = m_moduleAccesser->getReflectModule().getTeam();
        m_moduleAccesser->getTeamModule().setTeam(team, true);
        m_moduleAccesser->getTeamModule().setHitTeam(team);

        soKineticEnergyLocalNormal* energy = dynamic_cast<soKineticEnergyLocalNormal*>(
            m_moduleAccesser->getKineticModule().getEnergy(2));
        if (energy != NULL) {
            energy->unk34 -= 0.3846154f;
            if (energy->unk34 < 0.0f) {
                energy->unk34 += 360.0f;
            }
            Vec3f rotation(-energy->unk34, 90.0f, 0.0f);
            m_moduleAccesser->getPostureModule().setRot(&rotation, 1);
        }
        return false;
    }

    // HYPOTHESIS: these two work flags represent the native status-specific
    // collision transition gates; the flags themselves remain unnamed.
    if (statusKind == 0 && work.isFlag(0x22000001)) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        return true;
    }
    if (statusKind == 1 && work.isFlag(0x22000000)) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        return true;
    }
    return false;
}

bool wnSnakeNikitaMissile::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 0x6) {
        return false;
    }
    Weapon::notifyEventCollisionAttackCheck(flags);
    return false;
}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
int fn_122_24410(u8* p) { return *(int*)(p + 0x60); }
void* fn_122_245DC(u8* p) { return *(void**)(*(u8**)(p + 0xd8) + 0x70); }
} // extern "C"
