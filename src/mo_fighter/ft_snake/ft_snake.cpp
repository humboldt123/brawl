#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_TEAM_TYPED_INTERFACE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define FT_NOINLINE_SEARCH_BUILDER
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_SNAKE_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
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

#define FT_BC ftSnakeBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

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
// HYPOTHESIS: the offset constants (0xFA9-0xFAB) and the field at 0xE4 of the camera (a screen ratio).
void ftSnake::processFixCamera() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x130:
    case 0x131:
    case 0x132:
        if (gfCameraManager::getManager() != NULL) {
            if (gfCameraManager::getManager() != NULL) {
                gfCamera* camera = reinterpret_cast<gfCamera*>(gfCameraManager::getManager());
                if (camera != NULL) {
                    Matrix cameraMtx = *reinterpret_cast<Matrix*>(camera);
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
                    offset.m_x *= *reinterpret_cast<float*>(reinterpret_cast<u8*>(camera) + 0xE4) / divisor;
                    inverse.mulPos(&offset, &position);
                    Vec3f rotation;
                    inverse.getRotate(&rotation);
                    m_moduleAccesser->getPostureModule().setPos(&position);
                    m_moduleAccesser->getPostureModule().setRot(&Vec3f(57.29578f * rotation.m_x, 57.29578f * rotation.m_y, 57.29578f * rotation.m_z), 0);
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
bool ftSnake::activeArticle1(wnSnakeTrenchMortar* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    int founderTaskId = acc->getStageObject().m_taskId;
    soResourceModule& resources = acc->getResourceModule();
    float lr = acc->getPostureModule().getLr();
    weapon->activate(resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam(), lr, founderTaskId);
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
bool ftSnake::activeArticle3(wnSnakeCypher* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    weapon->activate(resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam(), acc->getStageObject().m_taskId);
    return true;
}

// The Cypher is shot with the speed of the fighter.
bool ftSnake::shootArticle(wnSnakeCypher* weapon, soModuleAccesser* acc) {
    const Vec2f& speed = acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    weapon->shoot(&speed.m_x);
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

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Snake, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftSnakeArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftSnakeArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftSnakeArticleKind : wnArticleKindInfo {
    ftSnakeArticleKind() {
        kind = 0;
        subKind = Fighter_Snake;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftSnakeArticleBuildInfo : wnArticleBuildInfo {
    ftSnakeArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
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

template <class W> struct ftSnakeArticleTraits;
#define FT_SNAKE_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftSnakeArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftSnakeArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Snake, &tag.m_resourceGroup)); \
        } \
    };

FT_SNAKE_TRAITS(wnSnakeMine, wnSnakeMineData, 0, fn_27_147F38)
FT_SNAKE_TRAITS(wnSnakeTrenchMortar, wnSnakeTrenchMortarData, 1, fn_27_147F4C)
FT_SNAKE_TRAITS(wnSnakeRpg7, wnSnakeRpg7Data, 2, fn_27_147F74)
FT_SNAKE_TRAITS(wnSnakeCypher, wnSnakeCypherData, 3, fn_27_147F88)
FT_SNAKE_TRAITS(wnSnakeC4, wnSnakeC4Data, 4, fn_27_147F9C)
FT_SNAKE_TRAITS(wnSnakeC4Switch, wnSnakeC4SwitchData, 5, fn_27_147FB0)
FT_SNAKE_TRAITS(wnSnakeNikita, wnSnakeNikitaData, 6, fn_27_147FC4)
FT_SNAKE_TRAITS(wnSnakeLadder, wnSnakeLadderData, 7, fn_27_147FEC)
FT_SNAKE_TRAITS(wnSnakeRgb6, wnSnakeRgb6Data, 8, fn_27_148000)
FT_SNAKE_TRAITS(wnSnakeRgb6Sight, wnSnakeRgb6SightData, 9, fn_27_148014)
FT_SNAKE_TRAITS(wnSnakeRgb6Clip, wnSnakeRgb6ClipData, 10, fn_27_148028)
FT_SNAKE_TRAITS(wnSnakeRgb6Bullet, wnSnakeRgb6BulletData, 11, fn_27_14803C)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftSnakeArticleTraits<W>::ArticleId, ftSnakeArticleBuildInfo(ftSnakeArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftSnakeArticleTraits<W>::getData()) { }


// The two simple articles (the shared generic article) are built from the simple data of the fighter kind.
struct ftSnakeSimpleBuildInfo : wnSimpleConstructionInfo {
    ftSnakeSimpleBuildInfo(wnSimpleKindInfo kindArg, void* heap) : wnSimpleConstructionInfo(kindArg, heap) { }
};
static wnSimpleData* ftSnakeSimpleData() {
    ftSnakeArticleDataTag tag;
    return g_ftCommonDataAccesser.getSimpleData(Fighter_Snake, &tag.m_resourceGroup);
}
template <>
wnArticleHolder<wnSimple>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(12, ftSnakeSimpleBuildInfo(wnSimpleKindInfo(Fighter_Snake), acc->m_enumerationStart->m_heapModule), ftSnakeSimpleData(), false) { }

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
    case 0: { wnArticleSlotTag tag; return 1; }
    case 1: { wnArticleSlotTag tag; return 1; }
    case 2: { wnArticleSlotTag tag; return 1; }
    case 3: { wnArticleSlotTag tag; return 1; }
    case 4: { wnArticleSlotTag tag; return 1; }
    case 5: { wnArticleSlotTag tag; return 1; }
    case 6: { wnArticleSlotTag tag; return 1; }
    case 7: { wnArticleSlotTag tag; return 1; }
    case 8: { wnArticleSlotTag tag; return 1; }
    case 9: { wnArticleSlotTag tag; return 1; }
    case 10: { wnArticleSlotTag tag; return 1; }
    case 11: { wnArticleSlotTag tag; return 1; }
    case 12: { wnArticleSlotTag tag; return 2; }
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

#define FT_SNAKE_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftSnakeActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftSnakeActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftSnakeArticleMediator::getActiveNum(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: FT_SNAKE_COUNT_ACTIVE(FT_SNAKE_POOL_MINE, 1)
    case 1: FT_SNAKE_COUNT_ACTIVE(FT_SNAKE_POOL_TRENCHMORTAR, 1)
    case 2: FT_SNAKE_COUNT_ACTIVE(FT_SNAKE_POOL_RPG7, 1)
    case 3: FT_SNAKE_COUNT_ACTIVE(FT_SNAKE_POOL_CYPHER, 1)
    case 4: FT_SNAKE_COUNT_ACTIVE(FT_SNAKE_POOL_C4, 1)
    case 5: FT_SNAKE_COUNT_ACTIVE(FT_SNAKE_POOL_C4SWITCH, 1)
    case 6: FT_SNAKE_COUNT_ACTIVE(FT_SNAKE_POOL_NIKITA, 1)
    case 7: FT_SNAKE_COUNT_ACTIVE(FT_SNAKE_POOL_LADDER, 1)
    case 8: FT_SNAKE_COUNT_ACTIVE(FT_SNAKE_POOL_RGB6, 1)
    case 9: FT_SNAKE_COUNT_ACTIVE(FT_SNAKE_POOL_RGB6SIGHT, 1)
    case 10: FT_SNAKE_COUNT_ACTIVE(FT_SNAKE_POOL_RGB6CLIP, 1)
    case 11: FT_SNAKE_COUNT_ACTIVE(FT_SNAKE_POOL_RGB6BULLET, 1)
    case 12: FT_SNAKE_COUNT_ACTIVE(FT_SNAKE_POOL_SIMPLE, 2)
    case 13: { wnArticleSlotTag tag; return 2 - acc->getGenerateArticleManageModule().getNum(13); }
    case 14: { wnArticleSlotTag tag; return 1 - acc->getGenerateArticleManageModule().getNum(14); }
    FT_SNAKE_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_SNAKE_CAN_GENERATE(POOL, N, ID) \
    { \
        wnArticleSlotTag tag; \
        bool allActive; \
        s32 i; \
        for (i = 0; i < N; ++i) { \
            if (static_cast<soArticle&>(*POOL.getInstanceAt(i)).isActiveArticle() == false) { \
                allActive = false; \
                goto done_##ID; \
            } \
        } \
        allActive = true; \
    done_##ID: \
        return allActive == false; \
    }

bool ftSnakeArticleMediator::isGeneratable(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: FT_SNAKE_CAN_GENERATE(FT_SNAKE_POOL_MINE, 1, 0)
    case 1: FT_SNAKE_CAN_GENERATE(FT_SNAKE_POOL_TRENCHMORTAR, 1, 1)
    case 2: FT_SNAKE_CAN_GENERATE(FT_SNAKE_POOL_RPG7, 1, 2)
    case 3: FT_SNAKE_CAN_GENERATE(FT_SNAKE_POOL_CYPHER, 1, 3)
    case 4: FT_SNAKE_CAN_GENERATE(FT_SNAKE_POOL_C4, 1, 4)
    case 5: FT_SNAKE_CAN_GENERATE(FT_SNAKE_POOL_C4SWITCH, 1, 5)
    case 6: FT_SNAKE_CAN_GENERATE(FT_SNAKE_POOL_NIKITA, 1, 6)
    case 7: FT_SNAKE_CAN_GENERATE(FT_SNAKE_POOL_LADDER, 1, 7)
    case 8: FT_SNAKE_CAN_GENERATE(FT_SNAKE_POOL_RGB6, 1, 8)
    case 9: FT_SNAKE_CAN_GENERATE(FT_SNAKE_POOL_RGB6SIGHT, 1, 9)
    case 10: FT_SNAKE_CAN_GENERATE(FT_SNAKE_POOL_RGB6CLIP, 1, 10)
    case 11: FT_SNAKE_CAN_GENERATE(FT_SNAKE_POOL_RGB6BULLET, 1, 11)
    case 12: FT_SNAKE_CAN_GENERATE(FT_SNAKE_POOL_SIMPLE, 2, 12)
    case 13: { wnArticleSlotTag tag; return acc->getGenerateArticleManageModule().getNum(13) < 2; }
    case 14: { wnArticleSlotTag tag; return acc->getGenerateArticleManageModule().getNum(14) < 1; }
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

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_SNAKE_GENERATE(W, POOL) \
    { \
        wnArticleSlotTag tag; \
        soArticleDeactivateChecker checker; \
        volatile u8 findTag = 0; /* MATCH-ONLY: the native traversal keeps a one-byte marker */ \
        W* weapon = POOL##_BASE.find(checker); \
        if (weapon == NULL) { \
            weapon = static_cast<W*>(checker.getCandidate()); \
            if (weapon == NULL) { \
                return wnGetNullArticle(); \
            } \
            weapon->deactivateArticle(); \
        } \
        if (ftSnakeArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftSnakeArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_SNAKE_GENERATE(wnSnakeMine, FT_SNAKE_POOL_MINE)
    case 1: FT_SNAKE_GENERATE(wnSnakeTrenchMortar, FT_SNAKE_POOL_TRENCHMORTAR)
    case 2: FT_SNAKE_GENERATE(wnSnakeRpg7, FT_SNAKE_POOL_RPG7)
    case 3: FT_SNAKE_GENERATE(wnSnakeCypher, FT_SNAKE_POOL_CYPHER)
    case 4: FT_SNAKE_GENERATE(wnSnakeC4, FT_SNAKE_POOL_C4)
    case 5: FT_SNAKE_GENERATE(wnSnakeC4Switch, FT_SNAKE_POOL_C4SWITCH)
    case 6: FT_SNAKE_GENERATE(wnSnakeNikita, FT_SNAKE_POOL_NIKITA)
    case 7: FT_SNAKE_GENERATE(wnSnakeLadder, FT_SNAKE_POOL_LADDER)
    case 8: FT_SNAKE_GENERATE(wnSnakeRgb6, FT_SNAKE_POOL_RGB6)
    case 9: FT_SNAKE_GENERATE(wnSnakeRgb6Sight, FT_SNAKE_POOL_RGB6SIGHT)
    case 10: FT_SNAKE_GENERATE(wnSnakeRgb6Clip, FT_SNAKE_POOL_RGB6CLIP)
    case 11: FT_SNAKE_GENERATE(wnSnakeRgb6Bullet, FT_SNAKE_POOL_RGB6BULLET)
    case 12: FT_SNAKE_GENERATE(wnSimple, FT_SNAKE_POOL_SIMPLE)
    case 13: {
        wnArticleSlotTag tag;
        if (acc->getGenerateArticleManageModule().getNum(13) >= 2) {
            return wnGetNullArticle();
        }
        BaseItem* item = soArticleMediatorHelper::createItem(0x59, 13, acc->getStageObject().m_taskId);
        if (item == NULL) {
            return wnGetNullArticle();
        }
        return soArticleMediatorHelper::castArticle(item);
    }
    case 14: {
        wnArticleSlotTag tag;
        if (acc->getGenerateArticleManageModule().getNum(14) >= 1) {
            return wnGetNullArticle();
        }
        BaseItem* item = soArticleMediatorHelper::createItem(0x53, 14, acc->getStageObject().m_taskId);
        if (item == NULL) {
            return wnGetNullArticle();
        }
        return soArticleMediatorHelper::castArticle(item);
    }
    FT_SNAKE_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftSnakeArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: {
        wnArticleSlotTag tag;
        wnSnakeMine& mine = dynamic_cast<wnSnakeMine&>(*article);
        mine.shoot(acc->getPostureModule().getPos());
        return true;
    }
    case 1: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSnakeTrenchMortar&>(*article);
        return true;
    }
    case 2: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSnakeRpg7&>(*article);
        return true;
    }
    case 3: {
        wnArticleSlotTag tag;
        return ftSnake::shootArticle(&dynamic_cast<wnSnakeCypher&>(*article), acc);
    }
    case 4: {
        wnArticleSlotTag tag;
        dynamic_cast<wnSnakeC4&>(*article).shoot();
        return true;
    }
    case 5: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSnakeC4Switch&>(*article);
        return true;
    }
    case 6: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSnakeNikita&>(*article);
        return true;
    }
    case 7: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSnakeLadder&>(*article);
        return true;
    }
    case 8: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSnakeRgb6&>(*article);
        return true;
    }
    case 9: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSnakeRgb6Sight&>(*article);
        return true;
    }
    case 10: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSnakeRgb6Clip&>(*article);
        return true;
    }
    case 11: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSnakeRgb6Bullet&>(*article);
        return true;
    }
    case 12: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSimple&>(*article);
        return true;
    }
    case 13: {
        volatile u8 itemMarker = 0; // MATCH-ONLY: the native case keeps one more marker byte than the article cases
        wnArticleSlotTag tag;
        bool flag = false;
        return ftSnake::shootItem(article, &flag, acc);
    }
    case 14: {
        volatile u8 itemMarker = 0; // MATCH-ONLY: the native case keeps one more marker byte than the article cases
        wnArticleSlotTag tag;
        bool flag = false;
        return ftSnake::shootItem1(article, &flag, acc);
    }
    WN_ARTICLE_UNUSED_SLOT(15, true)
    WN_ARTICLE_UNUSED_SLOT(16, true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftSnake is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftSnakeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftSnakeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Virtual-slot view used only to emit virtual tail calls; the slots are never defined in this unit.
struct ftSnakeVirtualSlots {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
};

// Empty virtual overrides.
void fn_122_CF10() {}
void fn_122_EFD0() {}
void fn_122_F1C4() {}
void fn_122_13394() {}
void fn_122_13418() {}
void fn_122_13500() {}
void fn_122_135E8() {}
void fn_122_136D0() {}
void fn_122_137B8() {}
void fn_122_138A0() {}
void fn_122_13988() {}
void fn_122_13A70() {}

// Constant returns.
int fn_122_F094() { return 0; }
int fn_122_11DBC() { return 15; }
int fn_122_133D0() { return 8; }
int fn_122_134C4() { return 0; }
int fn_122_135AC() { return 0; }
int fn_122_13694() { return 0; }
int fn_122_1377C() { return 0; }
int fn_122_13864() { return 0; }
int fn_122_1394C() { return 0; }
int fn_122_13A34() { return 0; }
int fn_122_13B1C() { return 0; }

// Byte store at +0x24900.
void fn_122_11DC4(u8* p, u8 v) { *(u8*)(p + 0x24900) = v; }

// Virtual tail call through slot 20.
void fn_122_11E3C(void* p) { return ((ftSnakeVirtualSlots*)p)->slot20(); }

// Sub-object thunks: adjust this by a constant, then tail-call the sibling function.
void* fn_122_5A94(void* p, s32 flags);
void* fn_122_11708(void* p, s32 flags);
void* fn_122_3254(void* p, s32 flags);
void* fn_122_F284(void* p, s32 flags);
void* fn_122_D7C8(void* p, s32 flags);
void* fn_122_CFC4(void* p, s32 flags);
void* fn_122_D188(void* p, s32 flags);
void* fn_122_DC1C(void* p, s32 flags);
void* fn_122_D25C(void* p, s32 flags);
void* fn_122_EE8C(void* p, s32 flags);
void* fn_122_EDF4(void* p, s32 flags);

void* fn_122_13D20(void* p, s32 flags) { return fn_122_5A94((u8*)p - 0x4, flags); }
void* fn_122_13D28(void* p, s32 flags) { return fn_122_11708((u8*)p - 0x4, flags); }
void* fn_122_13D30(void* p, s32 flags) { return fn_122_3254((u8*)p - 0x4, flags); }
void* fn_122_13D68(void* p, s32 flags) { return fn_122_F284((u8*)p - 0x40, flags); }
void* fn_122_13D80(void* p, s32 flags) { return fn_122_D7C8((u8*)p - 0x54, flags); }
void* fn_122_13D88(void* p, s32 flags) { return fn_122_CFC4((u8*)p - 0x64, flags); }
void* fn_122_13D90(void* p, s32 flags) { return fn_122_D188((u8*)p - 0x70, flags); }
void* fn_122_13DF8(void* p, s32 flags) { return fn_122_DC1C((u8*)p - 0xB8, flags); }
void* fn_122_13E60(void* p, s32 flags) { return fn_122_D25C((u8*)p - 0xE8, flags); }
void* fn_122_13E90(void* p, s32 flags) { return fn_122_F284((u8*)p - 0x2D020, flags); }
void* fn_122_13E9C(void* p, s32 flags) { return fn_122_EE8C((u8*)p - 0x2D020, flags); }
void* fn_122_13EA8(void* p, s32 flags) { return fn_122_EDF4((u8*)p - 0x2D020, flags); }

// Pointer offset returned only for selector 0 (ignores the sub-object if the selector is nonzero).
u8* fn_122_11E74(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11E8C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11EA4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11EBC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11ED4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11EEC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11F04(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11F1C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11F34(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11F4C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11F64(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_11F7C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_1341C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_13504(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_135EC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_136D4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_137BC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_138A4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_1398C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_122_13A74(u8* p, int x) { return x == 0 ? p + 0xC : 0; }

// Selector 0 and 1 sub-object pointers.
u8* fn_122_11E4C(u8* p, int x) {
    if (x == 1) {
        return p + 0x16DC;
    }
    if (x == 0) {
        return p + 0x10;
    }
    return 0;
}

// Delete the object when flags > 0 (no destructor call).
void* fn_122_1D5C(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_1DF8(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
#pragma dont_inline on
void* fn_122_3820(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
#pragma dont_inline reset
void* fn_122_3860(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3EE4(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3F94(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_4394(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
#pragma dont_inline on
void* fn_122_48DC(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
#pragma dont_inline reset

// Vec3 copy.
void fn_122_D564(float* dst, float* src) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}


// Sub-object destructor wrappers: destroy a sub-object (flag 0 or -1), then delete when flags > 0.
void* fn_122_12BB4(void* p, s16 flags);
void* fn_122_3320(void* p, s16 flags);
void* fn_27_D3B50(void* p, s16 flags);
void* fn_27_64874(void* p, s16 flags);

void* fn_27_63EBC(void* p, s16 flags);
void* fn_27_5C674(void* p, s16 flags);
void* fn_27_5AE44(void* p, s16 flags);
void __dt__16soStopModuleImplFv(void* p, s16 flags);
void __dt__22soVisibilityModuleImplFv(void* p, s16 flags);
void __dt__22soColorBlendModuleImplFv(void* p, s16 flags);
void __dt__22soControllerModuleImplFv(void* p, s16 flags);
void __dt__15cmPhotoCallBackFv(void* p, s16 flags);
void __dt__21ftExtendParamAccesserFv(void* p, s16 flags);
void* fn_122_A734(void* p, s16 flags);
void* fn_27_6880C(void* p, s16 flags);
void* fn_122_12054(void* p, s16 flags);
void __dt__25soKineticEnergyWindNormalFv(void* p, s16 flags);
void* fn_122_12828(void* p, s16 flags);
void* fn_122_37C4(void* p, s16 flags);
void* fn_122_12960(void* p, s16 flags);
void* fn_122_33FC(void* p, s16 flags);
void* fn_122_345C(void* p, s16 flags);
void __dt__19soGeneralWorkSimpleFv(void* p, s16 flags);
void* fn_27_B4780(void* p, s16 flags);
void __dt__17soSoundModuleImplFv(void* p, s16 flags);
void* fn_122_4F20(void* p, s16 flags);

void* fn_122_12C2C(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12BB4(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_32C4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_3320(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_38A0(void* p, s16 flags) {
    if (p != 0) {
        fn_27_D3B50(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_38FC(void* p, s16 flags) {
    if (p != 0) {
        fn_27_64874(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3958(void* p, s16 flags) {
    if (p != 0) {
        fn_122_3820(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3BC4(void* p, s16 flags) {
    if (p != 0) {
        static_cast<soGlowModuleSimple*>(p)->soGlowModuleSimple::~soGlowModuleSimple();
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3C20(void* p, s16 flags) {
    if (p != 0) {
        fn_27_63EBC(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3C7C(void* p, s16 flags) {
    if (p != 0) {
        fn_27_5C674(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_44FC(void* p, s16 flags) {
    if (p != 0) {
        fn_27_5AE44(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5438(void* p, s16 flags) {
    if (p != 0) {
        fn_122_3820(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5754(void* p, s16 flags) {
    if (p != 0) {
        fn_27_63EBC(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_4684(void* p, s16 flags) {
    if (p != 0) {
        __dt__16soStopModuleImplFv(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_593C(void* p, s16 flags) {
    if (p != 0) {
        __dt__22soVisibilityModuleImplFv(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6498(void* p, s16 flags) {
    if (p != 0) {
        __dt__22soColorBlendModuleImplFv(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6B18(void* p, s16 flags) {
    if (p != 0) {
        __dt__22soControllerModuleImplFv(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_A3D0(void* p, s16 flags) {
    if (p != 0) {
        fn_122_48DC(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_AA80(void* p, s16 flags) {
    if (p != 0) {
        __dt__15cmPhotoCallBackFv(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_F228(void* p, s16 flags) {
    if (p != 0) {
        __dt__21ftExtendParamAccesserFv(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_A6D8(void* p, s16 flags) {
    if (p != 0) {
        fn_122_A734(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_4624(void* p, s16 flags) {
    if (p != 0) {
        fn_27_6880C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_120B4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12054((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_12828(void* p, s16 flags) {
    if (p != 0) {
        __dt__25soKineticEnergyWindNormalFv((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_12888(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12828((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_12960(void* p, s16 flags) {
    if (p != 0) {
        fn_122_37C4((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_129C0(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12960((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_339C(void* p, s16 flags) {
    if (p != 0) {
        fn_122_33FC((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_33FC(void* p, s16 flags) {
    if (p != 0) {
        fn_122_345C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_3DE8(void* p, s16 flags) {
    if (p != 0) {
        __dt__19soGeneralWorkSimpleFv((u8*)p + 0x38, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_42AC(void* p, s16 flags) {
    if (p != 0) {
        __dt__19soGeneralWorkSimpleFv((u8*)p + 0x14, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_449C(void* p, s16 flags) {
    if (p != 0) {
        fn_27_B4780((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_45C4(void* p, s16 flags) {
    if (p != 0) {
        __dt__17soSoundModuleImplFv((u8*)p + 0xC, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_4EC0(void* p, s16 flags) {
    if (p != 0) {
        fn_122_4F20((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}

// Sub-object destructor wrappers (batch 2): destroy a sub-object, then delete when flags > 0.
void* fn_122_5D1C(void* p, s16 flags);
void* fn_122_5D7C(void* p, s16 flags);
void* fn_122_5FE4(void* p, s16 flags);
void* fn_122_6044(void* p, s16 flags);
void* fn_122_6648(void* p, s16 flags);
void* fn_122_66A8(void* p, s16 flags);
void* fn_122_6CC4(void* p, s16 flags);
void* fn_122_6D24(void* p, s16 flags);
void* fn_122_6FF0(void* p, s16 flags);
void* fn_122_7050(void* p, s16 flags);
void* fn_122_77B8(void* p, s16 flags);
void* fn_122_7818(void* p, s16 flags);
void* fn_122_7B08(void* p, s16 flags);
void* fn_122_7B68(void* p, s16 flags);
void* fn_122_7ED0(void* p, s16 flags);
void* fn_122_7F30(void* p, s16 flags);
void* fn_122_8224(void* p, s16 flags);
void* fn_122_8284(void* p, s16 flags);
void* fn_122_8628(void* p, s16 flags);
void* fn_122_8688(void* p, s16 flags);
void* fn_122_8944(void* p, s16 flags);
void* fn_122_8A10(void* p, s16 flags);
void* fn_122_89B0(void* p, s16 flags);
void* fn_122_90CC(void* p, s16 flags);
void* fn_122_12304(void* p, s16 flags);
void* fn_122_12364(void* p, s16 flags);

void* fn_122_5CBC(void* p, s16 flags) {
    if (p != 0) {
        fn_122_5D1C((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5D1C(void* p, s16 flags) {
    if (p != 0) {
        fn_122_5D7C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5F84(void* p, s16 flags) {
    if (p != 0) {
        fn_122_5FE4((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5FE4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_6044((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_65E8(void* p, s16 flags) {
    if (p != 0) {
        fn_122_6648((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6648(void* p, s16 flags) {
    if (p != 0) {
        fn_122_66A8((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6C64(void* p, s16 flags) {
    if (p != 0) {
        fn_122_6CC4((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6CC4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_6D24((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6F90(void* p, s16 flags) {
    if (p != 0) {
        fn_122_6FF0((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_6FF0(void* p, s16 flags) {
    if (p != 0) {
        fn_122_7050((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_767C(void* p, s16 flags) {
    if (p != 0) {
        fn_122_6B18((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_7758(void* p, s16 flags) {
    if (p != 0) {
        fn_122_77B8((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_77B8(void* p, s16 flags) {
    if (p != 0) {
        fn_122_7818((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_7AA8(void* p, s16 flags) {
    if (p != 0) {
        fn_122_7B08((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_7B08(void* p, s16 flags) {
    if (p != 0) {
        fn_122_7B68((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_7E70(void* p, s16 flags) {
    if (p != 0) {
        fn_122_7ED0((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_7ED0(void* p, s16 flags) {
    if (p != 0) {
        fn_122_7F30((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_81C4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_8224((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_8224(void* p, s16 flags) {
    if (p != 0) {
        fn_122_8284((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_85C8(void* p, s16 flags) {
    if (p != 0) {
        fn_122_8628((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_8628(void* p, s16 flags) {
    if (p != 0) {
        fn_122_8688((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_88E4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_8944((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_89B0(void* p, s16 flags) {
    if (p != 0) {
        fn_122_8A10((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_8D58(void* p, s16 flags) {
    if (p != 0) {
        fn_122_89B0((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_FBAC(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12C2C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_12364(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12304((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_123C4(void* p, s16 flags) {
    if (p != 0) {
        fn_122_12364((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}

// Delete the object when flags > 0 (no destructor call).
void* fn_122_4C3C(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_4C7C(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5AD4(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_5B14(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
void* fn_122_CDAC(void* p, s16 flags) {
    if (p != 0) {
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}

// Sub-object destructor wrapper: destroy the sub-object at +0x4, then delete when flags > 0.
void* fn_122_4F80(void* p, s16 flags);
void* fn_122_4F20(void* p, s16 flags) {
    if (p != 0) {
        fn_122_4F80((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}

} // extern "C"

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_122_13D40(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_122_13D48(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_122_13D50(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
void fn_122_13D58(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
void fn_122_13D60(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
bool fn_122_13D98(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_122_13DA0(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_122_13DA8(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_122_13DB0(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_122_13DC8(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_122_13DE0(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_122_13DF0(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
bool fn_122_13E00(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_122_13E08(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_122_13E10(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_122_13E18(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_122_13E20(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_122_13E28(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_122_13E30(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_122_13E38(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_122_13E40(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_122_13E48(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_122_13E50(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_122_13E58(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_122_13E68(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_122_13E70(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_122_13E78(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_122_13E80(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_122_13E88(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
bool fn_122_13D70(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_122_13DB8(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_122_13DC0(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_122_13DD0(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_122_13DD8(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_122_13DE8(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
} // extern "C"
