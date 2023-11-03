#include <driverlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bs.h"
#include "led.h"
#include "uart.h"

static void __attribute__((naked, section(".crt_0042"), used)) disable_watchdog(void) {
    WDTCTL = WDTPW | WDTHOLD;
}

int main(void) {
    LED_init();
    UART_init();
    PMM_unlockLPM5();
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN7);

    LED_turnOnLED1();

    // 禁用 FRAM 等待周期以允许 8 MHz 以上的时钟操作
    FRAMCtl_configureWaitStateControl(FRAMCTL_ACCESS_TIME_CYCLES_1);
    // 配置 DCO 为 16 MHz
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);
    // MCLK 时钟源配置为 DCO
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    // SMCLK 时钟源配置为 DCO (16MHz / 16 = 1MHz)
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_16);
    CS_enableClockRequest(CS_MCLK);
    CS_enableClockRequest(CS_SMCLK);

    uint8_t data[64];

    for (int i = 0; i < 64; i++) {
        data[i] = i;
    }

    int n = 0;

    for (;;) {
        if (n >= 10) {
            LED_toggleLED1();
            n = 0;
        }
        BS_transmitFrame(data, 64);
        __delay_cycles(8000);
        n++;
    }

    __bis_SR_register(GIE);
    return 0;
}
