#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_NOINLINE_SEARCH_BUILDER
#define FT_SHEIK_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#include <ft/ft_event_unit_instance.h>
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/zelda/ft_sheik.h>
#include <ft/ft_extend_param_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/ft_change_succeed_work.h>
#include <so/article/so_article_mediator.h>
#include <so/collision/so_collision_log.h>
#include <so/so_value_accesser.h>

#undef WN_ARTICLE_NATIVE_BUILDERS
#define FT_BC ftSheikBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// The extend parameter groups of Sheik (thirty floats and nine ints from five groups).
// MATCH-ONLY: the Cast accessors reload the group pointer for the first entry of a group, as Zelda's do.
class ftSheikExtendParamAccesser : public ftExtendParamAccesserEx<3999, 30, 23999, 9> {
    const u8* group1(const u8* base) { return *(const u8* const*)(base + 0x7C); }
    const u8* group1Cast(const u8* base) { return *(const u8**)(base + 0x7C); }
    const u8* group2(const u8* base) { return *(const u8* const*)(base + 0x80); }
    const u8* group2Cast(const u8* base) { return *(const u8**)(base + 0x80); }
    const u8* group3(const u8* base) { return *(const u8* const*)(base + 0x84); }
    const u8* group3Cast(const u8* base) { return *(const u8**)(base + 0x84); }
    const u8* group4(const u8* base) { return *(const u8* const*)(base + 0x88); }
    const u8* group4Cast(const u8* base) { return *(const u8**)(base + 0x88); }
    const u8* group5(const u8* base) { return *(const u8* const*)(base + 0x8C); }
    const u8* group5Cast(const u8* base) { return *(const u8**)(base + 0x8C); }

public:
    ftSheikExtendParamAccesser() : ftExtendParamAccesserEx(Fighter_Sheik) { }
    virtual ~ftSheikExtendParamAccesser() { }

    virtual void setup(const u8* extData) {
        for (s32 i = 0; i < NumVariations; i++) {
            m_floats[i][0] = (const float*)(group1Cast(extData) + 0x0);
            m_floats[i][1] = (const float*)(group1(extData) + 0x4);
            m_floats[i][2] = (const float*)(group1(extData) + 0x8);
            m_floats[i][3] = (const float*)(group1(extData) + 0xC);
            m_floats[i][4] = (const float*)(group1(extData) + 0x10);
            m_floats[i][5] = (const float*)(group2(extData) + 0x18);
            m_floats[i][6] = (const float*)(group3Cast(extData) + 0x0);
            m_floats[i][7] = (const float*)(group3(extData) + 0x4);
            m_floats[i][8] = (const float*)(group3(extData) + 0x8);
            m_floats[i][9] = (const float*)(group3(extData) + 0x14);
            m_floats[i][10] = (const float*)(group3(extData) + 0x18);
            m_floats[i][11] = (const float*)(group3(extData) + 0x1C);
            m_floats[i][12] = (const float*)(group3(extData) + 0x20);
            m_floats[i][13] = (const float*)(group3(extData) + 0x28);
            m_floats[i][14] = (const float*)(group3(extData) + 0x2C);
            m_floats[i][15] = (const float*)(group3(extData) + 0x30);
            m_floats[i][16] = (const float*)(group3(extData) + 0x34);
            m_floats[i][17] = (const float*)(group3(extData) + 0x38);
            m_floats[i][18] = (const float*)(group4Cast(extData) + 0x0);
            m_floats[i][19] = (const float*)(group4(extData) + 0x4);
            m_floats[i][20] = (const float*)(group4(extData) + 0x8);
            m_floats[i][21] = (const float*)(group4(extData) + 0xC);
            m_floats[i][22] = (const float*)(group4(extData) + 0x10);
            m_floats[i][23] = (const float*)(group5Cast(extData) + 0x0);
            m_floats[i][24] = (const float*)(group5(extData) + 0x4);
            m_floats[i][25] = (const float*)(group5(extData) + 0x8);
            m_floats[i][26] = (const float*)(group5(extData) + 0xC);
            m_floats[i][27] = (const float*)(group5(extData) + 0x10);
            m_floats[i][28] = (const float*)(group5(extData) + 0x14);
            m_floats[i][29] = (const float*)(group5(extData) + 0x18);
            m_ints[i][0] = (const int*)(group2Cast(extData) + 0x0);
            m_ints[i][1] = (const int*)(group2(extData) + 0x4);
            m_ints[i][2] = (const int*)(group2(extData) + 0x8);
            m_ints[i][3] = (const int*)(group2(extData) + 0xC);
            m_ints[i][4] = (const int*)(group2(extData) + 0x10);
            m_ints[i][5] = (const int*)(group2(extData) + 0x14);
            m_ints[i][6] = (const int*)(group3(extData) + 0xC);
            m_ints[i][7] = (const int*)(group3(extData) + 0x10);
            m_ints[i][8] = (const int*)(group3(extData) + 0x24);
        }
    }
};

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off

