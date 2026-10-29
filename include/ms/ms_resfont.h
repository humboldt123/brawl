#pragma once

// Local shadow of the BrawlHeaders header: the game's font set (sora/ms/ms_font.cpp). The system font is the
// ROM font; the others are BRFNT resources. The normal fonts are read compressed into the tail of one heap
// allocation and decompressed into its head when the read completes (isNormalFontLoadedEx).

#include <gf/gf_heap_manager.h>
#include <nw4r/ut/ut_Font.h>
#include <types.h>

struct FontData {
    enum FONT_RESOURCE {
        FONT_SYSTEM = 0,
        FONT_NORMAL = 1,
        FONT_HIRAGANA = 2,
        FONT_ENDING = 3,
        FONT_MELEE = 4,
        FONT_RESETSAN = 5,
        FONT_FOX = 6,
        FONT_ALERT = 7,
        FONT_NORMAL2 = 8,
    };

    // Resource fonts set from a buffer the caller owns (the result is SetResource's); the remove functions free
    // the buffer if one was recorded.
    static bool loadEndingFont(void* buffer);
    static bool removeEndingFont();
    static bool loadResetsanFont(void* buffer);
    static bool removeResetsanFont();
    static bool loadFoxFont(void* buffer);
    static bool removeFoxFont();
    static bool loadAlertFont(void* buffer);
    static bool loadMeleeFont(void* buffer);
    static bool removeMeleeFont();
    static bool loadHiraganaFont(void* buffer);
    static bool removeHiraganaFont();

    // Fonts read asynchronously from the disc into the given heap.
    static bool loadNormalFont(Heaps::HeapType heapType);
    static bool loadNormalFontWatch(Heaps::HeapType heapType);
    static bool removeNormalFont();
    static bool loadNormal12Font(Heaps::HeapType heapType);
    static bool removeNormal12Font();
    static bool isNormalFontLoadedEx();
    static bool isNormal2FontLoadedEx();

    static nw4r::ut::Font* getFontResource(int id);
};

// Heap buffers of the normal fonts (NULL while not loaded); the message writers read them to pick the font.
extern void* g_NormalFontBuf;
extern void* g_Normal12FontBuf;
