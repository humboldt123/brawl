// StageObject: the base class of everything that lives on the stage and owns a set of modules (fighters, weapons,
// items, stage grounds, ...). It is a gfTask (so it is scheduled by the task manager), keeps the module accesser that
// reaches all of its modules, and forwards the per-frame process* callbacks and the activate / deactivate to them.
//
// Unit boundary (sora_melee): .text 0x3860-0x77DC, between clManager and soModelModuleImpl.

// MATCH-ONLY: keeps the original local order of the checks in soEventObserver::addObserverSub (see so_event_presenter.h).
#define SO_EVENT_OBSERVER_CHECK_ORDER
#define FT_FIGHTER_ANIMCMD_LONG // the animation command callback takes a long, as in the original symbol
#define MT_VEC3F_MUL_INLINE // paired-single Vec3f scale, as in the original code
#define SO_INPUT_NULL_CTOR // IpNull is built here (StageObject::getInput), its constructor needs Input(bool)
#include <so/stageobject.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/link/so_link_event_presenter.h>
#include <so/so_slow.h>
#include <so/article/so_generate_article_manage_module.h>
#include <ip/ip_null.h>
#include <so/so_module_hyp.h>

// The unit ids 5 and 7 are the slots the two observers take in the event manager of the object.
StageObject::StageObject(const char* name, int category, int unk2, int unk3, bool unk4, soModuleAccesser* moduleAccesser) :
    gfTask(name, static_cast<gfTask::Category>(category), unk2, unk3, unk4),
    soActivatable(false),
    soAnimCmdEventObserver(static_cast<short>(-1), static_cast<s8>(-1)),
    soLinkEventObserver(static_cast<short>(-1), static_cast<s8>(-1)),
    m_moduleAccesser(moduleAccesser) { }

// The empty callbacks of the base class: objects override only what they use.
void StageObject::processPreMapCorrection() { }
void StageObject::processFixPositionPreAnimCmd() { }
void StageObject::processGameProc() { }
void StageObject::processEnd() { }
void StageObject::processDebug() { }
void StageObject::renderDebug() { }
void StageObject::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* other, int unk4) { }
void StageObject::adjustParentGroundCollision(int unk1, float* unk2) { }

bool StageObject::checkTransitionStatus(u32) {
    return true;
}

bool StageObject::isActive() {
    return m_isActive;
}

// Only the animation command type 12 is observed by default.
bool StageObject::isObserv(char unk1) {
    return unk1 == 12;
}

float StageObject::getCollisionLr(soModuleAccesser* moduleAccesser) {
    return moduleAccesser->getPostureModule().getLr();
}

void soLinkEventObserver::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* other, int unk4) { }

void soAnimCmdEventObserver::addObserver(short manageId, s8 p2) {
    addObserverSub(manageId, this, p2);
}

void soLinkEventObserver::addObserver(short manageId, s8 p2) {
    addObserverSub(manageId, this, p2);
}

Input* StageObject::getInput() {
    static IpNull sNullInput;
    return &sNullInput;
}

bool StageObject::isTreadPassive() {
    return false;
}

// While the game estimates the frame (it is not paused by a hit stop) the stop module is ticked.
void StageObject::processAnim() {
    if (soSlow::getInstance()->isEstimate() == 1) {
        m_moduleAccesser->getStopModule().update();
    }
}

void StageObject::processCatch() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (soSlow::getInstance()->isAdjust() == 1) {
        acc->getStatusModule().startWatchChange();
        getCatchModule(acc).unk44() == 1;
    }
}

// Applies the ground correction, then runs the status's own map correction unless the frame is stopped or skipped.
void StageObject::processMapCorrection() {
    soModuleAccesser* acc = m_moduleAccesser;
    bool adjust = soSlow::getInstance()->isAdjust();
    acc->getGroundModule().apply();
    if (adjust == 1) {
        acc->getSituationModule().update();
    }
    updateNodeSRT();
    if (adjust == 1 && !acc->getStopModule().isStop()) {
        if (!acc->getSlowModule().isSkip()) {
            acc->getStatusModule().execMapCorrection(acc);
        }
        if (acc->getSituationModule().getKind() != Situation_Outfield) {
            acc->getAreaModule().update();
        }
    }
    acc->getAreaModule().clearLog();
}

