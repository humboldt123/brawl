#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_GKOOPA_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#define MT_VEC3F_CTOR_NOINLINE // BYTES: the breath offset goes through the out-of-line constructor
#include <ft/ft_event_unit_instance.h>
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/koopa/ft_gkoopa.h>
#include <ft/ft_extend_param_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <so/so_slow.h>
#include <ft/koopa/ft_koopa_special_s.h>
#include <mt/mt_prng.h>
#include <so/jostle/so_jostle_module.h>
#include <ft/ft_change_succeed_work.h>

#undef WN_ARTICLE_NATIVE_BUILDERS
#define FT_BC ftGKoopaBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// The extend parameter groups of Giga Bowser: Bowser's four float groups one slot later (group 0 holds the transformation
// timer ints). MATCH-ONLY: the Cast accessors reload the group pointer for the first entry of a group, as Bowser's do.
class ftGKoopaExtendParamAccesser : public ftExtendParamAccesserEx<3999, 46, 23999, 10> {
    const u8* group0(const u8* base) { return *(const u8* const*)(base + 0x7C); }
    const u8* group0Cast(const u8* base) { return *(const u8**)(base + 0x7C); }
    const u8* group1(const u8* base) { return *(const u8* const*)(base + 0x80); }
    const u8* group1Cast(const u8* base) { return *(const u8**)(base + 0x80); }
    const u8* group2(const u8* base) { return *(const u8* const*)(base + 0x84); }
    const u8* group2Cast(const u8* base) { return *(const u8**)(base + 0x84); }
    const u8* group3(const u8* base) { return *(const u8* const*)(base + 0x88); }
    const u8* group3Cast(const u8* base) { return *(const u8**)(base + 0x88); }
    const u8* group4(const u8* base) { return *(const u8* const*)(base + 0x8C); }
    const u8* group4Cast(const u8* base) { return *(const u8**)(base + 0x8C); }

public:
    ftGKoopaExtendParamAccesser() : ftExtendParamAccesserEx(Fighter_GKoopa) { }
    virtual ~ftGKoopaExtendParamAccesser() { }

    virtual void setup(const u8* extData) {
        for (s32 i = 0; i < NumVariations; i++) {
            m_floats[i][0] = (const float*)(group1Cast(extData) + 0x0);
            m_floats[i][1] = (const float*)(group1(extData) + 0x4);
            m_floats[i][2] = (const float*)(group1(extData) + 0x8);
            m_floats[i][3] = (const float*)(group1(extData) + 0xC);
            m_floats[i][4] = (const float*)(group1(extData) + 0x10);
            m_floats[i][5] = (const float*)(group1(extData) + 0x14);
            m_floats[i][6] = (const float*)(group1(extData) + 0x1C);
            m_floats[i][7] = (const float*)(group1(extData) + 0x20);
            m_floats[i][8] = (const float*)(group1(extData) + 0x24);
            m_floats[i][9] = (const float*)(group1(extData) + 0x28);
            m_floats[i][10] = (const float*)(group1(extData) + 0x2C);
            m_floats[i][11] = (const float*)(group1(extData) + 0x30);
            m_floats[i][12] = (const float*)(group1(extData) + 0x34);
            m_floats[i][13] = (const float*)(group1(extData) + 0x38);
            m_floats[i][14] = (const float*)(group1(extData) + 0x44);
            m_floats[i][15] = (const float*)(group1(extData) + 0x48);
            m_floats[i][16] = (const float*)(group1(extData) + 0x4C);
            m_floats[i][17] = (const float*)(group2Cast(extData) + 0x0);
            m_floats[i][18] = (const float*)(group2(extData) + 0x4);
            m_floats[i][19] = (const float*)(group2(extData) + 0x8);
            m_floats[i][20] = (const float*)(group2(extData) + 0xC);
            m_floats[i][21] = (const float*)(group2(extData) + 0x10);
            m_floats[i][22] = (const float*)(group2(extData) + 0x14);
            m_floats[i][23] = (const float*)(group2(extData) + 0x18);
            m_floats[i][24] = (const float*)(group2(extData) + 0x1C);
            m_floats[i][25] = (const float*)(group3Cast(extData) + 0x0);
            m_floats[i][26] = (const float*)(group3(extData) + 0x4);
            m_floats[i][27] = (const float*)(group3(extData) + 0x8);
            m_floats[i][28] = (const float*)(group3(extData) + 0xC);
            m_floats[i][29] = (const float*)(group3(extData) + 0x10);
            m_floats[i][30] = (const float*)(group3(extData) + 0x14);
            m_floats[i][31] = (const float*)(group3(extData) + 0x18);
            m_floats[i][32] = (const float*)(group3(extData) + 0x1C);
            m_floats[i][33] = (const float*)(group4Cast(extData) + 0x0);
            m_floats[i][34] = (const float*)(group4(extData) + 0x4);
            m_floats[i][35] = (const float*)(group4(extData) + 0x8);
            m_floats[i][36] = (const float*)(group4(extData) + 0xC);
            m_floats[i][37] = (const float*)(group4(extData) + 0x10);
            m_floats[i][38] = (const float*)(group4(extData) + 0x14);
            m_floats[i][39] = (const float*)(group4(extData) + 0x18);
            m_floats[i][40] = (const float*)(group4(extData) + 0x1C);
            m_floats[i][41] = (const float*)(group4(extData) + 0x20);
            m_floats[i][42] = (const float*)(group4(extData) + 0x24);
            m_floats[i][43] = (const float*)(group4(extData) + 0x28);
            m_floats[i][44] = (const float*)(group4(extData) + 0x2C);
            m_floats[i][45] = (const float*)(group4(extData) + 0x30);
            m_ints[i][0] = (const int*)(group0Cast(extData) + 0x0);
            m_ints[i][1] = (const int*)(group0(extData) + 0x4);
            m_ints[i][2] = (const int*)(group0(extData) + 0x8);
            m_ints[i][3] = (const int*)(group1(extData) + 0x18);
            m_ints[i][4] = (const int*)(group1(extData) + 0x3C);
            m_ints[i][5] = (const int*)(group1(extData) + 0x40);
            m_ints[i][6] = (const int*)(group2(extData) + 0x20);
            m_ints[i][7] = (const int*)(group2(extData) + 0x24);
            m_ints[i][8] = (const int*)(group2(extData) + 0x28);
            m_ints[i][9] = (const int*)(group2(extData) + 0x2C);
        }
    }
};

