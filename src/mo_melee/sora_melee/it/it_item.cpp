// BaseItem: the base class of every item (food, bombs, capsules, trophies' stand-ins, Pokeballs, ...). It is a
// StageObject that carries its own module set inline and asks an itCustomizer (one per item kind) for the behavior that
// differs between kinds: most of the small members below just forward to a module that is embedded in the item or to
// the customizer.
//
// Unit boundary (sora_melee): .text 0x27F4D8-0x29065C (item.cpp: the camera helpers, BaseItem and the weak helpers its
// constructor instantiates).

#define IT_CUSTOMIZER_BOOL_HOOKS // the default hooks below return values, see it/item.h
#include <it/item.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/area/so_area_enviroment_element_checker_impl.h>
#include <so/so_slow.h>
#include <cm/cm_camera_controller.h>
#include <gf/gf_task.h>
#include <ft/ft_manager.h>
#include <so/so_external_value_accesser.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/team/so_team.h>
#include <so/event/so_event_manage_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/param/so_param_customize_module_impl.h>

// MATCH-ONLY: it_manager.h cannot be included here (its customizer declarations disagree with the bool hooks of
// itCustomizerInterface below), so the two itManager members BaseItem uses are declared on their own.
class itManager {
public:
    static itManager* getInstance();
    void removeItem(BaseItem* item);
};

// HYPOTHESIS: the global item value accesser (bss 0x71C8) has a float getter at vtable offset 0x1C taking the module accesser, parameter id and a default.
struct ItValueAccesserHyp {
    u32 unk0;
    virtual void unk08();
    virtual void unk0C();
    virtual float getCommonFloat(soModuleAccesser* accesser, int id, int unk);
    virtual int getCommonInt(soModuleAccesser* accesser, int id, int unk);
    virtual void unk18();
    virtual float getFloat(soModuleAccesser* accesser, int id, int unk);
    virtual int getInt(soModuleAccesser* accesser, int id, int unk);
};
extern char itValueAccesserGlobalStorage[];
#define itValueAccesserGlobal (*reinterpret_cast<ItValueAccesserHyp*>(itValueAccesserGlobalStorage))

// HYPOTHESIS: the glow module (module enumeration +0xC8) has a getter and a setter for the glow attack at 0x18/0x1C.
struct soGlowModuleHyp {
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual int getGlowAttack();
    virtual void setGlowAttack(int glowAttack);
};

// HYPOTHESIS: the item's collision attack module has three slots past soCollisionAttackModule that processUpdate uses to hand the throw speed power over (names guessed from their arguments).
struct soItemAttackModuleHyp {
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44();
    virtual void unk48();
    virtual void unk4C();
    virtual void unk50();
    virtual void unk54();
    virtual void unk58();
    virtual void unk5C();
    virtual void unk60();
    virtual void unk64();
    virtual void unk68();
    virtual void unk6C();
    virtual float getPower(u32 index, bool);
    virtual void unk74();
    virtual void unk78();
    virtual void unk7C();
    virtual void unk80();
    virtual void unk84();
    virtual void unk88();
    virtual void unk8C();
    virtual void unk90();
    virtual void unk94();
    virtual void unk98();
    virtual void unk9C();
    virtual void unkA0();
    virtual void unkA4();
    virtual void unkA8();
    virtual void unkAC();
    virtual void unkB0();
    virtual void unkB4();
    virtual void unkB8();
    virtual void setSpeedPowerAdd(float);
    virtual float getSpeedPowerCoef();
    virtual void setSpeedPowerMul(float);
};

// MATCH-ONLY: the embedded modules of the item as real members (MWCC addresses a member of `this` differently from a
// pointer computed by hand). Only the members the code below needs; the pads are sized so that every offset is checked
// by the compiler. The offsets come from BaseItem::BaseItem.
#define IT_PAD(name, from, to) u8 name[(to) - (from)]
struct ItemView {
    u8 pad0[0xDB4];
    soPostureModuleImpl posture; // 0xDB4
    IT_PAD(pad1, 0xDB4 + sizeof(soPostureModuleImpl), 0xE7C);
    soGroundModuleImpl ground; // 0xE7C
    IT_PAD(pad2a, 0xE7C + sizeof(soGroundModuleImpl), 0x15C0);
    soItemAttackModuleHyp attack; // 0x15C0
    IT_PAD(pad2b, 0x15C0 + sizeof(soItemAttackModuleHyp), 0x27A8);
    soLinkModuleImpl link; // 0x27A8
    IT_PAD(pad3a, 0x27A8 + sizeof(soLinkModuleImpl), 0x27FC);
    soVisibilityModuleSimple visibility; // 0x27FC
    IT_PAD(pad3b, 0x27FC + sizeof(soVisibilityModuleSimple), 0x297C);
    soArrayVector<s32, 31> intWork; // 0x297C
    IT_PAD(pad4, 0x297C + sizeof(soArrayVector<s32, 31>), 0x2AAC);
    soWorkManageModuleImpl workManage; // 0x2AAC
    IT_PAD(pad5, 0x2AAC + sizeof(soWorkManageModuleImpl), 0x2C34);
    soStatusModuleImpl status; // 0x2C34
    IT_PAD(pad6a, 0x2C34 + sizeof(soStatusModuleImpl), 0x33D0);
    soAnimCmdModuleImpl animCmd; // 0x33D0
    IT_PAD(pad6b, 0x33D0 + sizeof(soAnimCmdModuleImpl), 0x3474);
    itKineticModuleImpl kinetic; // 0x3474
    IT_PAD(pad7a, 0x3474 + sizeof(itKineticModuleImpl), 0x38CC);
    soEffectModuleImpl effect; // 0x38CC
    IT_PAD(pad7b, 0x38CC + sizeof(soEffectModuleImpl), 0x3A68);
    soAreaModuleImpl area; // 0x3A68
    soArrayVector<soAreaInstance, 3> areaInstances; // 0x3AB4
};
inline ItemView& view(BaseItem* item) {
    return *reinterpret_cast<ItemView*>(item);
}


// HYPOTHESIS: the edges of the camera range the item checks itself against (CameraController 0x148..0x154).
struct cmCameraControllerHyp {
    u8 pad[0x148];
    float left;
    float right;
    float top;
    float bottom;
};