void StageObject::renderPre() {
    m_moduleAccesser->getModelModule().setUpMtxFinal();
    m_moduleAccesser->getShadowModule().update();
    m_moduleAccesser->getVisibilityModule().update();
    getPhysicsModule(m_moduleAccesser).renderPreUnk44();
    m_moduleAccesser->getColorBlendModule().updateMaterial();
    m_moduleAccesser->getModelModule().setUpSkinningMtx();
}

// The article events tell the generate-article module that an article of this object is gone / ejected.
void StageObject::notifyArticleEventRemove(int unk1, int* unk2) {
    // HYPOTHESIS: unk2 is an article event record whose third word is the article's StageObject.
    StageObject* article = *reinterpret_cast<StageObject**>(reinterpret_cast<u8*>(unk2) + 8);
    m_moduleAccesser->getGenerateArticleManageModule().removeExistTaskId(article->m_taskId);
}

void StageObject::notifyArticleEventEject(int unk1, int unk2, int* unk3) {
    StageObject* article = *reinterpret_cast<StageObject**>(reinterpret_cast<u8*>(unk3) + 8);
    m_moduleAccesser->getGenerateArticleManageModule().eject(article->m_taskId);
}

// Keeps the previous rough position: the link target node when the object is constrained to a model, else the position.
void StageObject::updateRoughPos() {
    if (m_moduleAccesser->getLinkModule().isModelConstraint() == 1) {
        soModuleAccesser* acc = m_moduleAccesser;
        soPostureModule& posture = acc->getPostureModule();
        Vec3f pos = acc->getLinkModule().getModelConstraintTargetNodePosition();
        posture.setPrevRoughPos(&pos);
    } else {
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        Vec3f pos = posture.getPos();
        posture.setPrevRoughPos(&pos);
    }
}

StageObject::~StageObject() { }

// The ground lost the object (it walked off a ledge): the situation is refreshed and the status gets the chance to react.
void StageObject::notifyLostGround(soModuleAccesser* acc) {
    acc->getSituationModule().update();
    // HYPOTHESIS: the header types the argument of checkTransition as int, the call passes the accesser.
    acc->getMotionModule().checkTransition(reinterpret_cast<int>(acc));
    if (acc->getMotionModule().isForceImmediatelyStatusInterpret() == 1) {
        acc->getStatusModule().processFixPosition(acc, true);
    }
    acc->getStatusModule().checkTransition(acc);
}

// Position fix pass: the order is areas, items, targets, ground, then the motion and the status (which may run again in
// the "force immediately" case while the object is slowed).
void StageObject::processFixPosition() {
    soModuleAccesser* acc = m_moduleAccesser;
    bool adjust = soSlow::getInstance()->isAdjust();
    if (adjust == 1) {
        bool stopped = acc->getStopModule().isStop();
        if (!stopped) {
            acc->getAreaModule().check();
            acc->getItemManageModule().check();
        }
        if (!stopped) {
            getTargetSearchModule(acc).unk28();
        }
        acc->getGroundModule().processFixPosition();
        processFixPositionPreAnimCmd();
        bool skipped = acc->getSlowModule().isSkip();
        if (!stopped) {
            acc->getMotionModule().processFixPosition();
        }
        if (!skipped) {
            acc->getStatusModule().processFixPosition(acc, false);
        } else if (acc->getMotionModule().isForceImmediatelyStatusInterpret() == 1) {
            acc->getStatusModule().processFixPosition(acc, true);
        }
        acc->getStatusModule().checkTransition(acc);
        if (!stopped && !skipped) {
            acc->getStatusModule().execFixPosCounter(acc);
        }
        // HYPOTHESIS: the work manage module keeps a mask of the work values the fix position pass may touch (+0x8) and
        // a second word (+0xC); both are opened for the status's fix position and cleared afterwards.
        u32* workMask = reinterpret_cast<u32*>(reinterpret_cast<u8*>(&acc->getWorkManageModule()) + 8);
        workMask[0] |= 0x79FC;
        acc->getStatusModule().execFixPos(acc);
        workMask = reinterpret_cast<u32*>(reinterpret_cast<u8*>(&acc->getWorkManageModule()) + 8);
        workMask[0] = 0;
        workMask[1] = 0;
        getAbnormalModule(acc).unk1C();
    }
    getSlopeModule(acc).update(adjust);
    getPhysicsModule(acc).unk2C();
    getPhysicsModule(acc).unk3C();
    getPhysicsModule(acc).unk30(0);
}

