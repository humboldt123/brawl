#pragma once
// HYPOTHESIS: the trophy model loader of the Subspace HUD (original object if_figure_loader.o); class and method
// names from the symbol map, layouts from the accesses of this unit (no BrawlHeaders declaration exists).

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <string.h>
#include <types.h>

class MuObject;
struct tyFigListData;
struct tyFigListDataManager;

// HYPOTHESIS: a fixed path string (0x80 characters and the terminator) that is copied by value when appended.
struct IfFigurePath {
    char m_str[0x81];

    IfFigurePath() { }
    IfFigurePath(const char* str) {
        m_str[0] = 0;
        strcat(m_str, str);
    }
    IfFigurePath operator+(const char* str) const {
        IfFigurePath result = *this;
        strcat(result.m_str, str);
        return result;
    }
};

class IfFigureLoader {
public:
    class Entry {
    public:
        u16 m_refCount;                 // 0x00
        u16 m_wait;                     // 0x02 HYPOTHESIS: frames to wait before the read starts
        s32 m_state;                    // 0x04 HYPOTHESIS: 0 idle, 1 read pending, 2 cancel pending
        u32 m_figId;                    // 0x08
        tyFigListData* m_data;          // 0x0C
        nw4r::g3d::ResFile m_resFile;   // 0x10
        gfFileIOHandle m_handle;        // 0x14
        IfFigurePath m_path;            // 0x18 HYPOTHESIS: set to "" and not read in this unit
        void* m_buffer;                 // 0x9C

        Entry();
        ~Entry();
        bool isReady();
        void destroy();
    };
    static_assert(sizeof(Entry) == 0xA0, "Class is the wrong size!");

    // HYPOTHESIS: the reference that readRequest hands out; releasing it calls Entry::destroy.
    struct EntryRef {
        Entry* m_entry;
        EntryRef(Entry* entry) : m_entry(entry) { }
        ~EntryRef() {
            if (m_entry) {
                m_entry->destroy();
            }
            m_entry = NULL;
        }
    };

    tyFigListDataManager* m_listMgr; // 0x00
    s32 m_num;                       // 0x04
    Entry* m_entries;                // 0x08
    s32 m_next;                      // 0x0C

    // HYPOTHESIS: int parameters (the map has no parameter types).
    IfFigureLoader(tyFigListDataManager* listMgr, int num);
    ~IfFigureLoader();
    static IfFigureLoader* create(tyFigListDataManager* listMgr, int num);
    static void destroy();
    static IfFigureLoader* getInstance();
    bool isLoadFinish();
    // HYPOTHESIS: the trophy id is a u32 like the tyFigListDataManager ids.
    EntryRef readRequest(u32 id);
    bool waitForIdle();
    void adjustModel(u32 id, MuObject* model, MuObject* base);
};
static_assert(sizeof(IfFigureLoader) == 0x10, "Class is the wrong size!");
