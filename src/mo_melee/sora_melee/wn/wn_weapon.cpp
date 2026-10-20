#define SO_EVENT_OBSERVER_MANAGE_S16
#define MT_VEC3F_MUL_INLINE
#define WN_ARTICLE_ANIMCMD_INT
#define SO_ARTICLE_DTOR_INLINE
#include <wn/weapon.h>
#include <gf/gf_task.h>
#include <gf/gf_task_scheduler.h>
#include <gm/gm_global.h>
#include <so/ground/so_ground_util.h>
#include <so/motion/so_motion_change_param.h>
#include <so/so_common_data_accesser.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <types.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_info.h>
#include <wn/wn_module_slots.h>
#include <revolution/OS/OSError.h>
#include <so/area/so_area_module_impl.h>
#include <so/article/so_generate_article_manage_module.h>

// The base class of every article: Mario's fireball, Fox's blaster bullet, a Pikmin... A Weapon is a StageObject that owns
// a module accesser built by the article's builder, registers itself as observer of the status, attack and hit events of
// its modules, and implements the generic article protocol: activation and deactivation, linking to the owner that threw
// it, the reflect and hop reactions, being inhaled (Kirby) and the removal outside of the dead area.

// HYPOTHESIS: names of the work variables; the numbers come from the accessors below.
enum {
    Weapon_Work_Int_Activate_Founder_Id = 0x10000000,
    Weapon_Work_Int_Log_Attack_Info0 = 0x10000001,
    Weapon_Work_Int_Log_Attack_Info1 = 0x10000002,
    Weapon_Work_Int_Init_Life = 0x10000003,
    Weapon_Work_Int_Life = 0x10000004,
    Weapon_Work_Float_Active_Global_Frame = 0x11000000,
    Weapon_Work_Flag_No_Dead_Area_Remove = 0x12000000,
    Weapon_Work_Flag_Keep_Model = 0x12000001,
};

// Frame flag words of the work module (+8 and +0xC), cleared after every status exec.
static u32* workWord(soModuleAccesser* acc, int offset) {
    return reinterpret_cast<u32*>(reinterpret_cast<char*>(&acc->getWorkManageModule()) + offset);
}

// Run-time type information of the module classes (unnamed objects of the unit's data) for the observer setup.
extern "C" {
extern char lbl_27_data_BBD0[];
extern char lbl_27_data_3AD0[];
extern char lbl_27_data_BA78[];
extern char lbl_27_data_3F80[];
void* __dynamic_cast(void* object, int offset, void* source, void* target, int flag);
}
#define g_wnRttiAttackModule lbl_27_data_BBD0
#define g_wnRttiNullAttackModule lbl_27_data_3AD0
#define g_wnRttiHitModule lbl_27_data_BA78
#define g_wnRttiNullHitModule lbl_27_data_3F80
static void* wnDynamicCast(void* module, void* source, void* target) {
    return __dynamic_cast(module, 0, source, target, 0);
}

// The manage id is a virtual of the soEventManager base inside the event manage module (vtable at +8).
static s16 getEventManageId(soModuleAccesser* acc) {
    return static_cast<soEventManager&>(acc->getEventManageModule()).getManageId();
}

// Number of final-smash articles that are alive (the screen is dimmed while one exists).
static int g_wnFinalArticleCount;

// Local copy of the activation flag byte (HYPOTHESIS: the same bit grouping as the descriptor's per-bit view).
struct wnActivateFlagBits {
    u8 bit7 : 1;
    u8 bit6 : 1;
    u8 bit5 : 1;
    u8 bit4 : 1;
    u8 bit3 : 1;
    u8 bit2 : 1;
    u8 bit1 : 1;
    u8 bit0 : 1;
};

