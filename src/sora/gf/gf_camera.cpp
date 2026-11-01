#define MT_VEC3F_SCALE_FRESH_REGS
#define MT_VEC3F_MUL_INLINE
#include <gf/gf_camera.h>
#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <mt/mt_matrix.h>
#include <nw4r/g3d/g3d_camera.h>
#include <revolution/GX.h>
#include <revolution/MTX.h>
#include <math.h>

// Existing matrix helpers, verified against mt_matrix.cpp. Their source declarations remain unresolved.
extern "C" {
void fn_8003E918(Matrix*, float);
void fn_8003E87C(Matrix*, float);
void fn_8003EA9C(Matrix*, float);
void fn_8003EBF4(Matrix*, float);
void fn_8003F03C(Matrix*, float, float, float);
void fn_8003F074(Matrix*, float, float, float);
void fn_8003ECA0(Matrix*, float, float, float);
void fn_8003E3BC(const Matrix*, const Vec3f*, Vec3f*);
void fn_8003E46C(const Matrix*, const void*, void*);
void fn_8003F898(const void*, void*);
}

// HYPOTHESIS: private gfVideo view; aspect correction is read after setting the video mode.
struct CameraVideoView { u8 unk0[0x44]; float aspectCorrection; };
extern CameraVideoView* lbl_805A0098;
extern "C" void fn_800384CC(CameraVideoView*, bool);
extern gfCameraManager* lbl_8059FFC0;

void gfCamera::init() {
    unkFC = 0;
    m_centerPos.m_x = 0.0f; m_centerPos.m_y = 0.0f; m_centerPos.m_z = 85.0f;
    m_targetPos.m_x = 0.0f; m_targetPos.m_y = 0.0f; m_targetPos.m_z = 0.0f;
    unkCC = 85.0f;
    m_up.m_x = 0.0f; m_up.m_y = 1.0f; m_up.m_z = 0.0f;
    m_0x90.m_x = 1.0f; m_0x90.m_y = 0.0f; m_0x90.m_z = 0.0f;
    m_0x9C.m_x = 0.0f; m_0x9C.m_y = 0.0f; m_0x9C.m_z = 0.0f;
    m_targetOffset.m_x = 0.0f; m_targetOffset.m_y = 0.0f; m_targetOffset.m_z = 0.0f;
    m_rotationOffset.m_x = 0.0f; m_rotationOffset.m_y = 0.0f; m_rotationOffset.m_z = 0.0f;
    m_rot.m_z = 0.0f;
    m_fovScale = 1.0f;
    unkD0 = 0.5235988f;
    gfCameraManager* manager = gfCameraManager::getManager();
    m_aspect = manager->m_aspectCorrection * manager->m_aspectRatio;
    m_fovY = unkD0;
    m_near = 1.0f; m_far = 60000.0f;
    m_transformFlag.m_mask = 0x43; unkFA.m_mask = 0;
    m_viewportX = 0.0f; m_viewportY = 0.0f;
    m_viewportWidth = 640.0f; m_viewportHeight = 480.0f;
    m_viewportNear = 0.0f; m_viewportFar = 0.99999f;
    m_scissorX = 0; m_scissorY = 0; m_scissorWidth = 640; m_scissorHeight = 480;
    m_projectionType = Projection_Perspective;
    m_orthoParam.m_top = 0.0f; m_orthoParam.m_left = 0.0f;
    m_orthoParam.m_bottom = 480.0f; m_orthoParam.m_right = 640.0f;
    calc();
}

static inline float cameraVectorLength(const Vec3f& v) {
    float squared = v.m_z * v.m_z + (v.m_x * v.m_x + v.m_y * v.m_y);
    double wide = __fabs(squared);
    float magnitude = wide;
    if (magnitude <= 1.17549435e-38f) return 0.0f;
    return squared * rsqrtf(squared);
}

