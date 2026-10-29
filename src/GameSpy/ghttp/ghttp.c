#include <types.h>
#include <stdio.h>
#include <string.h>
#include <GameSpy/ghttp/ghttpConnection.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
u16 lbl_805A1060 = 0;
char* lbl_805A1064 = NULL;
unsigned char lbl_805A1068[0x4] __attribute__((aligned(4))) = { 0 };
int lbl_805A106C = 0;
int lbl_805A1070 = 0;
GHIConnection** lbl_805A1074 = NULL;
int lbl_805A1078 = 0;
unsigned char lbl_805A1080[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1084[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1088[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A108C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1090[0x4] __attribute__((aligned(4))) = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
// Checked: without this pragma 10 functions in this unit stop matching.
#pragma auto_inline off

GHTTPRequest fn_8037ED14(const char* URL, GHIPost* post, GHTTPBool blocking, ghttpCompletedCallback completedCallback, void* param) {
    return fn_8037ED38(URL, NULL, post, 0, blocking, NULL, completedCallback, param);
}

void fn_8037EFEC(void* post, GHTTPBool autoFree) {
    if (post != NULL) {
        fn_8037F1A4(post, autoFree);
    }
}

void fn_8037EFFC(GHIPost* post) {
    if (post) {
        fn_8037F1B4(post);
    }
}

// ---------------------------------------------------------------- ghttpBuffer

GHTTPBool fn_8037B37C(GHIBuffer* buffer, int sizeIncrement) {
    int newSize;
    char* tempPtr;

    if (buffer == NULL) {
        return 0;
    }
    if (sizeIncrement <= 0) {
        return 0;
    }
    newSize = buffer->size + sizeIncrement;
    tempPtr = fn_8035DFF8(buffer->data, newSize);
    if (tempPtr == NULL) {
        return 0;
    }
    buffer->data = tempPtr;
    buffer->size = newSize;
    return 1;
}

static inline GHTTPBool ghiResizeBufferInline(GHIBuffer* buffer, int sizeIncrement) {
    int newSize;
    char* tempPtr;

    if (buffer == NULL) {
        return 0;
    }
    if (sizeIncrement <= 0) {
        return 0;
    }
    newSize = buffer->size + sizeIncrement;
    tempPtr = fn_8035DFF8(buffer->data, newSize);
    if (tempPtr == NULL) {
        return 0;
    }
    buffer->data = tempPtr;
    buffer->size = newSize;
    return 1;
}

GHTTPBool fn_8037B3FC(GHIConnection* connection, GHIBuffer* buffer, int initialSize, int sizeIncrement) {
    if (connection == NULL) return 0;
    if (buffer == NULL) return 0;
    if (initialSize <= 0) return 0;
    if (sizeIncrement <= 0) return 0;
    buffer->connection = connection;
    buffer->data = NULL;
    buffer->size = 0;
    buffer->len = 0;
    buffer->pos = 0;
    buffer->sizeIncrement = sizeIncrement;
    buffer->fixed = 0;
    buffer->dontFree = 0;
    buffer->readOnly = 0;
    if (!ghiResizeBufferInline(buffer, initialSize)) return 0;
    *buffer->data = '\0';
    return 1;
}

GHTTPBool fn_8037B7B4(GHIBuffer* buffer, const char* data, int dataLen) {
    int bufSpace;
    int dataWritten;
    int chunkLen;
    int result;
    GHIConnection* connection;

    bufSpace = 0;
    dataWritten = 0;
    if (buffer == NULL) return 0;
    if (data == NULL) return 0;
    if (dataLen < 0) return 0;
    if (buffer->readOnly) return 0;
    connection = buffer->connection;
    if (connection->encryptor.mEngine == 0 || connection->encryptor.mSessionEstablished == 0) {
        return fn_8037B644(buffer, data, dataLen);
    }
    if (dataLen == 0) {
        dataLen = strlen(data);
    }
    if (dataLen == 0) return 1;
    bufSpace = buffer->size - buffer->len;
    do {
        chunkLen = (dataLen < 0x3F01) ? dataLen : 0x3F01;
        result = buffer->connection->encryptor.mEncryptFunc(buffer->connection, &buffer->connection->encryptor, data + dataWritten,
                                                            dataLen, buffer->data + buffer->len, &bufSpace);
        if (result == GHIEncryptionResult_BufferTooSmall) {
            if (!ghiResizeBufferInline(buffer, buffer->sizeIncrement)) return 0;
            bufSpace = buffer->size - buffer->len;
        } else if (result == GHIEncryptionResult_Success) {
            dataWritten += chunkLen;
            buffer->len = buffer->size - bufSpace;
        } else {
            return 0;
        }
    } while (dataWritten < dataLen);
    return 1;
}

GHTTPBool fn_8037B98C(GHIBuffer* buffer, const char* name, const char* value) {
    if (fn_8037B644(buffer, name, 0) == 0) return 0;
    if (fn_8037B644(buffer, lbl_8059FA40, 2) == 0) return 0;
    if (fn_8037B644(buffer, value, 0) == 0) return 0;
    return fn_8037B644(buffer, lbl_8059FA44, 2) != 0;
}

GHTTPBool fn_8037BB6C(GHIBuffer* buffer, int i) {
    char intValue[16];

    sprintf(intValue, lbl_8059FA48, i);
    return fn_8037B644(buffer, intValue, 0);
}

void fn_8037BBB8(GHIBuffer* buffer) {
    buffer->len = 0;
    buffer->pos = 0;
    if (!buffer->readOnly) {
        *buffer->data = '\0';
    }
}

GHTTPBool fn_8037BBDC(GHIConnection* connection) {
    int writeFlag;
    int exceptFlag;
    int rcode;
    int len;

    do {
        rcode = fn_8035C660(connection->socket, NULL, &writeFlag, &exceptFlag);
        if (rcode == -1 || (rcode == 1 && exceptFlag)) {
            connection->completed = 1;
            connection->result = 5;
            if (rcode == -1) {
                connection->socketError = fn_8035C658(connection->socket);
            } else {
                connection->socketError = 0;
            }
            return 0;
        }
        if (rcode < 1 || !writeFlag) {
            return 1;
        }
        len = fn_8037C104(connection, connection->sendBuffer.data + connection->sendBuffer.pos,
                          connection->sendBuffer.len - connection->sendBuffer.pos);
        if (len == -1) {
            return 0;
        }
        connection->sendBuffer.pos += len;
    } while (connection->sendBuffer.pos < connection->sendBuffer.len);
    return 1;
}

GHTTPBool fn_8037BCD0(GHIBuffer* buffer, char* data, int len) {
    if (len == 0) {
        return 1;
    }
    if (buffer->len < len) {
        return 0;
    }
    memcpy(data, buffer->data + buffer->pos, len);
    buffer->pos += len;
    return 1;
}

// ---------------------------------------------------------------- ghttpCallbacks

void fn_8037BD4C(GHIConnection* connection) {
    ghttpCompletedCallback callback;
    char* buffer;
    GHTTPBool freeBuffer;

    callback = connection->completedCallback;
    if (callback) {
        if (connection->type == 0) {
            buffer = connection->getFileBuffer.data;
        } else {
            buffer = NULL;
        }
        freeBuffer = callback(connection->request, connection->result, buffer, connection->fileBytesReceived, connection->callbackParam);
        if (buffer && !freeBuffer) {
            connection->getFileBuffer.dontFree = 1;
        }
    }
}

void fn_8037BDD4(GHIConnection* connection, const char* buffer, GHTTPByteCount bufferLen) {
    ghttpProgressCallback callback = connection->progressCallback;

    if (callback == NULL) {
        return;
    }
    callback(connection->request, connection->state, buffer, bufferLen, connection->fileBytesReceived, connection->totalSize,
             connection->callbackParam);
}

// ---------------------------------------------------------------- ghttpCommon

GHTTPBool fn_8037BE7C(GHIConnection* connection) {
    int encryptedLen;
    int decryptedLen;
    int result;
    char* encryptedData;
    char* decryptedData;
    int decodePos;
    int recvLen;

    encryptedLen = 0;
    decryptedLen = 0;
    while (1) {
        decodePos = connection->decodeBuffer.pos;
        encryptedData = connection->decodeBuffer.data;
        encryptedLen = connection->decodeBuffer.len - decodePos;
        encryptedData += decodePos;
        recvLen = connection->recvBuffer.len;
        decryptedData = connection->recvBuffer.data;
        decryptedLen = connection->recvBuffer.size - recvLen;
        decryptedData += recvLen;
        result = connection->encryptor.mDecryptFunc(connection, &connection->encryptor, encryptedData, &encryptedLen, decryptedData,
                                                    &decryptedLen);
        if (result == GHIEncryptionResult_BufferTooSmall) {
            if (!fn_8037B37C(&connection->recvBuffer, connection->recvBuffer.sizeIncrement)) {
                return 0;
            }
        } else if (result == GHIEncryptionResult_Error) {
            return 0;
        }
        if (result != GHIEncryptionResult_BufferTooSmall || decryptedLen != 0) {
            if (encryptedLen > connection->decodeBuffer.len) {
                return 0;
            }
            connection->decodeBuffer.pos += encryptedLen;
            connection->recvBuffer.len += decryptedLen;
            if (decryptedLen <= 0) {
                break;
            }
        }
    }
    if (connection->decodeBuffer.pos > 0xFF) {
        int bytesLeft = connection->decodeBuffer.len - connection->decodeBuffer.pos;
        if (bytesLeft == 0) {
            fn_8037BBB8(&connection->decodeBuffer);
        } else {
            memmove(connection->decodeBuffer.data, connection->decodeBuffer.data + connection->decodeBuffer.pos, bytesLeft);
            connection->decodeBuffer.len = bytesLeft;
            connection->decodeBuffer.pos = 0;
        }
    }
    return 1;
}

int fn_8037C104(GHIConnection* connection, const char* buffer, int len) {
    int rcode;
    int socketError;

    if (buffer == NULL || len == 0) {
        return 0;
    }
    rcode = send(connection->socket, buffer, len, 0);
    if (rcode == -1) {
        socketError = fn_8035C658(connection->socket);
        if (socketError == -6 || socketError == -0x1A || socketError == -0x4C) {
            return 0;
        }
        connection->socketError = socketError;
        connection->completed = 1;
        connection->result = 5;
        return -1;
    }
    if (connection->state == 6 && !connection->postWaitForUserResponse) {
        connection->postBytesSent += rcode;
    }
    return rcode;
}

// ---------------------------------------------------------------- ghttpConnection

void fn_8037C790(GHTTPBool (*callback)(GHIConnection*)) {
    int i;

    if (lbl_805A106C > 0) {
        fn_8037BE74();
        for (i = 0; i < lbl_805A1070; i++) {
            if (lbl_805A1074[i]->inUse) {
                callback(lbl_805A1074[i]);
            }
        }
        fn_8037BE78();
    }
}

// ---------------------------------------------------------------- ghttpEncryption

GHTTPBool fn_8037CA28(GHTTPRequest request, GHTTPEncryptionEngine engine) {
    GHIConnection* connection = fn_8037C720(request);

    if (connection == NULL)
        return 0;
    if (connection->encryptor.mEngine == engine)
        return 1;
    if (connection->encryptor.mInterface != NULL && connection->encryptor.mEngine != engine)
        return 0;
    if (engine == 0 && strncmp(connection->URL, lbl_804905F8, 8) == 0)
        return 0;
    connection->encryptor.mEngine = engine;
    if (engine == 0) {
        connection->encryptor.mInterface = NULL;
        return 1;
    }
    connection->encryptor.mInterface = NULL;
    connection->encryptor.mInitFunc = fn_8037D0D8;
    connection->encryptor.mCleanupFunc = fn_8037D2AC;
    connection->encryptor.mEncryptFunc = fn_8037D304;
    connection->encryptor.mDecryptFunc = fn_8037D520;
    connection->encryptor.mInitialized = 0;
    connection->encryptor.mSessionEstablished = 0;
    return 1;
}

int fn_8037D2AC(GHIConnection* connection, GHIEncryptor* encryptor) {
    if (encryptor != NULL) {
        if (encryptor->mInterface != NULL) {
            fn_8035E00C(encryptor->mInterface);
            encryptor->mInterface = NULL;
        }
        encryptor->mInitialized = 0;
        encryptor->mSessionEstablished = 0;
    }
    return 1;
}

// ---------------------------------------------------------------- ghttpMain

GHTTPBool fn_8037E8B4(GHIConnection* connection) {
    GHTTPBool completed;

    if (connection->processing) {
        return 0;
    }
    connection->processing = 1;
    if (connection->state == 0) {
        fn_80380BA0(connection);
    }
    if (connection->state == 1) {
        fn_80380C5C(connection);
    }
    if (connection->state == 2) {
        fn_80380D60(connection);
    }
    if (connection->state == 3) {
        fn_80380E00(connection);
    }
    if (connection->state == 4) {
        fn_80381010(connection);
    }
    if (connection->state == 5) {
        fn_80381148(connection);
    }
    if (connection->state == 6) {
        fn_8038141C(connection);
    }
    if (connection->state == 7) {
        fn_80381514(connection);
    }
    if (connection->state == 8) {
        fn_803816E4(connection);
    }
    if (connection->state == 9) {
        fn_80381BB8(connection);
    }
    if (connection->state == 10) {
        fn_8038215C(connection);
    }
    if (connection->redirectURL) {
        fn_8037C81C(connection);
    }
    completed = connection->completed;
    if (connection->result == 0x12 && !completed && !CanReceiveOnSocket(connection->socket)) {
        connection->completed = 1;
    }
    if (connection->completed) {
        fn_8037E7F4(connection);
        fn_8037BD4C(connection);
        fn_8037C5AC(connection);
    } else {
        connection->processing = 0;
    }
    return completed;
}

void fn_8037EA50(void) {
    fn_8037BE74();
    if (++lbl_805A1078 == 1) {
        fn_8037BE6C();
        lbl_8059FA50 = 125;
        lbl_8059FA54 = 250;
    } else {
        fn_8037BE78();
    }
}

static inline void ghttpStartupInline(void) {
    fn_8037BE74();
    if (++lbl_805A1078 == 1) {
        fn_8037BE6C();
        lbl_8059FA50 = 125;
        lbl_8059FA54 = 250;
    } else {
        fn_8037BE78();
    }
}

GHTTPRequest fn_8037EAFC(const char* URL, const char* headers, char* buffer, int bufferSize, GHIPost* post, GHTTPBool throttle,
                         GHTTPBool blocking, ghttpProgressCallback progressCallback, ghttpCompletedCallback completedCallback, void* param) {
    GHIConnection* connection;
    GHTTPBool bResult;

    if (URL == NULL || !URL[0]) {
        return -1;
    }
    if (bufferSize < 0) {
        return -1;
    }
    if (buffer && !bufferSize) {
        return -1;
    }
    if (lbl_805A1078 == 0) {
        ghttpStartupInline();
    }
    connection = fn_8037C3E8();
    if (connection == NULL) {
        return -1;
    }
    connection->type = 0;
    connection->URL = fn_8035D654(URL);
    if (connection->URL == NULL) {
        fn_8037C5AC(connection);
        return -1;
    }
    if (headers && *headers) {
        connection->sendHeaders = fn_8035D654(headers);
        if (connection->sendHeaders == NULL) {
            fn_8037C5AC(connection);
            return -1;
        }
    }
    connection->post = post;
    connection->blocking = blocking;
    connection->progressCallback = progressCallback;
    connection->completedCallback = completedCallback;
    connection->callbackParam = param;
    connection->throttle = throttle;
    connection->userBufferSupplied = (buffer != NULL);
    if (connection->userBufferSupplied) {
        bResult = fn_8037B4F8(connection, &connection->getFileBuffer, buffer, bufferSize);
    } else {
        bResult = fn_8037B3FC(connection, &connection->getFileBuffer, 0x800, 0x800);
    }
    if (!bResult) {
        fn_8037C5AC(connection);
        return -1;
    }
    if (post) {
        if (!fn_8037F894(connection)) {
            fn_8037C5AC(connection);
            return -1;
        }
    }
    if (blocking) {
        while (!fn_8037E8B4(connection)) {
            fn_8035D774(10);
        }
        return 0;
    }
    return connection->request;
}

GHTTPRequest fn_8037ED38(const char* URL, const char* headers, GHIPost* post, GHTTPBool throttle, GHTTPBool blocking,
                         ghttpProgressCallback progressCallback, ghttpCompletedCallback completedCallback, void* param) {
    GHIConnection* connection;

    if (URL == NULL || !URL[0]) {
        return -1;
    }
    if (post == NULL) {
        return -1;
    }
    if (lbl_805A1078 == 0) {
        ghttpStartupInline();
    }
    connection = fn_8037C3E8();
    if (connection == NULL) {
        return -1;
    }
    connection->type = 4;
    connection->URL = fn_8035D654(URL);
    if (connection->URL == NULL) {
        fn_8037C5AC(connection);
        return -1;
    }
    if (headers && *headers) {
        connection->sendHeaders = fn_8035D654(headers);
        if (connection->sendHeaders == NULL) {
            fn_8037C5AC(connection);
            return -1;
        }
    }
    connection->post = post;
    connection->blocking = blocking;
    connection->progressCallback = progressCallback;
    connection->completedCallback = completedCallback;
    connection->callbackParam = param;
    connection->throttle = throttle;
    if (post) {
        if (!fn_8037F894(connection)) {
            fn_8037C5AC(connection);
            return -1;
        }
    }
    if (blocking) {
        while (!fn_8037E8B4(connection)) {
            fn_8035D774(10);
        }
        return 0;
    }
    return connection->request;
}

void fn_8037EEDC(void) {
    fn_8037C790(fn_8037E8B4);
}

GHTTPBool fn_8037EEE8(GHTTPRequest request) {
    GHIConnection* connection = fn_8037C720(request);

    if (connection == NULL) {
        return 0;
    }
    fn_8037E8B4(connection);
    return 1;
}

void fn_8037EF20(GHTTPRequest request) {
    GHIConnection* connection = fn_8037C720(request);

    if (connection != NULL) {
        fn_8037C5AC(connection);
    }
}

void fn_8037EFB0(GHTTPRequest request, int value) {
    GHIConnection* connection = fn_8037C720(request);

    if (connection != NULL) {
        connection->unk184 = value;
    }
}

// ---------------------------------------------------------------- ghttpPost

void fn_8037F078(void* elem) {
    GHIPostData* data = elem;

    if (data->type != GHIXmlData) {
        fn_8035E00C(data->name);
    }
    if (data->type == GHIString) {
        fn_8035E00C(data->data.string);
    } else if (data->type == GHIFileDisk) {
        fn_8035E00C(data->data.fileDisk.filename);
        fn_8035E00C(data->data.fileDisk.reportFilename);
        fn_8035E00C(data->data.fileDisk.contentType);
    } else if (data->type == GHIFileMemory) {
        fn_8035E00C(data->data.fileMemory.reportFilename);
        fn_8035E00C(data->data.fileMemory.contentType);
    } else if (data->type == GHIXmlData) {
        fn_80362DD4(data->data.xml);
    }
}

GHIPost* fn_8037F11C(void) {
    GHIPost* post;

    post = fn_8035DFE8(sizeof(GHIPost));
    if (post == NULL) {
        return NULL;
    }
    memset(post, 0, sizeof(GHIPost));
    post->autoFree = 1;
    post->data = fn_8035A460(sizeof(GHIPostData), 0, fn_8037F078);
    if (post->data == NULL) {
        fn_8035E00C(post);
        return NULL;
    }
    return post;
}

void fn_8037F1B4(GHIPost* post) {
    fn_8035A4FC(post->data);
    fn_8035E00C(post);
}

GHTTPBool fn_8037F7D8(GHIPostDataState* state) {
    GHIPostDataType type;

    type = state->data->type;
    state->pos = -1;
    if (type == GHIString) {
    } else if (type == GHIFileDisk) {
        if (state->file == NULL) {
            return 0;
        }
        if (fn_803F5EA8(state->file, 0, 2) != 0) {
            return 0;
        }
        state->len = fn_803F5CE0(state->file);
        if (state->len == -1) {
            return 0;
        }
        fn_803F5EAC(state->file);
    } else if (type != GHIFileMemory && type != GHIXmlData) {
        return 0;
    }
    return 1;
}

int fn_8037FE60(GHIPostDataState* state, GHIConnection* connection) {
    char buffer[0x1000];
    struct {
        s16 a;
        u8 b;
    } padding;
    int len;
    int rcode;
    int padLen;

    while (1) {
        len = fn_803F524C(buffer, 1, sizeof(buffer), state->file);
        if (len <= 0) {
            connection->completed = 1;
            connection->result = 0xE;
            return 0;
        }
        state->pos += len;
        if (state->pos > state->len) {
            connection->completed = 1;
            connection->result = 0xE;
            return 0;
        }
        rcode = fn_8037C1C0(connection, buffer, len);
        if (rcode == 0) {
            return 0;
        }
        if (state->pos == state->len) {
            if (connection->post->useDIME) {
                padding.a = 0;
                padding.b = 0;
                padLen = 4 - (state->len % 4);
                if (padLen != 4 && padLen > 0) {
                    if (fn_8037C1C0(connection, (const char*)&padding, padLen) == 0) {
                        return 0;
                    }
                }
            }
            return 1;
        }
        if (rcode != 1) {
            return 2;
        }
    }
}

// ---------------------------------------------------------------- ghttpProcess

void fn_80380BA0(GHIConnection* connection) {
    int protocol;

    fn_8037BDD4(connection, NULL, 0);
    fn_8035D720();
    if (!fn_803809F4(connection)) {
        connection->completed = 1;
        connection->result = 3;
        return;
    }
    protocol = connection->protocol;
    if (protocol == 1 && connection->encryptor.mEngine == 0) {
        fn_8037CA28(connection->request, 1);
    } else if (protocol != 1 && connection->encryptor.mEngine != 0) {
        fn_8037CA28(connection->request, 0);
    }
    connection->state = 1;
    fn_8037BDD4(connection, NULL, 0);
}

static inline const char* ghiGetHostAddress(GHIConnection* connection) {
    if (connection->proxyOverrideServer) {
        return connection->proxyOverrideServer;
    }
    if (lbl_805A1064) {
        return lbl_805A1064;
    }
    return connection->serverAddress;
}

void fn_80380C5C(GHIConnection* connection) {
    const char* server;

    if (connection->handleThread != NULL) {
        connection->state = 2;
        fn_8037BDD4(connection, NULL, 0);
        return;
    }
    server = ghiGetHostAddress(connection);
    connection->serverIP = fn_8035C620(server);
    if (connection->serverIP == 0xFFFFFFFF && fn_8035D364(server, &connection->handleThread) == -1) {
        connection->handleThread = NULL;
        connection->completed = 1;
        connection->result = 4;
        return;
    }
    if (connection->serverIP == 0xFFFFFFFF) {
        connection->state = 2;
        fn_8037BDD4(connection, NULL, 0);
        return;
    }
    connection->state = 3;
    fn_8037BDD4(connection, NULL, 0);
}

void fn_80380D60(GHIConnection* connection) {
    u32 ip;

    ip = fn_8035D560(connection->handleThread);
    connection->serverIP = ip;
    if (ip == 0xFFFFFFFF) {
        connection->handleThread = NULL;
        connection->completed = 1;
        connection->result = 4;
        return;
    }
    if (ip == 0) {
        connection->state = 2;
        fn_8037BDD4(connection, NULL, 0);
        return;
    }
    connection->handleThread = NULL;
    connection->state = 3;
    fn_8037BDD4(connection, NULL, 0);
}

void fn_80381514(GHIConnection* connection) {
    int readFlag;
    int exceptFlag;
    int rcode;

    rcode = fn_8035C660(connection->socket, &readFlag, NULL, &exceptFlag);
    if (rcode == -1 || (rcode == 1 && exceptFlag)) {
        connection->completed = 1;
        connection->result = 5;
        if (rcode == -1) {
            connection->socketError = fn_8035C658(connection->socket);
            return;
        }
        connection->socketError = 0;
        return;
    }
    if (rcode == 1 && readFlag) {
        connection->state = 8;
        fn_8037BDD4(connection, NULL, 0);
    }
}

void fn_80380E00(GHIConnection* connection) {
    gs_sockaddr_in address;
    int writeFlag;
    int exceptFlag;
    int rcode;
    int socketError;

    if (connection->socket == -1) {
        rcode = socket(2, 1, 6);
        connection->socket = rcode;
        if (rcode == -1) {
            connection->completed = 1;
            connection->result = 5;
            connection->socketError = fn_8035C658(rcode);
            return;
        }
        if (!fn_8035CD04(rcode, 0)) {
            connection->completed = 1;
            connection->result = 5;
            connection->socketError = fn_8035C658(connection->socket);
            return;
        }
        if (connection->throttle) {
            SetReceiveBufferSize(connection->socket, lbl_8059FA50);
        }
        memset(&address, 0, sizeof(address));
        address.sin_family = 2;
        if (connection->proxyOverrideServer) {
            address.sin_port = SOHtoNs(connection->proxyOverridePort);
        } else if (lbl_805A1064) {
            address.sin_port = SOHtoNs(lbl_805A1060);
        } else {
            address.sin_port = SOHtoNs(connection->serverPort);
        }
        address.sin_addr = connection->serverIP;
        if (fn_8035C384(connection->socket, &address, sizeof(address)) == -1) {
            socketError = fn_8035C658(connection->socket);
            if (socketError != -6 && socketError != -0x1A && socketError != -0x4C) {
                connection->completed = 1;
                connection->result = 6;
                connection->socketError = socketError;
                return;
            }
        }
    }
    rcode = fn_8035C660(connection->socket, NULL, &writeFlag, &exceptFlag);
    if (rcode == -1 || (rcode == 1 && exceptFlag)) {
        connection->completed = 1;
        connection->result = 6;
        if (rcode == -1) {
            connection->socketError = fn_8035C658(connection->socket);
            return;
        }
        connection->socketError = 0;
        return;
    }
    if (rcode == 1 && writeFlag) {
        if (connection->encryptor.mEngine == 0) {
            connection->state = 5;
        } else {
            connection->state = 4;
        }
        fn_8037BDD4(connection, NULL, 0);
    }
}

void fn_8038141C(GHIConnection* connection) {
    int readFlag;
    int oldBytesPosted = connection->postBytesSent;
    int postingResult = fn_803807A8(connection);

    if (postingResult == 0) {
        readFlag = 0;
        fn_8037FA44(connection);
        if (fn_8035C660(connection->socket, &readFlag, NULL, NULL) == 1 && readFlag) {
            connection->state = 8;
            fn_8037BDD4(connection, NULL, 0);
        }
    } else {
        if (postingResult == 3) {
            connection->postWaitForUserResponse = 0;
            return;
        }
        if (oldBytesPosted != connection->postBytesSent) {
            fn_8037BE0C(connection);
        }
        if (postingResult == 1) {
            fn_8037FA44(connection);
            connection->postComplete = 1;
            connection->state = 7;
            fn_8037BDD4(connection, NULL, 0);
        }
    }
}