Weapon::Weapon(const char* name, s32 subKind, const void* data, s32 articleId, const void* kindInfo,
               soModuleAccesser* moduleAccesser, int deadAreaBits) :
    StageObject(name, 12, 9, 5, 1, NULL),
    soStatusEventObserver(-1, -1),
    soCollisionAttackEventObserver(-1, -1),
    soCollisionHitEventObserver(-1, -1),
    soArticle(-1, 0xB) {
    const s32* const* info = reinterpret_cast<const s32* const*>(kindInfo);
    m_reflectDirX = 0.0f;
    m_reflectDirY = 0.0f;
    m_deadAreaMask = deadAreaBits & 3;
    m_inhaleCount = 0;
    m_inhaleStartPos.m_x = 0.0f;
    m_inhaleStartPos.m_y = 0.0f;
    m_inhaleStartPos.m_z = 0.0f;
    m_subKind = subKind;
    m_data = data;
    m_articleId = articleId;
    unkC4 = (*info)[0];
    unkC8 = (*info)[1];
    m_moduleAccesser = moduleAccesser;
}

Weapon::~Weapon() {
}

void Weapon::activate(wnActivateDesc* desc) {
    unk30 = 9;
    StageObject::activate(desc->lr, 0.0f, &desc->pos, true);
    bool hasOwner = false;
    if (m_moduleAccesser->getLinkModule().isNull() == false && desc->founderTaskId != -1 && desc->flagBit7 == 1) {
        hasOwner = m_moduleAccesser->getLinkModule().link(3, desc->founderTaskId);
    }
    int attachKind = 2;
    if (desc->resourceId != 0xFFFF) {
        attachKind = 1;
    }
    wnActivateFlagBits flags = *reinterpret_cast<const wnActivateFlagBits*>(&desc->flags);
    attachInstance(desc->resourceId, desc->unk8, desc->unkC, desc->unk10, desc->unk14, desc->unk18, desc->unk1C, attachKind,
                   desc->unk38, desc->unk44, flags.bit5, flags.bit4, flags.bit2, flags.bit1, desc->unk48, flags.bit0, desc->unk3C);
    m_moduleAccesser->getWorkManageModule().setInt(desc->founderTaskId, Weapon_Work_Int_Activate_Founder_Id);
    m_moduleAccesser->getWorkManageModule().offFlag(Weapon_Work_Flag_No_Dead_Area_Remove);
    int teamOwner = desc->founderTaskId;
    if (desc->team != -1) {
        m_moduleAccesser->getTeamModule().setTeam(desc->team, true);
        m_moduleAccesser->getTeamModule().setHitTeam(desc->team);
        m_moduleAccesser->getTeamModule().setTeamOwnerId(teamOwner);
        teamOwner = m_moduleAccesser->getTeamModule().getTeamOwnerId();
    }
    if (hasOwner == true) {
        if (desc->flagBit6 == 1) {
            constraint(-1, -1, -1);
        } else {
            if (desc->flagBit3 == 1) {
                m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Reference_Parent_Scale, true);
                float linkScale = m_moduleAccesser->getLinkModule().getLinkScale();
                m_moduleAccesser->getPostureModule().setOwnerScale(linkScale);
                m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Reference_Parent_Scale, false);
            }
            if (m_moduleAccesser->getGroundModule().isNull() != true) {
                if (m_moduleAccesser->getLinkModule().isValidParentShape(3) == true) {
                    Vec2f center;
                    Vec2f::copy(center, m_moduleAccesser->getLinkModule().getParentShapeCenterPos(3));
                    m_moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
                } else {
                    u32 ownerTask = m_moduleAccesser->getTeamModule().getTeamOwnerId();
                    Vec2f center;
                    if (ownerTask != -1 && m_moduleAccesser->getLinkModule().getParentShapeCenterPosTaskId(ownerTask, &center) == true) {
                        m_moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
                    }
                }
            }
        }
        float powerMul = m_moduleAccesser->getLinkModule().getOwnerAttackPowerMulArticle(3);
        m_moduleAccesser->getCollisionAttackModule().setPowerMul2nd(powerMul);
        if (m_moduleAccesser->getLinkModule().isParentGlow(3) == true) {
            int linkIndex = 3;
            if (m_moduleAccesser->getLinkModule().link(4, teamOwner) == true) {
                linkIndex = 4;
            }
            wnGlowModuleIface* glowModule = static_cast<wnGlowModuleIface*>(wnModuleAt(m_moduleAccesser, 0xC8));
            int glow = m_moduleAccesser->getLinkModule().getParentWeaponGlowAttack(linkIndex);
            glowModule->setAttack(desc->unk40 + glow);
            m_moduleAccesser->getLinkModule().setAttribute(linkIndex, soLinkConnection::Attribute_Reference_Parent_Power_Up, true);
            if (desc->flag4DBit7 == 1) {
                static_cast<wnGlowModuleIface*>(wnModuleAt(m_moduleAccesser, 0xC8))->setWhole(linkIndex);
            }
        }
        if (m_moduleAccesser->getModelModule().isEnvMap() == true) {
            m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(0xB), true);
            if (m_moduleAccesser->getLinkModule().isParentEnableEnvMap(3) == true) {
                m_moduleAccesser->getModelModule().switchEnvMap(true);
            }
        }
        if (m_moduleAccesser->getLinkModule().isParentEffectSyncScale(3) == true) {
            m_moduleAccesser->getEffectModule().setSyncScale(true);
        }
        if (desc->flag4DBit6 == 1 && m_moduleAccesser->getLinkModule().isParentEnableSubColor(3) == true) {
            m_moduleAccesser->getColorBlendModule().setSubColor(m_moduleAccesser->getLinkModule().getParentSubColor(3), 1);
        }
    }
    updateNodeSRT();
    m_inhaleCount = 0;
    unkA4Bit7 = 0;
    m_isInhaled = 0;
    m_keepOnReflectLimit = 0;
    initialize();
    if (wnInfo::isFinal(m_subKind) == true) {
        g_wnFinalArticleCount++;
    }
    m_moduleAccesser->getWorkManageModule().setInt(desc->life, Weapon_Work_Int_Life);
    m_moduleAccesser->getWorkManageModule().setInt(desc->life, Weapon_Work_Int_Init_Life);
    m_moduleAccesser->getWorkManageModule().setFloat(g_GameGlobal->getGameFrame(), Weapon_Work_Float_Active_Global_Frame);
    m_moduleAccesser->getShadowModule().setIgnoreMode(1);
    m_moduleAccesser->getGroundModule().setEventToGround(false, 0);
    m_moduleAccesser->getCollisionHitModule().setCheckCatch(false, 0);
}

