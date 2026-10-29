#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_TEAM_TYPED_INTERFACE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_SHARED_STATUS_TABLE_CTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/yoshi/ft_yoshi_builder.h>
#include <ft/yoshi/ft_yoshi_extend_param_accesser.h>
#include <ft/ft_common_data_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_heap_module.h>
#include <so/so_value_accesser.h>
#include <so/article/so_generate_article_manage_module.h>
#include <ft/ft_owner.h>
#include <ft/yoshi/ft_yoshi_special_lw_param.h>
#include <ft/yoshi/ft_yoshi_special_hi_param.h>
#include <ft/yoshi/ft_yoshi_special_s_utility.h>
#include <so/catch/so_catch_module.h>
#include <wn/yoshi/wn_yoshi_star.h>
#include <wn/yoshi/wn_yoshi_tamago.h>
#include <wn/yoshi/wn_yoshi_wing.h>
#include <wn/wn_article_pool.h>
#include <ft/yoshi/ft_yoshi_article_calls.h>
#include <ft/yoshi/ft_yoshi_final_param.h>
#include <ft/yoshi/ft_yoshi_transactor.h>
#include <mt/mt_matrix.h>
#include <math.h>

namespace ftyoshi { template <class T> T ABS(T); }
// Existing rotate-only matrix transform; unlike mulPos, it omits translation.
extern "C" void fn_8003E3BC(const Matrix*, const Vec3f*, Vec3f*);

#define FT_BC ftYoshiBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

ftYoshiExtendParamAccesser g_ftYoshiExtendParamAccesser;

ftClassInfoImpl<Fighter_Yoshi, ftYoshi> g_ftClassInfoYoshi;

// Registration only uses the globals through their primary soStatusUniqProcess base,
// as with Ness and Lucas. Their concrete classes live in the status units.
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessSpecialSStart;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessSpecialHi;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessSpecialLw;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessFinalStart;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessSpecialNSwallow;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessSpecialSLoop;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessSpecialSEnd;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessSpecialSTurn;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessSpecialAirSJump;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessSpecialAirLw;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessFinalCommon;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessGuardOn;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessGuard;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessGuardDamage;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessEscapeFB;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessJumpAerial;
extern soStatusUniqProcess g_ftStatusUniqProcessAttackAirInheritJumpAerialMotion;
extern soStatusUniqProcess g_ftStatusUniqProcessEscapeAirInheritJumpAerialMotion;
extern soStatusUniqProcess g_ftYoshiStatusUniqProcessCatchPull;

