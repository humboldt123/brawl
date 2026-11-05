#define MT_VEC3F_SUB_FRESH_REGS
#define MT_VEC3F_CTOR_NOINLINE // BYTES: the original keeps Vec3f(x, y, z) out of line (weak copy at 0xD580), also inside processFixCamera's helper.
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define FT_MODULE_ACCESSER_BUILDER_BASE_BUILD_DATA // the so-level builder takes soModuleAccesserBuildData (ft/builder/ft_module_accesser_builder.h)
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_TEAM_TYPED_INTERFACE
#define FT_MARTH_PHOTO_CALLBACK_NOINLINE // BYTES: soPhotoCallBack's weak destructor (0xAA80) calls cmPhotoCallBack's out of line.
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define FT_NOINLINE_SEARCH_BUILDER
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_SNAKE_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_DATA_SIMPLE_ARTICLE_DATA
#define SO_GLOW_MODULE_ATTACK_API
#define SO_COLOR_BLEND_BUILDER_DTOR_OUT_OF_LINE
#define SO_STATUS_SINGLETON_NULL_ARRAY
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/snake/ft_snake_builder.h>
#include <ft/snake/ft_snake_extend_param_accesser.h>
#include <ft/ft_owner.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/team/so_team.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <mt/mt_prng.h>
#include <gf/gf_task_scheduler.h>
#include <gf/gf_camera.h>
#include <mt/mt_matrix.h>
#include <math.h>

#include <so/glow/so_glow_module_simple.h>
#include <it/item.h>
#include <ft/kirby/ft_kirby_copy_ability_process.h>
#include <cm/cm_controller_ai.h>
#include <cm/cm_camera_controller.h>
#include <so/so_external_value_accesser.h>

#define FT_BC ftSnakeBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// The article destructors call these empty destructors out of line: soParamAccesser (0x3860, same form as
// ft_fox.cpp), soDamageEventObserver (0x38A0) and wnKineticEnergyGravity (0x5438).
#pragma dont_inline on
soParamAccesser::~soParamAccesser() { }
soDamageEventObserver::~soDamageEventObserver() { }
wnKineticEnergyGravity::~wnKineticEnergyGravity() { }
#pragma dont_inline off

// The launcher's and the sight's link-ref controller modules are torn down by a weak copy in this unit (as in ft_mario.cpp).
inline soControllerModuleLinkRef::~soControllerModuleLinkRef() { }
// The articles' sound builder (0x45C4) inlines this empty destructor (same form as ft_mario.cpp).
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
// The launcher's missile task id list is torn down by sora_melee's weak copy.
template <> soArrayVector<unsigned long, 2>::~soArrayVector();

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }
// Weak copy every fighter REL emits (same form as ft_diddy.cpp / ft_marth.cpp).
ftKineticEnergyController::~ftKineticEnergyController() { }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

extern soStatusUniqProcess g_ftSnakeStatusUniqProcessCypherHang;
extern soStatusUniqProcess g_ftSnakeStatusUniqProcessNikitaOperation;
extern soStatusUniqProcess g_ftSnakeStatusUniqProcessGrenade;
extern soStatusUniqProcess g_ftSnakeStatusUniqProcessFinalEntry;
extern soStatusUniqProcess g_ftSnakeStatusUniqProcessFinalSet;
extern soStatusUniqProcess g_ftSnakeStatusUniqProcessFinalEnd;

ftSnakeExtendParamAccesser g_ftSnakeExtendParamAccesser;

ftClassInfoImpl<Fighter_Snake, ftSnake> g_ftClassInfoSnake;

ftSnake::ftSnake(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftSnakeBuildConfig>(entryId,
                                         Fighter_Snake,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Snake)) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[33] = {0};
    processes[5] = &g_ftSnakeStatusUniqProcessCypherHang;
    processes[13] = &g_ftSnakeStatusUniqProcessNikitaOperation;
    processes[16] = &g_ftSnakeStatusUniqProcessGrenade;
    processes[17] = &g_ftSnakeStatusUniqProcessGrenade;
    processes[18] = &g_ftSnakeStatusUniqProcessGrenade;
    processes[19] = &g_ftSnakeStatusUniqProcessGrenade;
    processes[20] = &g_ftSnakeStatusUniqProcessGrenade;
    processes[21] = &g_ftSnakeStatusUniqProcessGrenade;
    processes[22] = &g_ftSnakeStatusUniqProcessGrenade;
    processes[23] = &g_ftSnakeStatusUniqProcessGrenade;
    processes[24] = &g_ftSnakeStatusUniqProcessGrenade;
    processes[25] = &g_ftSnakeStatusUniqProcessGrenade;
    processes[26] = &g_ftSnakeStatusUniqProcessGrenade;
    processes[30] = &g_ftSnakeStatusUniqProcessFinalEntry;
    processes[31] = &g_ftSnakeStatusUniqProcessFinalSet;
    processes[32] = &g_ftSnakeStatusUniqProcessFinalEnd;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 33);
}

// ---- the fighter overrides ------------------------------------------------------------------------------------------
// Work flag 0x1200003d is cleared on start, on damage, on landing and whenever a special move that can leave a pending
// change ends. HYPOTHESIS: it marks a special move in progress (the same flag is cleared at the start of Mario's fighter).
void ftSnake::onActivate() {
    m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
}

void ftSnake::onStart(int startKind) {
    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    Fighter::onStart(startKind);
    // HYPOTHESIS: the work 0x10000033 holds the grenade count of the fighter.
    m_moduleAccesser->getWorkManageModule().setInt(6, 0x10000033);
}

// Snake does nothing when sealed (an empty override of Fighter::analyzeSeal).
void ftSnake::analyzeSeal(void* seal) { }

// The Final Smash gave the model its own scale; the end puts the scale back from the work float.
void ftSnake::onEndFinal() {
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    posture.setScale(work.getFloat(0x1100000b));
}

void ftSnake::notifyEventOnDamage(soDamage* damage, bool flag, soModuleAccesser* moduleAccesser) {
    if (flag == true) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    }
    Fighter::notifyEventOnDamage(damage, flag, moduleAccesser);
}

// Landing (and starting a fall off an edge) ends the special moves that held the flag.
// HYPOTHESIS: the statuses are the ground special moves of Snake.
void ftSnake::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
    if (kind == 0) {
        switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
        case 0x112:
        case 0x113:
        case 0x115:
        case 0x119:
        case 0x11a:
        case 0x11c:
        case 0x11d:
        case 0x11e:
        case 0x11f:
        case 0x120:
        case 0x121:
        case 0x12a:
        case 0x12b:
        case 0x12d:
        case 0x12f:
            m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
            break;
        }
    } else if (kind == 3) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    }
}

