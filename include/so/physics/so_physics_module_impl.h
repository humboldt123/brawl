#pragma once

#ifdef SO_PHYSICS_MODULE_IMPL_FULL
#define SO_PHYSICS_MODULE_IMPL_DEFINED

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/so_array.h>
#include <so/so_null.h>
#include <so/status/so_status_event_presenter.h>
#include <types.h>

class acAnimCmd;
class ph2ndaryController;
class ph2ndary2Controller;
class soModuleAccesser;
class soPhysicsIKHandle;
class soStatusData;

// HYPOTHESIS: the parameters of the slots that so_physics_module_impl.cpp does not define yet.
class soPhysicsModule : private soNull, public soNullable {
public:
    virtual ~soPhysicsModule() { }
    virtual void constructController() = 0;
    virtual void destructController() = 0;
    virtual void deactivate() = 0;
    virtual void activate() = 0;
    virtual void begin() = 0;
    virtual void setUpMtxIK() = 0;
    virtual void setUpMtx2ndBoth() = 0;
    virtual bool isNcessarySetUpMtx2ndBoth() = 0;
    virtual void correct2ndBoth() = 0;
    virtual void setUpMtx2ndPreCollision() = 0;
    virtual void setUpMtx2nd() = 0;
    virtual void setUpVisibility() = 0;
    virtual bool is2nd() = 0;
    virtual void set2nd(int on) = 0;
    virtual void set2ndDefault() = 0;
    virtual void set2ndDefault(int on) = 0;
    virtual void set2ndGravity(float gravity) = 0;
    virtual void set2ndSleep(bool sleep) = 0;
    virtual void set2ndType(int type) = 0;
    virtual void set2ndStatus(int status) = 0;
    virtual void set2ndEndPos(const Vec3f* pos) = 0;
    virtual void set2ndPreCollisionNode(int nodeId) = 0;
    virtual int get2ndLineCount() = 0;
    virtual float get2ndLineLength(int lineId) = 0;
    virtual void set2ndLineLength(int lineId, float length) = 0;
    virtual void set2ndLineInertia(int lineId, float inertia) = 0;
    virtual void set2ndLineInertiaValue(int lineId, float inertia) = 0;
    virtual void set2ndLineLengthBothAll(float length) = 0;
    virtual int get2ndActiveNodeNum() = 0;
    virtual void set2ndActiveNodeNum(int num) = 0;
    virtual void set2ndRewind(int rewind) = 0;
    virtual void off2ndWind() = 0;
    virtual bool is2ndBoth() = 0;
    virtual bool is2ndBothReverse() = 0;
    virtual void set2ndBoth(int on) = 0;
    virtual void set2ndBothStatus(int status) = 0;
    virtual int get2ndBothSituation() = 0;
    virtual int get2ndBothNodeMax() = 0;
    virtual void set2ndBothSpeed(const Vec3f* speed, int lineId, u8 flag) = 0;
    virtual void set2ndBothRewind(int rewind) = 0;
    virtual void set2ndBothWaver(int waver) = 0;
    virtual void set2ndBothHitStat(int hitStat) = 0;
    virtual void set2ndBothReverseFlag(int flag) = 0;
    virtual void set2ndPower(float power) = 0;
    virtual u32 get2ndNodeId(int lineId) = 0;
    virtual void set2ndLinePos(const Vec3f* pos) = 0;
    virtual void set2ndPosSplit(int split) = 0;
    virtual bool is2ndHitCollision() = 0;
    virtual void set2ndWind(const Vec3f* wind) = 0;
    virtual void set2ndWeaponScale(float scale) = 0;
    virtual void set2ndCliffDir(float lr) = 0;
    virtual void sync2ndSavedPosition() = 0;
    virtual bool isIK(int ikId) = 0;
    virtual bool isIK() = 0;
    virtual void setIK(int ikId, bool on) = 0;
    virtual void setIKTargetPos(int ikId, const Vec3f* pos) = 0;
    virtual void setIKNormal(int ikId, const Vec3f* normal) = 0;
    virtual const char* getIKEndNodeName(int ikId, int index) = 0;
    virtual void setToeAngleLimit(int ikId, float limit) = 0;
};
static_assert(sizeof(soPhysicsModule) == 0xC, "Class is wrong size!");

