#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "LED.h"

static const uint8_t redColor = 0x02;
static const uint8_t greenColor = 0x08;

void vPeriodicTask1(void *pvParameters){
	int* ledColor;
	ledColor = (int*) pvParameters;
	for( ;; ){
		GPIO_PORTF_DATA_R ^= *ledColor;
		vTaskDelay(2000 / portTICK_RATE_MS);
		}
	}

void vPeriodicTask2(void *pvParameters){
	int* ledColor;
	ledColor = (int*) pvParameters;
	for( ;; ){
		GPIO_PORTF_DATA_R ^= *ledColor;
		vTaskDelay(4000 / portTICK_RATE_MS);
		}
	}

void vApplicationIdleHook(void) {
    toggleBlue();
}

int main()
{
	LED_init();
	whiteOn();
	xTaskCreate( vPeriodicTask1, "RED", 120, (void *)&redColor, 1, NULL );
	xTaskCreate( vPeriodicTask2, "GREEN", 120, (void *)&greenColor, 1, NULL );
	vTaskStartScheduler();
	for (;;);
}

