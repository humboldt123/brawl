#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/purin/ft_purin_status_uniq_process.h>

// Rest has no additional per-frame C++ processing in these hooks.
void ftPurinStatusUniqProcessSpecialLw::initStatus(soModuleAccesser*) { }
void ftPurinStatusUniqProcessSpecialLw::execStatus(soModuleAccesser*) { }
void ftPurinStatusUniqProcessSpecialLw::exitStatus(soModuleAccesser*, int) { }

ftPurinStatusUniqProcessSpecialLw g_ftPurinStatusUniqProcessSpecialLw;
