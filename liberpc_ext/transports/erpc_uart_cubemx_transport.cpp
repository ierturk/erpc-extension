/*
 * Copyright (c) 2021, StarGate, Inc.
 * All rights reserved.
 * 
 * Ibrahim ERTURK <ierturk@ieee.org>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_uart_cubemx_transport.h"
#include "erpc_uart_cubemx_transport_ex.h"

#include <cstdio>

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

static UartTransport *s_uart_instance = NULL;

__attribute__ ((aligned (32))) static uint8_t erpc_uart_rx_buff[ERPC_UART_DMA_RX_BUFF_SIZE] = {};
__attribute__ ((aligned (32))) static uint8_t erpc_uart_tx_buff[ERPC_UART_DMA_TX_BUFF_SIZE] = {};

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

UartTransport::UartTransport(UART_HandleTypeDef *uartHandle)
: m_uartHandle(uartHandle)
, m_rxSemaphore()
, m_txSemaphore() {
    s_uart_instance = this;
}

UartTransport::~UartTransport(void) {
	HAL_UART_MspDeInit(m_uartHandle);
}

erpc_status_t UartTransport::init(void) {
    return kErpcStatus_Success;
}

void UartTransport::tx_cb(void) {
    m_txSemaphore.putFromISR();
}

void UartTransport::rx_cb(void) {
    m_rxSemaphore.putFromISR();
}

/* Transfer callback */
static void TransferCallback(ERPC_UART_EVENT_SRC_ModeTypeDef event) {
    UartTransport *transport = s_uart_instance;

    if (event == ERPC_UART_EVENT_TX)
    {
        transport->tx_cb();
    }

    if (event == ERPC_UART_EVENT_RX)
    {
        transport->rx_cb();
    }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    
    UartTransport *transport = s_uart_instance;
	
    if(huart->Instance == transport->getUartHandle()->Instance) {
		TransferCallback(ERPC_UART_EVENT_RX);
	 }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    
    UartTransport *transport = s_uart_instance;

	if(huart->Instance == transport->getUartHandle()->Instance) {
		TransferCallback(ERPC_UART_EVENT_TX);
	 }
}


erpc_status_t UartTransport::underlyingReceive(uint8_t *data, uint32_t size) {

    UartTransport *transport = s_uart_instance;

    if(handShaking() != kErpcStatus_Success) {
        return kErpcStatus_ReceiveFailed;
    }


    if (HAL_UART_Receive_DMA(transport->getUartHandle(), erpc_uart_rx_buff, size) == HAL_OK) {
        if(m_rxSemaphore.get(40000)) {
            memcpy(data, erpc_uart_rx_buff, size);
            return kErpcStatus_Success;
        }
        HAL_UART_Abort(transport->getUartHandle());
    }
    return kErpcStatus_ReceiveFailed;
}

erpc_status_t UartTransport::underlyingSend(const uint8_t *data, uint32_t size) {

    UartTransport *transport = s_uart_instance;

    if(handShaking() != kErpcStatus_Success) {
        return kErpcStatus_SendFailed;
    }

    memcpy(erpc_uart_tx_buff, data, size);
    if (HAL_UART_Transmit_DMA(transport->getUartHandle(), erpc_uart_tx_buff, size) == HAL_OK) {
        if(m_txSemaphore.get(40000)) {
            return kErpcStatus_Success;
        }
        HAL_UART_Abort(transport->getUartHandle());
    }
    return kErpcStatus_SendFailed;
}

erpc_status_t UartTransport::handShaking(void) {
    
    UartTransport *transport = s_uart_instance;

    if (HAL_UART_Receive_DMA(transport->getUartHandle(), erpc_uart_rx_buff, ERPC_BOARD_UART_SLAVE_READY_MARKER_LEN) == HAL_OK) {
        if(!m_rxSemaphore.get(40000)) {
            HAL_UART_Abort(transport->getUartHandle());
            return kErpcStatus_Fail;
        }
        if((erpc_uart_rx_buff[0] != ERPC_BOARD_UART_SLAVE_READY_MARKER1) 
            || (erpc_uart_rx_buff[1] != ERPC_BOARD_UART_SLAVE_READY_MARKER2)) {

            return kErpcStatus_Fail;
        }
    } else {
        return kErpcStatus_Fail;
    }

    erpc_uart_tx_buff[0] = ERPC_BOARD_UART_SLAVE_READY_MARKER1;
    erpc_uart_tx_buff[1] = ERPC_BOARD_UART_SLAVE_READY_MARKER2;

    if (HAL_UART_Transmit_DMA(transport->getUartHandle(), erpc_uart_tx_buff, ERPC_BOARD_UART_SLAVE_READY_MARKER_LEN) == HAL_OK) {
        if(!m_txSemaphore.get(40000)) {
            HAL_UART_Abort(transport->getUartHandle());
            return kErpcStatus_Fail;
        }
    } else {
        return kErpcStatus_Fail;
    }
    
    return kErpcStatus_Success;
}
