#include <types.h>
#include <string.h>
#include <GameSpy/gt2/gt2Main.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80534A40[0x1000] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80535A40[0x40] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1050[0x8] __attribute__((aligned(4))) = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
// Checked: without this pragma 9 functions in this unit stop matching.
#pragma auto_inline off

int fn_80377DDC(int a1, int a2, int a3, int a4, int a5, int a6);

int fn_80374878(int a1, int a2, int a3, int a4, int a5, int a6) {
    return fn_80377DDC(a1, a2, a3, a4, a5, 0);
}

void fn_80374BC0(GTI2Connection* connection) {
    fn_803746D0(connection, 1);
}

// ---------------------------------------------------------------- gt2Buffer

GTBool fn_80373780(GTI2Buffer* buffer, int size) {
    buffer->buffer = fn_8035DFE8(size);
    if (buffer->buffer == NULL) {
        return 0;
    }
    buffer->size = size;
    return 1;
}

int fn_803737D8(GTI2Buffer* buffer) {
    return buffer->size - buffer->len;
}

void fn_803737E8(GTI2Buffer* buffer, GTByte b) {
    buffer->buffer[buffer->len++] = b;
}

void fn_80373800(GTI2Buffer* buffer, u16 s) {
    buffer->buffer[buffer->len++] = (GTByte)(s >> 8);
    buffer->buffer[buffer->len++] = (GTByte)s;
}

void fn_8037382C(GTI2Buffer* buffer, const GTByte* data, int len) {
    if (data == NULL || len == 0) {
        return;
    }
    if (len == -1) {
        len = strlen((const char*)data);
    }
    memcpy(buffer->buffer + buffer->len, data, len);
    buffer->len += len;
}

void fn_803738B8(GTI2Buffer* buffer, int start, int shortenBy) {
    GTByte* p;

    if (start == -1) {
        start = buffer->len - shortenBy;
    }
    p = buffer->buffer + start;
    memmove(p, p + shortenBy, (buffer->len - start) - shortenBy);
    buffer->len -= shortenBy;
}

// ---------------------------------------------------------------- gt2Callback

GTBool fn_80373924(GTI2Socket* socket) {
    if (socket == NULL) {
        return 1;
    }
    if (socket->socketErrorCallback == NULL) {
        return 1;
    }
    socket->callbackLevel++;
    socket->socketErrorCallback(socket);
    socket->callbackLevel--;
    if (socket->close && socket->callbackLevel == 0) {
        fn_80378018(socket);
        return 0;
    }
    return 1;
}

GTBool fn_80373D10(GTI2Connection* connection, int latency) {
    if (connection == NULL) {
        return 1;
    }
    if (connection->pingCallback == NULL) {
        return 1;
    }
    connection->callbackLevel++;
    connection->socket->callbackLevel++;
    connection->pingCallback(connection, latency);
    connection->callbackLevel--;
    connection->socket->callbackLevel--;
    if (connection->socket->close && connection->socket->callbackLevel == 0) {
        fn_80378018(connection->socket);
        return 0;
    }
    return 1;
}

GTBool fn_80373DCC(GTI2Connection* connection, int filterID, const GTByte* message, int len, GTBool reliable) {
    gt2SendFilterCallback* callback;

    if (connection == NULL) {
        return 1;
    }
    callback = fn_8035A598(connection->sendFilters, filterID);
    if (callback == NULL) {
        return 1;
    }
    if (len == 0 || message == NULL) {
        message = NULL;
        len = 0;
    }
    connection->callbackLevel++;
    connection->socket->callbackLevel++;
    (*callback)(connection, filterID, message, len, reliable);
    connection->callbackLevel--;
    connection->socket->callbackLevel--;
    if (connection->socket->close && connection->socket->callbackLevel == 0) {
        fn_80378018(connection->socket);
        return 0;
    }
    return 1;
}

GTBool fn_80373ED4(GTI2Connection* connection, int filterID, GTByte* message, int len, GTBool reliable) {
    gt2ReceiveFilterCallback* callback;

    if (connection == NULL) {
        return 1;
    }
    callback = fn_8035A598(connection->receiveFilters, filterID);
    if (callback == NULL) {
        return 1;
    }
    if (len == 0 || message == NULL) {
        message = NULL;
        len = 0;
    }
    connection->callbackLevel++;
    connection->socket->callbackLevel++;
    (*callback)(connection, filterID, message, len, reliable);
    connection->callbackLevel--;
    connection->socket->callbackLevel--;
    if (connection->socket->close && connection->socket->callbackLevel == 0) {
        fn_80378018(connection->socket);
        return 0;
    }
    return 1;
}

