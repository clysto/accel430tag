$MSP430GCC/bin/msp430-elf-gdb -nx -batch -ex "file build/MSP430FR5969.elf" -ex "disassemble $*"
