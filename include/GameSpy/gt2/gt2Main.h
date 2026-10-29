#ifndef GAMESPY_GT2_GT2MAIN_H
#define GAMESPY_GT2_GT2MAIN_H

// GameSpy Transport 2 internal types (the SDK's gt2Main.h). Field order and
// names follow the SDK; offsets are checked against the matched code. The
// functions are still unnamed in our symbols, so their SDK names are noted
// next to the placeholders (HYPOTHESIS: by behavior and address order).
#include <types.h>
#include <GameSpy/common/darray.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int GTBool;
typedef u8 GTByte;
typedef int GT2Result;
typedef u32 gsi_time;

typedef struct GTI2Socket GTI2Socket;
typedef struct GTI2Connection GTI2Connection;

typedef void (*gt2ConnectAttemptCallback)(GTI2Socket* socket, GTI2Connection* connection, u32 ip, u16 port, int latency, GTByte* message, int len);
typedef void (*gt2SocketErrorCallback)(GTI2Socket* socket);
typedef void (*gt2DumpCallback)(GTI2Socket* socket, GTI2Connection* connection, u32 ip, u16 port, GTBool reset, const GTByte* message, int len);
typedef GTBool (*gt2UnrecognizedMessageCallback)(GTI2Socket* socket, u32 ip, u16 port, GTByte* message, int len);

typedef void (*gt2ConnectedCallback)(GTI2Connection* connection, GT2Result result, GTByte* message, int len);
typedef void (*gt2ReceivedCallback)(GTI2Connection* connection, GTByte* message, int len, GTBool reliable);
typedef void (*gt2ClosedCallback)(GTI2Connection* connection, int reason);
typedef void (*gt2PingCallback)(GTI2Connection* connection, int latency);
typedef void (*gt2SendFilterCallback)(GTI2Connection* connection, int filterID, const GTByte* message, int len, GTBool reliable);
typedef void (*gt2ReceiveFilterCallback)(GTI2Connection* connection, int filterID, GTByte* message, int len, GTBool reliable);

typedef enum GTI2ConnectionState {
    GTI2AwaitingServerChallenge,
    GTI2AwaitingClientChallenge,
    GTI2AwaitingClientResponse,
    GTI2AwaitingAcceptance,
    GTI2AwaitingAcceptReject,
    GTI2Connected,
    GTI2Closing,
    GTI2Closed
} GTI2ConnectionState;

typedef struct GTI2Buffer {
    GTByte* buffer;  // 0x0
    int size;        // 0x4
    int len;         // 0x8
} GTI2Buffer;

// one buffered reliable message in a connection's outgoing/incoming lists
typedef struct GTI2OutgoingBufferMessage {
    int start;              // 0x0
    int len;                // 0x4
    int type;               // 0x8
    gsi_time lastSend;      // 0xC
    u16 serialNumber;       // 0x10
} GTI2OutgoingBufferMessage;

typedef struct GTI2IncomingBufferMessage {
    int start;              // 0x0
    int len;                // 0x4
    int type;               // 0x8
    u16 serialNumber;       // 0xC
} GTI2IncomingBufferMessage;

struct GTI2Socket {
    int socket;                                              // 0x0
    u32 ip;                                                  // 0x4
    u16 port;                                                // 0x8
    HashTable connections;                                   // 0xC
    DArray closedConnections;                                // 0x10
    GTBool close;                                            // 0x14
    GTBool error;                                            // 0x18
    int callbackLevel;                                       // 0x1C
    gt2ConnectAttemptCallback connectAttemptCallback;        // 0x20
    gt2SocketErrorCallback socketErrorCallback;              // 0x24
    gt2DumpCallback sendDumpCallback;                        // 0x28
    gt2DumpCallback receiveDumpCallback;                     // 0x2C
    gt2UnrecognizedMessageCallback unrecognizedMessageCallback; // 0x30
    void* data;                                              // 0x34
    int outgoingBufferSize;                                  // 0x38
    int incomingBufferSize;                                  // 0x3C
    int protocolType;                                        // 0x40
    int protocolOffset;                                      // 0x44
};