extern soStatusUniqProcess g_ftSheikStatusUniqProcessSpecialN;
extern soStatusUniqProcess g_ftSheikStatusUniqProcessSpecialNShoot;
extern soStatusUniqProcess g_ftSheikStatusUniqProcessSpecialSStart;
extern soStatusUniqProcess g_ftSheikStatusUniqProcessSpecialSLoop;
extern soStatusUniqProcess g_ftSheikStatusUniqProcessSpecialHiStart;
extern soStatusUniqProcess g_ftSheikStatusUniqProcessSpecialHiMove;
extern soStatusUniqProcess g_ftSheikStatusUniqProcessSpecialHiEnd;
extern soStatusUniqProcess g_ftSheikStatusUniqProcessSpecialLw;
extern soStatusUniqProcess g_ftZeldaStatusUniqProcessFinal;

ftSheikExtendParamAccesser g_ftSheikExtendParamAccesser;

ftClassInfoImpl<Fighter_Sheik, ftSheik> g_ftClassInfoSheik;

ftSheik::ftSheik(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftSheikBuildConfig>(entryId,
                                         Fighter_Sheik,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Sheik)) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[13] = {0};
    processes[0] = &g_ftSheikStatusUniqProcessSpecialN;
    processes[1] = &g_ftSheikStatusUniqProcessSpecialSStart;
    processes[2] = &g_ftSheikStatusUniqProcessSpecialHiStart;
    processes[3] = &g_ftSheikStatusUniqProcessSpecialLw;
    processes[4] = &g_ftZeldaStatusUniqProcessFinal;
    processes[5] = &g_ftSheikStatusUniqProcessSpecialN;
    processes[6] = &g_ftSheikStatusUniqProcessSpecialNShoot;
    processes[7] = &g_ftSheikStatusUniqProcessSpecialN;
    processes[8] = &g_ftSheikStatusUniqProcessSpecialSLoop;
    processes[10] = &g_ftSheikStatusUniqProcessSpecialHiMove;
    processes[11] = &g_ftSheikStatusUniqProcessSpecialHiEnd;
    processes[12] = &g_ftSheikStatusUniqProcessSpecialLw;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 13);
}

void** ftSheik::getExtendParam() {
    return m_data->extendParam;
}

// HYPOTHESIS: the work 0x10000040 counts the frames since the transformation (carried over by changeSucceedCore) and the
// effect 0x1a is the glow of the change.
void ftSheik::onStart(int startKind) {
    Fighter::onStart(startKind);
    m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    m_moduleAccesser->getEffectModule().removeCommon(0x1a);
}

// The chain (subkind 0x46) tells the fighter when it caught a ledge (0x83b), when it is pulled back (0x83c) and when
// it ends (0x83e), as Link's hookshot does.
struct ftSheikLinkEventView {
    int kind;
    bool done;
    int value;
};

