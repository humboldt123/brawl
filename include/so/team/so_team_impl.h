#pragma once
#include <so/team/so_team.h>

// Shared weapon construction initializes both team views from the supplied
// number; the secondary number begins at -1. No fighter entry ID is stored.
// HYPOTHESIS: source qualifiers follow the recovered soTeam interface.
class soTeamImpl : public soTeam {
    int m_no;  // +0x08
    int m_2nd; // +0x0C
public:
    soTeamImpl(int no);
    virtual int getNo() const;
    virtual ~soTeamImpl() { }
    virtual void setNo(int no);
    virtual int getIndirectNo() const;
    virtual void setIndirectNo(int no);
    virtual int get2nd() const;
    virtual void set2nd(int no);
    virtual void reset();
};
static_assert(sizeof(soTeamImpl) == 0x10, "Shared weapon team extent");