// Collision pass, part one: every module that logs collisions starts the frame with a clean record, then the logs of the
// hit / attack / search / shield modules are filled from what touched the object.
void StageObject::processCollision() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (soSlow::getInstance()->isAdjust() == 1) {
        acc->getDamageModule().initInfo();
        getCaptureModule(acc).unk18();
        getCatchModule(acc).initInfo();
        acc->getReflectModule().initInfo();
        acc->getCollisionShieldModule().checkLog();
        acc->getCollisionReflectorModule().checkLog();
        acc->getCollisionAbsorberModule().checkLog();
        acc->getCollisionHitModule().checkLog();
        acc->getDamageModule().setInfo();
        getCollisionCatchModule(acc).unk44();
        acc->getCollisionAttackModule().checkLog();
        acc->getCollisionSearchModule().checkLog();
    }
}

// Hit pass: one reaction wins per frame. A status that changed this frame (or a capture) goes to the damage check right
// away; otherwise shields, reflectors, absorbers, damage, attacks and searches are consulted in that order and the first
// that reports a hit ends the pass. Afterwards the camera, the jostle (pushing apart) and the physics are updated.
void StageObject::processHit() {
    soModuleAccesser* acc = m_moduleAccesser;
    bool adjust = soSlow::getInstance()->isAdjust();
    if (adjust == 1) {
        if (!acc->getStatusModule().isChanged()) {
            if (getCaptureModule(acc).unk60() == 1) {
                acc->getDamageModule().check(false);
            } else if (acc->getCollisionShieldModule().check() != 1 &&
                       acc->getCollisionReflectorModule().check() != 1 &&
                       acc->getCollisionAbsorberModule().check() != 1 &&
                       acc->getDamageModule().check(false) != 1 &&
                       acc->getCollisionAttackModule().check() != 1) {
                acc->getCollisionSearchModule().check();
            }
        } else {
            acc->getDamageModule().check(true);
        }
    }
    SituationKind kind = acc->getSituationModule().getKind();
    if (kind != Situation_Outfield) {
        acc->getCameraModule().update();
        if (adjust == 1 && !isJostleNull(acc)) {
            int direction = -1;
            switch (kind) {
            case Situation_Ground:
                direction = 0;
                break;
            case Situation_Air:
            case Situation_Water:
                direction = 1;
                break;
            case Situation_Cliff:
                direction = 2;
                break;
            }
            getJostleModule(acc).unk30(direction);
        }
    }
    getPhysicsModule(acc).unk40();
}

// Camera pass: the status and model finish their camera-dependent work and the sound follows the object.
void StageObject::processFixCamera() {
    soModuleAccesser* acc = m_moduleAccesser;
    acc->getStatusModule().execFixCamera(acc);
    acc->getModelModule().setUpMtxFinal();
    bool adjust = soSlow::getInstance()->isAdjust();
    if (adjust == 1) {
        if (!acc->getStopModule().isStop() || acc->getMotionModule().isProcessFixPositionDone() == 1) {
            acc->getMotionModule().processFixCamera();
        }
    }
    acc->getEffectModule().update(adjust);
    Vec3f pos = acc->getPostureModule().getPos();
    acc->getSoundModule().update(&pos);
    acc->getGroundModule().updateCheckGrCollision();
    updateRoughPos();
}

