# md_display.py
#### CGP 12/4/25

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