void ftSnake::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0x455:
        switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
        case 0x117:
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x118, m_moduleAccesser);
            break;
        }
        m_moduleAccesser->getGenerateArticleManageModule().remove(3, 0);
        break;
    case 0x456:
        if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x11f) {
            m_moduleAccesser->getWorkManageModule().setFlag(true, 0x22000010);
        } else if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x113) {
            m_moduleAccesser->getWorkManageModule().setFlag(true, 0x22000010);
        }
        break;
    case 0x457:
        if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x11f) {
            m_moduleAccesser->getWorkManageModule().setFlag(true, 0x22000011);
        } else if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x113) {
            m_moduleAccesser->getWorkManageModule().setFlag(true, 0x22000011);
        }
        break;
    case 6:
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

// While the Final Smash grenade launcher is raised (statuses 0x130-0x132) the model gets its node SRT from the posture, the
// head node (10) turns with the aim and moves with the work floats.
// HYPOTHESIS: the statuses and the meaning of node 10.
void ftSnake::updateNodeSRT() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x130:
    case 0x131:
    case 0x132: {
        soModuleAccesser* acc = m_moduleAccesser;
        float size = acc->getPostureModule().getScale();
        Vec3f scale;
        scale.m_z = size;
        scale.m_y = size;
        scale.m_x = size;
        Vec3f rotation;
        Vec3f position;
        position = acc->getPostureModule().getPos();
        rotation = acc->getPostureModule().getRot(0);
        acc->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
        float aim = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFAE, 0);
        Vec3f headRot;
        headRot.m_x = 0.0f;
        headRot.m_y = aim;
        headRot.m_z = 0.0f;
        headRot.m_y += m_moduleAccesser->getWorkManageModule().getFloat(0x21000005);
        acc->getModelModule().setNodeRotate(10, &headRot);
        Vec3f headPos;
        headPos.m_x = m_moduleAccesser->getWorkManageModule().getFloat(0x21000004);
        headPos.m_y = 0.0f;
        headPos.m_z = 0.0f;
        acc->getModelModule().setNodeTranslate(10, &headPos);
        break;
    }
    default:
        StageObject::updateNodeSRT();
        break;
    }
}

extern float lbl_27_rodata_EF8; // Shared 30-degree reference camera angle (see ft_snake_status_uniq_process_final_common.cpp).
extern float lbl_27_rodata_EFC;

// While the grenade launcher is raised the fighter follows the camera: he stands at the offset of the constants in front of
// the main camera and turns with it.
// HYPOTHESIS: the offset constants (0xFA9-0xFAB); the x offset is scaled by the camera's aspect ratio.
// HYPOTHESIS: the rotation (degrees) was set through an inline helper; its arguments are evaluated last to first.
static inline float ftSnakeRadToDeg(float rad) {
    return 57.29578f * rad;
}
static inline void ftSnakeSetRot(soPostureModule& posture, float x, float y, float z) {
    posture.setRot(&Vec3f(x, y, z), 0);
}
void ftSnake::processFixCamera() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x130:
    case 0x131:
    case 0x132:
        if (gfCameraManager::getManager() != NULL) {
            if (gfCameraManager::getManager() != NULL) {
                gfCamera* camera = gfCameraManager::getManager()->getCamera(0);
                if (camera != NULL) {
                    Matrix cameraMtx = static_cast<const Matrix&>(camera->m_viewMatrix);
                    Matrix inverse(true);
                    cameraMtx.inverse(&inverse);
                    soModuleAccesser* acc = m_moduleAccesser;
                    Vec3f position;
                    Vec3f offset(soValueAccesser::getConstantFloat(acc, 0xFA9, 0), soValueAccesser::getConstantFloat(acc, 0xFAA, 0),
                                 soValueAccesser::getConstantFloat(acc, 0xFAB, 0));
                    float reference = static_cast<float>(tan(0.017453292f * lbl_27_rodata_EF8));
                    float tangent = static_cast<float>(tan(camera->unkD0));
                    float correction = tangent <= 0.0f ? 1.0f : reference / tangent;
                    offset.m_z *= correction;
                    float divisor = lbl_27_rodata_EFC;
                    offset.m_x *= camera->m_aspect / divisor;
                    inverse.mulPos(&offset, &position);
                    Vec3f rotation;
                    inverse.getRotate(&rotation);
                    m_moduleAccesser->getPostureModule().setPos(&position);
                    ftSnakeSetRot(m_moduleAccesser->getPostureModule(), ftSnakeRadToDeg(rotation.m_x), ftSnakeRadToDeg(rotation.m_y), ftSnakeRadToDeg(rotation.m_z));
                    updateNodeSRT();
                }
            }
        }
        break;
    }
    Fighter::processFixCamera();
}

// The Final Smash sight: the horizontal cursor value turns into the aim (0x21000005) and the sideways shift (0x21000004) of the
// model, each side with its own constants (0xFAC-0xFB0).
// HYPOTHESIS: the work float meanings.
void ftSnake::syncFinalCursorRate(Vec2f* cursor, soModuleAccesser* accesser) {
    if (cursor->m_x < 0.0f) {
        soWorkManageModule& aimWork = accesser->getWorkManageModule();
        aimWork.setFloat(-cursor->m_x * soValueAccesser::getConstantFloat(accesser, 0xFB0, 0), 0x21000005);
        soWorkManageModule& shiftWork = accesser->getWorkManageModule();
        shiftWork.setFloat(cursor->m_x * soValueAccesser::getConstantFloat(accesser, 0xFAD, 0), 0x21000004);
    } else if (cursor->m_x > 0.0f) {
        soWorkManageModule& aimWork = accesser->getWorkManageModule();
        aimWork.setFloat(-cursor->m_x * soValueAccesser::getConstantFloat(accesser, 0xFAF, 0), 0x21000005);
        soWorkManageModule& shiftWork = accesser->getWorkManageModule();
        shiftWork.setFloat(cursor->m_x * soValueAccesser::getConstantFloat(accesser, 0xFAC, 0), 0x21000004);
    } else {
        accesser->getWorkManageModule().setFloat(0.0f, 0x21000005);
        accesser->getWorkManageModule().setFloat(0.0f, 0x21000004);
    }
}

// The event the fighter sends to the C4 when its detonator status is left.
// HYPOTHESIS: 0x839 asks the C4 to detonate.
struct ftSnakeC4Event : public soLinkEventArgs {
    ftSnakeC4Event() : soLinkEventArgs(0x839) { }
};

void ftSnake::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (statusKind == 0x32) {
        wnSnakeTrenchMortar* mortar = dynamic_cast<wnSnakeTrenchMortar*>(moduleAccesser->getGenerateArticleManageModule().getArticle(1));
        if (mortar != NULL) {
            // HYPOTHESIS: the last word of the trench mortar holds the charge taken from the work float.
            *reinterpret_cast<float*>(reinterpret_cast<u8*>(mortar) + 0x7CE4) = moduleAccesser->getWorkManageModule().getFloat(0x21000000);
        }
    } else if (statusKind == 0x121) {
        wnSnakeNikita* nikita = dynamic_cast<wnSnakeNikita*>(moduleAccesser->getGenerateArticleManageModule().getArticle(6));
        if (nikita != NULL) {
            nikita->forceFallMissile();
        }
    }
    if (prevStatusKind == 0x12f && m_moduleAccesser->getGenerateArticleManageModule().isExist(4)) {
        soArticle* c4 = m_moduleAccesser->getGenerateArticleManageModule().getArticle(4);
        if (c4 != NULL && c4->isConstraint(false) == true) {
            ftSnakeC4Event event;
            m_moduleAccesser->getLinkModule().sendEventNodes(3, c4->getTaskId(), event);
        }
    }
}