GTBool fn_80373FDC(GTI2Socket* socket, GTI2Connection* connection, u32 ip, u16 port, GTBool reset, const GTByte* message, int len, GTBool send) {
    gt2DumpCallback callback;

    if (socket == NULL) {
        return 1;
    }
    if (send) {
        callback = socket->sendDumpCallback;
    } else {
        callback = socket->receiveDumpCallback;
    }
    if (callback == NULL) {
        return 1;
    }
    if (len == 0 || message == NULL) {
        message = NULL;
        len = 0;
    }
    socket->callbackLevel++;
    if (connection != NULL) {
        connection->callbackLevel++;
    }
    callback(socket, connection, ip, port, reset, message, len);
    socket->callbackLevel--;
    if (connection != NULL) {
        connection->callbackLevel--;
    }
    if (socket->close && socket->callbackLevel == 0) {
        fn_80378018(socket);
        return 0;
    }
    return 1;
}

// ---------------------------------------------------------------- gt2Connection

GT2Result fn_803741A4(GTI2Socket* socket, GTI2Connection** connection, u32 ip, u16 port) {
    GT2Result result;

    result = fn_80378084(socket, connection, ip, port);
    if (result != 0) {
        return result;
    }
    (*connection)->state = GTI2AwaitingServerChallenge;
    (*connection)->initiated = 1;
    return 0;
}

GT2Result fn_803741F8(GTI2Socket* socket, GTI2Connection** connection, u32 ip, u16 port) {
    GT2Result result;

    result = fn_80378084(socket, connection, ip, port);
    if (result != 0) {
        return result;
    }
    (*connection)->state = GTI2AwaitingClientResponse;
    (*connection)->initiated = 0;
    return 0;
}

// HYPOTHESIS: the SDK's gti2ConnectionClosed, inlined into its callers.
static inline void gti2ConnectionClosed(GTI2Connection* connection) {
    if (connection->state != GTI2Closed) {
        connection->state = GTI2Closed;
        fn_8035B120(connection->socket->connections, &connection);
        fn_8035A5C8(connection->socket->closedConnections, &connection);
    }
}

GTBool fn_80374488(GTI2Connection* connection, gsi_time now) {
    GTBool timedOut;

    if (connection->state < GTI2Connected) {
        timedOut = 0;
        if (connection->initiated) {
            if (connection->timeout != 0 && now - connection->startTime > connection->timeout) {
                timedOut = 1;
            }
        } else if (connection->state < GTI2AwaitingAcceptReject && now - connection->startTime > 60000) {
            timedOut = 1;
        }
        if (timedOut) {
            fn_80377AD0(connection);
            gti2ConnectionClosed(connection);
            if (!fn_80373AA0(connection, 6, NULL, 0)) {
                return 0;
            }
        }
    }
    return 1;
}

static inline GTBool gti2KeepAlive(GTI2Connection* connection, gsi_time now) {
    if (now - connection->lastSend > 30000 && !fn_80377724(connection)) {
        return 0;
    }
    return 1;
}

static inline GTBool gti2ResendMessages(GTI2Connection* connection, gsi_time now) {
    int num;
    int i;
    GTI2OutgoingBufferMessage* message;

    num = fn_8035A590(connection->outgoingBufferList);
    for (i = 0; i < num; i++) {
        message = fn_8035A598(connection->outgoingBufferList, i);
        if (now - message->lastSend > 1000 && !fn_80377B88(connection, message)) {
            return 0;
        }
    }
    return 1;
}

static inline GTBool gti2SendPendingAck(GTI2Connection* connection, gsi_time now) {
    if (!connection->pendingAck) {
        return 1;
    }
    if (now - connection->pendingAckTime > 100 && !fn_80377918(connection)) {
        return 0;
    }
    return 1;
}

GTBool fn_80374584(GTI2Connection* connection, gsi_time now) {
    if (!fn_80374488(connection, now)) {
        return 0;
    }
    if (!gti2KeepAlive(connection, now)) {
        return 0;
    }
    if (!gti2ResendMessages(connection, now)) {
        return 0;
    }
    return gti2SendPendingAck(connection, now) != 0;
}

void fn_803746D0(GTI2Connection* connection, GTBool hard) {
    if (hard) {
        if (connection->state < GTI2Closed) {
            gti2ConnectionClosed(connection);
            fn_80377AD0(connection);
            fn_80373C54(connection, 0);
            fn_803782F8(connection);
        }
    } else {
        connection->state = GTI2Closing;
        fn_8037765C(connection);
    }
}

