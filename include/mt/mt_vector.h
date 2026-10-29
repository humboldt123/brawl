// Local BrawlHeaders shadow: allow the original Marth collision constructor call.
#pragma once

#include <StaticAssert.h>
#include <math.h>
#include <mt/mt_common.h>
#include <types.h>

class Vec3f;

#ifdef MT_VEC2F_PLAIN_BASE
// MATCH-ONLY: the owning unit copies whole vectors with the aggregate word copy of a plain base.
struct Vec2fPlainBase {
    float m_x;
    float m_y;
};
class Vec2f : public Vec2fPlainBase {
public:
#else
class Vec2f {
public:
    float m_x;
    float m_y;
#endif

    Vec2f() { }
    // The map names it Vec2f::set; the one unit that uses it (wnem_proc_common.cpp) calls the weak copy out of line.
    void set(float x, float y) __attribute__((never_inline)) {
        m_x = x;
        m_y = y;
    }
    inline Vec2f(const Vec3f& xyz)
#ifdef MT_VEC2F_FROM_VEC3_CTOR_NOINLINE
        __attribute__((never_inline)) // MATCH-ONLY: retain the native constructor call.
#endif
        ;
    Vec2f(float x, float y)
#ifdef MT_VEC2F_CTOR_NOINLINE
        __attribute__((never_inline)) // MATCH-ONLY: retain a proved out-of-line constructor call.
#endif
#ifdef MT_VEC2F_CTOR_OUT_OF_LINE
        ; // MATCH-ONLY: an owning unit supplies the weak constructor in native order.
#elif defined(MT_VEC2F_PLAIN_BASE)
    { m_x = x; m_y = y; }
#else
        : m_x(x), m_y(y) { }
#endif

#ifndef MT_VEC2F_IMPLICIT_ASSIGN // MATCH-ONLY: some units copy with the implicit (word-wise) assignment and elide copy temporaries.
    Vec2f& operator=(const Vec2f& source)
#ifdef MT_VEC2F_ASSIGN_NOINLINE
        __attribute__((never_inline)) // MATCH-ONLY: preserve evidenced out-of-line assignments.
#endif
    {
        // MATCH-ONLY: retain the original assignment's scalar float copies.
        m_x = source.m_x;
        m_y = source.m_y;
        return *this;
    }
#endif

    friend Vec2f operator+(const Vec2f& lhs, const Vec2f& rhs) {
        Vec2f res;
        res.m_x = lhs.m_x + rhs.m_x;
        res.m_y = lhs.m_y + rhs.m_y;
        return res;
    }

    friend Vec2f operator-(const Vec2f& lhs, const Vec2f& rhs)
#ifdef MT_VEC2F_SUB_NOINLINE
        __attribute__((never_inline)) // MATCH-ONLY: retain an evidenced out-of-line subtraction call.
#endif
    {
        Vec2f res;
        res.m_x = lhs.m_x - rhs.m_x;
        res.m_y = lhs.m_y - rhs.m_y;
        return res;
    }

    friend Vec2f operator*(const Vec2f& lhs, const float c)
#ifdef MT_VEC2F_MUL_OUT_OF_LINE
        ; // MATCH-ONLY: the owning unit calls the weak copy another unit of the REL carries.
#else
    {
        Vec2f res;
        res.m_x = lhs.m_x * c;
        res.m_y = lhs.m_y * c;
        return res;
    }
#endif

    friend Vec2f operator/(const Vec2f& lhs, float c) {
        return lhs * (1 / c);
    }

    Vec2f& operator+=(const Vec2f& v)
#ifdef MT_VEC2F_ADD_ASSIGN_DIRECT
    { // MATCH-ONLY: the weak out-of-line copy in the Pikmin article adds the components directly.
        m_x = m_x + v.m_x;
        m_y = m_y + v.m_y;
        return *this;
    }
#else
    {
        *this = *this + v;
        return *this;
    }
#endif

    Vec2f& operator-=(const Vec2f& v) {
        *this = *this - v;
        return *this;
    }

    Vec2f& operator*=(const float c)
#ifdef MT_VEC2F_MUL_ASSIGN_DIRECT
    { // MATCH-ONLY: Pikachu's Quick Attack scales the components directly in the out-of-line copy.
        m_x = m_x * c;
        m_y = m_y * c;
        return *this;
    }
#else
    {
        *this = *this * c;
        return *this;
    }
#endif

    // HYPOTHESIS: native calls pass vector addresses; original reference qualifiers are unresolved.
    float mtVecAngle(Vec2f& other);

