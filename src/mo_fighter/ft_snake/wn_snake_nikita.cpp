#define WN_SNAKE_NIKITA_LAUNCHER_TU // this file is the launcher half of the original pair (the missile is wn_snake_nikita_missile.cpp)
#define SO_ARRAY_LIST_CTOR_FLAG // MATCH-ONLY: the launcher calls the sora_melee vector constructor with a zero argument
#define FT_POOL_ROOT_NONPOLYMORPHIC // the missile pool's root has no vtable (the first pool level owns it)
#define SO_COLLISION_SEARCH_OBSERVER_INLINE_CTOR // BYTES: the search observer's (manage id, priority) constructor is a weak copy in this unit
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS // BYTES: the missile destructor expands the rotation energy destructor
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_KINETIC_GENERIC_BUILDER
#define WN_KINETIC_NORMAL_EXTERN
#define WN_KINETIC_GRAVITY_INLINE_DTOR // BYTES: the missile destructor expands the gravity energy destructor
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
#include <wn/wn_activate_desc.h>
#include <so/motion/so_motion_change_param.h>
#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/link/so_link_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_external_value_accesser.h>

// BYTES: the missile destructor expands the damage observer's (empty) destructor and calls only its base's.
inline soDamageEventObserver::~soDamageEventObserver() { }

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

// The article manage builder and the missile pool chain tear down out of line (native order).
#pragma dont_inline on
template <class W, int N> wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template <class M> wnSelectedArticleMediator<M>::~wnSelectedArticleMediator() { }
template <class M, int N> wnArticleManageModuleBuilder<M, N>::~wnArticleManageModuleBuilder() { }
template wnArticleManageModuleBuilder<wnSnakeNikitaMediator, 2>::~wnArticleManageModuleBuilder();
template wnSelectedArticleMediator<wnSnakeNikitaMediator>::~wnSelectedArticleMediator();
template wnArticleSubPool<wnSnakeNikitaMissile, 2>::~wnArticleSubPool();
template wnArticleSubPool<wnSnakeNikitaMissile, 1>::~wnArticleSubPool();
#pragma dont_inline off

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

// MATCH-ONLY: stands in for the native weak Vec3f::operator-() (map: Vec3f::__mi, a paired-single negation) until the
// shared mt_vector.h declares that member.
asm Vec3f operator-(const Vec3f&) {
    nofralloc
    psq_l f1, 0x0(r4), 0, 0
    psq_l f0, 0x8(r4), 1, 0
    ps_neg f1, f1
    ps_neg f0, f0
    psq_st f1, 0x0(r3), 0, 0
    psq_st f0, 0x8(r3), 1, 0
    blr
}

// Shows the launcher in Snake's hands: link attribute 4 on node 3, motion 0. When both missile slots are taken the
// oldest missile (first recorded task id) is made to fall (status 2) and dropped from the list.
void wnSnakeNikita::activate(int founderTaskId, int resourceId, int team, float lr) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    soMotionChangeParam motion;
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, pos, lr,
                        team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);

    m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(4), true);
    motion.m_kind = 0;
    motion.m_frame = 0.0f;
    motion.m_rate = 1.0f;
    motion._12 = 0;
    motion._13 = 0;
    motion._14 = 0;
    motion._15 = 0;
    m_moduleAccesser->getMotionModule().changeMotion(&motion);

    if (m_unkMissileTaskIds.isFull() == true) {
        unsigned long& oldestId = m_unkMissileTaskIds.at(0);
        wnSnakeNikitaMissile* missile = dynamic_cast<wnSnakeNikitaMissile*>(gfTaskScheduler::getInstance()->getTask(oldestId));
        if (missile != NULL) {
            missile->changeStatus(2);
        }
        m_unkMissileTaskIds.shift();
    }
}

// Launches a pooled missile from the launcher's node 3 (at the ground's depth). Its rotation is a 90 degree yaw,
// mirrored when the owning fighter (work int 0x10000000, else the launcher itself) faces left.
bool wnSnakeNikita::activeArticle(wnSnakeNikitaMissile* missile, soModuleAccesser* acc) {
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(3, false);
    pos.m_z = acc->getGroundModule().getZ();
    Vec3f rot(0.0f, 90.0f, 0.0f);
    StageObject* owner = dynamic_cast<StageObject*>(gfTaskScheduler::getInstance()->getTaskById(
        gfTask::Category_Fighter, acc->getWorkManageModule().getInt(0x10000000)));
    if (owner != NULL) {
        if (soExternalValueAccesser::getLr(owner) < 0.0f) {
            rot = -rot;
        }
    } else if (acc->getPostureModule().getLr() < 0.0f) {
        rot = -rot;
    }
    missile->activate(acc->getStageObject().getTaskId(), acc->getResourceModule().getResourceIdAccesser()->getAnmResId(),
                      acc->getTeamModule().getTeam()->getNo(), pos, rot);
    return true;
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
            m_unkMissileTaskIds.push(static_cast<unsigned long>(missile->m_taskId));
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

// ---- the launcher's article mediator: article slot 0 is the pool of two missiles, slots 1-16 are unused ------------

// The pool activates a missile through its launcher.
struct wnSnakeNikitaActivator {
    static bool activate(wnSnakeNikitaMissile* missile, soModuleAccesser* acc) {
        return wnSnakeNikita::activeArticle(missile, acc);
    }
};

soArticle* wnSnakeNikitaMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<wnSnakeNikitaActivator>(m_pools, acc, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}
bool wnSnakeNikitaMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnSnakeNikitaMissile, 2, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(false)
    default: return false;
    }
}
s32 wnSnakeNikitaMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnSnakeNikitaMissile, 2, soInstancePoolRoot>, wnArticleSlotCounter>(this, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}
s32 wnSnakeNikitaMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(2, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}
bool wnSnakeNikitaMediator::shoot(soModuleAccesser*, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnSnakeNikitaMissile>(article, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(true)
    default: return false;
    }
}
void wnSnakeNikitaMediator::deactivate() {
    for (s32 i = 0; i < 2; ++i) {
        if (!static_cast<soArticle&>(*m_pools.getSub().getInstanceAt(i)).setDeactivateDescendant()) return;
    }
}
s32 wnSnakeNikitaMediator::getMediateNum() { return 1; }
void wnSnakeNikitaMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }
