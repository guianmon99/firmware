#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include <stdlib.h>
#include <string.h>

/*-----------------------------------------------------------*/

/* The Tx and Rx tasks as described at the top of this file. */
static void Task1( void *pvParameters );
static void task2( void *pvParameters );


/*-----------------------------------------------------------*/

/* The queue used by the Tx and Rx tasks, as described at the top of this
file. */


typedef struct tarea{
	int timer1;
	int flag;
	QueueHandle_t  Pq;
}task;



typedef struct mensaje{
	char mess[30];
	int task;
}message;

QueueHandle_t xQueue;

int main( void )
{

	task ts;
	task ts2;
	ts2.flag=1;
	ts.timer1=1000;
	ts2.timer1=1000;





	/* Create the two tasks.  The Tx task is given a lower priority than the
	Rx task, so the Rx task will leave the Blocked state and pre-empt the Tx
	task as soon as the Tx task places an item in the queue. */
	xTaskCreate( 	Task1, 					/* The function that implements the task. */
					( const char * ) "Tx", 		/* Text name for the task, provided to assist debugging only. */
					configMINIMAL_STACK_SIZE, 	/* The stack allocated to the task. */
					(void*)&ts, 						/* The task parameter is not used, so set to NULL. */
					tskIDLE_PRIORITY,			/* The task runs at the idle priority. */
					NULL );

	xTaskCreate( 	Task1, 					/* The function that implements the task. */
						( const char * ) "Tx2", 		/* Text name for the task, provided to assist debugging only. */
						configMINIMAL_STACK_SIZE, 	/* The stack allocated to the task. */
						(void*)&ts2, 						/* The task parameter is not used, so set to NULL. */
						tskIDLE_PRIORITY,			/* The task runs at the idle priority. */
						NULL );

	xTaskCreate( task2,
				 ( const char * ) "GB",
				 configMINIMAL_STACK_SIZE,
				 NULL,
				 tskIDLE_PRIORITY + 1,
				 NULL );

	/* Create the queue used by the tasks.  The Rx task has a higher priority
	than the Tx task, so will preempt the Tx task and remove values from the
	queue as soon as the Tx task writes to the queue - therefore the queue can
	never have more than one item in it. */
	xQueue = xQueueCreate( 	10,						/* There is only one space in the queue. */
							sizeof( message ) );	/* Each space in the queue is large enough to hold a uint32_t. */

	/* Check the queue was created. */
	configASSERT( xQueue );
	/* Create a timer with a timer expiry of 10 seconds. The timer would expire
	 after 10 seconds and the timer call back would get called. In the timer call back
	 checks are done to ensure that the tasks have been running properly till then.
	 The tasks are deleted in the timer call back and a message is printed to convey that
	 the example has run successfully.
	 The timer expiry is set to 10 seconds and the timer set to not auto reload. */

	/* Check the timer was created. */


	/* start the timer with a block time of 0 ticks. This means as soon
	   as the schedule starts the timer will start running and will expire after
	   10 seconds */

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}


/*-----------------------------------------------------------*/
static void Task1( void *pvParameters )
{
	task * param;
	param = (task *) pvParameters;



	for( ;; )
	{
		message * Ptr = (message *)pvPortMalloc(sizeof(message));
		strcpy(Ptr->mess,"hola ya funciono jeje");
		if (param->flag==1){
			Ptr->task=1;
			}
		else{
			Ptr->task=0;
		}


		/* Delay for 1 second. */


		/* Send the next value on the queue.  The queue should always be
		empty at this point so a block time of 0 is used. */
		xQueueSend(xQueue,			/* The queue being written to. */
					Ptr, /* The address of the data being sent. */
					param->timer1/portTICK_PERIOD_MS );			/* The block time. */

		free(Ptr);
		vTaskDelay( param->timer1/portTICK_PERIOD_MS );
	}
}

/*-----------------------------------------------------------*/
static void task2( void *pvParameters )
{
    message * RecP=pvPortMalloc(sizeof(message));
    //task * param;
	//param = (task *) pvParameters;
	for( ;; )
	{
		/* Block to wait for data arriving on the queue. */
		xQueueReceive( 	xQueue,				/* The queue being read. */
						RecP,	/* Data is read into this address. */
						portMAX_DELAY );	/* Wait without a timeout for data. */

		/* Print the received data. */
		xil_printf( "message %s received from task: %i \r\n", RecP->mess, RecP->task );

	}
}

/*-----------------------------------------------------------*/