void ftSheik::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftSheikLinkEventView* event = reinterpret_cast<ftSheikLinkEventView*>(eventInfo);
    switch (stageObj->soGetSubKind()) {
    case 0x46:
        switch (event->kind) {
        case 0x83b: {
            int* constant = static_cast<int*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA80E, 0));
            moduleAccesser->getWorkManageModule().setInt(*constant, 0x20000001);
            moduleAccesser->getWorkManageModule().setInt(2, 0x20000000);
            moduleAccesser->getStatusModule().changeStatusRequest(0x81, moduleAccesser);
            if (moduleAccesser->getGenerateArticleManageModule().isExist(2) == true) {
                soLinkModule& link = moduleAccesser->getLinkModule();
                if (link.link(3, moduleAccesser->getGenerateArticleManageModule().getArticle(2)->getTaskId()) == true) {
                    soLinkModule& linkAgain = moduleAccesser->getLinkModule();
                    linkAgain.setModelConstraintPosOrt(3, moduleAccesser->getModelModule().getCorrectNodeId(0x12e), event->value, 5, false);
                }
            }
            break;
        }
        case 0x83c:
            if (moduleAccesser->getGroundModule().isStCliff(0)) {
                moduleAccesser->getStatusModule().changeStatusRequest(0x75, moduleAccesser);
            } else {
                moduleAccesser->getStatusModule().changeStatusRequest(0x7b, moduleAccesser);
            }
            break;
        case 0x83e:
            moduleAccesser->getStatusModule().changeStatusRequest(0x83, moduleAccesser);
            break;
        }
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

bool ftSheik::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    switch (acmd->getType()) {
    case 5:
        // HYPOTHESIS: command 5 switches to Zelda (the transformation).
        if (static_cast<u8>(g_ftManager->getFighterCount(m_entryId)) > 1) {
            int fighterNo = g_ftManager->getFighterNo(m_entryId, m_taskId);
            u8 other = (u8)fighterNo == 0 ? 1 : 0;
            g_ftManager->toChange(m_entryId, other, 1, 0);
        }
        return true;
    }
    return Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
}

// The needles remember up to eight hit targets (task, collision group and index), as Zelda's search does.
void ftSheik::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    int count = work.getInt(0x20000000);
    if (count < 8) {
        work.setInt(collisionLog->m_taskId, 0x20000001 + count);
        work.setInt(collisionLog->_35, 0x20000009 + count);
        work.setInt(collisionLog->m_collsionIndex, 0x20000011 + count);
        ++count;
    }
    work.setInt(count, 0x20000000);
}

bool ftSheik::notifyEventCollisionSearchCheck() {
    return Fighter::notifyEventCollisionSearchCheck();
}

void ftSheik::change(void* work, int startKind) {
    onStart(startKind);
    changeSucceedCore(work, 0x11e);
}

// The frame count is carried over from Zelda; from the sixth frame on the transformation glow is requested again.
void ftSheik::changeSucceedCore(void* work, int status) {
    Fighter::changeSucceedCore(work, status);
    soModuleAccesser* acc = m_moduleAccesser;
    soWorkManageModule& workModule = acc->getWorkManageModule();
    workModule.setInt(static_cast<ftChangeSucceedWork*>(work)->unk100[0], 0x10000040);
    if (workModule.getInt(0x10000040) >= 6) {
        workModule.onFlag(0x1200003d);
        acc->getEffectModule().reqCommon(0.0f, 0x1a);
    }
}

// HYPOTHESIS: the collision points of the dummy fighter that takes over the move (as Zero Suit Samus's), with the hurt
// box of the fourth extend parameter group.
struct ftSheikChangeSucceedPoints {
    float point[4][2];
};
// The empty out-of-line constructor of the point blocks (Zelda's unit carries it under its placeholder name).
// MATCH-ONLY: called by name instead of through a constructor.
extern "C" void fn_104_A024(ftSheikChangeSucceedPoints* points);

