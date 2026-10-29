#ifndef GAMESPY_NATNEG_NATNEG_H
#define GAMESPY_NATNEG_NATNEG_H

// GameSpy SDK NAT negotiation (natneg.c, natify.c). Field order follows the
// SDK's _NATNegotiator and _NAT. The functions are unnamed in our symbols; the
// SDK names are noted next to the placeholders.
#include <types.h>
#include <GameSpy/common/darray.h>
#include <GameSpy/common/gsPlatform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gs_hostent {
    char* h_name;        // 0x0
    char** h_aliases;    // 0x4
    s16 h_addrtype;      // 0x8
    s16 h_length;        // 0xA
    char** h_addr_list;  // 0xC
} gs_hostent;

typedef enum NegotiateState {
    ns_initsent,
    ns_initack,
    ns_connectping,
    ns_finished,
    ns_canceled,
    ns_reportsent,
    ns_reportack
} NegotiateState;

typedef void (*NegotiateProgressFunc)(NegotiateState state, void* userdata);
typedef void (*NegotiateCompletedFunc)(int result, int gamesocket, gs_sockaddr_in* remoteaddr, void* userdata);

typedef struct _NATNegotiator {
    int negotiateSock;                         // 0x0
    int gameSock;                              // 0x4
    int cookie;                                // 0x8
    int clientindex;                           // 0xC
    NegotiateState state;                      // 0x10
    int initAckRecv[4];                        // 0x14
    int retryCount;                            // 0x24
    int maxRetryCount;                         // 0x28
    u32 retryTime;                             // 0x2C
    u32 guessedIP;                             // 0x30
    u16 guessedPort;                           // 0x34
    NegotiateProgressFunc progressCallback;    // 0x38
    NegotiateCompletedFunc completedCallback;  // 0x3C
    void* userdata;                            // 0x40
    int result;                                // 0x44
    int connectedSocket;                       // 0x48
    gs_sockaddr_in remoteaddr;                 // 0x4C
} _NATNegotiator;
typedef _NATNegotiator* NATNegotiator;

__static_assert(sizeof(_NATNegotiator) == 0x54, "_NATNegotiator");

// NAT negotiation packet: magic, version, type, cookie, then the payload.
typedef struct NatNegPacket {
    u8 magic[6];      // 0x0
    u8 version;       // 0x6
    u8 packettype;    // 0x7
    int cookie;       // 0x8
    u8 porttype;      // 0xC (Init/InitAck payload)
} NatNegPacket;

typedef struct AddressMapping {
    u32 privateIp;
    u16 privatePort;
    u32 publicIp;
    u16 publicPort;
} AddressMapping;

typedef struct _NAT {
    char brand[32];               // 0x0
    char model[32];               // 0x20
    char firmware[64];            // 0x40
    int ipRestricted;             // 0x80
    int portRestricted;           // 0x84
    int promiscuity;              // 0x88
    int natType;                  // 0x8C
    int mappingScheme;            // 0x90
    AddressMapping mappings[4];   // 0x94
    int qr2Compatible;            // 0xD4
} NAT;

__static_assert(sizeof(NAT) == 0xD8, "NAT");

typedef void (*NatDetectionResultsFunc)(int success, NAT nat);

// the list of active negotiators
extern DArray lbl_805A10F0;

gs_hostent* fn_8035CF8C(void);        // getlocalhost
int fn_8035D140(u32* addr);           // IsPrivateIP

u32 fn_8038563C(void);                                       // GetLocalIP (natneg.c)
void fn_80385CC4(void);                                      // NNFreeNegotiateList
u32 fn_80385CF8(void);                                       // GetLocalIP (natify.c)
int fn_80385A00(int sock, NAT* nat);                         // NatifyThink
int fn_80385A04(NAT* nat);                                   // DetermineNatType
int fn_80386238(int sock);                                   // NNThink detection step
void fn_80386658(int cookie);                                // NNCancel
void fn_8038724C(NATNegotiator neg, NatNegPacket* p, gs_sockaddr_in* fromaddr);  // ProcessPacket

#ifdef __cplusplus
}
#endif

#endif
