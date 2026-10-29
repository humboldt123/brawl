#pragma once

#include <mt/mt_vector.h>
#include <snd/snd_id.h>
#include <so/sound/so_sound_3d_generator_accesser_impl.h>
#include <types.h>

namespace soSoundUtil {
    // HYPOTHESIS: parameter types from the register use and the soSoundModuleImpl callers.
    int playSE(soSound3dGeneratorAccesser* accesser, int index, SndID sndID, int groupId, int soundGroup);
    void stopSE(soSound3dGeneratorAccesser* accesser, int index, int unk, int groupId);
    void setPos(soSound3dGeneratorAccesser* accesser, Vec3f* pos, int index);
}
