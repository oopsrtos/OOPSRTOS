/**
  * @Copyright Dorabot Inc.
  * @date : 2019.4.23
  * @author : dinghua.ma@dorabot.com
  * @brief :
  */
#ifndef __QUEUE_EX_H__
#define __QUEUE_EX_H__

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
//#include "stm32f1xx_hal.h"
#define QUEUE_EX_SIZE (512)

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint16_t byte_cnt;
  uint16_t head;
  uint16_t tail;
  uint8_t buffer[QUEUE_EX_SIZE];
  uint8_t block_size;
} queue_ex_t;

typedef enum
{
  QUEUE_EX_OK,
  QUEUE_EX_FULL,
  QUEUE_EX_EMPTY
} queue_status_ex_t;

/**
  * @brief : check if the queue is empty
  * @input : *queue refer to queue_ex_t
  * @return : refer to queue_status_ex_t
  * @date : 2019.4.23
  * @author : dinghua.ma@dorabot.com
  */
static inline queue_status_ex_t queue_empty_check_ex (queue_ex_t *queue)
{
  if (0 == queue->byte_cnt)
  {
    return QUEUE_EX_EMPTY;
  }
  else
  {
    return QUEUE_EX_OK;
  }
}

/**
  * @brief : check if the queue is full
  * @input : refer to queue_ex_t
  * @return : refer to queue_status_ex_t
  * @date : 2019.4.23
  * @author : dinghua.ma@dorabot.com
  */
static inline queue_status_ex_t queue_full_check_ex (queue_ex_t *queue)
{
  if (queue->byte_cnt >= QUEUE_EX_SIZE)
  {
    return QUEUE_EX_FULL;
  }
  else
  {
    return QUEUE_EX_OK;
  }
}

queue_status_ex_t queue_ex_init (queue_ex_t *queue);
queue_status_ex_t queue_pop_head_ex (queue_ex_t *queue, void *out_data);
queue_status_ex_t queue_push_tail_ex (queue_ex_t *queue, void *data_in, uint16_t push_length);
void init_queue_ex (queue_ex_t *queue, uint8_t block_size);

#endif
