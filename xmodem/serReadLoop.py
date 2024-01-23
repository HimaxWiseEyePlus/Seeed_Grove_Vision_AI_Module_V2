#!/usr/bin/env python
import serial
import time
import os
import sys
import logging
import argparse
import threading

#logging.basicConfig(level=logging.DEBUG)

DEF_TIMEOUT = 60
DEF_BAUDRATE = 115200

def uart_open(ser, com, baudrate, timeout):
    ser.port = com
    ser.timeout = timeout
    ser.baudrate = baudrate
    ser.bytesize = serial.EIGHTBITS
    ser.stopbits = serial.STOPBITS_ONE
    ser.xonxoff = 0
    ser.rtscts = 0
    ser.parity = serial.PARITY_NONE
    ser.open()
    print("Open Serial Port", ser.port)

def dev_init():
    global ser
    ser = serial.Serial()

    try:
        uart_open(ser=ser, com=args.port, baudrate=args.baudrate, timeout=args.timeout)
        ser.flushInput()
        ser.flushOutput()
    except:
        print("Uart port open fail")
        sys.exit(-1)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--port",
                        required=True, type=str,
                        help="Serial device port: COMn for windows; /dev/ttyUSBn,/dev/ttySn for unix; /dev/tty.usbserial-abcde for MacOS")
    parser.add_argument("--baudrate",
                        default=DEF_BAUDRATE, type=lambda x: int(x,0),
                        help="Serial device baudrate. Default is " + str(DEF_BAUDRATE))
    parser.add_argument("--timeout",
                        default=DEF_TIMEOUT, type=lambda x: int(x,0),
                        help="Serial device timeout. Default is " + str(DEF_TIMEOUT))
    args = parser.parse_args()

    dev_init()
    print('Device init successfully')

    while(True):
        response = ser.readline().strip()
        print(str(response))

