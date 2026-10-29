#pragma once

// The event unit record of the fighter manager's event manage entity (also the first include of units whose event
// system headers come before ft/ft_manager.h).

#include <so/so_instance_unit.h>

class soEventUnit;
// MATCH-ONLY: the array of event unit records (the event manager entity) is built with out-of-line record
// constructors/destructors in the original (a call to __construct_array), and the pointer is left untouched.
template <>
class soInstanceUnit<soEventUnit*> {
public:
    soEventUnit* m_element;
    int m_id;

    soInstanceUnit();
    ~soInstanceUnit();
};

