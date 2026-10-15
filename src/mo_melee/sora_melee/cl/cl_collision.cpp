#define MT_VEC3F_MUL_INLINE
// Stage collision primitives (the "cl" library): newCollision, clCapsule, clSphere and the clManager pools.
// Unit boundary (sora_melee): .text 0x70-0x3860 (after global_destructor_chain, before StageObject).

#include <cl/cl_collision.h>
#include <sr/sr_common.h>
#include <math.h>

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


bool clCapsule::check(newCollision* other, int useTargetMask) {
    if (checkTarget(other, useTargetMask) == 1) {
        if (checkSizeInfinity(other) == 1) {
            return true;
        }
        switch (other->getType()) {
        case 1: {
            clCapsule* otherCapsule = static_cast<clCapsule*>(other);
            Vec3f otherScale = otherCapsule->m_scale;
            float scale = m_scale.m_x;
            float dy = scale - m_scale.m_y;
            float dz = scale - m_scale.m_z;
            if (static_cast<float>(fabs(dy)) >= 0.001f || static_cast<float>(fabs(dz)) >= 0.001f) {
                return chkCapsuleScale(&m_scale, otherCapsule, &otherScale);
            }
            float oy = otherScale.m_x - otherScale.m_y;
            float oz = otherScale.m_x - otherScale.m_z;
            if (static_cast<float>(fabs(oy)) >= 0.001f || static_cast<float>(fabs(oz)) >= 0.001f) {
                return otherCapsule->chkCapsuleScale(&otherScale, this, &m_scale);
            }
            return chkCapsuleSameScale(m_radius * scale, otherCapsule);
        }
        case 0:
            return clUtil::chkCapsuleSphere(this, static_cast<clSphere*>(other));
        }
    }
    return false;
}

bool clSphere::check(newCollision* other, int useTargetMask) {
    if (checkTarget(other, useTargetMask) == 1) {
        if (checkSizeInfinity(other) == 1) {
            return true;
        }
        switch (other->getType()) {
        case 1:
            return clUtil::chkCapsuleSphere(static_cast<clCapsule*>(other), this);
        case 0:
            return chkSphere(static_cast<clSphere*>(other));
        }
    }
    return false;
}

// Grows the box (min xyz, max xyz) so that it contains the point.
static inline void clGrowBBox(float* bbox, Vec3f* p) {
    if (p->m_x < bbox[0]) {
        bbox[0] = p->m_x;
    }
    if (p->m_x > bbox[3]) {
        bbox[3] = p->m_x;
    }
    if (p->m_y < bbox[1]) {
        bbox[1] = p->m_y;
    }
    if (p->m_y > bbox[4]) {
        bbox[4] = p->m_y;
    }
    if (p->m_z < bbox[2]) {
        bbox[2] = p->m_z;
    }
    if (p->m_z > bbox[5]) {
        bbox[5] = p->m_z;
    }
}

void clCapsule::updateBBox(float* bbox) {
    if (m_isInfinite == 1) {
        bbox[2] = -3.4028235e+38f;
        bbox[1] = -3.4028235e+38f;
        bbox[0] = -3.4028235e+38f;
        bbox[5] = 3.4028235e+38f;
        bbox[4] = 3.4028235e+38f;
        bbox[3] = 3.4028235e+38f;
    } else {
        Vec3f extent = m_scale * m_radius;
        Vec3f lo;
        Vec3f hi;
        float x = m_pos.m_x;
        if (x > m_pos1.m_x) {
            hi.m_x = x + extent.m_x;
            lo.m_x = m_pos1.m_x - extent.m_x;
        } else {
            hi.m_x = m_pos1.m_x + extent.m_x;
            lo.m_x = x - extent.m_x;
        }
        float y = m_pos.m_y;
        if (y > m_pos1.m_y) {
            hi.m_y = y + extent.m_y;
            lo.m_y = m_pos1.m_y - extent.m_y;
        } else {
            hi.m_y = m_pos1.m_y + extent.m_y;
            lo.m_y = y - extent.m_y;
        }
        float z = m_pos.m_z;
        if (z > m_pos1.m_z) {
            hi.m_z = z + extent.m_z;
            lo.m_z = m_pos1.m_z - extent.m_z;
        } else {
            hi.m_z = m_pos1.m_z + extent.m_z;
            lo.m_z = z - extent.m_z;
        }
        clGrowBBox(bbox, &lo);
        clGrowBBox(bbox, &hi);
    }
}

void clSphere::updateBBox(float* bbox) {
    if (m_isInfinite == 1) {
        bbox[2] = -3.4028235e+38f;
        bbox[1] = -3.4028235e+38f;
        bbox[0] = -3.4028235e+38f;
        bbox[5] = 3.4028235e+38f;
        bbox[4] = 3.4028235e+38f;
        bbox[3] = 3.4028235e+38f;
    } else {
        Vec3f lo;
        Vec3f hi;
        float x = m_pos.m_x;
        if (x > m_pos1.m_x) {
            hi.m_x = x + m_radius;
            lo.m_x = m_pos1.m_x - m_radius;
        } else {
            hi.m_x = m_pos1.m_x + m_radius;
            lo.m_x = x - m_radius;
        }
        float y = m_pos.m_y;
        if (y > m_pos1.m_y) {
            hi.m_y = y + m_radius;
            lo.m_y = m_pos1.m_y - m_radius;
        } else {
            hi.m_y = m_pos1.m_y + m_radius;
            lo.m_y = y - m_radius;
        }
        float z = m_pos.m_z;
        if (z > m_pos1.m_z) {
            hi.m_z = z + m_radius;
            lo.m_z = m_pos1.m_z - m_radius;
        } else {
            hi.m_z = m_pos1.m_z + m_radius;
            lo.m_z = z - m_radius;
        }
        clGrowBBox(bbox, &lo);
        clGrowBBox(bbox, &hi);
    }
}

