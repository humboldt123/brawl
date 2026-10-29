#ifndef GAMESPY_GHTTP_GHTTPCONNECTION_H
#define GAMESPY_GHTTP_GHTTPCONNECTION_H

// GameSpy SDK HTTP (ghttpConnection.h, ghttpBuffer.h, ghttpPost.h,
// ghttpEncryption.h). Field order and names follow the SDK; fields whose name
// is not evidenced by the matched code are unkN. The functions are unnamed in
// our symbols; the SDK names are noted next to the placeholders.
#include <types.h>
#include <GameSpy/common/darray.h>
#include <GameSpy/common/gsXML.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int GHTTPBool;
typedef int GHTTPRequest;
typedef int GHTTPResult;
typedef int GHTTPState;
typedef int GHTTPByteCount;
typedef int GHTTPEncryptionEngine;

typedef struct GHIConnection GHIConnection;
typedef struct GHIEncryptor GHIEncryptor;

typedef void (*ghttpProgressCallback)(GHTTPRequest request, GHTTPState state, const char* buffer, GHTTPByteCount bufferLen,
                                      GHTTPByteCount bytesReceived, GHTTPByteCount totalSize, void* param);
typedef GHTTPBool (*ghttpCompletedCallback)(GHTTPRequest request, GHTTPResult result, char* buffer, GHTTPByteCount bufferLen, void* param);

typedef int (*GHTTPEncryptorInitFunc)(GHIConnection* connection, GHIEncryptor* encryptor);
typedef int (*GHTTPEncryptorCleanupFunc)(GHIConnection* connection, GHIEncryptor* encryptor);
typedef int (*GHTTPEncryptorEncryptFunc)(GHIConnection* connection, GHIEncryptor* encryptor, const char* plainText, int plainTextLength,
                                         char* encryptedBuffer, int* encryptedLength);
typedef int (*GHTTPEncryptorDecryptFunc)(GHIConnection* connection, GHIEncryptor* encryptor, const char* encryptedData, int* encryptedLength,
                                         char* decryptedBuffer, int* decryptedLength);

typedef enum GHIEncryptionResult {
    GHIEncryptionResult_None,
    GHIEncryptionResult_Success,
    GHIEncryptionResult_BufferTooSmall,
    GHIEncryptionResult_Error
} GHIEncryptionResult;

struct GHIEncryptor {
    void* mInterface;                         // 0x0
    GHTTPEncryptionEngine mEngine;            // 0x4
    GHTTPBool mInitialized;                   // 0x8
    GHTTPBool mSessionEstablished;            // 0xC
    GHTTPEncryptorInitFunc mInitFunc;         // 0x10
    GHTTPEncryptorCleanupFunc mCleanupFunc;   // 0x14
    GHTTPEncryptorEncryptFunc mEncryptFunc;   // 0x18
    GHTTPEncryptorDecryptFunc mDecryptFunc;   // 0x1C
};

typedef struct GHIBuffer {
    GHIConnection* connection;  // 0x0
    char* data;                 // 0x4
    int size;                   // 0x8
    int len;                    // 0xC
    int pos;                    // 0x10
    int sizeIncrement;          // 0x14
    GHTTPBool fixed;            // 0x18
    GHTTPBool dontFree;         // 0x1C
    GHTTPBool readOnly;         // 0x20
} GHIBuffer;

typedef struct GHIPost {
    DArray data;            // 0x0
    void* callback;         // 0x4
    void* param;            // 0x8
    GHTTPBool hasFiles;     // 0xC
    GHTTPBool hasSoap;      // 0x10
    GHTTPBool useDIME;      // 0x14
    GHTTPBool autoFree;     // 0x18
} GHIPost;

typedef enum GHIPostDataType {
    GHIString,
    GHIFileDisk,
    GHIFileMemory,
    GHIXmlData
} GHIPostDataType;

typedef struct GHIPostData {
    GHIPostDataType type;   // 0x0
    char* name;             // 0x4
    union {
        char* string;                                                   // 0x8
        struct { char* filename; char* reportFilename; char* contentType; } fileDisk;     // 0x8
        struct { const char* data; int len; char* reportFilename; char* contentType; } fileMemory; // 0x8
        GSXmlStreamWriter xml;                                          // 0x8
    } data;
} GHIPostData;