// Collision pass, part zero: every collision module is told where the object is (position, scale, facing, situation, speed)
// so that its shapes follow the object this frame.
void StageObject::processPreCollision() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (soSlow::getInstance()->isAdjust() == 1) {
        Vec3f pos = acc->getPostureModule().getPos();
        float scale = acc->getPostureModule().getScale();
        SituationKind kind = acc->getSituationModule().getKind();
        float lr = getCollisionLr(acc);
        Vec2f center(pos.m_x, pos.m_y);
        Vec3f speed3 = acc->getKineticModule().getSumSpeed3f(soKineticEnergy::AttributeFlag(0xFFFF));
        acc->getCollisionHitModule().update(pos.m_x, scale, lr, kind);
        Vec2f speed(speed3.m_x, speed3.m_y);
        acc->getCollisionAttackModule().update(&center, scale, lr, kind, &speed);
        acc->getCollisionShieldModule().update(pos.m_x, scale, lr);
        acc->getCollisionReflectorModule().update(pos.m_x, scale, lr);
        acc->getCollisionAbsorberModule().update(pos.m_x, scale, lr);
        getCollisionCatchModule(acc).unk3C(pos.m_x, scale, lr);
        acc->getCollisionSearchModule().update(pos.m_x, scale);
    }
}

// Writes the object's posture (uniform scale, rotation with the facing turn added to Y, position) into the model's root node.
void StageObject::updateNodeSRT() {
    soModuleAccesser* acc = m_moduleAccesser;
    float s = acc->getPostureModule().getScale();
    Vec3f scale;
    scale.m_x = scale.m_y = scale.m_z = s;
    Vec3f rot;
    Vec3f translate;
    translate = acc->getPostureModule().getPos();
    rot = acc->getPostureModule().getRot(0);
    rot.m_y += acc->getPostureModule().getRotYLr();
    acc->getModelModule().setNodeSRT(0, &scale, &rot, &translate);
}

// Takes the object off the stage: every module is deactivated (the argument tells the article module whether the
// object counts as removed), then the task leaves the task list and the object is marked inactive.
void StageObject::deactivate(bool treatAsRemoved) {
    soModuleAccesser* acc = m_moduleAccesser;
    acc->getAnimCmdModule().deactivate(acc);
    acc->getStatusModule().deactivate(acc);
    getCaptureModule(acc).unk10();
    getCatchModule(acc).deactivate();
    acc->getItemManageModule().deactivate();
    acc->getGenerateArticleManageModule().deactivate(treatAsRemoved);
    acc->getLinkModule().deactivate();
    getPhysicsModule(acc).unk20();
    acc->getMotionModule().deactivate();
    acc->getColorBlendModule().deactivate();
    acc->getModelModule().deactivate();
    acc->getEffectModule().deactivate();
    acc->getSoundModule().deactivate();
    acc->getGroundModule().deactivate();
    acc->getCameraModule().deactivate();
    acc->getAreaModule().deactivate();
    acc->getTeamModule().deactivate();
    acc->getControllerModule().deactivate();
    acc->getShadowModule().deactivate();
    acc->getSituationModule().deactivate();
    acc->getCollisionAttackModule().deactivate();
    acc->getCollisionHitModule().deactivate();
    acc->getCollisionShieldModule().deactivate();
    acc->getCollisionReflectorModule().deactivate();
    acc->getCollisionAbsorberModule().deactivate();
    getCollisionCatchModule(acc).unk10();
    acc->getCollisionSearchModule().deactivate();
    acc->getSlowModule().deactivate();
    acc->deactivate();
    m_alive = false;
    if (unk2C_b0 == 1) {
        unlink();
    }
    m_isActive = false;
}

