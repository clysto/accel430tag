#ifndef FM0_H
#define FM0_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define LSB_FIRST 0
#define MSB_FIRST 1

uint8_t FM0_encode(uint8_t const *bitstream, uint8_t *encodedBitstream, int size, int prevBit) ;

#endif