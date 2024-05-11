#include "MCAL/SYSTICK.h"
#include "HAL/LED.h"

int ctr = 0;

void blinky1(void){
  while(1){
    if(ctr == 2){
        toggleRed();
        ctr = 0;
    }
  }
}

void blinky2(void){
  while(1){
    if(ctr == 4){
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
    LED_init();
    
    SysTickDisable();
    SysTickInterruptEnable();
    SysTickClkSrc();
    SysTickEnable();
    SysTickPeriodSet(500);    
    
    blinky1();
    blinky2();
  
  return 0;
}
