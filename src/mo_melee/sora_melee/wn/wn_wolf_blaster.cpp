#include <wn/wolf/wn_wolf_blaster.h>
#include <wn/wn_class_info_impl.h>

// Registers the Wolf Blaster article class (kind 38) in the class info table during static initialization.
wnClassInfoImpl<38, wnWolfBlaster, wnWolfBlasterData> g_wnWolfBlasterClassInfo;
