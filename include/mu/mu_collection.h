#pragma once

// HYPOTHESIS: the collection (album) task (module sora, vtable 0x80457030; the map calls the functions mu_collection.o and
// the unnamed namespace "mu_collection_cpp", but muRecorder is in the same translation unit: see the notes of the
// layout pass). The layout is from the constructor, the destructor, the request functions and processDefault. It
// moved here from if_snap_save.h, which includes this header.

#include <StaticAssert.h>
#include <gf/gf_collection_io.h>
#include <mu/mu_recorder.h>
#include <std/auto_ptr.h>
#include <string.h>
#include <types.h>

class gfCollSummaryCommon;

// The task that lists, reads, writes, copies, moves and erases the files of a collection (the snapshots, replays and
// stage files) on the two drives (the console and the SD card) one request at a time; the menus start a request and
// look at m_busy until it is 0. It is a muRecorder (to check the save data of the NAND first) that owns a gfCollectionIO
// task (m_io) and runs the requests in processDefault with the same kind of coroutine as muRecorder.
class muCollection : public muRecorder {
public:
    // One drive of the collection (m_drives[0]: the console, m_drives[1]: the SD card). The constructor and the
    // destructor are the ones of the array that muCollection::muCollection and ~muCollection construct and destroy.
    class FileInfo {
    public:
        // HYPOTHESIS: field types follow constructor stores and filename-copy accesses.
        bool unk0;          // 0x00 HYPOTHESIS: the drive is there (drive 0: bit 7 of m_flags; drive 1: the SD status)
        bool unk1;          // 0x01 HYPOTHESIS: the drive can be written
        u16 unk2;
        s32 m_state;        // 0x04 HYPOTHESIS: 0 and 1 fine, 2 and 3 the error messages of the media
        u32 m_count;        // 0x08 the number of the file names in m_names
        u32 m_free;         // 0x0C HYPOTHESIS: free blocks (from lbl_80423690 of gf_collection_io.cpp)
        muRecorder::Data m_names;   // 0x10 the names of the files, one after the other, each ends with a 0 (freed with it)

        FileInfo();
        ~FileInfo();        // fn_800C37CC: frees m_names (the deleting destructor)
        // fn_800C2C00: the names sorted (std::sort with StringLess, strcmp), each cut to 0x40 characters, in one new block
        // that the caller owns; 0 when there are no names.
        char* copyFileName(Heaps::HeapType heap) const;
    };
    static_assert(sizeof(FileInfo) == 0x14, "Class is the wrong size!");

    // A file name of 0x41 bytes made by copying and adding to it.
    class FileHandle {
    public:
        // HYPOTHESIS: the collection filename occupies 0x41 bytes.
        char m_name[0x41];

        FileHandle() {
            m_name[0] = 0;
            strcat(m_name, "");
        }
        FileHandle(const char* name);                   // 0x800C34D8
        // fn_800C3538 = __ct__Q212muCollection10FileHandleFPCci: the index-th name of the names that FileInfo holds.
        FileHandle(const char* names, int index);
    };

    // HYPOTHESIS: a plain 0x41 byte file name, copied around as a structure.
    struct FileName {
        char m_name[0x41];

        FileName() {
            m_name[0] = 0;
            strcat(m_name, "");
        }
    };

    // HYPOTHESIS: a request key: a drive and a file name.
    struct FileKey {
        s32 m_drive;
        char m_name[0x41];
    };

    // The summary of the file that was read (the picture of a snapshot, the data of a replay or a stage) in
    // m_summary. HYPOTHESIS: the names follow the destructor, processDefault (State_ReadSummaryDone) and decodeJpeg.
    class Summary {
    public:
        // The decoded picture: the pixels (given back with gfHeapManager::free), the size. The same layout as
        // muSnapShotJpeg of mu_snap_shot.h.
        struct Decoded {
            muRecorder::Data m_pixels;  // 0x00
            u32 m_width;                // 0x04
            u32 m_height;               // 0x08

            Decoded& operator=(const Decoded& other) {
                Decoded& source = const_cast<Decoded&>(other);
                void* pixels = source.m_pixels.unk0;
                source.m_pixels.unk0 = 0;
                m_pixels.reset(pixels);
                m_width = other.m_width;
                m_height = other.m_height;
                return *this;
            }
        };