void Weapon::deactivate(bool treatAsRemoved) {
    soArticle::setNotifyArticleEventEject(-1, m_moduleAccesser);
    onDeactivate();
    m_moduleAccesser->getStatusModule().changeStatusForce(-1, m_moduleAccesser);
    if (m_moduleAccesser->getWorkManageModule().isFlag(Weapon_Work_Flag_Keep_Model) == false) {
        m_moduleAccesser->getModelModule().destructInstance();
    }
    if (isActive() == true && wnInfo::isFinal(m_subKind) == true) {
        g_wnFinalArticleCount--;
    }
    StageObject::deactivate(treatAsRemoved);
}

// HYPOTHESIS: the part of the article data block that initialize reads (names from how the modules use the pointers).
struct wnWeaponInitData {
    u8 unk0[0x28];
    void* groundSetup;                 // +0x28: ground shape data handed to the ground module
    soSet<soAreaData>* areaSet;        // +0x2C: areas added to the area module
};

void Weapon::remove() {
    deactivate(false);
}

void Weapon::deactivateArticle() {
    deactivate(true);
}

void Weapon::initialize() {
    if (m_moduleAccesser->getStatusModule().isNull() == false) {
        soStatusEventObserver::addObserverSubS16(getEventManageId(m_moduleAccesser), &static_cast<soStatusEventObserver&>(*this), -1);
    }
    if (m_moduleAccesser->getAnimCmdModule().getInterpreterNum() > 0) {
        soAnimCmdEventObserver::addObserverSubS16(getEventManageId(m_moduleAccesser), &static_cast<soAnimCmdEventObserver&>(*this), -1);
    }
    soLinkEventObserver::addObserverSubS16(getEventManageId(m_moduleAccesser), &static_cast<soLinkEventObserver&>(*this), -1);
    if (wnDynamicCast(wnModuleAt(m_moduleAccesser, 0x1C), g_wnRttiAttackModule, g_wnRttiNullAttackModule) == NULL) {
        soCollisionAttackEventObserver::addObserverSubS16(getEventManageId(m_moduleAccesser), &static_cast<soCollisionAttackEventObserver&>(*this), -1);
    }
    if (wnDynamicCast(wnModuleAt(m_moduleAccesser, 0x20), g_wnRttiHitModule, g_wnRttiNullHitModule) == NULL) {
        soCollisionHitEventObserver::addObserverSubS16(getEventManageId(m_moduleAccesser), &static_cast<soCollisionHitEventObserver&>(*this), -1);
    }
    soEventPresenter<soArticleEventObserver>::setManageId(getEventManageId(m_moduleAccesser));
    m_moduleAccesser->getCameraModule().setup();
    if (static_cast<const wnWeaponInitData*>(m_data)->groundSetup != NULL) {
        if (m_moduleAccesser->getGroundModule().isNull() == true) {
            OSReport("name: %s \n", m_taskName);
        }
        m_moduleAccesser->getGroundModule().setup(static_cast<const wnWeaponInitData*>(m_data)->groundSetup, m_taskId, true);
    } else if (m_moduleAccesser->getGroundModule().isNull() == false) {
        OSReport("name: %s \n", m_taskName);
    }
    if (static_cast<const wnWeaponInitData*>(m_data)->areaSet != NULL) {
        m_moduleAccesser->getAreaModule().addArea(static_cast<const wnWeaponInitData*>(m_data)->areaSet);
    }
    m_moduleAccesser->getGenerateArticleManageModule().setAutoRecycle(true);
}