template <class T> T (CLAMP)(T value, T minimum, T maximum);

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_102_bss_4FC;
extern soStatusUniqProcess lbl_102_bss_50C;
extern soStatusUniqProcess lbl_102_bss_51C;
extern soStatusUniqProcess lbl_102_bss_52C;
extern soStatusUniqProcess lbl_102_bss_53C;
extern soStatusUniqProcess lbl_102_bss_54C;
extern soStatusUniqProcess lbl_102_bss_55C;
extern soStatusUniqProcess lbl_102_bss_56C;
extern soStatusUniqProcess lbl_102_bss_57C;
}
extern soStatusUniqProcess g_ftGKoopaStatusUniqProcessBreath;

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off

ftGKoopaExtendParamAccesser g_ftGKoopaExtendParamAccesser;

ftClassInfoImpl<Fighter_GKoopa, ftGKoopa> g_ftClassInfoGKoopa;

ftGKoopa::ftGKoopa(s32 entryId,
                   Heaps::HeapType instHeap,
                   Heaps::HeapType nwModelInstHeap,
                   Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftGKoopaBuildConfig>(entryId,
                                          Fighter_GKoopa,
                                          instHeap,
                                          nwModelInstHeap,
                                          nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_GKoopa)) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[18] = {0};
    processes[0] = &g_ftGKoopaStatusUniqProcessBreath;
    processes[5] = &lbl_102_bss_4FC;
    processes[6] = &lbl_102_bss_50C;
    processes[7] = &lbl_102_bss_51C;
    processes[8] = &lbl_102_bss_52C;
    processes[10] = &lbl_102_bss_53C;
    processes[11] = &lbl_102_bss_53C;
    processes[12] = &lbl_102_bss_54C;
    processes[13] = &lbl_102_bss_55C;
    processes[14] = &lbl_102_bss_56C;
    processes[15] = &lbl_102_bss_57C;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 18);
}

// ---- the fighter overrides ------------------------------------------------------------------------------------------
// HYPOTHESIS: as for Bowser, the works 0x10000041 / 0x11000013 / 0x11000014 hold the breath state; Giga Bowser also starts
// with super armor (the damage module), a timer (0x10000040, counted down by processUpdate) and clears his transformation
// flags.
void ftGKoopa::onStart(int startKind) {
    soModuleAccesser* acc = m_moduleAccesser;
    soWorkManageModule& work = acc->getWorkManageModule();
    work.setInt(-1, 0x10000041);
    work.setFloat(soValueAccesser::getConstantFloat(acc, 0xfa6, 0), 0x11000014);
    work.setFloat(soValueAccesser::getConstantFloat(acc, 0xfa8, 0), 0x11000013);
    work.offFlag(0x1200003d);
    work.offFlag(0x1200003e);
    Fighter::onStart(startKind);
    acc->getDamageModule().setNoReactionModePerfect(true);
    work.onFlag(0x12000021);
    work.onFlag(0x12000024);
    work.onFlag(0x12000025);
    work.setInt(0, 0x10000040);
    work.setFloat(0.0f, 0x11000004);
    static_cast<soJostleModule*>(acc->m_enumerationStart->m_jostleModule)->setByte4A(true);
    work.offFlag(0x12000020);
    work.offFlag(0x1200002c);
}