    float lengthSq()
#ifdef MT_VEC2F_LENGTH_SQ_OUT_OF_LINE
        __attribute__((never_inline)); // MATCH-ONLY: retain the owning Vanish unit's helper calls.
#elif defined(MT_VEC2F_LENGTH_SQ_SINGLE_EXPR)
    { // MATCH-ONLY: the item kinetic energy units square both components in place (one expression).
        return m_x * m_x + m_y * m_y;
    }
#else
    {
        float xSq = m_x * m_x;
        float ySq = m_y * m_y;
        return xSq + ySq;
    }
#endif

    float length()
#ifdef MT_VEC2F_LENGTH_OUT_OF_LINE
        ; // MATCH-ONLY: the owning unit calls the weak copy another unit of the REL carries.
#else
    {
        float lengthSquared = this->lengthSq();
        // MATCH-ONLY: retain the original fabs double temporary and frsp at
        // conversion. The ordered <= comparison also preserves NaN behavior.
        double absolute = __fabs(lengthSquared);
        float magnitude = absolute;
        if (magnitude <= 1.17549435e-38f) return 0.0f;
        return lengthSquared * rsqrtf(lengthSquared);
    }
#endif

    // Writes this - v to out.
    void sub(const Vec2f* v, Vec2f* out)
#ifdef MT_VEC2F_DISTANCE_NOINLINE
        __attribute__((never_inline))
#endif
    {
        out->m_x = m_x - v->m_x;
        out->m_y = m_y - v->m_y;
    }

#ifdef MT_VEC2F_DISTANCE_NOINLINE
    // MATCH-ONLY: the Enemy unit calls its weak out-of-line copy, which subtracts through sub.
    float distance(Vec2f* v) __attribute__((never_inline)) {
        Vec2f disp;
        sub(v, &disp);
        return disp.length();
    }
#else
    float distance(Vec2f* v) {
        Vec2f disp = *this - *v;
        return disp.length();
    }
#endif

    void normalize();
    void normalize(Vec2f* input);

    void rot(float angle, Vec2f* out);

    // Writes this vector scaled by c to out (the weapon articles call the out-of-line copy: opt in with MT_VEC2F_SCALE_NOINLINE).
    void scale(float c, Vec2f* out)
#ifdef MT_VEC2F_SCALE_NOINLINE
        __attribute__((never_inline)) // MATCH-ONLY: retain the out-of-line weak helper.
#endif
    {
        out->m_x = m_x * c;
        out->m_y = m_y * c;
    }

    static void copy(Vec2f& dest, const Vec2f& src) {
#ifdef MATCHING
        __memcpy(&dest, &src, sizeof(Vec2f));
#else
        dest.m_x = src.m_x;
        dest.m_y = src.m_y;
#endif
    }
};
static_assert(sizeof(Vec2f) == 8, "Class is wrong size!");

class Vec3f {
public:
    union {
        struct {
            float m_x;
            float m_y;
            float m_z;
        };
        struct {
            float m_roll;
            float m_pitch;
            float m_yaw;
        };
    };

    Vec3f() { }
    Vec3f(float x, float y, float z)
#if defined(FT_MARTH_COLLISION_VEC3F_NOINLINE) || defined(MT_VEC3F_CTOR_NOINLINE)
        __attribute__((never_inline)) // MATCH-ONLY: retain the original out-of-line constructor.
#endif
        : m_x(x), m_y(y), m_z(z) { }

    // Native spring callers construct XYZ from an XY vector and a separate Z.
    // HYPOTHESIS: the original XY parameter's const qualifier is unresolved.
    Vec3f(const Vec2f& xy, float z)
#ifdef MT_VEC3F_FROM_VEC2_CTOR_NOINLINE
        __attribute__((never_inline)) // MATCH-ONLY: retain the native constructor call.
#endif
        : m_x(xy.m_x), m_y(xy.m_y), m_z(z) { }

    Vec3f& operator=(const Vec3f& orig)
#ifdef MT_VEC3F_ASSIGN_NOINLINE
        __attribute__((never_inline)) // MATCH-ONLY: verified R.O.B. article assignment calls.
#endif
    {
        m_x = orig.m_x;
        m_y = orig.m_y;
        m_z = orig.m_z;
        return *this;
    }

    void set(float x, float y, float z)
#ifdef MT_VEC3F_SET_NOINLINE
        __attribute__((never_inline)) // MATCH-ONLY: retain the out-of-line copy of Pikachu's Quick Attack.
#endif
    {
        m_x = x;
        m_y = y;
        m_z = z;
    }

