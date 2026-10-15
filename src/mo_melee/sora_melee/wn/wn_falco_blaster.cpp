#include <wn/falco/wn_falco_blaster.h>
#include <wn/wn_class_info_impl.h>

// Registers the Falco Blaster article class (kind 35) in the class info table during static initialization.
wnClassInfoImpl<35, wnFalcoBlaster, wnFalcoBlasterData> g_wnFalcoBlasterClassInfo;