// the progress of one post data entry while it is sent
typedef struct GHIPostDataState {
    GHIPostData* data;      // 0x0
    int pos;                // 0x4
    void* file;             // 0x8
    int len;                // 0xC
} GHIPostDataState;

struct GHIConnection {
    GHTTPBool inUse;                            // 0x0
    GHTTPRequest request;                       // 0x4
    int uniqueID;                               // 0x8
    int type;                                   // 0xC
    GHTTPState state;                           // 0x10
    char* URL;                                  // 0x14
    char* serverAddress;                        // 0x18
    u32 serverIP;                               // 0x1C
    u16 serverPort;                             // 0x20
    char* requestPath;                          // 0x24
    int protocol;                               // 0x28
    char* sendHeaders;                          // 0x2C
    void* saveFile;                             // 0x30
    GHTTPBool blocking;                         // 0x34
    GHTTPBool persistConnection;                // 0x38
    GHTTPResult result;                         // 0x3C
    ghttpProgressCallback progressCallback;     // 0x40
    ghttpCompletedCallback completedCallback;   // 0x44
    void* callbackParam;                        // 0x48
    int socket;                                 // 0x4C
    int socketError;                            // 0x50
    GHIBuffer sendBuffer;                       // 0x54
    GHIBuffer encodeBuffer;                     // 0x78
    GHIBuffer recvBuffer;                       // 0x9C
    GHIBuffer decodeBuffer;                     // 0xC0
    GHIBuffer getFileBuffer;                    // 0xE4
    GHTTPBool userBufferSupplied;               // 0x108
    int statusMajorVersion;                     // 0x10C
    int statusMinorVersion;                     // 0x110
    int statusCode;                             // 0x114
    int statusStringIndex;                      // 0x118
    int headerStringIndex;                      // 0x11C
    GHTTPBool completed;                        // 0x120
    GHTTPByteCount fileBytesReceived;           // 0x124
    GHTTPByteCount totalSize;                   // 0x128
    char* redirectURL;                          // 0x12C
    int redirectCount;                          // 0x130
    GHTTPBool chunkedTransfer;                  // 0x134
    char chunkHeader[11];                       // 0x138
    int chunkHeaderLen;                         // 0x144
    int chunkBytesLeft;                         // 0x148
    int chunkReadingState;                      // 0x14C
    GHTTPBool processing;                       // 0x150
    GHTTPBool connectionClosed;                 // 0x154
    GHTTPBool throttle;                         // 0x158
    u32 lastThrottleRecv;                       // 0x15C
    GHIPost* post;                              // 0x160
    int postingStateIndex;                      // 0x164
    int unk168;
    GHTTPByteCount postBytesSent;               // 0x16C
    int unk170;
    int unk174;
    int unk178;
    GHTTPBool postWaitForUserResponse;          // 0x17C
    GHTTPBool postComplete;                     // 0x180
    int unk184;
    char* proxyOverrideServer;                  // 0x188
    u16 proxyOverridePort;                      // 0x18C
    GHIEncryptor encryptor;                     // 0x190
    void* handleThread;                         // 0x1B0
};

__static_assert(sizeof(GHIBuffer) == 0x24, "GHIBuffer");
__static_assert(sizeof(GHIPost) == 0x1C, "GHIPost");
__static_assert(sizeof(GHIPostData) == 0x18, "GHIPostData");
__static_assert(sizeof(GHIEncryptor) == 0x20, "GHIEncryptor");
__static_assert(offsetof(GHIConnection, encryptor) == 0x190, "GHIConnection.encryptor");

// ghttp globals
extern u16 lbl_805A1060;            // ghiProxyPort
extern char* lbl_805A1064;          // ghiProxyAddress
extern int lbl_805A106C;            // ghiNumConnections
extern int lbl_805A1070;            // ghiConnectionsLen
extern GHIConnection** lbl_805A1074;  // ghiConnections
extern int lbl_805A1078;            // ghiReferenceCount
extern int lbl_8059FA50;            // ghiThrottleBufferSize
extern int lbl_8059FA54;            // ghiThrottleTimeDelay
extern char lbl_8059FA40[3];        // ": "
extern char lbl_8059FA44[3];        // CRLF
extern char lbl_8059FA48[3];        // "%d"
extern char lbl_804905F8[9];        // "https://"
extern u8 lbl_80490604[0x31];       // SSL MAC pad 1
extern u8 lbl_80490638[0x31];       // SSL MAC pad 2
extern u8 lbl_8059FA6C[4];          // SSL record content type

