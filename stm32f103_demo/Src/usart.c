/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "string.h"

uint8_t usart_txbuffer[128] = {0};
uint8_t usart_rxbuffer[128] = {0};
uint32_t receive_length = 0;

void USART1_DMA_INIT(void)
{
  LL_DMA_InitTypeDef DMA_InitStructure = {0};

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  
  LL_DMA_DeInit(DMA1,LL_DMA_CHANNEL_4);//reset USART1_TX DMA channel4
  LL_DMA_DeInit(DMA1,LL_DMA_CHANNEL_5);//reset USART1_RX DMA channel5

    /* DMA interrupt Init USART1_RX DMA receive completed*/
  NVIC_SetPriority(DMA1_Channel5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(DMA1_Channel5_IRQn);

  DMA_InitStructure.PeriphOrM2MSrcAddress = (uint32_t)(&USART1->DR);
  DMA_InitStructure.Mode = LL_DMA_MODE_NORMAL;
  DMA_InitStructure.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
  DMA_InitStructure.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
  DMA_InitStructure.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
  DMA_InitStructure.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
  DMA_InitStructure.NbData = 0;//Specifies the number of data to transfer.
  DMA_InitStructure.Priority = LL_DMA_PRIORITY_HIGH;

  //USART1 DMA send
  DMA_InitStructure.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
  DMA_InitStructure.MemoryOrM2MDstAddress = (uint32_t)&usart_txbuffer;
  LL_DMA_Init(DMA1,LL_DMA_CHANNEL_4, &DMA_InitStructure);
  LL_DMA_ClearFlag_TC4(DMA1);//must clean tc flag

  //USART1 DMA receive
  DMA_InitStructure.NbData = sizeof(usart_rxbuffer);//Specifies the number of data to receive.
  DMA_InitStructure.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
  DMA_InitStructure.MemoryOrM2MDstAddress = (uint32_t)usart_rxbuffer;
  LL_DMA_Init(DMA1,LL_DMA_CHANNEL_5, &DMA_InitStructure);
  LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_5);
  LL_DMA_ClearFlag_TC5(DMA1);//must clean tc flag
}

/**
  * @brief : uart dma initialize
  * @input : none
  * @return : none
  * @date : 2019.6.17
  * @author : dinghua.ma@dorabot.com
  **/
void MX_USART1_UART_Init(void)
{
  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
  /**USART1 GPIO Configuration */
  /* PA9     ------> USART1_TX */
  /* PA10    ------> USART1_RX */
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_MODE_OUTPUT_50MHz;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  GPIO_InitStruct.Speed = LL_GPIO_MODE_OUTPUT_50MHz;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USART1 interrupt Init */
  NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(USART1_IRQn);

  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART1, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART1);
  LL_USART_EnableIT_IDLE(USART1);
  LL_USART_EnableIT_TC(USART1);
  LL_USART_EnableDMAReq_RX(USART1);
  LL_USART_EnableDMAReq_TX(USART1);
  LL_USART_Enable(USART1);

  USART1_DMA_INIT();
}

/**
  * @brief : start dma sned
  * @input : *buff buffer for send size length for buffer
  * @return : none
  * @date : 2019.6.17
  * @author : dinghua.ma@dorabot.com
  **/
void dma_usart_tx_restart(uint8_t *res, uint32_t size)
{
  static uint8_t buffer[1000] = {0};//Cache consistency
  if (size < sizeof(buffer)){
    memcpy(buffer,res,size);
    LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_4);
    LL_DMA_ClearFlag_TC7(DMA1);//must clean tc flag
    LL_DMA_SetMemoryAddress(DMA1,LL_DMA_CHANNEL_4,(uint32_t)buffer);
    LL_DMA_SetDataLength(DMA1,LL_DMA_CHANNEL_4,size);
    LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_4);
  }
}

/**
 * @brief : usart1 dma send function
 * @input : *buff buffer for send size length for buffer
 * @return : none
 * @date : 2019.6.17
 * @author : dinghua.ma@dorabot.com
 */
uint8_t dma_usart_tx(uint8_t *buff, uint32_t size)
{
  uint8_t ret = 0;
  if (!LL_DMA_GetDataLength(DMA1,LL_DMA_CHANNEL_4)){//check if transfer idle
    dma_usart_tx_restart(buff, size);
    ret = 0;
  }else {
    ret = 1;
  }

  return ret;
}

/**
 * @brief : usart1 receive completed interruption
 * @input :  none
 * @return : none
 * @date : 2019.6.17
 * @author : dinghua.ma@dorabot.com
 */
void DMA1_Channel5_IRQHandler(void)
{
  if (LL_DMA_IsActiveFlag_TC5(DMA1)){
    LL_DMA_ClearFlag_TC5(DMA1);
  }
}

/**
 * @brief : usart3 receive completed interruption
 * @input :  none
 * @return : none
 * @date : 2019.6.17
 * @author : dinghua.ma@dorabot.com
 */
void USART1_IRQHandler(void)
{
  uint16_t length = 0;
  if (LL_USART_IsActiveFlag_IDLE (USART1)){
    LL_USART_ClearFlag_IDLE (USART1);
    LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_5);
    LL_DMA_ClearFlag_TC5(DMA1);//must clean tc flag
    length = sizeof(usart_rxbuffer) - LL_DMA_GetDataLength(DMA1,LL_DMA_CHANNEL_5);
    //for test code
    receive_length = length;
    memcpy(usart_txbuffer,usart_rxbuffer,length);
    //end of test
    LL_DMA_SetDataLength(DMA1,LL_DMA_CHANNEL_5,sizeof(usart_rxbuffer));
    LL_DMA_SetMemoryAddress(DMA1,LL_DMA_CHANNEL_5,(uint32_t)usart_rxbuffer);
    LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_5);
  } else if (LL_USART_IsActiveFlag_TC (USART1)){
    LL_USART_ClearFlag_TC (USART1);
    LL_DMA_ClearFlag_TC4(DMA1);//must clean tc flag
  }
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
