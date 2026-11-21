#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL
#define YK_DATA_INLINE_CTOR
#define SO_COLLISION_GROUP_ALIGNED

#include <yk/yk_normal.h>
#include <yk/yk_no_hit_normal.h>
#include <gr/gr_gimmick_motion_path.h>
#include <sora_adv_stage/gr_adventure_move_attack_ground.h>

grAdventureMoveAttackGround* grAdventureMoveAttackGround::create(int mdlIndex, const char* taskName) {
    grAdventureMoveAttackGround* ground = new (Heaps::StageInstance) grAdventureMoveAttackGround(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureMoveAttackGround::grAdventureMoveAttackGround(const char* taskName) : grAdventureMoveGround(taskName) {
    m_effectHandle = 0;
    m_moveAttackData = NULL;
    // HYPOTHESIS: unk15C is ykData.
    reinterpret_cast<ykData*>(unk15C)->m_dataGroupNum = 0;
    reinterpret_cast<ykData*>(unk15C)->m_dataGroups = NULL;
    m_effectId = static_cast<EfID>(0);
    m_transparencySettings = 0x1F;
    unkF8 = 50.0f;
}

grAdventureMoveAttackGround::~grAdventureMoveAttackGround() { }

// HYPOTHESIS: the hit object of the moving attack ground has an attack module and a damage module but no hit module; the
// original names it ykNormal.
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
          m_attackConfig(&moduleAccesser, ykGetTaskId(this), (gfTask::Category)(u8)m_taskCategory,
                         ykGetObserverRegistrationDesc()),
          m_damageBuilder(&moduleAccesser, lbl_27_bss_5FF4, ykGetObserverRegistrationDesc()) { }
    virtual ~ykNormal() { }
};

void grAdventureMoveAttackGround::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grAdventureMoveGround::startup(data, unk1, layerType);
    m_moveAttackData = m_gimmickData;
    reinterpret_cast<ykData*>(unk15C)->m_dataGroupNum = 0;
    reinterpret_cast<ykData*>(unk15C)->m_dataGroups = NULL;

    ykInitInfo info = { 0, 0, 0x11, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = reinterpret_cast<ykData*>(unk15C);
    typedef ykNormal<
        soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true>,
        soCollisionHitModuleBuildConfigNull>
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(info.m_pos, -1.0f, 0.0f);
    setYakumono(yakumono);
    setAttackMoveGround();
}

void grAdventureMoveAttackGround::setAttackMoveGround() {
    grGimmickMoveAttackGroundData* data = static_cast<grGimmickMoveAttackGroundData*>(m_moveAttackData);
    grYakumono::setAttackGimmick(0, 0, data->m_attackData.m_nodeIndex, &data->m_attackData, NULL);
}

void grAdventureMoveAttackGround::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_effectHandle != 0) {
        ecMgr::getInstance()->setPos(m_effectHandle, &getPos());
        ecMgr::getInstance()->setRot(m_effectHandle, &getRot());
    }
    if (!m_gimmickMotionPath->isMoveEnd()) {
        // HYPOTHESIS: unk0[5] is grGimmickMotionPathData::m_pathMode (Path_Loop == 1).
        if (static_cast<grGimmickMoveAttackGroundData*>(m_moveAttackData)->unk0[5] == 1) {
            if (m_gimmickMotionPath->isStartFrame()) {
                setValid(1);
            }
            if (m_gimmickMotionPath->isEndFrame()) {
                setValid(0);
            }
        }
    }
    grGimmick::updateCallback(0);
}

void grAdventureMoveAttackGround::setEffect(EfID effectId) {
    m_effectId = effectId;
    m_effectHandle = g_ecMgr->setEffect(effectId);
}

void grAdventureMoveAttackGround::setValid(u32 unk1) {
    grYakumono::setValid(unk1);
    if (unk1 == 1) {
        grYakumono::disableAttack(0);
        grYakumono::setAttackGimmick(0, 0, 0, &static_cast<grGimmickMoveAttackGroundData*>(m_moveAttackData)->m_attackData,
                                     NULL);
    }
}

void grAdventureMoveAttackGround::setValidAttachedEffect(u32 unk1) {
    grYakumono::setValidAttachedEffect(unk1);
    if (unk1 == 1) {
        grYakumono::disableAttack(0);
        grYakumono::setAttackGimmick(0, 0, 0, &static_cast<grGimmickMoveAttackGroundData*>(m_moveAttackData)->m_attackData,
                                     NULL);
    }
}

void grAdventureMoveAttackGround::setVisibilityByClipping(u32 unk1) {
    Ground::setVisibilityByClipping(unk1, 0);
    if (unk1 != 0 && !m_isVisibleByClipping) {
        if (m_effectId != 0) {
            m_effectHandle = g_ecMgr->setEffect(m_effectId);
        }
    } else if (unk1 == 0 && m_isVisibleByClipping) {
        if (m_effectHandle != 0) {
            g_ecMgr->endEffect(m_effectHandle);
        }
    }
    m_isVisibleByClipping = unk1;
}

// TODO: clean up - this only reproduces the loop of the inlined soArrayVectorAbstract<soCollisionAttackPart>::set without its guard;
// find where the original sets this (it is the state of the optimizer when the weak functions are compiled).
#pragma opt_loop_invariants off
