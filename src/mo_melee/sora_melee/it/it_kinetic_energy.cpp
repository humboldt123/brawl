#define MT_VEC2F_LENGTH_SQ_SINGLE_EXPR
#include <it/kinetic/it_kinetic_energy.h>
#include <mt/mt_vector.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_world.h>
#include <types.h>

void itKineticEnergy::updateEnergy(soModuleAccesser* moduleAccesser) {
    Vec2f accel;
    Vec2f::copy(accel, m_accel);
    Vec2f brake;
    Vec2f::copy(brake, m_brake);
    if (m_considerGroundFriction || m_isFollowGroundNormal) {
        if (moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
            if (moduleAccesser->getGroundModule().isTouch(0xFF, 0)) {
                if (m_considerGroundFriction == true) {
                    float friction = moduleAccesser->getGroundModule().getDownFriction(0);
                    if (friction != 1.0f) {
                        accel.m_x *= friction;
                        accel.m_y *= friction;
                        brake.m_x *= friction;
                        brake.m_y *= friction;
                    }
                }
                if (m_isFollowGroundNormal == true) {
                    Vec2f normal;
                    Vec2f::copy(normal, moduleAccesser->getGroundModule().getTouchNormal(8, 0));
                    accel = soKineticUtility::projectionNormalFollow(&accel, &normal);
                }
            }
        }
    }
    m_speed.m_x += accel.m_x;
    m_speed.m_y += accel.m_y;
    Vec2f stableSpeed;
    Vec2f::copy(stableSpeed, m_stableSpeed);
    if (stableSpeed.m_x < 0.0f) {
        stableSpeed.m_x = 0.0f;
    }
    if (stableSpeed.m_y < 0.0f) {
        stableSpeed.m_y = 0.0f;
    }
    if (m_speed.m_x > stableSpeed.m_x) {
        m_speed.m_x -= brake.m_x;
        if (m_speed.m_x < stableSpeed.m_x) {
            m_speed.m_x = stableSpeed.m_x;
        }
    } else {
        m_speed.m_x += brake.m_x;
        if (m_speed.m_x > stableSpeed.m_x) {
            m_speed.m_x = stableSpeed.m_x;
        }
    }
    if (m_speed.m_y > stableSpeed.m_y) {
        m_speed.m_y -= brake.m_y;
        if (m_speed.m_y < stableSpeed.m_y) {
            m_speed.m_y = stableSpeed.m_y;
        }
    } else {
        m_speed.m_y += brake.m_y;
        if (m_speed.m_y > stableSpeed.m_y) {
            m_speed.m_y = stableSpeed.m_y;
        }
    }
    if (m_limitFlags & Limit_Length) {
        float limit = 0.0f;
        if (m_limit.m_x >= limit) {
            limit += m_limit.m_x;
        }
        if (m_limit.m_y >= limit) {
            limit += m_limit.m_y;
        }
        if (limit >= 0.0f) {
            float length = m_speed.length();
            if (length > limit) {
                if (limit <= 0.0f) {
                    m_speed.m_x = m_speed.m_y = 0.0f;
                } else {
                    m_speed.normalize();
                    m_speed.m_x *= limit;
                    m_speed.m_y *= limit;
                }
            }
        }
    } else {
        if (m_limit.m_x >= 0.0f) {
            if ((m_limitFlags & Limit_PlusX) && m_speed.m_x > m_limit.m_x) {
                m_speed.m_x = m_limit.m_x;
            } else if ((m_limitFlags & Limit_MinusX) && m_speed.m_x < -m_limit.m_x) {
                m_speed.m_x = -m_limit.m_x;
            }
        }
        if (m_limit.m_y >= 0.0f) {
            if ((m_limitFlags & Limit_PlusY) && m_speed.m_y > m_limit.m_y) {
                m_speed.m_y = m_limit.m_y;
            } else if ((m_limitFlags & Limit_MinusY) && m_speed.m_y < -m_limit.m_y) {
                m_speed.m_y = -m_limit.m_y;
            }
        }
    }
}

void itKineticEnergy::updateEnergy(soModuleAccesser* moduleAccesser, float rate) {
    if (rate != 1.0f) {
        Vec2f accel;
        Vec2f::copy(accel, m_accel);
        Vec2f brake;
        Vec2f::copy(brake, m_brake);
        m_accel.m_x *= rate;
        m_accel.m_y *= rate;
        m_brake.m_x *= rate;
        m_brake.m_y *= rate;
        updateEnergy(moduleAccesser);
        m_accel = accel;
        m_brake = brake;
    } else {
        updateEnergy(moduleAccesser);
    }
}

void itKineticEnergy::mulSpeed(Vec3f* speed) {
    Vec2f mul = *speed->xy();
    m_speed.m_x *= mul.m_x;
    m_speed.m_y *= mul.m_y;
}

void itKineticEnergy::mulAccel(Vec3f* accel) {
    Vec2f mul = *accel->xy();
    m_accel.m_x *= mul.m_x;
    m_accel.m_y *= mul.m_y;
}

