#include <ms/ms_tag_processor.h>
#include <ms/ms_text_writer_base.h>
#include <sora/ms_message.h>
#include <string.h>
#include <types.h>

msTagProcessor::msTagProcessor() { }

// A fresh stack entry is opaque white.
msTagProcessor::tagStack::tagStack() : m_color(0xFFFFFFFF) { }

msTagProcessor::tagStack::~tagStack() { }

msTagProcessor::~msTagProcessor() { }

int msTagProcessor::strlenUtf8(const char* str, int len) {
    int count = 0;
    const char* end = str + len;
    while (str < end) {
        u8 c = *str;
        if ((c & 0x80) == 0) {
            if (static_cast<s8>(c) == 0) {
                break;
            }
            count++;
            str += 1;
        } else if ((c & 0xE0) == 0xC0) {
            count++;
            str += 2;
        } else if ((c & 0xF0) == 0xE0) {
            count++;
            str += 3;
        }
    }
    return count;
}

int MsgParseChar(const u8* p, int* kind, u8* out, int* count) {
    const u8* cur = p;
    int numChars = 0;

    s8 c;
    while ((c = *reinterpret_cast<const s8*>(cur)) != 0) {
        if (*cur < ' ') {
            // Control commands: the operand sizes are those of the Message put* writers (ms_message.cpp).
            switch (c) {
            case '\n':
                cur += 1;
                if (kind == NULL) {
                    continue;
                }
                *kind = 2;
                goto done;
            case 0x10:
                cur += 3;
                break;
            case 0x02:
                cur += 2;
                break;
            case 0x0B:
                cur = cur + cur[1] + cur[2] + 3;
                break;
            case 0x1D:
                cur += 2;
                break;
            case 0x1C:
                cur += 3;
                break;
            case 0x12:
                cur += 2;
                break;
            case 0x13:
                cur += 1;
                break;
            case 0x11:
                cur += 5;
                break;
            case 0x07:
                cur += 2;
                break;
            case 0x08:
                cur += 2;
                break;
            case 0x04:
            case 0x0C:
                cur += 5;
                break;
            case 0x03:
                cur += 3;
                break;
            case 0x1B:
                cur += 2;
                break;
            case 0x0D:
                cur += 3;
                break;
            case 0x0E:
                cur += 2;
                break;
            case 0x0F:
                cur += 3;
                break;
            case 0x14: {
                // One halfword per set flag bit.
                u8 flags = cur[1];
                cur += 2;
                if (flags & 1) {
                    cur += 2;
                }
                if (flags & 2) {
                    cur += 2;
                }
                if (flags & 4) {
                    cur += 2;
                }
                break;
            }
            case 0x16: {
                // One word per set flag bit.
                u8 flags = cur[1];
                cur += 2;
                if (flags & 1) {
                    cur += 4;
                }
                if (flags & 2) {
                    cur += 4;
                }
                if (flags & 4) {
                    cur += 4;
                }
                break;
            }
            default:
                // Commands the caller interprets itself (window, color, projection, ...) end the walk.
                goto done;
            }
        } else {
            numChars++;
            if (kind != NULL) {
                *kind = 1;
                u8 lead = *cur;
                if ((lead & 0x80) == 0) {
                    out[0] = lead;
                    if (cur[0] == '.' && cur[1] == '.' && cur[2] == '.' && cur[3] == '.') {
                        cur += 4;
                        out[3] = '.';
                        out[2] = '.';
                        out[1] = '.';
                    } else {
                        cur += 1;
                        out[1] = 0;
                    }
                } else if ((lead & 0xE0) == 0xC0) {
                    out[0] = lead;
                    out[1] = cur[1];
                    cur += 2;
                    out[2] = 0;
                } else if ((lead & 0xF0) == 0xE0) {
                    out[0] = lead;
                    out[1] = cur[1];
                    out[2] = cur[2];
                    cur += 3;
                    out[3] = 0;
                }
                goto done;
            }

            if ((c & 0x80) == 0) {
                cur += 1;
            } else if ((c & 0xE0) == 0xC0) {
                cur += 2;
            } else if ((c & 0xF0) == 0xE0) {
                cur += 3;
            }
        }
    }

done:
    if (count != NULL) {
        *count = numChars;
    }
    return cur - p;
}

bool msTagProcessor::pushFontTag(ContextType* pCtx, u8 flags) {
    if (m_stackCount >= 8) {
        m_stackCount = 7;
        for (int i = 1; i < 8; i++) {
            m_stack[i - 1] = m_stack[i];
        }
    }

    tagStack* entry = &m_stack[m_stackCount];
    ms::TextWriterBase<char>* writer = pCtx->writer;
    entry->m_flags = flags;
    if (flags & 2) {
        entry->m_color = writer->GetTextColor();
    }
    if (flags & 1) {
        entry->m_scaleH = writer->GetScaleH();
        entry->m_scaleV = writer->GetScaleV();
    }
    if (flags & 0x80) {
        unk4 = 1;
    }
    m_stackCount++;
    return true;
}

