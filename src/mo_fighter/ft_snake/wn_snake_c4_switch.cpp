#define FT_SNAKE_ARTICLE_POOLS
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <types.h>
#include <ft/snake/ft_snake_builder.h>
#include <wn/wn_activate_desc.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

void wnSnakeC4Switch::activate(int founderTaskId, int etcResourceId, int team) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, etcResourceId, etcResourceId, etcResourceId, -1, -1, 0, 0, pos, 0.0f, team, 0, 2,
                        0x80, 0, 0x35F, 0);
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
}