// The transform mask names the supplied inputs. Derive the missing camera basis and angles.
void gfCamera::calc() {
    u16 flags = m_transformFlag.m_mask;
    if (!(flags & 4)) {
        if (flags & 0x20) {
            Vec3f forward(0.0f, 0.0f, -1.0f);
            Matrix rotation(true);
            fn_8003E918(&rotation, m_rot.m_y);
            fn_8003EA9C(&rotation, m_rot.m_x);
            fn_8003E3BC(&rotation, &forward, &m_direction);
            flags |= 4;
        } else if ((flags & 2) && (flags & 1)) {
            m_direction = m_targetPos - m_centerPos;
            m_direction.normalize();
            flags |= 4;
        }
    }
    if (!(flags & 2)) m_centerPos = m_targetPos - m_direction * unkCC;
    if (!(flags & 1)) m_targetPos = m_centerPos + m_direction * unkCC;
    if (!(flags & 0x80)) { Vec3f delta = m_targetPos - m_centerPos; unkCC = cameraVectorLength(delta); }
    if (!(flags & 0x20)) {
        float value = m_direction.m_y / cameraVectorLength(m_direction);
        if (value > 1.0f) value = 1.0f;
        if (value < -1.0f) value = -1.0f;
        m_rot.m_x = asin(value);
        m_rot.m_y = atan2(-m_direction.m_x, -m_direction.m_z);
    }
    if (!(flags & 0x10)) {
        if (flags & 8) {
            m_0x90.m_x = m_direction.m_y * m_up.m_z - m_direction.m_z * m_up.m_y;
            m_0x90.m_y = m_direction.m_z * m_up.m_x - m_direction.m_x * m_up.m_z;
            m_0x90.m_z = m_direction.m_x * m_up.m_y - m_direction.m_y * m_up.m_x;
            m_0x90.normalize();
        } else if (flags & 0x40) {
            Vec3f right(1.0f, 0.0f, 0.0f);
            Matrix rotation(true);
            fn_8003E918(&rotation, m_rot.m_y);
            fn_8003EA9C(&rotation, m_rot.m_x);
            fn_8003EBF4(&rotation, -m_rot.m_z);
            fn_8003E3BC(&rotation, &right, &m_0x90);
            m_0x90.normalize();
        }
    }
    if (!(flags & 8)) {
        m_up.m_x = m_0x90.m_y * m_direction.m_z - m_0x90.m_z * m_direction.m_y;
        m_up.m_y = m_0x90.m_z * m_direction.m_x - m_0x90.m_x * m_direction.m_z;
        m_up.m_z = m_0x90.m_x * m_direction.m_y - m_0x90.m_y * m_direction.m_x;
    }
    if (!(flags & 0x40)) {
        Vec3f right(1.0f, 0.0f, 0.0f);
        Matrix rotation(true);
        fn_8003E87C(&rotation, -m_rot.m_x);
        rotation.rotY(-m_rot.m_y);
        fn_8003E3BC(&rotation, &m_0x90, &right);
        m_rot.m_z = -float(atan2(right.m_y, right.m_x));
    }
    // FSelect retains the native saturation, including unordered inputs.
    float fov = nw4r::math::FSelect(unkD0 - 0.01f, unkD0, 0.01f);
    float scale = nw4r::math::FSelect(m_fovScale - 0.01f, m_fovScale, 0.01f);
    unkD0 = nw4r::math::FSelect(fov - 3.1315928f, 3.1315928f, fov);
    m_fovScale = nw4r::math::FSelect(scale - 10.0f, 10.0f, scale);
    m_fovY = float(atan(float(tan(unkD0 * 0.5f)) / m_fovScale)) * 2.0f;
    Vec3f translation(-m_centerPos.m_x, -m_centerPos.m_y, -m_centerPos.m_z);
    if (unkFC == 0) translation += m_0x9C * -1.0f;
    float pitch = m_rot.m_x + m_rotationOffset.m_x;
    float roll = m_rot.m_z;
    float yaw = m_rot.m_y + m_rotationOffset.m_y;
    Matrix view(true);
    if (unkFC == 1) fn_8003F03C(&view, m_0x9C.m_x, m_0x9C.m_y, m_0x9C.m_z);
    else view.setIdentity();
    fn_8003EBF4(&view, roll);
    fn_8003EA9C(&view, -pitch);
    view.rotY(-yaw);
    fn_8003F074(&view, translation.m_x, translation.m_y, translation.m_z);
    *reinterpret_cast<Matrix*>(m_view) = view;
    view.inverse(reinterpret_cast<Matrix*>(m_inverseView));
    float halfFov = m_fovY * 0.5f;
    float tangentY = tan(halfFov);
    float tangentX = tangentY * m_aspect;
    float angleX = atan(tangentX);
    m_tanHalfFovY = tangentY; m_tanHalfFovX = tangentX;
    m_secHalfFovX = 1.0f / float(cos(angleX));
    m_secHalfFovY = 1.0f / float(cos(halfFov));
}

