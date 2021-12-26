#!/usr/bin/env python

# Copyright (c) 2021, StarGate, Inc.
# All rights reserved.
#
# Ibrahim ERTURK <ierturk@ieee.org>
#
# SPDX-License-Identifier: BSD-3-Clause

import erpc
from eRPCLibExt.erpc_py_lib_ext.transport_ext import SerialTransportExt
from erpc_service import erpc_nucleo_uart
import time

def runClient(transport):
    # eRPC service
    clientManager = erpc.client.ClientManager(transport, erpc.basic_codec.BasicCodec)
    client = erpc_nucleo_uart.client.uartClient(clientManager)

    hb_cnt = 0
    while True:
        try:
            status = erpc.Reference()

            try:
                client.erpc_uart_HeartBeat(status)
            except Exception as e:
                print(e)

            time.sleep(0.1)
            print("{} - {}".format(hb_cnt, status.value))
            hb_cnt += 1
        except KeyboardInterrupt:
            SystemExit()

def main():
    transport = SerialTransportExt("/dev/ttyACM0", 115200)
    runClient(transport)


if __name__ == "__main__":
    main()