// HYPOTHESIS: the object whose vptr sits at +0x3C, reduced to the 135 slots in front of the one updateLifeTime calls.
#define IT_V4(n) virtual void n##0(); virtual void n##1(); virtual void n##2(); virtual void n##3();
#define IT_V16(n) IT_V4(n##a) IT_V4(n##b) IT_V4(n##c) IT_V4(n##d)
struct ItemVtableHypPad {
    u8 pad[0x3C];
};
// HYPOTHESIS: slot 0xB4 of the same vtable (called after the posture changed; the name is unknown).
struct ItemVtableHypB4 : ItemVtableHypPad {
    IT_V16(a0) IT_V16(a1) IT_V4(a2) IT_V4(a3) virtual void u0(); virtual void u1(); virtual void u2();
    virtual void unkB4();
};
struct ItemVtableHyp : ItemVtableHypPad {
    IT_V16(s0) IT_V16(s1) IT_V16(s2) IT_V16(s3) IT_V16(s4) IT_V16(s5) IT_V16(s6) IT_V16(s7)
    IT_V4(t) virtual void t4(); virtual void t5(); virtual void t6();
   
    virtual void unk224(bool prev);
};

namespace {
// HYPOTHESIS: the modules that BaseItem::BaseItem builds inline, by offset in the item (module accesser at 0xCC).
const int kEventManageModule = 0x264;
const int kSituationModule = 0xF04;
const int kPostureModule = 0xDB4;
const int kTeamModule = 0xF60;
const int kLinkModule = 0x27A8;
const int kVisibilityModule = 0x27FC;
const int kWorkManageModule = 0x2AAC;
const int kStatusModule = 0x2C34;
const int kKineticModule = 0x3474;
const int kParamCustomizeModule = 0x3D10;
const int kGroundModule = 0xE7C;
const int kSoundModule = 0x25A0;
const int kModelModule = 0xA04;
const int kIntWork = 0x297C;
const int kAreaModule = 0x3A68;
const int kAreaInstances = 0x3AB4;
const int kPickUpAreaRequested = 0x3C68;

template <class T>
inline T& embedded(BaseItem* item, int offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<u8*>(item) + offset);
}
} // namespace

// Work value ids the item keeps in its work manage module (see BaseItem::Instance::Work).
itKind BaseItem::getItKind() {
    return m_kind;
}

u32 BaseItem::getItVariation() {
    return m_variation;
}

itCustomizerInterface* BaseItem::getCustomizer() {
    return m_customizer;
}

int BaseItem::getArticleId() {
    return m_articleId;
}

s16 BaseItem::getArticleEventManageId() {
    return embedded<soEventManageModuleImpl>(this, kEventManageModule).getManageId();
}

int BaseItem::getTaskId() {
    return m_taskId;
}

int BaseItem::getLotCreateItKind() {
    return m_lotCreateKind;
}

bool BaseItem::isSyncOwnerStatus() {
    return false;
}

void BaseItem::setSyncOwnerStatus(bool) { }

soKind BaseItem::soGetKind() {
    return static_cast<soKind>(4);
}

void BaseItem::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { }
void BaseItem::notifyEventCollisionShield(soCollisionAttackModule*, float, soCollisionLog*, int, float, float, soModuleAccesser*) { }

// ---- Forwarders to the embedded modules -----------------------------------------------------------------------------

void BaseItem::changeStatus(int statusKind) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->status.changeStatusRequest(statusKind, &moduleAccesser);
}

// The item is lost (it times out, is thrown away, ...) / is born (a lot-created item appears): both are status requests.
void BaseItem::requestLost() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->status.changeStatusRequest(Status::Lost, &moduleAccesser);
}

void BaseItem::born() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->status.changeStatusRequest(Status::Born, &moduleAccesser);
}

u8 BaseItem::getVisibilityWhole() {
    return embedded<soVisibilityModule>(this, kVisibilityModule).getWhole();
}

void BaseItem::setSituationKind(SituationKind kind) {
    embedded<soSituationModule>(this, kSituationModule).setKind(kind, false);
}

bool BaseItem::isConstraint() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    return v->link.isModelConstraint();
}

void BaseItem::setConstraintTargetNode(int node) {
    moduleAccesser.getLinkModule().setModelConstraintTargetNode(node);
}

void BaseItem::setAttackPowerMulPattern(float pattern) {
    embedded<soParamCustomizeModule>(this, kParamCustomizeModule).setAttackPowerMulPattern(pattern);
}

bool BaseItem::isActiveArticle() {
    return isActive();
}

void BaseItem::deactivateArticle() {
    deactivate();
}

bool BaseItem::isVanishMode() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    return v->workManage.isFlag(Instance::Work::Flag_Vanish);
}

void BaseItem::setVanishMode(bool vanish) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->workManage.setFlag(vanish, Instance::Work::Flag_Vanish);
}

void BaseItem::linkOwner(int index, int taskId) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->link.link(index, taskId);
}

float BaseItem::getInhaledCount() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    return v->workManage.getFloat(Instance::Work::Float_Inhaled_Count);
}

void BaseItem::setSmashPlateFigurelized(int figurelized) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->workManage.setInt(figurelized, Instance::Work::Int_Message_Enemy);
}

void BaseItem::resetRotation(Vec3f* rotation) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->posture.setRot(rotation, 0);
}

Vec3f BaseItem::getPos() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    return v->posture.getPos();
}

Vec3f BaseItem::getRot() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    return v->posture.getRot(0);
}

int BaseItem::getTeamOwnerTaskId() {
    return embedded<soTeamModule>(this, kTeamModule).getTeamOwnerId();
}

// ---- Forwarders to the customizer (the per item kind behavior) ------------------------------------------------------

void BaseItem::onUpdateAiMove(Vec2f* a, Vec2f* b, Vec2f* c) {
    m_customizer->onUpdateAiMove(this, a, b, c);
}

void BaseItem::onUpdateKinetic() {
    m_customizer->onUpdateKinetic(this);
}

void BaseItem::onPreBound(Vec2f* speed, BoundMask mask) {
    m_customizer->onPreBound(this, speed, mask);
}

bool BaseItem::onPreDamageCheck(int damageStatusKind, soDamageLog* log) {
    return m_customizer->onPreDamageCheck(this, damageStatusKind, log);
}

void BaseItem::onDamage(int damageStatusKind, soDamageLog* log) {
    m_customizer->onDamage(this, damageStatusKind, log);
}

bool BaseItem::isUseTurnDamage() {
    return m_customizer->isUseTurnDamage(this);
}

bool BaseItem::isUseSpeedDamage() {
    return m_customizer->isUseSpeedDamage(this);
}

bool BaseItem::isUseShake() {
    return m_customizer->isUseShake(this);
}

