#pragma once

#include <so/so_null.h>

// Native RTTI and both concrete team vtables establish this interface.
// HYPOTHESIS: source integer types and const qualifiers; slots and 32-bit
// argument/return widths are verified.
class soTeam : public soNullable {
public:
#ifdef FT_TEAM_TYPED_INTERFACE
    virtual ~soTeam();
#else
    virtual ~soTeam() { }
#endif
    virtual int getNo() const = 0;
    virtual void setNo(int) = 0;
    virtual int getIndirectNo() const = 0;
    virtual void setIndirectNo(int) = 0;
    virtual int get2nd() const = 0;
    virtual void set2nd(int) = 0;
    virtual void reset() = 0;
};
static_assert(sizeof(soTeam) == 8, "Team interface extent");