// Whether the point is within reach of the fighter (the group 1 float of the extend parameters) while he is waiting or
// has just been pushed out of the Final Smash cursor status.
// HYPOTHESIS: the name (from the module map) says the point is the ear of the camera; the semantics are unconfirmed.
bool ftSnake::ckeckChangeEarCloseStatus(Vec3f& position) {
    if (m_moduleAccesser->getStatusModule().getStatusKind() != 0 && m_moduleAccesser->getStatusModule().getStatusKind() != 0x12e) {
        return false;
    }
    Vec3f toPoint = position - m_moduleAccesser->getPostureModule().getPos();
    float lengthSq = toPoint.m_z * toPoint.m_z + (toPoint.m_x * toPoint.m_x + toPoint.m_y * toPoint.m_y);
    float length;
    if ((float)fabs(lengthSq) <= 1.17549435e-38f) {
        length = 0.0f;
    } else {
        length = lengthSq * rsqrtf(lengthSq);
    }
    if (length > *reinterpret_cast<const float*>(static_cast<const u8*>(m_data->extendParam[0]) + 0x20)) {
        return false;
    }
    return true;
}

// HYPOTHESIS: unk3 is the module accesser of the ejected article (its object sits at offset 8, as in the base version).
void ftSnake::notifyArticleEventEject(int unk1, int unk2, int* unk3) {
    if (unk1 == 0xd && unk2 == 1) {
        soModuleAccesser* articleAccesser = reinterpret_cast<soModuleAccesser*>(unk3);
        Vec3f pos = articleAccesser->getPostureModule().getPos();
        if (ckeckChangeEarCloseStatus(pos) == true) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x12e, m_moduleAccesser);
        }
    }
    StageObject::notifyArticleEventEject(unk1, unk2, unk3);
}

// Hitting something with the Final Smash grenade launcher flags the hit unless the target is of kind 2 or 3.
// HYPOTHESIS: the flag (0x22000010) marks that something other than a fighter or an item was hit.
void ftSnake::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser);
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x11a && collisionLog != NULL) {
        StageObject* object = dynamic_cast<StageObject*>(gfTaskScheduler::getInstance()->getTask(collisionLog->m_taskId));
        if (object != NULL && object->soGetKind() != 2 && object->soGetKind() != 3) {
            m_moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        }
    }
}

// One entry of the table of the eating sounds in the common data of the fighter (the table ends with the id -1).
// HYPOTHESIS: the weights add up to one; the entry whose running sum reaches the random number is played.
struct ftSnakeEatSoundEntry {
    u32 m_soundId;
    float m_weight;
};

void ftSnake::playEatSE() {
    int i = 0;
    float roll = randf();
    float sum = 0.0f;
    ftSnakeEatSoundEntry* table;
    u32 id;
    for (; (table = static_cast<ftSnakeEatSoundEntry*>(m_data->extendParam[4]), id = table[i].m_soundId) != 0xFFFFFFFF; i++) {
        sum += table[i].m_weight;
        if (roll <= sum) {
            m_moduleAccesser->getSoundModule().playSE(static_cast<SndID>(id), true, false, 0);
            break;
        }
    }
}

// The camera photo hides the sight of the grenade launcher while the picture is taken and shows it again afterwards.
void ftSnake::photoMoved() {
    m_moduleAccesser->getVisibilityModule().setWhole(0);
    if (m_moduleAccesser->getGenerateArticleManageModule().isExist(9)) {
        m_moduleAccesser->getGenerateArticleManageModule().setVisibilityWhole(9, 0, 0);
    }
}

void ftSnake::photoExit() {
    m_moduleAccesser->getVisibilityModule().setWhole(1);
    if (m_moduleAccesser->getGenerateArticleManageModule().isExist(9)) {
        m_moduleAccesser->getGenerateArticleManageModule().setVisibilityWhole(9, 1, 0);
    }
}

// The land mine starts with the power multiplier of the fighter.
bool ftSnake::activeArticle(wnSnakeMine* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam(), acc->getCollisionAttackModule().getPowerMulStatus());
    return true;
}

// The trench mortar shell starts at the stage object of the fighter with its team and facing.
// HYPOTHESIS: an inline helper; its arguments are evaluated last to first (task id, resources, facing, team).
static inline void ftSnakeActivateMortar(wnSnakeTrenchMortar* weapon, int team, float lr, soResourceModule& resources, int founderTaskId) {
    weapon->activate(resources.getResourceIdAccesser()->getEtcResId(), team, lr, founderTaskId);
}
bool ftSnake::activeArticle1(wnSnakeTrenchMortar* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    ftSnakeActivateMortar(weapon, fighter.getOwner()->getTeam(), acc->getPostureModule().getLr(), acc->getResourceModule(),
                          acc->getStageObject().m_taskId);
    return true;
}

// The rocket starts with the effect resource of the fighter.
bool ftSnake::activeArticle2(wnSnakeRpg7* weapon, soModuleAccesser* acc) {
    int founderTaskId = acc->getStageObject().m_taskId;
    int etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
    weapon->activate(founderTaskId, etcResId);
    return true;
}

// The Cypher starts with the team of the fighter.
// HYPOTHESIS: an inline helper; its arguments are evaluated last to first (task id, resources, team).
static inline void ftSnakeActivateCypher(wnSnakeCypher* weapon, int team, soResourceModule& resources, int founderTaskId) {
    weapon->activate(resources.getResourceIdAccesser()->getEtcResId(), team, founderTaskId);
}
bool ftSnake::activeArticle3(wnSnakeCypher* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    ftSnakeActivateCypher(weapon, fighter.getOwner()->getTeam(), acc->getResourceModule(), acc->getStageObject().m_taskId);
    return true;
}

// The Cypher is shot with the speed of the fighter.
bool ftSnake::shootArticle(wnSnakeCypher* weapon, soModuleAccesser* acc) {
    weapon->shoot(&acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)).m_x);
    return true;
}

// The C4 starts with the team and facing of the fighter.
bool ftSnake::activeArticle4(wnSnakeC4* weapon, soModuleAccesser* acc) {
    soTeamModule& teamModule = acc->getTeamModule();
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    float lr = acc->getPostureModule().getLr();
    int team = teamModule.getTeam()->getNo();
    int etcResId = resources.getResourceIdAccesser()->getEtcResId();
    weapon->activate(founderTaskId, etcResId, team, lr);
    return true;
}

// The C4 detonator starts with the team of the fighter.
bool ftSnake::activeArticle5(wnSnakeC4Switch* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), acc->getTeamModule().getTeam()->getNo());
    return true;
}

