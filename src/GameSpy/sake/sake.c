#include <types.h>
#include <stdio.h>
#include <string.h>
#include <GameSpy/sake/sakeMain.h>

char lbl_80536500[SAKE_MAX_URL_LENGTH] = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own; they follow the GameSpy SDK's
// sakeMain.c and sakeRequest.c.
// Checked: without this pragma 2 functions in this unit stop matching.
#pragma auto_inline off

int fn_8038D394(SAKE* sakePtr) {
    SAKE sake;

    if (lbl_805A1048 != 1) {
        return 1;
    }
    if (fn_8035E848() != 0) {
        return 2;
    }
    sake = fn_8035DFE8(sizeof(SAKEInternal));
    if (sake == NULL) {
        return 3;
    }
    memset(sake, 0, sizeof(SAKEInternal));
    *sakePtr = sake;
    return 0;
}

void fn_8038D480(SAKE sake, int profileId, const char* loginTicket) {
    sake->mProfileId = profileId;
    memcpy(sake->mLoginTicket, loginTicket, SAKE_LOGIN_TICKET_LENGTH);
    sake->mIsProfileAuthenticated = 1;
}

SAKERequest fn_8038D4D0(SAKE sake, void* input, void* callback, void* userData) {
    SAKERequest request;
    SAKERequest result;
    int startResult;

    request = fn_8038D77C(sake, 0, input, callback, userData);
    result = request;
    if (request == NULL) {
        result = NULL;
    } else {
        startResult = fn_8038E2AC(request);
        sake->mStartRequestResult = startResult;
        if (startResult != 0) {
            fn_8038D84C(result);
            result = NULL;
        }
    }
    return result;
}

int fn_8038D54C(SAKE sake, int fileId, char* url) {
    if (sake == NULL || url == NULL || !sake->mIsGameAuthenticated || !sake->mIsProfileAuthenticated) {
        return 0;
    }
    return snprintf(url, SAKE_MAX_URL_LENGTH, lbl_80490DA0, sake->mGameName, fileId, sake->mGameId, sake->mProfileId) >= 0;
}

int fn_8038D5D0(SAKE sake, char* url) {
    if (sake == NULL || url == NULL || !sake->mIsGameAuthenticated || !sake->mIsProfileAuthenticated) {
        return 0;
    }
    return snprintf(url, SAKE_MAX_URL_LENGTH, lbl_80490DF4, sake->mGameName, sake->mGameId, sake->mProfileId) >= 0;
}

int fn_8038D64C(const char* headers, int* result) {
    const char* headerName = lbl_80490E3C;
    const char* header = strstr(headers, headerName);
    int value;
    int found;
    int clamped;

    if (header != NULL && sscanf(header + strlen(headerName), lbl_8059FBD8, &value) == 1) {
        found = 1;
    } else {
        found = 0;
    }
    if (!found) {
        return 0;
    }
    clamped = value;
    if (clamped >= 6) {
        clamped = 6;
    }
    *result = clamped;
    return 1;
}

int fn_8038D6F8(const char* headers, int* fileId) {
    const char* headerName = lbl_80490E50;
    const char* header = strstr(headers, headerName);

    if (header != NULL) {
        if (sscanf(header + strlen(headerName), lbl_8059FBD8, fileId) == 1) {
            return 1;
        }
    }
    return 0;
}

int fn_8038DC08(SAKERequest request) {
    SAKEIRequestInfo* info;
    int result;

    info = request->mInfo;
    result = info->mValidateInputFunc(request);
    if (result != 0) {
        return result;
    }
    request->mSoapRequest = fn_80362578(lbl_8059FBE0, 1);
    if (request->mSoapRequest == NULL) {
        return 2;
    }
    fn_80364034(request->mSoapRequest, lbl_8059FBF0, request->mInfo->mFuncName);
    fn_80365548(request->mSoapRequest, lbl_8059FBF0, lbl_8059FBF4, request->mSake->mGameId);
    fn_803646CC(request->mSoapRequest, lbl_8059FBF0, lbl_80490F90, request->mSake->mSecretKey);
    fn_803646CC(request->mSoapRequest, lbl_8059FBF0, lbl_80490F9C, request->mSake->mLoginTicket);
    result = info->mFillSoapRequestFunc(request);
    if (result != 0) {
        fn_80362DD4(request->mSoapRequest);
        request->mSoapRequest = NULL;
        if (info->mFreeDataFunc) {
            info->mFreeDataFunc(request);
        }
        return result;
    }
    fn_80364340(request->mSoapRequest, lbl_8059FBF0, request->mInfo->mFuncName);
    fn_80362E4C(request->mSoapRequest);
    return 0;
}

int fn_8038DD48(SAKERequest request, SAKEIRequestInfo* info) {
    int result;

    request->mInfo = info;
    result = fn_8038DC08(request);
    if (result != 0) {
        return result;
    }
    if (lbl_80536500[0] == '\0') {
        snprintf(lbl_80536500, SAKE_MAX_URL_LENGTH, lbl_80490FA8, request->mSake->mGameName);
    }
    fn_803621B8(lbl_80536500, request->mInfo->mSoapAction, request->mSoapRequest, fn_8038DA60, request);
    return 0;
}

int fn_8038E268(SAKERequest request) {
    int found = fn_80367A60(request->mSoapResponse, lbl_80491088, request->mOutput);
    int result = 0;

    if (found == 0) {
        result = 5;
    }
    return result;
}

int fn_8038E2AC(SAKERequest request) {
    return fn_8038DD48(request, &lbl_80491104);
}

int fn_8038E370(SAKERequest request) {
    int result = 4;

    if (*(void**)request->mInput != NULL) {
        result = 0;
    }
    return result;
}
