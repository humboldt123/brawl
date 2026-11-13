#include <st/st_class_info.h>
#include <types.h>

// One class info per stage kind (0 .. BattleFieldS); every stage module registers its own.
static stClassInfo* g_stClassInfoTable[0x42];
static bool g_stClassInfoTableInit;

stClassInfo::stClassInfo() {
    if (!g_stClassInfoTableInit) {
        for (int i = 0; i < 0x42; i++) {
            g_stClassInfoTable[i] = NULL;
        }
        g_stClassInfoTableInit = true;
    }
}

stClassInfo::~stClassInfo() { }

stClassInfo* stClassInfo::getClassInfo(Stages::srStageKind kind) {
    return g_stClassInfoTable[kind];
}

void stClassInfo::setClassInfo(Stages::srStageKind kind, stClassInfo* info) {
    g_stClassInfoTable[kind] = info;
}
