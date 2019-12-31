#define OOPS_RTOS_GLOBALS
#include "OOPS-RTOS.h"
#undef OOPS_RTOS_GLOBALS

/***************************************************************************************************
						oops_rtos idle task
***************************************************************************************************/
#define IDLE_STACK_SIZE	128
#define IDLE_TASK_PRIORITY	0xffffffff

static   oops_rtos_u32   IdleTaskStk[IDLE_STACK_SIZE];	/*空闲任务栈*/
static	 oops_rtos_tcb		IdleTaskTcb;									/*空闲任务块*/
/**
 * @brief  idle task
 * @param  none
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void oops_rtos_idle_task( void *p_arg )
{
	p_arg = p_arg;
	while(1)
		__ASM("NOP");
}
/**
 * @brief  idle task
 * @param  none
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void oops_rtos_idle_init( void *p_arg )
{
	OOPS_RTOS_TaskCreate ((oops_rtos_tcb*)      &IdleTaskTcb,
	              (OOPS_RTOS_TASK_PTR ) 			oops_rtos_idle_task,
	              (void *)       					0,
	              (oops_rtos_u32*)     			&IdleTaskStk,
	              (oops_rtos_u32) 						IDLE_STACK_SIZE,
								(oops_rtos_u32)						IDLE_TASK_PRIORITY);
}
/***************************************************************************************************
    					oops_rtos ready task list
***************************************************************************************************/
/**
 * @brief  任务列表添加节点
 * @param  none
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void OOPS_RTOS_InsertNodeTaskList(oops_rtos_tcb *p_tcb,oops_rtos_task_list **task_list)
{
	oops_rtos_task_list *task_prt_new = (oops_rtos_task_list*)oops_rtos_malloc(sizeof(oops_rtos_task_list));
	/*这里需要添加内存获取失败的处理*/

	if(NULL == (*task_list)){
		(*task_list) = task_prt_new;
		(*task_list)->TaskPtr = p_tcb;
		(*task_list)->NextNode = (*task_list);
	}else{
		task_prt_new->TaskPtr = p_tcb;
		task_prt_new->NextNode = (*task_list)->NextNode;
		(*task_list)->NextNode = task_prt_new;
	}

}
/**
 * @brief  任务列表节点删除
 * @param  none
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void OOPS_RTOS_DeleteNodeTaskList(oops_rtos_tcb *p_tcb,oops_rtos_task_list **task_list)
{
	oops_rtos_task_list *del_node = NULL;

	if(NULL == task_list){
		return;
	}else{
		del_node = (*task_list)->NextNode;
		for (;del_node != (*task_list)->NextNode;del_node = (*task_list)->NextNode){
			if (((del_node->TaskPtr) == p_tcb) && (del_node->TaskPtr->Priority != IDLE_TASK_PRIORITY)){
				(*task_list)->NextNode = del_node->NextNode;
				oops_rtos_free(del_node);
				break;
			}
		}
	}
}
/**
 * @brief  任务列表节点查找
 * @param  none
 * @author ma57457@163.com
 * @date 2019-11-25
 */
m_rtos_u8 OOPS_RTOS_SearchNodeTaskList(oops_rtos_tcb *p_tcb,oops_rtos_task_list **task_list)
{
	oops_rtos_u8 ret = NULL;
	oops_rtos_task_list *bak_list = *task_list;

	if(p_tcb == (*task_list)->TaskPtr){
		ret = 1;
		return ret;
	}

	(*task_list) = (*task_list)->NextNode;
	for (; bak_list != (*task_list); (*task_list) = (*task_list)->NextNode){
		if(p_tcb == (*task_list)->TaskPtr){
			ret = 1;
			break;
		}
	}

	return ret;
}
/**
 * @brief  任务列表节点查找,查找上一个节点
 * @param  none
 * @author ma57457@163.com
 * @date 2019-11-25
 */
