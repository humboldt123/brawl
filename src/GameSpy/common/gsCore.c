#include <types.h>
#include <string.h>
#include <GameSpy/common/gsCore.h>

// HYPOTHESIS: the static core manager (gsiGetStaticCore in the SDK).
GSCoreMgr lbl_805348E0 = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own; they follow the GameSpy SDK's
// gsCore.c.

void fn_8035E380(void) {
    fn_8035D254(&lbl_805348E0.mQueueCrit);
}

void fn_8035E390(void) {
    GSCoreMgr* aCore = &lbl_805348E0;

    while (aCore->mIsShuttingDown) {
        fn_8035D774(100);
    }
    if (fn_8035D1B8(&aCore->mRefCount) == 1) {
        if (!aCore->mIsStaticInitComplete) {
            fn_8035D248(&aCore->mQueueCrit);
            fn_803FC9B8(fn_8035E380);
            aCore->mIsStaticInitComplete = 1;
        }
        fn_8035D24C(&aCore->mQueueCrit);
        fn_8035D250(&aCore->mQueueCrit);
        while (aCore->mIsShuttingDown) {
            fn_8035D774(100);
        }
        memset(aCore->mTaskArray, 0, sizeof(aCore->mTaskArray));
        fn_8037EA50();
        aCore->mIsInitialized = 1;
        return;
    }
    while (!aCore->mIsStaticInitComplete) {
        fn_8035D774(100);
    }
    fn_8035D24C(&aCore->mQueueCrit);
    fn_8035D250(&aCore->mQueueCrit);
    while (!aCore->mIsInitialized) {
        fn_8035D774(100);
    }
}

int fn_8035E848(void) {
    GSCoreMgr* aCore = &lbl_805348E0;

    int result;

    if (aCore->mIsShuttingDown) {
        return 1;
    }
    result = 0;
    if (aCore->mRefCount == 0) {
        result = 2;
    }
    return result;
}

void fn_8035E87C(GSTask* task, u32 timeoutMs) {
    GSCoreMgr* aCore = &lbl_805348E0;
    int aTaskSlot = -1;
    int i;

    task->mIsCallbackPending = 1;
    task->mIsStarted = 1;
    task->mIsRunning = 1;
    task->mTimeout = timeoutMs;
    task->mStartTime = fn_8035D728();
    if (task->mExecuteFunc) {
        task->mExecuteFunc(task->mTaskData);
    }
    fn_8035D24C(&aCore->mQueueCrit);
    for (i = 0; i < GSICORE_MAXTASKS; i++) {
        if (aCore->mTaskArray[i] == NULL) {
            aTaskSlot = i;
            break;
        }
    }
    aCore->mTaskArray[aTaskSlot] = task;
    fn_8035D250(&aCore->mQueueCrit);
}

GSTask* fn_8035EA18(void) {
    GSTask* aTask;

    aTask = fn_8035DFE8(sizeof(GSTask));
    if (aTask == NULL) {
        return NULL;
    }
    memset(aTask, 0, sizeof(GSTask));
    aTask->mAutoThink = 1;
    return aTask;
}
