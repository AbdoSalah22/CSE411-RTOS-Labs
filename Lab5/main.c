#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "LED.h"

static const uint8_t redColor = 0x02;
static const uint8_t blueColor = 0x04;
static const uint8_t greenColor = 0x08;

void vPeriodicTask1(void *pvParameters){
	int* ledColor;
	ledColor = (int*) pvParameters;
	for( ;; ){
		GPIO_PORTF_DATA_R ^= *ledColor;
		vTaskDelay(1000 / portTICK_RATE_MS);
		}
	}

void vPeriodicTask2(void *pvParameters){
	int* ledColor;
	ledColor = (int*) pvParameters;
	for( ;; ){
		GPIO_PORTF_DATA_R ^= *ledColor;
		vTaskDelay(2000 / portTICK_RATE_MS);
		}
	}

void vPeriodicTask3(void *pvParameters){
	int* ledColor;
	ledColor = (int*) pvParameters;
	for( ;; ){
		GPIO_PORTF_DATA_R ^= *ledColor;
		vTaskDelay(3000 / portTICK_RATE_MS);
		}
	}

int main()
{
	LED_init();
	xTaskCreate( vPeriodicTask1, "RED", 120, (void *)&redColor, 1, NULL );
	xTaskCreate( vPeriodicTask2, "BLUE", 120, (void *)&blueColor, 1, NULL );
	xTaskCreate( vPeriodicTask3, "GREEN", 120, (void *)&greenColor, 1, NULL );
	vTaskStartScheduler();
	for (;;);
}

