#include "SYSTICK.h"
#include "LED.h"
#include "scheduler.h"

void task1(){
	whiteOff();
	redOn();
}

void task2(){
	whiteOff();
	blueOn();
}

void task3(){
	whiteOff();
	greenOn();
}

int main()
{
	LED_init();
	SysTick_init(5);
	create_task(task1, 2000);
	create_task(task2, 4000);
	create_task(task3, 6000);
	
	while(1){
		Tasks_Scheduler();
	}
  
  return 0;
}
