#pragma once

// HYPOTHESIS: the parameter types of the controller-request methods (from the callers' and the
// melee overrides' register use) and the field names after the getters that return them.

#include <StaticAssert.h>
#include <st/operator/st_operator.h>
#include <types.h>

class stOperatorNetwork : public stOperator {
public:
    virtual void processBegin() { }
    virtual ~stOperatorNetwork() { }
    virtual void setPrepareLoadStageComplete() { }
    virtual bool isPreparedLoadStageEveryone() { return true; }
    virtual bool isSync() { return true; }
    virtual void startController() { }
    virtual void stopControllerAsync(int arg) { }
    virtual bool isStopControllerComplete() { return true; }
    virtual void stopControllerCompleteCallback(u8 result) { }
    virtual void abortControllerAsync(int arg) { }
    virtual bool isAbortControllerComplete() { return true; }
    virtual void abortControllerCompleteCallback(u8 aid, u8 result) { }
    virtual void setNullPadForNetworkOut(bool isNull) { }
    virtual bool isOnline() { return false; }
    virtual bool isError() { return false; }
    virtual int getErrorValue() { return 0; }
    virtual bool isNoContest() { return false; }
    virtual void checkNoContest() { }
};
static_assert(sizeof(stOperatorNetwork) == 0x48, "Class is wrong size!");

class stOperatorNetworkMelee : public stOperatorNetwork {
public:
    bool unk48;
    bool m_isOnline;     // 0x49
    bool unk4A;          // 0x4A HYPOTHESIS: a stop request is pending
    bool unk4B;          // 0x4B HYPOTHESIS: an abort request is pending
    bool m_isError;      // 0x4C
    bool m_isNoContest;  // 0x4D
    int m_errorValue;    // 0x50
    int unk54;
    int unk58;
    int unk5C;
    int unk60;
    int unk64;
    int unk68;
    int unk6C;
    int unk70;

    stOperatorNetworkMelee();
    virtual void processBegin();
    virtual ~stOperatorNetworkMelee();
    virtual void setPrepareLoadStageComplete();
    virtual bool isPreparedLoadStageEveryone();
    virtual bool isSync();
    virtual void startController();
    virtual void stopControllerAsync(int arg);
    virtual bool isStopControllerComplete();
    virtual void stopControllerCompleteCallback(u8 result) {
        if (m_isOnline && !unk4A) {
            unk5C = result;
            unk4A = true;
        }
    }
    virtual void abortControllerAsync(int arg);
    virtual bool isAbortControllerComplete();
    virtual void abortControllerCompleteCallback(u8 aid, u8 result);
    virtual void setNullPadForNetworkOut(bool isNull);
    virtual bool isOnline() { return m_isOnline; }
    virtual bool isError() { return m_isError; }
    virtual int getErrorValue() { return m_errorValue; }
    virtual bool isNoContest() { return m_isNoContest; }
    virtual void checkNoContest();
};
static_assert(sizeof(stOperatorNetworkMelee) == 0x74, "Class is wrong size!");