void Weapon::updatePosture(bool applyKinetic) {
    soModuleAccesser* acc = m_moduleAccesser;
    Vec3f pos = acc->getPostureModule().getPos();
    if (applyKinetic == true) {
        Vec3f speed = acc->getKineticModule().getSumSpeed3f(soKineticEnergy::AttributeFlag(0xFFF7));
        float ownerScale = acc->getPostureModule().getOwnerScale();
        speed = speed * ownerScale;
        Vec3fAdd(&pos, &pos, &speed);
        Vec3f rotation = acc->getKineticModule().getSumRotation(soKineticEnergy::AttributeFlag(0xFFFF));
        float slowRate = acc->getSlowModule().getRate();
        Vec3fScale(&rotation, &rotation, slowRate);
        Vec3f next;
        const Vec3f& current = acc->getPostureModule().getRot(0);
        Vec3fAdd(&next, &current, &rotation);
        acc->getPostureModule().setRot(&next, 0);
    }
    Vec3f offset = acc->getKineticModule().getSumSpeed3f(soKineticEnergy::AttributeFlag(8));
    Vec3fAdd(&pos, &pos, &offset);
    acc->getPostureModule().setPos(&pos);
}

void Weapon::processUpdate() {
    StageObject::processUpdate();
    unkA4Bit7 = 0;
}

void Weapon::processFixPosition() {
    StageObject::processFixPosition();
}

void Weapon::processHit() {
    StageObject::processHit();
}

void Weapon::processFixCamera() {
    StageObject::processFixCamera();
    if (soSlow::getInstance()->isAdjust() == true) {
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        if (isConstraint() == false && m_moduleAccesser->getWorkManageModule().isFlag(Weapon_Work_Flag_No_Dead_Area_Remove) == false &&
            m_deadAreaMask != -1) {
            int area = soGroundUtil::checkDeadArea(&pos, 0.0f, 0.0f);
            if (area != -1 && ((1 << area) & m_deadAreaMask) == 0) {
                soArticle::setNotifyArticleEventRemove(m_moduleAccesser);
                if (isActive() == true) {
                    deactivate(false);
                }
            }
        }
        if (pos.m_y >= 4096.0f) {
            soArticle::setNotifyArticleEventRemove(m_moduleAccesser);
            if (isActive() == true) {
                deactivate(false);
            }
        }
    }
}

