#include <driverlib.h>

void LED_init() {
  // LED1 <---> P4.7
  GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN7);
  // 默认熄灭
  GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);
}

void LED_toggleLED1() { GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN7); }

void LED_turnOnLED1() { GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN7); }

void LED_turnOffLED1() { GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7); }