bool BaseItem::isTreadPassive() {
    return m_customizer->isTreadPassive(this);
}

bool BaseItem::isSafe() {
    return m_customizer->isSafe(this);
}

void BaseItem::notifyEventGimmick(soGimmickEventArgs* eventInfo, int* taskId) {
    m_customizer->onNotifyEventGimmick(this, eventInfo, taskId);
}

int BaseItem::getBullet() {
    return m_customizer->onGetBullet(this);
}

u32 BaseItem::shootBlanks(u32 count) {
    return m_customizer->onShootBlanks(this, count);
}

void BaseItem::action(u32 kind, float rate) {
    m_customizer->onAction(this, kind, rate);
}

int BaseItem::getResult() {
    return m_customizer->onResult(this);
}

// ---- Embedded module helpers, part two -------------------------------------------------------------------------------

// Bit n of this byte says that fighter / player n has held the item (see setHavedLog).
void BaseItem::setHavedLog(int index, bool haved) {
    u8& log = *(reinterpret_cast<u8*>(this) + 0x8D4);
    if (haved) {
        log |= (1 << index);
    } else {
        log &= ~(1 << index);
    }
}

int BaseItem::getTeamNo() {
    return embedded<soTeamModule>(this, kTeamModule).getTeam()->getNo();
}

bool BaseItem::isSmashPlateFigurelized() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    return v->workManage.getInt(Instance::Work::Int_Message_Enemy) != 0;
}

// An item that is being sucked in (Kirby's inhale) counts its inhale frames in a float work value; -1 means not inhaled.
bool BaseItem::isInhaled() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    return v->workManage.getFloat(Instance::Work::Float_Inhaled_Count) >= 0.0f;
}

// Moves the item to a position: the posture follows and the model node is rewritten.
void BaseItem::move(Vec3f* pos) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->posture.setPos(pos);
    updateNodeSRT();
}

void BaseItem::resetLr(float lr) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->posture.setLr(lr);
    v->posture.updateRotYLr();
}

// 'e' is the extra animation command group that items observe on top of the StageObject default.
bool BaseItem::isObserv(char group) {
    return StageObject::isObserv(group) || group == 'e';
}

void BaseItem::remove() {
    itManager::getInstance()->removeItem(this);
}

// Back to the standby status; force skips the transition check.
void BaseItem::standby(bool force) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    if (force) {
        v->status.changeStatusForce(Status::Standby, &moduleAccesser);
    } else {
        v->status.changeStatusRequest(Status::Standby, &moduleAccesser);
    }
}

// Held by a fighter: either in the Have status or linked to a holder (link number 3).
bool BaseItem::isHaved() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    if (moduleAccesser.getStatusModule().getStatusKind() == Status::Have) {
        return true;
    }
    return v->link.isLink(Link::No_Have);
}

void BaseItem::renderDebug() {
    StageObject::renderDebug();
    m_customizer->onRenderDebug(this);
}

// ---- Pick up, eat, inhale, team and scale ----------------------------------------------------------------------------

// The pick-up area (area kind 2) is the third area of the item; it is only usable when it was requested.
bool BaseItem::isPickUp() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    if (!(*(reinterpret_cast<u8*>(this) + kPickUpAreaRequested) == 0 || v->areaInstances.size() <= 2)) {
        return v->area.isEnableArea(Area::Pickup);
    }
    return false;
}

// Switches the pick-up area on or off. It stays off while the item still has no-damage frames left.
void BaseItem::setPickUpAreaRequest(bool request) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    if (v->areaInstances.size() > 2) {
        *(reinterpret_cast<u8*>(this) + kPickUpAreaRequested) = request;
        if (!request ||
            v->workManage.getInt(Instance::Work::Int_No_Damage_Frame) < 1) {
            v->area.enableArea(Area::Pickup, *(reinterpret_cast<u8*>(this) + kPickUpAreaRequested), -1);
        } else {
            v->area.enableArea(Area::Pickup, false, -1);
        }
    }
}

// Food can be eaten while it is flagged eatable and nobody holds it.
bool BaseItem::isEatable() {
    if (!(reinterpret_cast<ItemView*>(this))->workManage.isFlag(Instance::Work::Flag_Eatable)) {
        return false;
    } else {
        if (isHaved() != 0) {
            return false;
        }
        return true;
    }
}

void BaseItem::warp(Vec3f* pos) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->ground.relocate(reinterpret_cast<Vec2f*>(pos), 0);
    v->posture.initPos(pos);
    updateNodeSRT();
    v->ground.update(1);
}

// The hit team follows the team until the two are told apart; then the team is set and announced.
void BaseItem::setTeam(int team) {
    soTeamModule& teamModule = embedded<soTeamModule>(this, kTeamModule);
    int hitTeamNo = teamModule.getHitTeam()->getNo();
    if (teamModule.getTeam()->getNo() == hitTeamNo) {
        teamModule.setHitTeam(team);
    }
    teamModule.setTeam(team, true);
}

// Inhale (Kirby): a free item whose trait flag does not forbid it and whose customizer calls it safe can be sucked in.
bool BaseItem::isCanInhaled() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    if (!isHaved() && m_customizer->isSafe(this)) {
        return (v->intWork.at(8) & 0x1000000) == 0;
    }
    return false;
}

Vec3f BaseItem::getStartInhaledPos() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    if (v->workManage.getFloat(Instance::Work::Float_Inhaled_Count) < 0.0f) {
        return v->posture.getPos();
    }
    return m_startInhaledPos;
}

// Starts a scale animation of the "keep" kind on the node the item is held with.
void BaseItem::setScale(float scale) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    soWorkManageModule& work = v->workManage;
    work.setInt(work.getInt(Instance::Work::Int_Have_Node_Index), Instance::Work::Int_Scale_Anim_Node_Index);
    work.setInt(Scale::Anim_Keep, Instance::Work::Int_Scale_Anim_Type);
    work.setInt(0, Instance::Work::Int_Scale_Anim_Counter);
    work.setInt(-1, Instance::Work::Int_Scale_Anim_Counter_Limit);
    work.setFloat(scale, Instance::Work::Float_Scale_Anim_Now);
}

void BaseItem::unlinkOwner(int index) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    if (v->link.isLink(index) == 1) {
        v->link.unlink(index);
    }
}

soLogAttackInfo BaseItem::getLogAttackInfo() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    soWorkManageModule& work = v->workManage;
    soLogAttackInfo info;
    int kind = work.getInt(Instance::Work::Int_Attack_Kind);
    info._0 = work.getInt(Instance::Work::Int_Attack_Serial_Id);
    info._4 = kind;
    info._8 = 0;
    return info;
}

