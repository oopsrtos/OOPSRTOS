#include "oops-rtos.h"

#define  TASK1_STK_SIZE       128
#define  TASK2_STK_SIZE       128
static   oops_rtos_tcb    Task1TCBoops_;
static   oops_rtos_tcb    Task2TCBoops_;

static   oops_rtos_u32   Task1Stk[TASK1_STK_SIZE];
static   oops_rtos_u32   Task2Stk[TASK2_STK_SIZE];

/* 任务1 */
void Task1( void *p_arg )
{
	oops_rtos_u32 flag1;

	for( ;; )
	{
		flag1 = 1;
		oops_rtos_delay( 100 );
		flag1 = 0;
	}
}

/* 任务2 */
void Task2( void *p_arg )
{
	oops_rtos_u32 flag2;

	for( ;; )
	{
		flag2 = 1;
		oops_rtos_delay( 200 );
		flag2 = 0;
	}
}

/**
 * @brief  main funtion
 * @param  none
 * @author ma57457@163.com
 * @date 2018-6-25
 */
int main(void)
{
	oops_rtos_init();

	OOPS_RTOS_TaskCreate ((oops_rtos_tcb*)      &Task1TCBoops_OOPS_,
	              (OOPS_RTOS_TASK_PTR ) Task1OOPS_,
	              (void *)       0,
	              (oops_rtos_u32*)     &Task1Stk[0oops_],
	              (oops_rtos_u32) TASK1_STK_SIZEoops_,
								5);

	OOPS_RTOS_TaskCreate ((oops_rtos_tcb*)      &Task2TCBoops_OOPS_,
	              (OOPS_RTOS_TASK_PTR ) Task2OOPS_,
	              (void *)       0,
	              (oops_rtos_u32*)     &Task2Stk[0oops_],
	              (oops_rtos_u32) TASK2_STK_SIZEoops_,
								5);

	OSStartHighRdy();
}