    Vec2f* xy() const {
        return (Vec2f*)this;
    }

    // True when all three components are finite numbers (the map's name); defined by the unit that uses it
    // (wnem_proc_common.cpp), which calls it out of line.
    bool testNormal() __attribute__((never_inline));

    Vec3f operator+(const Vec3f& v);
    Vec3f operator-(const Vec3f& v)
#ifdef MT_VEC3F_OPERATORS_NOINLINE
        __attribute__((never_inline)) // MATCH-ONLY: the Enemy unit calls its weak out-of-line copy.
#endif
        ;

#ifdef MATCHING
    // MATCH-ONLY: paired-single form preserves the verified inline vector add.
    friend void Vec3fAdd(register Vec3f* pOut, register const Vec3f* lhs,
                     register const Vec3f* rhs) {
#ifdef MT_VEC3F_ADD_RHS_RESULT_XY_FIRST
        // MATCH-ONLY: retain the native independent XY/Z input lifetimes.
        register f32 fr2, fr3, fr1, fr0;
#else
        register f32 fr3, fr2, fr1, fr0;
#endif

        // clang-format off
        asm {
#if defined(MT_VEC3F_BIKE_UTILITY_OPERATORS) || defined(MT_VEC3F_ADD_NATIVE_ORDER) || defined(MT_VEC3F_ADD_RHS_RESULT_XY_FIRST)
            // MATCH-ONLY: these native callers reuse the right operand registers for the result.
            psq_l  fr3, Vec3f.m_x(lhs), 0, 0
            psq_l  fr1, Vec3f.m_x(rhs), 0, 0
            psq_l  fr2, Vec3f.m_z(lhs), 1, 0
            psq_l  fr0, Vec3f.m_z(rhs), 1, 0
            ps_add fr1, fr3, fr1
            ps_add fr0, fr2, fr0
            psq_st fr1, Vec3f.m_x(pOut), 0, 0
            psq_st fr0, Vec3f.m_z(pOut), 1, 0
#else
            psq_l  fr0, Vec3f.m_x(lhs),   0, 0
            psq_l  fr2, Vec3f.m_x(rhs),   0, 0
            psq_l  fr1, Vec3f.m_z(lhs),   1, 0
            psq_l  fr3, Vec3f.m_z(rhs),   1, 0
            ps_add fr0, fr0, fr2
            ps_add fr1, fr1, fr3
            psq_st fr0, Vec3f.m_x(pOut), 0, 0
            psq_st fr1, Vec3f.m_z(pOut), 1, 0
#endif
        }
        // clang-format on
    }

