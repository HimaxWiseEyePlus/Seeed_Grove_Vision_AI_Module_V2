"""

Python script to parse WW500 serial port output and display motion detect data.

Thanks to ChatGPT!

The 32 motion detection bytes will bex hex characters.
These lines will begin with '>' and end with a newline, like this:
```
> dd 06 f4 00 c4 00 84 32 e0 12 a1 02 20 01 7f 46 7f 20 e7 18 e7 ac c8 4f ff ae f5 e2 f3 24 00 00
```



## 🛠️ How It Works

This application provides a live visualization of motion data from the Himax HM0360 image sensor streamed over a serial connection. It reads the 32-byte motion detection frame, decodes it into a 16×16 motion grid, and displays the motion visually and textually in real time using a GUI.

### 📡 Data Input (Serial)
- The HM0360 sensor outputs lines of motion data at 921600 baud.
- Each motion frame begins with a `>` and consists of **32 hexadecimal bytes** (e.g., `> dd 06 f4 ... 00`).
- Each byte represents 8 motion bits (LSB = bottom row, MSB = top), totaling **256 bits** for a 16×16 grid.
- Incoming data may include **escape characters or ANSI codes**, which are stripped before parsing.

### 🔢 Motion Parsing
- The 32 hex bytes are decoded into individual bits using bitwise operations.
- The 256 bits are reshaped into a 16×16 grid using **column-major order**, since motion bits map vertically in the RoI.
- The matrix is flipped vertically to match a top-left origin layout for display.

### 🎨 GUI Visualization
- A **Tkinter GUI** is used with an embedded **Matplotlib** plot to render the motion grid.
- Each grid cell is either:
  - **Blue** (motion detected), or
  - **Gray** (no motion).
- The frame number and total number of motion lines received are shown in the graph title.

### 🖥️ Serial Console (GUI-integrated)
- A scrolling text area in the GUI shows all raw serial input.
- ANSI escape codes are removed for clean display.
- Font size is configurable for readability.

### 🔁 Real-Time Updates
- Motion updates are not tied to a fixed timer. Instead:
  - Every valid motion line (`>` prefix) immediately updates the motion grid.
  - The GUI is refreshed as fast as motion lines are received (typically hundreds per second).

### 🔍 Diagnostics
- Two counters are shown:
  - **Frame Count**: how many motion frames have been parsed and rendered.
  - **Motion Line Count**: how many valid motion lines have been received (regardless of whether the data changed).

This makes it easy to spot if any frames are skipped or if data is malformed.

### Usage:
- Run the script from the command line:
  ```sh
  py md_display.py
  ```

"""

import serial
import numpy as np
import tkinter as tk
from tkinter import scrolledtext
import threading
import queue
import time
import re

import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.colors import ListedColormap

# --- CONFIG ---
SERIAL_PORT = "COM3"  # Change to your port
BAUD_RATE = 921600
FONT_SIZE = 12        # << Adjust font size here

# --- SHARED STATE ---
motion_grid = np.zeros((16, 16), dtype=int)
frame_count = 0
motion_queue = queue.Queue()
console_queue = queue.Queue()

# --- INIT SERIAL ---
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

# --- STRIP ANSI ESCAPE CODES ---
def strip_ansi(text):
    ansi_escape = re.compile(r'\x1B[@-_][0-?]*[ -/]*[@-~]')
    return ansi_escape.sub('', text)

# --- MOTION DATA PARSER ---
def parse_motion_line(line):
    hex_strs = line[1:].strip().split()
    if len(hex_strs) != 32:
        return None

    bits = []
    for hex_str in hex_strs:
        byte = int(hex_str, 16)
        bits.extend([(byte >> i) & 1 for i in range(8)])  # LSB first

    bit_array = np.array(bits).reshape((16, 16), order='F')  # Column-major
    return np.flipud(bit_array)

# --- SERIAL THREAD ---
def serial_reader():

    while True:
        try:
            raw = ser.readline()
            if not raw:
                continue  # Nothing read

            line = raw.decode('utf-8', errors='ignore').strip()
            # print(f"RAW: {raw} | DECODED: {line}" )

            if line:  # Only enqueue non-blank lines
                console_queue.put(strip_ansi(line))

            if line.lstrip().startswith('>'):
                motion_queue.put(line)
        except Exception as e:
            console_queue.put(f"[ERROR] {e}")

# --- GUI UPDATE LOOP ---
def gui_update():
    global frame_count  

    try:
        # Always process one motion line if present
        if not motion_queue.empty():
            line = motion_queue.get_nowait()
            new_grid = parse_motion_line(line)
            if new_grid is not None:
                motion_grid[:, :] = new_grid
                frame_count += 1
                ax.set_title(f"HM0360 Motion Grid - Frame #{frame_count}")
                img.set_array(motion_grid)
                canvas.draw()
    except queue.Empty:
        pass

    try:
        while not console_queue.empty():
            msg = console_queue.get_nowait()
            console_text.insert(tk.END, msg + "\n")
            console_text.see(tk.END)
    except queue.Empty:
        pass

    root.after(10, gui_update)  # ~100 FPS check loop

# --- GUI SETUP ---
root = tk.Tk()
root.title("HM0360 Motion Viewer")

# --- Matplotlib Figure ---
fig, ax = plt.subplots()
cmap = ListedColormap(["gray", "blue"])
img = ax.imshow(motion_grid, cmap=cmap, vmin=0, vmax=1)
ax.set_xticks([])
ax.set_yticks([])
ax.set_title("HM0360 Motion Grid...")

canvas = FigureCanvasTkAgg(fig, master=root)
canvas_widget = canvas.get_tk_widget()
canvas_widget.pack()

# --- Console Output (Below Grid) ---
console_text = scrolledtext.ScrolledText(root, height=15, width=100, bg="black", fg="white", font=("Courier", FONT_SIZE), insertbackground="white")
console_text.pack(fill=tk.BOTH, expand=True)

# --- Start Serial Thread ---
threading.Thread(target=serial_reader, daemon=True).start()

# --- Start GUI Update Loop ---
root.after(10, gui_update)
root.mainloop()

# --- Cleanup ---
ser.close()


