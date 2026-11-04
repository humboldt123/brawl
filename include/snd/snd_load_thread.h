#pragma once

#include <gf/gf_thread.h>
#include <revolution/OS/OSMutex.h>
#include <snd/snd_id.h>

class sndLoadThread : public gfThread {
    OSMutex unk340;
    OSCond unk358;
    OSCond unk360;
    struct {
        u32 unk0;
        int unk4;
        SndGroupID unk8;
        int unkC;
    } unk368[149], unkCB8[1];
    u32 unkCC8;
    u32 unkCCC;
    bool unkCD0;

public:
    // Inline constructor expanded by sndSystem::sndSystem: 149 free request entries and an empty running list.
    sndLoadThread() : gfThread(this, 0x1F, 0x1000, Heaps::Sound) {
        OSInitMutex(&unk340);
        OSInitCond(&unk358);
        OSInitCond(&unk360);
        for (int i = 0; i < 149; i++) {
            unk368[i].unk0 = 0;
            unk368[i].unk8 = (SndGroupID)-1;
            unk368[i].unkC = -1;
        }
        unkCB8[0].unk0 = 0;
        unkCB8[0].unk8 = (SndGroupID)-1;
        unkCB8[0].unkC = -1;
        unkCC8 = 0;
        unkCCC = 0;
        unkCD0 = true;
    }
    void run();
    void addFreeGroup(int, int);
    bool isFinish();
    bool isFinish(SndGroupID);
    bool isFinishFree();
};