// The Nikita missile starts with the team and facing of the fighter.
bool ftSnake::activeArticle6(wnSnakeNikita* weapon, soModuleAccesser* acc) {
    soTeamModule& teamModule = acc->getTeamModule();
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    float lr = acc->getPostureModule().getLr();
    int team = teamModule.getTeam()->getNo();
    int etcResId = resources.getResourceIdAccesser()->getEtcResId();
    weapon->activate(founderTaskId, etcResId, team, lr);
    return true;
}

// The first Snake parameter block (ftData::extendParam[0]): the grenade throw speeds.
// HYPOTHESIS: which throw each pair belongs to (chosen by the motion kind in shootItem).
struct ftSnakeGrenadeParam {
    u8 unk0[8];
    float m_throwSpeedAX; // 0x8: motions 0x1DE and 0x1E4
    float m_throwSpeedAY;
    float m_throwSpeedX;  // 0x10: the other motions
    float m_throwSpeedY;
    float m_throwSpeedBX; // 0x18: motions 0x1DF and 0x1E5
    float m_throwSpeedBY;
};

// A grenade is thrown (status 0x12D) with the speed of the throw motion, mirrored to the facing; a glowing fighter adds
// its glow attack bonus (work int 0x10000040) to the grenade. In any other status the grenade is dropped in place.
// HYPOTHESIS: status 0x12D is the grenade throw and work int 0x10000040 the glow attack bonus.
// HYPOTHESIS: the drop went through an inline helper (its arguments are evaluated last to first: posture, then the vector).
static inline void ftSnakeFallItem(BaseItem* item, Vec3f* speed, soPostureModule& posture) {
    item->fall(speed, posture.getLr());
}
bool ftSnake::shootItem(soArticle* article, bool* flag, soModuleAccesser* acc) {
    BaseItem* item = dynamic_cast<BaseItem*>(article);
    if (item == NULL) {
        return false;
    }
    ftSnake* snake = dynamic_cast<ftSnake*>(&acc->getStageObject());
    if (snake == NULL) {
        return false;
    }
    ftData* data = snake->m_data;
    if (acc->getStatusModule().getStatusKind() == 0x12D) {
        Vec3f speed(static_cast<ftSnakeGrenadeParam*>(data->extendParam[0])->m_throwSpeedX,
                    static_cast<ftSnakeGrenadeParam*>(data->extendParam[0])->m_throwSpeedY, 0.0f);
        switch (acc->getMotionModule().getKind()) {
        case 0x1DE:
        case 0x1E4:
            speed.m_x = static_cast<ftSnakeGrenadeParam*>(data->extendParam[0])->m_throwSpeedAX;
            speed.m_y = static_cast<ftSnakeGrenadeParam*>(data->extendParam[0])->m_throwSpeedAY;
            break;
        case 0x1DF:
        case 0x1E5:
            speed.m_x = static_cast<ftSnakeGrenadeParam*>(data->extendParam[0])->m_throwSpeedBX;
            speed.m_y = static_cast<ftSnakeGrenadeParam*>(data->extendParam[0])->m_throwSpeedBY;
            break;
        }
        if (acc->getPostureModule().getLr() < 0.0f) {
            speed.m_x = -speed.m_x;
        }
        item->throwAttack(acc->getPostureModule().getLr(), &speed, 1.0f);
        if (static_cast<soGlowModule*>(acc->m_enumerationStart->m_glowModule)->isGlow() == true) {
            int bonus = acc->getWorkManageModule().getInt(0x10000040);
            int attack = item->getGlowAttack();
            attack += bonus;
            item->setGlowAttack(attack);
        }
    } else {
        ftSnakeFallItem(item, &Vec3f(0.0f, 0.0f, 0.0f), acc->getPostureModule());
    }
    return true;
}

// The ladder starts with the Final Smash resource of the fighter.
bool ftSnake::activeArticle7(wnSnakeLadder* weapon, soModuleAccesser* acc) {
    int founderTaskId = acc->getStageObject().m_taskId;
    soResourceModule& resources = acc->getResourceModule();
    int finalResId = dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId();
    weapon->activate(founderTaskId, finalResId);
    return true;
}

// The grenade launcher parts start with the Final Smash resource and the team of the fighter.
bool ftSnake::activeArticle8(wnSnakeRgb6* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(), acc->getTeamModule().getTeam()->getNo());
    return true;
}

bool ftSnake::activeArticle9(wnSnakeRgb6Sight* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(), acc->getTeamModule().getTeam()->getNo());
    return true;
}

bool ftSnake::activeArticle10(wnSnakeRgb6Clip* weapon, soModuleAccesser* acc) {
    int founderTaskId = acc->getStageObject().m_taskId;
    soResourceModule& resources = acc->getResourceModule();
    int finalResId = dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId();
    weapon->activate(founderTaskId, finalResId);
    return true;
}

// A grenade launcher round starts at the muzzle of the launcher (article 8) and flies at the point under the sight
// (article 9, its work floats 0x11000001/0x11000002 are the sight position on screen) shifted by the camera target.
// HYPOTHESIS: the sight work float meanings. The pool keeps the round inactive (returns false).
// The task id is read through StageObject::getTaskId() (the m_taskId field form colors registers differently).
bool ftSnake::activeArticle11(wnSnakeRgb6Bullet* weapon, soModuleAccesser* acc) {
    wnSnakeRgb6* launcher = dynamic_cast<wnSnakeRgb6*>(acc->getGenerateArticleManageModule().getArticle(8));
    if (launcher == NULL) {
        return false;
    }
    Vec3f shootPos = launcher->getShootPos();
    wnSnakeRgb6Sight* sight = dynamic_cast<wnSnakeRgb6Sight*>(acc->getGenerateArticleManageModule().getArticle(9));
    if (sight == NULL) {
        return false;
    }
    if (g_cmAIController == NULL) {
        return false;
    }
    gfCamera* camera = CameraController::getInstance()->getCameraInternal();
    if (camera == NULL) {
        return false;
    }
    Vec3f target(soExternalValueAccesser::getWorkFloat(sight, 0x11000001), soExternalValueAccesser::getWorkFloat(sight, 0x11000002), 0.0f);
    Vec3f cameraTarget(0.0f, 0.0f, 0.0f);
    cameraTarget = camera->m_targetPos;
    Vec3fAdd(&target, &target, &cameraTarget);
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().getTaskId();
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(),
                     acc->getTeamModule().getTeam()->getNo(), &shootPos, &target);
    return false;
}

// Snake's copy-ability process (sora_melee; RTTI name ftSnakeTransactor, base ftKirbyCopyAbilityProcess). Its constructor
// (fn_27_3BAEF0), destructor (fn_27_3BAF00) and simple-article starter (fn_27_3BB0B4) are out of line in sora_melee.
// HYPOTHESIS: the starter's name; it activates the simple article at the founder with a zero offset and returns true.
class ftSnakeTransactor : public ftKirbyCopyAbilityProcess {
public:
    ftSnakeTransactor();
    virtual ~ftSnakeTransactor();
    bool activeSimple(wnSimple* weapon, soModuleAccesser* acc, int etcResId, const wnSimpleData* data);
};

