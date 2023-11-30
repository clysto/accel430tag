#include <driverlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "accel.h"
#include "bs.h"
#include "led.h"
#include "spi.h"
#include "uart.h"

static void __attribute__((naked, section(".crt_0042"), used)) disable_watchdog(void) {
    WDTCTL = WDTPW | WDTHOLD;
}

int main(void) {
    LED_init();
    PMM_unlockLPM5();
    SPI_init();
    ACCEL_init();
    // Tx (P1.7) 配置为输出
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN7);

    LED_turnOnLED2();

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

    uint8_t data[32];
    int n = 0;
    ACCEL_result result;

    for (;;) {
        if (n >= 100) {
            LED_toggleLED1();
            n = 0;
        }
        ACCEL_singleSample(&result);
        // repeat 5 times to make sure the data is sent
        for (int i = 0; i < 5; i++) {
            memcpy(data + i * sizeof(result), &result, sizeof(result));
        }
        BS_transmitFrame(data, 32);
        __delay_cycles(160000);
        n++;
    }

    __bis_SR_register(GIE);
    return 0;
}