struct GTI2Connection {
    u32 ip;                                    // 0x0
    u16 port;                                  // 0x4
    GTI2Socket* socket;                        // 0x8
    GTI2ConnectionState state;                 // 0xC
    GTBool initiated;                          // 0x10
    GTBool freeAtAcceptReject;                 // 0x14
    GT2Result connectionResult;                // 0x18
    gsi_time startTime;                        // 0x1C
    gsi_time timeout;                          // 0x20
    int callbackLevel;                         // 0x24
    gt2ConnectedCallback connectedCallback;    // 0x28
    gt2ReceivedCallback receivedCallback;      // 0x2C
    gt2ClosedCallback closedCallback;          // 0x30
    gt2PingCallback pingCallback;              // 0x34
    GTByte* initialMessage;                    // 0x38
    int initialMessageLen;                     // 0x3C
    void* data;                                // 0x40
    GTI2Buffer incomingBuffer;                 // 0x44
    GTI2Buffer outgoingBuffer;                 // 0x50
    DArray incomingBufferList;                 // 0x5C
    DArray outgoingBufferList;                 // 0x60
    u16 serialNumber;                          // 0x64
    u16 expectedSerialNumber;                  // 0x66
    char response[32];                         // 0x68
    gsi_time lastSend;                         // 0x88
    gsi_time challengeTime;                    // 0x8C
    GTBool pendingAck;                         // 0x90
    gsi_time pendingAckTime;                   // 0x94
    DArray sendFilters;                        // 0x98
    DArray receiveFilters;                     // 0x9C
};

__static_assert(sizeof(GTI2Connection) == 0xA0, "GTI2Connection");
__static_assert(sizeof(GTI2OutgoingBufferMessage) == 0x14, "GTI2OutgoingBufferMessage");

// the two magic bytes at the start of every GT2 message
extern u8 lbl_8059F980[2];