void Weapon::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    static_cast<wnPhysicsModuleIface*>(wnModuleAt(m_moduleAccesser, 0x9C))->changeStatus();
    updateNodeSRT();
}

bool Weapon::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3) {
    char group = acmd->getGroup();
    if (isObserv(group) == false) {
        return false;
    }
    if (acmd->getType() <= 4 || acmd->getType() >= 11) {
        return StageObject::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
    }
    switch (acmd->getType()) {
    case 5:
        deactivate(false);
        return true;
    case 6:
        break;
    case 7:
        constraint(-1, -1, -1);
        return true;
    case 8:
        soArticle::setNotifyArticleEventRemove(m_moduleAccesser);
        if (isActive() == true) {
            deactivate(false);
        }
        return true;
    case 9:
        soArticle::setNotifyArticleEventEject(-1, m_moduleAccesser);
        return true;
    case 10:
        unkA4 |= 0x10;
        return true;
    }
    return false;
}

void Weapon::changeMotion(int motionKind, bool restart) {
    float frame;
    if (restart == true) {
        frame = m_moduleAccesser->getMotionModule().getFrame();
    } else {
        frame = 0.0f;
    }
    soMotionChangeParam param;
    param.m_kind = motionKind;
    param.m_frame = frame;
    param.m_rate = 1.0f;
    param._12 = 0;
    param._13 = 0;
    param._14 = restart;
    param._15 = 0;
    m_moduleAccesser->getMotionModule().changeMotionRequest(&param);
}

void Weapon::setFrame(float frame) {
    m_moduleAccesser->getMotionModule().setFrame(frame);
}

void Weapon::setRate(float rate) {
    m_moduleAccesser->getMotionModule().setRate(rate);
}

void Weapon::changeStatus(int statusKind) {
    m_moduleAccesser->getStatusModule().changeStatusRequest(statusKind, m_moduleAccesser);
}

void Weapon::setVisibilityWhole(u8 visibility) {
    m_moduleAccesser->getVisibilityModule().setWhole(visibility);
}

void Weapon::setVisibility(int index, u8 visibility) {
    m_moduleAccesser->getVisibilityModule().set(index, visibility);
}

void Weapon::set2nd(u8 enable) {
    static_cast<wnPhysicsModuleIface*>(wnModuleAt(m_moduleAccesser, 0x9C))->set2nd(enable);
}

void Weapon::setSituationKind(SituationKind kind) {
    m_moduleAccesser->getSituationModule().setKind(kind, false);
}

bool Weapon::isConstraint() {
    return m_moduleAccesser->getLinkModule().isModelConstraint();
}

s16 Weapon::getArticleEventManageId() {
    return getEventManageId(m_moduleAccesser);
}

void Weapon::linkOwner(int index, int taskId) {
    m_moduleAccesser->getLinkModule().link(index, taskId);
}

void Weapon::unlinkOwner(int index) {
    if (m_moduleAccesser->getLinkModule().isLink(index) == true) {
        m_moduleAccesser->getLinkModule().unlink(index);
    }
}

void Weapon::have(int ownerTaskId, int node, int unused) {
    constraint(ownerTaskId, node, -1);
}