// Puts the object on the stage: the task is linked into the task list, then every module is activated in a fixed order
// (animation commands, status, controller, camera, model, ..., the collision modules once the posture, situation and
// scale are known, then the remaining modules). lr is the facing (1.0 right, -1.0 left); damage is the percent the
// damage module starts with.
void StageObject::activate(float lr, float damage, Vec3f* pos, bool updateTaskId) {
    m_isActive = true;
    soModuleAccesser* acc = m_moduleAccesser;
    m_alive = true;
    if (unk2C_b0 == 0) {
        link(true);
    }
    if (updateTaskId == 1) {
        updateId();
    }
    acc->activate();
    acc->getAnimCmdModule().activate(acc);
    acc->getStatusModule().activate(acc);
    acc->getControllerModule().activate();
    acc->getCameraModule().activate();
    acc->getModelModule().activate();
    getPhysicsModule(acc).unk24();
    getSlopeModule(acc).activate();
    acc->getShadowModule().activate();
    acc->getPostureModule().init(pos, lr);
    acc->getKineticModule().activate();
    acc->getMotionModule().activate();
    acc->getGroundModule().activate(m_taskId, reinterpret_cast<Vec2f*>(pos));
    acc->getSituationModule().activate();
    acc->getEffectModule().activate();
    getComboModule(acc).unk0C();
    acc->getItemManageModule().activate();
    acc->getAreaModule().activate(m_taskId);
    getTerritoryModule(acc).unk18();
    getTargetSearchModule(acc).unk1C();
    updateNodeSRT();
    float scale = acc->getPostureModule().getScale();
    SituationKind kind = acc->getSituationModule().getKind();
    Vec2f center(*pos);
    acc->getCollisionAttackModule().activate(scale, lr, m_taskId, &center, kind, true);
    acc->getCollisionHitModule().activate(pos->m_x, scale, lr, static_cast<int>(m_taskId), kind);
    acc->getCollisionShieldModule().activate(pos->m_x, scale, lr, m_taskId);
    acc->getCollisionReflectorModule().activate(pos->m_x, scale, lr, m_taskId);
    acc->getCollisionAbsorberModule().activate(pos->m_x, scale, lr, m_taskId);
    getCollisionCatchModule(acc).unk0C(pos->m_x, scale, lr, m_taskId, true);
    acc->getCollisionSearchModule().activate(scale, m_taskId);
    acc->getDamageModule().activate(damage);
    getCaptureModule(acc).unk0C();
    getCatchModule(acc).activate();
    acc->getReflectModule().activate();
    acc->getStopModule().activate();
    getTurnModule(acc).unk0C();
    getShakeModule(acc).unk0C();
    acc->getLinkModule().activate();
    acc->getVisibilityModule().activate();
    acc->getColorBlendModule().activate();
    acc->getGenerateArticleManageModule().activate();
    acc->getWorkManageModule().activate();
    acc->getSoundModule().activate(pos);
    acc->getTeamModule().activate();
    getJostleModule(acc).unk18();
    getAbnormalModule(acc).unk18();
    acc->getParamCustomizeModule().activate();
    getGlowModule(acc).unk10();
    acc->getSlowModule().activate();
}