static inline void reflectVec(Vec2f& out, const Vec2f& v, const Vec2f& n) {
    out = v - n * 2.0f * (n.m_x * v.m_x + n.m_y * v.m_y);
}

void itKineticEnergy::reflectSpeed(Vec3f* normal) {
    Vec2f n = *normal->xy();
    reflectVec(m_speed, m_speed, n);
}

void itKineticEnergy::reflectAccel(Vec3f* normal) {
    Vec2f n = *normal->xy();
    reflectVec(m_accel, m_accel, n);
}

// HYPOTHESIS: draft of the 3D energy (the original scales, adds and multiplies the vectors with paired-single
// helpers); itKineticEnergy with a third axis.
void itKineticEnergy3f::updateEnergy(soModuleAccesser* moduleAccesser) {
    Vec3f accel = m_accel;
    Vec3f brake = m_brake;
    if (m_considerGroundFriction || m_isFollowGroundNormal) {
        if (moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
            if (moduleAccesser->getGroundModule().isTouch(0xFF, 0)) {
                if (m_considerGroundFriction == true) {
                    float friction = moduleAccesser->getGroundModule().getDownFriction(0);
                    if (friction != 1.0f) {
                        accel.scale(friction);
                        brake.scale(friction);
                    }
                }
                if (m_isFollowGroundNormal == true) {
                    Vec2f normal;
                    Vec2f::copy(normal, moduleAccesser->getGroundModule().getTouchNormal(8, 0));
                    Vec2f planar(accel.m_x, accel.m_y);
                    Vec2f follow = soKineticUtility::projectionNormalFollow(&planar, &normal);
                    accel.m_x = follow.m_x;
                    accel.m_y = follow.m_y;
                }
            }
        }
    }
    m_speed.m_x += accel.m_x;
    m_speed.m_y += accel.m_y;
    m_speed.m_z += accel.m_z;
    Vec3f stableSpeed = m_stableSpeed;
    if (stableSpeed.m_x < 0.0f) {
        stableSpeed.m_x = 0.0f;
    }
    if (stableSpeed.m_y < 0.0f) {
        stableSpeed.m_y = 0.0f;
    }
    if (m_speed.m_x > stableSpeed.m_x) {
        m_speed.m_x -= brake.m_x;
        if (m_speed.m_x < stableSpeed.m_x) {
            m_speed.m_x = stableSpeed.m_x;
        }
    } else {
        m_speed.m_x += brake.m_x;
        if (m_speed.m_x > stableSpeed.m_x) {
            m_speed.m_x = stableSpeed.m_x;
        }
    }
    if (m_speed.m_y > stableSpeed.m_y) {
        m_speed.m_y -= brake.m_y;
        if (m_speed.m_y < stableSpeed.m_y) {
            m_speed.m_y = stableSpeed.m_y;
        }
    } else {
        m_speed.m_y += brake.m_y;
        if (m_speed.m_y > stableSpeed.m_y) {
            m_speed.m_y = stableSpeed.m_y;
        }
    }
    if (m_speed.m_z > stableSpeed.m_z) {
        m_speed.m_z -= brake.m_z;
        if (m_speed.m_z < stableSpeed.m_z) {
            m_speed.m_z = stableSpeed.m_z;
        }
    } else {
        m_speed.m_z += brake.m_z;
        if (m_speed.m_z > stableSpeed.m_z) {
            m_speed.m_z = stableSpeed.m_z;
        }
    }
    if (m_limitFlags & itKineticEnergy::Limit_Length) {
        float limit = 0.0f;
        if (m_limit.m_x >= limit) {
            limit += m_limit.m_x;
        }
        if (m_limit.m_y >= limit) {
            limit += m_limit.m_y;
        }
        if (m_limit.m_z >= limit) {
            limit += m_limit.m_z;
        }
        if (limit >= 0.0f) {
            float lengthSquared = m_speed.m_x * m_speed.m_x + m_speed.m_y * m_speed.m_y + m_speed.m_z * m_speed.m_z;
            double absolute = __fabs(lengthSquared);
            float magnitude = absolute;
            float length = magnitude <= 1.17549435e-38f ? 0.0f : lengthSquared * rsqrtf(lengthSquared);
            if (length > limit) {
                if (limit <= 0.0f) {
                    m_speed.m_x = m_speed.m_y = m_speed.m_z = 0.0f;
                } else {
                    m_speed.normalize();
                    m_speed.scale(limit);
                }
            }
        }
    } else {
        if (m_limit.m_x >= 0.0f) {
            if ((m_limitFlags & itKineticEnergy::Limit_PlusX) && m_speed.m_x > m_limit.m_x) {
                m_speed.m_x = m_limit.m_x;
            } else if ((m_limitFlags & itKineticEnergy::Limit_MinusX) && m_speed.m_x < -m_limit.m_x) {
                m_speed.m_x = -m_limit.m_x;
            }
        }
        if (m_limit.m_y >= 0.0f) {
            if ((m_limitFlags & itKineticEnergy::Limit_PlusY) && m_speed.m_y > m_limit.m_y) {
                m_speed.m_y = m_limit.m_y;
            } else if ((m_limitFlags & itKineticEnergy::Limit_MinusY) && m_speed.m_y < -m_limit.m_y) {
                m_speed.m_y = -m_limit.m_y;
            }
        }
        if (m_limit.m_z >= 0.0f) {
            if ((m_limitFlags & itKineticEnergy::Limit_PlusZ) && m_speed.m_z > m_limit.m_z) {
                m_speed.m_z = m_limit.m_z;
            } else if ((m_limitFlags & itKineticEnergy::Limit_MinusZ) && m_speed.m_z < -m_limit.m_z) {
                m_speed.m_z = -m_limit.m_z;
            }
        }
    }
}

