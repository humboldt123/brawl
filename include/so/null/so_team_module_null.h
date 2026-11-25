#pragma once

#include <so/team/so_team.h>
#include <so/team/so_team_module.h>

class soTeamNull : public soTeam {
public:
    soTeamNull() : soTeam(true) { }
    virtual ~soTeamNull();
    virtual int getNo() const;
    virtual void setNo(int);
    virtual int getIndirectNo() const;
    virtual void setIndirectNo(int);
    virtual int get2nd() const;
    virtual void set2nd(int);
    virtual void reset();
};

class soTeamModuleNull : public soTeamModule {
public:
    virtual ~soTeamModuleNull();
    virtual soTeam* getTeam();
    virtual void setTeam(s32 team, bool notify);
    virtual soTeam* getHitTeam();
    virtual soTeam* getIndirectTeam();
    virtual void setHitTeam(s32 team);
    virtual void setTeamOwnerId(s32 taskId);
    virtual s32 getTeamOwnerId();
    virtual void set2nd(s32);
    virtual void setHit2nd(s32);
    virtual void activate();
    virtual void deactivate();
    virtual void reset();
    virtual void setEnableOwnerEvent(bool);
};