ftYoshi::ftYoshi(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftYoshiBuildConfig>(entryId,
                                         Fighter_Yoshi,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Yoshi)) {
    // Null slots install no character-specific process. Other entries install
    // the special moves and Super Dragon phases in action order.
    soStatusUniqProcess* processes[19] = {0};
    processes[1] = &g_ftYoshiStatusUniqProcessSpecialSStart;
    processes[2] = &g_ftYoshiStatusUniqProcessSpecialHi;
    processes[3] = &g_ftYoshiStatusUniqProcessSpecialLw;
    processes[4] = &g_ftYoshiStatusUniqProcessFinalStart;
    processes[5] = &g_ftYoshiStatusUniqProcessSpecialNSwallow;
    processes[6] = &g_ftYoshiStatusUniqProcessSpecialNSwallow;
    processes[7] = &g_ftYoshiStatusUniqProcessSpecialSLoop;
    processes[8] = &g_ftYoshiStatusUniqProcessSpecialSEnd;
    processes[9] = &g_ftYoshiStatusUniqProcessSpecialSTurn;
    processes[10] = &g_ftYoshiStatusUniqProcessSpecialAirSJump;
    processes[11] = &g_ftYoshiStatusUniqProcessSpecialAirLw;
    processes[12] = &g_ftYoshiStatusUniqProcessSpecialAirLw;
    processes[14] = &g_ftYoshiStatusUniqProcessFinalCommon;
    processes[15] = &g_ftYoshiStatusUniqProcessFinalCommon;
    processes[16] = &g_ftYoshiStatusUniqProcessFinalCommon;
    processes[17] = &g_ftYoshiStatusUniqProcessFinalCommon;
    processes[18] = &g_ftYoshiStatusUniqProcessFinalCommon;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 19);

    // Yoshi's egg shield, aerial jump and tongue capture override common
    // fighter statuses as well as the dedicated special-move action range.
    m_moduleAccesser->getStatusModule().setUniqProc(Fighter::Status::Guard_On, &g_ftYoshiStatusUniqProcessGuardOn);
    m_moduleAccesser->getStatusModule().setUniqProc(Fighter::Status::Guard, &g_ftYoshiStatusUniqProcessGuard);
    m_moduleAccesser->getStatusModule().setUniqProc(Fighter::Status::Guard_Damage, &g_ftYoshiStatusUniqProcessGuardDamage);
    m_moduleAccesser->getStatusModule().setUniqProc(Fighter::Status::Escape_F, &g_ftYoshiStatusUniqProcessEscapeFB);
    m_moduleAccesser->getStatusModule().setUniqProc(Fighter::Status::Escape_B, &g_ftYoshiStatusUniqProcessEscapeFB);
    m_moduleAccesser->getStatusModule().setUniqProc(Fighter::Status::Jump_Aerial, &g_ftYoshiStatusUniqProcessJumpAerial);
    m_moduleAccesser->getStatusModule().setUniqProc(Fighter::Status::Attack_Air, &g_ftStatusUniqProcessAttackAirInheritJumpAerialMotion);
    m_moduleAccesser->getStatusModule().setUniqProc(Fighter::Status::Escape_Air, &g_ftStatusUniqProcessEscapeAirInheritJumpAerialMotion);
    m_moduleAccesser->getStatusModule().setUniqProc(Fighter::Status::Catch_Pull, &g_ftYoshiStatusUniqProcessCatchPull);
    m_moduleAccesser->getStatusModule().setUniqProc(Fighter::Status::Catch_Dash_Pull, &g_ftYoshiStatusUniqProcessCatchPull);
}

void** ftYoshi::getExtendParam() { return m_data->extendParam; }

void ftYoshi::onDeactivate() {
    deleteGuardColorAnim();
}

void ftYoshi::onStart(int kind) {
    Fighter::onStart(kind);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.setInt(0, 0x10000040);
    ftYoshiSpecialHiParam* param = static_cast<ftYoshiSpecialHiParam*>(m_data->extendParam[1]);
    work.setFloat(param->fallSpeedMax, 0x11000013);
}

void ftYoshi::processUpdate() {
    Fighter::processUpdate();
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    // The airborne Egg Throw boost loses strength across repeated uses. Touching
    // ground replenishes both the use counter and the stored boost value.
    if (work.getInt(0x10000040) != 0 &&
        m_moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
        work.setInt(0, 0x10000040);
        ftYoshiSpecialHiParam* param = static_cast<ftYoshiSpecialHiParam*>(m_data->extendParam[1]);
        work.setFloat(param->fallSpeedMax, 0x11000013);
    }
}

// HYPOTHESIS: original link-event type and field spelling. The Yoshi event-8 receiver
// establishes the inherited acknowledgment byte and node at +8.
struct ftYoshiCaptureLinkEvent : soLinkEventArgs {
    int captureNode;
};

void ftYoshi::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc,
                            StageObject* sender, int selector) {
    soStatusModule& status = acc->getStatusModule();
    soModelModule& model = acc->getModelModule();
    switch (event->m_eventKind) {
    case 8:
        if (status.getStatusKind() == 0x112) {
            ftYoshiCaptureLinkEvent& capture = static_cast<ftYoshiCaptureLinkEvent&>(*event);
            capture.captureNode = model.getCorrectNodeId(0x12C);
            capture._0x4 = true;
            soCatchModule& catchModule = *static_cast<soCatchModule*>(acc->m_enumerationStart->m_catchModule);
            catchModule.setCatch(-1);
            // HYPOTHESIS: 0x117 transfers Egg Lay's captured target to the egg state.
            status.changeStatusRequest(0x117, acc);
            break;
        }
        // Other capture states retain the common fighter link response.
    default:
        Fighter::notifyEventLink(event, acc, sender, selector);
        break;
    }
}

