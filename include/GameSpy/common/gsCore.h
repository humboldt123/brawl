#ifndef GAMESPY_COMMON_GSCORE_H
#define GAMESPY_COMMON_GSCORE_H

// GameSpy SDK core task manager (gsCore.c). The functions are still unnamed
// in our symbols; the SDK names are noted next to the placeholders.
#include <types.h>
#include <revolution/OS/OSMutex.h>
#include <GameSpy/common/gsPlatform.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GSICORE_MAXTASKS 40

typedef OSMutex GSICriticalSection;

typedef void (*GSTaskExecuteFunc)(void* theTaskData);
typedef void (*GSTaskCallbackFunc)(void* theTaskData, int theResult);
typedef void (*GSTaskCancelFunc)(void* theTaskData);
typedef int (*GSTaskCleanupFunc)(void* theTaskData);
typedef int (*GSTaskThinkFunc)(void* theTaskData);

typedef struct GSTask {
    int mId;                            // 0x0
    u32 mTimeout;                       // 0x4
    u32 mStartTime;                     // 0x8
    int mAutoThink;                     // 0xC
    int mIsStarted;                     // 0x10
    int mIsRunning;                     // 0x14
    int mIsCanceled;                    // 0x18
    int mIsCallbackPending;             // 0x1C
    void* mTaskData;                    // 0x20
    GSTaskExecuteFunc mExecuteFunc;     // 0x24
    GSTaskCallbackFunc mCallbackFunc;   // 0x28
    GSTaskCancelFunc mCancelFunc;       // 0x2C
    GSTaskCleanupFunc mCleanupFunc;     // 0x30
    GSTaskThinkFunc mThinkFunc;         // 0x34
} GSTask;

typedef struct GSCoreMgr {
    u32 mRefCount;                          // 0x0
    int mIsStaticInitComplete;              // 0x4
    int mIsInitialized;                     // 0x8
    int mIsShuttingDown;                    // 0xC
    GSICriticalSection mQueueCrit;          // 0x10
    GSTask* mTaskArray[GSICORE_MAXTASKS];   // 0x28
} GSCoreMgr;

__static_assert(sizeof(GSTask) == 0x38, "GSTask");
__static_assert(sizeof(GSCoreMgr) == 0xC8, "GSCoreMgr");

// gsPlatform helpers
void fn_8035D248(GSICriticalSection* theCrit);       // gsiInitializeCriticalSection
void fn_8035D24C(GSICriticalSection* theCrit);       // gsiEnterCriticalSection
void fn_8035D250(GSICriticalSection* theCrit);       // gsiLeaveCriticalSection
void fn_8035D254(GSICriticalSection* theCrit);       // gsiDeleteCriticalSection

int fn_803FC9B8(void (*func)(void));                 // atexit
void fn_8037EA50(void);                              // ghttpStartup

void fn_8035E380(void);                              // gsiCoreAtExitShutdown
void fn_8035E390(void);                              // gsCoreInitialize
int fn_8035E848(void);                               // gsCoreIsShutdown
void fn_8035E87C(GSTask* task, u32 timeoutMs);       // gsiCoreExecuteTask
GSTask* fn_8035EA18(void);                           // gsiCoreCreateTask

#ifdef __cplusplus
}
#endif

#endif