    friend void Vec3fSub(register Vec3f* pOut, register const Vec3f* lhs,
                     register const Vec3f* rhs) {
#if defined(MT_VEC3F_SUB_COMPACT)
        register f32 fr1, fr0, fr2;
#elif defined(MT_VEC3F_SUB_AIR_LASSO)
        register f32 fr0, fr1, fr2;
#elif defined(MT_VEC3F_SUB_DISTINCT)
        register f32 fr0, fr1, fr2, fr3, fr4, fr5;
#elif defined(MT_VEC3F_SUB_ASCENDING)
        // MATCH-ONLY: Finale's sweep uses the existing four-register subtract
        // with ascending register allocation; arithmetic and default are unchanged.
        register f32 fr0, fr1, fr2, fr3;
#else
        register f32 fr3, fr2, fr1, fr0;
#endif

        // clang-format off
        asm {
#if defined(MT_VEC3F_SUB_COMPACT)
            // MATCH-ONLY: the AI target-distance loop reuses the xy load
            // register for rhs.z, keeping the subtraction within three FPRs.
            psq_l  fr0, Vec3f.m_x(lhs), 0, 0
            psq_l  fr1, Vec3f.m_x(rhs), 0, 0
            psq_l  fr2, Vec3f.m_z(lhs), 1, 0
            ps_sub fr1, fr0, fr1
            psq_l  fr0, Vec3f.m_z(rhs), 1, 0
            ps_sub fr2, fr2, fr0
            psq_st fr1, Vec3f.m_x(pOut), 0, 0
            psq_st fr2, Vec3f.m_z(pOut), 1, 0
#elif defined(MT_VEC3F_SUB_AIR_LASSO)
            // MATCH-ONLY: xy half first, the z half reuses the load registers.
            psq_l  fr0, Vec3f.m_x(lhs), 0, 0
            psq_l  fr1, Vec3f.m_x(rhs), 0, 0
            ps_sub fr2, fr0, fr1
            psq_l  fr0, Vec3f.m_z(lhs), 1, 0
            psq_l  fr1, Vec3f.m_z(rhs), 1, 0
            ps_sub fr0, fr0, fr1
            psq_st fr2, Vec3f.m_x(pOut), 0, 0
            psq_st fr0, Vec3f.m_z(pOut), 1, 0
#elif defined(MT_VEC3F_SUB_DISTINCT)
            // MATCH-ONLY: every value in its own register variable (declared in ascending order, which selects
            // the physical registers); the adventure follower and Ike final smash fly units are built this way.
            psq_l  fr0, Vec3f.m_x(lhs), 0, 0
            psq_l  fr1, Vec3f.m_x(rhs), 0, 0
            psq_l  fr2, Vec3f.m_z(lhs), 1, 0
            psq_l  fr3, Vec3f.m_z(rhs), 1, 0
            ps_sub fr4, fr0, fr1
            ps_sub fr5, fr2, fr3
            psq_st fr4, Vec3f.m_x(pOut), 0, 0
            psq_st fr5, Vec3f.m_z(pOut), 1, 0
#elif defined(MT_VEC3F_BIKE_UTILITY_OPERATORS) || defined(MT_VEC3F_SUB_NATIVE_ORDER)
            // MATCH-ONLY: retain the same native register order for subtraction.
            psq_l  fr3, Vec3f.m_x(lhs), 0, 0
            psq_l  fr1, Vec3f.m_x(rhs), 0, 0
            psq_l  fr2, Vec3f.m_z(lhs), 1, 0
            psq_l  fr0, Vec3f.m_z(rhs), 1, 0
            ps_sub fr1, fr3, fr1
            ps_sub fr0, fr2, fr0
            psq_st fr1, Vec3f.m_x(pOut), 0, 0
            psq_st fr0, Vec3f.m_z(pOut), 1, 0
#else
            psq_l  fr0, Vec3f.m_x(lhs),   0, 0
            psq_l  fr1, Vec3f.m_x(rhs),   0, 0
            psq_l  fr2, Vec3f.m_z(lhs),   1, 0
            psq_l  fr3, Vec3f.m_z(rhs),   1, 0
            ps_sub fr0, fr0, fr1
            ps_sub fr1, fr2, fr3
            psq_st fr0, Vec3f.m_x(pOut), 0, 0
            psq_st fr1, Vec3f.m_z(pOut), 1, 0
#endif
        }
        // clang-format on
    }
    // MATCH-ONLY: second subtraction form seen at the air lasso call sites (opt in with MT_VEC3F_SUB_AIR_LASSO for the
    // operator form); the register variables are declared in the order that reproduces the allocation.
    friend void Vec3fSubLhsFirst(register Vec3f* pOut, register const Vec3f* lhs,
                     register const Vec3f* rhs) {
        register f32 frB, frE, frD, frC, frA;

        // clang-format off
        asm {
            psq_l  frA, Vec3f.m_x(lhs), 0, 0
            psq_l  frB, Vec3f.m_x(rhs), 0, 0
            psq_l  frC, Vec3f.m_z(lhs), 1, 0
            psq_l  frD, Vec3f.m_z(rhs), 1, 0
            ps_sub frB, frA, frB
            ps_sub frE, frC, frD
            psq_st frB, Vec3f.m_x(pOut), 0, 0
            psq_st frE, Vec3f.m_z(pOut), 1, 0
        }
        // clang-format on
    }
    // MATCH-ONLY: paired-single scale used by the generic article code (opt in with MT_VEC3F_MUL_INLINE).
    friend void Vec3fScale(register Vec3f* pOut, register const Vec3f* v, register float c) {
#ifdef MT_VEC3F_SCALE_SPLIT_RESULT
        // MATCH-ONLY: loaded Z and its product have distinct native lifetimes.
        register f32 frOut, frZ, frXY;
        // clang-format off
        asm {
            psq_l    frXY, Vec3f.m_x(v), 0, 0
            psq_l    frZ, Vec3f.m_z(v), 1, 0
            ps_muls0 frXY, frXY, c
            ps_muls0 frOut, frZ, c
            psq_st   frXY, Vec3f.m_x(pOut), 0, 0
            psq_st   frOut, Vec3f.m_z(pOut), 1, 0
        }
        // clang-format on
#else
        register f32 fr1, fr0;

        // clang-format off
        asm {
            psq_l    fr1, Vec3f.m_x(v), 0, 0
            psq_l    fr0, Vec3f.m_z(v), 1, 0
            ps_muls0 fr1, fr1, c
            ps_muls0 fr0, fr0, c
            psq_st   fr1, Vec3f.m_x(pOut), 0, 0
            psq_st   fr0, Vec3f.m_z(pOut), 1, 0
        }
        // clang-format on
#endif
    }

#else
    friend void Vec3fAdd(Vec3f* out, const Vec3f* lhs, const Vec3f* rhs) {
        out->m_x = lhs->m_x + rhs->m_x;
        out->m_y = lhs->m_y + rhs->m_y;
        out->m_z = lhs->m_z + rhs->m_z;
    }
    friend void Vec3fSub(Vec3f* pOut, const Vec3f* lhs, const Vec3f* rhs) {
        pOut->m_x = lhs->m_x - rhs->m_x;
        pOut->m_y = lhs->m_y - rhs->m_y;
        pOut->m_z = lhs->m_z - rhs->m_z;
    }
#endif

