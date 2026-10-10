#pragma once

#include <so/link/so_link_event_presenter.h>

// Link events Diddy Kong sends to the articles he owns (Peanut / Barrel / Gun states, ids 0x838/0x839).
struct ftDiddyLinkEvent : soLinkEventArgs {
    ftDiddyLinkEvent(int kind) : soLinkEventArgs(kind) { }
};
