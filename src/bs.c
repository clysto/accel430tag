#include "bs.h"

#include <driverlib.h>
#include <stdlib.h>
#include <string.h>

#include "fm0.h"

void BS_transmitData(uint8_t const* data, size_t size) {
    // R12: 一个符号多少个脉冲
    // R13: 数据指针
    // R14: 数据长度
    __asm__ __volatile__(
        "MOV %0, R12\n"
        "MOV %1, R13\n"
        "MOV %2, R14\n"
        "CALLA #BS_transmitDataInternal\n"
        :
        : "r"(30), "m"(data), "r"(size)
        : "R10", "R11", "R12", "R13", "R14", "R15");
}

void BS_transmitFrame(uint8_t const* data, size_t size) {
    int frameSize = 7 + (size + 4) * 2;
    uint8_t frame[frameSize + 1];
    memset(frame, 0, sizeof(frame));
    // 前导码 1010 1010 1010 1010 1010 1010 1010 1010 1010 1010 1010 1101 0010 0011
    frame[0] = 0xaa;
    frame[1] = 0xaa;
    frame[2] = 0xaa;
    frame[3] = 0xaa;
    frame[4] = 0xaa;
    frame[5] = 0xad;
    frame[6] = 0x23;
    uint8_t length[4];
    for (int i = 0; i < 4; i++) {
        length[i] = (size >> (i * 8)) & 0xFF;
    }
    uint8_t status = FM0_encode(length, frame + 7, 4, 1);
    FM0_encode(data, frame + 15, size, status);
    // FM0 End-of-Signaling
    if ((frame[frameSize - 1] & 1) == 0) {
        // Add dummy 1
        frame[frameSize] = 0xC0;
        frameSize++;
    } else {
        frame[frameSize] = 0x00;
    }
    BS_transmitData(frame, frameSize);
}