void BaseItem::setLogAttackInfo(soLogAttackInfo* info) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    soWorkManageModule& work = v->workManage;
    work.setInt(info->_0, Instance::Work::Int_Attack_Serial_Id);
    work.setInt(info->_4, Instance::Work::Int_Attack_Kind);
}

// ---- Per frame passes -------------------------------------------------------------------------------------------------

// Items run the customizer after the StageObject pass (and the kinetic module before it).
void BaseItem::processMapCorrection() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->kinetic.onProcessMapCorrection(m_moduleAccesser);
    StageObject::processMapCorrection();
}

void BaseItem::processFixCamera() {
    m_customizer->onProcessFixCamera(this);
    StageObject::processFixCamera();
}

// The scale animation advances one step per rendered frame (counter 0..2) when it was requested.
void BaseItem::renderPre() {
    StageObject::renderPre();
    u8& counter = *(reinterpret_cast<u8*>(this) + 0x3D24);
    u8& requested = *(reinterpret_cast<u8*>(this) + 0x3D25);
    if (requested != 0) {
        counter++;
        if (counter > 2) {
            counter = 0;
        }
        requested = 0;
    }
    m_customizer->onRenderPre(this);
}

// The item's remaining life time ticks down while it lies on the ground (or is marked to vanish); it blinks (the scale
// animation counter) before it is gone. While not held, it also hides itself when it leaves the camera range.
void BaseItem::updateLifeTime() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    soWorkManageModuleImpl* work = &v->workManage;
    int statusKind = v->status.getStatusKind();
    *(reinterpret_cast<u8*>(this) + 0x3D25) = 0;
    int lifeTime = work->getInt(Instance::Work::Int_Life_Time);
    if (lifeTime > 0) {
        u32 vanish = work->isFlag(Instance::Work::Flag_Vanish);
        if (vanish != 0 && (static_cast<const soArrayVector<s32, 31>&>(v->intWork).at(8) & 0x4C00) == 0) {
            u32 landed = v->kinetic.isTouchDown(*reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x60));
            if (landed != 0) {
                int held = (moduleAccesser.getStatusModule().getStatusKind() == 2) ? 1 : v->link.isLink(Link::No_Have);
                landed = (held == 0);
            }
            if (landed == 0) {
                landed = work->isFlag(Instance::Work::Flag_Ground_Lost);
            }
            vanish = landed;
        }
        bool counts = true;
        u32 shifted = statusKind - 2;
        if (shifted <= 5 && ((1 << shifted) & 0x31) != 0) {
            counts = false;
        }
        if ((counts && itValueAccesserGlobal.getInt(&moduleAccesser, 0x5B6F, 0) == 1 &&
             lifeTime < itValueAccesserGlobal.getInt(&moduleAccesser, 0x59D9, 0)) ||
            work->isFlag(Instance::Work::Flag_Auto_Play_Lost_Effect)) {
            bool prev = *(reinterpret_cast<u8*>(this) + 0x3D24) != 0;
            if (vanish == 0 && !work->isFlag(Instance::Work::Flag_Auto_Play_Lost_Effect)) {
                *(reinterpret_cast<u8*>(this) + 0x3D24) = 1;
            } else {
                *(reinterpret_cast<u8*>(this) + 0x3D25) = 1;
            }
            // HYPOTHESIS: slot 0x224 of the StageObject vtable (vptr at 0x3C) takes the previous blink state.
            reinterpret_cast<ItemVtableHyp*>(this)->unk224(prev);
        }
        if (vanish != 0) {
            work->setInt(lifeTime - 1, Instance::Work::Int_Life_Time);
            if (lifeTime - 1 <= 0) {
                v->status.changeStatusRequest(Status::Lost, &moduleAccesser);
                return;
            }
        }
    }
    const s32* flags = &static_cast<const soArrayVector<s32, 31>&>(v->intWork).at(8);
    if ((*flags & 0xC00) != 0 && ((flags = &static_cast<const soArrayVector<s32, 31>&>(v->intWork).at(8)), true) &&
        ((*flags & 0x8000000) | (*flags & 0x4000)) == 0) {
        bool held = moduleAccesser.getStatusModule().getStatusKind() == 2 ? true : v->link.isLink(Link::No_Have);
        if (!held && moduleAccesser.getStatusModule().getStatusKind() != 6 &&
            *(reinterpret_cast<u8*>(this) + 0x3D26) == 0) {
            bool visible = true;
            cmCameraControllerHyp* camera = reinterpret_cast<cmCameraControllerHyp*>(CameraController::getInstance());
            if (camera != NULL) {
                Vec3f pos = v->posture.getPos();
                pos.m_x += *reinterpret_cast<float*>(reinterpret_cast<u8*>(this) + 0x28BC);
                pos.m_y += *reinterpret_cast<float*>(reinterpret_cast<u8*>(this) + 0x28C0);
                float postureScale = v->posture.getScale();
                float modelScale = embedded<soModelModuleImpl>(this, kModelModule).getScale();
                float scaled = *reinterpret_cast<float*>(reinterpret_cast<u8*>(this) + 0x28C8) * modelScale;
                float radius = scaled * postureScale * 1.25f + 25.0f;
                camera = reinterpret_cast<cmCameraControllerHyp*>(CameraController::getInstance());
                if (pos.m_x + radius < camera->left || pos.m_x - radius > camera->right ||
                    pos.m_y - radius > camera->top || pos.m_y + radius < camera->bottom) {
                    visible = false;
                }
            }
            if (visible != v->visibility.isVisible()) {
                bool sync = false;
                if (m_kind == 0x6A || (m_kind < 0x6A && m_kind == 0x67)) {
                    sync = true;
                }
                v->visibility.setWhole(visible);
                embedded<soEffectModuleImpl>(this, 0x38CC)
                    .setSyncVisibility(visible ? *reinterpret_cast<bool*>(reinterpret_cast<u8*>(this) + 0x3D27) : sync);
            }
        }
    }
}

// An info window request restarts the item's anim cmd at the given frame (the same call requestAnimEvent makes).
void BaseItem::notifyEventInfoWindow(int frame) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->workManage.setInt(frame, Instance::Work::Int_Bound_Flag);
    v->animCmd.resetInterpreter2(0.0f, 5, &moduleAccesser, 1);
    v->animCmd.interpretCmd1(1.0f, 5, &moduleAccesser);
}

