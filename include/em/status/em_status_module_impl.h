#pragma once

// The status module of the enemies: soStatusModuleImpl that also runs the enemy's AI from the status transition check,
// with an AI update priority window (mo_enemy/sora_enemy/status/em_status_module_impl.cpp).

#include <StaticAssert.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soGeneralWorkSimple;
class soModuleAccesser;

class emStatusModuleImpl : public soStatusModuleImpl {
public:
    // The priority window handed to emAIModuleImpl::update before the base status transition check. The default
    // (-50, 250) also selects the extra checks of checkTransition. HYPOTHESIS: names (a lower and an upper bound).
    int m_aiUpdatePriMin; // 0xAC
    int m_aiUpdatePriMax; // 0xB0

    // HYPOTHESIS: the three trailing values mirror soStatusModuleImpl's preCheck data, status kind count and flag. The
    // last one is a byte (the constructor reads it with lbz from its stack word), so it is u8 here, not int.
    emStatusModuleImpl(soModuleAccesser* acc, soArrayContractibleTable<const soStatusData>* statusTable, void* uniqProcs,
                       soGeneralWorkSimple* statusWork, soTransitionModule* transition, void* changeRequests, void* preCheckData,
                       int unk1, u8 unk2);
    virtual ~emStatusModuleImpl();
    virtual bool checkTransition(soModuleAccesser* moduleAccesser);

    // Enemy::processGameProc raises the window to (-350, -250) around the status transition check of a dead area hit,
    // then puts it back.
    void setAIUpdatePri(int min, int max);
    void initAIUpdatePri();
};
static_assert(sizeof(emStatusModuleImpl) == 0xB4, "Class is wrong size!");
