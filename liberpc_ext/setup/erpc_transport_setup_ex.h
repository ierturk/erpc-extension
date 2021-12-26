/*
 * Copyright (c) 2021, StarGate, Inc.
 * All rights reserved.
 * 
 * Ibrahim ERTURK <ierturk@ieee.org>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ERPC_TRANSPORT_SETUP_EX_H_
#define _ERPC_TRANSPORT_SETUP_EX_H_

/*!
 * @addtogroup transport_setup
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

//! @brief Opaque transport object type.
typedef struct ErpcTransport *erpc_transport_t;
//! @brief Ready callback object type for RPMsg-Lite transport.
typedef void (*rpmsg_ready_cb)(void);

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

erpc_transport_t erpc_transport_cubemx_uart_init(void *uartDrv);
erpc_transport_t erpc_transport_serial_init(const char *portName, long baudRate);

#ifdef __cplusplus
}
#endif

#endif // _ERPC_TRANSPORT_SETUP_EX_H_
