#pragma once

// Local shadow: BrawlHeaders does not know nw4r::g3d::Camera / ScnRoot::GetCamera, which the stage uses to aim the camera of
// the scene at the search light (main.dol: GetCamera, SetPosture).

#include <mt/mt_vector.h>
#include <types.h>

namespace nw4r {
namespace g3d {

class Camera {
public:
    enum PostureType { POSTURE_LOOKAT, POSTURE_ROTATE, POSTURE_AIM };

    struct PostureInfo {
        PostureType tp;       // 0x00
        Vec3f cameraUp;       // 0x04
        Vec3f cameraTarget;   // 0x10
        Vec3f cameraRotate;   // 0x1C
        float cameraTwist;    // 0x28
    };

    void SetPosture(const PostureInfo& info);

private:
    void* m_data;
};

class ScnRoot {
public:
    Camera GetCamera(int index);
};

} // namespace g3d
} // namespace nw4r
