#include <gf/gf_file_io.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_heap_manager.h>
#include <ms/ms_resfont.h>
#include <nw4r/ut/ut_ResFont.h>
#include <nw4r/ut/ut_RomFont.h>
#include <revolution/CX/CXCompression.h>
#include <revolution/OS/OSCache.h>
#include <types.h>

// Font objects, in the order getFontResource's ids are laid out in memory (not in id order).
nw4r::ut::RomFont g_SystemFont;
nw4r::ut::ResFont g_NormalFont;
nw4r::ut::ResFont g_Normal12Font;
nw4r::ut::ResFont g_HiraganaFont;
nw4r::ut::ResFont g_EndingFont;
nw4r::ut::ResFont g_ResetsanFont;
nw4r::ut::ResFont g_FoxFont;
nw4r::ut::ResFont g_MeleeFont;
nw4r::ut::ResFont g_AlertFont;

// Heap buffers the fonts were set from (freed by the remove functions when not NULL).
void* g_HiraganaFontBuf;
void* g_NormalFontBuf;
void* g_Normal12FontBuf;
void* g_EndingFontBuf;
void* g_ResetsanFontBuf;
void* g_FoxFontBuf;
void* g_MeleeFontBuf;
void* g_AlertFontBuf;

bool g_isEndingFontLoaded;
bool g_isResetsanFontLoaded;
bool g_isFoxFontLoaded;
bool g_isAlertFontLoaded;

gfFileIOHandle g_NormalFontHandle;
gfFileIOHandle g_Normal12FontHandle;
gfFileIOHandle g_FontHandle3; // never used by this unit
gfFileIOHandle g_FontHandle4; // never used by this unit

// Asynchronous read state of the normal fonts: the file is read into the tail of the allocation (src) and
// decompressed into its head (dst) once the read completes.
void* g_NormalFontDst;
void* g_NormalFontSrc;
int g_NormalFontSize;
void* g_Normal12FontDst;
void* g_Normal12FontSrc;
int g_Normal12FontSize;

bool FontData::loadEndingFont(void* buffer) {
    bool result = g_EndingFont.SetResource(buffer);
    g_isEndingFontLoaded = true;
    g_EndingFontBuf = NULL;
    return result;
}

bool FontData::removeEndingFont() {
    g_EndingFont.RemoveResource();
    if (g_EndingFontBuf != NULL) {
        gfHeapManager::free(g_EndingFontBuf);
        g_EndingFontBuf = NULL;
    }
    return true;
}

bool FontData::loadResetsanFont(void* buffer) {
    bool result = g_ResetsanFont.SetResource(buffer);
    g_isResetsanFontLoaded = true;
    g_ResetsanFontBuf = NULL;
    return result;
}

bool FontData::removeResetsanFont() {
    g_ResetsanFont.RemoveResource();
    if (g_ResetsanFontBuf != NULL) {
        gfHeapManager::free(g_ResetsanFontBuf);
        g_ResetsanFontBuf = NULL;
    }
    return true;
}

bool FontData::loadFoxFont(void* buffer) {
    bool result = g_FoxFont.SetResource(buffer);
    g_isFoxFontLoaded = true;
    g_FoxFontBuf = NULL;
    return result;
}

bool FontData::removeFoxFont() {
    g_FoxFont.RemoveResource();
    if (g_FoxFontBuf != NULL) {
        gfHeapManager::free(g_FoxFontBuf);
        g_FoxFontBuf = NULL;
    }
    return true;
}

bool FontData::loadAlertFont(void* buffer) {
    bool result = g_AlertFont.SetResource(buffer);
    g_isAlertFontLoaded = true;
    g_AlertFontBuf = NULL;
    return result;
}

bool FontData::loadMeleeFont(void* buffer) {
    bool result = g_MeleeFont.SetResource(buffer);
    g_MeleeFontBuf = NULL;
    return result;
}

bool FontData::removeMeleeFont() {
    g_MeleeFont.RemoveResource();
    if (g_MeleeFontBuf != NULL) {
        gfHeapManager::free(g_MeleeFontBuf);
        g_MeleeFontBuf = NULL;
    }
    return true;
}

// Takes (almost) all of the heap: the compressed file is read into the end of the allocation.
bool FontData::loadNormalFont(Heaps::HeapType heapType) {
    if (g_NormalFontBuf != NULL) {
        return true;
    }

    const char* path = "/system/font/font_latin1.arc";
    g_NormalFontSize = gfHeapManager::getMaxFreeSize(heapType) - 0x80;
    g_NormalFontDst = gfHeapManager::alloc(heapType, g_NormalFontSize);
    g_NormalFontBuf = g_NormalFontDst;
    int fileSize = gfFileIO::getFileSize(path);
    g_NormalFontSrc = static_cast<u8*>(g_NormalFontDst) + g_NormalFontSize - fileSize;
    g_NormalFontHandle.readRequestCached(path, g_NormalFontSrc, 0, 0);
    return true;
}

