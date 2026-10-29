#pragma once

// Local shadow of the BrawlHeaders header. ms::TextWriterBase is the game's copy of nw4r::ut::TextWriterBase
// (same tag-processor driven layout loop, line/char spacing, tab width, draw flags) with these changes:
// no width limit or format buffer, a window rectangle with a horizontal offset scale, the per-font
// adjust scale applied to the font metrics, the tag processor's saveEnv/restoreEnv bracketing every
// measurement, and the print loop stopping at the message buffer's delay-print position.
// Member names follow nw4r where the code is the same.

#include <StaticAssert.h>
#include <ms/ms_char_writer.h>
#include <ms/ms_tag_processor.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/ut/ut_Rect.h>
#include <types.h>

struct MsgBuf;
namespace nw4r {
    namespace ut {
        class Font;
    }
}

namespace ms {
    template <typename T>
    class TextWriterBase : public CharWriter {
    public:
        typedef TagProcessorBase<T> TagProcessorType;

        enum DrawFlag {
            DRAWFLAG_ALIGN_TEXT_BASELINE = 0,
            DRAWFLAG_ALIGN_TEXT_CENTER = (1 << 0),
            DRAWFLAG_ALIGN_TEXT_RIGHT = (1 << 1),

            DRAWFLAG_ALIGN_H_BASELINE = 0,
            DRAWFLAG_ALIGN_H_CENTER = (1 << 4),
            DRAWFLAG_ALIGN_H_RIGHT = (1 << 5),

            DRAWFLAG_ALIGN_V_BASELINE = 0,
            DRAWFLAG_ALIGN_V_CENTER = (1 << 8),
            DRAWFLAG_ALIGN_V_TOP = (1 << 9),

            DRAWFLAG_MASK_ALIGN_TEXT = DRAWFLAG_ALIGN_TEXT_BASELINE | DRAWFLAG_ALIGN_TEXT_CENTER | DRAWFLAG_ALIGN_TEXT_RIGHT,
            DRAWFLAG_MASK_ALIGN_H = DRAWFLAG_ALIGN_H_BASELINE | DRAWFLAG_ALIGN_H_CENTER | DRAWFLAG_ALIGN_H_RIGHT,
            DRAWFLAG_MASK_ALIGN_V = DRAWFLAG_ALIGN_V_BASELINE | DRAWFLAG_ALIGN_V_CENTER | DRAWFLAG_ALIGN_V_TOP,

            DRAWFLAG_MASK_ALL = DRAWFLAG_MASK_ALIGN_TEXT | DRAWFLAG_MASK_ALIGN_H | DRAWFLAG_MASK_ALIGN_V,
        };

        TextWriterBase();
        ~TextWriterBase();

        void SetLineHeight(f32 height);
        f32 GetLineHeight() const;
        void SetLineSpace(f32 space);
        void SetCharSpace(f32 space);
        f32 GetCharSpace() const;
        int GetTabWidth() const;
        void SetDrawFlag(u32 flag);
        u32 GetDrawFlag() const;
        void SetTagProcessor(TagProcessorBase<T>* pProcessor);

        f32 CalcStringWidth(const T* pStr, int len) const;
        void CalcStringRect(nw4r::ut::Rect* pRect, const T* pStr, int len) const;
        f32 Print(const T* pStr, int len);
        static u32 GetBufferSize();
        // Called out of line by PrintImpl and AdjustCursor.
        f32 CalcLineWidth(const T* pStr, int len) __attribute__((never_inline));

        // Window rectangle (left, top, right, bottom); defined in ms_message.cpp as an inline helper there.
        void SetWindowRect(f32 l, f32 t, f32 r, f32 b) {
            f32 h = b - t;
            f32 w = r - l;
            m_rect.top = nw4r::math::FSelect(h, t, b);
            m_rect.left = nw4r::math::FSelect(w, l, r);
            m_rect.right = nw4r::math::FSelect(w, r, l);
            m_rect.bottom = nw4r::math::FSelect(h, b, t);
        }

    private:
        bool IsDrawFlagSet(u32 mask, u32 flag) const {
            return (mDrawFlag & mask) == flag;
        }

        // Returns the number of characters consumed from pStr.
        int CalcLineRectImpl(nw4r::ut::Rect* pRect, const T* pStr, int len);
        void CalcStringRectImpl(nw4r::ut::Rect* pRect, const T* pStr, int len);
        f32 PrintImpl(const T* pStr, int len);
        f32 AdjustCursor(f32* pX, f32* pY, const T* pStr, int len, f32* pTextWidth, f32* pTextHeight);

    public:
        int m_70;                           // 0x70, not written by this class
        nw4r::ut::Rect m_rect;              // 0x74, window rectangle
        f32 m_84;                           // 0x84, horizontal alignment offset scale (Message writes it from the window ratio)
        f32 mCharSpace;                     // 0x88
        f32 mLineSpace;                     // 0x8C
        int mTabWidth;                      // 0x90
        u32 mDrawFlag;                      // 0x94
        TagProcessorBase<T>* mTagProcessor; // 0x98
        MsgBuf* m_curBuf;                   // 0x9C, buffer being printed (delay-print limit)

        static u32 mFormatBufferSize;
        static TagProcessorBase<T> mDefaultTagProcessor;
    };
    static_assert(sizeof(TextWriterBase<char>) == 0xA0, "Class is the wrong size!");
} // namespace ms
