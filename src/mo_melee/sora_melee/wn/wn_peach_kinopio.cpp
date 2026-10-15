#include <wn/peach/wn_peach_kinopio.h>
#include <wn/wn_class_info_impl.h>

// Registers the Peach Kinopio article class (kind 93) in the class info table during static
// initialization.
wnClassInfoImpl<93, wnPeachKinopio, wnPeachKinopioData> g_wnPeachKinopioClassInfo;