// Keeps the article attached to the node of its owner that the build data names (-1 takes the value from the data).
void Weapon::constraint(int taskId, int unk1, int unk2) {
    u32 task = taskId;
    u32 nodeB = unk2;
    u32 nodeA = unk1;
    if (task == 0xFFFFFFFF) {
        task = m_moduleAccesser->getWorkManageModule().getInt(Weapon_Work_Int_Activate_Founder_Id);
        if (task == 0xFFFFFFFF) {
            return;
        }
    }
    StageObject* parent = m_moduleAccesser->getLinkModule().getParent(3);
    if (parent == NULL || task != parent->m_taskId) {
        return;
    }
    if (nodeA == 0xFFFFFFFF) {
        nodeA = reinterpret_cast<const s32*>(m_data)[2];
        if (nodeA == 0xFFFFFFFF) {
            return;
        }
    }
    if (nodeB == 0xFFFFFFFF) {
        nodeB = reinterpret_cast<const s32*>(m_data)[1];
        if (nodeB == 0xFFFFFFFF) {
            return;
        }
    }
    m_moduleAccesser->getLinkModule().setModelConstraintPosOrt(3, nodeB, nodeA, 3, true);
    m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(2), true);
    m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(8), true);
    m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(3), true);
    m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(0xE), true);
}

void Weapon::removeConstraint() {
    float scale = m_moduleAccesser->getLinkModule().getConstraintModelScale();
    float ownerScale = m_moduleAccesser->getLinkModule().getLinkScale();
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    m_moduleAccesser->getModelModule().setScale(scale);
    m_moduleAccesser->getPostureModule().setOwnerScale(ownerScale);
    m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(2), false);
}

// Raw views of soCollisionLog bytes that the shared header leaves unnamed.
float vec2fAngle(Vec2f*, Vec2f*); // native helper in the main binary

#define LOG_U8(log, offset) (*(reinterpret_cast<u8*>(log) + (offset)))

void Weapon::notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(static_cast<gfTask::Category>(LOG_U8(collisionLog, 0x22)), collisionLog->m_taskId);
    switch (LOG_U8(collisionLog, 0x21)) {
    case 2:
        unkA4Bit7 = 0;
        if (m_moduleAccesser->getSituationModule().getKind() == 2) {
            soCollisionAttackModule& attack = m_moduleAccesser->getCollisionAttackModule();
            soCollisionAttackData* data = attack.getData(LOG_U8(collisionLog, 0x25), false);
            if ((*reinterpret_cast<u32*>(reinterpret_cast<char*>(data) + 0x3C) >> 31) == 0) {
                soCollisionShieldModule& shield = *soExternalValueAccesser::getCollisionShieldModule(dynamic_cast<StageObject*>(task));
                if (shield.isHop(LOG_U8(collisionLog, 0x23)) == true) {
                    float hopAngle = shield.getHopAngle(LOG_U8(collisionLog, 0x23)) * 0.017453292f;
                    float hopCos = cos(hopAngle);
                    m_reflectDirX = hopCos * shield.getLR(0);
                    m_reflectDirY = sin(hopAngle);
                    unkA4Bit7 = 1;
                } else if (shield.isNoHop(LOG_U8(collisionLog, 0x23)) == false) {
                    Vec3f center = shield.getCenterPos(LOG_U8(collisionLog, 0x24), LOG_U8(collisionLog, 0x23));
                    Vec3f logPos;
                    logPos = collisionLog->m_pos;
                    Vec2f dir(logPos.m_x - center.m_x, logPos.m_y - center.m_y);
                    float length = dir.length();
                    if (length > 0.00001f) {
                        dir.m_x *= 1.0f / length;
                        dir.m_y *= 1.0f / length;
                        Vec2f speed;
                        Vec2f::copy(speed, attack.getSpeed());
                        if (vec2fAngle(&dir, &speed) * 57.29578f < 135.0f) {
                            m_reflectDirX = dir.m_x;
                            m_reflectDirY = dir.m_y;
                            unkA4Bit7 = 1;
                        }
                    }
                }
            }
        }
        break;
    case 1:
        break;
    case 0:
        if (task != NULL) {
            float reboundX = 0.0f;
            float reboundY = 0.0f;
            if (moduleAccesser->getCollisionAttackModule().checkRebound(power, moduleAccesser, dynamic_cast<StageObject*>(task),
                                                                        collisionLog, &reboundX, &reboundY)) {
            }
        }
        break;
    }
}

