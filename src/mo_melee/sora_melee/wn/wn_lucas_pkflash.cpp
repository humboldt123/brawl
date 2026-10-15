#include <wn/lucas/wn_lucas_pkflash.h>
#include <wn/wn_class_info_impl.h>

// Registers the Lucas PK Flash article class (kind 77) in the class info table during static initialization.
wnClassInfoImpl<77, wnLucasPKFlash, wnLucasPKFlashData> g_wnLucasPKFlashClassInfo;