// When the transformation ends while Giga Bowser is down, Bowser takes over again.
void ftGKoopa::onDeadEnd() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (acc->getWorkManageModule().getInt(0x10000040) > 0 && static_cast<u8>(g_ftManager->getFighterCount(m_entryId)) > 1) {
        int fighterNo = g_ftManager->getFighterNo(m_entryId, m_taskId);
        acc->getMotionModule().setFrame(0.0f);
        u8 other = (u8)fighterNo == 0 ? 1 : 0;
        g_ftManager->toChange(m_entryId, other, 0, 0);
        int entryId = m_entryId;
        ftManager* manager = g_ftManager;
        manager->setControllerRumble(entryId, 6, soValueAccesser::getConstantInt(acc, 0x5a38, 0), 0, m_taskId);
    }
    Fighter::onDeadEnd();
}

void ftGKoopa::toKnockOut() {
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    if (work.getInt(0x10000040) > 2) {
        work.setInt(2, 0x10000040);
    }
}

void ftGKoopa::processUpdate() {
    Fighter::processUpdate();
    if (soSlow::getInstance()->isEstimate() == true && !m_moduleAccesser->getStopModule().isStop()) {
        float first = m_moduleAccesser->getWorkManageModule().getFloat(0x11000014);
        float second = m_moduleAccesser->getWorkManageModule().getFloat(0x11000013);
        if (m_moduleAccesser->getStatusModule().getStatusKind() != 0x112) {
            first += soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfaa, 0);
            second += soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfab, 0);
        } else if (m_moduleAccesser->getWorkManageModule().getInt(0x20000002) == 1) {
            first -= soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfac, 0);
            second -= soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfad, 0);
        }
        float firstMinimum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa7, 0);
        float firstMaximum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa6, 0);
        first = CLAMP<float>(first, firstMinimum, firstMaximum);
        m_moduleAccesser->getWorkManageModule().setFloat(first, 0x11000014);
        float secondMinimum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa9, 0);
        float secondMaximum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa8, 0);
        second = CLAMP<float>(second, secondMinimum, secondMaximum);
        m_moduleAccesser->getWorkManageModule().setFloat(second, 0x11000013);
    }
    // The transformation timer: at 1 (outside the end status 0x123) the end starts; it counts down otherwise, and the
    // flash of the first extend parameter group starts at its marked frame.
    soModuleAccesser* acc = m_moduleAccesser;
    soWorkManageModule& work = acc->getWorkManageModule();
    int status = acc->getStatusModule().getStatusKind();
    int timer = work.getInt(0x10000040);
    if (timer == 1 && status != 0x123) {
        timer = 2;
    }
    if (timer > 1 && status != 0x122) {
        if (timer == 2) {
            acc->getMotionModule().setFrame(0.0f);
            work.setInt(1, 0x10000040);
            acc->getStatusModule().changeStatusForce(0x123, acc);
        } else {
            int* param = static_cast<int*>(g_ftCommonDataAccesser.getData(Fighter_GKoopa)->extendParam[0]);
            if (timer == param[1] + param[2]) {
                acc->getCollisionHitModule().setInvincibleFrameGlobal(param[0], false, 0);
            }
            work.setInt(timer - 1, 0x10000040);
        }
    }
}

bool ftGKoopa::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    switch (acmd->getType()) {
    case 5:
        // HYPOTHESIS: command 5 switches back to Bowser (the end of the transformation).
        if (static_cast<u8>(g_ftManager->getFighterCount(m_entryId)) > 1) {
            int fighterNo = g_ftManager->getFighterNo(m_entryId, m_taskId);
            u8 other = (u8)fighterNo == 0 ? 1 : 0;
            g_ftManager->toChange(m_entryId, other, 1, 0);
        }
        return true;
    }
    return Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
}

