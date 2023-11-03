#include <driverlib.h>
#include <stdlib.h>
#include <string.h>

void UART_init() {
    // SMCLK = 1MHz
    // Baud Rate = 9600 bps
    // 参数计算工具: https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
    EUSCI_A_UART_initParam param = {0};
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
    param.clockPrescalar = 6;
    param.firstModReg = 8;
    param.secondModReg = 17;
    param.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    EUSCI_A_UART_init(EUSCI_A0_BASE, &param);
    EUSCI_A_UART_enable(EUSCI_A0_BASE);
}
