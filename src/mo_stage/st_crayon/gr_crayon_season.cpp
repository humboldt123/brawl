#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>

#include <st_crayon/gr_crayon.h>
#include <st_crayon/gr_crayon_anim.h>

grCrayonSeason* grCrayonSeason::create(int mdlIndex, const char* taskName) {
    grCrayonSeason* ground = new (Heaps::StageInstance) grCrayonSeason(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grCrayonSeason::~grCrayonSeason() { }

void grCrayonSeason::update(float deltaFrame) {
    CRAYON_SEASON_UPDATE(deltaFrame)
}