void BaseItem::requestAnimEvent(u32 frame) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->animCmd.resetInterpreter2(0.0f, 5, &moduleAccesser, frame);
    v->animCmd.interpretCmd1(static_cast<s32>(frame), 5, &moduleAccesser);
}

bool BaseItem::isSteal() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    if (m_kind == 0x53) {
        if (*(reinterpret_cast<u8*>(this) + kPickUpAreaRequested) != 0 && v->areaInstances.size() > 2) {
            return v->area.isEnableArea(2);
        }
        return false;
    }
    return false;
}

// Starts an interpolating scale animation on the given node; the lower end is the current scale unless a minimum is given.
void BaseItem::setScaleAnimation(float scaleMax, float scaleMin, int nodeIndex, int counterLimit) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    soWorkManageModule& work = v->workManage;
    work.setInt(nodeIndex, Instance::Work::Int_Scale_Anim_Node_Index);
    work.setInt(Scale::Anim_Interpolate, Instance::Work::Int_Scale_Anim_Type);
    work.setInt(0, Instance::Work::Int_Scale_Anim_Counter);
    work.setInt(counterLimit, Instance::Work::Int_Scale_Anim_Counter_Limit);
    float low = work.getFloat(Instance::Work::Float_Scale_Anim_Now);
    if (0.0f <= scaleMin) {
        low = scaleMin;
    }
    work.setFloat(low, Instance::Work::Float_Scale_Anim_Min);
    work.setFloat(scaleMax, Instance::Work::Float_Scale_Anim_Max);
}

// The scale of an item that was thrown or dropped by another item follows the owner's scale (or the scale of the link
// when the owner is carried), but only for items whose scale type says so.
void BaseItem::updateOwnerScale(BaseItem* owner) {
    ItemView* o = reinterpret_cast<ItemView*>(owner);
    if (itValueAccesserGlobal.getInt(&moduleAccesser, 0x5B6D, 0) == 1 &&
        itValueAccesserGlobal.getInt(&owner->moduleAccesser, 0x5B6D, 0) == 1) {
        float scale;
        if (itValueAccesserGlobal.getInt(&owner->moduleAccesser, 0x5B6D, 0) == 1) {
            int held;
            if (owner->moduleAccesser.getStatusModule().getStatusKind() == 2) {
                held = 1;
            } else {
                held = o->link.isLink(Link::No_Have);
            }
            if (held != 0) {
                scale = o->link.getLinkScale();
            } else {
                scale = o->posture.getOwnerScale();
            }
        } else {
            scale = 1.0f;
        }
        setOwnerScale(scale);
    }
}

void BaseItem::setOwnerScale(float scale) {
    reinterpret_cast<ItemView*>(this)->posture.setOwnerScale(scale);
    reinterpret_cast<ItemVtableHypB4*>(this)->unkB4();
}

// Tells the parent of the item (link 5) that it was touched by this item, at the given position. When nobody
// accepted the touch the "touch" work flag is cleared again.
bool BaseItem::sendTouchMessage(int taskId, Vec3f* pos, float power) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->link.link(Link::No_Touch, taskId);
    soLinkTouchItemEventArgs args(this, *pos, power, false);
    v->link.sendEventParents(Link::No_Touch, args);
    if (v->link.isLink(Link::No_Touch)) {
        v->link.unlink(Link::No_Touch);
    }
    if (!args.m_0x1C) {
        v->workManage.offFlag(0x22000005);
    }
    return args.m_0x1C;
}

float BaseItem::getAttackPower(int index) {
    return reinterpret_cast<ItemView*>(this)->attack.getPower(index, false);
}

void BaseItem::addSpeed(Vec3f* speed, bool unk) {
    reinterpret_cast<ItemView*>(this)->kinetic.addSpeed(speed, unk, &moduleAccesser);
}

void BaseItem::setGlowAttack(int glowAttack) {
    reinterpret_cast<soGlowModuleHyp*>(moduleAccesser.m_enumerationStart->m_glowModule)->setGlowAttack(glowAttack);
}

int BaseItem::getGlowAttack() {
    return reinterpret_cast<soGlowModuleHyp*>(moduleAccesser.m_enumerationStart->m_glowModule)->getGlowAttack();
}

void BaseItem::setSafePos(Vec2f* pos) {
    reinterpret_cast<ItemView*>(this)->ground.setShapeSafePos(pos, 0);
}

int BaseItem::getTrait() {
    return static_cast<const soArrayVector<s32, 31>&>(reinterpret_cast<ItemView*>(this)->intWork).at(8);
}

template <class T>
inline T* castTask(u32 taskId) {
    if (taskId == 0xFFFFFFFF) {
        return NULL;
    }
    gfTask* task = gfTask::getTask(taskId);
    if (task != NULL) {
        return dynamic_cast<T*>(task);
    }
    return NULL;
}

BaseItem* BaseItem::getCreaterItem() {
    BaseItem* item;
    if (static_cast<u32>(m_createOwnerTaskId) == 0xFFFFFFFF) {
        item = NULL;
    } else {
        item = castTask<BaseItem>(m_createOwnerTaskId);
    }
    return item;
}

// Hides or shows the item; the effects follow it unless the "sync visibility" flag is off. The Force variant also
// pauses the effects while the item is hidden.
void BaseItem::setVisibilityWhole(u8 visible) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->visibility.setWhole(visible);
    u32 synced = v->effect.isSyncVisibility();
    if (*(reinterpret_cast<u8*>(this) + 0x3D27) != synced) {
        v->effect.setSyncVisibility(*reinterpret_cast<bool*>(reinterpret_cast<u8*>(this) + 0x3D27));
        bool whole = true;
        if (*(reinterpret_cast<u8*>(this) + 0x3D27) != 0) {
            whole = visible;
        }
        v->effect.setWhole(whole);
    }
    if (*(reinterpret_cast<u8*>(this) + 0x3D26) != 0) {
        v->effect.reqPause(false);
        *(reinterpret_cast<u8*>(this) + 0x3D26) = 0;
    }
}

void BaseItem::setVisibilityWholeForce(u8 visible) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    v->visibility.setWhole(visible);
    if (v->effect.isSyncVisibility() != 1) {
        v->effect.setSyncVisibility(true);
        v->effect.setWhole(visible);
    }
    bool hidden = (visible == 0);
    *(reinterpret_cast<u8*>(this) + 0x3D26) = hidden;
    v->effect.reqPause(hidden);
}