oops_rtos_task_list* OOPS_RTOS_SearchNodeTaskList_Prev(oops_rtos_tcb *p_tcb,oops_rtos_task_list **task_list)
{
	oops_rtos_task_list *ret_addr = (oops_rtos_task_list *)NULL;
	oops_rtos_task_list *bak_list = *task_list;

	if(p_tcb == (*task_list)->NextNode->TaskPtr){
		ret_addr = (*task_list);
		return ret_addr;
	}

	(*task_list) = (*task_list)->NextNode;
	for (; bak_list != (*task_list); (*task_list) = (*task_list)->NextNode){
		if(p_tcb == (*task_list)->NextNode->TaskPtr){
			ret_addr = (*task_list);
			return ret_addr;
		}
	}

	return ret_addr;
}
/***************************************************************************************************
								oops_rtos  任务挂起与就绪操作
***************************************************************************************************/
/**
 * @brief  挂起任务
 * @param  *target_tcb 需要挂起的目标任务
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void oops_rtos_task_move(oops_rtos_tcb *target_tcb,oops_rtos_task_list **src_list,oops_rtos_task_list **tar_list)
{
	oops_rtos_task_list *rdy_node = (oops_rtos_task_list *)NULL;
	oops_rtos_task_list *prev_node = OOPS_RTOS_SearchNodeTaskList_Prev(target_tcb,src_list);

	if(prev_node != NULL){
		rdy_node = prev_node->NextNode;
		prev_node->NextNode = prev_node->NextNode->NextNode;

		if(NULL == (*tar_list)){
			(*tar_list) = rdy_node;
			(*tar_list)->NextNode = (*tar_list);
		}else{
			rdy_node->NextNode = (*tar_list)->NextNode->NextNode;
			(*tar_list)->NextNode = rdy_node;
		}
	}


}
/***************************************************************************************************
								oops_rtos  cotrex switch
***************************************************************************************************/
/**
 * @brief  获取当前就绪列表中优先级最大的任务块，有一个缺点是需要就绪列表全部遍历
 * @param  none
 * @author ma57457@163.com
 * @date 2019-11-25
 */
oops_rtos_tcb*  oopsrtoe_get_highest_rdy_task(void)
{
	oops_rtos_tcb *ret_tcb	= OOPS_RTOS_CurPtr;
	oops_rtos_task_list *bak_node = OOPS_RTOS_RdyTaskList;

	if( NULL == OOPS_RTOS_SearchNodeTaskList(OOPS_RTOS_CurPtr,&OOPS_RTOS_RdyTaskList))
		ret_tcb = OOPS_RTOS_RdyTaskList->TaskPtr;

	if(OOPS_RTOS_RdyTaskList == OOPS_RTOS_RdyTaskList->NextNode)/*没有任务的情况*/
		return ret_tcb = OOPS_RTOS_RdyTaskList->TaskPtr;

	OOPS_RTOS_RdyTaskList = OOPS_RTOS_RdyTaskList->NextNode;/*如果只有一个任务的情况*/
	if ((OOPS_RTOS_RdyTaskList->TaskPtr->Priority <= ret_tcb->Priority) || (OOPS_RTOS_RdyTaskList->TaskPtr->Virtual_Pri <= ret_tcb->Virtual_Pri))
		ret_tcb =  OOPS_RTOS_RdyTaskList->TaskPtr;

	for (; OOPS_RTOS_RdyTaskList != bak_node; OOPS_RTOS_RdyTaskList = OOPS_RTOS_RdyTaskList->NextNode){
			if ((OOPS_RTOS_RdyTaskList->TaskPtr->Priority <= ret_tcb->Priority) || (OOPS_RTOS_RdyTaskList->TaskPtr->Virtual_Pri <= ret_tcb->Virtual_Pri))
				ret_tcb =  OOPS_RTOS_RdyTaskList->TaskPtr;
	}

	return ret_tcb;
}
/**
 * @brief  cotrex_switch
 * @param  none
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void oops_rtos_sched(void)
{
	OOPS_RTOS_RdyPtr = oopsrtoe_get_highest_rdy_task();

	if(OOPS_RTOS_RdyPtr == OOPS_RTOS_CurPtr)
		return;

	oops_rtos_contex_switch();
}
/***************************************************************************************************
																			oops_rtos  cotrex switch
***************************************************************************************************/

