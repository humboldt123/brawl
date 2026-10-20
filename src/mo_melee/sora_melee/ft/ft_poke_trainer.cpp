// The trainer's long animation-command override puts an extra Fighter virtual before getOwner.
#define FT_FIGHTER_ANIMCMD_LONG
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <ft/poke/ft_poke_trainer.h>
#include <so/so_slow.h>
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

// Event kinds sent to the Pokemon through the link module (nodes).
struct ftPokeTrainerLinkEvent : soLinkEventArgs {
    ftPokeTrainerLinkEvent(int kind) : soLinkEventArgs(kind) { }
};

bool ftPokeTrainer::isExistAdvFollowEffect() {
    return m_moduleAccesser->getWorkManageModule().getInt(Work_Int_TrainerEffect) != 0;
}

void ftPokeTrainer::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { }

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
