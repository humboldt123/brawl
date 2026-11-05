#define WN_SNAKE_NIKITA_LAUNCHER_TU // this file is the launcher half of the original pair (the missile is wn_snake_nikita_missile.cpp)
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

// Lets the current missile fly on by itself and remembers its task id (once).
void wnSnakeNikita::onDeactivate() {
    soArrayVector<soArticle*, 3> articles(0);
    getArticleManage(*m_moduleAccesser).getArticleList(&articles, 0, 2);

    if (!articles.isEmpty()) {
        wnSnakeNikitaMissile* missile = dynamic_cast<wnSnakeNikitaMissile*>(articles.at(0));
        if (missile == NULL) {
            return;
        }
        missile->setAutonomy();

        bool isNew = true;
        for (s32 i = 0; i < m_unkMissileTaskIds.size(); i++) {
            if (m_unkMissileTaskIds.at(i) == missile->m_taskId) {
                isNew = false;
            }
        }
        if (isNew) {
            m_unkMissileTaskIds.push(missile->m_taskId);
        }
    }
}

// Drops the current missile (status 1) while it is still in flight (status 0).
// HYPOTHESIS: work flag 0x12000003 marks a missile that may not be dropped.
void wnSnakeNikita::forceFallMissile() {
    soArrayVector<soArticle*, 3> articles(0);
    getArticleManage(*m_moduleAccesser).getArticleList(&articles, 0, 2);

    if (!articles.isEmpty()) {
        wnSnakeNikitaMissile* missile = dynamic_cast<wnSnakeNikitaMissile*>(articles.at(0));
        if (missile == NULL) {
            return;
        }
        if (soExternalValueAccesser::getStatusKind(missile) == 0 &&
            !soExternalValueAccesser::getWorkFlag(missile, 0x12000003)) {
            missile->changeStatus(1);
        }
    }
}
