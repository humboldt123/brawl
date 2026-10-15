#include <wn/pikmin/wn_kirby_pikmin.h>
#include <wn/wn_class_info_impl.h>

// Registers the Kirby's Pikmin article class (kind 150) in the class info table during static initialization.
wnClassInfoImpl<150, wnKirbyPikmin, wnKirbyPikminData> g_wnKirbyPikminClassInfo;
