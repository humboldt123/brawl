// The trainer's long animation-command override puts an extra Fighter virtual before getOwner.
#define FT_FIGHTER_ANIMCMD_LONG
#define SO_EVENT_PRESENTER_STORE_NULL_LIST
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <ft/poke/ft_poke_trainer.h>
#include <so/so_slow.h>
#include <so/motion/so_motion_change_param.h>
#include <mt/mt_prng.h>
#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/so_external_value_accesser.h>
#include <gf/gf_task_scheduler.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <st/stage.h>
#include <types.h>

// HYPOTHESIS: only the module slots called here are known.
class soDebugModuleTrainerView {
public:
    virtual void unk08();
    virtual void unk0c();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1c();
    virtual void unk20(int);
};

struct soTwoWords {
    int unk0;
    int unk4;
};

// The jostle module keeps its vtable pointer behind two words.
class soJostleModuleTrainerView : public soTwoWords {
public:
    virtual void unk08();
    virtual void unk0c();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1c();
    virtual void unk20(int);
};

extern "C" {
extern u8 lbl_27_data_93910;
extern char lbl_27_data_9391C[];
}

// HYPOTHESIS: the trainer's part of ftData::extendParam (offsets in the common data record of the Pokemon Trainer). The sound
// table holds three voice lines per Pokemon (picked by how much damage the player took), the tiers are damage thresholds, the two
// index tables pick the reaction motion for the special moves.
struct ftPokeTrainerVoiceEntry {
    int* ids;
    int unk4;
};

struct ftPokeTrainerVoiceTable {
    ftPokeTrainerVoiceEntry* entries;
};

struct ftPokeTrainerDamageTiers {
    float high;
    float low;
};

struct ftPokeTrainerMotionRow {
    int motions[3];
};

struct ftPokeTrainerExtendView {
    u8 unk0[0x84];
    ftPokeTrainerVoiceTable* voice; // 0x84
    ftPokeTrainerDamageTiers* tiers; // 0x88
    u8 unk8C[8];
    ftPokeTrainerMotionRow* specialMotions; // 0x94: one row per Pokemon
    ftPokeTrainerMotionRow* specialMotionsSide; // 0x98: one row per stick side
};

// Work slots of the trainer fighter (the names are HYPOTHESES from how the slots are used): LA-Int 0x4A holds the handle of the
// effect that is live while the trainer follows the fight, LA-Int 0x42 counts down after a reaction, LA-Float 0x13..0x15 hold
// the position and facing that the next Pokemon appears with.
enum {
    Work_Int_ChangePose = 0x10000040,
    Work_Int_ReactionCooldown = 0x10000042,
    Work_Int_TrainerEffect = 0x1000004A,
    Work_Flag_NoVoice = 0x12000033,
    Work_Flag_ChangeRequested = 0x1200003D,
    Work_Flag_PartnerDown = 0x1200003E,
    Work_Flag_NoPartnerLeft = 0x12000043,
};

void ftPokeTrainer::onActivate() {
    ftOwner* owner = getOwner();
    soWorkManageModule* work;
    soModuleAccesser* acc = m_moduleAccesser;
    reinterpret_cast<soDebugModuleTrainerView*>(acc->m_enumerationStart->m_debugModule)->unk20(1);
    acc->getVisibilityModule().activate();
    acc->getVisibilityModule().setStatusDefault(0, -1, true);
    work = &acc->getWorkManageModule();
    work->setFloat(acc->getPostureModule().getLr(), 0x11000015);
    acc->getWorkManageModule().offFlag(Work_Flag_NoPartnerLeft);
    acc->getCameraModule().setPri(1, -1);
    reinterpret_cast<soJostleModuleTrainerView*>(acc->m_enumerationStart->m_jostleModule)->unk20(3);
    acc->getWorkManageModule().setInt(-1, 0x10000040);
    acc->getGroundModule().setTestDynamicCollision(false, 0);
    acc->getGroundModule().setEventToGround(false, 0);
    soWorkManageModule& work2 = acc->getWorkManageModule();
    work2.setFlag(owner->isNoVoice(), Work_Flag_NoVoice);
    acc->getShadowModule().setGroundPokemon(1);
}

void ftPokeTrainer::onDeactivate() {
    soModuleAccesser* acc = m_moduleAccesser;
    int effectHandle = acc->getWorkManageModule().getInt(Work_Int_TrainerEffect);
    if (effectHandle != 0) {
        acc->getEffectModule().remove(effectHandle);
        acc->getWorkManageModule().setInt(0, Work_Int_TrainerEffect);
    }
}

void ftPokeTrainer::attachInstance() {
    ftData* data = g_ftCommonDataAccesser.getData(getKindDirect());
    int layer = g_Stage->getPokeTrainerDrawLayer();
    ftOwner* owner = getOwner();
    bool spycloak = owner->getSpycloak();
    m_moduleAccesser->getShadowModule().setYOffset(8.0f);
    void* extra = (spycloak == 1) ? NULL : *reinterpret_cast<void**>(reinterpret_cast<u8*>(data) + 0x48);
    const char* name = (spycloak == 1) ? lbl_27_data_9391C : NULL;
    if (spycloak == 1) {
        layer = 5;
    }
    m_moduleAccesser->getModelModule().constructInstance(static_cast<gfSceneRoot::LayerType>(layer), 0x735B, true, 1, nw4r::g3d::ResFile(static_cast<void*>(NULL)), name, NULL, 0xFFFF, extra, lbl_27_data_93910, 0);
}