        Decoded m_decoded;                              // 0x00 (0x1EC) made by decodeJpeg
        std::auto_ptr<gfCollSummaryCommon> m_info;      // 0x0C (0x1F8) the object of the io (gfCollSummary*), deleted with it
        void* m_jpegInfo;                               // 0x10 (0x1FC) the picture inside m_info (+0x54 or +0x78)
        u32 m_size;                                     // 0x14 (0x200) the size of the summary (io->unkE8)

        Summary() {
            m_info.reset(0);
            m_decoded.m_pixels.reset(0);
            m_jpegInfo = 0;
            m_size = 0;
        }

        // fn_800C7324 (0xEC bytes): hidden result, decodes the jpeg of m_jpegInfo into a block of `heap`
        Decoded decodeJpeg(Heaps::HeapType heap, int unk) const;
    };
    static_assert(sizeof(Summary) == 0x18, "Class is the wrong size!");

    // The kinds of the request in m_busy (non-zero while a request runs; processDefault sets it back to 0).
    enum Busy {
        Busy_None = 0,
        Busy_Locked = 1,        // lock()/unlock(): the caller uses m_io
        Busy_Scan = 2,          // scan(), scanAll()
        Busy_CheckLimit = 3,    // checkLimit()
        Busy_Write = 4,         // write(), overwriteIf1000()
        Busy_Read = 5,          // read()
        Busy_Erase = 6,         // erase()
        Busy_Copy = 7,          // copy()
        Busy_Move = 8,          // move()
        Busy_ReadSummary = 9,   // readSummary()
    };

    // The states of processDefault (see muScriptState in mu_recorder.h; the values are the lines of the source). Names are
    // HYPOTHESIS. "Ret" is a state that only returns to the caller. The subroutines get a locals frame
    // (State_Idle is the top; the argument frames are in the notes of the layout pass).
    enum State {
        State_Init = 0,
        State_Start = 0x3E0,            // go to State_Idle
        State_WaitWnd = 0x3E5,          // wait until m_wnd->m_step is 3
        State_WaitIo = 0x3ED,           // wait until m_io->m_isReady
        State_ResetIo = 0x3F7,          // wait for m_request == 0 and m_io->isDropReady(), exit m_io, go to State_NewIo
        State_NewIo = 0x403,            // new gfCollectionIO(m_heap2); drives[0].unk0/unk1 = m_flag7
        State_ListFiles = 0x416,        // (sd, drive) requestListSD/VF, wait
        State_ListFilesDone = 0x424,    // the names of the io into m_drives[drive]
        State_ScanVf = 0x452,           // ResetIo, then State_ScanVfList
        State_ScanVfList = 0x454,
        State_ScanVfRet = 0x457,
        State_ScanSd = 0x45C,           // checkSDInserted, window 0x36 or ListFiles(sd)
        State_ScanSdWndRet = 0x466,
        State_ScanSdListed = 0x46D,     // window 0x39 when the SD card has no files
        State_ScanSdWndRet2 = 0x473,
        State_MediaWnd = 0x47C,         // window 0x34/0x35 (console) or 0x3F/0x40 (SD card) about free blocks
        State_MediaWndRet = 0x48A,
        State_MediaWndRet2 = 0x493,
        State_ListCount = 0x49A,        // requestListCountSD/VF
        State_ListCountDone = 0x49F,    // 1000 files or more: window with the text 0x42, 0x43 and "%d"
        State_ListCountFullRet = 0x4AE,
        State_ReadSummary = 0x4B9,      // requestSummarySD/VF
        State_ReadSummaryDone = 0x4BF,  // fills m_summary
        State_NewFile = 0x517,          // count, new name, list, check the name, save
        State_NewFileCount = 0x522,
        State_NewFileName = 0x52D,      // requestNewFileName; the name into m_fileName
        State_NewFileList = 0x538,
        State_NewFileCheck = 0x547,     // strcmp with all the names; window 0x41
        State_NewFileSave = 0x566,      // requestSave
        State_NewFileSaved = 0x573,
        State_Write = 0x585,            // ResetIo first
        State_WriteIo = 0x58C,
        State_WriteReport = 0x597,      // windows with the text 0x2C... and "%d"
        State_WriteReportRet = 0x5A6,
        State_WriteReportRet2 = 0x5AB,
        State_Erase = 0x5B1,
        State_EraseIo = 0x5B8,          // requestDeleteVF
        State_EraseDone = 0x5C0,
        State_EraseRet = 0x5C8,
        State_ReportMedia = 0x5D2,      // SD problems (gf state in lbl_80423690): windows 0x36, 0x37, 0x38, 0x50
        State_ReportMediaRet = 0x5E3,
        State_ReportMediaRet2 = 0x5EC,
        State_ReportMediaNoSpace = 0x5F7,
        State_ReportMediaRet3 = 0x606,
        State_LoadFile = 0x613,         // requestLoadSD/VF
        State_LoadFileDone = 0x61A,     // the data into m_loadData
        State_Read = 0x645,
        State_ReadIo = 0x64C,
        State_ReadRet = 0x653,
        State_Copy = 0x659,
        State_CopyNewFile = 0x65D,
        State_CopyReport = 0x667,       // window 0x3D
        State_CopyReportRet = 0x672,
        State_CopyReportRet2 = 0x677,
        State_EraseSd = 0x67D,
        State_EraseSdIo = 0x682,        // requestDeleteSD
        State_EraseSdReport = 0x689,    // window 0x3E
        State_EraseSdReportRet = 0x690,
        State_EraseSdReportRet2 = 0x697,
        State_LoadForCopy = 0x69F,
        State_LoadForCopyDone = 0x6A9,  // nteSection check/own state
        State_LoadForCopyRet = 0x6CB,
        State_LoadForCopyRet2 = 0x6D1,
        State_ReadAll = 0x6E5,
        State_ReadAllLoaded = 0x6EC,
        State_ReadAllRet = 0x6F3,
        State_MoveAll = 0x6F9,
        State_MoveAllLoaded = 0x701,
        State_MoveAllCopied = 0x709,
        State_MoveAllErased = 0x712,
        State_MoveAllRet = 0x718,
        State_Idle = 0x723,             // dispatches on m_busy
        State_ScanVfRet2 = 0x72B,
        State_ScanSdRet2 = 0x72E,
        State_CheckLimitDone = 0x737,
        State_CheckLimitRet = 0x73F,
        State_WriteNewRet = 0x748,
        State_WriteRet = 0x74D,
        State_ReadDone = 0x756,
        State_EraseVfRet = 0x75E,
        State_EraseSdRet3 = 0x763,
        State_CopyDone = 0x76A,
        State_MoveDone = 0x770,
        State_ReadSummaryRet = 0x776,
    };

