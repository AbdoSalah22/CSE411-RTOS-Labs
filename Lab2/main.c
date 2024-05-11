#include "SYSTICK.h"
#include "LED.h"

int ctr = 0;

uint32 stack_red[40];
uint32 *sp_red = &stack_red[40];

uint32 stack_blue[40];
uint32 *sp_blue = &stack_blue[40];

void blinky1(void){
  while(1){
    if(ctr == 1){
        toggleRed();
        ctr = 0;
    }
  }
}

void blinky2(void){
  while(1){
    if(ctr == 1){
        toggleBlue();
        ctr = 0;
    }
  }
}

void SysTick_Handler(void){
  ctr++;
}

int main()
{
	//red
	*(--sp_red) = 1<<24;
	*(--sp_red) = (uint32)blinky1;
	*(--sp_red) = 0x01;
	*(--sp_red) = 0x01;
	*(--sp_red) = 0x01;
	*(--sp_red) = 0x01;
	*(--sp_red) = 0x01;
	*(--sp_red) = 0x01;
	
	//blue
	*(--sp_blue) = 1<<24;
	*(--sp_blue) = (uint32)blinky2;
	*(--sp_blue) = 0x02;
	*(--sp_blue) = 0x02;
	*(--sp_blue) = 0x02;
	*(--sp_blue) = 0x02;
	*(--sp_blue) = 0x02;
	*(--sp_blue) = 0x02;
	
	LED_init();
	
	SysTickDisable();
	SysTickInterruptEnable();
	SysTickClkSrc();
	SysTickEnable();
	SysTickPeriodSet(500);    
	
//	blinky1();
//	blinky2();

//switch gap
	
	while(1){};
  
  return 0;
}