/***************************************************************************************************
																			tick
***************************************************************************************************/
/**
 * @brief  tick initialize
 * @param  tick_ms uint is ms
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void oops_rtos_tick_init(oops_rtos_u32 tick_ms)
{
	SysTick->LOAD = tick_ms*SystemCoreClock/1000-1;
	NVIC_SetPriority(SysTick_IRQn,(1<<__NVIC_PRIO_BITS)-1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk|\
									SysTick_CTRL_TICKINT_Msk|\
									SysTick_CTRL_ENABLE_Msk;
}
/**
 * @brief  tick delay
 * @param  tick_ms uint is ms
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void oops_rtos_delay(oops_rtos_u32 tick)
{
	OOPS_RTOS_CurPtr->DelayTicks = tick;

	oops_rtos_task_move(M_RTOS_CurPtr,&M_RTOS_RdyTaskList,&M_RTOS_PendTaskList);

	oops_rtos_sched();
}
/**
 * @brief  oops_rtos_pend_list_deal
 * @param  tick_ms uint is ms
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void oops_rtos_pend_list_deal()
{
	oops_rtos_task_list *bak_node = OOPS_RTOS_PendTaskList;
	oops_rtos_task_list *del_dode = NULL;

	if(M_RTOS_PendTaskList != NULL){/*假如有任务挂起*/
		M_RTOS_PendTaskList = OOPS_RTOS_PendTaskList->NextNode;
		/*假定只有一个挂起任务的情况*/
			del_dode = OOPS_RTOS_PendTaskList;

			if(del_dode->TaskPtr->Wait_Semaphore != NULL){								 /*等待信号量的处理*/
				if (NULL == del_dode->TaskPtr->Wait_Semaphore->OwnerTaskPtr){/*获取信号量成功*/
					del_dode->TaskPtr->Wait_Semaphore->OwnerTaskPtr = del_dode->TaskPtr;
					del_dode->TaskPtr->DelayTicks = 0;
				}else  if(del_dode->TaskPtr->DelayTicks){
					if (del_dode->TaskPtr->DelayTicks != INFINITE_WAIT)				/*无限等待不处理延时时间*/
						del_dode->TaskPtr->DelayTicks--;
				}
			}else		if (del_dode->TaskPtr->DelayTicks != NULL){
				del_dode->TaskPtr->DelayTicks--;
			}

			if (0 == del_dode->TaskPtr->DelayTicks){
				if(bak_node == del_dode)
					bak_node = bak_node->NextNode;
				oops_rtos_task_move(del_dode->TaskPtr,&M_RTOS_PendTaskList,&M_RTOS_RdyTaskList);
			}
		/*假定多个挂起任务的情况*/
		bak_node = OOPS_RTOS_PendTaskList;
		for (; M_RTOS_PendTaskList != bak_node; M_RTOS_PendTaskList = OOPS_RTOS_PendTaskList->NextNode){
			del_dode = OOPS_RTOS_PendTaskList;

			if(del_dode->TaskPtr->Wait_Semaphore != NULL){								 /*等待信号量的处理*/
				if (NULL == del_dode->TaskPtr->Wait_Semaphore->OwnerTaskPtr){/*获取信号量成功*/
					del_dode->TaskPtr->Wait_Semaphore->OwnerTaskPtr = del_dode->TaskPtr;
					del_dode->TaskPtr->DelayTicks = 0;
				}else  if(del_dode->TaskPtr->DelayTicks){
					if (del_dode->TaskPtr->DelayTicks != INFINITE_WAIT)				/*无限等待不处理延时时间*/
						del_dode->TaskPtr->DelayTicks--;
				}
			}else		if (del_dode->TaskPtr->DelayTicks != NULL){
				del_dode->TaskPtr->DelayTicks--;
			}

			if (0 == del_dode->TaskPtr->DelayTicks){
				if(bak_node == del_dode)
					bak_node = bak_node->NextNode;
				oops_rtos_task_move(del_dode->TaskPtr,&M_RTOS_PendTaskList,&M_RTOS_RdyTaskList);
			}
		}
	}
}
/***************************************************************************************************
																			系统初始化
	* 1、栈初始化，
	* 2、空闲任务初始化
***************************************************************************************************/
/**
 * @brief  oops_rtos_initialize
 * @param  none
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void oops_rtos_init(void)
{
	oops_rtos_set_interrupt_stop();

    oops_rtos_heap_buffer_init();

	oops_rtos_idle_init(NULL);/*空闲任务初始化*/

	M_RTOS_CurPtr = (oops_rtos_tcb *)0;
	M_RTOS_RdyPtr = &IdleTaskTcb;
	M_RTOS_PendTaskList = (oops_rtos_task_list *)0;

	oops_rtos_tick_init(10);/*10ms slice*/
}
/**
 * @brief  task stack initialize
 * @param  none
 * @author ma57457@163.com
 * @date 2019-11-25
 */