void ftPokeTrainer::detachInstance() {
    Fighter::detachInstance();
}

void ftPokeTrainer::setVisibility(int visibility) {
    soModuleAccesser* acc = m_moduleAccesser;
    if (acc->getVisibilityModule().getWhole() != visibility) {
        acc->getVisibilityModule().setWhole(visibility);
        struct {
            Vec3f rotRange;
            Vec3f posRange;
            Vec3f rot;
            Vec3f pos;
        } zero;
        zero.rotRange.m_x = 0.0f;
        zero.rotRange.m_y = 0.0f;
        zero.rotRange.m_z = 0.0f;
        zero.posRange.m_x = 0.0f;
        zero.posRange.m_y = 0.0f;
        zero.posRange.m_z = 0.0f;
        zero.rot.m_x = 0.0f;
        zero.rot.m_y = 0.0f;
        zero.rot.m_z = 0.0f;
        zero.pos.m_x = 0.0f;
        zero.pos.m_y = 0.0f;
        zero.pos.m_z = 0.0f;
        acc->getEffectModule().req(static_cast<EfID>(0x104004E), 0, &zero.pos, &zero.rot, 1.0f, &zero.posRange, &zero.rotRange, false, 0);
    }
}

// Starts the trainer for a battle or after a Pokemon change. startKind 0/3: stand ready; 1/2: step in next to the Pokemon;
// 4: cheer for the Pokemon that was just picked (voice line by Pokemon kind); 5: final appearance.
void ftPokeTrainer::trainerStart(float lr, Vec3f* pos, Vec3f* wanderPos, int startKind, bool unk5) {
    soModuleAccesser* acc = m_moduleAccesser;
    ftOwner* owner = getOwner();
    acc->getPostureModule().init(pos, 0.0f);
    acc->getPostureModule().setOwnerScale(owner->getVisibilityScale());
    acc->getGenerateArticleManageModule().deactivate(true);
    acc->getWorkManageModule().setFloat(lr, 0x11000015);
    acc->getWorkManageModule().setFloat(0.0f, 0x11000018);
    acc->getWorkManageModule().setFloat(0.0f, 0x11000016);
    acc->getWorkManageModule().setFloat(0.0f, 0x11000017);
    acc->getWorkManageModule().setInt(startKind, Work_Int_ChangePose);
    acc->getWorkManageModule().offFlag(Work_Flag_PartnerDown);
    acc->getWorkManageModule().onFlag(0x12000001);
    acc->getWorkManageModule().setInt(0, Work_Int_ReactionCooldown);
    acc->getWorkManageModule().setInt(0, 0x10000044);
    acc->getWorkManageModule().onFlag(Work_Flag_ChangeRequested);
    m_wandering.start(this, pos, wanderPos, acc);
    switch (startKind) {
    case 0:
    case 3:
        acc->getStatusModule().changeStatusForce(0x117, acc);
        break;
    case 1:
    case 2:
        if (unk5 == true) {
            if (acc->getSituationModule().getKind() == Situation_Outfield) {
                acc->getStatusModule().changeStatusForce(0x11D, acc);
            } else {
                if (acc->getSituationModule().getKind() == Situation_Ground) {
                    acc->getStatusModule().changeStatusForce(0x119, acc);
                } else {
                    acc->getStatusModule().changeStatusForce(0x11A, acc);
                }
                struct {
                    Vec3f rotRange;
                    Vec3f posRange;
                    Vec3f rot;
                    Vec3f pos;
                } zero;
                zero.rotRange.m_x = 0.0f;
                zero.rotRange.m_y = 0.0f;
                zero.rotRange.m_z = 0.0f;
                zero.posRange.m_x = 0.0f;
                zero.posRange.m_y = 0.0f;
                zero.posRange.m_z = 0.0f;
                zero.rot.m_x = 0.0f;
                zero.rot.m_y = 0.0f;
                zero.rot.m_z = 0.0f;
                zero.pos.m_x = 0.0f;
                zero.pos.m_y = 0.0f;
                zero.pos.m_z = 0.0f;
                acc->getEffectModule().req(static_cast<EfID>(0x1030039), 0x12E, &zero.pos, &zero.rot, 1.0f, &zero.posRange, &zero.rotRange, false, 0);
            }
        } else if (startKind == 2) {
            if (acc->getSituationModule().getKind() == Situation_Ground) {
                acc->getStatusModule().changeStatusForce(0x119, acc);
            } else {
                acc->getStatusModule().changeStatusForce(0x11A, acc);
            }
        } else {
            acc->getStatusModule().changeStatusForce(0x117, acc);
        }
        break;
    case 4: {
        Fighter* partner = getPartner();
        acc->getWorkManageModule().setInt(soExternalValueAccesser::getWorkInt(partner, 0x20000000), 0x10000047);
        switch (getPartner()->soGetSubKind()) {
        case 0x1D:
            acc->getWorkManageModule().setInt(0, 0x10000048);
            acc->getSoundModule().playSE(static_cast<SndID>(0x869), false, false, 0);
            break;
        case 0x1E:
            acc->getWorkManageModule().setInt(1, 0x10000048);
            acc->getSoundModule().playSE(static_cast<SndID>(0x86D), false, false, 0);
            break;
        case 0x1F:
            acc->getWorkManageModule().setInt(2, 0x10000048);
            acc->getSoundModule().playSE(static_cast<SndID>(0x871), false, false, 0);
            break;
        }
        acc->getStatusModule().changeStatusForce(0x128, acc);
        break;
    }
    case 5:
        acc->getStatusModule().changeStatusForce(0x129, acc);
        break;
    }
}