float BaseItem::getLinkOwnerScale() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    if (itValueAccesserGlobal.getInt(&moduleAccesser, 0x5B6D, 0) == 1) {
        int held;
        if (moduleAccesser.getStatusModule().getStatusKind() == 2) {
            held = 1;
        } else {
            held = v->link.isLink(Link::No_Have);
        }
        if (held != 0) {
            return v->link.getLinkScale();
        }
        return v->posture.getOwnerScale();
    }
    return 1.0f;
}

Vec3f BaseItem::getHavedParentSafePos() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    int held;
    if (moduleAccesser.getStatusModule().getStatusKind() == 2) {
        held = 1;
    } else {
        held = v->link.isLink(Link::No_Have);
    }
    if (held != 0) {
        Vec2f center = v->link.getParentShapeCenterPos(Link::No_Have);
        return Vec3f(center.m_x, center.m_y, 0.0f);
    }
    return v->posture.getPos();
}

void BaseItem::setRenderPriority(u8 priority) {
    nw4r::g3d::ScnMdl* model = embedded<soModelModuleImpl>(this, kModelModule).getScnMdl(false);
    if (model != NULL) {
        if (m_customizer->onGetLayerType(this) >= 3) {
            model->SetPriorityDrawXlu(priority);
            model->SetPriorityDrawOpa(priority);
        } else {
            model->SetPriorityDrawOpa(priority);
        }
    }
}

// Collects the fighters (and their sub fighters) the item may interact with; flag 1 leaves out the fighters of the item's own team.
int BaseItem::enumFighter(StageObject** out, u32 flags) {
    int* idPtr;
    int team = -1;
    ftManager* manager = g_ftManager;
    if ((flags & 1) != 0) {
        team = embedded<soTeamModule>(this, kTeamModule).getTeam()->getNo();
    }
    int buf[14];
    // HYPOTHESIS: the argument of enumEntryId is really the array that receives the entry ids.
    int entryCount = manager->enumEntryId(reinterpret_cast<int>(&buf[2]));
    idPtr = &buf[2];
    int added = 0;
    for (int i = 0; i < entryCount; i++) {
        int entryId = *idPtr;
        buf[0] = reinterpret_cast<int>(manager->getFighter(entryId, -1));
        buf[1] = reinterpret_cast<int>(manager->getSubFighter(entryId));
        if (team == -1 || manager->getTeam(entryId, true, true) != team) {
            int* fighterPtr = buf;
            u32 j = 0;
            do {
                StageObject* fighter = reinterpret_cast<StageObject*>(*fighterPtr);
                if (fighter != NULL && soExternalValueAccesser::getSituationKind(fighter) != Situation_Outfield &&
                    m_customizer->onEnumFighter(this, reinterpret_cast<Fighter*>(fighter))) {
                    *out = fighter;
                    out++;
                    added++;
                }
                j++;
                fighterPtr++;
            } while (j < 2);
        }
        idPtr++;
    }
    return added;
}

// How the item reacts when its attack hits a shield (the mode comes from the item parameters): knocked away in the
// Lost/status 6 or 7 cases, bounced back off the shield or, in the air, bounced on the ground.
bool BaseItem::onShield() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    soWorkManageModuleImpl* work = &v->workManage;
    int mode = itValueAccesserGlobal.getInt(&moduleAccesser, 0x5B73, 0);
    if (mode == 1) {
        v->status.changeStatusRequest(6, &moduleAccesser);
        return true;
    } else if (mode == 2) {
        v->status.changeStatusRequest(7, &moduleAccesser);
        return true;
    } else if (*(reinterpret_cast<u8*>(this) + 0x2130) != 0) {
        work->offFlag(0x22000003);
        float reflectY = *reinterpret_cast<float*>(reinterpret_cast<u8*>(this) + 0x212C);
        float reflectX = *reinterpret_cast<float*>(reinterpret_cast<u8*>(this) + 0x2128);
        Vec3f reflect;
        reflect.m_x = reflectX;
        reflect.m_y = reflectY;
        reflect.m_z = 0.0f;
        soKineticEnergy::AttributeFlag attr(1);
        v->kinetic.reflectSpeed(&reflect, attr);
        return true;
    } else {
        if (mode == 3 && moduleAccesser.getSituationModule().getKind() != 0) {
            work->offFlag(0x22000003);
            v->kinetic.characterBound(&moduleAccesser, false);
            return true;
        }
        return false;
    }
}

// Targets the stage object with the given task id (-1 clears the target): its position is remembered in the work module.
void BaseItem::setTarget(int taskId) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    soWorkManageModuleImpl* work = &v->workManage;
    if (v->link.isLink(Link::No_Target)) {
        v->link.unlink(Link::No_Target);
    }
    if (static_cast<u32>(taskId) == 0xFFFFFFFF) {
        work->offFlag(Instance::Work::Flag_Target_Found);
    } else {
        StageObject* target = castTask<StageObject>(taskId);
        if (target != NULL) {
            Vec3f pos = soExternalValueAccesser::getPos(target);
            work->onFlag(Instance::Work::Flag_Target_Found);
            work->setInt(taskId, Instance::Work::Int_Target_Task_Id);
            work->setFloat(pos.m_x, Instance::Work::Float_Target_Pos_X);
            work->setFloat(pos.m_y, Instance::Work::Float_Target_Pos_Y);
            if (static_cast<u32>(taskId) != 0xFFFFFFFF) {
                v->link.link(Link::No_Target, taskId);
            }
        }
    }
}

// Attaches the item to a model node of another object (e.g. a fighter's hand): the link number 3 is the held link.
bool BaseItem::setModelConstraint(int taskId, int targetNode, int node) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    if (static_cast<u32>(taskId) == 0xFFFFFFFF || static_cast<u32>(targetNode) == 0xFFFFFFFF || static_cast<u32>(node) == 0xFFFFFFFF) {
        return false;
    }
    v->link.removeModelConstraint(true);
    if (v->link.isLink(Link::No_Have)) {
        v->link.unlink(Link::No_Have);
    }
    if (!v->link.link(Link::No_Have, taskId)) {
        return false;
    }
    if (m_customizer->isConstraintHave(this)) {
        v->link.setModelConstraintPosOrt(Link::No_Have, node, targetNode, 3, true);
        if (itValueAccesserGlobal.getInt(&moduleAccesser, 0x5B6D, 0) == 1) {
            v->posture.setOwnerScale(1.0f);
            v->link.setAttribute(Link::No_Have, soLinkConnection::Attribute_Reference_Parent_Scale, true);
        }
    }
    return true;
}