GTBool fn_80373780(GTI2Buffer* buffer, int size);                                    // gti2AllocateBuffer
int fn_803737D8(GTI2Buffer* buffer);                                                 // gti2GetBufferFreeSpace
void fn_803737E8(GTI2Buffer* buffer, GTByte b);                                      // gti2BufferWriteByte
void fn_80373800(GTI2Buffer* buffer, u16 s);                                         // gti2BufferWriteUShort
void fn_8037382C(GTI2Buffer* buffer, const GTByte* data, int len);                   // gti2BufferWriteData
void fn_803738B8(GTI2Buffer* buffer, int start, int shortenBy);                      // gti2BufferShorten
GTBool fn_80373924(GTI2Socket* socket);                                              // gti2SocketErrorCallback
GTBool fn_80373AA0(GTI2Connection* connection, GT2Result result, GTByte* message, int len); // gti2ConnectedCallback
GTBool fn_80373C54(GTI2Connection* connection, int reason);                          // gti2ClosedCallback
GTBool fn_80373D10(GTI2Connection* connection, int latency);                         // gti2PingCallback
GTBool fn_80373DCC(GTI2Connection* connection, int filterID, const GTByte* message, int len, GTBool reliable); // gti2SendFilterCallback
GTBool fn_80373ED4(GTI2Connection* connection, int filterID, GTByte* message, int len, GTBool reliable);       // gti2ReceiveFilterCallback
GTBool fn_80373FDC(GTI2Socket* socket, GTI2Connection* connection, u32 ip, u16 port, GTBool reset, const GTByte* message, int len, GTBool send); // gti2DumpCallback
GT2Result fn_803741A4(GTI2Socket* socket, GTI2Connection** connection, u32 ip, u16 port);  // gti2NewOutgoingConnection
GT2Result fn_803741F8(GTI2Socket* socket, GTI2Connection** connection, u32 ip, u16 port);  // gti2NewIncomingConnection
GTBool fn_80374488(GTI2Connection* connection, gsi_time now);                        // gti2CheckTimeout
GTBool fn_80374584(GTI2Connection* connection, gsi_time now);                        // gti2ConnectionThink
void fn_803746D0(GTI2Connection* connection, GTBool hard);                           // gti2CloseConnection
void fn_80374778(GTI2Connection* connection);                                        // gti2ConnectionCleanup
void fn_803747D4(GTI2Connection* connection);                                        // gti2FreeSocketConnection
void fn_80374880(GTI2Socket* socket);                                                // gti2CloseSocket
void fn_803748C4(GTI2Socket* socket);                                                // gti2Think
int fn_80374AC4(GTI2Connection* connection, const GTByte* message, int len, GTBool reliable); // gti2Send
void fn_80374BC8(void* elem, void* clientData);                                      // gti2CloseConnectionsMap
void fn_80374BD4(GTI2Socket* socket);                                                // gti2CloseAllConnectionsHard
int fn_80374BF0(GTI2Connection* connection);                                         // incoming buffer free space
int fn_80374C00(GTI2Connection* connection);                                         // outgoing buffer free space
GTBool fn_80376DF4(GTI2Socket* socket);                                              // gti2ReceiveMessages
GTBool fn_80376AAC(GTI2Socket* socket, u32 ip, u16 port);                            // gti2HandleConnectionReset
GTBool fn_80377028(GTI2Connection* connection, int type, int size, GTBool* closed);  // gti2BeginReliableMessage
GTBool fn_80377568(GTI2Connection* connection, const GTByte* message, int len);      // gti2SendReliableMessage
void fn_8037765C(GTI2Connection* connection);                                        // gti2SendClosing
GTBool fn_80377724(GTI2Connection* connection);                                      // gti2SendKeepAlive
GTBool fn_80377918(GTI2Connection* connection);                                      // gti2SendAck
GTBool fn_803779DC(GTI2Connection* connection, u16 serialNumber, u16 lastSerialNumber); // gti2SendNack
GTBool fn_80377AD0(GTI2Connection* connection);                                      // gti2SendClosed
GTBool fn_80377B88(GTI2Connection* connection, GTI2OutgoingBufferMessage* message);  // gti2ResendMessage
GTBool fn_8037442C(GTI2Connection* connection, const GTByte* message, int len);      // gti2ConnectionSendData
GTBool fn_80377C44(GTI2Connection* connection, const GTByte* message, int len, GTBool reliable); // gti2InternalSend
int fn_80377D34(const void* elem, int numBuckets);                                   // gti2ConnectionHash
int fn_80377D54(const void* elem1, const void* elem2);                               // gti2ConnectionCompare
GTI2Connection* fn_80377D90(GTI2Socket* socket, u32 ip, u16 port);                   // gti2SocketFindConnection
void fn_80378018(GTI2Socket* socket);                                                // gti2FreeSocket
GT2Result fn_80378084(GTI2Socket* socket, GTI2Connection** connection, u32 ip, u16 port); // gti2NewSocketConnection
void fn_803782F8(GTI2Connection* connection);                                        // gti2FreeClosedConnection
GTBool fn_803783C4(GTI2Socket* socket, u32 ip, u16 port, const GTByte* message, int len); // gti2SocketSend
int fn_803785E0(void* elem, void* clientData);                                       // gti2ConnectionsThinkMap
GTBool fn_803786FC(GTI2Socket* socket);                                              // gti2SocketConnectionsThink
void fn_80378748(GTI2Socket* socket);                                                // gti2FreeClosedConnections
void fn_80378838(GTI2Socket* socket);                                                // gti2CloseSocketSafe
void fn_80378B50(const GTByte** message, int* len);                                  // gti2MessageCheck

#ifdef __cplusplus
}
#endif

#endif
