#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class gfArchive;
class muBootNandTask;
class muBootNetworkTask;

// The boot scene: loads the boot archive, then runs the save data check (NAND task) and the network
// notice check (network task) one after the other before handing over to the next scene.
class scBoot : public gfScene {
public:
    gfArchive* m_archive;              // 0x08
    muBootNandTask* m_nandTask;        // 0x0C
    muBootNetworkTask* m_networkTask;  // 0x10
    int m_step;                        // 0x14

    static scBoot* create();
    scBoot(const char* name) : gfScene(name), m_archive(NULL), m_nandTask(NULL), m_networkTask(NULL) { }
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scBoot) == 0x18, "Class is the wrong size!");