float BaseItem::getParam(itValueAccesser::ParamFloat param) {
    return itValueAccesserGlobal.getFloat(&moduleAccesser, param, 0);
}

int BaseItem::getParam(itValueAccesser::ParamInt param) {
    return itValueAccesserGlobal.getInt(&moduleAccesser, param, 0);
}

// A parameter id says where it lives: the high nibble 1 or 2 means a work value (nibble below: 0 int, 1 float), ids
// below 1999 / 20000 are float parameters of the common / item tables and the rest are ints. getParamFloat and
// getParamInt convert between the two kinds. MATCH-ONLY: the original expands each into the other three levels deep, so
// the levels are spelled out as inline helpers.
#define IT_PARAM_IS_WORK(param) ((param) >> 28 != 0 && (param) >> 28 < 3)
#define IT_PARAM_FLOAT(NAME, ELSE)                                                                 \
    static inline float NAME(BaseItem* item, u32 param) {                                          \
        ItemView* v = reinterpret_cast<ItemView*>(item);                                           \
        int kind = param >> 28;                                                                    \
        bool isWork = false;                                                                       \
        if (kind != 0 && kind < 3) {                                                               \
            isWork = true;                                                                         \
        }                                                                                          \
        if (isWork) {                                                                              \
            if (static_cast<int>((param >> 24) & 0xF) == 0) {                                                      \
                return v->workManage.getInt(param);                                                \
            }                                                                                      \
            return v->workManage.getFloat(param);                                                  \
        } else if (static_cast<int>(param) < 20000) {                                              \
            if (static_cast<int>(param) < 1999) {                                                  \
                return itValueAccesserGlobal.getCommonFloat((reinterpret_cast<soModuleAccesser*>(reinterpret_cast<u8*>(item) + 0xCC)), param, 0);      \
            }                                                                                      \
            return itValueAccesserGlobal.getFloat((reinterpret_cast<soModuleAccesser*>(reinterpret_cast<u8*>(item) + 0xCC)), param, 0);                \
        }                                                                                          \
        return ELSE;                                                                               \
    }
#define IT_PARAM_INT(NAME, ELSE)                                                                   \
    static inline int NAME(BaseItem* item, u32 param) {                                            \
        ItemView* v = reinterpret_cast<ItemView*>(item);                                           \
        int kind = param >> 28;                                                                    \
        bool isWork = false;                                                                       \
        if (kind != 0 && kind < 3) {                                                               \
            isWork = true;                                                                         \
        }                                                                                          \
        if (isWork) {                                                                              \
            if (static_cast<int>((param >> 24) & 0xF) == 1) {                                                      \
                return static_cast<int>(v->workManage.getFloat(param));                            \
            }                                                                                      \
            return v->workManage.getInt(param);                                                    \
        } else if (static_cast<int>(param) < 19999) {                                              \
            return static_cast<int>(ELSE);                                                         \
        } else if (static_cast<int>(param) < 21999) {                                              \
            return itValueAccesserGlobal.getCommonInt((reinterpret_cast<soModuleAccesser*>(reinterpret_cast<u8*>(item) + 0xCC)), param, 0);            \
        }                                                                                          \
        return itValueAccesserGlobal.getInt((reinterpret_cast<soModuleAccesser*>(reinterpret_cast<u8*>(item) + 0xCC)), param, 0);                      \
    }

IT_PARAM_FLOAT(paramFloatC, item->getParamInt(param))
IT_PARAM_INT(paramIntB, paramFloatC(item, param))
IT_PARAM_FLOAT(paramFloatB, paramIntB(item, param))
IT_PARAM_INT(paramIntA, paramFloatB(item, param))
IT_PARAM_FLOAT(paramFloatA, paramIntA(item, param))

float BaseItem::getParamFloat(u32 param) {
    return paramFloatA(this, param);
}

IT_PARAM_FLOAT(paramFloatY, item->getParamInt(param))
IT_PARAM_INT(paramIntY, paramFloatY(item, param))
IT_PARAM_FLOAT(paramFloatX, paramIntY(item, param))

int BaseItem::getParamInt(u32 param) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    int kind = param >> 28;
    bool isWork = false;
    if (kind != 0 && kind < 3) {
        isWork = true;
    }
    if (isWork) {
        if (static_cast<int>((param >> 24) & 0xF) == 1) {
            return static_cast<int>(v->workManage.getFloat(param));
        }
        return v->workManage.getInt(param);
    } else if (static_cast<int>(param) < 19999) {
        return static_cast<int>(paramFloatX(this, param));
    } else if (static_cast<int>(param) < 21999) {
        return itValueAccesserGlobal.getCommonInt(&moduleAccesser, param, 0);
    }
    return itValueAccesserGlobal.getInt(&moduleAccesser, param, 0);
}

void BaseItem::setModelScale(float scale) {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    embedded<soModelModuleImpl>(this, kModelModule).setScale(scale * v->workManage.getFloat(Instance::Work::Float_Base_Scale));
}

bool BaseItem::notifyEventCollisionReflectorCheck() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    int soundId = v->workManage.getInt(0x1000001A);
    if (soundId != 0) {
        embedded<soSoundModule>(this, kSoundModule).playSE(static_cast<SndID>(soundId), false, false, 0);
    }
    return false;
}

// Entering water plays the item's water sound once (the Water flag remembers it); then the customizer and the life time run.
void BaseItem::processHit() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    StageObject::processHit();
    bool standby = moduleAccesser.getStatusModule().getStatusKind() == Status::Standby;
    soSlow* slow = soSlow::getInstance();
    bool run = false;
    if (slow->isEstimate() == 1 && !standby) {
        run = true;
    }
    if (run) {
        soWorkManageModuleImpl* work = &v->workManage;
        if (v->area.getEnviromentElementChecker()->isWater()) {
            if (!work->isFlag(Instance::Work::Flag_Water)) {
                work->setFlag(true, Instance::Work::Flag_Water);
                int soundId = work->getInt(Instance::Work::Int_SE_Water_Id);
                if (soundId != 0) {
                    embedded<soSoundModule>(this, kSoundModule).playSE(static_cast<SndID>(soundId), false, 0, 0);
                }
            }
        } else {
            work->setFlag(false, Instance::Work::Flag_Water);
        }
    }
    m_customizer->onProcessHit(this);
    if (run) {
        updateLifeTime();
    }
}

