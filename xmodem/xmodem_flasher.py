import argparse
import serial
from xmodem import XMODEM
from tqdm import tqdm
import os
import requests
import time

class Flasher:
    def __init__(self, file_path, port, baudrate=115200):
        self.file_path = file_path
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.modem = None

    def open_serial_connection(self):
        # Open serial connection
        self.ser = serial.Serial(self.port, baudrate=self.baudrate, timeout=1)

    def close_serial_connection(self):
        # Close serial connection
        if self.ser:
            self.ser.close()

    def read_func(self, size, timeout=1):
        return self.ser.read(size)

    def write_func(self, data, timeout=1):
        return self.ser.write(data)

    def wait_for_burn_mode(self, timeout=5000):
        rbuf = b""
        self.ser.timeout = 0.01
        print("Waiting for burn mode")
        while True:
            if timeout % 100 == 0:
                print(".", end="")
            self.ser.write(b"1")
            rbuf += self.ser.read(128)
            if "Xmodem download and burn FW image".encode() in rbuf:
                rbuf = b""
                self.ser.write(b"1")
                break
            timeout -= 10
            if timeout <= 0:
                print(".")
                raise Exception("Timeout waiting for burn mode")

    def start_transfer(self):
        while True:
            if self.ser.read(1) == b"C":
                print("\nStart transfer")
                time.sleep(0.2)
                break
        self.ser.timeout = 1
        self.modem = XMODEM(self.read_func, self.write_func)

    def transfer_file(self):
        # Get file size
        file_size = os.path.getsize(self.file_path)

        # Open the file to be sent
        with open(self.file_path, "rb") as file:
            # Set up progress bar
            progress_bar = tqdm(
                total=file_size, unit="B", unit_scale=True, unit_divisor=1024, ncols=80
            )

            # Define callback function to update progress bar
            def callback_written(total_packets, success_count, error_count):
                progress_bar.update(total_packets * 128 - progress_bar.n)

            # Perform transfer
            status = self.modem.send(file, callback=callback_written)

        progress_bar.close()
        return status

    def wait_for_completion(self, timeout=10000):
        rbuf = b""
        self.ser.timeout = 0.01
        print("Waiting for completion")
        while True:
            if timeout % 100 == 0:
                print(".", end="")
            rbuf += self.ser.read(128)
            if (
                "Do you want to end file transmission and reboot system".encode()
                in rbuf
            ):
                rbuf = b""
                self.ser.write(b"y")
                break
            timeout -= 10
            if timeout <= 0:
                print(".")
                raise Exception("Timeout waiting for completion")
        print("\nFlash completed")

    def flash(self):
        try:
            self.open_serial_connection()
            self.wait_for_burn_mode()
            self.start_transfer()
            status = self.transfer_file()
            if status:
                self.wait_for_completion()
            else:
                print("\nFlash failed")
        finally:
            self.close_serial_connection()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Flash a file via XMODEM over serial port."
    )
    parser.add_argument(
        "--file_path",
        type=str,
        default="/tmp/output.img",
        help="Path to the file to be flashed",
    )
    parser.add_argument("--port", type=str, default="/dev/ttyACM0", help="Serial port")
    parser.add_argument(
        "--baudrate", type=int, default=921600, help="Baud rate (default: 921600)"
    )
    parser.add_argument("--server", type=str, default="127.0.0.1:8000")
    parser.add_argument("--firmware", type=str, default="output.img")
    args = parser.parse_args()

    if len(args.server):
        # download firmware from server
        url = f"http://{args.server}/{args.firmware}"
        r = requests.get(url)

        # save firmware to /tmp, delete if already exists
        with open(f"/tmp/{args.firmware}", "wb") as f:
            f.write(r.content)

        # display firmware hash
        print("Firmware hash:")
        os.system(f"sha256sum /tmp/{args.firmware}")

    flasher = Flasher(args.file_path, args.port, args.baudrate)
    flasher.flash()
    