// platform helpers
char* fn_8035D654(const char* s);                          // goastrdup
u32 fn_8035C620(const char* name);                         // inet_addr
int fn_8035D364(const char* hostname, void** handle);      // gsiStartResolvingHostname
int fn_8035C384(int s, const void* addr, int len);         // connect
int fn_8035CD04(int s, int blocking);                      // SetSockBlocking
void SetReceiveBufferSize(int s, int size);                         // SetReceiveBufferSize

// MSL stdio stand-ins of this build
int fn_803F524C(void* buffer, int size, int count, void* file);  // fread
int fn_803F5CE0(void* file);                                  // ftell
int fn_803F5EA8(void* file, long offset, int origin);         // fseek
void fn_803F5EAC(void* file);                                 // rewind

// md5 and rc4
void fn_80361680(void* rc4, const u8* src, u8* dst, int len);  // RC4Encrypt

GHTTPBool fn_8037B37C(GHIBuffer* buffer, int sizeIncrement);                                          // ghiResizeBuffer
GHTTPBool fn_8037B3FC(GHIConnection* connection, GHIBuffer* buffer, int initialSize, int sizeIncrement); // ghiInitBuffer
GHTTPBool fn_8037B4F8(GHIConnection* connection, GHIBuffer* buffer, char* userBuffer, int userBufferLen); // ghiInitFixedBuffer
GHTTPBool fn_8037B644(GHIBuffer* buffer, const char* data, int dataLen);                              // ghiAppendDataToBuffer
GHTTPBool fn_8037B7B4(GHIBuffer* buffer, const char* data, int dataLen);                              // ghiEncodeDataIntoBuffer
GHTTPBool fn_8037B98C(GHIBuffer* buffer, const char* name, const char* value);                        // ghiAppendHeaderToBuffer
GHTTPBool fn_8037BB6C(GHIBuffer* buffer, int i);                                                     // ghiAppendIntToBuffer
void fn_8037BBB8(GHIBuffer* buffer);                                                                 // ghiResetBuffer
GHTTPBool fn_8037BBDC(GHIConnection* connection);                                                    // ghiSendBufferedData
GHTTPBool fn_8037BCD0(GHIBuffer* buffer, char* data, int len);                                       // ghiReadDataFromBuffer
void fn_8037BD4C(GHIConnection* connection);                                                         // ghiCallCompletedCallback
void fn_8037BDD4(GHIConnection* connection, const char* buffer, GHTTPByteCount bufferLen);            // ghiCallProgressCallback
void fn_8037BE0C(GHIConnection* connection);                                                         // ghiCallPostCallback
void fn_8037BE6C(void);                                                                              // ghiCreateLock
void fn_8037BE74(void);                                                                              // ghiLock
void fn_8037BE78(void);                                                                              // ghiUnlock
GHTTPBool fn_8037BE7C(GHIConnection* connection);                                                    // ghiDecryptReceivedData
int fn_8037C104(GHIConnection* connection, const char* buffer, int len);                             // ghiTrySendThenBuffer
int fn_8037C1C0(GHIConnection* connection, const char* buffer, int len);                             // ghiSendPostData
GHIConnection* fn_8037C3E8(void);                                                                    // ghiNewConnection
GHTTPBool fn_8037C5AC(GHIConnection* connection);                                                    // ghiFreeConnection
GHIConnection* fn_8037C720(GHTTPRequest request);                                                    // ghiRequestToConnection
void fn_8037C790(GHTTPBool (*callback)(GHIConnection*));                                             // ghiEnumConnections
void fn_8037C81C(GHIConnection* connection);                                                         // ghiRedirectConnection
GHTTPBool fn_8037CA28(GHTTPRequest request, GHTTPEncryptionEngine engine);                           // ghttpSetRequestEncryptionEngine
int fn_8037D0D8(GHIConnection* connection, GHIEncryptor* encryptor);                                 // ghiEncryptorSslInitFunc
int fn_8037D2AC(GHIConnection* connection, GHIEncryptor* encryptor);                                 // ghiEncryptorSslCleanupFunc
int fn_8037D304(GHIConnection* connection, GHIEncryptor* encryptor, const char* plainText, int plainTextLength,
                char* encryptedBuffer, int* encryptedLength);                                        // ghiEncryptorSslEncryptFunc
