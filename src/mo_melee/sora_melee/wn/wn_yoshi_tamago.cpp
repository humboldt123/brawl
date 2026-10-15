#include <wn/yoshi/wn_yoshi_tamago.h>
#include <wn/wn_class_info_impl.h>

// Registers the Yoshi egg article class (kind 60) in the class info table during static initialization.
wnClassInfoImpl<60, wnYoshiTamago, wnYoshiTamagoData> g_wnYoshiTamagoClassInfo;
