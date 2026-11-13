#pragma once

#include <StaticAssert.h>
#include <sr/sr_common.h>
#include <types.h>

struct NWC24MsgObj;
struct NWC24DlTask;

// The interface that nteWiiConnect24 tells its errors to (the name is the one of its RTTI in sora_menu_main).
// HYPOTHESIS: the parameters follow how muProcOptWifi::notify_Error uses them: the kind of the error (0: none), a word
// that the screen does not read, and the error code that the screen shows.
class IWiiConnect24_Error {
public:
    virtual void notify_Error(int kind, int unk, int code) = 0;
};

// The use of WiiConnect24 (the mail and the download tasks of the Wii) by the game; the methods and the file name
// (nt_etc_nwc24.o) come from the map of the debug build.
// HYPOTHESIS: the names of the fields follow the constructor and the destructor.
class nteWiiConnect24 {
public:
    bool m_opened;                      // 0x00 the library was opened by this object (the destructor closes it)
    Heaps::HeapType m_heap;             // 0x04
    IWiiConnect24_Error* m_errorHandler; // 0x08
    void* m_buffer;                     // 0x0C freed by the destructor
    int m_bufferSize;                   // 0x10 HYPOTHESIS: cleared with m_buffer

    nteWiiConnect24(Heaps::HeapType heap);
    ~nteWiiConnect24();
    void setErrorHandler(IWiiConnect24_Error* handler);
    // HYPOTHESIS: the return types; the callers test them against 1.
    bool check_getStatus(int kind);
    u64 getMyWiiID();
    bool friend_makeList();
    u16* friend_getName(u32 index);
    u64 friend_getWiiID(u32 index);
    // HYPOTHESIS: count and message-pointer array follow the commit loop.
    bool sendMails(u32 count, NWC24MsgObj** messages);
    void startSearchMail(u32 appId);
    bool getNextMailForMe(NWC24MsgObj* message, u32* remaining);
    bool _setupDlTask(NWC24DlTask* task, u16 index, int* error);
    int _openLibrary(int kind);
    int _procGeneralError(int error);
    bool dl_isRegisterDLTask();
    bool dl_registerDLTask();
    bool dl_deleteDLTask();
    bool dl_deleteDLTaskAll();
    u32 _getMailSentSizeToday();
    u8 getLangId();
};
static_assert(sizeof(nteWiiConnect24) == 0x14, "Class is wrong size!");
