import tkinter as tk
from tkinter import scrolledtext
import serial
import threading
import time
import re

# --- Configuration ---
SERIAL_PORT = 'COM13'
BAUD_RATE = 921600
ROWS = 30
COLS = 90
FONT = ('Lucida Console', 10)

SLIP_END = 0xC0
SLIP_ESC = 0xDB
SLIP_ESC_END = 0xDC
SLIP_ESC_ESC = 0xDD

# --- Globals ---
ser = None
binary_mode = False
binary_buffer = bytearray()
binary_count = 0

# --- ANSI Color Mapping (basic) ---
ANSI_COLORS = {
    '30': 'black', '31': 'red', '32': 'green', '33': 'yellow',
    '34': 'blue', '35': 'magenta', '36': 'cyan', '37': 'white'
}

class TerminalApp:
    def __init__(self, master):
        self.master = master
        master.title(f"SLIP Terminal - {SERIAL_PORT} @ {BAUD_RATE} baud")

        self.text = scrolledtext.ScrolledText(master, width=COLS, height=ROWS, bg='black', fg='white', font=FONT)
        self.text.pack(fill=tk.BOTH, expand=True)
        self.text.config(state=tk.DISABLED)

        self.text.bind('<Key>', self.send_key)

        self.current_tag = 'white'
        for color in ANSI_COLORS.values():
            self.text.tag_config(color, foreground=color)
        self.text.tag_config('green', foreground='green')

        self.start_serial_thread()

    def send_key(self, event):
        if ser and ser.is_open and not binary_mode:
            ser.write(event.char.encode('utf-8'))

    def insert_text(self, s, color='white'):
        self.text.config(state=tk.NORMAL)
        self.text.insert(tk.END, s, color)
        self.text.see(tk.END)
        self.text.config(state=tk.DISABLED)

    def parse_ansi(self, s):
        """Very basic ANSI color parser."""
        ansi_escape = re.compile(r'\x1b\[(\d+)m')
        parts = ansi_escape.split(s)
        current_color = 'white'
        for i, part in enumerate(parts):
            if i % 2 == 0:
                if part:
                    self.insert_text(part, current_color)
            else:
                current_color = ANSI_COLORS.get(part, 'white')

    def start_serial_thread(self):
        global ser
        try:
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
            threading.Thread(target=self.read_serial, daemon=True).start()
        except serial.SerialException as e:
            self.insert_text(f"Failed to open serial port: {e}\n", 'red')

    def read_serial(self):
        global binary_mode, binary_buffer, binary_count
        esc_flag = False
        while True:
            if not ser or not ser.is_open:
                time.sleep(0.5)
                continue

            try:
                b = ser.read(1)
                if not b:
                    continue
                byte = b[0]

                if binary_mode:
                    if byte == SLIP_END:
                        self.insert_text(f"Binary data received: {binary_count}\n", 'green')
                        binary_mode = False
                        binary_count = 0
                        binary_buffer.clear()
                    elif esc_flag:
                        if byte == SLIP_ESC_END:
                            self.processBinary(bytes([SLIP_END]))
                        elif byte == SLIP_ESC_ESC:
                            self.processBinary(bytes([SLIP_ESC]))
                        esc_flag = False
                    elif byte == SLIP_ESC:
                        esc_flag = True
                    else:
                        self.processBinary(bytes([byte]))
                else:
                    if byte == SLIP_END:
                        binary_mode = True
                        self.insert_text("Entering binary mode...", 'green')
                    else:
                        self.parse_ansi(b.decode('utf-8', errors='ignore'))
            except Exception as e:
                self.insert_text(f"\n[Error reading serial: {e}]\n", 'red')
                time.sleep(0.5)

    def processBinary(self, data):
        global binary_count
        binary_count += len(data)
        # Stub: count bytes. You can later extend this to do real processing.


if __name__ == '__main__':
    root = tk.Tk()
    app = TerminalApp(root)
    root.mainloop()
