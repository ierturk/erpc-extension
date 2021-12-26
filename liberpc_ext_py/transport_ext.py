#!/usr/bin/env python

# Copyright (c) 2021, StarGate, Inc.
# All rights reserved.
#
# Ibrahim ERTURK <ierturk@ieee.org>
#
# SPDX-License-Identifier: BSD-3-Clause

from erpc.transport import SerialTransport 
import serial
import threading
import struct
from erpc.client import RequestError


class SerialTransportExt(SerialTransport):
    HS_MARKER_LEN   = 2
    HS_MARKER1      = 0xab
    HS_MARKER2      = 0xcd
    
    def __init__(self, url, baudrate, **kwargs):
        super(SerialTransportExt, self).__init__(url, baudrate, timeout=0.1)
        self._markerData = bytearray(struct.pack('<BB', self.HS_MARKER1, self.HS_MARKER2))

    def hand_shake(self):
        try:
            self._base_send(self._markerData)
        except serial.SerialTimeoutException:
            return False

        try:
            markerData = self._base_receive(len(self._markerData))
        except serial.SerialTimeoutException:
            return False

        try:
            m1, m2 = struct.unpack('<BB', markerData)
        except:
            return False
        
        if m1 == self.HS_MARKER1 and m2 == self.HS_MARKER2:
            return True
        else:
            return False


    def send(self, message):

        try:
            self._sendLock.acquire()

            # Send header
            if self.hand_shake() == False:
                return False
            crc = self._Crc16.computeCRC16(message)
            header = bytearray(struct.pack('<HH', len(message), crc))
            assert len(header) == self.HEADER_LEN
            self._base_send(header)

            # Send message 
            if self.hand_shake() == False:
                return False
            self._base_send(message)

        finally:
            self._sendLock.release()


    def receive(self):
        try:
            self._receiveLock.acquire()

            # Read fixed size header containing the message length.
            if self.hand_shake() == False:
                return False
            headerData = self._base_receive(self.HEADER_LEN)
            messageLength, crc = struct.unpack('<HH', headerData)

            # Now we know the length, read the rest of the message.
            if self.hand_shake() == False:
                return False
            data = self._base_receive(messageLength)
            computedCrc = self._Crc16.computeCRC16(data)

            if computedCrc != crc:
                raise RequestError("invalid message CRC")
            return data
        except Exception as e:
            print(e)
        finally:
            self._receiveLock.release()

