#include "scheduler.h"

osThread threads[NUM_OF_TASKS];
int threadCount = 0;

void create_task (void (*Task)(), int ms_periodicity ){
	if (threadCount < NUM_OF_TASKS){
		threads[threadCount].sp = Task;
		threads[threadCount].periodicity = ms_periodicity;
		threadCount++;	
	}
}


int check;
void Tasks_Scheduler(void){
	if (flag == 1){
		flag = 0;
							__asm(" CPSID I");
		for(int i = 0; i < NUM_OF_TASKS; ++i){
			check = (counter*5) % threads[i].periodicity;
			if (check == 0){
				threads[i].sp();
			}
		}
	}
							__asm(" CPSIE I");
}