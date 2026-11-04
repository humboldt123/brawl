#include <mt/mt_vector.h>
#include <types.h>

void Vec3f::normalize() {
    // MATCH-ONLY: the squared terms are summed in this order to match the original schedule.
    f32 invLength = rsqrtf(m_z * m_z + (m_x * m_x + m_y * m_y));
    m_x *= invLength;
    m_y *= invLength;
    m_z *= invLength;
}


void Vec2f::normalize() {
    f32 invLength = rsqrtf(m_x * m_x + m_y * m_y);
    m_x *= invLength;
    m_y *= invLength;
}
