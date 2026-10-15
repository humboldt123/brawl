#include <wn/ness/wn_ness_pkflash.h>
#include <wn/wn_class_info_impl.h>

// Registers the Ness PK Flash article class (kind 102) in the class info table during static initialization.
wnClassInfoImpl<102, wnNessPKFlash, wnNessPKFlashData> g_wnNessPKFlashClassInfo;
