// Local BrawlHeaders shadow: adds the screen-handle array, constructor and destructor of soEffectScreen.
#pragma once

#include <ac/ac_anim_cmd_impl.h>
#include <so/so_module_accesser.h>
#include <StaticAssert.h>

class efScreenHandle;
template <class T> class soArray;

class soEffectScreen {
public:
    soArray<efScreenHandle>* m_handles;

    soEffectScreen(soArray<efScreenHandle>* handles);
    ~soEffectScreen();
    void notifyEventAnimCmd(const acAnimCmd* cmd, soModuleAccesser* accessor);
};
static_assert(sizeof(soEffectScreen) == 4, "Class is wrong size!");