void ftSheik::setupChangeSucceedWork(void* work) {
    Fighter::setupChangeSucceedWork(work);
    soModuleAccesser* acc = m_moduleAccesser;
    float* param = static_cast<float*>(m_data->extendParam[3]);
    ftSheikChangeSucceedPoints hurt;
    fn_104_A024(&hurt);
    hurt.point[0][0] = 0.0f;
    hurt.point[1][0] = soValueAccesser::getConstantFloat(acc, 0xbd6, 0);
    hurt.point[2][0] = 0.0f;
    hurt.point[3][0] = soValueAccesser::getConstantFloat(acc, 0xbd8, 0);
    hurt.point[0][1] = -param[2];
    hurt.point[1][1] = soValueAccesser::getConstantFloat(acc, 0xbd1, 0);
    hurt.point[2][1] = param[3];
    hurt.point[3][1] = param[3];
    ftSheikChangeSucceedPoints hit;
    fn_104_A024(&hit);
    hit.point[0][0] = 0.0f;
    hit.point[1][0] = soValueAccesser::getConstantFloat(acc, 0xbbc, 0);
    hit.point[2][0] = 0.0f;
    hit.point[3][0] = -1.0f;
    hit.point[0][1] = 0.0f;
    hit.point[1][1] = 0.0f;
    hit.point[2][1] = 0.0f;
    hit.point[3][1] = 0.0f;
    Vec2f* speed = &acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(0xFFFF));
    soPostureModule& posture = acc->getPostureModule();
    static_cast<ftChangeSucceedWork*>(work)->activateDummyFighter(&posture.getPos(), speed, &hit.point[0][0], &hurt.point[0][0], 0xe0013, posture.getScale(), posture.getLr());
    static_cast<ftChangeSucceedWork*>(work)->unk100[0] = acc->getWorkManageModule().getInt(0x10000040);
}

// The chain starts from the fighter with its resource, team and facing, and is linked to the fighter (link 6).
// HYPOTHESIS: the last argument tells the chain it was thrown from the air status 0x10c.
bool ftSheik::activeArticle(wnSheikIronWire* weapon, soModuleAccesser* acc) {
    bool fromAir = acc->getStatusModule().getStatusKind() == 0x10c;
    soTeamModule& teams = acc->getTeamModule();
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    float lr = acc->getPostureModule().getLr();
    int team = teams.getTeam()->getNo();
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getMdlArchiveId(), team, lr, fromAir);
    acc->getLinkModule().link(6, weapon->m_taskId);
    return true;
}

// The vanish (Fusin) starts at the fighter, moved by the offset of the third extend parameter group.
bool ftSheik::activeArticle1(wnSheikFusin* weapon, soModuleAccesser* acc) {
    ftSheik& sheik = dynamic_cast<ftSheik&>(acc->getStageObject());
    Vec3f pos = acc->getPostureModule().getPos();
    pos.m_x += static_cast<float*>(sheik.m_data->extendParam[2])[0x34 / 4];
    pos.m_y += static_cast<float*>(sheik.m_data->extendParam[2])[0x38 / 4];
    soTeamModule& teams = acc->getTeamModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    float lr = acc->getPostureModule().getLr();
    weapon->activate(founderTaskId, teams.getTeam()->getNo(), &pos, lr);
    return false;
}

// The light bow of the Final Smash starts at the stage object of the fighter with its team.
bool ftSheik::activeArticle2(wnZeldaLightingBow* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(), acc->getTeamModule().getTeam()->getNo(), 0x1e);
    return true;
}

// The light arrow of the Final Smash starts the same way.
bool ftSheik::activeArticle3(wnZeldaLightingBowArrow* weapon, soModuleAccesser* acc) {
    // HYPOTHESIS: the unused cast is in the native code.
    dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(), acc->getTeamModule().getTeam()->getNo(), 0x39);
    return true;
}

// ---- the article holders --------------------------------------------------------------------------------------------
#include <ft/builder/ft_builder_article.h>

