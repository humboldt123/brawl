#include <wn/samus/wn_samus_cshot.h>
#include <wn/wn_class_info_impl.h>

// Registers the Samus charge shot article class (kind 83) in the class info table during static initialization.
wnClassInfoImpl<83, wnSamusCShot, wnSamusCShotData> g_wnSamusCShotClassInfo;
