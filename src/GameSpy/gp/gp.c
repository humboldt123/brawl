#include <types.h>
#include <string.h>
#include <GameSpy/gp/gpi.h>

// Function bodies below are derived from https://github.com/jj-link/brawl.

void fn_8036FD90(void** arg0) {
    fn_8035E00C(*arg0);
    *arg0 = 0;
}

int fn_803703DC(int* a, int b) {
    return *a % b;
}

int fn_803703F0(int *arg0, int *arg1) {
    return *arg0 - *arg1;
}

GPResult fn_80367C28(GPConnection* connection) {
    if (lbl_805A1048 != 1) {
        return 2;
    }
    if (connection == NULL) {
        return 2;
    }
    return fn_803689DC(connection);
}

GPResult fn_8036A1A4(GPConnection* connection, GPIBuffer* outputBuffer, int c) {
    int len = outputBuffer->len;
    int size = outputBuffer->size;
    char* output = outputBuffer->buffer;
    char* tempPtr;

    if (size == len) {
        size += 0x4000;
        tempPtr = fn_8035DFF8(output, size + 1);
        output = tempPtr;
        if (tempPtr == NULL) {
            fn_803732E0(connection, lbl_8048ED80);
            return 1;
        }
    }
    output[len] = (char)c;
    output[len + 1] = '\0';
    outputBuffer->size = size;
    outputBuffer->len++;
    outputBuffer->buffer = output;
    return 0;
}

GPResult fn_8036A3E4(GPConnection* connection, int sock, const char* buffer, int bufferLen, int* closed, int* sent, const char* id) {
    int rcode;

    rcode = send(sock, buffer, bufferLen, 0);
    if (rcode == -1) {
        rcode = fn_8035C658(sock);
        if (rcode != -6 && rcode != -26 && rcode != -76) {
            if (id[0] == 'P' && id[1] == 'R') {
                return 3;
            }
            fn_80373290(connection, 5, lbl_8048ED90);
            fn_8036AAF0(connection, 3, 0);
            return 3;
        }
        *sent = 0;
        *closed = 0;
    } else if (rcode == 0) {
        *sent = 0;
        *closed = 1;
    } else {
        *sent = rcode;
        *closed = 0;
    }
    return 0;
}

GPResult fn_8036A680(GPConnection* connection, void* peer, const char* string) {
    return fn_8036A594(connection, peer, string, strlen(string));
}

GPResult fn_8036A81C(GPConnection* connection, int sock, GPIBuffer* outputBuffer, int* closed, int clipSentData, const char* id) {
    int connClosed;
    int sent;
    int total;
    int remaining;
    char* buffer;
    int pos;
    int len;
    GPResult result;

    len = outputBuffer->len;
    pos = outputBuffer->pos;
    buffer = outputBuffer->buffer;
    remaining = len - pos;
    total = 0;
    if (remaining == 0) {
        return 0;
    }
    do {
        result = fn_8036A3E4(connection, sock, buffer + (pos + total), remaining, &connClosed, &sent, id);
        if (result != 0) {
            return result;
        }
        if (sent != 0) {
            total += sent;
            remaining -= sent;
        }
    } while (sent != 0 && remaining != 0);
    if (clipSentData) {
        if (total > 0) {
            memmove(buffer, buffer + total, remaining + 1);
            len -= total;
        }
    } else {
        pos += total;
    }
    outputBuffer->len = len;
    outputBuffer->pos = pos;
    if (closed != NULL) {
        *closed = connClosed;
    }
    return 0;
}

GPResult fn_8036AA70(GPConnection* connection, GPIBuffer* buffer) {
    int newLen;
    char* data;
    int pos;

    if (buffer == NULL || (data = buffer->buffer) == NULL || (pos = buffer->pos) == 0) {
        return 0;
    }
    newLen = buffer->len - pos;
    buffer->len = newLen;
    if (newLen != 0) {
        memmove(data, data + pos, newLen);
    }
    buffer->buffer[buffer->len] = '\0';
    buffer->pos = 0;
    return 0;
}

void fn_80372DD8(char* dest, const char* src, u32 len) {
    strncpy(dest, src, len);
    dest[len - 1] = '\0';
}

int fn_80372F14(const char* command, const char* key, char* value, int len) {
    char delimiter = key[0];
    const char* str = strstr(command, key);
    char* out;
    int max;
    int i;
    char c;
    u32 keyLen;

    if (str == NULL) {
        return 0;
    }
    keyLen = strlen(key);
    out = value;
    max = len - 1;
    str += keyLen;
    i = 0;
    for (; i < max && (c = *str) != '\0' && c != delimiter; i++, str++, out++) {
        *out = c;
    }
    value[i] = '\0';
    return 1;
}

void fn_8037331C(const char* passwordIn, char* passwordOut) {
    s8 password[0x20];
    s8* p;
    s8 k;
    u32 i;
    u32 len;

    len = strlen(passwordIn);
    fn_8035DA18(0x79707367);
    p = password;
    i = 0;
    while (i < len) {
        k = fn_8035DA30(0, 0xFF);
        *p = k ^ *passwordIn;
        i++;
        passwordIn++;
        p++;
    }
    password[i] = 0;
    fn_8035DAA4((const char*)password, passwordOut, len, 1);
}
