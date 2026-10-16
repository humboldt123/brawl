#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/pikachu/wn_pikachu_thunder_jolt.h>
#include <wn/wn_class_info_impl.h>

// Registers the Pikachu thunder jolt article (kind 9) in the class info table during static initialization.
wnClassInfoImpl<9, wnPikachuThunderJolt, wnPikachuThunderJoltData> g_wnPikachuThunderJoltClassInfo;
