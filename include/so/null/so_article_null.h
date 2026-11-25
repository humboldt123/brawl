#pragma once

#define FT_MODULE_BUILDER
#define WN_TRANSITION_CTOR_INLINE
#define WN_MARIO_FIREBALL_UNIT
#include <so/resource/so_resource_id_accesser.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/turn/so_turn_module.h>
#include <em/em_switch_decide_module_impl.h>
#include <em/term/em_general_term_decide_module_impl.h>
#include <so/transition/so_transition_module_impl.h>
#undef FT_MODULE_BUILDER
#undef WN_TRANSITION_CTOR_INLINE
#undef WN_MARIO_FIREBALL_UNIT
#include <so/article/so_article.h>
#include <so/controller/so_controller_impl.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/resource/so_resource_id_accesser.h>
#include <so/turn/so_turn_module.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/kinetic/so_kinetic_energy_null.h>
#include <em/em_switch_decide_module_impl.h>
#include <em/term/em_general_term_decide_module_impl.h>




class soKineticModuleNull : public soNullable {
public:
    virtual void activate();
    virtual void deactivate();
    virtual void updateEnergy(soModuleAccesser* moduleAccesser, soKineticEnergy::AttributeFlag);
    virtual int addEnergy(soKineticEnergy* kineticEnergy, int, soKineticEnergy::AttributeFlag, short);
    virtual soKineticEnergy* getEnergy(int index);
    virtual bool isContain(int index);
    virtual void clearSpeedAll();
    virtual void suspendEnergyAll();
    virtual void resumeEnergyAll();
    virtual void unableEnergyAll();
    virtual void eraseEnergy(int index);
    virtual void eraseEnergyAll();
    virtual Vec2f getSumSpeed(soKineticEnergy::AttributeFlag);
    virtual Vec3f getSumSpeed3f(soKineticEnergy::AttributeFlag);
    virtual Vec3f getSumRotation(soKineticEnergy::AttributeFlag);
    virtual void changeKinetic(int kineticType, soModuleAccesser* moduleAccesser);
    virtual int getKineticType();
    virtual void setConsiderGroundFriction(bool, soKineticEnergy::AttributeFlag);
    virtual void mulSpeed(Vec3f* speed, soKineticEnergy::AttributeFlag);
    virtual void mulAccel(Vec3f* accel, soKineticEnergy::AttributeFlag);
    virtual void reflectSpeed(Vec3f* speed, soKineticEnergy::AttributeFlag);
    virtual void reflectAccel(Vec3f* accel, soKineticEnergy::AttributeFlag);
    virtual void addSpeed(Vec3f* speed, soModuleAccesser* moduleAccesser);
    virtual void addSpeedOutside(soKineticEnergy::OutsideType, Vec3f* speed);
};
#include <so/status/so_status_module_impl.h>
#include <so/anim/so_anim_cmd_module_impl.h>


class soArticleNull : public soArticle {
public:
    virtual ~soArticleNull();
    virtual void remove();
    virtual int getArticleId();
    virtual s16 getArticleEventManageId();
    virtual int getTaskId();
    virtual void changeMotion(int motionKind, bool);
    virtual bool isActiveArticle();
    virtual void deactivateArticle();
    virtual void have(int, int, int);
    virtual void linkOwner(int, int);
    virtual void unlinkOwner(int);
    virtual bool isSyncOwnerStatus();
    virtual void setSyncOwnerStatus(bool);
    virtual void updateLogAttackInfo();
};


extern soResourceIdAccesserNull g_soResourceIdAccesserNull;


extern soResourceModuleNull g_soResourceModuleNull;

inline soTurnModule::~soTurnModule() { }

class soTurnModuleNull : public soTurnModule {
public:
    virtual ~soTurnModuleNull();
    virtual void activate();
    virtual void set(const soTurnData* data, bool changeLr, bool isExtern, float lr);
    virtual void end();
    virtual void update(bool active);
    virtual bool isTurn();
    virtual bool isExtern();
    virtual float getRY();
    virtual float getRYLr();
    virtual unsigned int getEndFrame();
    virtual void setOmitIntermediate(bool omit);
};

