#include <stdint.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include "LED.h"
#include "tm4c123gh6pm.h"

#define PortF_IRQn 30
void PortF_Init(void);

xSemaphoreHandle xBinarySemaphore;

void semaphoreTask(void *pvParameters){
	xSemaphoreTake(xBinarySemaphore, 0);
	for(;;){
		xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );
		toggleBlue();
	}
}


void vPeriodicTask(void *pvParameters){
	for(;;) {
		toggleRed();
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}


//Initialize the hardware of Port-F
void PortF_Init(void){
		SYSCTL->RCGCGPIO |= 0x00000020; // 1) F clock
		GPIOF->LOCK = 0x4C4F434B; // 2) unlock PortF PF0
		GPIOF->CR = 0x1F; // allow changes to PF4-0
		GPIOF->AMSEL= 0x00; // 3) disable analog function
		GPIOF->PCTL = 0x00000000; // 4) GPIO clear bit PCTL
		GPIOF->DIR = 0x0E; // 5) PF4,PF0 input, PF3,PF2,PF1 output
		GPIOF->AFSEL = 0x00; // 6) no alternate function
		GPIOF->AFSEL = 0x00; // 6) no alternate function
		GPIOF->PUR = 0x11; // enable pullup resistors on PF4,PF0
		GPIOF->DEN = 0x1F; // 7) enable digital pins PF4-PF0
		GPIOF->DATA = 0x00;
	
		// Setup the interrupt on PortF
		GPIOF->ICR = 0x11; // Clear any Previous Interrupt
		GPIOF->IM |=0x11; // Unmask the interrupts for PF0 and PF4
		GPIOF->IS |= 0x11; // Make bits PF0 and PF4 level sensitive
		GPIOF->IEV &= ~0x11; // Sense on Low Level
		NVIC_EnableIRQ(PortF_IRQn); // Enable the Interrupt for PortF in NVIC
	  NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00500000;  // Set priority to 5 for IRQ30
}


void GPIOF_Handler(void){
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	
	//Give the semaphore to the Task named handler, use xSemaphoreGiveFromISR, refer to FreeRTOS documentation.
	xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );
	
	// clear the interrupt flag of PORTF
  GPIOF->ICR = 0x11;
	
	/* Giving the semaphore may have unblocked a task - if it did and the
	unblocked task has a priority equal to or above the currently executing
	task then xHigherPriorityTaskWoken will have been set to pdTRUE and
	portEND_SWITCHING_ISR() will force a context switch to the newly unblocked
	higher priority task.
	
	NOTE: The syntax for forcing a context switch within an ISR varies between
	FreeRTOS ports. The portEND_SWITCHING_ISR() macro is provided as part of
	the Corte M3 port layer for this purpose. taskYIELD() must never be called
	from an ISR!, use portEND_SWITCHING_ISR, refer to FreeRTOS documentation.*/
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


int main( void ) {
	PortF_Init();
	__ASM("CPSIE i");
	
	vSemaphoreCreateBinary(xBinarySemaphore);
	// xSemaphoreCreateBinary();
	
	if( xBinarySemaphore != NULL ) {
	xTaskCreate( semaphoreTask, "semaphoreTask", 120, NULL, 3, NULL );
	xTaskCreate( vPeriodicTask, "Periodic", 120, NULL, 1, NULL );
	vTaskStartScheduler();
	}
	
	for( ;; );
}
