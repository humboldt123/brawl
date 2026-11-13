#define GR_MADEIN_NAMED_FLAGS
#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL
#define YK_DATA_INLINE_CTOR
#define SO_COLLISION_GROUP_ALIGNED

#include <gr/gr_madein.h>
#include <gf/gf_task_scheduler.h>
#include <ft/ft_manager.h>
#include <gm/gm_global.h>
#include <gf/gf_model.h>
#include <ut/ut_nw.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>
#include <yk/yk_normal.h>
#include <yk/yk_no_hit_normal.h>

// normalize/[Matrix]/(mt_matrix.o) (the map name); other files call it by this stand-in name.
extern "C" void fn_8003E5B4(Matrix* mtx);

void grMadein::setTgtNode(const char* unk1) { }

char* grMadein::getTgtNode() {
    return NULL;
}

float grMadein::getEntityFrame() {
    gfModelAnimation* anim = m_modelAnims[0];
    if (anim == NULL) {
        return 0.0f;
    } else {
        return anim->getFrame();
    }
}

grMadein::StageHitData::StageHitData() { }

void grMadein::switchToMatrix() {
    m_calcWorldCallBack.m_nodeCallbackDatas->m_flags &= ~7;
    m_calcWorldCallBack.m_nodeCallbackDatas->m_flags |= 8;
}

void grMadein::pauseEntity(bool isPause) {
    if (isPause == true) {
        m_motionRatio = 0.0f;
    } else {
        m_motionRatio = 1.0f;
    }
}

void grMadein::setMatrix(Matrix* matrix, bool followAttack) {
    m_calcWorldCallBack.m_nodeCallbackDatas->m_matrix = *matrix;
    if (!followAttack) {
        fn_8003E5B4(&m_calcWorldCallBack.m_nodeCallbackDatas->m_matrix);
    }
    setPos(matrix);
}

// HYPOTHESIS: the original resets attack data with this sequence (default values, then a zero fill and the defaults again).
static inline void grMadeinResetAttack(soCollisionAttackData* data) {
    memset(data, 0, sizeof(soCollisionAttackData));
    data->m_slipChance = 1.0f;
    data->m_hitStopFrame = 1.0f;
    data->m_hitStopDelay = 1.0f;
    data->m_region = soCollisionAttackData::Region_None;
    data->m_size = 0.0f;
    data->m_isCatch = 0;
    data->m_isDirect = 1;
}

// HYPOTHESIS: the hit object with an attack module and a damage module but no hit module is the original's ykNormal.
struct soCollisionHitModuleBuildConfigNull { }; // HYPOTHESIS: source spelling of the null configuration.

template <class TAttackConfig>
class ykNormal<TAttackConfig, soCollisionHitModuleBuildConfigNull> : public Yakumono {
    struct DamageBuilder {
        u32 unk0; // HYPOTHESIS: unused word before the damage module
        ykDamageModuleImpl m_damageModule;

        DamageBuilder(soModuleAccesser* moduleAccesser, void* unk, soEventObserverRegistrationDesc* registrationDesc)
            : m_damageModule(moduleAccesser, getNullDamages(), unk, getNullTransactor(), registrationDesc) { }

        static soArrayNull<soDamage>* getNullDamages() {
            static soArrayNull<soDamage> sNull;
            return &sNull;
        }
        static soDamageTransactor* getNullTransactor() {
            static soDamageTransactorNull sNull;
            return &sNull;
        }
    };

    TAttackConfig m_attackConfig;
    u32 unk51C; // HYPOTHESIS: unused word before the damage builder
    DamageBuilder m_damageBuilder;

public:
    ykNormal(ykInitInfo* info)
        : Yakumono(info, "ykNormal", &m_attackConfig.m_attackModule, lbl_27_bss_398, &m_damageBuilder.m_damageModule,
                   lbl_27_bss_598, lbl_27_bss_444),
          m_attackConfig(&moduleAccesser, m_taskId, (gfTask::Category)(u8)m_taskCategory, lbl_27_data_54C60),
          m_damageBuilder(&moduleAccesser, lbl_27_bss_5FF4, lbl_27_data_54C60) { }
    virtual ~ykNormal() { }
};