// Brings the trainer back after the Pokemon changed (or was knocked out).
void ftPokeTrainer::trainerRestart(float lr, float* pos, bool unk3, bool unk4) {
    soModuleAccesser* acc = m_moduleAccesser;
    acc->getWorkManageModule().setFloat(lr, 0x11000015);
    acc->getWorkManageModule().setFloat(pos[0], 0x11000013);
    acc->getWorkManageModule().setFloat(pos[1], 0x11000014);
    acc->getWorkManageModule().setInt(7, Work_Int_ChangePose);
    acc->getGenerateArticleManageModule().deactivate(true);
    if (unk3 == true) {
        if (unk4 == true) {
            soPostureModule& posture = acc->getPostureModule();
            Vec3f upper = m_wandering.getTargetUpperPos(acc);
            posture.setPos(&upper);
        }
        acc->getStatusModule().changeStatusForce(0x117, acc);
    } else {
        if (acc->getWorkManageModule().isFlag(Work_Flag_NoPartnerLeft) == 1) {
            setChange(0);
        }
        if (acc->getSituationModule().getKind() != Situation_Outfield) {
            acc->getStatusModule().changeStatusRequest(0x125, acc);
        } else {
            acc->getStatusModule().changeStatusRequest(0x126, acc);
        }
    }
    acc->getWorkManageModule().offFlag(Work_Flag_NoPartnerLeft);
}

void ftPokeTrainer::disappear(bool effect) {
    if (effect == true) {
        struct {
            Vec3f rotRange;
            Vec3f posRange;
            Vec3f rot;
            Vec3f pos;
        } zero;
        zero.rotRange.m_x = 0.0f;
        zero.rotRange.m_y = 0.0f;
        zero.rotRange.m_z = 0.0f;
        zero.posRange.m_x = 0.0f;
        zero.posRange.m_y = 0.0f;
        zero.posRange.m_z = 0.0f;
        zero.rot.m_x = 0.0f;
        zero.rot.m_y = 0.0f;
        zero.rot.m_z = 0.0f;
        zero.pos.m_x = 0.0f;
        zero.pos.m_y = 0.0f;
        zero.pos.m_z = 0.0f;
        m_moduleAccesser->getEffectModule().req(static_cast<EfID>(0x1030039), 0x12E, &zero.pos, &zero.rot, 1.0f, &zero.posRange, &zero.rotRange, false, 0);
    }
    m_moduleAccesser->getStatusModule().changeStatusRequest(0x10B, m_moduleAccesser);
}

Fighter* ftPokeTrainer::getPartner() {
    return g_ftManager->getFighter(m_entryId, -1);
}

bool ftPokeTrainer::isInclude(float* rect, Vec3f* out) {
    soGenerateArticleManageModule& articleModule = m_moduleAccesser->getGenerateArticleManageModule();
    soArticle* article = articleModule.getArticle(0);
    if (article != NULL) {
        int taskId = article->getTaskId();
        gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Weapon, taskId);
        if (task != NULL) {
            StageObject& object = dynamic_cast<StageObject&>(*task);
            return soExternalValueAccesser::isIncludeRect(&object, rect, out, 4.0f, 10.0f);
        }
    }
    return false;
}

