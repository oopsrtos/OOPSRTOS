/* ########################################################################################################
    全局变量&函数
 ******************************************************************************************************** */
.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb
.text

/* 外部文件引人的参考 */
.extern  OOPS_RTOS_CurPtr
.extern  OOPS_RTOS_RdyPtr
.extern  OOPS_SWITCH_LOCK
/* 本文件定义的函数 */
/********************************************************************************************************
*                                               常量
*********************************************************************************************************
*--------------------------------------------------------------------------------------------------------
*有关内核外设寄存器定义可参考官方文档：STM32F10xxx Cortex-M3 programming manual
*系统控制块外设SCB地址范围：0xE000ED00-0xE000ED3F
*--------------------------------------------------------------------------------------------------------*/
/* 中断控制及状态寄存器 SCB_ICSR。*/
.equ NVIC_INT_CTRL, 0xE000ED04
/* 系统优先级寄存器 SCB_SHPR3：bit16~23 */
.equ NVIC_SYSPRI14, 0xE000ED22
/* PendSV 优先级的值(最低)。*/
.equ NVIC_PENDSV_PRI, 0xFF
/* 触发PendSV异常的值 Bit28：PENDSVSET。*/
.equ NVIC_PENDSVSET, 0x10000000

/* ******************************************************************************************************
*                                          代码产生指令
*********************************************************************************************************
*	PRESERVE8
*	THUMB

*	AREA CODE, CODE, READONLY

*********************************************************************************************************
*                                          暂停全局中断
********************************************************************************************************* */
    .global oops_rtos_set_interrupt_stop
    .type oops_rtos_set_interrupt_stop, %function
oops_rtos_set_interrupt_stop:
    CPSID   I
    BX      LR
/*********************************************************************************************************
*                                          使能全局中断
    *********************************************************************************************************/
    .global oops_rtos_set_interrupt_start
    .type oops_rtos_set_interrupt_start, %function
oops_rtos_set_interrupt_start:
    CPSIE   I
    BX      LR
/*********************************************************************************************************
*                                          开始第一次上下文切换
* 1、配置PendSV异常的优先级为最低
* 2、在开始第一次上下文切换之前，设置psp=0
* 3、触发PendSV异常，开始上下文切换
    *********************************************************************************************************/
    .global OSStartHighRdy
    .type OSStartHighRdy, %function
OSStartHighRdy:
/* 设置  PendSV 异常优先级为最低 */
    LDR		R0, = NVIC_SYSPRI14
    LDR     R1, = NVIC_PENDSV_PRI
    STRB    R1, [R0]
/* 设置psp的值为0，开始第一次上下文切换 */
    MOVS    R0, #0
    MSR     PSP, R0
/* 触发PendSV异常  */
    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
/* 开中断 */
    CPSIE   I

OSStartHang:
/* 程序应永远不会运行到这里 */
    B       OSStartHang

/*********************************************************************************************************
*                                          PendSVHandler异常
*********************************************************************************************************/
/* 任务的保存，即把CPU寄存器的值存储到任务的堆栈中 */
    .global PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
/* 关中断，NMI和HardFault除外，防止上下文切换被中断 */
    CPSID   I
    /* 载入外部变量OOPS_SWITCH_LOCK */
    LDR     R0, = OOPS_SWITCH_LOCK
    LDR     R1, = 0
    /* 存储 OOPS_RTOS_RdyPtr 到 OOPS_SWITCH_LOCK */
    STR     R1, [R0]
/* 将psp的值加载到R0 */
    MRS     R0, PSP
/* 判断R0，如果值为0则跳转到OS_CPU_PendSVHandler_nosave */
    CBZ     R0, OS_CPU_PendSVHandler_nosave
/* 进行第一次任务切换的时候，R0肯定为0 */

/* 在进入PendSV异常的时候，当前CPU的xPSR，PC（任务入口地址），R14，R12，R3，R2，R1，R0会自动存储到当前任务栈，同时递减PSP的值 */
/* 手动存储CPU寄存器R4-R11的值到当前任务的栈 */
    STMDB   R0!, {R4-R11}
/* 加载 OOPS_RTOS_CurPtr 指针的地址到R1 */
    LDR     R1, = OOPS_RTOS_CurPtr
/* 加载 OOPS_RTOS_CurPtr 指针到R1 */
    LDR     R1, [R1]
/* 存储R0的值到OOPS_RTOS_CurPtr指向的任务栈 */
    STR     R0, [R1]

/* 任务的切换，即把下一个要运行的任务的栈内容加载到CPU寄存器中 */
OS_CPU_PendSVHandler_nosave:
    /* OOPS_RTOS_CurPtr = OOPS_RTOS_RdyPtr */
    /* 加载 OOPS_RTOS_CurPtr 指针的地址到R0 */
    LDR     R0, = OOPS_RTOS_CurPtr
    /* 加载 OOPS_RTOS_RdyPtr 指针的地址到R1 */
    LDR     R1, = OOPS_RTOS_RdyPtr
    /* 加载 OOPS_RTOS_RdyPtr 指针到R2 */
    LDR     R2, [R1]
    /* 存储 OOPS_RTOS_RdyPtr 到 OOPS_RTOS_CurPtr */
    STR     R2, [R0]
    /* 加载 OOPS_RTOS_RdyPtr 到 R0 */
    LDR     R0, [R2]
    /* 加载需要手动保存的信息到CPU寄存器R4-R11 */
    LDMIA   R0!, {R4-R11}
    /* 更新PSP的值，这个时候PSP指向下一个要执行的任务的栈的栈底（这个栈底已经加上刚刚手动加载到CPU寄存器R4-R11的偏移）*/
    MSR     PSP, R0
    /* 确保异常返回使用的栈指针是PSP，即LR寄存器的位2要为1*/
    ORR     LR, LR, #0x04
    /* 开中断 */
    CPSIE   I
    /* 异常返回，这个时候任务栈中的剩下内容将会自动加载到xPSR，PC（任务入口地址），R14，R12，R3，R2，R1，R0（任务的形参）*/
    BX      LR
    /* 同时PSP的值也将更新，即指向任务栈的栈顶。在STM32中，栈是由高地址向低地址生长的。*/
    /* 为了汇编指令对齐，不然会有警告 */
    NOP
    /* 汇编文件结束 */
.end