bool clUtil::chkRough(float radius, float* a, float* b, float* c, float* d) {
    if (a[0] > b[0]) {
        if (a[0] + radius < c[0] && a[0] + radius < d[0]) {
            return false;
        }
        if (b[0] - radius > c[0] && b[0] - radius > d[0]) {
            return false;
        }
    } else {
        if (a[0] - radius > c[0] && a[0] - radius > d[0]) {
            return false;
        }
        if (b[0] + radius < c[0] && b[0] + radius < d[0]) {
            return false;
        }
    }
    if (a[1] > b[1]) {
        if (a[1] + radius < c[1] && a[1] + radius < d[1]) {
            return false;
        }
        if (b[1] - radius > c[1] && b[1] - radius > d[1]) {
            return false;
        }
    } else {
        if (a[1] - radius > c[1] && a[1] - radius > d[1]) {
            return false;
        }
        if (b[1] + radius < c[1] && b[1] + radius < d[1]) {
            return false;
        }
    }
    if (a[2] > b[2]) {
        if (a[2] + radius < c[2] && a[2] + radius < d[2]) {
            return false;
        }
        if (b[2] - radius > c[2] && b[2] - radius > d[2]) {
            return false;
        }
    } else {
        if (a[2] - radius > c[2] && a[2] - radius > d[2]) {
            return false;
        }
        if (b[2] + radius < c[2] && b[2] + radius < d[2]) {
            return false;
        }
    }
    return true;
}

bool clUtil::chkCapsuleSphere(clCapsule* capsule, clSphere* sphere) {
    Vec3f scale = capsule->m_scale;
    if (static_cast<float>(fabs(scale.m_x - scale.m_y)) < 0.001f && static_cast<float>(fabs(scale.m_x - scale.m_z)) < 0.001f) {
        return chkCapsuleSphereSameScale(capsule->m_radius * scale.m_x, capsule, sphere);
    }
    return chkCapsuleSphereScale(capsule, &scale, sphere);
}

bool clUtil::chkRough1(float* a, float* b, float* c, float* d, float* radius) {
    if (a[0] > b[0]) {
        if (a[0] + radius[0] < c[0] && a[0] + radius[0] < d[0]) {
            return false;
        }
        if (b[0] - radius[0] > c[0] && b[0] - radius[0] > d[0]) {
            return false;
        }
    } else {
        if (a[0] - radius[0] > c[0] && a[0] - radius[0] > d[0]) {
            return false;
        }
        if (b[0] + radius[0] < c[0] && b[0] + radius[0] < d[0]) {
            return false;
        }
    }
    if (a[1] > b[1]) {
        if (a[1] + radius[1] < c[1] && a[1] + radius[1] < d[1]) {
            return false;
        }
        if (b[1] - radius[1] > c[1] && b[1] - radius[1] > d[1]) {
            return false;
        }
    } else {
        if (a[1] - radius[1] > c[1] && a[1] - radius[1] > d[1]) {
            return false;
        }
        if (b[1] + radius[1] < c[1] && b[1] + radius[1] < d[1]) {
            return false;
        }
    }
    if (a[2] > b[2]) {
        if (a[2] + radius[2] < c[2] && a[2] + radius[2] < d[2]) {
            return false;
        }
        if (b[2] - radius[2] > c[2] && b[2] - radius[2] > d[2]) {
            return false;
        }
    } else {
        if (a[2] - radius[2] > c[2] && a[2] - radius[2] > d[2]) {
            return false;
        }
        if (b[2] + radius[2] < c[2] && b[2] + radius[2] < d[2]) {
            return false;
        }
    }
    return true;
}

// Vec3f::length() inlined (the original inlines it here)
static inline float clLength(const Vec3f& v) {
    float lengthSq = v.m_z * v.m_z + v.m_x * v.m_x + v.m_y * v.m_y;
    if (static_cast<float>(fabs(lengthSq)) <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSq * rsqrtf(lengthSq);
}

bool clCapsule::chkCapsuleSameScale(float radius, clCapsule* other) {
    float otherRadius = other->m_radius * other->m_scale.m_x;
    float sum = radius + otherRadius;
    if (clUtil::chkRough(sum, &m_pos1.m_x, &m_pos.m_x, &other->m_pos1.m_x, &other->m_pos.m_x) == 1) {
        Vec3f dir = m_pos - m_pos1;
        Vec3f otherDir = other->m_pos - other->m_pos1;
        float t;
        float otherT;
        clUtil::getLines(&m_pos1, &m_pos, &dir, &other->m_pos1, &other->m_pos, &otherDir, &t, &otherT);
        Vec3f offset = dir * t;
        Vec3f otherOffset = otherDir * otherT;
        Vec3f closest = m_pos1 + offset;
        Vec3f otherClosest = other->m_pos1 + otherOffset;
        Vec3f diff = closest - otherClosest;
        float distance = clLength(diff);
        if (sum + otherRadius >= distance) {
            if (distance > sum) {
                if (distance > otherRadius) {
                    float ratio = sum / distance;
                    Vec3f delta = otherClosest - closest;
                    Vec3f scaled = delta * ratio;
                    m_hitPos = closest + scaled;
                } else {
                    m_hitPos = otherClosest;
                }
            } else if (distance > otherRadius) {
                m_hitPos = closest;
            } else {
                Vec3f delta = otherClosest - closest;
                Vec3f scaled = delta * 0.5f;
                m_hitPos = closest + scaled;
            }
            other->m_hitPos = m_hitPos;
            float overlap = (otherRadius + sum) - distance;
            m_distance = overlap;
            other->m_distance = overlap;
            return true;
        }
    }
    return false;
}
