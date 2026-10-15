#include <wn/poke/wn_poke_fushigisou_seed.h>
#include <wn/wn_class_info_impl.h>

// Registers the Pokemon seed article class (kind 74) in the class info table during static initialization.
wnClassInfoImpl<74, wnPokeFushigisouSeed, wnPokeFushigisouSeedData> g_wnPokeFushigisouSeedClassInfo;
