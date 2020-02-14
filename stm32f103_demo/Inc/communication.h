/**
 * @Copyright Dorabot Inc.
 * @date : 2019.2.19
 * @author : dinghua.ma@dorabot.com
 * @brief : communication via uart and can bus
 */
#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include "main.h"
#include "usart.h"

void message_can (uint8_t *buffer_address, uint32_t length);
void message_uart (uint8_t *buffer_address, uint32_t length);
void message_value (uint32_t value, uint8_t format);
int get_message_can (uint8_t *buffer_address, uint32_t *length, uint16_t *message_type);
#define DEBUG 1

#if DEBUG == 1
#define message_string(string) message_uart((uint8_t *)string, sizeof(string)-1)
#define message_debug(string,length) message_uart((uint8_t *)string, length)
#define message_int(string) message_value(string,0)
#define message_hex(string) message_value(string,1)
#else
#define message_string(string)
#define message_debug(string,length)
#define message_int(string)
#define message_hex(string)
#endif

#endif