void ftPokeTrainer::processUpdate() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (soSlow::getInstance()->isEstimate() == 1) {
        acc->getWorkManageModule().countDownInt(Work_Int_ReactionCooldown, 0);
        acc->getWorkManageModule().countDownInt(0x10000045, 0);
        acc->getWorkManageModule().countDownInt(0x10000046, 0);
        m_entryEventPresenter.notifyEventPokemonTrainerUpdate();
        soModuleAccesser* acc2 = m_moduleAccesser;
        int effectHandle = acc2->getWorkManageModule().getInt(Work_Int_TrainerEffect);
        if (effectHandle != 0) {
            Vec3f pos = acc2->getPostureModule().getPos();
            Vec2f diff(pos.m_x - m_advFollowPos.m_x, (10.0f + pos.m_y) - m_advFollowPos.m_y);
            if (diff.m_y * diff.m_y + diff.m_x * diff.m_x < 25.0f) {
                soModuleAccesser* acc3 = m_moduleAccesser;
                int handle = acc3->getWorkManageModule().getInt(Work_Int_TrainerEffect);
                if (handle != 0) {
                    acc3->getEffectModule().remove(handle);
                    acc3->getWorkManageModule().setInt(0, Work_Int_TrainerEffect);
                }
                struct {
                    Vec3f rotRange;
                    Vec3f posRange;
                    Vec3f rot;
                    Vec3f pos;
                } zero;
                zero.pos.m_x = 0.0f;
                zero.pos.m_y = 0.0f;
                zero.pos.m_z = 0.0f;
                zero.rot.m_x = 0.0f;
                zero.rot.m_y = 0.0f;
                zero.rot.m_z = 0.0f;
                zero.posRange.m_x = 0.0f;
                zero.posRange.m_y = 0.0f;
                zero.posRange.m_z = 0.0f;
                zero.rotRange.m_x = 0.0f;
                zero.rotRange.m_y = 0.0f;
                zero.rotRange.m_z = 0.0f;
                acc3->getEffectModule().req(static_cast<EfID>(0x1030039), 0x12E, &zero.pos, &zero.rot, 1.0f, &zero.posRange, &zero.rotRange, false, 0);
                acc3->getVisibilityModule().setWhole(true);
                m_wandering.setDirectionTarget(acc3);
            } else {
                Vec3f follow;
                follow.m_z = pos.m_z;
                follow.m_y = m_advFollowPos.m_y + 0.1f * diff.m_y;
                follow.m_x = m_advFollowPos.m_x + 0.1f * diff.m_x;
                m_advFollowPos.m_x = follow.m_x;
                m_advFollowPos.m_y = follow.m_y;
                acc2->getEffectModule().setPos(effectHandle, &follow);
            }
        }
    }
    StageObject::processUpdate();
}

void ftPokeTrainer::processFixPosition() {
    if (soSlow::getInstance()->isEstimate() == 1) {
        m_wandering.preprocessFixPosition(this, m_moduleAccesser);
    }
    StageObject::processFixPosition();
    if (soSlow::getInstance()->isEstimate() == 1) {
        // MATCH-ONLY: Fighter::getPartner is reached through the vtable (slot 0x268), which the Fighter header does not declare as virtual yet.
        typedef Fighter* (*GetPartnerFunc)(Fighter*);
        GetPartnerFunc getPartnerVirtual = *reinterpret_cast<GetPartnerFunc*>(*reinterpret_cast<u8**>(reinterpret_cast<u8*>(this) + 0x3C) + 0x268);
        m_wandering.processFixPosition(this, getPartnerVirtual(this), m_moduleAccesser);
    }
}

void ftPokeTrainer::processHit() {
    StageObject::processHit();
}

void ftPokeTrainer::processFixCamera() {
    StageObject::processFixCamera();
}

void ftPokeTrainer::renderDebug() {
    StageObject::renderDebug();
}

// Places the trainer model: the posture position, the posture rotation turned to the facing side, the posture scale.
void ftPokeTrainer::updateNodeSRT() {
    soModuleAccesser* acc = m_moduleAccesser;
    float scale = acc->getPostureModule().getScale();
    Vec3f sclVec;
    Vec3f rot;
    Vec3f pos;
    sclVec.m_x = sclVec.m_y = sclVec.m_z = scale;
    pos = acc->getPostureModule().getPos();
    rot = acc->getPostureModule().getRot(0);
    if (acc->getPostureModule().getLr() == 0.0f) {
        rot.m_y += 90.0f;
        rot.m_y += acc->getWorkManageModule().getFloat(0x11000018);
    } else {
        rot.m_y += acc->getPostureModule().getRotYLr();
    }
    acc->getModelModule().setNodeSRT(0, &sclVec, &rot, &pos);
}

void ftPokeTrainer::reqAdvFollowEffect(Vec2f* pos) {
    soModuleAccesser* acc = m_moduleAccesser;
    if (acc->getWorkManageModule().getInt(Work_Int_TrainerEffect) == 0) {
        struct {
            Vec3f rot;
            Vec3f pos;
        } effect;
        effect.rot.m_x = 0.0f;
        effect.rot.m_y = 0.0f;
        effect.rot.m_z = 0.0f;
        effect.pos.m_x = pos->m_x;
        effect.pos.m_y = pos->m_y;
        effect.pos.m_z = 0.0f;
        int handle = acc->getEffectModule().req(static_cast<EfID>(0x1030038), &effect.pos, &effect.rot, 1.0f, 0, -1);
        acc->getWorkManageModule().setInt(handle, Work_Int_TrainerEffect);
        m_advFollowPos.m_y = pos->m_y;
        m_advFollowPos.m_x = pos->m_x;
    }
}

// Link events between the trainer and its Pokemon. The Pokemon ask the trainer where to appear or leave (0x455..0x459, the answer
// is written into the event) and tell it about statuses (0x45A). HYPOTHESIS: names; the kinds are from the dispatch below.
struct ftPokeTrainerRawPos {
    int x;
    int y;
    int z;
};

struct ftPokeTrainerPosEvent : soLinkEventArgs {
    Vec3f m_pos;
    ftPokeTrainerPosEvent(int kind) : soLinkEventArgs(kind) { }
    ftPokeTrainerPosEvent(int kind, const Vec3f& pos) : soLinkEventArgs(kind), m_pos(pos) { }
};

