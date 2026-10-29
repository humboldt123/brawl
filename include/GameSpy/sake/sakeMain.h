#ifndef GAMESPY_SAKE_SAKEMAIN_H
#define GAMESPY_SAKE_SAKEMAIN_H

// GameSpy SDK Sake (sakeMain.c, sakeRequest.c). Field order follows the SDK's
// SAKEInternal, SAKERequestInternal and SAKEIRequestInfo. The functions are
// unnamed in our symbols; the SDK names are noted next to the placeholders.
#include <types.h>
#include <GameSpy/common/gsXML.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SAKE_MAX_URL_LENGTH 128
#define SAKE_LOGIN_TICKET_LENGTH 25

typedef struct SAKEInternal {
    int mIsGameAuthenticated;                     // 0x0
    char mGameName[16];                           // 0x4
    int mGameId;                                  // 0x14
    char mSecretKey[9];                           // 0x18
    int mIsProfileAuthenticated;                  // 0x24
    int mProfileId;                               // 0x28
    char mLoginTicket[SAKE_LOGIN_TICKET_LENGTH];  // 0x2C
    int mStartRequestResult;                      // 0x48
} SAKEInternal;
typedef SAKEInternal* SAKE;

typedef struct SAKERequestInternal SAKERequestInternal;
typedef SAKERequestInternal* SAKERequest;

typedef int (*SAKEIValidateInputFunc)(SAKERequest request);
typedef int (*SAKEIFillSoapRequestFunc)(SAKERequest request);
typedef int (*SAKEIProcessSoapResponseFunc)(SAKERequest request);
typedef void (*SAKEIFreeDataFunc)(SAKERequest request);

typedef struct SAKEIRequestInfo {
    u32 mSakeOutputSize;                                    // 0x0
    const char* mFuncName;                                  // 0x4
    const char* mSoapAction;                                // 0x8
    const char* mResponseTag;                               // 0xC
    const char* mResultTag;                                 // 0x10
    SAKEIValidateInputFunc mValidateInputFunc;              // 0x14
    SAKEIFillSoapRequestFunc mFillSoapRequestFunc;          // 0x18
    SAKEIProcessSoapResponseFunc mProcessSoapResponseFunc;  // 0x1C
    SAKEIFreeDataFunc mFreeDataFunc;                        // 0x20
} SAKEIRequestInfo;

struct SAKERequestInternal {
    SAKE mSake;                       // 0x0
    int mType;                        // 0x4
    void* mInput;                     // 0x8
    void* mOutput;                    // 0xC
    void* mCallback;                  // 0x10
    void* mCallbackData;              // 0x14
    GSXmlStreamWriter mSoapRequest;   // 0x18
    GSXmlStreamReader mSoapResponse;  // 0x1C
    SAKEIRequestInfo* mInfo;          // 0x20
};

__static_assert(sizeof(SAKEInternal) == 0x4C, "SAKEInternal");
__static_assert(sizeof(SAKEIRequestInfo) == 0x24, "SAKEIRequestInfo");

// the Sake availability result (__GSIACResult) and the SOAP url buffer
extern int lbl_805A1048;
extern char lbl_80536500[SAKE_MAX_URL_LENGTH];
// SOAP namespace table and names
extern const char* lbl_8059FBE0[2];
extern char lbl_8059FBF0[4];       // "ns1"
extern char lbl_8059FBF4[7];       // "gameid"
extern char lbl_80490F90[10];      // "secretKey"
extern char lbl_80490F9C[12];      // "loginTicket"
extern char lbl_80490FA8[0x48];    // SOAP url format
extern char lbl_80490DA0[0x53];    // download url format
extern char lbl_80490DF4[0x47];    // upload url format
extern char lbl_80490E3C[0x12];
extern char lbl_80490E50[0xE];
extern char lbl_8059FBD8[4];       // "%d"
extern char lbl_80491088[9];       // "recordid"
extern SAKEIRequestInfo lbl_80491104;  // create record request info


int fn_8038D394(SAKE* sakePtr);                                        // sakeStartup
void fn_8038D480(SAKE sake, int profileId, const char* loginTicket);   // sakeSetProfile
SAKERequest fn_8038D4D0(SAKE sake, void* input, void* callback, void* userData);  // sakeCreateRecord
int fn_8038D54C(SAKE sake, int fileId, char* url);                     // sakeGetFileDownloadURL
int fn_8038D5D0(SAKE sake, char* url);                                 // sakeGetFileUploadURL
int fn_8038D64C(const char* headers, int* result);                    // sakeGetFileResultFromHeaders
int fn_8038D6F8(const char* headers, int* fileId);                    // sakeGetFileIdFromHeaders
SAKERequest fn_8038D77C(SAKE sake, int type, void* input, void* callback, void* userData);  // sakeiInitRequest
void fn_8038D84C(SAKERequest request);                                 // sakeiFreeRequest
void fn_8038DA60(int httpResult, GSXmlStreamWriter request, GSXmlStreamReader response, void* userData);  // sakeiSoapCallback
int fn_8038DC08(SAKERequest request);                                  // sakeiStartRequest
int fn_8038DD48(SAKERequest request, SAKEIRequestInfo* info);          // sakeiStartXRequest
int fn_8038E268(SAKERequest request);                                  // create record: read the record id
int fn_8038E2AC(SAKERequest request);                                  // sakeiStartCreateRecordRequest
int fn_8038E370(SAKERequest request);                                  // validate input

#ifdef __cplusplus
}
#endif

#endif
