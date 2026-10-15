#include <wn/pit/wn_pit_bow_arrow.h>
#include <wn/wn_class_info_impl.h>

// Registers the Pit bow arrow article class (kind 91) in the class info table during static initialization.
wnClassInfoImpl<91, wnPitBowArrow, wnPitBowArrowData> g_wnPitBowArrowClassInfo;