// Event kinds sent to the Pokemon through the link module (nodes).
struct ftPokeTrainerLinkEvent : soLinkEventArgs {
    ftPokeTrainerLinkEvent(int kind) : soLinkEventArgs(kind) { }
};

bool ftPokeTrainer::isExistAdvFollowEffect() {
    return m_moduleAccesser->getWorkManageModule().getInt(Work_Int_TrainerEffect) != 0;
}

// Tells the Pokemon where they should stand: while the trainer is in the Final Smash throw (status 0x116) the positions come
// from the Final Smash article, otherwise from the work slots that hold where the trainer wants the next Pokemon to appear.
inline void ftPokeTrainer::sendPartnerPosEvents() {
    soModuleAccesser* acc = m_moduleAccesser;
    Vec3f pos;
    if (acc->getStatusModule().getStatusKind() == 0x116) {
        int taskId = acc->getWorkManageModule().getInt(0x10000043);
        gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Weapon, taskId);
        if (task != NULL) {
            wnPokeTrainerFinal& finalArticle = dynamic_cast<wnPokeTrainerFinal&>(*task);
            ftPokeTrainerPosEvent first(0x839, finalArticle.getFinalPokemonPos(0));
            acc->getLinkModule().sendEventNodes(-1, first, 0);
            ftPokeTrainerPosEvent second(0x839, finalArticle.getFinalPokemonPos(1));
            acc->getLinkModule().sendEventNodes(-1, second, 0);
        }
    } else {
        float y = acc->getWorkManageModule().getFloat(0x11000014);
        float x = acc->getWorkManageModule().getFloat(0x11000013);
        pos.m_x = x;
        pos.m_y = y;
        pos.m_z = 0.0f;
        ftPokeTrainerPosEvent event(0x839, pos);
        acc->getLinkModule().sendEventNodes(-1, event, 0);
    }
}

// Reacts to the trainer's own status changes. Statuses 0x123/0x124 are the "go and fetch the next Pokemon" statuses: when the
// owner still has more than one Pokemon the entry observers are asked where the next one should appear (and the trainer plays
// the throw motion), otherwise the trainer just stays in the wait motion. Entering 0x119 re-rolls the idle timer. The camera is
// only told to frame the trainer in the statuses where the Pokemon is being chosen.
void ftPokeTrainer::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if ((u32)(statusKind - 0x123) <= 1) {
        Vec3f requestPos;
        float requestLr;
        if (getOwner()->getPokemonChange() && getOwner()->getRemainPokemonCount() > 1 && m_entryEventPresenter.notifyEventPokemonRequestChange(&requestPos, &requestLr) == 1) {
            moduleAccesser->getWorkManageModule().setFloat(requestPos.m_x, 0x11000013);
            moduleAccesser->getWorkManageModule().setFloat(requestPos.m_y, 0x11000014);
            moduleAccesser->getWorkManageModule().setFloat(requestLr, 0x11000015);
            moduleAccesser->getWorkManageModule().setInt(6, Work_Int_ChangePose);
            soMotionChangeParam param;
            param.m_kind = 0x1ED;
            param.m_frame = 0.0f;
            param.m_rate = 1.0f;
            param._12 = 0;
            param._13 = 0;
            param._14 = 0;
            param._15 = 0;
            moduleAccesser->getMotionModule().changeMotionRequest(&param);
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
            float logDamage = getOwner()->getLog().m_logDataAccesser.getFloat(2, 0);
            float damageSince = logDamage - moduleAccesser->getWorkManageModule().getFloat(0x11000021);
            ftPokeTrainerExtendView* ext = reinterpret_cast<ftPokeTrainerExtendView*>(m_commonData);
            if (ext->tiers->high < damageSince) {
                moduleAccesser->getSoundModule().playSE(static_cast<SndID>(ext->voice->entries[3].ids[2]), false, false, 0);
            } else if (ext->tiers->low < damageSince) {
                moduleAccesser->getSoundModule().playSE(static_cast<SndID>(ext->voice->entries[3].ids[1]), false, false, 0);
            } else {
                moduleAccesser->getSoundModule().playSE(static_cast<SndID>(ext->voice->entries[3].ids[0]), false, false, 0);
            }
        } else if (moduleAccesser->getMotionModule().getKind() != 0x1EC) {
            soMotionChangeParam param;
            param.m_kind = 0x1EC;
            param.m_frame = 0.0f;
            param.m_rate = 1.0f;
            param._12 = 0;
            param._13 = 0;
            param._14 = 0;
            param._15 = 0;
            moduleAccesser->getMotionModule().changeMotionRequest(&param);
        }
    } else if (statusKind == 0x119 && statusKind != prevStatusKind) {
        int base = soValueAccesser::getConstantInt(moduleAccesser, 0x5DC0, 0);
        int range = soValueAccesser::getConstantInt(moduleAccesser, 0x5DC1, 0);
        int value = base + randi(range);
        moduleAccesser->getWorkManageModule().setInt(value, 0x10000041);
    }
    if (moduleAccesser->getSituationModule().getKind() != Situation_Outfield && !moduleAccesser->getWorkManageModule().isFlag(0x1200003F)) {
        switch (statusKind) {
        case 0x117:
        case 0x125:
            moduleAccesser->getCameraModule().setWhole(true);
            break;
        case 0x123:
            if (moduleAccesser->getMotionModule().getKind() == 0x1ED) {
                moduleAccesser->getCameraModule().setWhole(true);
            } else {
                moduleAccesser->getCameraModule().setWhole(false);
            }
            break;
        default:
            moduleAccesser->getCameraModule().setWhole(false);
            break;
        }
    } else {
        moduleAccesser->getCameraModule().setWhole(false);
    }
    updateNodeSRT();
}

