#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define FT_REL_LINK_EXTERN
#define WN_MARIO_FIREBALL_CTOR_OUTLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/luigi/wn_luigi_fireball.h>
#include <wn/wn_class_info_impl.h>

// Registers the Luigi fireball article (kind 149) in the class info table during static initialization.
wnClassInfoImpl<149, wnLuigiFireball, wnMarioFireballData> g_wnLuigiFireballClassInfo;
