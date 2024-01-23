#!/usr/bin/env python
import serial
import xmodem
import time
import os
import sys
import logging
import argparse

#logging.basicConfig(level=logging.DEBUG)

DEF_TIMEOUT = 60
DEF_BAUDRATE = 115200
DEF_PROTOCOL = 'xmodem'
DEF_CHECK = 'crc16'

PROTOCOL = [DEF_PROTOCOL, 'xmodem1k']
CHECK = [DEF_CHECK, 'sum8']

xmodemRecType = {b'\x06' : 'ACK', b'\x15' : 'NAK'}
xmodemSndType = {0x01 : 'SOH', 0x02 : 'STX', 0x04 : 'EOT', 0x10 : 'DLE', 0xC : 'CRC', 0x18 : 'CAN'}

def callback(total_packets, success_count, error_count, packet_size):
    print('packets {0} '.format(total_packets) + ' success {0}'.format(success_count) + ' error {0}'.format(error_count) + ' packet_size {0}'.format(packet_size))

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

def send_at_command(command):
    ser.write(bytes(command+"\r", encoding='ascii'))

def getc_user(size, timeout=1):
    return ser.read(size)

    #gbytes = ser.read(size)
    #if gbytes in xmodemRecType:
    #    rec = xmodemRecType[gbytes]
    #else:
    #    rec = gbytes
    #print(f'->R: {rec}')
    #return gbytes or None

def putc_user(data, timeout=1):
    return ser.write(data)

    #pbytes = ser.write(data)
    #if data[0] in xmodemSndType:
    #    header = xmodemSndType[data[0]]
    #else:
    #    header = data[0]
    #if len(data) >= 4:
    #    print(f'<-S: {header} | {data[1]}~{data[2]} | {data[3:-2]} | {data[-2]} {data[-1]}')
    #else:
    #    print(f'<-S: {data} - {header}')
    #return pbytes or None

def xmodem_recv_bin():
    modem = xmodem.XMODEM(getc=getc_user, putc=putc_user)
    print("xmodem_receiving ... ", args.file)
    stream = open(args.file, 'wb')

    _crc_mode = 1

    if (args.check != DEF_CHECK) :
        _crc_mode = 0

    ret = modem.recv(stream, crc_mode=_crc_mode, callback=callback)
    stream.close()

    if (ret) :
        print("xmodem_recv bin file done!!")
    else :
        print("xmodem_recv bin file FAIL!!!!")
        return ret

    return ret

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--port",
                        required=True, type=str,
                        help="Serial device port: COMn for windows; /dev/ttyUSBn,/dev/ttySn for unix; /dev/tty.usbserial-abcde for MacOS")
    parser.add_argument("--file",
                        required=True, type=str,
                        help="File path for saving bin file")
    parser.add_argument("--baudrate",
                        default=DEF_BAUDRATE, type=lambda x: int(x,0),
                        help="Serial device baudrate. Default is " + str(DEF_BAUDRATE))
    parser.add_argument("--check", 
                        default=DEF_CHECK, type=str,
                        choices=CHECK, 
                        help="xmodem crc_mode. Default is " + DEF_CHECK)
    parser.add_argument("--timeout",
                        default=DEF_TIMEOUT, type=lambda x: int(x,0),
                        help="Serial device timeout. Default is " + str(DEF_TIMEOUT))
    args = parser.parse_args()

    dev_init()
    print('Device init successfully')

    ret = xmodem_recv_bin()
    print("xmodem_recv bin file result = ",ret)

    # show message after recv 
    while(True):
        response = ser.readline().strip()
        print(str(response))
