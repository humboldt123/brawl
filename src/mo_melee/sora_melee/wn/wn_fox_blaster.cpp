#include <wn/fox/wn_fox_blaster.h>
#include <wn/wn_class_info_impl.h>

// Registers the Fox Blaster article class (kind 30) in the class info table during static initialization.
wnClassInfoImpl<30, wnFoxBlaster, wnFoxBlasterData> g_wnFoxBlasterClassInfo;
