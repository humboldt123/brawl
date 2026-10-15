#define MT_VEC3F_MUL_INLINE
// Stage collision primitives (the "cl" library): newCollision, clCapsule, clSphere and the clManager pools.
// Unit boundary (sora_melee): .text 0x70-0x3860 (after global_destructor_chain, before StageObject).

#include <cl/cl_collision.h>
#include <sr/sr_common.h>

void getScalingFactor(Matrix* matrix, Vec3f* scale); // main binary, name unknown

extern char g_clTargetNull[]; // bss: the shared null target array every primitive starts with

newCollision::newCollision() {
    m_targets = reinterpret_cast<soArray<clTarget>*>(g_clTargetNull);
    m_isActive = false;
    m_isInfinite = false;
}

// Two primitives can only touch when both are active and, if the masks are used, every layer pair agrees: the first
// object has to be hit by what the second one is (and the other way round).
bool newCollision::checkTarget(newCollision* other, int useTargetMask) {
    if (m_isActive == 1 && other->m_isActive == 1) {
        if (useTargetMask == 0) {
            return true;
        }
        int count = m_targets->size();
        int otherCount = other->m_targets->size();
        if (count > otherCount) {
            count = otherCount;
        }
        for (int i = 0; i < count; i++) {
            clTarget* mine = &m_targets->at(i);
            clTarget* theirs = &other->m_targets->at(i);
            if ((mine->m_0 & theirs->m_4) == 0 || (mine->m_4 & theirs->m_0) == 0) {
                return false;
            }
        }
        return true;
    }
    return false;
}

// Infinite shapes always overlap: both objects report the center of the finite one as the hit position.
bool newCollision::checkSizeInfinity(newCollision* other) {
    if (m_isInfinite == 1 || other->m_isInfinite == 1) {
        if (m_isInfinite == 1) {
            m_hitPos = other->getCenterPos();
        } else {
            m_hitPos = getCenterPos();
        }
        other->m_hitPos = m_hitPos;
        m_distance = 3.4028235e+38f;
        other->m_distance = 3.4028235e+38f;
        return true;
    }
    return false;
}

clCapsule::clCapsule() { }

clCapsule::~clCapsule() { }

int clCapsule::getType() {
    return 1;
}

void clCapsule::update(float radius, Vec3f* pos0, Vec3f* pos1, Matrix* matrix) {
    float z1, y1, x1, z0, y0, x0;
    x0 = pos0->m_x; y0 = pos0->m_y; z0 = pos0->m_z;
    x1 = pos1->m_x; y1 = pos1->m_y; z1 = pos1->m_z;
    m_pos.m_x = x0;
    m_pos.m_y = y0;
    m_pos.m_z = z0;
    m_radius = radius;
    m_pos1.m_x = x1;
    m_pos1.m_y = y1;
    m_pos1.m_z = z1;
    if (matrix != NULL) {
        m_matrix = *matrix;
        getScalingFactor(static_cast<Matrix*>(&m_matrix), &m_scale);
    } else {
        static_cast<Matrix&>(m_matrix).setIdentity();
        m_scale.m_x = m_scale.m_y = m_scale.m_z = 1.0f;
    }
}

void clCapsule::renew(Vec3f* pos0, Vec3f* pos1, Matrix* matrix) {
    float z1, y1, x1, z0, y0, x0;
    x0 = pos0->m_x; y0 = pos0->m_y; z0 = pos0->m_z;
    x1 = pos1->m_x; y1 = pos1->m_y; z1 = pos1->m_z;
    m_pos.m_x = x0;
    m_pos.m_y = y0;
    m_pos.m_z = z0;
    m_pos1.m_x = x1;
    m_pos1.m_y = y1;
    m_pos1.m_z = z1;
    if (matrix != NULL) {
        m_matrix = *matrix;
    } else {
        static_cast<Matrix&>(m_matrix).setIdentity();
    }
}

Vec3f clCapsule::getCenterPos() {
    return (m_pos + m_pos1) * 0.5f;
}

void clCapsule::setHitPosCenter() {
    m_hitPos = (m_pos + m_pos1) * 0.5f;
}

void clCapsule::display() { }

clSphere::clSphere() { }

clSphere::~clSphere() { }

int clSphere::getType() {
    return 0;
}

void clSphere::update(float radius, Vec3f* pos0, Vec3f* pos1, Matrix* matrix) {
    float z1, y1, x1, z0, y0, x0;
    x0 = pos0->m_x; y0 = pos0->m_y; z0 = pos0->m_z;
    x1 = pos1->m_x; y1 = pos1->m_y; z1 = pos1->m_z;
    m_pos.m_x = x0;
    m_pos.m_y = y0;
    m_pos.m_z = z0;
    m_radius = radius;
    m_pos1.m_x = x1;
    m_pos1.m_y = y1;
    m_pos1.m_z = z1;
}

void clSphere::renew(Vec3f* pos0, Vec3f* pos1, Matrix* matrix) {
    float z0, y0, x0;
    x0 = pos0->m_x; y0 = pos0->m_y; z0 = pos0->m_z;
    m_pos.m_x = x0;
    m_pos.m_y = y0;
    m_pos.m_z = z0;
}

Vec3f clSphere::getCenterPos() {
    return m_pos;
}

void clSphere::setHitPosCenter() {
    m_hitPos = m_pos;
}

void clSphere::display() { }

clManager::clManager() {
    m_pools[0] = NULL;
    m_poolSizes[0] = 0;
    m_pools[1] = NULL;
    m_poolSizes[1] = 0;
}

template <class T> newCollision* clManager::createCollision(nw4r::ut::List* list, int count) {
    T* array = new (Heaps::System) T[count];
    for (int i = 0; i < count; i++) {
        nw4r::ut::List_Append(list, &array[i]);
    }
    return array;
}

typedef newCollision* (*clCreateFunc)(nw4r::ut::List*, int);
static const clCreateFunc clCreateTable[2] = {
    clManager::createCollision<clCapsule>,
    clManager::createCollision<clSphere>,
};

void clManager::init(int* sizes) {
    for (int i = 0; i < 2; i++) {
        nw4r::ut::List_Init(&m_freeLists[i], offsetof(newCollision, m_link));
        newCollision* pool = clCreateTable[i](&m_freeLists[i], sizes[i]);
        int size = sizes[i];
        m_pools[i] = pool;
        m_poolSizes[i] = size;
    }
}

newCollision* clManager::allocateCollision(int type) {
    newCollision* collision = static_cast<newCollision*>(nw4r::ut::List_GetPrev(&m_freeLists[type], NULL));
    nw4r::ut::List_Remove(&m_freeLists[type], collision);
    return collision;
}

void clManager::freeCollision(int type, newCollision* collision) {
    nw4r::ut::List_Append(&m_freeLists[type], collision);
}

