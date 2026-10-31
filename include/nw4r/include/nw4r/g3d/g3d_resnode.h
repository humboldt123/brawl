#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_rescommon.h>
#include <types.h>

namespace nw4r {
    namespace g3d {

        struct ResNodeData {
        public:
            char _0[8];
            u32 m_nodeNameStrOffset;
            u32 m_nodeIndex;
            u32 m_id;
            union {
                u32 m_flags; // 0x14
                struct {
                    u32 : 23;
                    u32 m_isVisible : 1; // 0x100, toggled by Ground::setNodeVisibility (a u32 field: read as a word)
                    u32 : 8;
                };
            };
            u32 m_billboardMode;
            u32 m_billboardRefNode;
            Vec3f m_scale;
            Vec3f m_rotation;
            Vec3f m_translation;
            Vec3f m_boxMin;
            Vec3f m_boxMax;
            s32 m_toParent; // 0x5C
            s32 m_toChild; // 0x60
            s32 m_toNext; // 0x64
            s32 m_toPrev;
            s32 m_toUserData;
            char _112[96];
        };
        static_assert(sizeof(ResNodeData) == 208, "Class is wrong size!");

        class ResNode : public ResCommon<ResNodeData> {
            /* data */
        public:
            inline ResNode() : ResCommon() {}
            inline ResNode(void* data) : ResCommon(data) {}
            void SetTranslate(float x, float y, float z);
        };

    } // namespace g3d
} // namespace nw4