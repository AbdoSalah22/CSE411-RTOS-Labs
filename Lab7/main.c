#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "queue.h"
#include "semphr.h"
#include "LED.h"
#include "UART.h"

QueueHandle_t countQueue;
uint32_t counter = 0;

void UART0_Init(void);
void UART0_Transmit(char data);
char UART0_Receive(void);
void UART0_Send_String(char *str);

void BTN1_CHK_TASK(void *pvParameters);
void BTN2_CHK_TASK(void *pvParameters);
void UART_TASK(void *pvParameters);

int main()
{
	GPIO_init(PORTF, PIN0, DIGITAL, INPUT);
	GPIO_init(PORTF, PIN4, DIGITAL, INPUT);

  GPIO_PORTF_PUR_R |= (1U << 0);  // Enable pull-down resistor for pin 0
  GPIO_PORTF_PUR_R |= (1U << 4);  // Enable pull-down resistor for pin 4
	
	LED_init();
	UART0_Init();
	
	countQueue = xQueueCreate(1, sizeof(uint32_t));

	// Create tasks
	xTaskCreate(BTN1_CHK_TASK, "BTN1_CHK_TASK", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(BTN2_CHK_TASK, "BTN2_CHK_TASK", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(UART_TASK, "UART_TASK", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	vTaskStartScheduler();
	for (;;);
}

// Need to write driver for UART later
void UART0_Init(void){
    // Enable clock to GPIOF and GPIOA
    SYSCTL_RCGCGPIO_R |= (1 << 0); // Enable clock to GPIOA
    SYSCTL_RCGCGPIO_R |= (1 << 5); // Enable clock to GPIOF
    
	  // Parameters: UART0, baud rate = 9600, word length = 8-bit, FIFO enabled, no parity
    UART_init(0, 9600, UART_LCRH_WLEN_8, UART_LCRH_FEN, 0);

}

void UART0_Send(char data) {
    while ((UART0_FR_R & (1 << 5)) != 0); // Wait until the Tx FIFO is not full
    UART0_DR_R = data; // Send data
}

char UART0_Receive(void) {
    while ((UART0_FR_R & (1 << 4)) != 0); // Wait until the Rx FIFO is not empty
    return UART0_DR_R; // Read received data
}

void UART0_Send_String(char *str) {
		uint8_t i = 0;
		while(str[i] != '\0') {
			UART0_Send(str[i]);
			i++;
		}
}

void BTN1_CHK_TASK(void *pvParameters) {
    while (1) {
        if (!(GPIO_readPin(PORTF, PIN0))) {
						toggleRed();
            counter++;
						while (!(GPIO_readPin(PORTF, PIN0))){};
        }
			}
    }

void BTN2_CHK_TASK(void *pvParameters) {
    while (1) {
        if (!(GPIO_readPin(PORTF, PIN4))) {
						toggleBlue();
            xQueueOverwrite(countQueue, &counter);
            counter = 0;
						while (!(GPIO_readPin(PORTF, PIN4))){};
        }
			}
    }

void UART_TASK(void *pvParameters) {
    uint32_t receivedValue;
    char buffer[16];
		char label[16] = "Counter: ";
    while (1) {
        if (xQueuePeek(countQueue, &receivedValue, portMAX_DELAY) == pdTRUE) {
            // Send the received value via UART to PC
            sprintf(buffer, "%d\n", receivedValue);
						UART0_Send_String(label);
            UART0_Send_String(buffer);
						UART0_Send('\r');
            xQueueReceive(countQueue, &receivedValue, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
