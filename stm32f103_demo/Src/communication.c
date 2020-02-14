/**
 * @Copyright Dorabot Inc.
 * @date : 2019.2.19
 * @author : dinghua.ma@dorabot.com
 * @brief : communication via uart and can bus
 */
#include "main.h"
#include "stdio.h"
#include "communication.h"
#include "usart.h"
#include "queue.h"

//extern queue_t pb_msg_queue;

/**
 * @brief : message send via usart1
 * @input :  string : send data address
             length : data buffer length
 * @return : none
 * @date : 2019.2.20
 * @author : dinghua.ma@dorabot.com
 */
void message_uart (uint8_t *buffer_address, uint32_t length)
{
  //dma_usart_tx(buffer_address,length);
}

/**
 * @brief : message send via usart1,message format is decimal
 * @input :  value : print decimal number
 * @return : none
 * @date : 2019.2.20
 * @author : dinghua.ma@dorabot.com
 */
void message_value (uint32_t value, uint8_t format)
{
  uint8_t fucking_buffer[256];
  uint8_t length = 0;

  if (0 == format)
  { length = snprintf ((char *)fucking_buffer, sizeof (fucking_buffer), "%u", (unsigned int)value); }
  else if (1 == format)
  { length = snprintf ((char *)fucking_buffer, sizeof (fucking_buffer), "%x", (unsigned int)value); }
  message_uart (fucking_buffer, length);
}


/***********************************************************************************************

***********************************************************************************************/
