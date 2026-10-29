#include <types.h>
#include <string.h>
#include <GameSpy/common/gsXML.h>
#include <GameSpy/ghttp/ghttpConnection.h>

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
// Checked: without this pragma 1 function in this unit stop matching.
#pragma auto_inline off

void fn_80362DD4(GSXmlStreamWriter stream) {
    GSIXmlStreamWriter* writer = stream;

    fn_8035E00C(writer->mBuffer);
    fn_8035E00C(writer);
}

GSSoapTask* fn_803621B8(const char* URL, const char* service, GSXmlStreamWriter request, GSSoapCallbackFunc callback, void* userData) {
    GSSoapTask* aSoapTask;
    GSTask* aCoreTask;

    aSoapTask = fn_8035DFE8(sizeof(GSSoapTask));
    if (aSoapTask == NULL) {
        return NULL;
    }
    aSoapTask->mCallbackFunc = callback;
    aSoapTask->mCustomFunc = NULL;
    aSoapTask->mURL = URL;
    aSoapTask->mService = service;
    aSoapTask->mRequestSoap = request;
    aSoapTask->mPostData = NULL;
    aSoapTask->mResponseSoap = NULL;
    aSoapTask->mResponseBuffer = NULL;
    aSoapTask->mUserData = userData;
    aSoapTask->mRequestResult = 0;
    aSoapTask->mCompleted = 0;

    aCoreTask = fn_8035EA18();
    if (aCoreTask == NULL) {
        fn_8035E00C(aSoapTask);
        return NULL;
    }
    aCoreTask->mCallbackFunc = fn_80362488;
    aCoreTask->mExecuteFunc = fn_80362384;
    aCoreTask->mThinkFunc = fn_80362348;
    aCoreTask->mCleanupFunc = fn_803624A8;
    aCoreTask->mCancelFunc = fn_80362434;
    aCoreTask->mTaskData = aSoapTask;
    aSoapTask->mCoreTask = aCoreTask;
    fn_8035E87C(aCoreTask, 0);
    return aSoapTask;
}

int fn_803622B0(int request, int result, char* buffer, int bufferLen, void* param) {
    GSSoapTask* aSoapTask = param;
    GSXmlStreamReader reader;

    aSoapTask->mRequestResult = result;
    aSoapTask->mCompleted = 1;
    aSoapTask->mResponseBuffer = buffer;
    if (result == 0) {
        reader = fn_80362C98();
        aSoapTask->mResponseSoap = reader;
        if (reader == NULL) {
            aSoapTask->mRequestResult = 1;
        } else if (!fn_80362D50(reader, buffer, bufferLen)) {
            aSoapTask->mRequestResult = 7;
        }
    }
    return 0;
}

int fn_80362348(void* taskData) {
    GSSoapTask* aSoapTask = taskData;

    if (aSoapTask->mCompleted) {
        return 4;
    }
    fn_8037EEE8(aSoapTask->mRequestId);
    return 1;
}

void fn_80362384(void* taskData) {
    GSSoapTask* aSoapTask = taskData;
    void* post;

    post = fn_8037EFE8();
    aSoapTask->mPostData = post;
    if (post == NULL) {
        aSoapTask->mCompleted = 1;
        aSoapTask->mRequestResult = 1;
        return;
    }
    fn_8037EFEC(post, 0);
    fn_8037F074(aSoapTask->mPostData, aSoapTask->mRequestSoap);
    if (aSoapTask->mCustomFunc) {
        aSoapTask->mCustomFunc(aSoapTask->mPostData, aSoapTask->mUserData);
    }
    aSoapTask->mRequestId = fn_8037EAFC(aSoapTask->mURL, aSoapTask->mService, NULL, 0, aSoapTask->mPostData, 0, 0, NULL,
                                        fn_803622B0, aSoapTask);
}

void fn_80362434(void* taskData) {
    GSSoapTask* aSoapTask = taskData;

    if (!aSoapTask->mCompleted) {
        if (aSoapTask->mRequestId >= 0) {
            fn_8037EF20(aSoapTask->mRequestId);
        }
        aSoapTask->mRequestResult = 0x12;
        aSoapTask->mCompleted = 1;
    }
}

void fn_80362488(void* taskData, int result) {
    GSSoapTask* aSoapTask = taskData;

    aSoapTask->mCallbackFunc(aSoapTask->mRequestResult, aSoapTask->mRequestSoap, aSoapTask->mResponseSoap, aSoapTask->mUserData);
}

GSXmlStreamReader fn_80362C98(void) {
    GSIXmlStreamReader* newStream;

    newStream = fn_8035DFE8(sizeof(GSIXmlStreamReader));
    if (newStream == NULL) {
        return NULL;
    }
    newStream->mElementArray = fn_8035A460(0x18, 0x20, fn_80362F4C);
    if (newStream->mElementArray == NULL) {
        fn_8035E00C(newStream);
        return NULL;
    }
    newStream->mAttributeArray = fn_8035A460(0x18, 0x10, fn_80362F50);
    if (newStream->mAttributeArray == NULL) {
        fn_8035A4FC(newStream->mElementArray);
        fn_8035E00C(newStream);
        return NULL;
    }
    newStream->mElemReadIndex = -1;
    newStream->mValueReadIndex = -1;
    return newStream;
}

int fn_80362D50(GSXmlStreamReader stream, char* data, int len) {
    int readPos = 0;
    GSIXmlStreamReader* reader = stream;
    int length = len;
    char* buffer = data;

    while (readPos < length) {
        if (!fn_80362F64(reader, buffer, length, &readPos, -1)) {
            return 0;
        }
    }
    return 1;
}

void fn_80362E0C(GSXmlStreamReader stream) {
    GSIXmlStreamReader* reader = stream;

    fn_8035A4FC(reader->mAttributeArray);
    fn_8035A4FC(reader->mElementArray);
    fn_8035E00C(reader);
}

int fn_80367838(GSXmlStreamReader stream) {
    GSIXmlStreamReader* reader = stream;

    reader->mElemReadIndex = -1;
    reader->mValueReadIndex = -1;
    return 1;
}

int fn_8036784C(GSXmlStreamReader stream, const char* matchtag) {
    GSIXmlStreamReader* reader = stream;
    int i;

    for (i = reader->mElemReadIndex + 1; i < fn_8035A590(reader->mElementArray); i++) {
        if (fn_80367B44(matchtag, fn_8035A598(reader->mElementArray, i))) {
            reader->mElemReadIndex = i;
            reader->mValueReadIndex = -1;
            return 1;
        }
    }
    return 0;
}

int fn_803679BC(GSXmlStreamReader stream, const char* matchtag, char* valueOut, int maxLen) {
    const char* strValue = NULL;
    int strLen = 0;

    if (!fn_803678E0(stream, matchtag, &strValue, &strLen)) {
        valueOut[0] = '\0';
        return 0;
    }
    strncpy(valueOut, strValue, maxLen < strLen ? maxLen : strLen);
    valueOut[maxLen - 1 < strLen ? maxLen - 1 : strLen] = '\0';
    return 1;
}
