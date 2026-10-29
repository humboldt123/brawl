#include <ms/ms_text_writer_base.h>
#include <nw4r/ut/ut_CharStrmReader.h>
#include <nw4r/ut/ut_Font.h>
#include <nw4r/ut/ut_algorithm.h>
#include <sora/ms_message.h>
#include <types.h>

// The game's copy of nw4r::ut::TextWriterBase (src/nw4r/ut/ut_TextWriterBase.cpp). Differences from nw4r are
// commented at each function; the shared logic keeps nw4r's names.

namespace ms {

    template <typename T>
    u32 TextWriterBase<T>::mFormatBufferSize = 256;

    template <typename T>
    TagProcessorBase<T> TextWriterBase<T>::mDefaultTagProcessor;

    template <typename T>
    TextWriterBase<T>::TextWriterBase()
        : m_rect(0.0f, 0.0f, 640.0f, 480.0f)
        , mCharSpace(0.0f)
        , mLineSpace(0.0f)
        , mTabWidth(4)
        , mDrawFlag(0)
        , mTagProcessor(&mDefaultTagProcessor)
        , m_curBuf(NULL) { }

    template <typename T>
    TextWriterBase<T>::~TextWriterBase() { }

    // nw4r has no setter: the line space is chosen so that GetLineHeight() returns the given height.
    template <typename T>
    void TextWriterBase<T>::SetLineHeight(f32 height) {
        const nw4r::ut::Font* pFont = GetFont();
        int lf = pFont != NULL ? (int)(pFont->GetLineFeed() * getAdjustFontScale()) : 0;
        mLineSpace = height - lf * GetScaleV();
    }

    // nw4r: mLineSpace + GetScaleV() * lf, with the per-font adjust scale applied to the line feed here.
    template <typename T>
    f32 TextWriterBase<T>::GetLineHeight() const {
        const nw4r::ut::Font* pFont = GetFont();
        int lf = pFont != NULL ? (int)(pFont->GetLineFeed() * getAdjustFontScale()) : 0;
        return mLineSpace + lf * GetScaleV();
    }

    template <typename T>
    void TextWriterBase<T>::SetLineSpace(f32 space) {
        mLineSpace = space;
    }

    template <typename T>
    void TextWriterBase<T>::SetCharSpace(f32 space) {
        mCharSpace = space;
    }

    template <typename T>
    f32 TextWriterBase<T>::GetCharSpace() const {
        return mCharSpace;
    }

    template <typename T>
    int TextWriterBase<T>::GetTabWidth() const {
        return mTabWidth;
    }

    template <typename T>
    void TextWriterBase<T>::SetDrawFlag(u32 flag) {
        mDrawFlag = flag;
    }

    template <typename T>
    u32 TextWriterBase<T>::GetDrawFlag() const {
        return mDrawFlag;
    }

    template <typename T>
    void TextWriterBase<T>::SetTagProcessor(TagProcessorBase<T>* pProcessor) {
        mTagProcessor = pProcessor;
    }

    // nw4r measures through CalcStringRect; here the tag processor's environment is saved around the measurement
    // (the tag stack is restored afterwards, so measuring does not change the colors/scales of the real print).
    template <typename T>
    f32 TextWriterBase<T>::CalcStringWidth(const T* pStr, int len) const {
        nw4r::ut::Rect rect;
        CalcStringRect(&rect, pStr, len);
        return rect.GetWidth();
    }

    template <typename T>
    void TextWriterBase<T>::CalcStringRect(nw4r::ut::Rect* pRect, const T* pStr, int len) const {
        TextWriterBase<T> clone(*this);
        PrintContext<T> context = { &clone, pStr, 0.0f, 0.0f, 0.0f, 0 };

        mTagProcessor->saveEnv(&context);
        clone.CalcStringRectImpl(pRect, pStr, len);
        mTagProcessor->restoreEnv(&context);
    }

    // nw4r prints through a clone and moves the cursor afterwards; here the writer itself prints.
    template <typename T>
    f32 TextWriterBase<T>::Print(const T* pStr, int len) {
        return PrintImpl(pStr, len);
    }

    template <typename T>
    u32 TextWriterBase<T>::GetBufferSize() {
        return mFormatBufferSize;
    }

    template <typename T>
    f32 TextWriterBase<T>::CalcLineWidth(const T* pStr, int len) {
        nw4r::ut::Rect rect;
        TextWriterBase<T> clone(*this);
        PrintContext<T> context = { this, pStr, 0.0f, 0.0f, 0.0f, 0 };

        mTagProcessor->saveEnv(&context);
        clone.SetCursor(0.0f, 0.0f);
        clone.CalcLineRectImpl(&rect, pStr, len);
        mTagProcessor->restoreEnv(&context);

        f32 width = rect.GetWidth();
        return width;
    }

