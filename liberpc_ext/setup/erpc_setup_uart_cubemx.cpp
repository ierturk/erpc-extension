/*
 * Copyright (c) 2021, StarGate, Inc.
 * All rights reserved.
 * 
 * Ibrahim ERTURK <ierturk@ieee.org>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_manually_constructed.h"
#include "erpc_transport_setup_ex.h"
#include "erpc_uart_cubemx_transport.h"

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

static ManuallyConstructed<UartTransport> s_transport;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

erpc_transport_t erpc_transport_cubemx_uart_init(void *uartDrv)
{
    erpc_transport_t transport;

    s_transport.construct((UART_HandleTypeDef *)uartDrv);
    if (s_transport->init() == kErpcStatus_Success)
    {
        transport = reinterpret_cast<erpc_transport_t>(s_transport.get());
    }
    else
    {
        transport = NULL;
    }

    return transport;
}