    // HYPOTHESIS: the constructor of the script state (stack pointer, then state) is part of the one of muCollection.
    struct Script : muScriptState<64> {
        Script() {
            m_sp = m_stack;
            m_state = 0;
        }
    };

    // HYPOTHESIS: argument types and order from if_snap_save.cpp and the registers; the constructor of muRecorder gets
    // the kind 0 -> 0, 1 -> 2, 2 -> 3, 3 -> 4, otherwise 0; m_fileType is the kind itself.
    // fn_800C35CC = __ct__12muCollectionFiQ34nw4r3g3d7ResFileQ34nw4r3g3d7ResFilePvPviQ25Heaps8HeapTypeQ25Heaps8HeapTypeUcUc
    muCollection(int kind, nw4r::g3d::ResFile modelRes, nw4r::g3d::ResFile msgRes, void* msgData, void* scnRoot,
                 int drawPriority, Heaps::HeapType heap, Heaps::HeapType heap2, u8 unk0, u8 unk2);
    virtual ~muCollection();            // fn_800C383C (vtable 0x80457030)
    virtual void processDefault();      // fn_800C3A24, 0x33EC bytes; calls muRecorder::processDefault first

    // HYPOTHESIS: the io task is held in an object that exits it when it is destroyed (inline in ~muCollection).
    class IoPtr {
    public:
        IoPtr(gfCollectionIO* ptr) : m_ptr(ptr) {}
        ~IoPtr() {
            if (m_ptr) {
                static_cast<gfTask*>(m_ptr)->exit();
            }
            m_ptr = 0;
        }
        IoPtr& operator=(gfCollectionIO* ptr) {
            m_ptr = ptr;
            return *this;
        }
        operator gfCollectionIO*() const { return m_ptr; }
        gfCollectionIO* operator->() const { return m_ptr; }
    private:
        gfCollectionIO* m_ptr;
    };