    // nw4r returns whether the width limit was hit and advances *ppStr; this version has no width limit and
    // returns the number of characters consumed (the whole remaining length when the tag processor ends the draw).
    template <typename T>
    int TextWriterBase<T>::CalcLineRectImpl(nw4r::ut::Rect* pRect, const T* pStr, int len) {
        PrintContext<T> context = { this, pStr, 0.0f, 0.0f, 0.0f, 0 };

        const nw4r::ut::Font* pFont = GetFont();
        f32 x = 0.0f;
        bool charSpace = false;
        nw4r::ut::CharStrmReader reader = pFont->GetCharStrmReader();

        pRect->left = 0.0f;
        pRect->right = 0.0f;
        pRect->top = nw4r::ut::Min(0.0f, GetLineHeight());
        pRect->bottom = nw4r::ut::Max(0.0f, GetLineHeight());

        reader.Set(pStr);

        u16 ch = reader.Next();

        while (static_cast<const T*>(reader.GetCurrentPos()) - pStr <= len) {
            if (ch < ' ') {
                nw4r::ut::Rect r(x, 0.0f, 0.0f, 0.0f);
                context.str = static_cast<const T*>(reader.GetCurrentPos());
                context.flags = charSpace ? 0 : PrintContext<T>::FLAGS_CHARSPACE;

                SetCursorX(x);

                typename TagProcessorType::Operation oper = mTagProcessor->CalcRect(&r, ch, &context);

                reader.Set(context.str);

                pRect->left = nw4r::ut::Min(pRect->left, r.left);
                pRect->top = nw4r::ut::Min(pRect->top, r.top);
                pRect->right = nw4r::ut::Max(pRect->right, r.right);
                pRect->bottom = nw4r::ut::Max(pRect->bottom, r.bottom);

                x = GetCursorX();

                if (oper == TagProcessorType::OPERATION_END_DRAW) {
                    mTagProcessor->restoreEnv(&context);
                    return len;
                }

                if (oper == TagProcessorType::OPERATION_NO_CHAR_SPACE) {
                    charSpace = false;
                } else if (oper == TagProcessorType::OPERATION_CHAR_SPACE) {
                    charSpace = true;
                } else if (oper == TagProcessorType::OPERATION_NEXT_LINE) {
                    break;
                }
            } else {
                if (charSpace) {
                    x += GetCharSpace();
                }

                charSpace = true;

                if (IsWidthFixed()) {
                    x += GetFixedWidth();
                } else {
                    x += GetScaleH() * (GetFont()->GetCharWidth(ch) * getAdjustFontScale());
                }

                pRect->left = nw4r::ut::Min(pRect->left, x);
                pRect->right = nw4r::ut::Max(pRect->right, x);
            }

            ch = reader.Next();
        }

        return static_cast<const T*>(reader.GetCurrentPos()) - pStr;
    }

    template <typename T>
    void TextWriterBase<T>::CalcStringRectImpl(nw4r::ut::Rect* pRect, const T* pStr, int len) {
        int remain = len;

        pRect->left = 0.0f;
        pRect->right = 0.0f;
        pRect->top = 0.0f;
        pRect->bottom = 0.0f;

        SetCursor(0.0f, 0.0f);

        do {
            nw4r::ut::Rect r;
            int consumed = CalcLineRectImpl(&r, pStr, remain);
            pStr += consumed;
            remain -= consumed;

            pRect->left = nw4r::ut::Min(pRect->left, r.left);
            pRect->top = nw4r::ut::Min(pRect->top, r.top);
            pRect->right = nw4r::ut::Max(pRect->right, r.right);
            pRect->bottom = nw4r::ut::Max(pRect->bottom, r.bottom);
        } while (remain > 0);
    }

    // Differences from nw4r: no width limit, the horizontal alignment offsets are scaled by m_84, the vertical
    // block alignment is applied per glyph against the window rectangle, every glyph is offset by the window
    // origin, and glyphs past the message buffer's delay-print position are skipped.
    template <typename T>
    f32 TextWriterBase<T>::PrintImpl(const T* pStr, int len) {
        f32 cursorX = GetCursorX();
        f32 cursorY = GetCursorY();

        f32 cursorXAdj = 0.0f;
        f32 cursorYAdj = 0.0f;

        f32 orgCursorX = cursorX;
        f32 orgCursorY = cursorY;

        f32 textWidth = 0.0f;
        f32 textHeight = 0.0f;

        bool charSpace = false;

        AdjustCursor(&cursorX, &cursorY, pStr, len, &textWidth, &textHeight);

        cursorXAdj = orgCursorX - GetCursorX();
        cursorYAdj = orgCursorY - GetCursorY();

        PrintContext<T> context = { this, pStr, cursorX, cursorY, textHeight, 0 };

        nw4r::ut::CharStrmReader reader = GetFont()->GetCharStrmReader();
        reader.Set(pStr);

        typename TagProcessorType::Operation oper;
        u16 ch = reader.Next();

        while (static_cast<const T*>(reader.GetCurrentPos()) - pStr <= len) {
            if (ch < ' ') {
                context.str = static_cast<const T*>(reader.GetCurrentPos());
                context.flags = charSpace ? 0 : PrintContext<T>::FLAGS_CHARSPACE;

                oper = mTagProcessor->Process(ch, &context);
                // HYPOTHESIS: the line width of the alignment cases below; it is only measured when still zero
                // (the compiler keeps that test as a self-comparison of the zero constant).
                f32 width = 0.0f;
                if (oper == TagProcessorType::OPERATION_NEXT_LINE) {
                    if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_TEXT, DRAWFLAG_ALIGN_TEXT_CENTER)) {
                        if (width == 0.0f) {
                            int remain = len - (context.str - pStr);
                            width = CalcLineWidth(context.str, remain);
                        }
                        f32 offset = (textWidth - width) / 2.0f;
                        SetCursorX(context.x + offset * m_84);
                    } else if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_TEXT, DRAWFLAG_ALIGN_TEXT_RIGHT)) {
                        if (width == 0.0f) {
                            int remain = len - (context.str - pStr);
                            width = CalcLineWidth(context.str, remain);
                        }
                        f32 offset = textWidth - width;
                        SetCursorX(context.x + offset * m_84);
                    } else {
                        if (width == 0.0f) {
                            width = GetCursorX() - context.x;
                            textWidth = nw4r::ut::Max(textWidth, width);
                        }
                        SetCursorX(context.x);
                    }

