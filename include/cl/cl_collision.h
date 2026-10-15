#pragma once

#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/ut/ut_list.h>
#include <so/collision/so_collision.h>
#include <so/so_array.h>
#include <types.h>

// Collision primitives of the stage "cl" library: capsules and spheres that stage hazards sweep through the world, with a
// per-object target mask (clTarget) and a cheap bounding-box pre-test. Every primitive keeps the previous and the current
// end point, so a moving sphere is a capsule between the two positions.
//
// HYPOTHESIS: all field names (the library has no symbols); the layout comes from the constructors, update/renew and the
// check functions.
class newCollision {
public:
    Vec3f m_pos;                         // 0x00 center of a sphere / first end of a capsule
    float m_radius;                      // 0x0C
    Vec3f m_hitPos;                      // 0x10 center of the shape, written by setHitPosCenter
    soArray<clTarget>* m_targets;        // 0x1C per layer masks: (hit by, hits)
    float m_distance;                    // 0x20 FLT_MAX once a hit against an infinite shape was reported
    bool m_isActive;                     // 0x24
    bool m_isInfinite;                   // 0x25 the shape has no extent (always overlaps)
    nw4r::ut::Link m_link;               // 0x28 link in the clManager free list

    newCollision() __attribute__((never_inline));
    virtual ~newCollision() __attribute__((never_inline)) { }
    virtual int getType() = 0;           // 0 sphere, 1 capsule
    virtual void update(float radius, Vec3f* pos0, Vec3f* pos1, Matrix* matrix) = 0;
    virtual void renew(Vec3f* pos0, Vec3f* pos1, Matrix* matrix) = 0;
    virtual Vec3f getCenterPos() = 0;
    virtual void setHitPosCenter() = 0;
    virtual bool check(newCollision* other, int useTargetMask) = 0;
    virtual void display() = 0;
    virtual void updateBBox(float* bbox) = 0; // grows the box (min xyz, max xyz) around the shape

    bool checkTarget(newCollision* other, int useTargetMask) __attribute__((never_inline));
    bool checkSizeInfinity(newCollision* other) __attribute__((never_inline));
};

class clCapsule : public newCollision {
public:
    Vec3f m_pos1;     // 0x34 second end
    nw4r::math::MTX34 m_matrix; // 0x40 transform of the owner (identity when none; stored raw, no Matrix constructor)
    Vec3f m_scale;    // 0x70 scale of the matrix, per axis

    clCapsule();
    virtual ~clCapsule();
    virtual int getType();
    virtual void update(float radius, Vec3f* pos0, Vec3f* pos1, Matrix* matrix);
    virtual void renew(Vec3f* pos0, Vec3f* pos1, Matrix* matrix);
    virtual Vec3f getCenterPos();
    virtual void setHitPosCenter();
    virtual bool check(newCollision* other, int useTargetMask);
    virtual void display();
    virtual void updateBBox(float* bbox);

    bool chkCapsuleSameScale(float radius, clCapsule* other);
    bool chkCapsuleScale(Vec3f* scale, clCapsule* other, Vec3f* otherScale);
};
static_assert(sizeof(clCapsule) == 0x7C, "Class is wrong size!");

class clSphere : public newCollision {
public:
    Vec3f m_pos1;     // 0x34 position of the previous frame

    clSphere();
    virtual ~clSphere();
    virtual int getType();
    virtual void update(float radius, Vec3f* pos0, Vec3f* pos1, Matrix* matrix);
    virtual void renew(Vec3f* pos0, Vec3f* pos1, Matrix* matrix);
    virtual Vec3f getCenterPos();
    virtual void setHitPosCenter();
    virtual bool check(newCollision* other, int useTargetMask);
    virtual void display();
    virtual void updateBBox(float* bbox);

    bool chkSphere(clSphere* other);
};
static_assert(sizeof(clSphere) == 0x40, "Class is wrong size!");

// Overlap tests between the primitives.
class clUtil {
public:
    // Rejects when both points c and d lie beyond segment a-b grown by the radius on some axis.
    static bool chkRough(float radius, float* a, float* b, float* c, float* d);
    // Same as chkRough with a per-axis radius.
    static bool chkRough1(float* a, float* b, float* c, float* d, float* radius);
    static bool chkCapsuleSphereSameScale(float scaledRadius, clCapsule* capsule, clSphere* sphere);
    static bool chkCapsuleSphereScale(clCapsule* capsule, Vec3f* scale, clSphere* sphere);
    static bool chkCapsuleSphere(clCapsule* capsule, clSphere* sphere);
    // Closest points of two segments (each given by its two ends and direction): parameters ta/tb along the directions.
    static void getLines(Vec3f* a1, Vec3f* a0, Vec3f* da, Vec3f* b1, Vec3f* b0, Vec3f* db, float* ta, float* tb);
};

// Pools of capsules and spheres: free objects live in an intrusive list per type.
class clManager {
public:
    enum Type {
        Type_Capsule = 0,
        Type_Sphere = 1,
    };

    nw4r::ut::List m_freeLists[2];       // 0x00
    newCollision* m_pools[2];            // 0x18
    int m_poolSizes[2];                  // 0x20

    clManager();
    void init(int* sizes);
    newCollision* allocateCollision(int type);
    void freeCollision(int type, newCollision* collision);

    // HYPOTHESIS: one allocator per Type, called through a table by init.
    template <class T> static newCollision* createCollision(nw4r::ut::List* list, int count);
};
