#pragma once

#include <so/team/so_team.h>

class ftTeam : public soTeam {
    int m_entryId; // +0x08: constructor input used by reset's fighter lookup
    int m_no;     // +0x0C: primary team number
    int m_2nd;    // +0x10: secondary team number
public:
    ftTeam(int entryId);
    // The native getNo implementation keeps the vtable in sora_melee.
    virtual int getNo() const;
#ifdef FT_TEAM_TYPED_INTERFACE
    virtual ~ftTeam() { }
#else
    virtual ~ftTeam() __attribute__((never_inline));
#endif
    virtual void setNo(int);
    virtual int getIndirectNo() const;
    virtual void setIndirectNo(int);
    virtual int get2nd() const;
    virtual void set2nd(int);
    virtual void reset();
};

class ftTeamIndirect : public ftTeam {
    int m_indirectNo; // +0x14: -1 selects the primary team number
public:
    ftTeamIndirect(int entryId) : ftTeam(entryId), m_indirectNo(-1) { }
    virtual int getNo() const;
#ifdef FT_TEAM_TYPED_INTERFACE
    // MATCH-ONLY: retain this call while allowing the ftTeam base teardown
    // to inline into it, as in the native fighter REL.
    virtual ~ftTeamIndirect() __attribute__((never_inline));
#else
    virtual ~ftTeamIndirect();
#endif
    virtual int getIndirectNo() const;
    virtual void setIndirectNo(int);
};
static_assert(sizeof(ftTeam) == 0x14, "Team extent");
static_assert(sizeof(ftTeamIndirect) == 0x18, "Indirect team extent");
