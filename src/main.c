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

    // 初始化 TIMER
    Timer_A_initUpModeParam initContParam = {0};
    // 1 MHz
    initContParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    // 1MHz / 20 = 50KHz
    initContParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_20;
    // 允许中断 CCR0 -> 0 时触发中断
    initContParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
    initContParam.timerClear = TIMER_A_DO_CLEAR;
    initContParam.startTimer = true;
    // 20ms 触发一次中断
    initContParam.timerPeriod = 50000 / 50;
    Timer_A_initUpMode(TIMER_A0_BASE, &initContParam);

    __bis_SR_register(GIE);
    return 0;
}

int n = 0;

// 50Hz 中断
void __attribute__((interrupt(TIMER0_A0_VECTOR))) Timer_A0_ISR(void) {
    uint8_t data[32];
    ACCEL_result result;
    // 每两秒闪烁一次 LED1
    if (n >= 49) {
        LED_toggleLED1();
        n = 0;
    }
    ACCEL_singleSample(&result);
    for (int i = 0; i < 5; i++) {
        memcpy(data + i * sizeof(result), &result, sizeof(result));
    }
    BS_transmitFrame(data, 32);
    n++;
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
}