// The simple article is started by the shared copy-ability process with the simple data of the fighter kind.
// BYTES: the native function addresses the local static's guard, object and destructor registration separately
// instead of from one pooled BSS base (same as ftSonicTransactor::getInstance).
#pragma pool_data off
bool ftSnake::activeArticle12(wnSimple* weapon, soModuleAccesser* acc) {
    static ftSnakeTransactor transactor;
    soResourceModule& resources = acc->getResourceModule();
    ftData* data = g_ftCommonDataAccesser.getData(Fighter_Snake);
    return transactor.activeSimple(weapon, acc, resources.getResourceIdAccesser()->getEtcResId(), data->simpleArticleData);
}
#pragma pool_data reset

// The second item (slot 14) is let go: in status 0x10F it is thrown with no speed, otherwise it is tossed backwards, up
// and dropped while work flag 0x22000010 is set, down and thrown when it is clear.
// HYPOTHESIS: the status and flag meanings.
// HYPOTHESIS: the two let-go speeds were inline helpers; their vectors sit below the inline zero vector in the frame.
static inline void ftSnakeDropItem(BaseItem* item, soModuleAccesser* acc) {
    Vec3f speed;
    speed.m_x = -1.0f * acc->getPostureModule().getLr();
    speed.m_y = 2.6f;
    speed.m_z = 0.0f;
    item->fall(&speed, acc->getPostureModule().getLr());
}
static inline void ftSnakeTossItem(BaseItem* item, soModuleAccesser* acc) {
    Vec3f speed;
    speed.m_x = -1.0f * acc->getPostureModule().getLr();
    speed.m_y = -0.6f;
    speed.m_z = 0.0f;
    item->throwAttack(acc->getPostureModule().getLr(), &speed, 1.0f);
}
bool ftSnake::shootItem1(soArticle* article, bool* flag, soModuleAccesser* acc) {
    BaseItem* item = dynamic_cast<BaseItem*>(article);
    if (item == NULL) {
        return false;
    }
    ftSnake* snake = dynamic_cast<ftSnake*>(&acc->getStageObject());
    if (snake == NULL) {
        return false;
    }
    if (acc->getStatusModule().getStatusKind() == 0x10F) {
        item->throwAttack(acc->getPostureModule().getLr(), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f);
    } else if (acc->getWorkManageModule().isFlag(0x22000010)) {
        ftSnakeDropItem(item, acc);
    } else {
        ftSnakeTossItem(item, acc);
    }
    return false;
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Snake, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftSnakeArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftSnakeArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
    ftSnakeArticleDataTag& get() { return *this; }
};

struct ftSnakeArticleKind : wnArticleKindInfo {
    ftSnakeArticleKind() {
        kind = 0;
        subKind = Fighter_Snake;
    }
    s32 getSubKind() const { return subKind; } // HYPOTHESIS: accessor spelling (the lookup reads the kind through a call)
};

