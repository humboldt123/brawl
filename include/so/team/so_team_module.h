#pragma once

#include <StaticAssert.h>
#include <so/so_null.h>
#include <types.h>

// HYPOTHESIS: source spelling of opaque team return types. The original
// getters return pointers to the embedded team objects, not team numbers.
class soTeam;
class soTeamModule : public soNullable {
public:
    virtual ~soTeamModule();
    virtual soTeam* getTeam() = 0;
    virtual void setTeam(s32 team, bool notify) = 0;
    virtual soTeam* getHitTeam() = 0;
    virtual soTeam* getIndirectTeam() = 0;
    virtual void setHitTeam(s32 team) = 0;
    virtual void setTeamOwnerId(s32 taskId) = 0;
    virtual s32 getTeamOwnerId() = 0;
    // HYPOTHESIS: uncalled second-team source argument types. Native paths
    // forward one integer register to the corresponding embedded team setter.
    virtual void set2nd(s32) = 0;
    virtual void setHit2nd(s32) = 0;
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual void reset() = 0;
    // HYPOTHESIS: original bool spelling; native storage is a byte.
    virtual void setEnableOwnerEvent(bool) = 0;
};
static_assert(sizeof(soTeamModule) == 8, "Team interface layout");