oops_rtos_u32 *OOPS_RTOS_TaskStackInit (M_RTOS_TASK_PTR  p_task,
                        void         *p_arg,
                        oops_rtos_u32      *p_stk_base,
                        oops_rtos_u32 stk_size)
{
	oops_rtos_u32  *p_stk;

	p_stk = &p_stk_base[stk_size];
													  /* 异常发生时自动保存的寄存器                              */
	*--p_stk = (oops_rtos_u32)0x01000000u;               /* xPSR的bit24必须置1                                     */
	*--p_stk = (oops_rtos_u32)p_task;                    /* 任务的入口地址                                         */
	*--p_stk = (oops_rtos_u32)0u;                        /* R14 (LR)                                               */
	*--p_stk = (oops_rtos_u32)0u;                        /* R12                                                    */
	*--p_stk = (oops_rtos_u32)0u;                        /* R3                                                     */
	*--p_stk = (oops_rtos_u32)0u;                        /* R2                                                     */
	*--p_stk = (oops_rtos_u32)0u;                        /* R1                                                     */
	*--p_stk = (oops_rtos_u32)p_arg;                     /* R0 : 任务形参                                          */
													  /* 异常发生时需手动保存的寄存器                            */
	*--p_stk = (oops_rtos_u32)0u;                        /* R11                                                    */
	*--p_stk = (oops_rtos_u32)0u;                        /* R10                                                    */
	*--p_stk = (oops_rtos_u32)0u;                        /* R9                                                     */
	*--p_stk = (oops_rtos_u32)0u;                        /* R8                                                     */
	*--p_stk = (oops_rtos_u32)0u;                        /* R7                                                     */
	*--p_stk = (oops_rtos_u32)0u;                        /* R6                                                     */
	*--p_stk = (oops_rtos_u32)0u;                        /* R5                                                     */
	*--p_stk = (oops_rtos_u32)0u;                        /* R4                                                     */
	return (p_stk);
}
/**
 * @brief  initialize task
 * @param  *p_tcb task control bulk point
 * @param  p_task task point
 * @param  *p_arg 任务参数
 * @param	 *p_stk_base 任务栈基地址
 * @param  stk_size	栈大小
 * @param  stk_pri 任务优先级
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void OOPS_RTOS_TaskCreate (oops_rtos_tcb 				*p_tcb,
												OOPS_RTOS_TASK_PTR   p_task,
												void          		*p_arg,
												oops_rtos_u32        *p_stk_base,
												oops_rtos_u32  			stk_size,
												oops_rtos_u32	 			stk_pri)
{
	oops_rtos_u32       *p_sp;

	p_sp = OOPS_RTOS_TaskStackInit (p_task,
	p_arg,
	p_stk_base,
	stk_size);
	p_tcb->StkPtr = p_sp;
	p_tcb->StkSize = stk_size;
	p_tcb->Priority = stk_pri;
	p_tcb->Virtual_Pri = IDLE_TASK_PRIORITY;/*虚拟优先级直接拉到最低*/
	p_tcb->Wait_Semaphore = NULL;
	OOPS_RTOS_InsertNodeTaskList(p_tcb,&OOPS_RTOS_RdyTaskList);
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @author ma57457@163.com
  * @date 2019-11-25
  */
