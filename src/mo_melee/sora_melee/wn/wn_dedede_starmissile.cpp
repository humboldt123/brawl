#include <wn/dedede/wn_dedede_starmissile.h>
#include <wn/wn_class_info_impl.h>

// Registers the Dedede star missile article class (kind 53) in the class info table during static initialization.
wnClassInfoImpl<53, wnDededeStarmissile, wnDededeStarmissileData> g_wnDededeStarmissileClassInfo;
