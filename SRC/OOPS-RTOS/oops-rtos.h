#ifndef   OOPS_RTOS_H
#define   OOPS_RTOS_H
/*************************************************************************************************************************
                                                   global value define
*************************************************************************************************************************/
#ifdef     OOPS_RTOS_GLOBALS
#define    OOPS_RTOS_EXT
#else
#define    OOPS_RTOS_EXT  extern
#endif

#define  oops_rtos_u32 unsigned int
#define  oops_rtos_u8 unsigned char

typedef  void                        (*OOPS_RTOS_TASK_PTR)(void *p_arg); /*函数指针定义*/
#define  OOPS_RTOS_PRIO_MAX          32u					/* 支持最大的优先级 */

#define	NULL	0

typedef struct	oops_rtos_semaphore_def oops_rtos_semaphore;//信号量

/*************************************************************************************************************************
                                                   task define
*************************************************************************************************************************/
typedef struct
{
	oops_rtos_u32       *StkPtr;
	oops_rtos_u32       StkSize;
	oops_rtos_u32		Priority;		/*优先级*/
	oops_rtos_u32		Virtual_Pri;/*虚拟优先级*/
	oops_rtos_u32		DelayTicks; /*任务延时时间,单位为tick*/
	oops_rtos_semaphore *Wait_Semaphore;
}oops_rtos_tcb;
/*************************************************************************************************************************
                                                   task list define
*************************************************************************************************************************/
struct	oops_rtos_task_list_def
{
	struct oops_rtos_task_list_def		*PrevNode;//下一个任务节点
	struct oops_rtos_task_list_def		*NextNode;//下一个任务节点
	oops_rtos_tcb        				*TaskPtr; //任务指针
};

typedef struct	oops_rtos_task_list_def oops_rtos_task_list;
/*************************************************************************************************************************
                                                   semaphore
*************************************************************************************************************************/
struct	oops_rtos_semaphore_def
{
	oops_rtos_tcb        						*OwnerTaskPtr; //任务指针
	oops_rtos_u32        						*Value; 			 //信号量
	oops_rtos_u8								Type;					 //信号量类型 0：二值信号量 1：消息邮箱
};

#define MUTEX 	                        0
#define MAIL_BOX                        1
#define SEMAPHORE_GET_FILE 		        1
#define SEMAPHORE_GET_SUCCESS           0
#define SEMAPHORE_RELEASE_FILE 		    1
#define SEMAPHORE_RELEASE_SUCCESS       0
#define INFINITE_WAIT					0xffffffff
/*************************************************************************************************************************
                                                   全局变量
*************************************************************************************************************************/
M_RTOS_EXT    oops_rtos_tcb         *OOPS_RTOS_CurPtr;		//汇编函数调用的全局变量
M_RTOS_EXT    oops_rtos_tcb         *OOPS_RTOS_RdyPtr;		//汇编函数调用的全局变量
M_RTOS_EXT    oops_rtos_task_list   *OOPS_RTOS_RdyTaskList;	//就绪任务列表
M_RTOS_EXT    oops_rtos_task_list   *OOPS_RTOS_PendTaskList;//阻塞任务列表
/*************************************************************************************************************************
                                                  函数声明
*************************************************************************************************************************/

/*************************************************************************************************************************
                                                   head file include
*************************************************************************************************************************/
#include "oops_rtos_core.h"
#include "ARMCM3.h"
#include <stdlib.h>
/*************************************************************************************************************************
                                                   汇编函数的外部声明
*************************************************************************************************************************/
void oops_rtos_set_interrupt_stop( void );
void oops_rtos_set_interrupt_start( void );
void OSStartHighRdy( void );

#endif