    IoPtr m_io;                         // 0x110 the io task (a gfTask 4 bytes into the object; exit() when replaced)
    bool m_ioPending;                   // 0x114 set when a request was handed to the io
    u8 m_pad115[3];
    int m_fileType;                     // 0x118 the kind of the files (gfCollectionIO file type: 1, 2, 3 the same as the constructor argument)
    FileHandle m_fileName;              // 0x11C the name of the new file (made by requestNewFileName)
    FileName m_readName;                // 0x15D the file of read/erase/copy/move
    u8 m_pad19E[2];
    muRecorder::Data m_loadData;        // 0x1A0 what LoadFile read (freed with gfHeapManager::free)
    u32 m_loadSize;                     // 0x1A4
    FileName m_summaryName;             // 0x1A8 the file of readSummary
    u8 m_pad1E9[3];
    Summary m_summary;                  // 0x1EC the summary that readSummary got
    FileName m_keyName;                 // 0x204 the file name of overwriteIf1000
    u8 m_pad245[3];
    Script m_script;                    // 0x248 state, 0x24C stack[64], 0x34C stack pointer
    s32 m_busy;                         // 0x350 a Busy: non-zero while a request runs
    s32 m_result;                       // 0x354 HYPOTHESIS: how the last request ended (0 fine; 2 failed; 3, 5 more errors)
    FileInfo m_drives[2];               // 0x358 (the constructor builds the array with __construct_array(..., 0x14, 2))

    // The arguments of the request that is running (the request functions below fill it; the dispatcher in
    // State_Idle copies the ones it needs into the locals frame of the callee). The m_arg* names are the ones that the
    // draft of the request functions in mu_recorder.cpp uses; they are the same words. HYPOTHESIS: the dispatcher shows
    // that write and overwriteIf1000 really take (drive, data, size) - word 0x384 is the drive (0: console, else SD
    // card), 0x38C the data and 0x390 its size - but the draft types them (void* data, int size, int unk), which gives
    // the same code, and symbols.txt has the names of that draft (write__12muCollectionFPvii and
    // overwriteIf1000__12muCollectionFQ212muCollection7FileKeyii; the other signature would be
    // write__12muCollectionFiPvi and overwriteIf1000__12muCollectionFQ212muCollection7FileKeyPvi).
    union {
        u8 unk380[0x14];
        struct {                        // Busy_Scan
            bool m_scanDrive0;          // 0x380
            bool m_scanDrive1;          // 0x381
            u8 m_scanFlagA : 1;         // 0x382 HYPOTHESIS: list the files
            u8 m_scanFlagB : 1;         // HYPOTHESIS: show a window when the drive does not work
            u8 m_scanFlagRest : 6;
        };
        struct {                        // the others
            union {
                u8 m_writeNamed;        // 0x380 write: 0 (make a new name), overwriteIf1000: 1 (use m_writeName)
                u8 m_argFlag;
                s32 m_reqDrive;         // 0x380 the drive of checkLimit, read, erase, copy, move, readSummary
                s32 m_argDrive;
            };
            union {
                s32 m_writeDrive;       // 0x384 write, overwriteIf1000: the drive (0: console, else SD card)
                s32 m_argId;
                char* m_reqName;        // 0x384 read, erase, copy, move, readSummary: the file name
                void* m_argData;
            };
            char* m_writeName;          // 0x388 write: 0; overwriteIf1000: m_keyName
            union {
                void* m_writeData;      // 0x38C write, overwriteIf1000: the data
                s32 m_argSize;
            };
            union {
                s32 m_writeSize;        // 0x390 write, overwriteIf1000: the size of the data
                s32 m_argUnk;
            };
        };
    };

    void* lock();                       // 0x800C6E10: m_busy = Busy_Locked, returns the io
    void unlock();                      // fn_800C6E20: m_busy = 0
    bool isDropReady();                 // fn_800C39D8: m_busy == 0 && m_io->isDropReady()
    void checkLimit(int drive);
    void scanAll(int unk, int unk2);
    void scan(int drive, int unk, int unk2);
    void write(void* data, int size, int unk);
    void overwriteIf1000(FileKey key, int size, int unk);
    void read(int drive, FileName name);
    void erase(int drive, FileName name);
    void erase(const FileKey& key);
    void copy(int drive, FileName name);
    void move(int drive, FileName name);
    void readSummary(int drive, FileName name);
};
static_assert(sizeof(muCollection) == 0x394, "Class is the wrong size!");

// The old name of muCollection::FileInfo (if_snap_save.cpp reads the drives through it).
typedef muCollection::FileInfo muCollectionDrive;