void ftYoshi::notifyEventCollisionAttackFighter(soCollisionLog*, soModuleAccesser* acc) {
    int status = acc->getStatusModule().getStatusKind();
    if (status == 0x113 || static_cast<unsigned>(status - 0x119) <= 3)
        ftYoshiStatusUniqProcessSpecialSUtility::procHit(acc);
}

void ftYoshi::notifyEventChangeSituation(SituationKind kind, SituationKind previous,
                                       soModuleAccesser* acc) {
    int jumpCount = m_moduleAccesser->getWorkManageModule().getInt(0x10000001);
    Fighter::notifyEventChangeSituation(kind, previous, acc);
    if (acc->getStatusModule().getStatusKind() == Fighter::Status::Jump_Aerial) {
        // Native performs this lookup before restoring the saved jump count;
        // its return value is unused. Keep the call's observable side effects.
        soValueAccesser::getConstantInt(acc, 0x59DB, 0);
        m_moduleAccesser->getWorkManageModule().setInt(jumpCount, 0x10000001);
    }
}

// MATCH-ONLY: native addresses the local static's guard, object, and
// destructor registration separately, as in R.O.B.'s transactor singleton.
#pragma pool_data off
bool ftYoshi::activeArticle1(wnYoshiTamago* article, soModuleAccesser* acc) {
    static ftYoshiTransactor transactor;
    u32 resourceId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
    return transactor.activeArticle1(article, acc, resourceId);
}

#pragma pool_data reset

bool ftYoshi::activeArticle(wnYoshiStar* article, soModuleAccesser* acc) {
    ftYoshiSpecialLwParam* param = static_cast<ftYoshiSpecialLwParam*>(
        g_ftCommonDataAccesser.getData(Fighter_Yoshi)->extendParam[2]);
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    Vec3f position = acc->getModelModule().getNodeGlobalPosition(1, false);
    soGenerateArticleManageModule& articles = *static_cast<soGenerateArticleManageModule*>(
        acc->m_enumerationStart->m_generateArticleManageModule);
    // The second landing star travels away from the opposite side of Yoshi.
    float direction = articles.isExist(0) ? -1.0f : 1.0f;
    position.m_x += param->starOffsetX * direction;
    position.m_y += param->starOffsetY;
    int founderTaskId = acc->getStageObject().m_taskId;
    article->activate(founderTaskId, fighter.getOwner()->getTeam(), &position, direction);
    return true;
}

bool ftYoshi::activeArticle(wnYoshiTamago* article, soModuleAccesser* acc) {
    ftYoshiSpecialHiParam* param = static_cast<ftYoshiSpecialHiParam*>(
        g_ftCommonDataAccesser.getData(Fighter_Yoshi)->extendParam[1]);
    soWorkManageModule& work = acc->getWorkManageModule();
    soControllerModule& input = acc->getControllerModule();
    float scale = acc->getPostureModule().getScale();
    float lr = acc->getPostureModule().getLr();
    Vec3f position = acc->getPostureModule().getPos();
    position.m_x += lr * (param->throwOffsetX * scale);
    position.m_y += param->throwOffsetY * scale;

    // Horizontal input tilts the arc away from its default angle. Keep the
    // ordered comparisons, including their behavior for non-finite input.
    float stick = input.getStickX();
    float angle = ftyoshi::ABS(stick) / param->throwStickFull;
    if (angle > 1.0f) angle = 1.0f;
    angle *= param->throwStickAngle;
    if (angle < param->throwAngleDeadzone) angle = 0.0f;
    angle *= stick < 0.0f ? -1.0f : 1.0f;
    if (lr == 1.0f) angle = param->throwBaseAngle - angle;
    else angle = (3.1415927f - param->throwBaseAngle) - angle;

    // HYPOTHESIS: this animation-command work value is throw strength/hold
    // time. The egg consumes the last argument as a spin factor, not life.
    float strength = work.getInt(0x20000000);
    Vec2f speed;
    speed.m_x = (param->throwSpeed + strength * param->throwStrengthSpeed) * static_cast<float>(cos(angle));
    speed.m_y = (param->throwSpeed + strength * param->throwStrengthSpeed) * static_cast<float>(sin(angle));
    float spin = param->throwSpin + strength * param->throwStrengthSpin;
    article->shoot(&position, &speed, spin);
    return false;
}

