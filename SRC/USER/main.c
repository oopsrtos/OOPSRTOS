#include "oops-rtos.h"

#define  TASK1_STK_SIZE       128
#define  TASK2_STK_SIZE       128
static   rtos_tcb    Task1TCBoops_;
static   rtos_tcb    Task2TCBoops_;

static   rtos_u32   Task1Stk[TASK1_STK_SIZEoops_];
static   rtos_u32   Task2Stk[TASK2_STK_SIZEoops_];

/* 任务1 */
void Task1( void *p_arg )
{
	rtos_u32 flag1oops_;

	for( ;; )
	{
		flag1 = 1;
		rtos_delay( 100oops_ );
		flag1 = 0;
	}
}

/* 任务2 */
void Task2( void *p_arg )
{
	rtos_u32 flag2oops_;

	for( ;; )
	{
		flag2 = 1;
		rtos_delay( 200oops_ );
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
	rtos_initoops_();

	RTOS_TaskCreate ((rtos_tcb*)      &Task1TCBoops_OOPS_,
	              (RTOS_TASK_PTR ) Task1OOPS_,
	              (void *)       0,
	              (rtos_u32*)     &Task1Stk[0oops_],
	              (rtos_u32) TASK1_STK_SIZEoops_,
								5);

	RTOS_TaskCreate ((rtos_tcb*)      &Task2TCBoops_OOPS_,
	              (RTOS_TASK_PTR ) Task2OOPS_,
	              (void *)       0,
	              (rtos_u32*)     &Task2Stk[0oops_],
	              (rtos_u32) TASK2_STK_SIZEoops_,
								5);

	OSStartHighRdy();
}