// Moves the posture by the object's kinetic energies. Energies are grouped by attribute flags: bit 0 marks the ones that
// are scaled by the object's own slow rate, bit 3 the ones that are applied last and never scaled.
// HYPOTHESIS: the argument is true when the movement of this frame should honour the slow rate.
void StageObject::updatePosture(bool honourSlow) {
    soModuleAccesser* acc = m_moduleAccesser;
    Vec3f pos = acc->getPostureModule().getPos();
    Vec3f rot = acc->getPostureModule().getRot(0);
    if (honourSlow == 1) {
        float rate = acc->getSlowModule().getRate();
        if (rate == 1.0f) {
            Vec3f speed = acc->getKineticModule().getSumSpeed3f(soKineticEnergy::AttributeFlag(0xFFF7));
            Vec3fAdd(&pos, &pos, &speed);
            Vec3f rotSpeed = acc->getKineticModule().getSumRotation(soKineticEnergy::AttributeFlag(0xFFF7));
            Vec3fAdd(&rot, &rot, &rotSpeed);
        } else {
            Vec3f scaled = acc->getKineticModule().getSumSpeed3f(soKineticEnergy::AttributeFlag(1));
            Vec3fScale(&scaled, &scaled, rate);
            Vec3fAdd(&pos, &pos, &scaled);
            Vec3f fixedSpeed = acc->getKineticModule().getSumSpeed3f(soKineticEnergy::AttributeFlag(0xFFF6));
            Vec3fAdd(&pos, &pos, &fixedSpeed);
            scaled = acc->getKineticModule().getSumRotation(soKineticEnergy::AttributeFlag(1));
            Vec3fScale(&scaled, &scaled, rate);
            Vec3fAdd(&rot, &rot, &scaled);
            Vec3f fixedRot = acc->getKineticModule().getSumRotation(soKineticEnergy::AttributeFlag(0xFFF6));
            Vec3fAdd(&rot, &rot, &fixedRot);
        }
    }
    Vec3f lastSpeed = acc->getKineticModule().getSumSpeed3f(soKineticEnergy::AttributeFlag(8));
    Vec3fAdd(&pos, &pos, &lastSpeed);
    acc->getPostureModule().setPos(&pos);
    Vec3f lastRot = acc->getKineticModule().getSumRotation(soKineticEnergy::AttributeFlag(8));
    Vec3fAdd(&rot, &rot, &lastRot);
    acc->getPostureModule().setRot(&rot, 0);
}

// The animation commands every stage object understands (motion scripts use them to show / hide / reposition the object):
//   Update_Node_SRT  the model's root node follows the posture again,
//   Set_Inactive     hide the object, switch its collision, area, camera, jostle and shadow off and park it outside the
//                    field (the situation becomes Outfield, no ground correction) with the default motion,
//   Set_Active       show it and switch those modules back on,
//   Set_Air          put the object in the air (Air situation, air ground correction).
bool StageObject::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (acmd->getType() <= -1 || acmd->getType() >= 5) {
        return false;
    }
    {
        {
            switch (acmd->getType()) {
            case AnimCmd::Type_Update_Node_SRT:
                updateNodeSRT();
                return true;
            case AnimCmd::Type_Set_Inactive: {
                m_moduleAccesser->getVisibilityModule().setWhole(0);
                m_moduleAccesser->getCollisionHitModule().setWhole(2, 0);
                m_moduleAccesser->getAreaModule().setWhole(0);
                m_moduleAccesser->getCameraModule().setWhole(0);
                m_moduleAccesser->getSituationModule().setKind(Situation_Outfield, false);
                m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_None, 0);
                soMotionChangeParam changeParam;
                changeParam.m_kind = -1;
                changeParam.m_frame = 0.0f;
                changeParam.m_rate = 1.0f;
                changeParam._12 = 0;
                changeParam._13 = 0;
                changeParam._14 = 0;
                changeParam._15 = 0;
                m_moduleAccesser->getMotionModule().changeMotionRequest(&changeParam);
                getJostleModule(m_moduleAccesser).unk1C(0);
                m_moduleAccesser->getShadowModule().setActiveStatus(0);
                return true;
            }
            case AnimCmd::Type_Set_Active:
                m_moduleAccesser->getVisibilityModule().setWhole(1);
                m_moduleAccesser->getCollisionHitModule().setWhole(0, 0);
                m_moduleAccesser->getAreaModule().setWhole(1);
                m_moduleAccesser->getCameraModule().setWhole(1);
                getJostleModule(m_moduleAccesser).unk1C(1);
                m_moduleAccesser->getShadowModule().setActiveStatus(1);
                return true;
            case AnimCmd::Type_Set_Air:
                m_moduleAccesser->getSituationModule().setKind(Situation_Air, false);
                m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
                return true;
            default:
                return false;
            }
        }
    }
}