struct ftSheikArticleKind : wnArticleKindInfo {
    ftKind getSubKind() const { return static_cast<ftKind>(subKind); }
    ftSheikArticleKind() {
        kind = 0;
        subKind = Fighter_Sheik;
    }
};

// The data blocks of the held needle, the chain and the vanish come from the common data accessor of sora_melee (only
// declared here).
extern "C" void* fn_27_148CE8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148D7C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148D90(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

FT_DEFINE_ARTICLE_HOLDER_MEMBER(, wnSheikNeedle, const wnSheikNeedleData, 0, ftSheikArticleKind, getWeaponData<wnSheikNeedleData>)
FT_DEFINE_ARTICLE_HOLDER(inline, wnSheikNeedleHave, wnSheikNeedleHaveData, 1, ftSheikArticleKind, fn_27_148CE8)
FT_DEFINE_ARTICLE_HOLDER(inline, wnSheikIronWire, wnSheikIronWireData, 2, ftSheikArticleKind, fn_27_148D7C)
FT_DEFINE_ARTICLE_HOLDER(inline, wnSheikFusin, wnSheikFusinData, 3, ftSheikArticleKind, fn_27_148D90)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(inline, wnZeldaLightingBow, const wnZeldaLightingBowData, 4, ftSheikArticleKind, getWeaponData<wnZeldaLightingBowData>)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(inline, wnZeldaLightingBowArrow, const wnZeldaLightingBowArrowData, 5, ftSheikArticleKind, getWeaponData<wnZeldaLightingBowArrowData>)

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnSheikNeedle>::~wnArticleHolder();
template wnArticleHolder<wnSheikNeedleHave>::~wnArticleHolder();
template wnArticleHolder<wnSheikIronWire>::~wnArticleHolder();
template wnArticleHolder<wnSheikFusin>::~wnArticleHolder();
template wnArticleHolder<wnZeldaLightingBow>::~wnArticleHolder();
template wnArticleHolder<wnZeldaLightingBowArrow>::~wnArticleHolder();
template wnArticleHolder<wnSheikNeedle>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftSheikArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftSheikArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftSheikArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftSheikArticleMediator::~ftSheikArticleMediator() { }
#pragma dont_inline off

s32 ftSheikArticleMediator::getMediateNum() { return 6; }
void ftSheikArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Needle, 1 NeedleHave, 2 IronWire, 3 Fusin, 4 LightingBow, 5 LightingBowArrow (the rest are unused).

#define FT_SHEIK_POOL(W, N, BASE) static_cast<wnArticlePool<W, N, BASE>&>(m_pools)

#define FT_SHEIK_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

#define FT_SHEIK_DEACTIVATE(W, N, BASE) \
    for (s32 i = 0; i < N; ++i) { \
        W* weapon = FT_SHEIK_POOL(W, N, BASE).getSub().getInstanceAt(i); \
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) { \
            return; \
        } \
    }

void ftSheikArticleMediator::deactivate() {
    FT_SHEIK_DEACTIVATE(wnSheikNeedle, 8, ftSheikNeedleHavePool)
    FT_SHEIK_DEACTIVATE(wnSheikNeedleHave, 1, ftSheikIronWirePool)
    FT_SHEIK_DEACTIVATE(wnSheikIronWire, 1, ftSheikFusinPool)
    FT_SHEIK_DEACTIVATE(wnSheikFusin, 1, ftSheikLightingBowPool)
    FT_SHEIK_DEACTIVATE(wnZeldaLightingBow, 1, ftSheikLightingBowArrowPool)
    FT_SHEIK_DEACTIVATE(wnZeldaLightingBowArrow, 1, soInstancePoolRoot)
}