void gfCamera::setGX() {
    Mtx44 proj;
    GXSetScissor(m_scissorX, m_scissorY, m_scissorWidth, m_scissorHeight);
    GXSetViewport(m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight, m_viewportNear, m_viewportFar);
    GXSetScissorBoxOffset(0, 0);
    if (m_projectionType == Projection_Orthogonal) {
        C_MTXOrtho(proj, m_orthoParam.m_top, m_orthoParam.m_bottom, m_orthoParam.m_left, m_orthoParam.m_right, m_near, m_far);
        GXSetProjection(proj, GX_ORTHOGRAPHIC);
    } else {
        C_MTXPerspective(proj, 57.29578f * m_fovY, m_aspect, m_near, m_far);
        GXSetProjection(proj, GX_PERSPECTIVE);
    }
    GXLoadPosMtxImm(m_view, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
}

void gfCamera::setNwCamera(nw4r::g3d::Camera* camera) {
    gfCamera adjusted = *this;
    adjusted.m_centerPos += m_0x9C;
    adjusted.m_rot.m_x += m_rotationOffset.m_x;
    adjusted.m_rot.m_y += m_rotationOffset.m_y;
    adjusted.m_transformFlag.m_mask = 0xE2;
    adjusted.calc();
    nw4r::g3d::Camera::PostureInfo posture;
    posture.tp = nw4r::g3d::Camera::POSTURE_AIM;
    posture.cameraTwist = -m_rot.m_z * 57.29578f;
    if (unkFC == 0) {
        Vec3f target = adjusted.m_targetPos + m_targetOffset;
        camera->SetPosition(adjusted.m_centerPos.m_x, adjusted.m_centerPos.m_y, adjusted.m_centerPos.m_z);
        posture.cameraTarget.x = target.m_x; posture.cameraTarget.y = target.m_y; posture.cameraTarget.z = target.m_z;
    } else {
        camera->SetPosition(*reinterpret_cast<const nw4r::math::VEC3*>(&m_centerPos));
        posture.cameraTarget.x = m_targetPos.m_x; posture.cameraTarget.y = m_targetPos.m_y; posture.cameraTarget.z = m_targetPos.m_z;
    }
    camera->SetPosture(posture);
    camera->SetViewport(m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight);
    camera->SetViewportZRange(m_viewportNear, m_viewportFar);
    camera->SetScissor(m_scissorX, m_scissorY, m_scissorWidth, m_scissorHeight);
    if (m_projectionType == Projection_Perspective) camera->SetPerspective(m_fovY * 57.29578f, m_aspect, m_near, m_far);
    else camera->SetOrtho(m_orthoParam.m_top, m_orthoParam.m_bottom, m_orthoParam.m_left, m_orthoParam.m_right, m_near, m_far);
}

void gfCamera::setFromNwCamera(nw4r::g3d::Camera* camera) {
    nw4r::math::VEC3 position;
    camera->GetPosition(&position);
    m_centerPos.m_x = position.x; m_centerPos.m_y = position.y; m_centerPos.m_z = position.z;
    nw4r::g3d::Camera::PostureInfo posture;
    camera->GetPosture(&posture);
    u16 flags;
    switch (posture.tp) {
    case nw4r::g3d::Camera::POSTURE_LOOKAT:
        m_targetPos.m_x = posture.cameraTarget.x; m_targetPos.m_y = posture.cameraTarget.y; m_targetPos.m_z = posture.cameraTarget.z;
        unkFA.m_mask |= 2;
        m_up.normalize(reinterpret_cast<Vec3f*>(&posture.cameraUp));
        unkFA.m_mask |= 1;
        flags = 9;
        break;
    case nw4r::g3d::Camera::POSTURE_ROTATE: {
        Vec3f angles(posture.cameraRotate.x * 0.017453294f, posture.cameraRotate.y * 0.017453294f, posture.cameraRotate.z * 0.017453294f);
        Vec3f direction(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f);
        Matrix rotation(true);
        fn_8003ECA0(&rotation, angles.m_x, angles.m_y, angles.m_z);
        fn_8003E3BC(&rotation, &direction, &direction);
        fn_8003E3BC(&rotation, &up, &up);
        m_direction = direction; unkFA.m_mask |= 4;
        m_up.normalize(&up); unkFA.m_mask |= 1;
        // Preserve native degree-valued roll storage; converting it here changes the original behavior.
        m_rot.m_z = posture.cameraRotate.z;
        unkCC = 200.0f; unkFA.m_mask |= 0xC0;
        flags = 0x8C;
        break;
    }
    case nw4r::g3d::Camera::POSTURE_AIM:
        m_targetPos.m_x = posture.cameraTarget.x; m_targetPos.m_y = posture.cameraTarget.y; m_targetPos.m_z = posture.cameraTarget.z;
        unkFA.m_mask |= 2;
        m_rot.m_z = posture.cameraTwist; unkFA.m_mask |= 0x40;
        flags = 0x41;
        break;
    }
    m_transformFlag.m_mask = flags | 2;
    unkD0 = camera->ref().projFovy * 0.017453292f;
    m_near = camera->ref().projNear; m_far = camera->ref().projFar;
    if (camera->GetProjectionType() == GX_PERSPECTIVE) m_projectionType = Projection_Perspective;
    else {
        m_projectionType = Projection_Orthogonal;
        m_orthoParam.m_top = camera->ref().projTop; m_orthoParam.m_bottom = camera->ref().projBottom;
        m_orthoParam.m_left = camera->ref().projLeft; m_orthoParam.m_right = camera->ref().projRight;
    }
}

void gfCamera::setFromNwCameraAllParam(nw4r::g3d::Camera* camera) {
    nw4r::g3d::Camera copy = *camera;
    setFromNwCamera(&copy);
    m_aspect = camera->ref().projAspect;
}

void gfCamera::calcProjection3Dto2D(Vec3f* world, Vec2f* screen) {
    Mtx44 projection, transposed;
    if (m_projectionType == Projection_Perspective) C_MTXPerspective(projection, m_fovY * 57.29578f, m_aspect, m_near, m_far);
    else C_MTXOrtho(projection, 224.0f, -224.0f, -320.0f, 320.0f, m_near, m_far);
    fn_8003F898(projection, transposed);
    Matrix view(reinterpret_cast<Matrix*>(m_view));
    struct Position4 { float x, y, z, w; } position = {world->m_x, world->m_y, world->m_z, 1.0f};
    fn_8003E46C(&view, &position, &position);
    float invW = 1.0f / (transposed[0][3] * position.x + transposed[1][3] * position.y + transposed[2][3] * position.z + transposed[3][3]);
    screen->m_x = ((transposed[0][0] * position.x + transposed[1][0] * position.y + transposed[2][0] * position.z + transposed[3][0]) * invW + 1.0f) * 0.5f * m_viewportWidth;
    screen->m_y = (1.0f - (transposed[0][1] * position.x + transposed[1][1] * position.y + transposed[2][1] * position.z + transposed[3][1]) * invW) * 0.5f * m_viewportHeight;
}

// These tests deliberately use abs(depth), ignore near/far, and accept spheres behind the eye.
// Negated ordered comparisons also preserve the native handling of unordered inputs.
bool gfCamera::isClipIn(const ClipSphere* sphere) {
    Vec3f position(sphere->center.m_x, sphere->center.m_y, sphere->center.m_z), viewPosition;
    bool inside = false;
    reinterpret_cast<Matrix*>(m_view)->mulPos(&position, &viewPosition);
    float x, depth, tanX, tanY, secY, radius;
    double depthWide = __fabs(viewPosition.m_z);
    double xWide = __fabs(viewPosition.m_x);
    tanX = m_tanHalfFovX;
    radius = sphere->radius;
    depth = depthWide;
    tanY = m_tanHalfFovY; secY = m_secHalfFovY;
    x = xWide;
    if (!(tanX * depth + radius * m_secHalfFovX < x)) {
        double yWide = __fabs(viewPosition.m_y);
        float y = yWide;
        if (!(tanY * depth + radius * secY < y)) inside = true;
    }
    return inside;
}
bool gfCamera::isClipInAll(const ClipSphere* sphere) {
    Vec3f position(sphere->center.m_x, sphere->center.m_y, sphere->center.m_z), viewPosition;
    bool inside = false;
    reinterpret_cast<Matrix*>(m_view)->mulPos(&position, &viewPosition);
    float x, depth, tanX, tanY, secY, radius;
    double depthWide = __fabs(viewPosition.m_z);
    double xWide = __fabs(viewPosition.m_x);
    tanX = m_tanHalfFovX;
    radius = sphere->radius;
    depth = depthWide;
    tanY = m_tanHalfFovY; secY = m_secHalfFovY;
    x = xWide;
    if (!(radius * m_secHalfFovX + x >= tanX * depth)) {
        double yWide = __fabs(viewPosition.m_y);
        float y = yWide;
        if (!(radius * secY + y >= tanY * depth)) inside = true;
    }
    return inside;
}
u8 gfCamera::getClipOutStatus(const ClipSphere* sphere) {
    Vec3f position(sphere->center.m_x, sphere->center.m_y, sphere->center.m_z), viewPosition;
    reinterpret_cast<Matrix*>(m_view)->mulPos(&position, &viewPosition);
    u8 result = 0;
    float x, depth, tanX, tanY, secY, radius;
    double depthWide = __fabs(viewPosition.m_z);
    double xWide = __fabs(viewPosition.m_x);
    tanX = m_tanHalfFovX;
    radius = sphere->radius;
    depth = depthWide;
    tanY = m_tanHalfFovY; secY = m_secHalfFovY;
    x = xWide;
    if (tanX * depth + radius * m_secHalfFovX < x) result |= 1;
    double yWide = __fabs(viewPosition.m_y);
    float y = yWide;
    if (tanY * depth + radius * secY < y) result |= 2;
    return result;
}
void gfCamera::getScreenPosition2WorldVector(Vec3f* direction, float x, float y) {
    float tangent = tan(0.5f * m_fovY);
    direction->m_x = (2.0f * tangent * m_aspect * (x - 0.5f * m_viewportWidth)) / m_viewportWidth;
    float height = m_viewportHeight;
    direction->m_z = -1.0f;
    direction->m_y = -((2.0f * tangent * (y - 0.5f * height)) / height);
    fn_8003E3BC(reinterpret_cast<Matrix*>(m_inverseView), direction, direction);
    direction->normalize();
}

gfCameraManager* gfCameraManager::create() {
    if (lbl_8059FFC0 == NULL) new (Heaps::SystemFW) gfCameraManager;
    return lbl_8059FFC0;
}
gfCameraManager* gfCameraManager::getManager() {
    // MATCH-ONLY: retain the native repeated singleton check after inlining create().
    if (lbl_8059FFC0 == NULL && lbl_8059FFC0 == NULL) new (Heaps::SystemFW) gfCameraManager;
    return lbl_8059FFC0;
}
gfCameraManager::gfCameraManager() {
    // Publish before init(): camera initialization itself queries the manager.
    lbl_8059FFC0 = this;
    if (g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) setAspectRatio(0);
    else setAspectRatio(1);
    for (int i = 0; i < 6; ++i) m_cameras[i].init();
    m_cameras[4].m_projectionType = gfCamera::Projection_Orthogonal;
}
void gfCameraManager::setAspectRatio(int mode) {
    float aspect = 1.3333334f;
    bool wide = false;
    if (mode == 0) { aspect = 1.7777778f; wide = true; }
    fn_800384CC(lbl_805A0098, wide);
    m_aspectCorrection = lbl_805A0098->aspectCorrection;
    m_aspectRatio = aspect;
    float effective = aspect * lbl_805A0098->aspectCorrection;
    m_cameras[0].m_aspect = effective; m_cameras[1].m_aspect = effective;
    m_cameras[2].m_aspect = effective; m_cameras[3].m_aspect = effective;
    m_cameras[4].m_aspect = effective; m_cameras[5].m_aspect = effective;
}
