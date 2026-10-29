#include <so/so_value_accesser.h>
#include <yk/yk_value_accesser.h>

ykValueAccesser::ykValueAccesser() : soValueAccesser(3) { }

soParamAccesser* ykValueAccesser::getExtendParamAccesser() {
    return &g_soParamAccesserNull;
}

ykValueAccesser::~ykValueAccesser() { }

// HYPOTHESIS: local name of the yakumono value accesser instance.
ykValueAccesser g_ykValueAccesser;
