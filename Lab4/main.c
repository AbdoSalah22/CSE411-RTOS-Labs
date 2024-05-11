#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "LED.h"


void vPeriodicTask(void *pvParameters){
	for( ;; ){
		toggleRed();
		vTaskDelay(1000 / portTICK_RATE_MS);
		}
	}

int main()
{
	LED_init();
	xTaskCreate( vPeriodicTask, "RED", 240, NULL, 1, NULL );
	vTaskStartScheduler();
	for (;;);
	
}


// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06



