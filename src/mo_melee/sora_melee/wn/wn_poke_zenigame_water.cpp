#include <wn/poke/wn_poke_zenigame_water.h>
#include <wn/wn_class_info_impl.h>

// Registers the Pokemon water gun article class (kind 95) in the class info table during static initialization.
wnClassInfoImpl<95, wnPokeZenigameWater, wnPokeZenigameWaterData> g_wnPokeZenigameWaterClassInfo;
