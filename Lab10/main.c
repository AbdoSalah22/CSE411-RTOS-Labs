#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "queue.h"
#include "semphr.h"
#include "LED.h"
#include "UART.h"
#include "delay.h"

#define PortF_IRQn 30

SemaphoreHandle_t mutex;
SemaphoreHandle_t semaphore;


//Initialize the hardware of Port-F
void PortF_Init(void){
		SYSCTL->RCGCGPIO |= 0x00000020; // 1) F clock
		GPIOF->LOCK = 0x4C4F434B; // 2) unlock PortF PF0
		GPIOF->CR = 0x11; // allow changes to PF4-0
		GPIOF->AMSEL= 0x00; // 3) disable analog function
		GPIOF->PCTL = 0x00000000; // 4) GPIO clear bit PCTL
		GPIOF->DIR = 0x0E; // 5) PF4,PF0 input, PF3,PF2,PF1 output
		GPIOF->AFSEL = 0x00; // 6) no alternate function
		GPIOF->PUR = 0x11; // enable pullup resistors on PF4,PF0
		GPIOF->DEN = 0x11; // 7) enable digital pins PF4-PF0
		GPIOF->DATA = 0x00;
	
		// Setup the interrupt on PortF
		GPIOF->ICR = 0x11; // Clear any Previous Interrupt
		GPIOF->IM |=0x11; // Unmask the interrupts for PF0 and PF4
		GPIOF->IS |= 0x11; // Make bits PF0 and PF4 level sensitive
		GPIOF->IEV &= ~0x11; // Sense on Low Level
		NVIC_EnableIRQ(PortF_IRQn); // Enable the Interrupt for PortF in NVIC
	  NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00600000;  // Set priority to 3 for IRQ30
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




void CounterTask(void *pvParameters) {
		char buffer[16] = "CounterTask";
    while(1){
			xSemaphoreTake( mutex, portMAX_DELAY);
			UART0_Send_String(buffer);
			UART0_Send_String("\r\n");
			delayMs(100);
			UART0_Send_String("0\r\n");
			delayMs(100);
			UART0_Send_String("1\r\n");
			delayMs(100);
			UART0_Send_String("2\r\n");
			delayMs(100);
			UART0_Send_String("3\r\n");
			delayMs(100);
			UART0_Send_String("4\r\n");
			delayMs(100);
			UART0_Send_String("5\r\n");
			delayMs(100);
			xSemaphoreGive(mutex);
			delayMs(1000);
			}
}


void LedTogglerTask(void *pvParameters) {
		xSemaphoreTake(semaphore, 0);
		char buffer[16] = "LedTogglerTask";
    while(1){
			xSemaphoreTake( semaphore, portMAX_DELAY );
			xSemaphoreTake( mutex, portMAX_DELAY);
			
			UART0_Send_String(buffer);
			UART0_Send_String("\r\n");
			toggleRed();
			delayMs(1000);
			
			xSemaphoreGive(mutex);  
    }
}


void GPIOF_Handler(void){
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semaphore, &xHigherPriorityTaskWoken );
	GPIOF->ICR = 0x11;
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


int main(){
	PortF_Init();
	LED_init();
	UART0_Init();
	
	mutex = xSemaphoreCreateMutex();
	semaphore = xSemaphoreCreateBinary();
	
	// Create tasks
	xTaskCreate(CounterTask, "CounterTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(LedTogglerTask, "LedTogglerTask", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

	vTaskStartScheduler();
	for (;;);
}