bool Weapon::reflect() {
    float speedMul = m_moduleAccesser->getReflectModule().getSpeedMul();
    Vec3f mul(-speedMul, 1.0f, 1.0f);
    soKineticEnergy::AttributeFlag attribute(1);
    m_moduleAccesser->getKineticModule().mulSpeed(&mul, attribute);
    m_moduleAccesser->getKineticModule().mulAccel(&mul, attribute);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    float lifeMul = m_moduleAccesser->getReflectModule().getLifeMul();
    int initLife = work.getInt(Weapon_Work_Int_Init_Life);
    m_moduleAccesser->getWorkManageModule().setInt((int)((float)initLife * lifeMul), Weapon_Work_Int_Life);
    return false;
}

bool Weapon::hop() {
    Vec3f dir(m_reflectDirX, m_reflectDirY, 0.0f);
    m_moduleAccesser->getKineticModule().reflectSpeed(&dir, soKineticEnergy::AttributeFlag(1));
    return false;
}

void Weapon::reflectSettingTeam() {
    m_moduleAccesser->getPostureModule().reverseLr();
    m_moduleAccesser->getPostureModule().updateRotYLr();
    int team = m_moduleAccesser->getReflectModule().getTeam();
    m_moduleAccesser->getTeamModule().setTeam(team, true);
    m_moduleAccesser->getTeamModule().setHitTeam(team);
    m_moduleAccesser->getTeamModule().setTeamOwnerId(m_moduleAccesser->getReflectModule().getTaskId());
}

bool Weapon::notifyEventCollisionAttackCheck(u32 flags) {
    if ((flags & 0x13) != 0) {
        deactivate(false);
        return true;
    }
    if ((flags & 4) != 0) {
        if (unkA4Bit7 == 1) {
            return hop();
        }
        deactivate(false);
        return true;
    }
    if ((flags & 8) != 0 && m_moduleAccesser->getReflectModule().isReflect() == true) {
        if (!m_keepOnReflectLimit && m_moduleAccesser->getReflectModule().isCountMax() == true) {
            soArticle::setNotifyArticleEventRemove(m_moduleAccesser);
            if (isActive() == true) {
                deactivate(false);
            }
            return true;
        }
        reflectSettingTeam();
        return reflect();
    }
    return false;
}

void Weapon::notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int unk, soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog) {
}

void Weapon::notifyEventCollisionHit2nd(float posX, float collisionLr, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, bool unk) {
}

void Weapon::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0x835:
        deactivate(false);
        break;
    }
    StageObject::notifyEventLink(eventInfo, moduleAccesser, sender, unk4);
}

// Per article kind: how the article reacts when its owner is deactivated.
int Weapon::getOwnerDeactiveTreatType() {
    return wnInfo::getOwnerDeactiveTreatType(m_subKind);
}

void Weapon::setHitStop(float power, float minFrame, int allowSdi) {
    int frames = soStopModuleImpl::calcHitStopFrame(power, 0.3846154f, 5.0f, minFrame, 30.0f);
    m_moduleAccesser->getStopModule().setHitStopFrame(frames, allowSdi);
}

void Weapon::setLogAttackInfo(soLogAttackInfo* logAttackInfo) {
    m_moduleAccesser->getWorkManageModule().setInt(logAttackInfo->_0, Weapon_Work_Int_Log_Attack_Info0);
    m_moduleAccesser->getWorkManageModule().setInt(logAttackInfo->_4, Weapon_Work_Int_Log_Attack_Info1);
}

soLogAttackInfo Weapon::getLogAttackInfo() {
    int info1 = m_moduleAccesser->getWorkManageModule().getInt(Weapon_Work_Int_Log_Attack_Info1);
    int info0 = m_moduleAccesser->getWorkManageModule().getInt(Weapon_Work_Int_Log_Attack_Info0);
    return soLogAttackInfo(info0, info1);
}