bool msTagProcessor::popFontTag(ContextType* pCtx) {
    if (m_stackCount < 1) {
        return false;
    }
    m_stackCount--;
    if (unk4 != 0) {
        return true;
    }

    tagStack* entry = &m_stack[m_stackCount];
    ms::TextWriterBase<char>* writer = pCtx->writer;
    u8 flags = entry->m_flags;
    if (flags & 2) {
        writer->SetTextColor(entry->m_color);
    }
    if (flags & 1) {
        writer->SetScale(entry->m_scaleH, entry->m_scaleV);
        if (pCtx->writer->IsWidthFixed()) {
            pCtx->writer->SetFixedWidth(32.0f * entry->m_scaleH);
        }
    }
    return true;
}

u8* floatToShortBytes(u8* out, float f) {
    union {
        s16 value;
        u8 bytes[2];
    } u;

    u.value = f;
    out[0] = u.bytes[0];
    out[1] = u.bytes[1];
    return out;
}

u8* floatToBytes(u8* out, float f) {
    union {
        float value;
        u8 bytes[4];
    } u;

    u.value = f;
    out[0] = u.bytes[0];
    out[1] = u.bytes[1];
    out[2] = u.bytes[2];
    out[3] = u.bytes[3];
    return out;
}

void msTagProcessor::saveEnv(ContextType* pCtx) {
    m_savedStackCount = m_stackCount;
    memcpy(m_savedStack, m_stack, m_stackCount * sizeof(tagStack));
}

void msTagProcessor::restoreEnv(ContextType* pCtx) {
    m_stackCount = m_savedStackCount;
    memcpy(m_stack, m_savedStack, m_stackCount * sizeof(tagStack));
}

namespace ms {

template <typename T>
void TagProcessorBase<T>::ProcessTab(ContextType* pCtx) {
    TextWriterBase<T>& rWriter = *pCtx->writer;

    int tabWidth = rWriter.GetTabWidth();
    if (tabWidth <= 0) {
        return;
    }

    f32 charWidth = rWriter.IsWidthFixed() ? rWriter.GetFixedWidth() : rWriter.GetFontWidth();

    f32 dx = rWriter.GetCursorX() - pCtx->x;
    f32 tabPixel = tabWidth * charWidth;
    int numTab = static_cast<int>(dx / tabPixel) + 1;
    f32 x = pCtx->x + (tabPixel * numTab);

    rWriter.SetCursorX(x);
}

template <typename T>
void TagProcessorBase<T>::ProcessLinefeed(ContextType* pCtx) {
    TextWriterBase<T>& rWriter = *pCtx->writer;

    f32 x = pCtx->x;
    f32 y = rWriter.GetCursorY() + rWriter.GetLineHeight();

    rWriter.SetCursor(x, y);
}

template <typename T>
typename TagProcessorBase<T>::Operation TagProcessorBase<T>::Process(u16 ch, ContextType* pCtx) {
    switch (ch) {
    case '\n':
        ProcessLinefeed(pCtx);
        return OPERATION_NEXT_LINE;

    case '\t':
        ProcessTab(pCtx);
        return OPERATION_NO_CHAR_SPACE;
    }

    return OPERATION_DEFAULT;
}

template <typename T>
typename TagProcessorBase<T>::Operation TagProcessorBase<T>::CalcRect(nw4r::ut::Rect* pRect, u16 ch, ContextType* pCtx) {
    switch (ch) {
    case '\n': {
        const TextWriterBase<T>& rWriter = *pCtx->writer;
        pRect->right = rWriter.GetCursorX();
        pRect->top = rWriter.GetCursorY();
        ProcessLinefeed(pCtx);
        pRect->left = rWriter.GetCursorX();
        pRect->bottom = rWriter.GetCursorY() + pCtx->writer->GetFontHeight();
        pRect->Normalize();
        return OPERATION_NEXT_LINE;
    }

    case '\t': {
        const TextWriterBase<T>& rWriter = *pCtx->writer;
        pRect->left = rWriter.GetCursorX();
        ProcessTab(pCtx);
        pRect->right = rWriter.GetCursorX();
        pRect->top = rWriter.GetCursorY();
        pRect->bottom = pRect->top + rWriter.GetFontHeight();
        pRect->Normalize();
        return OPERATION_NO_CHAR_SPACE;
    }
    }

    return OPERATION_DEFAULT;
}

template <typename T>
TagProcessorBase<T>::TagProcessorBase() { }

template <typename T>
TagProcessorBase<T>::~TagProcessorBase() { }

template <typename T>
void TagProcessorBase<T>::saveEnv(ContextType* pCtx) {}

template <typename T>
void TagProcessorBase<T>::restoreEnv(ContextType* pCtx) {}

template TagProcessorBase<char>::TagProcessorBase();
template TagProcessorBase<char>::~TagProcessorBase();
template TagProcessorBase<char>::Operation TagProcessorBase<char>::Process(u16, ContextType*);
template TagProcessorBase<char>::Operation TagProcessorBase<char>::CalcRect(nw4r::ut::Rect*, u16, ContextType*);
template void TagProcessorBase<char>::saveEnv(ContextType*);
template void TagProcessorBase<char>::restoreEnv(ContextType*);

} // namespace ms