class soPhysicsModuleImpl : public soPhysicsModule, public soStatusEventObserver, public soAnimCmdEventObserver {
public:
    soModuleAccesser* m_moduleAccesser;         // 0x24
    ph2ndaryController* m_2ndaryController;     // 0x28
    ph2ndary2Controller* m_2ndary2Controller;   // 0x2C
    u32 unk30;                                  // 0x30
    soArray<soPhysicsIKHandle>* m_ikHandles;    // 0x34
    void* m_ikData;                             // 0x38
    int m_2ndPreCollisionNode;                  // 0x3C
    int m_2ndDefault;                           // 0x40
    u8 unk44_80 : 1;                            // 0x44
    u8 m_2ndSleep : 1;
    u8 unk44_20 : 1;
    u8 unk44_10 : 1;
    u8 unk44_08 : 1;

    soPhysicsModuleImpl(soModuleAccesser* acc, void* ikData, soArray<soPhysicsIKHandle>* handles, int n);
    virtual ~soPhysicsModuleImpl();
    virtual void constructController();
    virtual void destructController();
    virtual void deactivate();
    virtual void activate();
    virtual void begin();
    virtual void setUpMtxIK();
    virtual void setUpMtx2ndBoth();
    virtual bool isNcessarySetUpMtx2ndBoth();
    virtual void correct2ndBoth();
    virtual void setUpMtx2ndPreCollision();
    virtual void setUpMtx2nd();
    virtual void setUpVisibility();
    virtual bool is2nd();
    virtual void set2nd(int on);
    virtual void set2ndDefault();
    virtual void set2ndDefault(int on);
    virtual void set2ndGravity(float gravity);
    virtual void set2ndSleep(bool sleep);
    virtual void set2ndType(int type);
    virtual void set2ndStatus(int status);
    virtual void set2ndEndPos(const Vec3f* pos);
    virtual void set2ndPreCollisionNode(int nodeId);
    virtual int get2ndLineCount();
    virtual float get2ndLineLength(int lineId);
    virtual void set2ndLineLength(int lineId, float length);
    virtual void set2ndLineInertia(int lineId, float inertia);
    virtual void set2ndLineInertiaValue(int lineId, float inertia);
    virtual void set2ndLineLengthBothAll(float length);
    virtual int get2ndActiveNodeNum();
    virtual void set2ndActiveNodeNum(int num);
    virtual void set2ndRewind(int rewind);
    virtual void off2ndWind();
    virtual bool is2ndBoth();
    virtual bool is2ndBothReverse();
    virtual void set2ndBoth(int on);
    virtual void set2ndBothStatus(int status);
    virtual int get2ndBothSituation();
    virtual int get2ndBothNodeMax();
    virtual void set2ndBothSpeed(const Vec3f* speed, int lineId, u8 flag);
    virtual void set2ndBothRewind(int rewind);
    virtual void set2ndBothWaver(int waver);
    virtual void set2ndBothHitStat(int hitStat);
    virtual void set2ndBothReverseFlag(int flag);
    virtual void set2ndPower(float power);
    virtual u32 get2ndNodeId(int lineId);
    virtual void set2ndLinePos(const Vec3f* pos);
    virtual void set2ndPosSplit(int split);
    virtual bool is2ndHitCollision();
    virtual void set2ndWind(const Vec3f* wind);
    virtual void set2ndWeaponScale(float scale);
    virtual void set2ndCliffDir(float lr);
    virtual void sync2ndSavedPosition();
    virtual bool isIK(int ikId);
    virtual bool isIK();
    virtual void setIK(int ikId, bool on);
    virtual void setIKTargetPos(int ikId, const Vec3f* pos);
    virtual void setIKNormal(int ikId, const Vec3f* normal);
    virtual const char* getIKEndNodeName(int ikId, int index);
    virtual void setToeAngleLimit(int ikId, float limit);

    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual bool isObserv(char group);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
};
static_assert(sizeof(soPhysicsModuleImpl) == 0x48, "Class is wrong size!");
#endif
