#include <wn/pikmin/wn_pikmin_pikmin_general_term_decide_module_impl.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/transition/so_transition_module_impl.h>
#include <types.h>
#include <wn/pikmin/wn_pikmin_pikmin_utility.h>

extern "C" {
// HYPOTHESIS: soGeneralTermDisideModuleImpl::checkEstablishSub and soGeneralTerm::getArg (sora_melee map),
// called by their REL names until they are recovered there.
bool fn_27_77A88(soGeneralTermDisideModuleImpl* module, int termId, soGeneralTerm* term,
                 soModuleAccesser* moduleAccesser, u32* returnWord, soGeneralTermCache* cache);
acCmdArg fn_27_778CC(soGeneralTerm* term, int index);
}

bool wnPikminPikminGeneralTermDecideModuleImpl::checkEstablishSub(int termId, soGeneralTerm* term,
                                                                  soModuleAccesser* moduleAccesser,
                                                                  u32* returnWord, soGeneralTermCache* cache) {
    if (termId <= 9999) {
        return fn_27_77A88(this, termId, term, moduleAccesser, returnWord, cache);
    }
    switch (termId) {
    case 10000:
        return checkEstablishSub(soIntToType<10000>(), term, moduleAccesser);
    case 10001:
        return checkEstablishSub(soIntToType<10001>(), term, moduleAccesser);
    case 10002:
        return checkEstablishSub(soIntToType<10002>(), term, moduleAccesser);
    case 10003:
        return checkEstablishSub(soIntToType<10003>(), term, moduleAccesser);
    case 10004:
        return checkEstablishSub(soIntToType<10004>(), term, moduleAccesser);
    }
    return true;
}

bool wnPikminPikminGeneralTermDecideModuleImpl::checkEstablishSub(const soIntToType<10000>&, soGeneralTerm* term,
                                                                  soModuleAccesser* moduleAccesser) {
    float dist = 0.0f; // MATCH-ONLY: the dead float initializer keeps the paired-single save of f31.
    acCmdArg arg = fn_27_778CC(term, 0);
    acCmdArg* p = &arg; // MATCH-ONLY: keeps the argument address in a saved register.
    if (p->getArgType() == 1) {
        dist = p->getFloatData();
    } else {
        dist = soValueAccesser::getValueFloat(moduleAccesser, p->getValueIndexData(), 0);
    }
    int mode = 3;
    if (term->m_animCmdTable.size() - 1 >= 2) {
        mode = fn_27_778CC(term, 1).getIntData();
    }
    return wnPikminPikminUtility::getTargetDist(moduleAccesser, mode) >= dist;
}

bool wnPikminPikminGeneralTermDecideModuleImpl::checkEstablishSub(const soIntToType<10001>&, soGeneralTerm* term,
                                                                  soModuleAccesser* moduleAccesser) {
    float dist = 0.0f; // MATCH-ONLY: the dead float initializer keeps the paired-single save of f31.
    acCmdArg arg = fn_27_778CC(term, 0);
    acCmdArg* p = &arg; // MATCH-ONLY: keeps the argument address in a saved register.
    if (p->getArgType() == 1) {
        dist = p->getFloatData();
    } else {
        dist = soValueAccesser::getValueFloat(moduleAccesser, p->getValueIndexData(), 0);
    }
    int mode = 3;
    if (term->m_animCmdTable.size() - 1 >= 2) {
        mode = fn_27_778CC(term, 1).getIntData();
    }
    return wnPikminPikminUtility::getOwnerDist(moduleAccesser, mode) >= dist;
}

bool wnPikminPikminGeneralTermDecideModuleImpl::checkEstablishSub(const soIntToType<10002>&, soGeneralTerm* term,
                                                                  soModuleAccesser* moduleAccesser) {
    Vec2f pos(moduleAccesser->getPostureModule().getPos().m_x, moduleAccesser->getPostureModule().getPos().m_y);
    Vec2f diff = Vec2f(moduleAccesser->getWorkManageModule().getFloat(0x11000004),
                       moduleAccesser->getWorkManageModule().getFloat(0x11000005)) - pos;
    return diff.m_x * moduleAccesser->getPostureModule().getLr() < 0.0f;
}

bool wnPikminPikminGeneralTermDecideModuleImpl::checkEstablishSub(const soIntToType<10003>&, soGeneralTerm* term,
                                                                  soModuleAccesser* moduleAccesser) {
    Vec2f pos(moduleAccesser->getPostureModule().getPos().m_x, moduleAccesser->getPostureModule().getPos().m_y);
    Vec2f diff = Vec2f(moduleAccesser->getWorkManageModule().getFloat(0x11000002),
                       moduleAccesser->getWorkManageModule().getFloat(0x11000003)) - pos;
    return diff.m_x * moduleAccesser->getPostureModule().getLr() < 0.0f;
}

bool wnPikminPikminGeneralTermDecideModuleImpl::checkEstablishSub(const soIntToType<10004>&, soGeneralTerm* term,
                                                                  soModuleAccesser* moduleAccesser) {
    u32 mask = fn_27_778CC(term, 0).getIntData();
    u32 flags = moduleAccesser->getWorkManageModule().getInt(0x10000011);
    return (flags & mask) != 0;
}

wnPikminPikminGeneralTermDecideModuleImpl lbl_113_bss_1DC;