void Weapon::updateLogAttackInfo() {
    soArticle::updateLogAttackInfo(soExternalValueAccesser::getTeamOwnerId(this));
}

void Weapon::setPosForce(Vec3f* pos) {
    m_moduleAccesser->getPostureModule().setPos(pos);
}

bool Weapon::isCanInhaled() {
    soModuleAccesser* acc = m_moduleAccesser;
    bool result;
    if (acc->getLinkModule().isModelConstraint() != false) {
        result = false;
    } else {
        result = wnInfo::isInhaled(m_subKind);
    }
    return result;
}

bool Weapon::isInhaled() {
    return m_isInhaled;
}

Vec3f Weapon::startInhaled() {
    m_isInhaled = true;
    m_inhaleCount = 0;
    m_inhaleStartPos = m_moduleAccesser->getPostureModule().getPos();
    soModuleAccesser* acc = m_moduleAccesser;
    acc->getStopModule().setSpecialStop(1);
    acc->getAreaModule().setWhole(0);
    acc->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(0), 0);
    acc->getGroundModule().setGrCollisionMode(false);
    acc->getSituationModule().setKind(Situation_Air, false);
    acc->getCollisionHitModule().setWhole(3, 0);
    acc->getCollisionAttackModule().sleep(true);
    acc->getCollisionShieldModule().clearAll();
    acc->getCollisionReflectorModule().clearAll();
    static_cast<wnCollisionCatchModuleIface*>(wnModuleAt(acc, 0x30))->clearAll();
    acc->getCollisionSearchModule().clearAll();
    acc->getWorkManageModule().setInt(Weapon_Work_Int_Init_Life, Weapon_Work_Int_Life);
    return m_inhaleStartPos;
}

void Weapon::updateInhaled() {
    soModuleAccesser* acc = m_moduleAccesser;
    m_inhaleCount++;
    acc->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(0), 0);
    acc->getWorkManageModule().setInt(Weapon_Work_Int_Init_Life, Weapon_Work_Int_Life);
}

void Weapon::endInhaled() {
    m_inhaleCount = 0;
    m_isInhaled = false;
    deactivate(false);
}

Vec3f* Weapon::getInhaledStartPos() {
    return &m_inhaleStartPos;
}

int Weapon::getInhaledCount() {
    return m_inhaleCount;
}

bool Weapon::isCanEat() {
    return false;
}

float Weapon::getActiveGlobalFrame() {
    if (isActive() == false) {
        return -1.0f;
    }
    return m_moduleAccesser->getWorkManageModule().getFloat(Weapon_Work_Float_Active_Global_Frame);
}

u32 Weapon::getFounderTaskId() {
    u32 founder = m_moduleAccesser->getWorkManageModule().getInt(Weapon_Work_Int_Activate_Founder_Id);
    if (founder == 0xFFFFFFFF) {
        return 0xFFFFFFFF;
    }
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Weapon, founder);
    Weapon* owner = dynamic_cast<Weapon*>(task);
    founder = (owner == NULL) ? founder : owner->getFounderTaskId();
    return founder;
}

void Weapon::setAttackPowerMulPattern(float pattern) {
    m_moduleAccesser->getParamCustomizeModule().setAttackPowerMulPattern(pattern);
}

void Weapon::setGroundShapeSafePosWithAncestor() {
    if (m_moduleAccesser->getGroundModule().isNull() != true) {
        if (m_moduleAccesser->getLinkModule().isValidParentShape(3) == true) {
            Vec2f center;
            Vec2f::copy(center, m_moduleAccesser->getLinkModule().getParentShapeCenterPos(3));
            m_moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
        } else {
            u32 ownerTask = m_moduleAccesser->getTeamModule().getTeamOwnerId();
            Vec2f center;
            if (ownerTask != 0xFFFFFFFF && m_moduleAccesser->getLinkModule().getParentShapeCenterPosTaskId(ownerTask, &center) == true) {
                m_moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
            }
        }
    }
}
