#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

class ftLinkStatusUniqProcessSpecialRSlash : public soStatusUniqProcess {
public:
    virtual ~ftLinkStatusUniqProcessSpecialRSlash() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftLinkStatusUniqProcessSpecialRSlash::initStatus(soModuleAccesser*) { }

void ftLinkStatusUniqProcessSpecialRSlash::execStatus(soModuleAccesser*) { }

// Shared virtual defaults are emitted in their original function order (this is the first unit of the REL that needs them).
bool soStatusUniqProcess::checkTransitionPrecede(soModuleAccesser*, void*, int) { return true; }
void soStatusUniqProcess::leaveStop(soModuleAccesser*, int, bool) {}
bool soStatusUniqProcess::onChangeLr(soModuleAccesser*, float, float) { return false; }
void soStatusUniqProcess::checkAttack(soModuleAccesser*, void*, float) {}
bool soStatusUniqProcess::checkDamage(soModuleAccesser*, void*) { return false; }
void soStatusUniqProcess::execFixCamera(soModuleAccesser*) {}
void soStatusUniqProcess::execFixPos(soModuleAccesser*) {}
void soStatusUniqProcess::execFixPosCounter(soModuleAccesser*) {}
void soStatusUniqProcess::execMapCorrection(soModuleAccesser*) {}
void soStatusUniqProcess::execStop(soModuleAccesser*) {}
void soStatusUniqProcess::exitStatus(soModuleAccesser*, int) {}
void soStatusUniqProcess::execStatus(soModuleAccesser*) {}

ftLinkStatusUniqProcessSpecialRSlash g_ftLinkStatusUniqProcessSpecialRSlash;
