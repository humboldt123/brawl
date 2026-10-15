#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/mario/wn_mario_fireball.h>
#include <wn/wn_class_info_impl.h>

// Registers the Mario fireball article (kind 0) in the class info table during static initialization.
wnClassInfoImpl<0, wnMarioFireball, wnMarioFireballData> g_wnMarioFireballClassInfo;