// The descriptor keeps the address of the kind temporary of the caller.
struct ftSnakeArticleBuildInfo : wnArticleBuildInfo {
    ftSnakeArticleBuildInfo(const wnArticleKindInfo& kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_147F38(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147F4C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147F74(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147F88(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147F9C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147FB0(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147FC4(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147FEC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148000(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148014(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148028(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14803C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

// Every article of the fighter is built from the data of the fighter kind; the data lookup is called in the argument
// list, after the descriptor (the inline holder constructors are spliced into the article manage builder).
#define FT_SNAKE_ARTICLE_HOLDER(W, DATA, ID, GETTER) \
    template <> inline wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc), \
        m_instance(ID, ftSnakeArticleBuildInfo(ftSnakeArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), \
                   static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, ftSnakeArticleKind().getSubKind(), \
                                                   &ftSnakeArticleDataTag().get().m_resourceGroup))) { }

FT_SNAKE_ARTICLE_HOLDER(wnSnakeMine, wnSnakeMineData, 0, fn_27_147F38)
FT_SNAKE_ARTICLE_HOLDER(wnSnakeTrenchMortar, wnSnakeTrenchMortarData, 1, fn_27_147F4C)
FT_SNAKE_ARTICLE_HOLDER(wnSnakeRpg7, wnSnakeRpg7Data, 2, fn_27_147F74)
FT_SNAKE_ARTICLE_HOLDER(wnSnakeCypher, wnSnakeCypherData, 3, fn_27_147F88)
FT_SNAKE_ARTICLE_HOLDER(wnSnakeC4, wnSnakeC4Data, 4, fn_27_147F9C)
FT_SNAKE_ARTICLE_HOLDER(wnSnakeC4Switch, wnSnakeC4SwitchData, 5, fn_27_147FB0)
FT_SNAKE_ARTICLE_HOLDER(wnSnakeNikita, wnSnakeNikitaData, 6, fn_27_147FC4)
FT_SNAKE_ARTICLE_HOLDER(wnSnakeLadder, wnSnakeLadderData, 7, fn_27_147FEC)
FT_SNAKE_ARTICLE_HOLDER(wnSnakeRgb6, wnSnakeRgb6Data, 8, fn_27_148000)
FT_SNAKE_ARTICLE_HOLDER(wnSnakeRgb6Sight, wnSnakeRgb6SightData, 9, fn_27_148014)
FT_SNAKE_ARTICLE_HOLDER(wnSnakeRgb6Clip, wnSnakeRgb6ClipData, 10, fn_27_148028)
FT_SNAKE_ARTICLE_HOLDER(wnSnakeRgb6Bullet, wnSnakeRgb6BulletData, 11, fn_27_14803C)


// The two simple articles (the shared generic article) are built from the simple data of the fighter kind.
struct ftSnakeSimpleKind : wnSimpleKindInfo {
    ftSnakeSimpleKind() : wnSimpleKindInfo(Fighter_Snake) { }
    s32 getSubKind() const { return subKind; } // HYPOTHESIS: accessor spelling, as the article kind's
};
template <>
wnArticleHolder<wnSimple>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(12, wnSimpleConstructionInfo(wnSimpleKindInfo(Fighter_Snake), acc->m_enumerationStart->m_heapModule),
               g_ftCommonDataAccesser.getSimpleData(static_cast<ftKind>(ftSnakeSimpleKind().getSubKind()),
                                                    &ftSnakeArticleDataTag().get().m_resourceGroup), false) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnSnakeMine>::~wnArticleHolder();
template wnArticleHolder<wnSnakeTrenchMortar>::~wnArticleHolder();
template wnArticleHolder<wnSnakeRpg7>::~wnArticleHolder();
template <class BC>
soColorBlendModuleBuilder<BC>::~soColorBlendModuleBuilder() { }
template soColorBlendModuleBuilder<wnSnakeCypherModuleAccesserBuildConfig::ColorBlendModuleBuildConfig>::~soColorBlendModuleBuilder();
template wnArticleHolder<wnSnakeCypher>::~wnArticleHolder();
template wnArticleHolder<wnSnakeC4>::~wnArticleHolder();
template wnArticleHolder<wnSnakeC4Switch>::~wnArticleHolder();
template wnArticleHolder<wnSnakeNikita>::~wnArticleHolder();
template wnArticleHolder<wnSnakeLadder>::~wnArticleHolder();
template wnArticleHolder<wnSnakeRgb6>::~wnArticleHolder();
template wnArticleHolder<wnSnakeRgb6Sight>::~wnArticleHolder();
template wnArticleHolder<wnSnakeRgb6Clip>::~wnArticleHolder();
template wnArticleHolder<wnSnakeRgb6Bullet>::~wnArticleHolder();
template wnArticleHolder<wnSimple>::~wnArticleHolder();
template wnArticleHolder<wnSimple>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftSnakeArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftSnakeArticleMediator, 7>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftSnakeArticleMediator, 7>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftSnakeArticleMediator::~ftSnakeArticleMediator() { }
#pragma dont_inline off
soArticleMediator::~soArticleMediator() { } // the base of every mediator (its teardown is in this unit, as in ft_mario.cpp)
wnSimple::~wnSimple() { } // the simple article's teardown is a copy in each fighter that pools one (as in ft_purin.cpp)

s32 ftSnakeArticleMediator::getMediateNum() { return 15; }
void ftSnakeArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Mine, 1 TrenchMortar, 2 Rpg7, 3 Cypher, 4 C4, 5 C4Switch, 6 Nikita, 7 Ladder, 8 Rgb6, 9 Rgb6Sight, 10 Rgb6Clip, 11 Rgb6Bullet, 12 Simple (the rest are unused).

#define FT_SNAKE_POOL_MINE_BASE static_cast<wnArticlePool<wnSnakeMine, 1, ftSnakeTrenchMortarPool>&>(m_pools)
#define FT_SNAKE_POOL_MINE FT_SNAKE_POOL_MINE_BASE.getSub()
#define FT_SNAKE_POOL_TRENCHMORTAR_BASE static_cast<wnArticlePool<wnSnakeTrenchMortar, 1, ftSnakeRpg7Pool>&>(m_pools)
#define FT_SNAKE_POOL_TRENCHMORTAR FT_SNAKE_POOL_TRENCHMORTAR_BASE.getSub()
#define FT_SNAKE_POOL_RPG7_BASE static_cast<wnArticlePool<wnSnakeRpg7, 1, ftSnakeCypherPool>&>(m_pools)
#define FT_SNAKE_POOL_RPG7 FT_SNAKE_POOL_RPG7_BASE.getSub()
#define FT_SNAKE_POOL_CYPHER_BASE static_cast<wnArticlePool<wnSnakeCypher, 1, ftSnakeC4Pool>&>(m_pools)
#define FT_SNAKE_POOL_CYPHER FT_SNAKE_POOL_CYPHER_BASE.getSub()
#define FT_SNAKE_POOL_C4_BASE static_cast<wnArticlePool<wnSnakeC4, 1, ftSnakeC4SwitchPool>&>(m_pools)
#define FT_SNAKE_POOL_C4 FT_SNAKE_POOL_C4_BASE.getSub()
#define FT_SNAKE_POOL_C4SWITCH_BASE static_cast<wnArticlePool<wnSnakeC4Switch, 1, ftSnakeNikitaPool>&>(m_pools)
#define FT_SNAKE_POOL_C4SWITCH FT_SNAKE_POOL_C4SWITCH_BASE.getSub()
#define FT_SNAKE_POOL_NIKITA_BASE static_cast<wnArticlePool<wnSnakeNikita, 1, ftSnakeLadderPool>&>(m_pools)
#define FT_SNAKE_POOL_NIKITA FT_SNAKE_POOL_NIKITA_BASE.getSub()
#define FT_SNAKE_POOL_LADDER_BASE static_cast<wnArticlePool<wnSnakeLadder, 1, ftSnakeRgb6Pool>&>(m_pools)
#define FT_SNAKE_POOL_LADDER FT_SNAKE_POOL_LADDER_BASE.getSub()
#define FT_SNAKE_POOL_RGB6_BASE static_cast<wnArticlePool<wnSnakeRgb6, 1, ftSnakeRgb6SightPool>&>(m_pools)
#define FT_SNAKE_POOL_RGB6 FT_SNAKE_POOL_RGB6_BASE.getSub()
#define FT_SNAKE_POOL_RGB6SIGHT_BASE static_cast<wnArticlePool<wnSnakeRgb6Sight, 1, ftSnakeRgb6ClipPool>&>(m_pools)
#define FT_SNAKE_POOL_RGB6SIGHT FT_SNAKE_POOL_RGB6SIGHT_BASE.getSub()
#define FT_SNAKE_POOL_RGB6CLIP_BASE static_cast<wnArticlePool<wnSnakeRgb6Clip, 1, ftSnakeRgb6BulletPool>&>(m_pools)
#define FT_SNAKE_POOL_RGB6CLIP FT_SNAKE_POOL_RGB6CLIP_BASE.getSub()
#define FT_SNAKE_POOL_RGB6BULLET_BASE static_cast<wnArticlePool<wnSnakeRgb6Bullet, 1, ftSnakeSimplePool>&>(m_pools)
#define FT_SNAKE_POOL_RGB6BULLET FT_SNAKE_POOL_RGB6BULLET_BASE.getSub()
#define FT_SNAKE_POOL_SIMPLE_BASE static_cast<wnArticlePool<wnSimple, 2, soInstancePoolRoot>&>(m_pools)
#define FT_SNAKE_POOL_SIMPLE FT_SNAKE_POOL_SIMPLE_BASE.getSub()

#define FT_SNAKE_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftSnakeArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnSnakeMine* weapon = FT_SNAKE_POOL_MINE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSnakeTrenchMortar* weapon = FT_SNAKE_POOL_TRENCHMORTAR.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSnakeRpg7* weapon = FT_SNAKE_POOL_RPG7.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSnakeCypher* weapon = FT_SNAKE_POOL_CYPHER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSnakeC4* weapon = FT_SNAKE_POOL_C4.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSnakeC4Switch* weapon = FT_SNAKE_POOL_C4SWITCH.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSnakeNikita* weapon = FT_SNAKE_POOL_NIKITA.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSnakeLadder* weapon = FT_SNAKE_POOL_LADDER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSnakeRgb6* weapon = FT_SNAKE_POOL_RGB6.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSnakeRgb6Sight* weapon = FT_SNAKE_POOL_RGB6SIGHT.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSnakeRgb6Clip* weapon = FT_SNAKE_POOL_RGB6CLIP.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSnakeRgb6Bullet* weapon = FT_SNAKE_POOL_RGB6BULLET.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnSimple* weapon = FT_SNAKE_POOL_SIMPLE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftSnakeArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 6: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 7: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 8: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 9: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 10: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 11: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 12: return wnArticleSlotGenerateMaxNum(2, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOT(13, 0)
    WN_ARTICLE_UNUSED_SLOT(14, 0)
    FT_SNAKE_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftSnakeActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

template <int S, int L>
inline s32 ftSnakeGetActiveNumSlot13(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
        return 2 - acc->getGenerateArticleManageModule().getNum(13);
}

template <int S, int L>
inline s32 ftSnakeGetActiveNumSlot14(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
        return 1 - acc->getGenerateArticleManageModule().getNum(14);
}

s32 ftSnakeArticleMediator::getActiveNum(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnSnakeMine, 1, ftSnakeTrenchMortarPool>, ftSnakeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnSnakeTrenchMortar, 1, ftSnakeRpg7Pool>, ftSnakeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotActiveNum<wnArticlePool<wnSnakeRpg7, 1, ftSnakeCypherPool>, ftSnakeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotActiveNum<wnArticlePool<wnSnakeCypher, 1, ftSnakeC4Pool>, ftSnakeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotActiveNum<wnArticlePool<wnSnakeC4, 1, ftSnakeC4SwitchPool>, ftSnakeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotActiveNum<wnArticlePool<wnSnakeC4Switch, 1, ftSnakeNikitaPool>, ftSnakeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 6: return wnArticleSlotActiveNum<wnArticlePool<wnSnakeNikita, 1, ftSnakeLadderPool>, ftSnakeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 7: return wnArticleSlotActiveNum<wnArticlePool<wnSnakeLadder, 1, ftSnakeRgb6Pool>, ftSnakeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 8: return wnArticleSlotActiveNum<wnArticlePool<wnSnakeRgb6, 1, ftSnakeRgb6SightPool>, ftSnakeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 9: return wnArticleSlotActiveNum<wnArticlePool<wnSnakeRgb6Sight, 1, ftSnakeRgb6ClipPool>, ftSnakeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 10: return wnArticleSlotActiveNum<wnArticlePool<wnSnakeRgb6Clip, 1, ftSnakeRgb6BulletPool>, ftSnakeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 11: return wnArticleSlotActiveNum<wnArticlePool<wnSnakeRgb6Bullet, 1, ftSnakeSimplePool>, ftSnakeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 12: return wnArticleSlotActiveNum<wnArticlePool<wnSimple, 2, soInstancePoolRoot>, ftSnakeActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 13: return ftSnakeGetActiveNumSlot13(acc, WN_ARTICLE_SLOT_TAGS);
    case 14: return ftSnakeGetActiveNumSlot14(acc, WN_ARTICLE_SLOT_TAGS);
    FT_SNAKE_UNUSED_SLOTS(0)
    default: return 0;
    }
}

template <int S, int L>
inline bool ftSnakeIsGeneratableSlot13(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
        return acc->getGenerateArticleManageModule().getNum(13) < 2;
}

template <int S, int L>
inline bool ftSnakeIsGeneratableSlot14(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
        return acc->getGenerateArticleManageModule().getNum(14) < 1;
}

bool ftSnakeArticleMediator::isGeneratable(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnSnakeMine, 1, ftSnakeTrenchMortarPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnSnakeTrenchMortar, 1, ftSnakeRpg7Pool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotIsGeneratable<wnArticlePool<wnSnakeRpg7, 1, ftSnakeCypherPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotIsGeneratable<wnArticlePool<wnSnakeCypher, 1, ftSnakeC4Pool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotIsGeneratable<wnArticlePool<wnSnakeC4, 1, ftSnakeC4SwitchPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotIsGeneratable<wnArticlePool<wnSnakeC4Switch, 1, ftSnakeNikitaPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 6: return wnArticleSlotIsGeneratable<wnArticlePool<wnSnakeNikita, 1, ftSnakeLadderPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 7: return wnArticleSlotIsGeneratable<wnArticlePool<wnSnakeLadder, 1, ftSnakeRgb6Pool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 8: return wnArticleSlotIsGeneratable<wnArticlePool<wnSnakeRgb6, 1, ftSnakeRgb6SightPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 9: return wnArticleSlotIsGeneratable<wnArticlePool<wnSnakeRgb6Sight, 1, ftSnakeRgb6ClipPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 10: return wnArticleSlotIsGeneratable<wnArticlePool<wnSnakeRgb6Clip, 1, ftSnakeRgb6BulletPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 11: return wnArticleSlotIsGeneratable<wnArticlePool<wnSnakeRgb6Bullet, 1, ftSnakeSimplePool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 12: return wnArticleSlotIsGeneratable<wnArticlePool<wnSimple, 2, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    case 13: return ftSnakeIsGeneratableSlot13(acc, WN_ARTICLE_SLOT_TAGS);
    case 14: return ftSnakeIsGeneratableSlot14(acc, WN_ARTICLE_SLOT_TAGS);
    FT_SNAKE_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftSnakeArticleActivator;
template <> struct ftSnakeArticleActivator<wnSnakeMine> {
    static bool activate(wnSnakeMine* weapon, soModuleAccesser* acc) { return ftSnake::activeArticle(weapon, acc); }
};
template <> struct ftSnakeArticleActivator<wnSnakeTrenchMortar> {
    static bool activate(wnSnakeTrenchMortar* weapon, soModuleAccesser* acc) { return ftSnake::activeArticle1(weapon, acc); }
};
template <> struct ftSnakeArticleActivator<wnSnakeRpg7> {
    static bool activate(wnSnakeRpg7* weapon, soModuleAccesser* acc) { return ftSnake::activeArticle2(weapon, acc); }
};
template <> struct ftSnakeArticleActivator<wnSnakeCypher> {
    static bool activate(wnSnakeCypher* weapon, soModuleAccesser* acc) { return ftSnake::activeArticle3(weapon, acc); }
};
template <> struct ftSnakeArticleActivator<wnSnakeC4> {
    static bool activate(wnSnakeC4* weapon, soModuleAccesser* acc) { return ftSnake::activeArticle4(weapon, acc); }
};
template <> struct ftSnakeArticleActivator<wnSnakeC4Switch> {
    static bool activate(wnSnakeC4Switch* weapon, soModuleAccesser* acc) { return ftSnake::activeArticle5(weapon, acc); }
};
template <> struct ftSnakeArticleActivator<wnSnakeNikita> {
    static bool activate(wnSnakeNikita* weapon, soModuleAccesser* acc) { return ftSnake::activeArticle6(weapon, acc); }
};
template <> struct ftSnakeArticleActivator<wnSnakeLadder> {
    static bool activate(wnSnakeLadder* weapon, soModuleAccesser* acc) { return ftSnake::activeArticle7(weapon, acc); }
};
template <> struct ftSnakeArticleActivator<wnSnakeRgb6> {
    static bool activate(wnSnakeRgb6* weapon, soModuleAccesser* acc) { return ftSnake::activeArticle8(weapon, acc); }
};
template <> struct ftSnakeArticleActivator<wnSnakeRgb6Sight> {
    static bool activate(wnSnakeRgb6Sight* weapon, soModuleAccesser* acc) { return ftSnake::activeArticle9(weapon, acc); }
};
template <> struct ftSnakeArticleActivator<wnSnakeRgb6Clip> {
    static bool activate(wnSnakeRgb6Clip* weapon, soModuleAccesser* acc) { return ftSnake::activeArticle10(weapon, acc); }
};
template <> struct ftSnakeArticleActivator<wnSnakeRgb6Bullet> {
    static bool activate(wnSnakeRgb6Bullet* weapon, soModuleAccesser* acc) { return ftSnake::activeArticle11(weapon, acc); }
};
template <> struct ftSnakeArticleActivator<wnSimple> {
    static bool activate(wnSimple* weapon, soModuleAccesser* acc) { return ftSnake::activeArticle12(weapon, acc); }
};

template <int S, int L>
inline soArticle* ftSnakeGenerateSlot13(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    if (acc->getGenerateArticleManageModule().getNum(13) >= 2) {
        return wnGetNullArticle();
    }
    BaseItem* item = soArticleMediatorHelper::createItem(0x59, 13, acc->getStageObject().getTaskId());
    if (item == NULL) {
        return wnGetNullArticle();
    }
    return soArticleMediatorHelper::castArticle(item);
}

template <int S, int L>
inline soArticle* ftSnakeGenerateSlot14(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    if (acc->getGenerateArticleManageModule().getNum(14) >= 1) {
        return wnGetNullArticle();
    }
    BaseItem* item = soArticleMediatorHelper::createItem(0x53, 14, acc->getStageObject().getTaskId());
    if (item == NULL) {
        return wnGetNullArticle();
    }
    return soArticleMediatorHelper::castArticle(item);
}

soArticle* ftSnakeArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftSnakeArticleActivator<wnSnakeMine> >(static_cast<wnArticlePool<wnSnakeMine, 1, ftSnakeTrenchMortarPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftSnakeArticleActivator<wnSnakeTrenchMortar> >(static_cast<wnArticlePool<wnSnakeTrenchMortar, 1, ftSnakeRpg7Pool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerate<ftSnakeArticleActivator<wnSnakeRpg7> >(static_cast<wnArticlePool<wnSnakeRpg7, 1, ftSnakeCypherPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerate<ftSnakeArticleActivator<wnSnakeCypher> >(static_cast<wnArticlePool<wnSnakeCypher, 1, ftSnakeC4Pool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotGenerate<ftSnakeArticleActivator<wnSnakeC4> >(static_cast<wnArticlePool<wnSnakeC4, 1, ftSnakeC4SwitchPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotGenerate<ftSnakeArticleActivator<wnSnakeC4Switch> >(static_cast<wnArticlePool<wnSnakeC4Switch, 1, ftSnakeNikitaPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 6: return wnArticleSlotGenerate<ftSnakeArticleActivator<wnSnakeNikita> >(static_cast<wnArticlePool<wnSnakeNikita, 1, ftSnakeLadderPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 7: return wnArticleSlotGenerate<ftSnakeArticleActivator<wnSnakeLadder> >(static_cast<wnArticlePool<wnSnakeLadder, 1, ftSnakeRgb6Pool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 8: return wnArticleSlotGenerate<ftSnakeArticleActivator<wnSnakeRgb6> >(static_cast<wnArticlePool<wnSnakeRgb6, 1, ftSnakeRgb6SightPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 9: return wnArticleSlotGenerate<ftSnakeArticleActivator<wnSnakeRgb6Sight> >(static_cast<wnArticlePool<wnSnakeRgb6Sight, 1, ftSnakeRgb6ClipPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 10: return wnArticleSlotGenerate<ftSnakeArticleActivator<wnSnakeRgb6Clip> >(static_cast<wnArticlePool<wnSnakeRgb6Clip, 1, ftSnakeRgb6BulletPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 11: return wnArticleSlotGenerate<ftSnakeArticleActivator<wnSnakeRgb6Bullet> >(static_cast<wnArticlePool<wnSnakeRgb6Bullet, 1, ftSnakeSimplePool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 12: return wnArticleSlotGenerate<ftSnakeArticleActivator<wnSimple> >(static_cast<wnArticlePool<wnSimple, 2, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 13: return ftSnakeGenerateSlot13(acc, WN_ARTICLE_SLOT_TAGS);
    case 14: return ftSnakeGenerateSlot14(acc, WN_ARTICLE_SLOT_TAGS);
    FT_SNAKE_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

template <int S, int L>
inline bool ftSnakeShootMine(soArticle* article, soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    wnSnakeMine& mine = dynamic_cast<wnSnakeMine&>(*article);
    mine.shoot(acc->getPostureModule().getPos());
    return true;
}

template <int S, int L>
inline bool ftSnakeShootCypher(soArticle* article, soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    return ftSnake::shootArticle(&dynamic_cast<wnSnakeCypher&>(*article), acc);
}

template <int S, int L>
inline bool ftSnakeShootC4(soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    dynamic_cast<wnSnakeC4&>(*article).shoot();
    return true;
}

// The item slots (13 and 14) take a third tag after the slot and level tags, the native code's third zero byte in these
// two cases. HYPOTHESIS: it is the item counterpart of the article type the other slots are checked against.
template <int S, int L, int I>
inline bool ftSnakeShootSlot13(soArticle* article, soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&, const soIntToType<I>&) {
    bool flag = false;
    return ftSnake::shootItem(article, &flag, acc);
}

template <int S, int L, int I>
inline bool ftSnakeShootSlot14(soArticle* article, soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&, const soIntToType<I>&) {
    bool flag = false;
    return ftSnake::shootItem1(article, &flag, acc);
}

bool ftSnakeArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return ftSnakeShootMine(article, acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotShoot<wnSnakeTrenchMortar>(article, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotShoot<wnSnakeRpg7>(article, WN_ARTICLE_SLOT_TAGS);
    case 3: return ftSnakeShootCypher(article, acc, WN_ARTICLE_SLOT_TAGS);
    case 4: return ftSnakeShootC4(article, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotShoot<wnSnakeC4Switch>(article, WN_ARTICLE_SLOT_TAGS);
    case 6: return wnArticleSlotShoot<wnSnakeNikita>(article, WN_ARTICLE_SLOT_TAGS);
    case 7: return wnArticleSlotShoot<wnSnakeLadder>(article, WN_ARTICLE_SLOT_TAGS);
    case 8: return wnArticleSlotShoot<wnSnakeRgb6>(article, WN_ARTICLE_SLOT_TAGS);
    case 9: return wnArticleSlotShoot<wnSnakeRgb6Sight>(article, WN_ARTICLE_SLOT_TAGS);
    case 10: return wnArticleSlotShoot<wnSnakeRgb6Clip>(article, WN_ARTICLE_SLOT_TAGS);
    case 11: return wnArticleSlotShoot<wnSnakeRgb6Bullet>(article, WN_ARTICLE_SLOT_TAGS);
    case 12: return wnArticleSlotShoot<wnSimple>(article, WN_ARTICLE_SLOT_TAGS);
    case 13: return ftSnakeShootSlot13(article, acc, WN_ARTICLE_SLOT_TAGS, soIntToType<0>());
    case 14: return ftSnakeShootSlot14(article, acc, WN_ARTICLE_SLOT_TAGS, soIntToType<0>());
    case 15: return wnArticleSlotUnused(true, WN_ARTICLE_SLOT_TAGS);
    case 16: return wnArticleSlotUnused(true, WN_ARTICLE_SLOT_TAGS);
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftSnake is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftSnakeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