// Frame update: a freshly dropped item goes to its Initialize status (once, when the frame is estimated), the StageObject
// pass runs, and the per frame attack bookkeeping is cleared. An item that can hurt by speed (flag Use_Speed_Power, no
// trait that forbids it) tells its attack module the throw speed power from the parameters.
void BaseItem::processUpdate() {
    ItemView* v = reinterpret_cast<ItemView*>(this);
    if (*(reinterpret_cast<u8*>(this) + 0x3D5D) != 0) {
        if (soSlow::getInstance()->isEstimate() == 1) {
            *(reinterpret_cast<u8*>(this) + 0x3D5D) = 0;
            v->status.changeStatusRequest(Status::Initialize, &moduleAccesser);
            // slot 0x204 of the ground module
            v->ground.setGrCollisionMode(true);
        }
    }
    StageObject::processUpdate();
    if (soSlow::getInstance()->isEstimate() == 1) {
        m_strongestAttackPower = 0.0f;
        m_hitStopFrames = 0;
        m_attackHitTaskId = -1;
        m_searchHitTaskId = -1;
        searchHitCollisionIndex = 0;
        m_15681 = 0;
        *(reinterpret_cast<u8*>(this) + 0x2130) = 0;
        if (v->attack.getSpeedPowerCoef() <= 0.0f &&
            v->workManage.isFlag(Status::Work::Flag_Use_Speed_Power) &&
            (static_cast<const soArrayVector<s32, 31>&>(v->intWork).at(8) & 0x4C00) == 0) {
            v->attack.setSpeedPowerAdd(itValueAccesserGlobal.getFloat(&moduleAccesser, itValueAccesser::Specialized_Param_Float_Throw_Power_Speed_Add, 0));
            v->attack.setSpeedPowerMul(itValueAccesserGlobal.getFloat(&moduleAccesser, itValueAccesser::Specialized_Param_Float_Throw_Power_Speed_Mul, 0));
        }
    }
    m_customizer->onProcessUpdate(this);
}

// ---- itCustomizerInterface: what an item does when its kind does not override a hook ---------------------------------
// An item that does not customize anything (itNullCustomizer) behaves like this: no per-frame work, the damage and
// speed-damage hooks accept the hit, the item uses its z scale, can be held by the usual constraint, is safe to pick up
// and has no bullets / results.

void itCustomizerInterface::onConstruct(BaseItem*) { }
void itCustomizerInterface::onDestruct(BaseItem*) { }
void itCustomizerInterface::onReset(BaseItem*) { }
void itCustomizerInterface::onProcessUpdate(BaseItem*) { }
bool itCustomizerInterface::onProcessFixPosition(BaseItem*) { return true; }
void itCustomizerInterface::onProcessHit(BaseItem*) { }
void itCustomizerInterface::onProcessFixCamera(BaseItem*) { }
void itCustomizerInterface::onProcessGameProc(BaseItem*) { }
void itCustomizerInterface::onRenderPre(BaseItem*) { }
void itCustomizerInterface::onRenderDebug(BaseItem*) { }
void itCustomizerInterface::onNotifyEventChangeStatus(BaseItem*, int, int, soStatusData*, soModuleAccesser*) { }
bool itCustomizerInterface::onNotifyEventAnimCmd(BaseItem*, acAnimCmd*, soModuleAccesser*, int) { return false; }
void itCustomizerInterface::onNotifyEventCollisionAttackCheck(BaseItem*, int) { }
bool itCustomizerInterface::onNotifyEventCollisionSearch(BaseItem*, soCollisionLog*) { return true; }
bool itCustomizerInterface::onNotifyEventCollisionSearchCheck(BaseItem*, int, u8, Vec3f*) { return false; }
void itCustomizerInterface::onNotifyEventLink(BaseItem*, void*, soModuleAccesser*, StageObject*, int) { }
bool itCustomizerInterface::onNotifyEventCaptureStatus(BaseItem*) { return false; }
void itCustomizerInterface::onNotifyEventGimmick(BaseItem*, soGimmickEventArgs*, int*) { }
void itCustomizerInterface::onUpdateKinetic(BaseItem*) { }
void itCustomizerInterface::onUpdateAiDir(BaseItem*, Vec2f*, Vec2f*, Vec2f*, Vec2f*) { }
void itCustomizerInterface::onUpdateAiMove(BaseItem*, Vec2f*, Vec2f*, Vec2f*) { }
bool itCustomizerInterface::onSearchTarget(BaseItem*, int*, Vec3f, int) { return false; }
u32 itCustomizerInterface::onShoot(BaseItem*, u32, float, float) { return 0; }
u32 itCustomizerInterface::onShootBlanks(BaseItem*, u32) { return 0; }
int itCustomizerInterface::onGetBullet(BaseItem*) { return 0; }
void itCustomizerInterface::onThrowAttack(BaseItem*, float, float*, Vec3f*, Vec3f*, float*, bool*) { }
void itCustomizerInterface::onAction(BaseItem*, u32, float) { }
int itCustomizerInterface::onResult(BaseItem*) { return -1; }
bool itCustomizerInterface::onHave(BaseItem*, int) { return true; }
bool itCustomizerInterface::onPreDamageCheck(BaseItem*, int, soDamageLog*) { return true; }
bool itCustomizerInterface::onDamage(BaseItem*, int, soDamageLog*) { return true; }
bool itCustomizerInterface::onReflect(BaseItem*) { return false; }
void itCustomizerInterface::onPreBound(BaseItem*, Vec2f*, BaseItem::BoundMask) { }
bool itCustomizerInterface::onRemoveModelConstraint(BaseItem*, int) { return true; }
bool itCustomizerInterface::onEnumFighter(BaseItem*, Fighter*) { return true; }
bool itCustomizerInterface::isTreadPassive(BaseItem*) { return false; }
bool itCustomizerInterface::isUseTurnDamage(BaseItem*) { return false; }
bool itCustomizerInterface::isUseSpeedDamage(BaseItem*) { return true; }
bool itCustomizerInterface::isUseShake(BaseItem*) { return true; }
bool itCustomizerInterface::isUseScaleZ(BaseItem*) { return true; }
bool itCustomizerInterface::isUseThrowRotParam(BaseItem*) { return true; }
bool itCustomizerInterface::isSafe(BaseItem*) { return true; }
bool itCustomizerInterface::isConstraintHave(BaseItem*) { return true; }
bool itCustomizerInterface::isReferenceControlerHave(BaseItem*) { return false; }
itParam::KineticFlag itCustomizerInterface::getKineticFlags(BaseItem*) { return itParam::KineticFlag(0); }
bool itCustomizerInterface::isUsePhysics(BaseItem*) { return false; }