bool ftYoshi::activeArticle(wnYoshiWing* article, soModuleAccesser* acc) {
    int founderTaskId;
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resource = acc->getResourceModule();
    founderTaskId = fighter.m_taskId;
    article->activate(founderTaskId, resource.getResourceIdAccesser()->getMdlResId(),
                      fighter.getOwner()->getTeam());
    return true;
}

bool ftYoshi::activeArticle(wnYoshiFireBreath* article, soModuleAccesser* acc) {
    // MATCH-ONLY: declare the saved ID before the other persistent locals.
    int founderTaskId;
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    ftYoshiFinalParam* param = static_cast<ftYoshiFinalParam*>(
        g_ftCommonDataAccesser.getData(Fighter_Yoshi)->extendParam[3]);
    float lr = acc->getPostureModule().getLr();
    Vec3f position;
    Vec3f mouthOffset(0.0f, param->unk44, param->unk40);
    Matrix mouthMatrix(true);
    acc->getModelModule().getNodeGlobalMtx(0x26, &mouthMatrix, false);
    fn_8003E3BC(&mouthMatrix, &mouthOffset, &position);
    Vec3f mouthPosition = acc->getModelModule().getNodeGlobalPosition(0x26, false);
    Vec3fAdd(&position, &position, &mouthPosition);
    soWorkManageModule& work = acc->getWorkManageModule();
    float angle = work.getFloat(0x21000008);
    founderTaskId = fighter.m_taskId;
    lr = acc->getPostureModule().getLr();
    article->activate(founderTaskId, fighter.getOwner()->getTeam(), &position, lr, angle);

    // Successive flames sweep around a center angle. Reverse the next step
    // after reaching either edge, retaining the native overshoot for this shot.
    float step = work.getFloat(0x21000009);
    angle += param->unk3C * step;
    if (ftyoshi::ABS(param->unk34 - angle) >= param->unk38) step = -step;
    work.setFloat(angle, 0x21000008);
    work.setFloat(step, 0x21000009);
    return false;
}

bool ftYoshi::activeArticle(wnYoshiFireCannon* article, soModuleAccesser* acc) {
    // MATCH-ONLY: declare the saved ID before the other persistent locals.
    int founderTaskId;
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    ftYoshiFinalParam* param = static_cast<ftYoshiFinalParam*>(
        g_ftCommonDataAccesser.getData(Fighter_Yoshi)->extendParam[3]);
    float lr = acc->getPostureModule().getLr();
    Vec3f position = acc->getModelModule().getNodeGlobalPosition(0x26, false);
    position.m_x += param->unk50 * lr;
    position.m_y += param->unk54;
    founderTaskId = fighter.m_taskId;
    article->activate(founderTaskId, 0xFFFF, fighter.getOwner()->getTeam(), &position, lr, param->unk4C);
    return false;
}

// Guard statuses create this animation once and remove it on exit. The work
// flag is kept separate from the bundle's own resource-ownership flags.
void ftYoshi::createGuardColorAnim() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (!acc->getWorkManageModule().isFlag(0x1200003D)) {
        soResourceModule& resource = acc->getResourceModule();
        nw4r::g3d::ResFile animation(resource.getFile(
            resource.getResourceIdAccesser()->getMdlResId(), Data_Type_Anim, 0));
        const soModuleEnumeration& modules = *acc->m_enumerationStart;
        soHeapModule& heap = *static_cast<soHeapModule*>(modules.m_heapModule);
        nw4r::g3d::ResMdl model = modules.m_modelModule->getScnMdl()->m_resMdl;
        nw4r::g3d::ResMdl modelCopy(model);
        nw4r::g3d::ResFile animationCopy(animation);
        m_guardColorAnim.create(animationCopy, modelCopy, heap.getNWMotionInstanceHeap(), NULL);
        m_guardColorAnim.set(acc->getModelModule().getScnMdl());
        acc->getWorkManageModule().onFlag(0x1200003D);
    }
}

void ftYoshi::deleteGuardColorAnim() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (acc->getWorkManageModule().isFlag(0x1200003D)) {
        m_guardColorAnim.remove();
        m_guardColorAnim.destroy();
        acc->getWorkManageModule().offFlag(0x1200003D);
    }
}

