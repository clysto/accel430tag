#!/usr/bin/env python3

import sys

N = 3
if len(sys.argv) > 1:
    N = int(sys.argv[1])


def bit_loop(i, n=3):
    assert n >= 3
    if i == 0:
        print("    MOV.B       @R_DATA+,       R_BYTE")
    else:
        print("    RLA.B       R_BYTE")
    print(
        f"""    MOV.B       R_PULSE,        R_LOOP
B{i}_LOOP:
    MOV.B       R_BYTE,         &TX_OUT
    NOPN        {n+4}
    MOV.B       R_ZERO,         &TX_OUT    
    NOPN        {n}
    DEC.B       R_LOOP
    JNZ         B{i}_LOOP
"""
    )


print(
    """#include <msp430.h>

; 输入寄存器
#define R_PULSE     R12
#define R_DATA      R13
#define R_DATASIZE  R14

#define R_ZERO      R10
#define R_BYTE      R11
#define R_LOOP      R15
; 发射天线连接到 P2.7
#define TX_PIN      BIT7
#define TX_OUT      P2OUT

.global BS_transmitDataInternal

.macro NOPN num
    .rept \\num
        NOP
    .endr
.endm

BS_transmitDataInternal:
    MOV.B       #0x00,          R_ZERO
    MOV.B       #0x00,          R_BYTE
"""
)

print("SEND_BYTE:")
for i in range(8):
    bit_loop(i, N)

print(
    """    DEC.B       R_DATASIZE
    ; 如果没有发送完成继续发送下一个字节
    JNZ         SEND_BYTE
    MOV.B       R_ZERO,         &TX_OUT
    RETA
.end
"""
)
