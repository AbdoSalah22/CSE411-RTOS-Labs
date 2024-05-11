#ifndef UART_H
#define UART_H

#include "GPIO.h"

#define CLKSPEED 16000000

#define UART_0   0
#define UART_1   1
#define UART_2   2
#define UART_3   3
#define UART_4   4
#define UART_5   5
#define UART_6   6
#define UART_7   7

#define FIFO_ENABLE     0x10
#define PARITY_ENABLE   0x02
#define FIFO_DISABLE     0x00
#define PARITY_DISABLE   0x00

#define WLEN_5          0x00
#define WLEN_6          0x20
#define WLEN_7          0x40
#define WLEN_8          0x60

void UART_init(uint32_t uart, uint32_t baud_rate, uint32_t wlen, uint32_t fifo, uint32_t parity);

#endif //UART_H