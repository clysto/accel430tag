#include "fm0.h"

#include <stdint.h>

#define SETBIT(bitstream, index) bitstream[(index) / 8] |= (0x80U >> ((index) % 8))

uint8_t const SYMS[2][2][2] = {
    {{1, 0}, {0, 1}},  // symbol 0
    {{1, 1}, {0, 0}},  // symbol 1
};

uint8_t FM0_encode(uint8_t const *bitstream, uint8_t *encodedBitstream, int size, int prevBit) {
    uint8_t a, b = prevBit;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 8; j++) {
            int cur_bit = (bitstream[i] >> (7 - j)) & 1;
            a = SYMS[cur_bit][b][0];
            b = SYMS[cur_bit][b][1];
            if (a) SETBIT(encodedBitstream, (i * 8 + j) * 2);
            if (b) SETBIT(encodedBitstream, (i * 8 + j) * 2 + 1);
        }
    }
    return b;
}
