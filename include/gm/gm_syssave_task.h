#pragma once

// Local shadow of the header of the shared headers (this directory comes first in the include paths): adds the task that
// writes the system save, which the main menu starts when an option or a name changed.

#include <gf/gf_system_callback.h>
#include <sr/sr_common.h>

class gmSysSave : public gfReturnStatusCallback {
public:
    static gmSysSave* create(Heaps::HeapType heap);
    virtual bool returnStatus() const;
};

// The task that saves the system data (names, options, records) to the NAND.
// HYPOTHESIS: the argument is the kind of save; the main menu always passes 0x2E.
class gmSysSaveTask {
public:
    static gmSysSaveTask* create(int kind);
};
