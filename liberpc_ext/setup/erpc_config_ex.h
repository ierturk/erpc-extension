/*
 * Copyright (c) 2021, StarGate, Inc.
 * All rights reserved.
 * 
 * Ibrahim ERTURK <ierturk@ieee.org>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ERPC_CONFIG_EX_H_
#define _ERPC_CONFIG_EX_H_

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

#ifndef ERPC_BOARD_UART_SLAVE_READY_USE_GPIO
#define ERPC_BOARD_UART_SLAVE_READY_MARKER_LEN 2U
#define ERPC_BOARD_UART_SLAVE_READY_MARKER1 0xABU
#define ERPC_BOARD_UART_SLAVE_READY_MARKER2 0xCDU
#else
#ifndef ERPC_BOARD_UART_INT_GPIO
#error "Please define the ERPC_BOARD_UART_INT_GPIO used to notify when the UART Slave is ready to transmit"
#endif
#endif


#endif // _ERPC_CONFIG_EX_H_