void ftYoshi::updateGuardColorAnim() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (acc->getWorkManageModule().isFlag(0x1200003D)) {
        soWorkManageModule& work = acc->getWorkManageModule();
        // HYPOTHESIS: constant 0xCA1 is the reference shield strength. Native
        // code maps the depleted fraction to a 0..100 animation frame without
        // clamping; preserve values outside that range as well.
        float referenceStrength = soValueAccesser::getConstantFloat(acc, 0xCA1, 0);
        float shieldStrength = work.getFloat(0x11000003);
        float frame = 100.0f * (1.0f - shieldStrength / referenceStrength);
        m_guardColorAnim.setFrame(frame);
    }
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftYoshi is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftYoshiInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftYoshiInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtual overrides and default observers (ftEntryEventObserver, Fighter, StageObject).
void fn_95_B29C() {}
void fn_95_B424() {}
void fn_95_B430() {}
void fn_95_B470() {}
void fn_95_B474() {}
void fn_95_B504() {}
void fn_95_B530() {}
void fn_95_B534() {}
void fn_95_B538() {}
void fn_95_B53C() {}
void fn_95_B540() {}
void fn_95_B544() {}
void fn_95_B548() {}
void fn_95_B54C() {}
void fn_95_B550() {}
void fn_95_B554() {}
void fn_95_B558() {}
void fn_95_B55C() {}
void fn_95_B560() {}
void fn_95_B564() {}
void fn_95_B568() {}
void fn_95_B574() {}
void fn_95_B578() {}
void fn_95_B57C() {}
void fn_95_B580() {}
void fn_95_B5AC() {}
void fn_95_B5B0() {}
void fn_95_B5B4() {}
void fn_95_B5B8() {}
void fn_95_B5BC() {}
void fn_95_B5E0() {}
void fn_95_B5E4() {}
void fn_95_B5E8() {}
void fn_95_B5FC() {}
void fn_95_B600() {}
void fn_95_B604() {}
void fn_95_B608() {}
void fn_95_B60C() {}
void fn_95_B610() {}

// Constant returns.
int fn_95_AF98() { return 0; }
int fn_95_B56C() { return 0; }
int fn_95_B5D8() { return 0; }
int fn_95_B5EC() { return 0; }
int fn_95_B5F4() { return 0; }
int fn_95_B9B8() { return 0; }

// Field and sub-object accessors.
u8 fn_95_8008(u8* p) { return *(u8*)(p + 0x4); }
u8 fn_95_B428(u8* p) { return *(u8*)(p + 0x44); }
int fn_95_B5C0(u8* p) { return *(int*)(p + 0x110); }
u8* fn_95_B614(u8* p, u32 idx) { return p + 4 + (idx << 3); }
u8* fn_95_B6D4(u8* p) { return p + 0x458; }
u8* fn_95_B6DC(u8* p) { return p + 0x3C8; }
u8* fn_95_B6E4(u8* p) { return p + 0x8; }
u8* fn_95_B6EC(u8* p) { return p + 0x84; }
u8* fn_95_B6F4(u8* p) { return p + 0x70; }
u8* fn_95_B6FC(u8* p) { return p + 0x5C; }
u8* fn_95_B704(u8* p) { return p + 0x48; }
u8* fn_95_B70C(u8* p) { return p + 0x34; }
u8* fn_95_B714(u8* p) { return p + 0x20; }
u8* fn_95_B71C(u8* p) { return p + 0x8; }


