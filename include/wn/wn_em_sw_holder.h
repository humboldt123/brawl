#pragma once

// wnemSwHolder: the small (0x28-byte) enemy article the Primid keeps six of next to its wnemSimple shots. Its unit is not
// reconstructed yet; only the size, the soArticle base at offset 0 and the entry points the owners call are known.
// HYPOTHESIS: "Sw" = sword/swing; the boomerang Primid is the only variant that treats these specially.

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/article/so_article.h>
#include <types.h>

struct soArticleConstructInfo;
class wnemDummy_Param;
class wnemSimple;
struct wnemSimple_Data;

// HYPOTHESIS: the third base at 0x14: the constructor stores its vptr last, and the vtable object ends with its table (a
// destructor thunk that subtracts 0x14, then isActive). The real class and its name are unknown.
class wnemSwHolderActivity {
public:
    virtual ~wnemSwHolderActivity() { }
};

class wnemSwHolder : public soArticle, public wnemSwHolderActivity {
    bool m_isActive;        // 0x18, set while the holder owns an activated wnemSimple
    wnemSimple* m_weapon;   // 0x1c, the activated wnemSimple (its holder pointer at 0x2208 points back)
    u8 unk20[0x4];
    s32 m_articleId;        // 0x24, the first constructor argument

public:
    // HYPOTHESIS: parameter types, as for wnemSimple.
    wnemSwHolder(s32 articleId, s16 manageId, const soArticleConstructInfo& info, const wnemDummy_Param* param);
    virtual ~wnemSwHolder();
    virtual void remove();
    virtual int getArticleId();
    virtual s16 getArticleEventManageId();
    virtual int getTaskId();
    virtual bool isActiveArticle();
    virtual void deactivateArticle();
    virtual void have(int, int, int);
    virtual void linkOwner(int, int);
    virtual void unlinkOwner(int);
    virtual bool isSyncOwnerStatus();
    virtual void setSyncOwnerStatus(bool);
    virtual bool isActive();
    virtual void setAttackPowerMulPattern(float);
    virtual float getActiveGlobalFrame();
    virtual void intrudeLogAttackInfo();
    virtual void updateLogAttackInfo();
    virtual soLogAttackInfo getLogAttackInfo();
    virtual void setLogAttackInfo(soLogAttackInfo*);
    virtual u32 getFounderTaskId();
    virtual int getOwnerDeactiveTreatType();
    virtual void deactivateDescendantForce();
    virtual void setSituationKind(SituationKind);
    virtual void action(int);
    virtual void set2nd(u8);
    virtual void setVisibility(int, u8);
    virtual void setVisibilityWhole(u8);
    virtual void changeStatus(int);
    virtual void setRate(float);
    virtual void setFrame(float);
    virtual bool isConstraint(bool);
    // Launches the article from the given position.
    // HYPOTHESIS: parameter roles, from what the Porky passes (compare fn_41_3091C, wnemSimple's activation). The facing
    // comes before the team, as in wnemSimple's activation: emWeaponUtil::activeSimpleWeaponHolder evaluates the team
    // before the facing (right to left).
    void active(Vec3f* pos, float lr, int team, int, int founderTaskId, int resourceId, wnemSimple_Data* work, float speed,
                bool flag, u32 life);
    void deactivate();
};
static_assert(sizeof(wnemSwHolder) == 0x28, "Class is wrong size!");