// HYPOTHESIS: the events of the Flying Slam: 8 with the catch request 0xee grabs the victim during the start statuses
// (0x11c/0x11d), 0xe releases it from the slam statuses (0x11e-0x121).
struct ftGKoopaLinkEventView {
    int kind;
    bool done;
    int value;
    int request;
};

void ftGKoopa::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftGKoopaLinkEventView* event = reinterpret_cast<ftGKoopaLinkEventView*>(eventInfo);
    switch (event->kind) {
    case 8:
        if (event->request == 0xee) {
            switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
            case 0x11c:
            case 0x11d:
                event->value = m_moduleAccesser->getModelModule().getCorrectNodeId(0x12c);
                event->done = true;
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x11e, m_moduleAccesser);
                m_moduleAccesser->getWorkManageModule().setInt(m_moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000002);
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000011);
                return;
            }
        }
        break;
    case 0xe:
        switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
        case 0x11e:
        case 0x11f:
        case 0x120:
        case 0x121:
            if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
                getCatchView(m_moduleAccesser).unk38(0);
                m_moduleAccesser->getWorkManageModule().offFlag(0x22000011);
                m_moduleAccesser->getWorkManageModule().offFlag(0x12000001);
                if (m_moduleAccesser->getStatusModule().getStatusKind() != 0x121 ||
                    m_moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
                    if (m_moduleAccesser->getWorkManageModule().isFlag(0x1200003e)) {
                        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003e);
                    } else if (m_moduleAccesser->getSituationModule().getKind() == 0) {
                        m_moduleAccesser->getStatusModule().changeStatusRequest(0, m_moduleAccesser);
                    } else {
                        m_moduleAccesser->getStatusModule().changeStatusRequest(0xe, m_moduleAccesser);
                    }
                }
            }
            return;
        }
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

// HYPOTHESIS: Giga Bowser ignores the touch of item kind 0x39.
void ftGKoopa::touchItem(soLinkTouchItemEventArgs* args) {
    switch (reinterpret_cast<StageObject*>(args->m_item)->soGetSubKind()) {
    case 0x39:
        return;
    default:
        Fighter::touchItem(args);
        break;
    }
}

// The breath starts at the mouth (node 0x30 moved by the offset of the breath parameters) with the charge of the two
// breath works, a random spread around the angle of the work 0x21000004, and the team of the owner.
bool ftGKoopa::activeArticle(wnGKoopaBreath* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    ftKind kind = static_cast<ftKind>(acc->getStageObject().soGetSubKind());
    Vec3f pos = acc->getPostureModule().getPos();
    float lr = acc->getPostureModule().getLr();
    // HYPOTHESIS: the scale is read and not used (the call stays in the native code).
    float scale = acc->getPostureModule().getScale();
    float speedMul = 1.0f;
    float* param = static_cast<float*>(g_ftCommonDataAccesser.getGKoopaBreathParam(kind));
    float angle = acc->getWorkManageModule().getFloat(0x21000004);
    pos = acc->getModelModule().getNodeGlobalPosition(0x30, &Vec3f(0.0f, param[0x48 / 4], param[0x44 / 4]), false, false);
    float speed = acc->getWorkManageModule().getFloat(0x11000014) / soValueAccesser::getConstantFloat(acc, 0xfa6, 0);
    float size = acc->getWorkManageModule().getFloat(0x11000013) / soValueAccesser::getConstantFloat(acc, 0xfa8, 0);
    float baseAngle = soValueAccesser::getConstantFloat(acc, 0xfa0, 0);
    float spread = soValueAccesser::getConstantFloat(acc, 0xfa1, 0);
    angle = baseAngle + angle;
    angle += randf() * spread - 0.5f * spread;
    weapon->activate(acc->getStageObject().m_taskId, fighter.getOwner()->getTeam(), &pos, 0, lr, speed, size, speedMul, 0.017453292f * angle);
    return false;
}

// The fighter that takes over (the transformation partner) is started with the work of this one; the transformation
// timer restarts from the first extend parameter group.
void ftGKoopa::change(void* work, int startKind) {
    onStart(startKind);
    int* param = static_cast<int*>(g_ftCommonDataAccesser.getData(Fighter_GKoopa)->extendParam[0]);
    m_moduleAccesser->getWorkManageModule().setInt(param[1] + param[2] + 1, 0x10000040);
    Fighter::changeSucceedCore(work, 0x122);
}

// HYPOTHESIS: the collision points of the dummy fighter that takes over the move (as Zero Suit Samus's).
struct ftGKoopaChangeSucceedPoints {
    float point[4][2];
};
// The empty out-of-line constructor of the point blocks (Bowser's unit carries it under its placeholder name).
// MATCH-ONLY: called by name instead of through a constructor.
extern "C" void fn_102_8A2C(ftGKoopaChangeSucceedPoints* points);