// Bit flags in the u32 array at +0x1C.
bool fn_95_B800(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_95_B81C(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_95_B834(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_95_B848(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_95_B890(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_95_B8A8(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_95_B8C0(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_95_B8D8(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_95_B8E8(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_95_B900(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_95_B918(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_95_B930(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_95_B950(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_95_B968(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_95_B980(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_95_B998(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }
int fn_95_B9A8(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); return a[idx]; }

// Work-size getters.
int fn_95_B784(u8* p) { return *(int*)(p + 0x20); }
int fn_95_B860(u8* p) { return *(int*)(p + 0x18); }
int fn_95_B8F8(u8* p) { return *(int*)(p + 0x10); }

// Virtual pointer blocks and methods on the ftYoshi fighter.


// Virtual destructor wrappers without a base destructor call: delete the object when flags > 0.
void* fn_95_1EE4(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_1F80(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_95_233C(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_237C(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_37C8(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_3864(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_3CC8(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_40B4(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_45D8(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_6138(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_6178(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_61B8(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_95_727C(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}

// Wrapper that destroys the Acmd interpreter sub-object, then deletes when flags > 0.
void __dt__16acCmdInterpreterFv(void* p, s16 flags);
void* fn_95_22E0(void* p, s16 flags) {
    if (p != 0) {
        __dt__16acCmdInterpreterFv(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}

// Empty and constant-return slots from the same stage-side observer block.
void fn_95_E65C() {}
void fn_95_E744() {}
void fn_95_E82C() {}
void fn_95_E914() {}
void fn_95_E9FC() {}
void fn_95_EAE4() {}
void fn_95_EBCC() {}
void fn_95_ECB4() {}
int fn_95_E708() { return 0; }
int fn_95_E7F0() { return 0; }
int fn_95_E8D8() { return 0; }
int fn_95_E9C0() { return 0; }
int fn_95_EAA8() { return 0; }
int fn_95_EB90() { return 0; }
int fn_95_EC78() { return 0; }
int fn_95_ED60() { return 0; }

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
void fn_95_EF84(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_95_EF8C(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
void fn_95_EFB4(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
bool fn_95_EFC4(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_95_EFCC(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_95_EFD4(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_95_EFDC(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_95_EFF4(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_95_F00C(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_95_F01C(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_95_F024(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
bool fn_95_F02C(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_95_F084(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_95_F08C(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_95_F094(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_95_F09C(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_95_F0A4(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_95_F0AC(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_95_F0B4(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }



// Fighter item event thunks, same adjustment and tail call as above.
void fn_95_F034(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_95_F03C(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_95_F044(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_95_F04C(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_95_F054(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_95_F05C(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_95_F064(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_95_F06C(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_95_F074(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_95_F07C(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }

// Kinetic energy and small leaf helpers.
void fn_95_DECC() {}
void fn_95_E5D8() {}
void fn_95_DFB4(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_95_DFC0(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_95_CFFC(u8* p, u8 v) { *(u8*)(p + 0x19DA0) = v; }
void fn_95_B868(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
int fn_95_CFF4() { return 5; }
int fn_95_E614() { return 8; }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
bool fn_95_EF9C(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_95_EFE4(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_95_EFEC(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_95_EFFC(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_95_F004(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_95_F014(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_95_3DA4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_95_41DC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_95_4938(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_95_4978(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
u8* fn_95_E660(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_E748(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_E830(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_E918(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_EA00(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_EAE8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_EBD0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_95_ECB8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"

// The holders and sub pools of the article pools (soKindInfoGeneric<0, 4>); the mediator destroys them out of line.
#pragma dont_inline on
template wnInstanceHolder<wnYoshiStar, soKindInfoGeneric<0, 4>, soIntToType<0> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnYoshiStar, 1, wnInstanceHolder, soIntToType<0> >, soKindInfoGeneric<0, 4> >;
template class soInstancePoolSub<soInstancePoolInfo<wnYoshiStar, 2, wnInstanceHolder, soIntToType<0> >, soKindInfoGeneric<0, 4> >;
template wnInstanceHolder<wnYoshiTamago, soKindInfoGeneric<0, 4>, soIntToType<1> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnYoshiTamago, 1, wnInstanceHolder, soIntToType<1> >, soKindInfoGeneric<0, 4> >;
template class soInstancePoolSub<soInstancePoolInfo<wnYoshiTamago, 2, wnInstanceHolder, soIntToType<1> >, soKindInfoGeneric<0, 4> >;
template class soInstancePoolSub<soInstancePoolInfo<wnYoshiTamago, 3, wnInstanceHolder, soIntToType<1> >, soKindInfoGeneric<0, 4> >;
template class soInstancePoolSub<soInstancePoolInfo<wnYoshiTamago, 4, wnInstanceHolder, soIntToType<1> >, soKindInfoGeneric<0, 4> >;
template wnInstanceHolder<wnYoshiWing, soKindInfoGeneric<0, 4>, soIntToType<2> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnYoshiWing, 1, wnInstanceHolder, soIntToType<2> >, soKindInfoGeneric<0, 4> >;
#pragma dont_inline off