void itKineticEnergy3f::updateEnergy(soModuleAccesser* moduleAccesser, float rate) {
    if (rate != 1.0f) {
        Vec3f accel = m_accel;
        Vec3f brake = m_brake;
        m_accel.scale(rate);
        m_brake.scale(rate);
        updateEnergy(moduleAccesser);
        m_accel = accel;
        m_brake = brake;
    } else {
        updateEnergy(moduleAccesser);
    }
}

void itKineticEnergy3f::mulSpeed(Vec3f* speed) {
    m_speed.m_x *= speed->m_x;
    m_speed.m_y *= speed->m_y;
    m_speed.m_z *= speed->m_z;
}

void itKineticEnergy3f::mulAccel(Vec3f* accel) {
    m_accel.m_x *= accel->m_x;
    m_accel.m_y *= accel->m_y;
    m_accel.m_z *= accel->m_z;
}

void itKineticEnergy3f::reflectSpeed(Vec3f* normal) {
    float dot = normal->m_x * m_speed.m_x + normal->m_y * m_speed.m_y + normal->m_z * m_speed.m_z;
    m_speed.m_x -= normal->m_x * 2.0f * dot;
    m_speed.m_y -= normal->m_y * 2.0f * dot;
    m_speed.m_z -= normal->m_z * 2.0f * dot;
}

void itKineticEnergy3f::reflectAccel(Vec3f* normal) {
    float dot = normal->m_x * m_accel.m_x + normal->m_y * m_accel.m_y + normal->m_z * m_accel.m_z;
    m_accel.m_x -= normal->m_x * 2.0f * dot;
    m_accel.m_y -= normal->m_y * 2.0f * dot;
    m_accel.m_z -= normal->m_z * 2.0f * dot;
}

void itKineticEnergyGravity::updateEnergy(soModuleAccesser*) {
    m_speed += m_accel * g_soWorld->m_gravityUp;
    float stableSpeed = m_stableSpeed * g_soWorld->m_gravityDown;
    if (stableSpeed < 0.0f) {
        stableSpeed = 0.0f;
    }
    float brake = m_brake;
    if (m_speed > stableSpeed) {
        m_speed -= brake;
        if (m_speed < stableSpeed) {
            m_speed = 0.0f;
        }
    } else {
        m_speed += brake;
        if (m_speed > stableSpeed) {
            m_speed = stableSpeed;
        }
    }
    if (m_limit >= 0.0f) {
        if ((m_limitFlags & itKineticEnergy::Limit_PlusY) && m_speed > m_limit) {
            m_speed = m_limit;
        } else if ((m_limitFlags & itKineticEnergy::Limit_MinusY) && m_speed < -m_limit) {
            m_speed = -m_limit;
        }
    }
}

void itKineticEnergyGravity::updateEnergy(soModuleAccesser* moduleAccesser, float rate) {
    if (rate != 1.0f) {
        float accel = m_accel;
        float brake = m_brake;
        m_accel *= rate;
        m_brake *= rate;
        updateEnergy(moduleAccesser);
        m_accel = accel;
        m_brake = brake;
    } else {
        updateEnergy(moduleAccesser);
    }
}

void itKineticEnergyGravity::mulSpeed(Vec3f* speed) {
    m_speed *= speed->m_y;
}

void itKineticEnergyGravity::mulAccel(Vec3f* accel) {
    m_accel *= accel->m_y;
}

void itKineticEnergyGravity::reflectSpeed(Vec3f* normal) {
    Vec2f n = *normal->xy();
    Vec2f speed(0.0f, m_speed);
    Vec2f reflected = speed - n * 2.0f * (n.m_x * speed.m_x + n.m_y * speed.m_y);
    m_speed = reflected.m_y;
}

void itKineticEnergyGravity::reflectAccel(Vec3f* normal) {
    Vec2f n = *normal->xy();
    Vec2f accel(0.0f, m_accel);
    Vec2f reflected = accel - n * 2.0f * (n.m_x * accel.m_x + n.m_y * accel.m_y);
    m_accel = reflected.m_y;
}