s32 ftSheikArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(8, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    FT_SHEIK_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftSheikActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftSheikArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnSheikNeedle, 8, ftSheikNeedleHavePool>, ftSheikActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnSheikNeedleHave, 1, ftSheikIronWirePool>, ftSheikActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotActiveNum<wnArticlePool<wnSheikIronWire, 1, ftSheikFusinPool>, ftSheikActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotActiveNum<wnArticlePool<wnSheikFusin, 1, ftSheikLightingBowPool>, ftSheikActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotActiveNum<wnArticlePool<wnZeldaLightingBow, 1, ftSheikLightingBowArrowPool>, ftSheikActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotActiveNum<wnArticlePool<wnZeldaLightingBowArrow, 1, soInstancePoolRoot>, ftSheikActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_SHEIK_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftSheikArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnSheikNeedle, 8, ftSheikNeedleHavePool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnSheikNeedleHave, 1, ftSheikIronWirePool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotIsGeneratable<wnArticlePool<wnSheikIronWire, 1, ftSheikFusinPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotIsGeneratable<wnArticlePool<wnSheikFusin, 1, ftSheikLightingBowPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotIsGeneratable<wnArticlePool<wnZeldaLightingBow, 1, ftSheikLightingBowArrowPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotIsGeneratable<wnArticlePool<wnZeldaLightingBowArrow, 1, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_SHEIK_UNUSED_SLOTS(false)
    default: return false;
    }
}

// HYPOTHESIS: the needles start through their own activation (no fighter hook).
template <class W> struct ftSheikArticleActivator {
    static bool activate(W*, soModuleAccesser*) { return true; }
};
template <> struct ftSheikArticleActivator<wnSheikIronWire> {
    static bool activate(wnSheikIronWire* weapon, soModuleAccesser* acc) { return ftSheik::activeArticle(weapon, acc); }
};
template <> struct ftSheikArticleActivator<wnSheikFusin> {
    static bool activate(wnSheikFusin* weapon, soModuleAccesser* acc) { return ftSheik::activeArticle1(weapon, acc); }
};
template <> struct ftSheikArticleActivator<wnZeldaLightingBow> {
    static bool activate(wnZeldaLightingBow* weapon, soModuleAccesser* acc) { return ftSheik::activeArticle2(weapon, acc); }
};
template <> struct ftSheikArticleActivator<wnZeldaLightingBowArrow> {
    static bool activate(wnZeldaLightingBowArrow* weapon, soModuleAccesser* acc) { return ftSheik::activeArticle3(weapon, acc); }
};

soArticle* ftSheikArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftSheikArticleActivator<wnSheikNeedle> >(FT_SHEIK_POOL(wnSheikNeedle, 8, ftSheikNeedleHavePool), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftSheikArticleActivator<wnSheikNeedleHave> >(FT_SHEIK_POOL(wnSheikNeedleHave, 1, ftSheikIronWirePool), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerate<ftSheikArticleActivator<wnSheikIronWire> >(FT_SHEIK_POOL(wnSheikIronWire, 1, ftSheikFusinPool), acc, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerate<ftSheikArticleActivator<wnSheikFusin> >(FT_SHEIK_POOL(wnSheikFusin, 1, ftSheikLightingBowPool), acc, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotGenerate<ftSheikArticleActivator<wnZeldaLightingBow> >(FT_SHEIK_POOL(wnZeldaLightingBow, 1, ftSheikLightingBowArrowPool), acc, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotGenerate<ftSheikArticleActivator<wnZeldaLightingBowArrow> >(FT_SHEIK_POOL(wnZeldaLightingBowArrow, 1, soInstancePoolRoot), acc, WN_ARTICLE_SLOT_TAGS);
    FT_SHEIK_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftSheikArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnSheikNeedle>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotShoot<wnSheikNeedleHave>(article, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotShoot<wnSheikIronWire>(article, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotShoot<wnSheikFusin>(article, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotShoot<wnZeldaLightingBow>(article, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotShoot<wnZeldaLightingBowArrow>(article, WN_ARTICLE_SLOT_TAGS);
    FT_SHEIK_UNUSED_SLOTS(true)
    default: return false;
    }
}