int fn_8037D520(GHIConnection* connection, GHIEncryptor* encryptor, const char* encryptedData, int* encryptedLength,
                char* decryptedBuffer, int* decryptedLength);                                        // ghiEncryptorSslDecryptFunc
void fn_8037E7F4(GHIConnection* connection);                                                         // ghiHandleComplete
GHTTPBool fn_8037E8B4(GHIConnection* connection);                                                    // ghiProcessConnection
void fn_8037EA50(void);                                                                              // ghttpStartup
GHTTPRequest fn_8037EAFC(const char* URL, const char* headers, char* buffer, int bufferSize, GHIPost* post, GHTTPBool throttle,
                         GHTTPBool blocking, ghttpProgressCallback progressCallback, ghttpCompletedCallback completedCallback, void* param);  // ghttpGetEx
GHTTPRequest fn_8037ED14(const char* URL, GHIPost* post, GHTTPBool blocking, ghttpCompletedCallback completedCallback, void* param);  // ghttpPost
GHTTPRequest fn_8037ED38(const char* URL, const char* headers, GHIPost* post, GHTTPBool throttle, GHTTPBool blocking,
                         ghttpProgressCallback progressCallback, ghttpCompletedCallback completedCallback, void* param);  // ghttpPostEx
void fn_8037EEDC(void);                                                                              // ghttpThink
GHTTPBool fn_8037EEE8(GHTTPRequest request);                                                         // ghttpRequestThink
void fn_8037EF20(GHTTPRequest request);                                                              // ghttpCancelRequest
void fn_8037EFB0(GHTTPRequest request, int value);                                                   // sets connection unk184
GHIPost* fn_8037EFE8(void);                                                                          // ghttpNewPost
void fn_8037EFEC(void* post, GHTTPBool autoFree);                                                    // ghttpPostSetAutoFree
void fn_8037EFFC(GHIPost* post);                                                                     // ghttpFreePost
void fn_8037F074(void* post, GSXmlStreamWriter xml);                                                 // ghttpPostAddXml
void fn_8037F078(void* elem);                                                                        // ghiFreePostData
GHIPost* fn_8037F11C(void);                                                                          // ghiNewPost
void fn_8037F1A4(GHIPost* post, GHTTPBool autoFree);                                                 // ghiPostSetAutoFree
void fn_8037F1B4(GHIPost* post);                                                                     // ghiFreePost
GHTTPBool fn_8037F7D8(GHIPostDataState* state);                                                      // ghiPostDataStateInit
GHTTPBool fn_8037F894(GHIConnection* connection);                                                    // ghiPostInitState
int fn_8037FA44(GHIConnection* connection);                                                          // ghiPostCleanupState
int fn_8037FE60(GHIPostDataState* state, GHIConnection* connection);                                 // ghiPostFileDiskStateDoPosting
int fn_803807A8(GHIConnection* connection);                                                          // ghiPostDoPosting
GHTTPBool fn_803809F4(GHIConnection* connection);                                                    // ghiParseURL
void fn_80380BA0(GHIConnection* connection);                                                         // ghiDoSocketInit
void fn_80380C5C(GHIConnection* connection);                                                         // ghiDoHostLookup
void fn_80380D60(GHIConnection* connection);                                                         // ghiDoLookupPending
void fn_80380E00(GHIConnection* connection);                                                         // ghiDoConnecting
void fn_80381010(GHIConnection* connection);                                                         // ghiDoSecuringSession
void fn_80381148(GHIConnection* connection);                                                         // ghiDoSendingRequest
void fn_8038141C(GHIConnection* connection);                                                         // ghiDoPosting
void fn_80381514(GHIConnection* connection);                                                         // ghiDoWaiting
void fn_803816E4(GHIConnection* connection);                                                         // ghiDoReceivingStatus
void fn_80381BB8(GHIConnection* connection);                                                         // ghiDoReceivingHeaders
void fn_8038215C(GHIConnection* connection);                                                         // ghiDoReceivingFile

#ifdef __cplusplus
}
#endif

#endif
