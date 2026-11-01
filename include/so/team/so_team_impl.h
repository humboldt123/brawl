#pragma once
#include <so/team/so_team.h>

// Shared weapon construction initializes both team views from the supplied
// number; the secondary number begins at -1. No fighter entry ID is stored.
// HYPOTHESIS: source qualifiers follow the recovered soTeam interface.
class soTeamImpl : public soTeam {
    int m_no;  // +0x08
    int m_2nd; // +0x0C
public:
#ifdef EM_ENEMY_BUILDER
    // The Enemy constructor expands it.
    soTeamImpl(int no) : soTeam(false), m_no(no), m_2nd(-1) { }
#else
    // BYTES: not inlined into the article builders (weak copy in the unit).
    soTeamImpl(int no) __attribute__((never_inline)) : soTeam(false), m_no(no), m_2nd(-1) { }
#endif
    virtual int getNo() const { return m_no; }
    virtual ~soTeamImpl() { }
    virtual void setNo(int no) { m_no = no; }
    virtual int getIndirectNo() const { return m_no; }
    virtual void setIndirectNo(int) { }
    virtual int get2nd() const { return m_2nd; }
    virtual void set2nd(int no) { m_2nd = no; }
    virtual void reset() { }
};
static_assert(sizeof(soTeamImpl) == 0x10, "Shared weapon team extent");