void ftGKoopa::setupChangeSucceedWork(void* work) {
    Fighter::setupChangeSucceedWork(work);
    soModuleAccesser* acc = m_moduleAccesser;
    ftGKoopaChangeSucceedPoints hurt;
    fn_102_8A2C(&hurt);
    hurt.point[0][0] = 0.0f;
    hurt.point[1][0] = soValueAccesser::getConstantFloat(acc, 0xbd6, 0);
    hurt.point[2][0] = 0.0f;
    hurt.point[3][0] = soValueAccesser::getConstantFloat(acc, 0xbd8, 0);
    hurt.point[0][1] = 0.0f;
    hurt.point[1][1] = soValueAccesser::getConstantFloat(acc, 0xbd1, 0);
    hurt.point[2][1] = 0.0f;
    hurt.point[3][1] = 0.0f;
    ftGKoopaChangeSucceedPoints hit;
    fn_102_8A2C(&hit);
    hit.point[0][0] = 0.0f;
    hit.point[1][0] = soValueAccesser::getConstantFloat(acc, 0xbbc, 0);
    hit.point[2][0] = 0.0f;
    hit.point[3][0] = -1.0f;
    hit.point[0][1] = 0.0f;
    hit.point[1][1] = 0.0f;
    hit.point[2][1] = 0.0f;
    hit.point[3][1] = 0.0f;
    u32 option = 0;
    if (acc->getStatusModule().getStatusKind() != 0x10b) {
        option = 0xc000c;
    }
    Vec2f* speed = &acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(0xFFFF));
    soPostureModule& posture = acc->getPostureModule();
    static_cast<ftChangeSucceedWork*>(work)->activateDummyFighter(&posture.getPos(), speed, &hit.point[0][0], &hurt.point[0][0], option, posture.getScale(), posture.getLr());
}

// ---- the article holders --------------------------------------------------------------------------------------------
#include <ft/builder/ft_builder_article.h>

struct ftGKoopaArticleKind : wnArticleKindInfo {
    ftKind getSubKind() const { return static_cast<ftKind>(subKind); }
    ftGKoopaArticleKind() {
        kind = 0;
        subKind = Fighter_GKoopa;
    }
};

// The data block of the breath comes from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_1497CC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

FT_DEFINE_ARTICLE_HOLDER(, wnGKoopaBreath, wnGKoopaBreathData, 0, ftGKoopaArticleKind, fn_27_1497CC)

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnGKoopaBreath>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftGKoopaArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftGKoopaArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftGKoopaArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftGKoopaArticleMediator::~ftGKoopaArticleMediator() { }
#pragma dont_inline off

s32 ftGKoopaArticleMediator::getMediateNum() { return 1; }
void ftGKoopaArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Breath (the rest are unused).

#define FT_GKOOPA_POOL_BREATH_BASE static_cast<wnArticlePool<wnGKoopaBreath, 6, soInstancePoolRoot>&>(m_pools)
#define FT_GKOOPA_POOL_BREATH FT_GKOOPA_POOL_BREATH_BASE.getSub()

#define FT_GKOOPA_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(1, value) WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

void ftGKoopaArticleMediator::deactivate() {
    for (s32 i = 0; i < 6; ++i) {
        wnGKoopaBreath* weapon = FT_GKOOPA_POOL_BREATH.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftGKoopaArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(6, WN_ARTICLE_SLOT_TAGS);
    FT_GKOOPA_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftGKoopaActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftGKoopaArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnGKoopaBreath, 6, soInstancePoolRoot>, ftGKoopaActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_GKOOPA_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftGKoopaArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnGKoopaBreath, 6, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_GKOOPA_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftGKoopaArticleActivator;
template <> struct ftGKoopaArticleActivator<wnGKoopaBreath> {
    static bool activate(wnGKoopaBreath* weapon, soModuleAccesser* acc) { return ftGKoopa::activeArticle(weapon, acc); }
};

soArticle* ftGKoopaArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftGKoopaArticleActivator<wnGKoopaBreath> >(static_cast<wnArticlePool<wnGKoopaBreath, 6, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_GKOOPA_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftGKoopaArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnGKoopaBreath>(article, WN_ARTICLE_SLOT_TAGS);
    FT_GKOOPA_UNUSED_SLOTS(true)
    default: return false;
    }
}