// HYPOTHESIS: the hit object is a ykNormal with an attack module and/or a hit module, by what the gimmick has.
#define GR_MADEIN_NEW_YAKUMONO(CATEGORY, YAKUMONO)                                                                              {                                                                                                                               typedef soCollisionAttackModuleBuildConfig<CATEGORY, 1, 0, soCollisionAttackModuleImpl, 1, false, true> AttackConfig;         typedef soCollisionHitModuleBuildConfig<CATEGORY, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> HitConfig;                   if (m_hasAttack == true && m_hasHitPoint == true) {                                                                             YAKUMONO = new (m_heapType) ykNormal<AttackConfig, HitConfig>(&info);                                                   } else if (m_hasAttack == true) {                                                                                               YAKUMONO = new (m_heapType) ykNormal<AttackConfig, soCollisionHitModuleBuildConfigNull>(&info);                         } else {                                                                                                                        YAKUMONO = new (m_heapType) ykNormal<soCollisionAttackModuleBuildConfigNull, HitConfig>(&info);                         }                                                                                                                       }

// sora_melee's out-of-line scene model getter and ScnMdl cast (names unknown).
extern "C" nw4r::g3d::ScnMdl* fn_27_2655FC(grGimmick* gimmick, int index);
extern "C" nw4r::g3d::ScnMdl* fn_27_26560C(nw4r::g3d::ScnMdl* model);

void grMadein::setupYakumonoClass() {
    if (m_hasHitPoint == true || m_hasAttack == true) {
        ykData data;
        m_stageHitData = new (m_heapType) StageHitData;
        if (m_hitPointInfo != NULL) {
            m_stageHitData->m_startOffsetPos = m_hitPointInfo->m_startOffsetPos;
            m_stageHitData->m_endOffsetPos = m_hitPointInfo->m_endOffsetPos;
            m_stageHitData->m_size = m_hitPointInfo->m_size;
        } else {
            m_stageHitData->m_startOffsetPos.m_x = 0.0f;
            m_stageHitData->m_startOffsetPos.m_y = 0.0f;
            m_stageHitData->m_startOffsetPos.m_z = 0.0f;
            m_stageHitData->m_endOffsetPos.m_x = 0.0f;
            m_stageHitData->m_endOffsetPos.m_y = 0.0f;
            m_stageHitData->m_endOffsetPos.m_z = 0.0f;
            m_stageHitData->m_size = 0.0f;
        }
        m_stageHitData->m_300_7 = m_300_7;
        m_stageHitData->m_20 = 1;
        if (m_hitPointInfo != NULL) {
            m_stageHitData->m_nodeIndex = m_hitPointInfo->m_nodeIndex;
        } else {
            m_stageHitData->m_nodeIndex = 0;
        }
        m_stageHitData->m_hitDataArray = m_stageHitData;
        m_stageHitData->m_hitDataCount = 1;
        m_stageHitData->m_dataGroup.m_hitDataSimpleSet = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(&m_stageHitData->m_hitDataArray);
        m_stageHitData->m_dataGroup.m_hitGroupIndex = 0;
        data.m_dataGroupNum = 1;
        data.m_dataGroups = &m_stageHitData->m_dataGroup;

        ykInitInfo info = { 0, 0, 0x10, 0, 0 };
        info.m_ground = this;
        info.m_node = fn_27_26560C(fn_27_2655FC(this, 0));
        Vec3f pos = getPos();
        info.m_pos = &pos;
        info.m_work = &data;

        Yakumono* yakumono;
        if (m_hitEnemy == true) {
            GR_MADEIN_NEW_YAKUMONO(soCollision::Category_Enemy, yakumono)
        } else if (m_hitWall == true) {
            GR_MADEIN_NEW_YAKUMONO(soCollision::Category_Wall, yakumono)
        } else {
            GR_MADEIN_NEW_YAKUMONO(soCollision::Category_Gimmick, yakumono)
        }
        if (m_attackInfo != NULL) {
            if ((u32)m_attackInfo->m_preset == Attack_Left) {
                yakumono->setLr(-1.0f);
            }
            if ((u32)m_attackInfo->m_preset == Attack_Right) {
                yakumono->setLr(1.0f);
            }
        }
        setYakumono(yakumono);
    }
}

