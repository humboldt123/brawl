#pragma once

#include <havok/hkBase.h>

// Jacobian schema base. Word 0 packs the schema type (top byte, signed) and the schema size (low half).
struct hkJacobianSchema {
    u32 m_tag; // 0x00

    s32 getSchemaType() const;
    u32 getSchemaSize() const;
    template <class T>
    T* hkAddByteOffset(int off) const;
};