void fn_803747D4(GTI2Connection* connection) {
    if (connection->initialMessage) {
        fn_8035E00C(connection->initialMessage);
    }
    if (connection->incomingBuffer.buffer) {
        fn_8035E00C(connection->incomingBuffer.buffer);
    }
    if (connection->outgoingBuffer.buffer) {
        fn_8035E00C(connection->outgoingBuffer.buffer);
    }
    if (connection->incomingBufferList) {
        fn_8035A4FC(connection->incomingBufferList);
    }
    if (connection->outgoingBufferList) {
        fn_8035A4FC(connection->outgoingBufferList);
    }
    if (connection->sendFilters) {
        fn_8035A4FC(connection->sendFilters);
    }
    if (connection->receiveFilters) {
        fn_8035A4FC(connection->receiveFilters);
    }
    fn_8035E00C(connection);
}

// ---------------------------------------------------------------- gt2Main

void fn_80374880(GTI2Socket* socket) {
    fn_8035B264(socket->connections, fn_80374BC8, NULL);
    fn_80378018(socket);
}

void fn_803748C4(GTI2Socket* socket) {
    if (fn_80376DF4(socket)) {
        if (fn_803786FC(socket)) {
            fn_80378748(socket);
        }
    }
}

int fn_80374AC4(GTI2Connection* connection, const GTByte* message, int len, GTBool reliable) {
    u16 length;
    GTBool sent;
    int result;

    if (connection->state != GTI2Connected) {
        return 8;
    }
    fn_80378B50(&message, &len);
    if (reliable && connection->socket->protocolType == 2) {
        memcpy(&length, message, 2);
        if (len != length + connection->socket->protocolOffset) {
            return 9;
        }
    }
    if (fn_8035A590(connection->sendFilters)) {
        fn_80373DCC(connection, 0, message, len, reliable);
        return 0;
    }
    sent = fn_80377C44(connection, message, len, reliable);
    result = 10;
    if (sent) {
        result = 0;
    }
    return result;
}

void fn_80374BC8(void* elem, void* clientData) {
    fn_803746D0(*(GTI2Connection**)elem, 1);
}

void fn_80374BD4(GTI2Socket* socket) {
    fn_8035B264(socket->connections, fn_80374BC8, NULL);
}

int fn_80374BF0(GTI2Connection* connection) {
    return connection->incomingBuffer.size - connection->incomingBuffer.len;
}

int fn_80374C00(GTI2Connection* connection) {
    return connection->outgoingBuffer.size - connection->outgoingBuffer.len;
}

// ---------------------------------------------------------------- gt2Message

int fn_80375B58(const GTI2IncomingBufferMessage* message1, const GTI2IncomingBufferMessage* message2) {
    return (s16)(message1->serialNumber - message2->serialNumber);
}

static inline GTBool gti2ConnectionReset(GTI2Connection* connection, GT2Result result) {
    if (connection->state < GTI2Connected) {
        if (connection->initiated) {
            fn_80374778(connection);
            if (!fn_80373AA0(connection, result, NULL, 0)) {
                return 0;
            }
        } else {
            if (connection->state == GTI2AwaitingAcceptReject) {
                connection->freeAtAcceptReject = 1;
            }
            fn_80374778(connection);
        }
    } else if (connection->state != GTI2Closed) {
        fn_80374778(connection);
        if (!fn_80373C54(connection, 1)) {
            return 0;
        }
    }
    return 1;
}

GTBool fn_80376AAC(GTI2Socket* socket, u32 ip, u16 port) {
    GTI2Connection* connection;

    connection = fn_80377D90(socket, ip, port);
    if (socket->receiveDumpCallback && !fn_80373FDC(socket, connection, ip, port, 1, NULL, 0, 0)) {
        return 0;
    }
    if (connection == NULL) {
        return 1;
    }
    if (connection->state == GTI2AwaitingServerChallenge) {
        if (connection->timeout == 0 || fn_8035D728() - connection->startTime < connection->timeout) {
            return 1;
        }
        if (!gti2ConnectionReset(connection, 6)) {
            return 0;
        }
    } else {
        if (!gti2ConnectionReset(connection, 2)) {
            return 0;
        }
    }
    return 1;
}

GTBool fn_80377568(GTI2Connection* connection, const GTByte* message, int len) {
    GTBool closed;
    GTI2OutgoingBufferMessage* msg;
    GTBool result;

    if (!fn_80377028(connection, 5, len + connection->socket->protocolOffset + 7, &closed)) {
        return 0;
    }
    if (closed) {
        return 1;
    }
    fn_8037382C(&connection->outgoingBuffer, message, len);
    msg = fn_8035A598(connection->outgoingBufferList, fn_8035A590(connection->outgoingBufferList) - 1);
    if (!fn_8037442C(connection, connection->outgoingBuffer.buffer + msg->start, msg->len)) {
        result = 0;
    } else {
        result = 1;
        connection->pendingAck = 0;
    }
    if (!result) {
        return 0;
    }
    return 1;
}

