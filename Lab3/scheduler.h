#ifndef scheduler_h
#define scheduler_h

#include "SYSTICK.h"

#define NUM_OF_TASKS 3


/*Creating a Thread Control Block*/
typedef struct{
    void (*sp)(); //Pointer to function
    int periodicity;   
}osThread;

extern osThread threads[NUM_OF_TASKS];
extern int threadCount;

void create_task (void (*Task)(), int ms_periodicity );
void Tasks_Scheduler  (void);

#endif	//scheduler_h