grMadein* grMadein::create(int mdlIndex, const char* tgtNodeName, const char* taskName, HeapType heapType) {
    grMadein* ground = new (heapType) grMadein(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->m_heapType = heapType;
        ground->makeCalcuCallback(1, heapType);
        ground->setCalcuCallbackRoot(7);
        ground->setupMelee();
    }
    return ground;
}

grMadein::grMadein(const char* taskName) : grSeqYakumono(taskName) {
    m_loopCount = 0;
    setPos(0.0f, 0.0f, 0.0f);
    setRot(0.0f, 0.0f, 0.0f);
    setScale(1.0f, 1.0f, 1.0f);
    m_flags = 0;
    m_stageHitData = NULL;
    m_304 = 0.0f;
    m_attackPower = 10;
    for (int i = 0; i < 8; i++) {
        m_anmVals[i] = 1;
    }
    m_anmVals[8] = 0;
    m_overwriteAttackData = NULL;
    m_attackInfo = NULL;
    m_hitPointInfo = NULL;
}

grMadein::~grMadein() {
    if (m_stageHitData != NULL) {
        delete m_stageHitData;
    }
    if (m_overwriteAttackData != NULL) {
        if (m_overwriteAttackData != NULL) {
            delete m_overwriteAttackData;
        }
    }
    if (m_attackInfo != NULL) {
        delete m_attackInfo;
    }
    if (m_hitPointInfo != NULL) {
        delete m_hitPointInfo;
    }
}

soCollisionAttackData* grMadein::getOverwriteAttackData() {
    if (m_overwriteAttackData == NULL) {
        m_overwriteAttackData = new (m_heapType) soCollisionAttackData();
        grMadeinResetAttack(m_overwriteAttackData);
    }
    return m_overwriteAttackData;
}

void grMadein::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    grYakumono::startup(data, unk1, layerType);
    nw4r::g3d::ScnMdl* model = m_sceneModels[0];
    if (model != NULL) {
        model->GetScnObjOption(1, &m_anmVals[0]);
        model->GetScnObjOption(2, &m_anmVals[1]);
        model->GetScnObjOption(3, &m_anmVals[2]);
        model->GetScnObjOption(4, &m_anmVals[3]);
        model->GetScnObjOption(5, &m_anmVals[4]);
        model->GetScnObjOption(6, &m_anmVals[5]);
        model->GetScnObjOption(7, &m_anmVals[6]);
        model->GetScnObjOption(8, &m_anmVals[7]);
        model->GetScnObjOption(9, &m_anmVals[8]);
        model->m_calcWorldCallBack = calcWorldCallBack;
        model->EnableScnMdlCallbackTiming(1);
        model->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
    }
}

void grMadein::setHitPoint(float size, Vec3f* startOffsetPos, Vec3f* endOffsetPos, bool unk, int nodeIndex) {
    if (m_hitPointInfo == NULL) {
        m_hitPointInfo = new (m_heapType) HitPointInfo;
    }
    m_300_7 = unk;
    m_hasHitPoint = true;
    m_hitPointInfo->m_size = size;
    m_hitPointInfo->m_startOffsetPos = *startOffsetPos;
    m_hitPointInfo->m_endOffsetPos = *endOffsetPos;
    m_hitPointInfo->m_nodeIndex = nodeIndex;
}