void ftPokeTrainer::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { }

void ftPokeTrainer::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftPokeTrainerPosEvent* posEvent = static_cast<ftPokeTrainerPosEvent*>(eventInfo);
    switch (eventInfo->m_eventKind) {
    case 0x455: {
        Vec3f pos = soExternalValueAccesser::getPos(stageObj);
        pos.m_z = 0.0f;
        soWorkManageModule* work;
        int changePose;
        ftManager* manager = g_ftManager;
        int entryId = m_entryId;
        work = &moduleAccesser->getWorkManageModule();
        changePose = work->getInt(Work_Int_ChangePose);
        manager->toChangeAppear(entryId, work->getFloat(0x11000015), &pos, changePose);
        break;
    }
    case 0x456:
        if (moduleAccesser->getWorkManageModule().isFlag(0x1200003F) == 1) {
            posEvent->m_pos = m_wandering.getTargetUpperPos(moduleAccesser);
        } else {
            posEvent->m_pos = moduleAccesser->getModelModule().getNodeGlobalPosition(0x27, false);
        }
        break;
    case 0x457:
        if (moduleAccesser->getWorkManageModule().isFlag(0x1200003F) == 1) {
            posEvent->m_pos = m_wandering.getTargetUpperPos(moduleAccesser);
        } else {
            posEvent->m_pos = moduleAccesser->getModelModule().getNodeGlobalPosition(0x17, false);
        }
        break;
    case 0x458: {
        ftPokeTrainerPosEvent forward(0x455);
        // MATCH-ONLY: the original copies the position words as a plain struct.
        *reinterpret_cast<ftPokeTrainerRawPos*>(&forward.m_pos) = *reinterpret_cast<ftPokeTrainerRawPos*>(&posEvent->m_pos);
        moduleAccesser->getLinkModule().sendEventParents(6, forward);
        break;
    }
    case 0x459:
        if (moduleAccesser->getLinkModule().isLink(6) == 1) {
            posEvent->m_pos = soExternalValueAccesser::getPos(moduleAccesser->getLinkModule().getParent(6));
            posEvent->_0x4 = true;
        }
        break;
    case 0x45A:
        moduleAccesser->getStatusModule().changeStatusRequest(0x116, moduleAccesser);
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

bool ftPokeTrainer::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    if (!isObserv(acmd->getGroup())) {
        return false;
    }
    switch (acmd->getType()) {
    case 5:
        setChange(1);
        return true;
    case 0x2C:
        m_wandering.setDirectionTarget(moduleAccesser);
        return true;
    case 0x2D:
        sendPartnerPosEvents();
        return true;
    case 0x2E: {
        if (acmd->getArgNum() < 1) {
            return true;
        }
        acCmdArg arg;
        if (!acmd->getArg(&arg, 0)) {
            return true;
        }
        int amount = arg.getIntData();
        soInstanceManagerFullProperty<ftEntryEventObserver*>* list = m_entryEventPresenter.getObserverList();
        for (int i = 0; i < (s32)list->size(); i++) {
            list->atIndex(i)->notifyEventSetRumble(amount, 0);
        }
        return true;
    }
    case 0x2F: {
        if (acmd->getArgNum() < 1) {
            return true;
        }
        acCmdArg arg;
        if (!acmd->getArg(&arg, 0)) {
            return true;
        }
        int amount = arg.getIntData();
        soInstanceManagerFullProperty<ftEntryEventObserver*>* list = m_entryEventPresenter.getObserverList();
        for (int i = 0; i < (s32)list->size(); i++) {
            list->atIndex(i)->notifyEventStopRumble(amount);
        }
        return true;
    }
    }
    return Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
}

bool ftPokeTrainer::isObserv(char kind) {
    // HYPOTHESIS: the trainer answers to the fighter and the entry event kinds.
    return kind == 12 || kind == 101;
}

void ftPokeTrainer::notifyEventPartnerResourcePrepared() {
    ftPokeTrainerLinkEvent event(0x838);
    m_moduleAccesser->getLinkModule().sendEventNodes(-1, event, 0);
    m_moduleAccesser->getWorkManageModule().onFlag(Work_Flag_ChangeRequested);
}

