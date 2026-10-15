#include <wn/zelda/wn_szerosuit_paralyzer.h>
#include <wn/wn_class_info_impl.h>

// Registers the Zero Suit paralyzer article class (kind 98) in the class info table during static initialization.
wnClassInfoImpl<98, wnSZerosuitParalyzer, wnSZerosuitParalyzerData> g_wnSZerosuitParalyzerClassInfo;
