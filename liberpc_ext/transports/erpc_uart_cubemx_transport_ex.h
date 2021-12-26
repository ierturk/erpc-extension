/*
 * Copyright (c) 2021, StarGate, Inc.
 * All rights reserved.
 * 
 * Ibrahim ERTURK <ierturk@ieee.org>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ERPC_UART_CUBEMX_TRANSPORT_EX_H_
#define _ERPC_UART_CUBEMX_TRANSPORT_EX_H_

#include "usart.h"

#define ERPC_UART_DMA_RX_BUFF_SIZE  1024
#define ERPC_UART_DMA_TX_BUFF_SIZE  1024


typedef enum {
  ERPC_UART_EVENT_NONE  = 0,
  ERPC_UART_EVENT_TX    = 1,
  ERPC_UART_EVENT_RX    = 2
} ERPC_UART_EVENT_SRC_ModeTypeDef;

#endif /* _ERPC_UART_CUBEMX_TRANSPORT_EX_H_ */
