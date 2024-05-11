#include "FreeRTOS.h"
#include "queue.h"
#include "stdio.h"
#include "LED.h"
#include "UART.h"
#include "tm4c123gh6pm.h"

#define PortF_IRQn 30

static void vIntegerGenerator(void *pvParameters);
static void vStringPrinter( void *pvParameters );

void GPIOF_Handler(void);
void UART0_Send(char data);
char UART0_Receive(void);
void UART0_Send_String(char *str);

QueueHandle_t xIntegerQueue;
QueueHandle_t xStringQueue;



int main(void) {
	
		SYSCTL->RCGCGPIO |= 0x00000020; // 1) F clock
		GPIOF->LOCK = 0x4C4F434B; // 2) unlock PortF PF0
		GPIOF->CR = 0x1F; // allow changes to PF4-0
		GPIOF->AMSEL= 0x00; // 3) disable analog function
		GPIOF->PCTL = 0x00000000; // 4) GPIO clear bit PCTL
		GPIOF->DIR = 0x0E; // 5) PF4,PF0 input, PF3,PF2,PF1 output
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
	
	
	  float full_baud_rate = (float)CLKSPEED / (16 * 9600);
    uint32_t int_baud_rate = (uint16_t)full_baud_rate;
    uint32_t frac_baud_rate_int = (uint8_t)((full_baud_rate - int_baud_rate) * 64 + 0.5);
	
		SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
		SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;

		// Disable UART0 while configuring
		UART0_CTL_R &= ~UART_CTL_UARTEN;

		// Configure baud rate
		UART0_IBRD_R = int_baud_rate;
		UART0_FBRD_R = frac_baud_rate_int;

		// Configure UART line control settings
		UART0_LCRH_R = WLEN_8;  // Set word length
		UART0_LCRH_R |= FIFO_ENABLE;    // Set FIFO settings
		UART0_LCRH_R |= PARITY_DISABLE; // Set parity settings

		// Enable UART0 module, Rx and Tx
		UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;

		// GPIO configuration for PA0/Rx and PA1/Tx
		GPIO_PORTA_DEN_R |= 0x03;   // Enable digital I/O on PA0 and PA1
		GPIO_PORTA_AFSEL_R |= 0x03; // Enable alternate function on PA0 and PA1
		GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0x00000011; // Configure PA0 and PA1 for UART	
    xIntegerQueue = xQueueCreate(10, sizeof(unsigned long));
    xStringQueue = xQueueCreate(10, sizeof(char *));
	
	
    xTaskCreate(vIntegerGenerator, "IntGen", 120, NULL, 1, NULL);
    xTaskCreate(vStringPrinter, "String", 120, NULL, 1, NULL);
		
    vTaskStartScheduler();
	
    for (;;)
        ;
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


/* This task generates integers and sends them to a queue. */
static void vIntegerGenerator(void *pvParameters)
{
    portTickType xLastExecutionTime;
    unsigned long ulValueToSend = 0;
    int i;
    xLastExecutionTime = xTaskGetTickCount();

    for (;;)
    {
        /* This is a periodic task. Block until it is time to run again.
        The task will execute every 200ms. */
        vTaskDelay(1000 / portTICK_RATE_MS);

        /* Send an incrementing number to the queue five times. The values will
        be read from the queue by the interrupt service routine. The interrupt
        service routine always empties the queue so this task is guaranteed to be
        able to write all five values, so a block time is not required. */
        for (i = 0; i < 5; i++)
        {
            xQueueSendToBack(xIntegerQueue, &ulValueToSend, 0);
            ulValueToSend++;
        }

        /* Force an interrupt so the interrupt service routine can read the
        values from the queue. */
        // UART0_Send_String("Generator task - About to generate an interrupt.\n");
        // mainTRIGGER_INTERRUPT();
        // UART0_Send_String("Generator task - Interrupt generated.\n\n");
    }
}



/* This task prints strings received from a queue. */
static void vStringPrinter(void *pvParameters)
{
    char *pcString;

    for (;;)
    {
        /* Block on the queue to wait for data to arrive. */
        xQueueReceive(xStringQueue, &pcString, portMAX_DELAY);

        /* Print out the string received. */
        UART0_Send_String(pcString);
				UART0_Send('\r');
			
    }
}


//Initialize the hardware of Port-F
void PortF_Init(void){

}


void GPIOF_Handler(void){
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    static unsigned long ulReceivedNumber;

    /* The strings are declared static const to ensure they are not allocated to the
    interrupt service routine stack, and exist even when the interrupt service routine
    is not executing. */
    static const char *pcStrings[] =
    {
        "String 0\n",
        "String 1\n",
        "String 2\n",
        "String 3\n"
    };

    /* Loop until the queue is empty. */
    while (xQueueReceiveFromISR(xIntegerQueue,
                                &ulReceivedNumber,
                                &xHigherPriorityTaskWoken) != errQUEUE_EMPTY)
    {
        /* Truncate the received value to the last two bits (values 0 to 3 inc.),
        then send the string that corresponds to the truncated value to the other
        queue. */
        ulReceivedNumber &= 0x03;
        xQueueSendToBackFromISR(xStringQueue,
                                &pcStrings[ulReceivedNumber],
                                &xHigherPriorityTaskWoken);
    }
	
		
	toggleRed();
	
	// clear the interrupt flag of PORTF
  GPIOF->ICR = 0x11;
	
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}