void ftPokeTrainer::notifyEventDeadPartner(int index) {
    soModuleAccesser* acc = m_moduleAccesser;
    ftOwner* owner = getOwner();
    acc->getWorkManageModule().offFlag(Work_Flag_PartnerDown);
    ftManager* manager = g_ftManager;
    if (manager->m_mode == 0) {
        if (manager->isStamina() == true) {
            return;
        }
        if (manager->m_gameRule == 1 && owner->getStockCount() == 0) {
            acc->getWorkManageModule().onFlag(Work_Flag_NoPartnerLeft);
            return;
        }
        if (g_ftManager->isGameSet() == true) {
            return;
        }
    }
    if (owner->getNextPokemonAutoReload()) {
        if (owner->getPokemonChange() == 1 && owner->getRemainPokemonCount() > 1) {
            setChange(0);
            if (owner->getDeadPokemonLost() == 1) {
                owner->lostPokemon(index);
            }
            int entryId = m_entryId;
            ftManager* manager2 = g_ftManager;
            manager2->setControllerRumble(entryId, 6, soValueAccesser::getConstantInt(acc, 0x5A38, 0), 0, m_taskId);
        } else {
            ftPokeTrainerLinkEvent event(0x838);
            m_moduleAccesser->getLinkModule().sendEventNodes(-1, event, 0);
            m_moduleAccesser->getWorkManageModule().onFlag(Work_Flag_ChangeRequested);
        }
    }
}

// Tells the trainer that one of its Pokemon was sent out: the trainer links to it, and the voice line depends on how much damage
// the player took with the previous Pokemon.
void ftPokeTrainer::notifyEventPokemonStart(int taskId) {
    soModuleAccesser* acc = m_moduleAccesser;
    float damageTaken = getOwner()->getLog().m_logDataAccesser.getFloat(2, 0);
    int changePose = acc->getWorkManageModule().getInt(Work_Int_ChangePose);
    acc->getWorkManageModule().onFlag(Work_Flag_PartnerDown);
    if (acc->getLinkModule().isLink(6) == 1) {
        acc->getLinkModule().unlink(6);
    }
    acc->getLinkModule().link(6, taskId);
    // HYPOTHESIS: the two link attributes keep the Pokemon in step with the trainer (names from the module header).
    acc->getLinkModule().setAttribute(6, static_cast<soLinkConnection::Attribute>(4), true);
    acc->getLinkModule().setAttribute(6, static_cast<soLinkConnection::Attribute>(13), true);
    int pokemonIndex = taskId;
    switch (acc->getLinkModule().getParent(6)->soGetSubKind()) {
    case 0x1D:
        pokemonIndex = 0;
        break;
    case 0x1E:
        pokemonIndex = 1;
        break;
    case 0x1F:
        pokemonIndex = 2;
        break;
    }
    acc->getWorkManageModule().setInt(pokemonIndex, 0x10000049);
    if (changePose == 0 || (changePose >= 6 && changePose < 9)) {
        float damageSince = damageTaken - acc->getWorkManageModule().getFloat(0x11000021);
        ftPokeTrainerExtendView* ext = reinterpret_cast<ftPokeTrainerExtendView*>(m_commonData);
        float maxDamage = g_ftManager->getDamageMax(m_entryId);
        int tier;
        if (maxDamage < 30.0f || ext->tiers->low > damageSince) {
            tier = 0;
        } else if (maxDamage < 60.0f || ext->tiers->high > damageSince) {
            tier = 1;
        } else {
            tier = 2;
        }
        acc->getSoundModule().playSE(static_cast<SndID>(ext->voice->entries[pokemonIndex].ids[tier]), false, false, 0);
    }
    acc->getWorkManageModule().setFloat(damageTaken, 0x11000021);
    acc->getWorkManageModule().offFlag(Work_Flag_ChangeRequested);
}

void ftPokeTrainer::notifyEventPokemonCollect() {
    m_moduleAccesser->getWorkManageModule().offFlag(Work_Flag_PartnerDown);
    sendPartnerPosEvents();
    setChange(0);
}

void ftPokeTrainer::notifyEventPokemonChangeCancel() {
    soModuleAccesser* acc = m_moduleAccesser;
    acc->getGenerateArticleManageModule().removeExist(0, 0);
    if (acc->getSituationModule().getKind() == Situation_Outfield) {
        acc->getStatusModule().changeStatusRequest(0x11D, acc);
    } else if (acc->getSituationModule().getKind() == Situation_Air) {
        acc->getStatusModule().changeStatusRequest(0x11A, acc);
    } else {
        acc->getStatusModule().changeStatusRequest(0x119, acc);
    }
}

