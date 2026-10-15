#include <wn/dummy/wn_dummy_fighter.h>
#include <wn/wn_class_info_impl.h>

// Registers the dummy fighter article class (kind 152) in the class info table during static initialization.
wnClassInfoImpl<152, wnDummyFighter, wnDummyFighterData> g_wnDummyFighterClassInfo;
