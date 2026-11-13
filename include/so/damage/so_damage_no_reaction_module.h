#pragma once

#include <StaticAssert.h>
#include <types.h>

// NOTE: shadows the BrawlHeaders copy to add the member functions (names from the map).
// One no-reaction rule: its mode and the two limits the mode compares against.
struct soDamageNoReactionStatus {
    int m_mode; // -1 off; 1 always; 2 and 3 compare the reaction or the power with the limit (see checkNoReaction)
    float m_reaction;
    float m_powerMax;
};

class soDamageNoReactionModule {
public:
    int m_mode;
    soDamageNoReactionStatus m_status[2]; // [0] the status's rule, [1] the second rule
    bool m_isModeAlways;
    bool m_isModePerfect;
    char _30[2];

public:
    soDamageNoReactionModule(int mode);
    void set(float unk1, float unk2, bool is2nd, int mode);
    void resetModeStatus();
    void setMode(int mode);
    void resetMode();
    void reset();
    bool checkNoReaction(float reaction, float powerMax, bool isAbsolute);
};
static_assert(sizeof(soDamageNoReactionModule) == 32, "Class is wrong size!");