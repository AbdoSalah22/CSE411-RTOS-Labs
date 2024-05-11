#include "UART.h"


void UART_init(uint32_t uart, uint32_t baud_rate, uint32_t wlen, uint32_t fifo, uint32_t parity){
    float full_baud_rate = (float)CLKSPEED / (16 * baud_rate);
    uint32_t int_baud_rate = (uint16_t)full_baud_rate;
    uint32_t frac_baud_rate_int = (uint8_t)((full_baud_rate - int_baud_rate) * 64 + 0.5);
	
    switch(uart){
			case 0:
				// Enable UART0 and GPIO port A
				SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
				SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;

				// Disable UART0 while configuring
				UART0_CTL_R &= ~UART_CTL_UARTEN;

				// Configure baud rate
				UART0_IBRD_R = int_baud_rate;
				UART0_FBRD_R = frac_baud_rate_int;

				// Configure UART line control settings
				UART0_LCRH_R = wlen;  // Set word length
				if (fifo) UART0_LCRH_R |= fifo;    // Set FIFO settings
				if (parity) UART0_LCRH_R |= parity; // Set parity settings

				// Enable UART0 module, Rx and Tx
				UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;

				// GPIO configuration for PA0/Rx and PA1/Tx
				GPIO_PORTA_DEN_R |= 0x03;   // Enable digital I/O on PA0 and PA1
				GPIO_PORTA_AFSEL_R |= 0x03; // Enable alternate function on PA0 and PA1
				GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0x00000011; // Configure PA0 and PA1 for UART
				break;
						
						
      case 5:
        SYSCTL_RCGCUART_R |= 0x20;
        SYSCTL_RCGCGPIO_R |= 0x10;

        UART5_CTL_R &= ~0x01;
        UART5_IBRD_R = int_baud_rate;
        UART5_FBRD_R = frac_baud_rate_int;
        UART5_CC_R = 0x00;
				
				UART5_LCRH_R |= wlen;
        if (fifo) UART5_LCRH_R |= fifo;
        if (parity) UART5_LCRH_R |= parity;

        // enable UART5 module, Rx and Tx
        UART5_CTL_R = 0x301;

        // GPIO configuration for PE4/Rx and PE5/Tx
        SYSCTL_RCGCGPIO_R |= 0x10;
        GPIO_PORTE_DEN_R = 0x30;
        GPIO_PORTE_AFSEL_R = 0x30;
        GPIO_PORTE_AMSEL_R = 0;
        GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & 0xFF00FFFF) | 0x00110000; // configure PE4 and PE5 for UART
        break;

        default:
        //other cases to be implemented
        break;
    }
}



