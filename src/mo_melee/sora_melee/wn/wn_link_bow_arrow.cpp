#include <wn/link/wn_link_bow_arrow.h>
#include <wn/wn_class_info_impl.h>

// Registers the Link bow arrow article class (kind 5) in the class info table during static initialization.
wnClassInfoImpl<5, wnLinkBowArrow, wnLinkBowArrowData> g_wnLinkBowArrowClassInfo;
