#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#pragma pool_data off
#include <wn/wn_module_accesser_builder.h>
#include <wn/dedede/wn_dedede_jethammer.h>
#include <so/so_module_accesser.h>

wnDededeJetHammerStatusUniqProcess g_wnDededeJetHammerStatusUniqProcess;

// The Jet Hammer's effects: entering status 1 or 7 (the hammer is raised) starts two effects that follow the hammer, once.
// HYPOTHESIS: work flag 0x22000005 is set while the effects exist; ints 0x20000000/0x20000002 keep their handles.
void wnDededeJetHammerStatusUniqProcess::initStatus(soModuleAccesser* acc) {
    switch (acc->getStatusModule().getStatusKind()) {
    case 1:
    case 7:
        if (!acc->getWorkManageModule().isFlag(0x22000005)) {
            acc->getPostureModule().getLr();
            u32 handle = acc->getEffectModule().reqFollow(static_cast<EfID>(0x210003), 0x14, &Vec3f(0.0f, 0.0f, 0.0f),
                                                         &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 0, 0, -1);
            acc->getWorkManageModule().setInt(handle, 0x20000000);
            handle = acc->getEffectModule().reqFollow(static_cast<EfID>(0x21000C), 0xB, &Vec3f(0.0f, 0.0f, 0.0f),
                                                      &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 0, 0, -1);
            acc->getWorkManageModule().setInt(handle, 0x20000002);
            acc->getWorkManageModule().onFlag(0x22000005);
        }
        break;
    case 9:
        break;
    }
}

// Once flag 0x22000002 is set (and 0x22000006 is not) the hammer starts the second pair of effects: the first set is cut off.
void wnDededeJetHammerStatusUniqProcess::execStatus(soModuleAccesser* acc) {
    if (acc->getWorkManageModule().isFlag(0x22000002) && !acc->getWorkManageModule().isFlag(0x22000006)) {
        acc->getEffectModule().killAll(0, true, true);
        acc->getPostureModule().getLr();
        u32 handle = acc->getEffectModule().reqFollow(static_cast<EfID>(0x210004), 0x14, &Vec3f(0.0f, 0.0f, 0.0f),
                                                     &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 0, 0, -1);
        acc->getWorkManageModule().setInt(handle, 0x20000001);
        handle = acc->getEffectModule().reqFollow(static_cast<EfID>(0x21000D), 0xB, &Vec3f(0.0f, 0.0f, 0.0f),
                                                  &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 0, 0, -1);
        acc->getWorkManageModule().setInt(handle, 0x20000003);
        acc->getWorkManageModule().onFlag(0x22000006);
    }
}

void wnDededeJetHammerStatusUniqProcess::execStop(soModuleAccesser*) { }

// Leaving for any status other than 0-8 puts the effects out.
void wnDededeJetHammerStatusUniqProcess::exitStatus(soModuleAccesser* acc, int nextStatus) {
    switch (nextStatus) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        return;
    }
    acc->getEffectModule().killAll(0, true, true);
}