bool FontData::loadNormalFontWatch(Heaps::HeapType heapType) {
    if (g_NormalFontBuf != NULL) {
        return true;
    }

    const char* path = "/system/font/font_watch.arc";
    g_NormalFontSize = gfHeapManager::getMaxFreeSize(heapType) - 0x80;
    g_NormalFontDst = gfHeapManager::alloc(heapType, g_NormalFontSize);
    g_NormalFontBuf = g_NormalFontDst;
    int fileSize = gfFileIO::getFileSize(path);
    g_NormalFontSrc = static_cast<u8*>(g_NormalFontDst) + g_NormalFontSize - fileSize;
    g_NormalFontHandle.readRequestCached(path, g_NormalFontSrc, 0, 0);
    return true;
}

bool FontData::removeNormalFont() {
    g_NormalFont.RemoveResource();
    if (g_NormalFontBuf != NULL) {
        gfHeapManager::free(g_NormalFontBuf);
        g_NormalFontBuf = NULL;
    }
    return true;
}

bool FontData::loadNormal12Font(Heaps::HeapType heapType) {
    if (g_Normal12FontBuf != NULL) {
        return true;
    }

    const char* path = "/system/font/font_latin12.arc";
    g_Normal12FontSize = gfHeapManager::getMaxFreeSize(heapType) - 0x80;
    g_Normal12FontDst = gfHeapManager::alloc(heapType, g_Normal12FontSize);
    g_Normal12FontBuf = g_Normal12FontDst;
    int fileSize = gfFileIO::getFileSize(path);
    g_Normal12FontSrc = static_cast<u8*>(g_Normal12FontDst) + g_Normal12FontSize - fileSize;
    g_Normal12FontHandle.readRequestCached(path, g_Normal12FontSrc, 0, 0);
    return true;
}

bool FontData::removeNormal12Font() {
    g_Normal12Font.RemoveResource();
    if (g_Normal12FontBuf != NULL) {
        gfHeapManager::free(g_Normal12FontBuf);
        g_Normal12FontBuf = NULL;
    }
    return true;
}

bool FontData::loadHiraganaFont(void* buffer) {
    bool result = g_HiraganaFont.SetResource(buffer);
    g_HiraganaFontBuf = NULL;
    return result;
}

bool FontData::removeHiraganaFont() {
    g_HiraganaFont.RemoveResource();
    if (g_HiraganaFontBuf != NULL) {
        gfHeapManager::free(g_HiraganaFontBuf);
        g_HiraganaFontBuf = NULL;
    }
    return true;
}

// True once the normal font is usable; decompresses it when the read has just completed.
bool FontData::isNormalFontLoadedEx() {
    if (g_NormalFontSrc == NULL) {
        return true;
    }
    if (!g_NormalFontHandle.isReady()) {
        return false;
    }

    CXUncompressLZ(g_NormalFontSrc, g_NormalFontDst);
    DCFlushRange(g_NormalFontDst, g_NormalFontSize);
    g_NormalFont.SetResource(g_NormalFontDst);
    g_NormalFontSrc = NULL;
    return true;
}

bool FontData::isNormal2FontLoadedEx() {
    if (g_Normal12FontSrc == NULL) {
        return true;
    }
    if (!g_Normal12FontHandle.isReady()) {
        return false;
    }

    CXUncompressLZ(g_Normal12FontSrc, g_Normal12FontDst);
    DCFlushRange(g_Normal12FontDst, g_Normal12FontSize);
    g_Normal12Font.SetResource(g_Normal12FontDst);
    g_Normal12FontSrc = NULL;
    return true;
}

nw4r::ut::Font* FontData::getFontResource(int id) {
    switch (id) {
    case FONT_SYSTEM:
        return &g_SystemFont;
    case FONT_NORMAL:
        return &g_NormalFont;
    case FONT_HIRAGANA:
        return &g_HiraganaFont;
    case FONT_ENDING:
        return &g_EndingFont;
    case FONT_MELEE:
        return &g_MeleeFont;
    case FONT_RESETSAN:
        return &g_ResetsanFont;
    case FONT_FOX:
        return &g_FoxFont;
    case FONT_ALERT:
        return &g_AlertFont;
    case FONT_NORMAL2:
        return &g_Normal12Font;
    default:
        return &g_NormalFont;
    }
}