                    charSpace = false;
                } else if (oper == TagProcessorType::OPERATION_NO_CHAR_SPACE) {
                    charSpace = false;
                } else if (oper == TagProcessorType::OPERATION_CHAR_SPACE) {
                    charSpace = true;
                } else if (oper == TagProcessorType::OPERATION_END_DRAW) {
                    break;
                }

                reader.Set(context.str);
            } else {
                f32 baseY = GetCursorY();

                if (charSpace) {
                    MoveCursorX(GetCharSpace());
                }

                charSpace = true;

                const nw4r::ut::Font* pFont = GetFont();
                f32 adj = GetScaleV() * (-pFont->GetBaselinePos() * getAdjustFontScale());
                MoveCursorY(adj);

                if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_V, DRAWFLAG_ALIGN_V_CENTER)) {
                    MoveCursorY((m_rect.bottom - m_rect.top - textHeight) / 2.0f);
                }
                if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_V, DRAWFLAG_ALIGN_V_TOP)) {
                    MoveCursorY(m_rect.bottom - m_rect.top - textHeight);
                }

                MoveCursorX(m_rect.left);
                MoveCursorY(m_rect.top);

                MsgBuf* pBuf = m_curBuf;
                if (pBuf->m_rect[0] != 0) {
                    if (static_cast<const u8*>(reader.GetCurrentPos()) <= pBuf->m_14) {
                        CharWriter::Print(ch);
                    }
                } else {
                    CharWriter::Print(ch);
                }

                MoveCursorX(-m_rect.left);
                SetCursorY(baseY);
            }

            ch = reader.Next();
        }

        if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_V, DRAWFLAG_ALIGN_V_CENTER) ||
            IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_V, DRAWFLAG_ALIGN_V_TOP)) {
            SetCursorY(orgCursorY);
        } else {
            MoveCursorY(cursorYAdj);
        }

        return textWidth;
    }

    // Differences from nw4r: the measurement goes through the tag processor's saved environment, there is no
    // width limit, the vertical block alignment is left to PrintImpl, the text alignment offsets are scaled by
    // m_84, and the text block size is also returned through the two out parameters.
    template <typename T>
    f32 TextWriterBase<T>::AdjustCursor(f32* pX, f32* pY, const T* pStr, int len, f32* pTextWidth, f32* pTextHeight) {
        f32 textWidth = 0.0f;
        f32 textHeight = 0.0f;

        if (!IsDrawFlagSet(DRAWFLAG_MASK_ALL, DRAWFLAG_MASK_ALIGN_V) && !IsDrawFlagSet(DRAWFLAG_MASK_ALL, 0)) {
            nw4r::ut::Rect rect;
            CalcStringRect(&rect, pStr, len);

            textWidth = rect.left + rect.right;
            textHeight = rect.top + rect.bottom;
        }

        if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_H, DRAWFLAG_ALIGN_H_CENTER)) {
            *pX -= textWidth / 2.0f;
        } else if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_H, DRAWFLAG_ALIGN_H_RIGHT)) {
            *pX -= textWidth;
        }

        if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_TEXT, DRAWFLAG_ALIGN_TEXT_CENTER)) {
            f32 offset = (textWidth - CalcLineWidth(pStr, len)) / 2.0f;
            SetCursorX(*pX + offset * m_84);
        } else if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_TEXT, DRAWFLAG_ALIGN_TEXT_RIGHT)) {
            f32 offset = textWidth - CalcLineWidth(pStr, len);
            SetCursorX(*pX + offset * m_84);
        } else {
            SetCursorX(*pX);
        }

        if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_V, DRAWFLAG_ALIGN_V_CENTER | DRAWFLAG_ALIGN_V_TOP)) {
            SetCursorY(*pY);
        } else {
            SetCursorY(*pY + GetFontAscent());
        }

        *pTextWidth = textWidth;
        *pTextHeight = textHeight;
        return textWidth;
    }

    template class TextWriterBase<char>;

} // namespace ms
