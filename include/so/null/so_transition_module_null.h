#pragma once

#include <so/transition/so_transition_module_impl.h>

class soTransitionModuleNull : public soTransitionModule {
public:
    virtual int checkEstablish(soModuleAccesser* accesser, u32* targetKindOut, int groupID, u16* attrMask, soGeneralTermCache* generalTermCache);
    virtual void enableTerm(int unitID, int groupID);
    virtual void unableTerm(int unitID, int groupID);
    virtual void enableTermAll(int groupID);
    virtual void unableTermAll(int groupID);
    virtual void enableTermGroup(int groupID);
    virtual void unableTermGroup(int groupID);
    virtual bool isEnableTermGroup(int groupID);
    virtual int addTerm(int groupID, soTransitionTerm* term, int unitID, u32 targetKind, u16* option);
    virtual void addGeneralTerm(int groupID, int unitID, soGeneralTerm* term);
    virtual void addGeneralTermLastTerm(int groupID, soGeneralTerm* term);
    virtual void clearTransitionTermAll(int groupID);
    virtual int notifyEventAnimCmd(int commandType, soArrayContractibleTable<acCmdArgConv> commandArgList, u8* option, soModuleAccesser* accesser);
    virtual soTransitionInfo* getLastTransitionInfo();
    virtual ~soTransitionModuleNull();
};