void SysTick_Handler(void)
{
	oops_rtos_pend_list_deal();
	oops_rtos_sched();
}
/***************************************************************************************************
    semaphore
	* 1、只考虑了单个互斥信号量的情况，如果任务持有多个锁就完蛋了，系统直接挂掉
	* 2、消息队列没有写
***************************************************************************************************/
/**
  * @brief  semaphore initialize
  * @param  None
  * @author ma57457@163.com
  * @date 2019-11-25
  */
void OOPS_RTOS_SemaphoreInit(oops_rtos_semaphore *semaphore,oops_rtos_u8 semaphore_type)
{
	if(0 == semaphore_type){//二值信号量
		semaphore->OwnerTaskPtr = NULL;
		semaphore->Type = MUTEX;
		semaphore->Value = NULL;
	}else if(1 == semaphore_type){
		/*下一版写*/
	}
}
/**
  * @brief  semaphore pend
	* @param  semaphore:信号量指针 wait_time：超时等待时间
	* @param  return MUTEX_GET_FILE 获取失败 MUTEX_GET_SUCCESS 获取成功
  * @author ma57457@163.com
  * @date 2019-11-25
  */
m_rtos_u8 OOPS_RTOS_SemaphorePend(oops_rtos_semaphore *semaphore,oops_rtos_u32 wait_time)
{
	oops_rtos_u8 ret=SEMAPHORE_GET_FILE;

	if(NULL == semaphore)
		return ret;

	switch(semaphore->Type)
	{
		case MUTEX:{
			oops_rtos_set_interrupt_stop();
			if (NULL == semaphore->OwnerTaskPtr || OOPS_RTOS_CurPtr == semaphore->OwnerTaskPtr){
				semaphore->OwnerTaskPtr = OOPS_RTOS_CurPtr;
				ret = SEMAPHORE_GET_SUCCESS;
			}else{
				OOPS_RTOS_CurPtr->Wait_Semaphore = semaphore;
				if (semaphore->OwnerTaskPtr->Priority < OOPS_RTOS_CurPtr->Priority)
					semaphore->OwnerTaskPtr->Virtual_Pri = OOPS_RTOS_CurPtr->Priority;
				oops_rtos_set_interrupt_start();
				oops_rtos_delay (wait_time);/*挂起任务*/

				if (OOPS_RTOS_CurPtr->Wait_Semaphore == semaphore)
					ret = SEMAPHORE_GET_SUCCESS;
			}
			oops_rtos_set_interrupt_start();
		}
			break;
		case MAIL_BOX:
			/*第二版再写*/
			break;
		default:
			break;
	}
	return ret;
}
/**
  * @brief  semaphore post
  * @param  semaphore:信号量指针
  * @param  return MUTEX_GET_FILE 获取失败 MUTEX_GET_SUCCESS 获取成功
  * @author ma57457@163.com
  * @date 2019-11-25
  */
m_rtos_u8 OOPS_RTOS_SemaphorePost(oops_rtos_semaphore *semaphore)
{
	oops_rtos_u8 ret=SEMAPHORE_GET_FILE;

	if(NULL == semaphore)
		return ret;
	oops_rtos_set_interrupt_stop();
	switch(semaphore->Type)
	{
		case MUTEX:{
			semaphore->OwnerTaskPtr = NULL;
			OOPS_RTOS_CurPtr->Wait_Semaphore = NULL;
			OOPS_RTOS_CurPtr->Virtual_Pri = IDLE_TASK_PRIORITY;
			ret = SEMAPHORE_GET_SUCCESS;
		}
			break;
		case MAIL_BOX:
			/*第二版再写*/
			break;
		default:
			break;
	}
	oops_rtos_set_interrupt_start();
	return ret;
}
/***************************************************************************************************
***************************************************************************************************/