void grMadein::deleteHitPoint() {
    if (m_hasHitPoint) {
        disableHit(0, 0);
    }
}

void grMadein::setAttack(float size, Vec3f* offsetPos) {
    if (m_attackInfo == NULL) {
        m_attackInfo = new (m_heapType) AttackInfo;
    }
    m_hasAttack = true;
    m_attackInfo->m_offsetPos = *offsetPos;
    m_attackInfo->m_size = size;
}

void grMadein::deleteAttackPoint() {
    if (m_hasAttack) {
        disableAttack(0);
    }
}

void grMadein::update(float deltaFrame) {
    m_isHit = false;
    if (g_GameGlobal->isPrevJustGameFrame() == true) {
        if (m_attackInfo != NULL) {
            m_attackInfo->m_playerHit[0] = false;
            m_attackInfo->m_playerHit[1] = false;
            m_attackInfo->m_playerHit[2] = false;
            m_attackInfo->m_playerHit[3] = false;
        }
    }
    gfModelAnimation* anim = m_modelAnims[0];
    if (anim != NULL) {
        if (m_loopCount != 0) {
            float lastFrame = anim->getFrameCount();
            if (anim->getFrame() >= lastFrame) {
                if (m_loopCount != 0) {
                    anim->setFrame(0.0f);
                    m_loopCount--;
                }
            }
        }
    }
    if (g_GameGlobal->isJustGameFrame() == true) {
        if (m_isLandedOn == true) {
            m_304 += deltaFrame;
            m_isLandedOn = false;
        } else {
            m_304 = 0.0f;
        }
    }
    grGimmick::update(deltaFrame);
}

void grMadein::initializeEntity() {
    gfModelAnimation* anim = m_modelAnims[0];
    if (anim != NULL) {
        anim->setLoop(false);
    }
    setupYakumonoClass();
    endEntity();
    setMotion(0);
    m_motionRatio = 1.0f;
}

void grMadein::startEntityLoop(int loopCount) {
    startEntity();
    m_loopCount = loopCount;
}

void grMadein::startEntityAutoLoop() {
    gfModelAnimation* anim = m_modelAnims[0];
    startEntity();
    anim->setLoop(true);
}

bool grMadein::isEndEntity() {
    gfModelAnimation* anim = m_modelAnims[0];
    if (anim == NULL) {
        return false;
    }
    if (m_loopCount == 0) {
        if (anim->getFrame() >= (float)anim->getFrameCount()) {
            return true;
        }
    } else if (anim->getFrame() >= (float)anim->getFrameCount() && m_loopCount == 0) {
        return true;
    }
    return false;
}

bool grMadein::isFrameEndOffset(float offset) {
    gfModelAnimation* anim = m_modelAnims[0];
    if (anim == NULL) {
        return false;
    }
    if (m_loopCount == 0) {
        if (offset + anim->getFrame() >= (float)anim->getFrameCount()) {
            return true;
        }
    } else if (offset + anim->getFrame() >= (float)anim->getFrameCount() && m_loopCount == 0) {
        return true;
    }
    return false;
}

bool grMadein::isFrameOffset(float offset) {
    gfModelAnimation* anim = m_modelAnims[0];
    if (anim == NULL) {
        return false;
    }
    if (m_loopCount == 0) {
        if (anim->getFrame() >= offset) {
            return true;
        }
    } else if (anim->getFrame() >= offset && m_loopCount == 0) {
        return true;
    }
    return false;
}

void grMadein::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    if (attackerInfo->m_indirectSoKind == 0) {
        int playerNo = g_ftManager->getPlayerNo(attackerInfo->m_indirectEntryId);
        if (playerNo != -1) {
            m_hitPointInfo->m_lastPlayerHit = playerNo;
            m_hitPointInfo->m_lastDamageTaken = damage->unk4; // damageAdd
            m_hitPointInfo->m_lastSide = damage->unk7c; // lr
            m_hitPointInfo->m_hitPos = reinterpret_cast<Vec3f&>(damage->unk80); // pos
            m_hitPointInfo->m_lastSpeed = reinterpret_cast<Vec2f&>(damage->unk8c); // speed
            m_isHit = true;
        }
    }
}

