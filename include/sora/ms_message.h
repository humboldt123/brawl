#pragma once

// Local description of the menu text writer (class Message) used by MuMsg and ms_message.cpp.
// This intentionally does not include the BrawlHeaders ms_message.h: that header lacks the
// real layout and several members that the game code needs.

#include <ms/ms_char_writer.h>
#include <ms/ms_tag_processor.h>
#include <nw4r/ut/ut_Color.h>
#include <nw4r/ut/ut_Rect.h>
#include <sr/sr_common.h>
#include <stdarg.h>
#include <types.h>

namespace nw4r {
    namespace g3d {
        class ScnMdlSimple;
        class ScnObj;
    } // namespace g3d
} // namespace nw4r

#include <ms/ms_resfont.h>

class Message;

#include <nw4r/g3d/g3d_scnobjcallback.h>

// Draw callback registered on a ScnMdlSimple so that a message buffer is drawn with the model.
class msScnObjCallback : public nw4r::g3d::IScnObjCallback {
public:
    int m_idx;                // 0x04, index of the message buffer to draw (-1 = base buffer)
    float m_scale;            // 0x08
    Message* m_msg;           // 0x0c
    u8 m_zCompare;            // 0x10
    int m_zUpdate;            // 0x14
    msScnObjCallback* m_next; // 0x18, previously registered callback
    void* m_scnMdl;           // 0x1c
    int m_nodeId;             // 0x20
    u8 m_enable;              // 0x24

    msScnObjCallback()
        : m_idx(0)
        , m_scale(1.0f)
        , m_msg(0)
        , m_zCompare(1)
        , m_zUpdate(3)
        , m_next(0)
        , m_enable(1) {}
    virtual ~msScnObjCallback() {}
    virtual void ExecCallback_DRAW_XLU(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* object, u32 param, void* info);
};

// One command buffer for a message (0x54 bytes).
struct MsgBuf {
    int m_rect[4];         // 0x00, delay-print parameters (16.16)
    int m_10;              // 0x10
    u8* m_14;              // 0x14
    u8 m_18[4];            // 0x18
    u8 m_1c;               // 0x1c
    u8 m_1d;               // 0x1d
    u8 m_1e[2];            // 0x1e
    msScnObjCallback m_cb; // 0x20
    int m_size;            // 0x48
    int m_pos;             // 0x4c
    u8* m_data;            // 0x50

    MsgBuf(int size, Heaps::HeapType heap)
        : m_pos(0) {
        m_data = new (heap) u8[size];
        m_size = size;
        m_rect[0] = 0;
        m_rect[1] = 0;
        m_rect[2] = 0;
        m_rect[3] = 0;
        m_10 = 0;
        m_18[0] = 0;
        m_1c = 0;
        m_1d = 0;
        m_14 = 0;
    }
    ~MsgBuf() { delete[] m_data; }
};

u8* floatToBytes(u8* out, float f);
// Walks the message command stream from p. With kind == NULL it skips the control commands and counts the
// printable characters until the end of the string or an unknown command. With kind it stops after the first
// line feed (kind 2) or printable character (kind 1, its UTF-8 bytes copied to out, "...." copied whole).
// Returns the number of bytes consumed; count receives the number of printable characters when given.
int MsgParseChar(const u8* p, int* kind, u8* out, int* count);
int strncmp_(const char* a, const char* b, unsigned long n);
void SetFontResource(ms::CharWriter* w, int id);
void ScnObj_EnableCallbackTiming(nw4r::g3d::ScnObj* obj, u32 timing);
void ScnObj_EnableCallbackExecOp(nw4r::g3d::ScnObj* obj, u32 op);
void ScnObj_DisableCallbackTiming(nw4r::g3d::ScnObj* obj, u32 timing);
void ScnObj_DisableCallbackExecOp(nw4r::g3d::ScnObj* obj, u32 op);
void G3DState_Invalidate(u32 flags);
u8* floatToShortBytes(u8* out, float f);

#include <ms/ms_text_writer_base.h>

class Message : public ms::TextWriterBase<char> {
public:
    virtual ~Message();

    msTagProcessor m_tagProc; // 0xa4
    GXColor m_1b4;        // 0x1b4
    float m_1b8;
    float m_1bc;
    int m_1c0;
    u8 m_1c4;
    void (*m_1c8)();
    void (*m_1cc)();
    MsgBuf* m_cur;   // 0x1d0
    MsgBuf* m_base;  // 0x1d4
    MsgBuf** m_bufs; // 0x1d8
    int m_numBufs;   // 0x1dc

    Message(u32, Heaps::HeapType heapType);

    bool allocMsgBuf(u32 msgSizes, u32 numMsgs, Heaps::HeapType heapType);
    bool attachMsgBuf(u32 index, nw4r::g3d::ScnMdlSimple* sceneModel, const char* nodeName, u8, int, float);
    bool changeMsgBuf(int index);
    void clearMsgBuf();
    void printMsgBuf(FontData::FONT_RESOURCE fontId);
    void setWindow(float, float, float, float);
    void setCallProjection(u8 p1);
    void setDrawFlag(u32 p1, u32 p2);
    void setFace(u8);
    void setFixedWidth(float);
    void setColor(int);
    void setScale(float);
    void setScale(float, float);
    void setCursorX(float);
    void setCursorY(float);
    void printf(const char* format, ...);

    // Not in the symbol map under a name; named from behaviour.
    void put(u8 b) {
        MsgBuf* buf = m_cur;
        buf->m_data[buf->m_pos++] = b;
    }
    void putBytes(const u8* p, int n) {
        MsgBuf* buf = m_cur;
        for (int i = 0; i < n; i++) {
            buf->m_data[buf->m_pos++] = p[i];
        }
    }
    void setWindowRect(u32 color, u8 width);
    void setWidthModeAuto(u8 v);
    void setCharSpace(float f1, float f2);
    void setEdge(const GXColor& color, float width);
    void setColor(const GXColor& color);
    void setColor2(const GXColor& color);
    void setColor2(int color);
    void setLineHeight(float h);
    void setSpace(float s);
    void setDelayPrint(float f1, float f2, float f3, float f4);
    void detachMsgBuf(nw4r::g3d::ScnMdlSimple* sceneModel);
    void vprintf(const char* format, va_list args);
    void write(const void* data, int len);
    void writeString(const char* str);
    u8 getTag(char** tagOut);
    u8 isEndDelayPrint();
    const u8* advanceDelayPrint(const u8* p);
    void init(bool, int);
    void writeIndexData(void* msgbin, u32 index);
    void setBufField34(int index, int value);
    void getPrintRect(float* rect, const u8* data);

    static u32 utf8to16(wchar_t* dst, const char* src);
    static u32 utf16to8(char* dst, const wchar_t* src);
    static void getPrintIndexData(void* msgbin, u32 index, char** outStr, u32* outLen);
    static void drawBoxLine(s32 color, s32 lineWidth, s32 zTest, float x1, float y1, float x2, float y2, float z);
    static int writeTagClear(char* dst);
    static bool appendSubstr(char* dst, const char* src, int skip, int count);
    static char* fullToHalf(char* dst, const char* src);
    static int halfToFull(char* dst, const char* src);
    static int stripTags(const u8* src, int len, int unused, u8* dst);
};
