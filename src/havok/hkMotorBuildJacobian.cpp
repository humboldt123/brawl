// Havok translation unit hkMotorBuildJacobian.o (main.dol 0x802951A0-0x80295788).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802951A0   468  hkCalcMotorData   [map: hkMotorBuildJacobian__hkCalcMotorData]
//   0x80295374   928  hk1dLinearVelocityMotorBeginJacobian   [map: hkMotorBuildJacobian__hk1dLinearVelocityMotorBeginJacobian]
//   0x80295714   116  hk1dLinearVelocityMotorCommitJacobian   [map: hkMotorBuildJacobian__hk1dLinearVelocityMotorCommitJacobian]

#include <havok/hkMotorBuildJacobian.h>

// Raw field access: the motor record and the input record are read at fixed byte offsets (the same byte can be a
// float in one mode and a flag in another).
static inline float hkMotorLdF(const u8* base, u32 off) { return *(const float*)(base + off); }

static inline float hkMotorAbs(float v) { return (float)fabs((double)v); }

static inline float hkMotorClamp(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }

void hkMotorBuildJacobian::hkCalcMotorData(const u8* in, float* out) {
    u8* self = (u8*)this;
    switch (*(s8*)(self + 0x08)) {
    case 1: {
        const float* p = *(const float* const*)(in + 0x04);
        float a = hkMotorLdF(in, 0x14);
        float m1c = hkMotorLdF(self, 0x1c);
        float s = a * m1c;
        float m20 = hkMotorLdF(self, 0x20);
        float p3 = p[3];
        float in10 = hkMotorLdF(in, 0x10);
        float u = s * p3;
        float t = m20 * p3;
        float d = a - u;
        float v;
        if (hkMotorAbs(d) > t) {
            v = u + (d > 0.0f ? t : -t); // HYPOTHESIS: sign of d selects the side
        } else {
            v = a;
        }
        float g = hkMotorAbs(a);
        float c = hkMotorLdF(in, 0x0C);
        float hi = g - c;
        float lo = -g - c;
        float r;
        if (v < lo) {
            r = lo;
        } else if (v <= hi) {
            r = v;
        } else {
            r = hi;
        }
        out[0] = c;
        out[1] = (in10 + r) * p[4];
        out[2] = hkMotorLdF(self, 0x10);
        out[3] = -hkMotorLdF(self, 0x10);
        out[4] = hkMotorLdF(self, 0x14);
        out[5] = hkMotorLdF(self, 0x18);
        break;
    }
    case 2: {
        const float* p = *(const float* const*)(in + 0x04);
        float f;
        if (*(s8*)(self + 0x1c) != 0) {
            f = (hkMotorLdF(in, 0x10) + hkMotorLdF(in, 0x14)) * p[4];
        } else {
            f = hkMotorLdF(self, 0x18);
        }
        out[0] = hkMotorLdF(in, 0x0C);
        out[1] = f;
        out[2] = hkMotorLdF(self, 0x10);
        out[3] = hkMotorLdF(self, 0x0C);
        out[4] = hkMotorLdF(self, 0x14);
        out[5] = hkMotorLdF(self, 0x14);
        break;
    }
    case 3: {
        const float* p = *(const float* const*)(in + 0x04);
        float inv = 1.0f / hkMotorLdF(in, 0x00);
        float q = hkMotorLdF(self, 0x14) * p[0];
        q = p[0] * q;
        q = inv * q;
        float w = hkMotorLdF(self, 0x18) * p[0];
        w = inv * w;
        out[0] = hkMotorLdF(in, 0x10) + hkMotorLdF(in, 0x14);
        out[1] = 0.0f;
        out[2] = hkMotorLdF(self, 0x10);
        out[3] = hkMotorLdF(self, 0x0C);
        out[4] = hkMotorClamp(q, 0.0f, 1.0f);
        out[5] = hkMotorClamp(w, 0.0f, 1.0f);
        break;
    }
    default:
        break;
    }
}

void hkMotorBuildJacobian::hk1dLinearVelocityMotorCommitJacobian(const hkVector4* v, hkMotorJacobianCursor* c) {
    u8* elem = c->m_elem;
    float* sch = (float*)c->m_schema;
    float vy = v->y;
    sch[2] = unk08 * vy;
    sch[3] = unk0C * vy;
    sch[4] = unk04;
    sch[1] = 0.0f; // HYPOTHESIS: constant value
    sch[5] = unk10;
    sch[6] = unk14;
    *(u32*)sch = 0x0609001C;
    float vz = v->z;
    ((float*)elem)[3] = unk00 * vz;
    c->m_elem = elem + 0x30;
    c->m_schema = (u8*)sch + 0x1c;
}