void grMadein::onInflictEach(soCollisionLog* collisionLog, float power) {
    Vec3f playerPos;
    Vec3f pos = getPos();
    ftManager* manager = g_ftManager;
    int entryId = manager->getEntryIdFromTaskId(collisionLog->m_taskId, NULL);
    if (entryId != -1) {
        m_attackInfo->m_playerNo = manager->getPlayerNo(entryId);
        if (stMelee::getPlayerPosition(m_attackInfo->m_playerNo, &playerPos) == true) {
            m_attackInfo->m_power = power;
            m_attackInfo->m_playerHit[m_attackInfo->m_playerNo] = true;
            m_attackInfo->m_hitRelPos = pos - playerPos;
        }
    }
}

// HYPOTHESIS: the original tests the lander with an inline helper like this one.
static inline bool grMadeinIsLandingTask(int taskId) {
    gfTask* task = gfTaskScheduler::getInstance()->getTask(taskId);
    return (task != NULL && task->m_taskCategory != gfTask::Category_Item) ? true : false;
}

void grMadein::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    int taskId = collStatus->m_taskId;
    if (m_useLandCheck != true || grMadeinIsLandingTask(taskId)) {
        m_landerTaskId = taskId;
        m_isLandedOn = true;
    }
}

void grMadein::startEntity() {
    gfModelAnimation* anim = m_modelAnims[0];
    if (anim != NULL) {
        anim->setFrame(0.0f);
        anim->setLoop(false);
    }
    m_alive = true;
    setVisibility(1);
    m_loopCount = 0;
    createAttackPoint();
    if (m_hasHitPoint) {
        enableHit(0, 0);
    }
    link(this != NULL);
    if (m_yakumono != NULL) {
        m_yakumono->link(m_yakumono != NULL);
    }
    nw4r::g3d::ScnMdl* model = m_sceneModels[0];
    if (model != NULL) {
        model->SetScnObjOption(1, m_anmVals[0]);
        if (m_isUpdateG3dCalcWorld) {
            model->SetScnObjOption(2, m_anmVals[1]);
            model->SetScnObjOption(3, m_anmVals[2]);
            model->SetScnObjOption(4, m_anmVals[3]);
            model->SetScnObjOption(8, m_anmVals[7]);
        }
        model->SetScnObjOption(5, m_anmVals[4]);
        model->SetScnObjOption(6, m_anmVals[5]);
        model->SetScnObjOption(7, m_anmVals[6]);
        model->SetScnObjOption(9, m_anmVals[8]);
        nwSMSetVisibility(model, true);
    }
}

void grMadein::endEntity() {
    gfModelAnimation* anim = m_modelAnims[0];
    if (anim != NULL) {
        anim->setFrame(0.0f);
    }
    m_alive = false;
    setVisibility(0);
    if (m_hasAttack) {
        disableAttack(0);
    }
    if (m_hasHitPoint) {
        disableHit(0, 0);
    }
    unlink();
    if (m_yakumono != NULL) {
        m_yakumono->unlink();
    }
    nw4r::g3d::ScnMdl* model = m_sceneModels[0];
    if (model != NULL) {
        nwSMSetVisibility(model, false);
        model->SetScnObjOption(1, 1);
        model->SetScnObjOption(2, 1);
        model->SetScnObjOption(3, 1);
        model->SetScnObjOption(4, 1);
        model->SetScnObjOption(5, 1);
        model->SetScnObjOption(6, 1);
        model->SetScnObjOption(7, 1);
        model->SetScnObjOption(8, 1);
        model->SetScnObjOption(9, 0);
    }
}
