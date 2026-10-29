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
struct wnemSimple_Data;

class wnemSwHolder : public soArticle {
    u8 m_unreconstructed[0x28 - sizeof(soArticle)];

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
    // Launches the article from the given position.
    // HYPOTHESIS: parameter roles, from what the Porky passes (compare fn_41_3091C, wnemSimple's activation). The facing
    // comes before the team, as in wnemSimple's activation: emWeaponUtil::activeSimpleWeaponHolder evaluates the team
    // before the facing (right to left).
    void active(Vec3f* pos, float lr, int team, int, int founderTaskId, int resourceId, wnemSimple_Data* work, float speed,
                bool flag, u32 life);
};
static_assert(sizeof(wnemSwHolder) == 0x28, "Class is wrong size!");
