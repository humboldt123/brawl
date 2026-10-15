#include <wn/lucario/wn_lucario_aura_ball.h>
#include <wn/wn_class_info_impl.h>

// Registers the Lucario aura ball article class (kind 99) in the class info table during static initialization.
wnClassInfoImpl<99, wnLucarioAuraBall, wnLucarioAuraBallData> g_wnLucarioAuraBallClassInfo;
