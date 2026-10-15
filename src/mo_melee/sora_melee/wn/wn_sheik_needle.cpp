#include <wn/sheik/wn_sheik_needle.h>
#include <wn/wn_class_info_impl.h>

// Registers the Sheik needle article class (kind 68) in the class info table during static initialization.
wnClassInfoImpl<68, wnSheikNeedle, wnSheikNeedleData> g_wnSheikNeedleClassInfo;
