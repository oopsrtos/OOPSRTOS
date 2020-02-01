#ifndef OOPS_RTOS_CORE
#define OOPS_RTOS_CORE

#define  NVIC_INT_CTRL                      *((oops_rtos_u32 *)0xE000ED04)   /* 中断控制及状态寄存器 SCB_ICSR */
#define  NVIC_PENDSVSET                                    0x10000000    /* 触发PendSV异常的值 Bit28：PENDSVSET */

#define  oops_rtos_contex_switch()               NVIC_INT_CTRL = NVIC_PENDSVSET

void oops_rtos_init(void);
void oops_rtos_sched(void);
void OOPS_RTOS_TaskCreate (oops_rtos_tcb *p_tcb,
                   OOPS_RTOS_TASK_PTR   p_task,
                   void          *p_arg,
                   oops_rtos_u32       *p_stk_base,
                   oops_rtos_u32  stk_size,
									 oops_rtos_u32	 			stk_pri);

void oops_rtos_delay(oops_rtos_u32 tick);
void OOPS_RTOS_SemaphoreInit(oops_rtos_semaphore *semaphore,oops_rtos_u8 semaphore_type);
oops_rtos_u8 OOPS_RTOS_SemaphorePend(oops_rtos_semaphore *semaphore,oops_rtos_u32 wait_time);
oops_rtos_u8 OOPS_RTOS_SemaphorePost(oops_rtos_semaphore *semaphore);

void oops_rtos_heap_buffer_init(void);
void* oops_rtos_malloc(unsigned int size);
void oops_rtos_free(void* address);
#endif