    Vec3f operator*(const float c);

    Vec3f operator/(const float c) {
        return *this * (1 / c);
    }

    Vec3f& operator+=(const Vec3f& v)
#ifdef MT_VEC3F_OPERATORS_NOINLINE
        __attribute__((never_inline)) // MATCH-ONLY: the Enemy unit calls its weak out-of-line copy, which adds in place.
    {
        Vec3fAdd(this, this, &v);
        return *this;
    }
#else
    {
        *this = *this + v;
        return *this;
    }
#endif

    Vec3f& operator-=(const Vec3f& v) {
        *this = *this - v;
        return *this;
    }

    Vec3f& operator*=(const float c) {
        *this = *this * c;
        return *this;
    }

    // Scales this vector by c in place.
    void scale(float c)
#ifdef MT_VEC3F_SCALE_NOINLINE
        __attribute__((never_inline)) // MATCH-ONLY: the Enemy unit calls its weak out-of-line copy.
#endif
    {
        m_x = m_x * c;
        m_y = m_y * c;
        m_z = m_z * c;
    }

    float lengthSq()
#ifdef MT_VEC3F_LENGTH_SQ_INLINE
    { // MATCH-ONLY: this unit expands the verified Z + (X + Y) grouping inline.
        float xSq = m_x * m_x;
        float ySq = m_y * m_y;
        float zSq = m_z * m_z;
        return zSq + (xSq + ySq);
    }
#else
    ;
#endif
    float length();
    float distance(Vec3f* v) {
        Vec3f disp = *this - *v;
        return disp.length();
    }
    // HYPOTHESIS: source argument order is unresolved; axis/angle/output roles are verified.
    // Original DOL fn_8003DF50 stays unrenamed until its source signature is established.
    void rot(Vec3f* axis, float angle, Vec3f* out);
    void normalize();
    void normalize(Vec3f* input);
};
static_assert(sizeof(Vec3f) == 12, "Class is wrong size!");

// Native Wario Utility callers identify these member operators and hidden returns.
// HYPOTHESIS: source const qualifiers retain the established declarations.
inline Vec3f Vec3f::operator+(const Vec3f& v) {
    Vec3f result;
    Vec3fAdd(&result, this, &v);
    return result;
}
#ifdef MT_VEC2F_PLAIN_BASE
inline Vec2f::Vec2f(const Vec3f& xyz) { m_x = xyz.m_x; m_y = xyz.m_y; }
#else
inline Vec2f::Vec2f(const Vec3f& xyz) : m_x(xyz.m_x), m_y(xyz.m_y) { }
#endif
#if defined(MATCHING) && defined(MT_VEC3F_MUL_INLINE)
inline Vec3f Vec3f::operator*(const float c) {
    Vec3f result;
    Vec3fScale(&result, this, c);
    return result;
}
#endif

inline Vec3f Vec3f::operator-(const Vec3f& v)
#ifdef MT_VEC3F_SUB_NOINLINE
    __attribute__((never_inline)) // MATCH-ONLY: retain the module's shared out-of-line subtraction.
#endif
{
    Vec3f result;
    Vec3fSub(&result, this, &v);
    return result;
}


class Rect2D {
public:
    float m_left;
    float m_right;
    float m_up;
    float m_down;
};

Vec3f operator*(const float c, const Vec3f& v);
