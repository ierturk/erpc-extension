/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_config_ex.h"
#include "erpc_serial_transport.h"

#include "erpc_message_buffer.h"
#include "erpc_serial.h"

#include <cstdio>
#include <string>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#else
#include <termios.h>
#endif

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

static inline erpc_status_t handShaking(int serialHandle);

SerialTransport::SerialTransport(const char *portName, speed_t baudRate)
: m_serialHandle(0)
, m_portName(portName)
, m_baudRate(baudRate)
{
}

SerialTransport::~SerialTransport(void)
{
    serial_close(m_serialHandle);
}

erpc_status_t SerialTransport::init(uint8_t vtime, uint8_t vmin)
{
    erpc_status_t status = kErpcStatus_Success;

    m_serialHandle = serial_open(m_portName);
    if (-1 == m_serialHandle)
    {
        status = kErpcStatus_InitFailed;
    }
#ifdef _WIN32
    // TODO
#else
    if (status == kErpcStatus_Success)
    {
        if (!isatty(m_serialHandle))
        {
            status = kErpcStatus_InitFailed;
        }
    }
#endif
    if (status == kErpcStatus_Success)
    {
        if (-1 == serial_setup(m_serialHandle, m_baudRate))
        {
            status = kErpcStatus_InitFailed;
        }
    }

    if (status == kErpcStatus_Success)
    {
        if (-1 == serial_set_read_timeout(m_serialHandle, vtime, vmin))
        {
            status = kErpcStatus_InitFailed;
        }
    }
#ifdef _WIN32
    // TODO
#else
    if (status == kErpcStatus_Success)
    {
        if (-1 == tcflush(m_serialHandle, TCIOFLUSH))
        {
            status = kErpcStatus_InitFailed;
        }
    }
#endif

    return status;
}

uint32_t hs_cnt = 0; 
erpc_status_t SerialTransport::underlyingSend(const uint8_t *data, uint32_t size) {

    if(handShaking(m_serialHandle) != kErpcStatus_Success) {
        return kErpcStatus_SendFailed;
    }

    uint32_t bytesWritten = serial_write(m_serialHandle, (char *)data, size);
    return (size != bytesWritten) ? kErpcStatus_SendFailed : kErpcStatus_Success;
}


erpc_status_t SerialTransport::underlyingReceive(uint8_t *data, uint32_t size) {

    if(handShaking(m_serialHandle) != kErpcStatus_Success) {
        return kErpcStatus_ReceiveFailed;
    }

    uint32_t bytesRead = serial_read(m_serialHandle, (char *)data, size);
    return (size != bytesRead) ? kErpcStatus_ReceiveFailed : kErpcStatus_Success;
}


static inline erpc_status_t handShaking(int serialHandle) {

    uint8_t sync_data[ERPC_BOARD_UART_SLAVE_READY_MARKER_LEN] = {0};
    sync_data[0] = ERPC_BOARD_UART_SLAVE_READY_MARKER1;
    sync_data[1] = ERPC_BOARD_UART_SLAVE_READY_MARKER2;
    
    tcflush(serialHandle, TCIOFLUSH);

    uint32_t bytesWritten = serial_write(serialHandle, (char *)sync_data, ERPC_BOARD_UART_SLAVE_READY_MARKER_LEN);
    if(bytesWritten != ERPC_BOARD_UART_SLAVE_READY_MARKER_LEN) {
        return kErpcStatus_Fail;
    }

    uint32_t bytesRead = serial_read(serialHandle, (char *)sync_data, ERPC_BOARD_UART_SLAVE_READY_MARKER_LEN);
    if(bytesRead != ERPC_BOARD_UART_SLAVE_READY_MARKER_LEN) {
        return kErpcStatus_Fail;
    }

    if((sync_data[0] != ERPC_BOARD_UART_SLAVE_READY_MARKER1) 
            || (sync_data[1] != ERPC_BOARD_UART_SLAVE_READY_MARKER2)) {

        tcflush(serialHandle, TCIOFLUSH);
        return kErpcStatus_Fail;
    }
    return kErpcStatus_Success;
}