class soSwitchDecideModuleNull : public soNullable {
public:
    virtual int decideSwitch(int kind, soArrayContractibleTable<const acCmdArgConv>* args, soModuleAccesser* acc);
    virtual ~soSwitchDecideModuleNull();
};

class soGeneralTermDecideModuleNull : public soNullable {
public:
    virtual bool checkEstablish(soGeneralTerm* term, soModuleAccesser* moduleAccesser, u32* returnWord,
                                soGeneralTermCache* cache);
    virtual ~soGeneralTermDecideModuleNull();
};

class soStatusModuleNull : public soNullable {
public:
    virtual void activate(soModuleAccesser* moduleAccesser);
    virtual void deactivate(soModuleAccesser* moduleAccesser);
    virtual bool changeStatusRequest(int status, soModuleAccesser* moduleAccesser);
    virtual void processFixPosition(soModuleAccesser* moduleAccesser, bool);
    virtual void begin();
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execMapCorrection(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execFixCamera(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
    virtual void checkAttack(soModuleAccesser* moduleAccesser, void*, float);
    virtual bool onChangeLr(soModuleAccesser* moduleAccesser, float, float);
    virtual void leaveStop(soModuleAccesser* moduleAccesser, int, bool);
    virtual int getStatusKind();
    virtual bool isCollisionAttackOccer();
    virtual bool isCollisionAttackTarget(u32* collisionAttackCategory);
    virtual void startWatchChange();
    virtual bool isChanged();
    virtual bool checkTransition(soModuleAccesser* moduleAccesser);
    virtual void enableTransitionTermGroup(int groupID);
    virtual void unableTransitionTermGroup(int groupID);
    virtual bool isEnableTransitionTermGroup(int groupID);
    virtual void enableTransitionTermAll(int);
    virtual void clearTransitionTermAll(int);
    virtual soTransitionInfo* getLastStatusTransitionInfo();
    virtual void addRangeUniqProc(soStatusUniqProcess** uniqProcs, u32 numUniqProcs);
    virtual char* getStatusName();
    virtual char* getStatusName(int);
    virtual u32 getStatusGroundCorrect(int);
    virtual int getPrevStatusKind(u32);
    virtual void connectStatusDataList(void*);
    virtual void changeStatusForce(int status, soModuleAccesser* moduleAccesser);
    virtual void unableTransitionTerm(int, int);
    virtual void setUniqProc(u32 index, soStatusUniqProcess* uniqProc);
    virtual ~soStatusModuleNull();
    virtual void changeStatus(int status, soModuleAccesser* moduleAccesser);
};

class soAnimCmdModuleNull : public soNullable {
public:
    virtual int registInterpreter(const soAnimCmdControlUnit& unit, const soAttributeFlag& attr);
    virtual void interpretCmd(soModuleAccesser* moduleAccesser, const soAttributeFlag& attr, float frame);
    virtual void interpretCmdAdjust();
    virtual void interpretCmd(float frame, int group, soModuleAccesser* moduleAccesser);
    virtual void resetInterpreter();
    virtual void resetInterpreter(float frame, int group, soModuleAccesser* moduleAccesser, u32 unk);
    virtual void resetInterpreter(int group, void* animCmdTable, soModuleAccesser* moduleAccesser);
    virtual void resetInterpreter(const soAttributeFlag& attr, void* animCmdTable, soModuleAccesser* moduleAccesser, int unk1, int unk2);
    virtual void resetInterpreter(int group);
    virtual bool isInterpreterAnimCmd(int group);
    virtual void skipNextMotionGroup();
    virtual bool isAnimCmdEnd();
    virtual float getAnimCmdFrame();
    virtual void getCmdAddressPack();
    virtual void setAddressPackList();
    virtual int getInterpreterNum();
    virtual ~soAnimCmdModuleNull();
    virtual void activate(soModuleAccesser*);
    virtual void deactivate(soModuleAccesser*);
};
