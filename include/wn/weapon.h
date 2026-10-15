// Local BrawlHeaders shadow: additive nonvirtual Weapon helpers used by Super Sonic.
#pragma once

#include <StaticAssert.h>
#include <so/article/so_article.h>
#include <so/event/so_event_presenter.h>
#include <so/stageobject.h>
#include <so/status/so_status_event_presenter.h>
#include <mt/mt_vector.h>
#include <types.h>

struct wnActivateDesc; // HYPOTHESIS: original descriptor type spelling.

class Weapon : public StageObject, public soStatusEventObserver, public soCollisionAttackEventObserver, public soCollisionHitEventObserver, public soArticle {

    float m_reflectDirX; // +0x9C: direction in which a hop reflects the speed (valid while unkA4Bit7)
    float m_reflectDirY; // +0xA0
protected:
    // Native article collision and sync accessors load/store this byte at A4.
    // Bit 6 is the owner status sync flag; the other bit meanings are unresolved.
    union {
        u8 unkA4;
        struct {
            u8 unkA4Bit7 : 1; // a shield of the attacker asked for a hop this frame
            u8 m_isSyncOwnerStatus : 1;
            u8 m_isInhaled : 1;
            u8 m_keepOnReflectLimit : 1;
            s32 m_deadAreaMask : 2; // dead areas (bit mask) in which the article is not removed
            u8 : 2;
        };
    };
private:
    char unkA5[3];
    s32 m_inhaleCount;       // +0xA8
    Vec3f m_inhaleStartPos;  // +0xAC
    s32 m_subKind;      // +0xB8: the article kind of the build data
protected:
    const void* m_data; // +0xBC: the article data block of the build data
private:
    s32 m_articleId;    // +0xC0
    // The constructor copies the two words reached through its kind descriptor.
    s32 unkC4;
    s32 unkC8;

public:
    // HYPOTHESIS: parameter names; the stores establish their destinations.
    Weapon(const char* name, s32 subKind, const void* data, s32 articleId, const void* kindInfo,
           soModuleAccesser* moduleAccesser, int unkA4Bits);
    void activate(wnActivateDesc* desc);
    // Map-named helper; -1 is the sentinel for each unresolved constraint argument.
    void constraint(int taskId, int unk1, int unk2);
    void setHitStop(float power, float minFrame, int allowSdi);
    void removeConstraint();
    void attachInstance(int resourceId, int unk8, int unkC, int unk10, int unk14, int unk18, int unk1C, int kind, int unk38, int unk44, bool flagBit5, bool flagBit4, bool flagBit2, bool flagBit1, int unk48, bool flagBit0, int unk3C);
    void setGroundShapeSafePosWithAncestor();

    struct Instance {
        struct Work {
            enum Int {
                Int_Activate_Founder_Id = 0x10000000,

                Int_Init_Life = 0x10000003,
                Int_Life = 0x10000004,
            };
            enum Float {
                Float_Active_Global_Frame = 0x11000000,
            };
            enum Flag {
            };
        };
    };

    virtual void processUpdate();
    virtual void processFixPosition();
    virtual void processHit();
    virtual void processFixCamera();
    virtual ~Weapon();

    virtual void updatePosture(bool);
    virtual soKind soGetKind() { return StageObject_Weapon; }
    virtual int soGetSubKind() { return m_subKind; }
    virtual bool isObserv(char unk1) { return unk1 == 12; }
#ifdef WN_WEAPON_ANIMCMD_LONG
    // Match StageObject's long-index virtual in fighter translation units.
    // An int declaration adds a new slot and shifts subsequent Weapon methods.
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
#else
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
#endif
    virtual void notifyEventLink(soLinkEventArgs *eventInfo, soModuleAccesser* moduleAccesser, StageObject*, int unk4);

    virtual void remove();
    virtual void changeMotion(int motionKind, bool);
    virtual void setFrame(float);
    virtual void setRate(float);
    virtual void changeStatus(int statusKind);
    virtual void setVisibilityWhole(u8);
    virtual void setVisibility(int, u8);
    virtual void set2nd(u8);
    virtual void setSituationKind(SituationKind);
    virtual bool isConstraint();
    virtual void have(int, int, int);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual int getArticleId() { return m_articleId; }
    virtual s16 getArticleEventManageId();
    virtual int getTaskId() { return m_taskId; }
    virtual u32 getFounderTaskId();
    virtual void linkOwner(int, int);
    virtual void unlinkOwner(int);
    virtual bool isActiveArticle() { return isActive(); }
    virtual void deactivateArticle();
    virtual float getActiveGlobalFrame();
    virtual bool isSyncOwnerStatus() { return m_isSyncOwnerStatus; }
    virtual void setSyncOwnerStatus(bool sync) { m_isSyncOwnerStatus = sync; }
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventCollisionHit(float power, soCollisionAttackData*, u32 index, int, soModuleAccesser* moduleAccesser, soCollisionLog*);
    virtual void notifyEventCollisionHit2nd(float posX, float collisionLr, soCollisionAttackModule*, soCollisionLog*, u32 groupIndex, soModuleAccesser* moduleAccesser, bool);
    virtual int getOwnerDeactiveTreatType();
    virtual void initialize();
    virtual void onDeactivate() { }
    virtual bool reflect();
    virtual bool hop();
    virtual void reflectSettingTeam();
    virtual void deactivate(bool);
    virtual void setLogAttackInfo(soLogAttackInfo* logAttackInfo);
    virtual soLogAttackInfo getLogAttackInfo();
    virtual void updateLogAttackInfo();
    virtual void setAttackPowerMulPattern(float);
    virtual void setPosForce(Vec3f*);
    virtual bool isCanInhaled();
    virtual bool isInhaled();
    virtual Vec3f startInhaled();
    virtual void updateInhaled();
    virtual void endInhaled();
    virtual Vec3f* getInhaledStartPos();
    virtual int getInhaledCount();
    virtual bool isCanEat();
};
static_assert(sizeof(Weapon) == 0xCC, "Class is wrong size!");
