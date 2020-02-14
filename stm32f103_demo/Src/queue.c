#include "queue.h"
#include "string.h"
#include "usart.h"

/**
  * @brief : pop queue
  * @input : *queue refer to queue_ex_t
             *out_data data pop from queue
  * @return : refer to queue_status_ex_t
  * @date : 2019.4.23
  * @author : dinghua.ma@dorabot.com
  **/
queue_status_ex_t queue_pop_head_ex (queue_ex_t *queue, void *out_data)
{
  if (QUEUE_EX_EMPTY == queue_empty_check_ex (queue))
  {
    return QUEUE_EX_EMPTY;
  }

  //This is a circular queue, and if the pop operation reaches the end,
  //the queue falls back to the head of the queue.
  if ((queue->head + queue->block_size) > QUEUE_EX_SIZE)
  {
      queue->head = 0;
  }
  memcpy (out_data, &queue->buffer[queue->head], queue->block_size);
  //Queue head pointer increment
  queue->head += queue->block_size;
  //Queue pointer overflow fallback
  if (queue->head >= QUEUE_EX_SIZE)
  {
      queue->head = 0;
  }
  //Queue remaining element maintenance
  queue->byte_cnt -= queue->block_size;
  return QUEUE_EX_OK;
}

/**
  * @brief : push data to queue
  * @input : *queue refer to queue_ex_t
             *data_in source data buffer address
             push_length Length of data that needs to be enqueued
  * @return : refer to queue_status_ex_t
  * @date : 2019.4.23
  * @author : dinghua.ma@dorabot.com
  */
queue_status_ex_t queue_push_tail_ex (queue_ex_t *queue, void *data_in, uint16_t push_length)
{
  if (QUEUE_EX_FULL == queue_full_check_ex (queue))
  {
    return QUEUE_EX_FULL;
  }
  //This is a circular queue, and if the push operation reaches the end,
  //the queue falls back to the head of the queue.
  if ((queue->tail + queue->block_size) > QUEUE_EX_SIZE)
  {
      queue->tail = 0;
  }
  //Clear operation
  memset (&queue->buffer[queue->tail], 0, queue->block_size);
  //push operation
  memcpy (&queue->buffer[queue->tail], data_in, push_length);
  //Queue remaining element maintenance
  queue->byte_cnt += queue->block_size;
  //Queue tail pointer decrement
  queue->tail += queue->block_size;
  //Queue pointer overflow fallback
  if (queue->tail >= QUEUE_EX_SIZE)
  {
      queue->tail = 0;
  }
  return QUEUE_EX_OK;
}

/**
  * @brief : initialize queue
  * @input : refer to queue_ex_t
  *          block_size queue element size
  * @return : none
  * @date : 2019.4.23
  * @author : dinghua.ma@dorabot.com
  **/
void init_queue_ex (queue_ex_t *queue, uint8_t block_size)
{
  queue->byte_cnt = 0;
  queue->head = 0;
  queue->tail = 0;
  queue->block_size = block_size;
}
