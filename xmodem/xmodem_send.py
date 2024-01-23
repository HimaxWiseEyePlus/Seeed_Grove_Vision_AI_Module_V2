#!/usr/bin/env python
import serial
import xmodem
import time
import os
import sys
import logging
import argparse
import math

#logging.basicConfig(level=logging.DEBUG)

DEF_TIMEOUT = 60
DEF_BAUDRATE = 115200
DEF_PROTOCOL = 'xmodem'
DEF_CHECK = 'crc16'

PROTOCOL = [DEF_PROTOCOL, 'xmodem1k']
CHECK = [DEF_CHECK, 'sum8']

xmodemRecType = {b'\x06' : 'ACK', b'\x15' : 'NAK'}
xmodemSndType = {0x01 : 'SOH', 0x02 : 'STX', 0x04 : 'EOT', 0x10 : 'DLE', 0xC : 'CRC', 0x18 : 'CAN'}
send_bin_total_packtets = 0

def callback(total_packets, success_count, error_count):
    #print('packets {0} '.format(total_packets) + ' success {0}'.format(success_count) + ' error {0}'.format(error_count))

    if (send_bin_total_packtets != 0):
        bar_total = 30
        bar_string_fmt = "\r[{}{}] {:.2%} {}/{} error: {}"
        bar_cnt = (int((total_packets/send_bin_total_packtets)*bar_total))
        space_cnt = bar_total - bar_cnt

        progress = bar_string_fmt.format(
            "█" * bar_cnt,
            " " * space_cnt,
            total_packets/send_bin_total_packtets,
            total_packets,
            send_bin_total_packtets,
            error_count
        )

        print(progress, end="")
        percent = total_packets/send_bin_total_packtets
        
        if percent >= 1:
            print("\n")

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

def xmodem_send_bin():
    global send_bin_total_packtets
    model_list = args.model
    image_file = args.file

    if ((image_file == None) and (model_list == None)):
        print("--file and --model error parameter")
        return False

    print("Please press reset button!!")

    while(True):
        response = ser.readline().strip()
        print(str(response))
        send_at_command('1')

        if (str(response).count('Send data using the xmodem protocol from your terminal') > 0) :
            break

    time.sleep(1)
    ser.flushInput()
    send_at_command('1')

    ret = False
    _wait_reboot_system = False
    packtet_size = 1024

    if (args.protocol == DEF_PROTOCOL):
        packtet_size = 128

    modem = xmodem.XMODEM(getc=getc_user, putc=putc_user, mode=args.protocol)

    if (image_file != None):
        print("xmodem_sending >>", image_file)
        send_bin_total_packtets = math.ceil(os.path.getsize(image_file) / packtet_size)
        stream = open(image_file, 'rb')
        ret = modem.send(stream, callback=callback)
        stream.close()
        
        if (ret) :
            print("xmodem_send bin file done!!")
            _wait_reboot_system = True
        else :
            print("xmodem_send bin file FAIL!!!!")
            return ret

    if model_list == None:
        return ret

    idx = 0

    while (idx < len(model_list)):
        model_arg = model_list[idx].split(" ")
        
        if (len(model_arg) < 2) :
            print("--model error parameter")
            return False

        while(_wait_reboot_system):
            response = ser.readline().strip()        
            if (str(response).count('Do you want to end file transmission and reboot system? (y)') > 0) :
                break

        time.sleep(1)
        ser.flushInput()
        send_at_command('n')

        model_file = model_list[idx].split(" ")[0]
        model_file_dir = os.path.dirname(os.path.realpath(model_file))
        preamble_data = '_temp_model_' + str(idx) + '_preamble_data.bin'
        preamble_data_filepath = os.path.join(model_file_dir, preamble_data)
        print("generate {0} for {1} preamble data".format(preamble_data, model_file))

        model_position = int(model_list[idx].split(" ")[1], 16)
        model_offset = 0
        if (len(model_arg) > 2) :
            model_offset = int(model_list[idx].split(" ")[2], 16)
            
        header = [0xC0, 0x5A] + list(model_position.to_bytes(4, 'little')) + list(model_offset.to_bytes(4, 'little')) + [0x5A, 0xC0] + [0xFF] * (packtet_size - 12)
        idx = idx + 1

        wb = open(preamble_data_filepath, 'wb')
        wb.write(bytearray(header))
        wb.close()

        print("xmodem_sending >>", preamble_data)
        send_bin_total_packtets = math.ceil(os.path.getsize(preamble_data_filepath) / packtet_size)
        stream = open(preamble_data_filepath, 'rb')
        ret = modem.send(stream, callback=callback)
        stream.close()

        if (ret) :
            print("xmodem_send bin file done!!")
        else :
            print("xmodem_send bin file FAIL!!!!")
            return ret

        while(True):
            response = ser.readline().strip()        
            if (str(response).count('Do you want to end file transmission and reboot system? (y)') > 0) :
                break

        time.sleep(1)
        ser.flushInput()
        send_at_command('n')

        print("xmodem_sending >>", model_file)

        stream = open(model_file, 'rb')
        send_bin_total_packtets = math.ceil(os.path.getsize(model_file) / packtet_size)
        ret = modem.send(stream, callback=callback)
        stream.close()

        if (ret) :
            print("xmodem_send bin file done!!")
        else :
            print("xmodem_send bin file FAIL!!!!")
            return ret
    return ret

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--port",
                        required=True, type=str,
                        help="Serial device port: COMn for windows; /dev/ttyUSBn,/dev/ttySn for unix; /dev/tty.usbserial-abcde for MacOS")
    parser.add_argument("--file",
                        #required=True, 
                        type=str,
                        help="File path for sending bin file")
    parser.add_argument("--baudrate",
                        default=DEF_BAUDRATE, type=lambda x: int(x,0),
                        help="Serial device baudrate. Default is " + str(DEF_BAUDRATE))
    parser.add_argument("--protocol", 
                        default=DEF_PROTOCOL, type=str,
                        choices=PROTOCOL, 
                        help="File transfer protocol. Default is " + DEF_PROTOCOL)
    parser.add_argument("--model", type=str, action='append', help='--model="bin_file flash_address_hex offset_hex"')
    parser.add_argument("--timeout",
                        default=DEF_TIMEOUT, type=lambda x: int(x,0),
                        help="Serial device timeout. Default is " + str(DEF_TIMEOUT))

    args = parser.parse_args()

    dev_init()
    print('Device init successfully')

    ret = xmodem_send_bin()
    print("xmodem_send bin file result = ",ret)

    # show message after send 
    while(True):
        response = ser.readline().strip()
        print(str(response))