// The frame update. While the frame is estimated (not paused by a hit stop) every module starts its frame (begin), the
// status runs (execStatus, or execStop while the object itself is stopped), the kinetic energies are applied and the
// posture is moved. Afterwards the global slow motion decides how the motion frame advances: kind 0 rewinds the motion /
// posture and plays it again at the slow rate, kind 1 just plays the motion at the rate, kind 2 turns it back.
void StageObject::processUpdate() {
    soModuleAccesser* acc = m_moduleAccesser;
    bool stopped = acc->getStopModule().isStop();
    acc->getModelModule().begin(true);
    if (soSlow::getInstance()->isEstimate() == 1) {
        bool skipped = acc->getSlowModule().isSkip();
        getDebugModule(acc).unk10();
        acc->getSlowModule().update(true);
        acc->getControllerModule().update(getInput(), false);
        if (!skipped) {
            acc->getModelModule().begin(false);
        }
        acc->getPostureModule().begin();
        acc->getMotionModule().begin();
        getPhysicsModule(acc).unk28();
        getShakeModule(acc).unk20();
        getTurnModule(acc).unk18(true);
        acc->getEffectModule().begin();
        acc->getStatusModule().begin();
        acc->getCollisionAttackModule().begin();
        acc->getCollisionHitModule().begin();
        acc->getCollisionShieldModule().begin();
        acc->getCollisionReflectorModule().begin();
        acc->getCollisionAbsorberModule().begin();
        getCollisionCatchModule(acc).unk14();
        acc->getCollisionSearchModule().begin();
        acc->getItemManageModule().begin();
        if (!skipped) {
            acc->getColorBlendModule().update();
        }
        acc->getGroundModule().begin();
        if (!stopped) {
            getComboModule(acc).unk14();
            if (!skipped) {
                acc->getStatusModule().execStatus(acc);
            }
            acc->getMotionModule().updateFrame(1.0f);
            if (!skipped) {
                acc->getKineticModule().updateEnergy(acc, soKineticEnergy::AttributeFlag(0xFFFF));
            } else {
                acc->getKineticModule().updateEnergy(acc, soKineticEnergy::AttributeFlag(0xFFFE));
            }
            updatePosture(true);
            acc->getDamageModule().update();
        } else {
            if (!acc->getSlowModule().isSkip()) {
                acc->getStatusModule().execStop(acc);
            }
            acc->getKineticModule().updateEnergy(acc, soKineticEnergy::AttributeFlag(8));
            updatePosture(false);
        }
    } else {
        acc->getSlowModule().update(false);
        getTurnModule(acc).unk18(false);
        getPhysicsModule(acc).unk28();
        acc->getKineticModule().updateEnergy(acc, soKineticEnergy::AttributeFlag(8));
        updatePosture(false);
    }
    acc->getMotionModule().updateNode();
    if (soSlow::getInstance()->isOn() == 1) {
        switch (soSlow::getInstance()->getKind()) {
        case 0:
            if (!stopped) {
                acc->getMotionModule().rewind();
                acc->getMotionModule().updateFrame(soSlow::getInstance()->getRate());
            }
            acc->getPostureModule().rewind();
            break;
        case 1:
            if (!stopped) {
                acc->getMotionModule().updateFrame(soSlow::getInstance()->getRate());
            }
            break;
        case 2:
            if (!stopped) {
                acc->getMotionModule().turnBack();
            }
            break;
        }
        acc->getPostureModule().interpolate(soSlow::getInstance()->getRate());
    }
    if (acc->getGroundModule().isReferenceModelNode() || getPhysicsModule(acc).unk34(1)) {
        updateNodeSRT();
    }
    getPhysicsModule(acc).unk30(1);
    acc->getGroundModule().update(soSlow::getInstance()->isAdjust() == 1);
    acc->getGroundModule().updateGrCollisionMatrix();
}
