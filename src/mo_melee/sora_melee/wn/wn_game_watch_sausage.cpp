#include <wn/gamewatch/wn_game_watch_sausage.h>
#include <wn/wn_class_info_impl.h>

// Registers the Game & Watch sausage article class (kind 117) in the class info table during static initialization.
wnClassInfoImpl<117, wnGameWatchSausage, wnGameWatchSausageData> g_wnGameWatchSausageClassInfo;