// A Pokemon used one of its special moves (status 0x112..0x114): the trainer shouts a reaction whose motion depends on the Pokemon
// and on which side the player pushes the stick.
void ftPokeTrainer::notifyEventPokemonSpecial(int unk1, int special) {
    soModuleAccesser* acc = m_moduleAccesser;
    if (acc->getSituationModule().getKind() == Situation_Outfield || acc->getStatusModule().getStatusKind() == 0x11B) {
    } else {
        int specialIndex;
        switch (special) {
        case 0x112:
            specialIndex = 0;
            break;
        case 0x113:
            specialIndex = 1;
            break;
        case 0x114:
            specialIndex = 2;
            break;
        default:
            return;
        }
        int pokemonIndex = acc->getWorkManageModule().getInt(0x10000049);
        int side = 1;
        int facing = 0;
        if (reinterpret_cast<ftPokeTrainerExtendView*>(m_commonData)->specialMotions[pokemonIndex].motions[specialIndex] == 1) {
            if (special == 0x112) {
                facing = (acc->getWorkManageModule().getFloat(0x11000015) == 1.0f) ? 2 : 1;
            } else {
                facing = (acc->getControllerModule().getStickX() > 0.0f) ? 2 : 1;
            }
        }
        float stickY = acc->getWorkManageModule().getFloat(0x1100001A);
        if (stickY > soValueAccesser::getConstantFloat(acc, 0xFA6, 0)) {
            float stickX = acc->getWorkManageModule().getFloat(0x11000017);
            float sideThreshold = soValueAccesser::getConstantFloat(acc, 0xFA7, 0);
            if (stickX > sideThreshold) {
                side = 0;
            } else if (stickX < -sideThreshold) {
                side = 2;
            }
        }
        acc->getWorkManageModule().setInt(reinterpret_cast<ftPokeTrainerExtendView*>(m_commonData)->specialMotionsSide[side].motions[facing], 0x20000000);
        acc->getStatusModule().changeStatusRequest(0x122, acc);
    }
}

// HYPOTHESIS: the trainer's own statuses (the ids are not named yet): 0x119 waits while a Pokemon fights, 0x11A/0x11D are
// the cancelled-change variants, 0x11B is the appeal pose, 0x122 plays a reaction motion that the work slot selects.
void ftPokeTrainer::notifyEventPokemonAppeal() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (acc->getSituationModule().getKind() != Situation_Outfield) {
        if (acc->getStatusModule().getStatusKind() == 0x11B) {
        } else {
            acc->getStatusModule().changeStatusRequest(0x127, acc);
        }
    }
}

void ftPokeTrainer::notifyEventPokemonRebirthEnd() {
    soModuleAccesser* acc = m_moduleAccesser;
    ftPokeTrainerLinkEvent event(0x83A);
    acc->getLinkModule().sendEventNodes(-1, event, 0);
}

void ftPokeTrainer::notifyEventPokemonAttack() { }

void ftPokeTrainer::notifyEventPokemonInflict() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (acc->getSituationModule().getKind() != Situation_Outfield) {
        if (acc->getStatusModule().getStatusKind() == 0x119) {
            if (acc->getWorkManageModule().getInt(Work_Int_ReactionCooldown) == 0) {
                acc->getWorkManageModule().setInt(0x1E8, 0x20000000);
                acc->getStatusModule().changeStatusRequest(0x122, acc);
            }
        }
    }
}

void ftPokeTrainer::notifyEventPokemonDamage() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (acc->getSituationModule().getKind() != Situation_Outfield) {
        if (acc->getStatusModule().getStatusKind() == 0x119) {
            if (acc->getWorkManageModule().getInt(Work_Int_ReactionCooldown) == 0) {
                acc->getWorkManageModule().setInt(0x1EA, 0x20000000);
                acc->getStatusModule().changeStatusRequest(0x122, acc);
            }
        }
    }
}

void ftPokeTrainer::notifyEventPokeTrainerReplace(u8 index) {
    m_wandering.replace(index, this, m_moduleAccesser);
}

void ftPokeTrainer::notifyEventBeat() {
    soModuleAccesser* acc = m_moduleAccesser;
    if (acc->getSituationModule().getKind() != Situation_Outfield) {
        switch (acc->getStatusModule().getStatusKind()) {
        case 0x122:
            if (acc->getMotionModule().getKind() == 0x1EB || acc->getMotionModule().getKind() == 0x1E9) {
            } else {
            }
            break;
        case 0x119:
        case 0x11E:
        case 0x11F:
        case 0x120:
        case 0x121:
        case 0x127:
            acc->getWorkManageModule().setInt(0x1E9, 0x20000000);
            acc->getStatusModule().changeStatusRequest(0x122, acc);
            break;
        }
    }
}

// Hands the turn to the next Pokemon that is not lost yet (the owner keeps one lost flag per Pokemon).
void ftPokeTrainer::setChange(int changeKind) {
    soModuleAccesser* acc = m_moduleAccesser;
    int count = g_ftManager->getFighterCount(m_entryId);
    if ((u8)count > 2) {
        int last;
        int next = g_ftManager->getCurrentFighterNo(m_entryId);
        last = (u8)count - 1;
        u32 tries = 0;
        do {
            next++;
            if ((u8)next >= last) {
                next = 0;
            }
            if (!getOwner()->isLostPokemon(next)) {
                break;
            }
            tries++;
        } while (tries < 3);
        g_ftManager->toChange(m_entryId, (u8)next, changeKind, 0);
    }
    acc->getWorkManageModule().offFlag(Work_Flag_ChangeRequested);
}

void ftPokeTrainer::notifyEventWarp() { }

void ftPokeTrainer::setLoupeDamage(bool) { }

void ftPokeTrainer::setLoupe(bool) { }

void ftPokeTrainer::setNameCursor(bool) { }

void ftPokeTrainer::setCursor(bool) { }

void ftPokeTrainer::standbyAdvFollow() { }
