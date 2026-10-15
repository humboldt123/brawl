#pragma once

#include <mt/mt_vector.h>
#include <so/link/so_link_event_presenter.h>
#include <types.h>

// HYPOTHESIS: the event the string sends to its owner's head to ask for its current motion (0x838): the head answers with its
// speed, acceleration and speed limit.
struct wnNessYoyoPullEvent : soLinkEventArgs {
    float unk8;
    Vec2f unkC;
    Vec2f unk14;
    Vec2f unk1C;
    wnNessYoyoPullEvent(float speed) : soLinkEventArgs(0x838), unk8(speed) { }
};

// HYPOTHESIS: the event the yo-yo head sends to freeze the string for a number of frames (0x83f).
struct wnNessYoyoHeadLinkEvent : soLinkEventArgs {
    int unk8;
    bool unkC;
    wnNessYoyoHeadLinkEvent() : soLinkEventArgs(0x83f) { }
};
