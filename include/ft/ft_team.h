#pragma once

#include <so/team/so_team.h>

// Member names follow upstream's ftTeam (BrawlHeaders so/team/so_team_module_impl.h, doldecomp/brawl #157), whose
// sora_melee unit src/mo_melee/sora_melee/ft/ft_team.cpp defines the out-of-line members.
class ftTeam : public soTeam {
protected:
    int m_entryId; // +0x08: constructor input used by reset's fighter lookup
    int m_teamNo;  // +0x0C: primary team number
    int m_2nd;     // +0x10: secondary team number
public:
    ftTeam(int entryId);
    // The native getNo implementation keeps the vtable in sora_melee.
    virtual int getNo() const;
#if defined(FT_TEAM_TYPED_INTERFACE) || !defined(FT_MODULE_BUILDER)
    // sora_melee's ft_team.cpp emits it next to the vtable.
    virtual ~ftTeam() { }
#else
    virtual ~ftTeam() __attribute__((never_inline));
#endif
    virtual void setNo(int);
#ifdef FT_MODULE_BUILDER
    virtual int getIndirectNo() const;
    virtual void setIndirectNo(int);
#else
    virtual int getIndirectNo() const { return getNo(); }
    virtual void setIndirectNo(int) { }
#endif
    virtual int get2nd() const;
    virtual void set2nd(int);
    virtual void reset();
};

class ftTeamIndirect : public ftTeam {
    int m_indirectTeamNo; // +0x14: -1 selects the primary team number
public:
    ftTeamIndirect(int entryId) : ftTeam(entryId), m_indirectTeamNo(-1) { }
    virtual int getNo() const;
#ifndef FT_MODULE_BUILDER
    // sora_melee's ft_team.cpp emits it next to the vtable.
    virtual ~ftTeamIndirect() { }
    virtual int getIndirectNo() const { return m_indirectTeamNo; }
    virtual void setIndirectNo(int num) { m_indirectTeamNo = num; }
#else
#ifdef FT_TEAM_TYPED_INTERFACE
    // MATCH-ONLY: retain this call while allowing the ftTeam base teardown
    // to inline into it, as in the native fighter REL.
    virtual ~ftTeamIndirect() __attribute__((never_inline));
#else
    virtual ~ftTeamIndirect();
#endif
    virtual int getIndirectNo() const;
    virtual void setIndirectNo(int);
#endif
};
static_assert(sizeof(ftTeam) == 0x14, "Team extent");
static_assert(sizeof(ftTeamIndirect) == 0x18, "Indirect team extent");
