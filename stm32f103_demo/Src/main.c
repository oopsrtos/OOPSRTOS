/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "oops_rtos.h"
#include "main.h"
#include "usart.h"
#include "gpio.h"

void SystemClock_Config(void);
#define  TASK1_STK_SIZE       128
#define  TASK2_STK_SIZE       128
static   oops_rtos_tcb    Task1TCB;
static   oops_rtos_tcb    Task2TCB;

static   oops_rtos_u32   Task1Stk[TASK1_STK_SIZE];
static   oops_rtos_u32   Task2Stk[TASK2_STK_SIZE];


void Task1( void *p_arg )
{
	uint8_t oops[] = "hello oops_rtos!\r\n";

	for( ;; ){
		dma_usart_tx(oops,sizeof(oops));
		oops_rtos_delay( 500 );
	}
}


void Task2( void *p_arg )
{
    uint8_t hello_wlorld[] = "hello world!\r\n";
    MX_USART1_UART_Init();

	for( ;; ){
		dma_usart_tx(hello_wlorld,sizeof(hello_wlorld));
		oops_rtos_delay( 1000 );
	}
}


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  LL_GPIO_AF_Remap_SWJ_NOJTAG();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  oops_rtos_init();

  OOPS_RTOS_TaskCreate ((oops_rtos_tcb*)      &Task1TCB,\
                        (OOPS_RTOS_TASK_PTR ) Task1,\
                        (void *)       0,\
                        (oops_rtos_u32*)     Task1Stk,\
                        (oops_rtos_u32) TASK1_STK_SIZE,\
                        5);

  OOPS_RTOS_TaskCreate ((oops_rtos_tcb*)      &Task2TCB,\
                        (OOPS_RTOS_TASK_PTR ) Task2,\
                        (void *)       0,\
                        (oops_rtos_u32*)     Task2Stk,\
                        (oops_rtos_u32) TASK2_STK_SIZE,\
                        5);

  OSStartHighRdy();

  while (1){
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

   if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
    Error_Handler();
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(72000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(72000000);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