GTBool fn_803779DC(GTI2Connection* connection, u16 serialNumber, u16 lastSerialNumber) {
    GTByte buffer[12];
    u16 length;
    int pos = 0;
    int i;

    if (connection->socket->protocolType == 2) {
        length = 7;
        memcpy(buffer, &length, 2);
        pos = 2;
    }
    memcpy(buffer + pos, lbl_8059F980, 2);
    pos += 2;
    buffer[pos++] = 0x65;
    buffer[pos] = (GTByte)(serialNumber >> 8);
    i = pos + 1;
    pos += 2;
    buffer[i] = (GTByte)serialNumber;
    if (serialNumber != lastSerialNumber) {
        buffer[pos] = (GTByte)(lastSerialNumber >> 8);
        i = pos + 1;
        pos += 2;
        buffer[i] = (GTByte)lastSerialNumber;
    }
    if (!fn_8037442C(connection, buffer, pos)) {
        return 0;
    }
    return 1;
}

GTBool fn_80377AD0(GTI2Connection* connection) {
    s16 length;
    GTByte buffer[12];
    GTI2Socket* socket;
    u32 ip;
    u16 port;
    int pos;

    pos = 0;
    socket = connection->socket;
    port = connection->port;
    ip = connection->ip;
    if (socket->protocolType == 2) {
        length = 3;
        memcpy(buffer, &length, 2);
        pos = 2;
    }
    memcpy(buffer + pos, lbl_8059F980, 2);
    buffer[pos += 2] = 0x68;
    return fn_803783C4(socket, ip, port, buffer, pos + 1) != 0;
}

GTBool fn_80377B88(GTI2Connection* connection, GTI2OutgoingBufferMessage* message) {
    GTByte* buffer;
    u32 serialNumber;
    int pos;

    serialNumber = connection->expectedSerialNumber;
    buffer = connection->outgoingBuffer.buffer;
    pos = message->start + connection->socket->protocolOffset + 5;
    buffer[pos++] = serialNumber >> 8;
    buffer[pos] = serialNumber;
    if (!fn_8037442C(connection, connection->outgoingBuffer.buffer + message->start, message->len)) {
        return 0;
    }
    message->lastSend = connection->lastSend;
    if ((s8)connection->outgoingBuffer.buffer[message->start + connection->socket->protocolOffset + 2] == 2) {
        connection->challengeTime = connection->lastSend;
    }
    return 1;
}

// ---------------------------------------------------------------- gt2Socket

int fn_80377D34(const void* elem, int numBuckets) {
    GTI2Connection* connection = *(GTI2Connection**)elem;

    return (connection->ip * connection->port) % (u32)numBuckets;
}

int fn_80377D54(const void* elem1, const void* elem2) {
    GTI2Connection* connection1 = *(GTI2Connection**)elem1;
    GTI2Connection* connection2 = *(GTI2Connection**)elem2;

    if (connection1->ip != connection2->ip) {
        return connection1->ip - connection2->ip;
    }
    return (s16)(connection1->port - connection2->port);
}

GTI2Connection* fn_80377D90(GTI2Socket* socket, u32 ip, u16 port) {
    GTI2Connection* connectionPtr;
    GTI2Connection connection;
    GTI2Connection** connectionPtrPtr;

    connection.ip = ip;
    connection.port = port;
    connectionPtr = &connection;
    connectionPtrPtr = fn_8035B1C4(socket->connections, &connectionPtr);
    if (connectionPtrPtr) {
        return *connectionPtrPtr;
    }
    return NULL;
}

GTBool fn_803786FC(GTI2Socket* socket) {
    gsi_time now;

    now = fn_8035D728();
    return fn_8035B2D0(socket->connections, fn_803785E0, &now) == NULL;
}

void fn_80378838(GTI2Socket* socket) {
    if (socket->error) {
        return;
    }
    socket->error = 1;
    fn_80374BD4(socket);
    if (fn_80373924(socket)) {
        if (socket->callbackLevel) {
            socket->close = 1;
        } else {
            closesocket(socket->socket);
            fn_8035AF74(socket->connections);
            fn_8035A4FC(socket->closedConnections);
            fn_8035E00C(socket);
            fn_8035D724();
        }
    }
}
