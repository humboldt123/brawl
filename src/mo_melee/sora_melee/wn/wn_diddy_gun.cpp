#include <wn/diddy/wn_diddy_gun.h>
#include <wn/wn_class_info_impl.h>

// Registers the Diddy popgun article (kind 54) in the class info table during static initialization.
wnClassInfoImpl<54, wnDiddyGun, wnDiddyGunData> g_wnDiddyGunClassInfo;
