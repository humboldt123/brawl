#pragma once

#include <StaticAssert.h>
#include <nw4r/ut/ut_Color.h>
#include <nw4r/ut/ut_Rect.h>
#include <types.h>

namespace ms {
    template <class T>
    class TextWriterBase;

    // The game's copy of nw4r::ut::PrintContext with one extra float before the flags: the text writer fills it
    // with the text block height computed by its cursor adjustment (ms_text_writer.cpp).
    template <typename T>
    struct PrintContext {
        TextWriterBase<T>* writer; // 0x0
        const T* str;              // 0x4
        f32 x;                     // 0x8
        f32 y;                     // 0xc
        f32 textHeight;            // 0x10, HYPOTHESIS: only the writer stores it
        u32 flags;                 // 0x14

        enum { FLAGS_CHARSPACE = (1 << 0) };
    };

    template <typename T>
    class TagProcessorBase {
    public:
        typedef PrintContext<T> ContextType;

        enum Operation {
            OPERATION_DEFAULT,
            OPERATION_NO_CHAR_SPACE,
            OPERATION_CHAR_SPACE,
            OPERATION_NEXT_LINE,
            OPERATION_END_DRAW
        };

        // Called out of line by the derived constructor/destructor.
        TagProcessorBase() __attribute__((never_inline));
        virtual ~TagProcessorBase() __attribute__((never_inline));

        virtual Operation Process(u16 ch, ContextType* pCtx);
        virtual Operation CalcRect(nw4r::ut::Rect* pRect, u16 ch, ContextType* pCtx);
        // The text writer brackets every measurement and print with these, passing its context.
        virtual void saveEnv(ContextType* pCtx);
        virtual void restoreEnv(ContextType* pCtx);

    private:
        void ProcessTab(ContextType* pCtx);
        void ProcessLinefeed(ContextType* pCtx);
    };
} // namespace ms

// The tag processor class Message carries at +0xa4: a stack of saved font states (color, scale, flags) for
// the message tag commands, with a second copy for saveEnv/restoreEnv.
class msTagProcessor : public ms::TagProcessorBase<char> {
public:
    struct tagStack {
        nw4r::ut::Color m_color; // 0x0
        f32 m_scaleH;            // 0x4
        f32 m_scaleV;            // 0x8
        u8 m_flags;              // 0xc

        tagStack();
        ~tagStack();
    };

    msTagProcessor();
    virtual ~msTagProcessor();
    virtual Operation Process(u16 ch, ContextType* pCtx);
    virtual Operation CalcRect(nw4r::ut::Rect* pRect, u16 ch, ContextType* pCtx);
    virtual void saveEnv(ContextType* pCtx);
    virtual void restoreEnv(ContextType* pCtx);

    // Number of UTF-8 characters in the first len bytes of str (stops at a NUL).
    // HYPOTHESIS: parameter types; the map names it a member of this class.
    static int strlenUtf8(const char* str, int len);

    // HYPOTHESIS: the flag byte is a u8 (the caller passes an unsigned byte of the tag string).
    bool pushFontTag(ContextType* pCtx, u8 flags);
    bool popFontTag(ContextType* pCtx);

    u8 unk4;
    s32 m_stackCount;            // 0x8
    tagStack m_stack[8];         // 0xc
    s32 m_savedStackCount;       // 0x8c
    tagStack m_savedStack[8];    // 0x90
};
static_assert(sizeof(msTagProcessor::tagStack) == 0x10, "msTagProcessor::tagStack size");
static_assert(sizeof(msTagProcessor) == 0x110, "msTagProcessor size");
