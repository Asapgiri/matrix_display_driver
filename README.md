# 8x8 LED Matrix text driver

This repository demonstrates how to use an 8x8 MAX7219 display trough SPI, with STM32 board.

**You can find the driver file in:**
| File                            | Description |
| ------------------------------- | ----------- |
| `./Core/Inc/ledmatrix_driver.h` | Header file |
| `./Core/Src/ledmatrix_driver.c` | Source file |
| `./Core/Src/main.c`             | Example  |

# Driver panel - MAX7219
<details open>
<summary>Description</summary>

I am using driver panels, that has the MAX7219 ICs on them.
These are very common on LED Matrix displays.
Originally they are meant to drive 7 segment displays, but they are capable of driving LED Matrixes, up to 8x8 segments.

This approach works, by setting the displays line by line.

## Inputs of the board
<details open>
<summary>Inputs</summary>

| PIN | Definition |
| --- | ---------- |
| VCC | Power |
| GND | Ground |
| DIN | Digitral Input |
| CS  | Chip Select |
| CLK | Clock signal |

</details>

## Outputs of the board
<details open>
<summary>Outputs</summary>

| PIN | Definition |
| --- | ---------- |
| VCC | Power |
| GND | Ground |
| DOUT | Digitral Output |
| CS  | Chip Select |
| CLK | Clock signal |

</details>

### Useful:
These driver ICs can be chained on the DOT and DIN pins.
When chained, they send out the priviously recived bit, 0.5 clock cycles later.

### Mailfunction:
Chip select must be pulled down all the time on write like in the example.
If used by the default driver, it will mailfunction, and do not separate display writes!


## The commands available on the display
<details open>
<summary>Setup</summary>

The command structure for the SPI is command first, data second, and it waits for commands in Lower Bit First mode.

The initialization sequence of the displays are:

1. Disable Test mode `(TEST 0x00)`,
2. Set Scan limit to 7, to show the digits `(SCAN_LIMIT 0x07)`,
3. Disable Decode mode `(DECODE 0x00)`,
4. Enable normal mode `(SHUTDOWN/ENABLE 0x01)`.

</details>

### Command table
<details open>
<summary>Commands</summary>

The top 4 bits of the command are ignored.

| Command    | HEX    | Description | Accept
| ---------- | ------ | ----------- | -----
| NOOP       | `0xX0` | No operand
| D0         | `0xX1` | Digit 0     | 8 bits data
| D1         | `0xX2` | Digit 1     | 8 bits data
| D2         | `0xX3` | Digit 2     | 8 bits data
| D3         | `0xX4` | Digit 3     | 8 bits data
| D4         | `0xX5` | Digit 4     | 8 bits data
| D5         | `0xX6` | Digit 5     | 8 bits data
| D6         | `0xX7` | Digit 6     | 8 bits data
| D7         | `0xX8` | Digit 7     | 8 bits data
| DECODE     | `0xX9` | Decode Mode | `0x00` - no decode, `0x01`, `0x0F`, `0xFF` - decode settings for 7 digit display.
| INTENSITY  | `0xXA` | Set display brightness | `0xX[0-F]` 4 bits intensity.
| SCAN_LIMIT | `0xXB` | Scan Limit  | `0xX[0-7]` 4 bits, how many digits are displayed. from display 0 only to all.
| SHUTDOWN   | `0xXC` | Disable or Enable | 1 bit [set]
| TEST       | `0xXF` | Display Test

</details>

</details>


# Driver implementation
<details open>
<summary>Summary</summary>

To drive this dispaly you need to wire up the board, and **don't use the hardver cpip select**.
You can eather use **3.3V** or **5V** to drive the displays.

In this implementation I used hardver **SPI1**, with the following pins:
| PIN  | Function |
| ---- | -------- |
| PE15 | MOSI     |
| PE13 | SCK      |
| PE12 | CS       |

And set mode to **Transmit Only Master**,
and NSS (NCS) to **Disable**.


## Usage

After setting up the condigurations, use the driver like the following:
```c
#include "ledmatrix_driver.h"

/* Default init variables */

#define LEDMATRIX_SEGMENTS_W 	(12)    // 12 segment wide
#define LEDMATRIX_SEGMENTS_H 	(1)     //  1 segment high

int main(void) {
  /* Default init function */

  ledmx_init(&hspi1, LEDMATRIX_SEGMENTS_W, LEDMATRIX_SEGMENTS_H, true);
  ledmx_clear();
  ledmx_setup_shift_text("Hello World", 3, left, 100);

  while (1) {
    ledmx_shift_text(); // rerender text in internal buffer
    ledmx_display();    // show stored buffer on LED Matrixes
  }
}
```

## Structures used
```c
enum ledmx_text_dir {
    left  = 0,
    right = 1
};

struct shift_text {
    char*               text;
    enum ledmx_text_dir direction;
    uint64_t            ms;         // Change interval
    uint64_t            ms_change;
    uint64_t            ms_i_min;
    uint64_t            ms_i_max;
    uint                position;   // current position on display in pixels
    uint                length;     // length on display in pixels
    uint                space;
};
```

## Function descriptions
```c
/* Content of ledmatrix_driver.h */

/// Initialize LED Matrix driver
/// 
/// input hspi:         SPI interface
/// input s_width:      Number of difplays horiyontally
/// input s_height:     Number of difplays vertically
/// input odd_inverted: Are second lines inverted? (Can be better to wire) (not used)
void ledmx_init(SPI_HandleTypeDef* hspi, uint s_width, uint s_height, char odd_inverted);

/// Set constans text to display
/// 
/// input text:         Text to dispay
void ledmx_text(const char* text);

/// Set shifting constans text to display
/// 
/// input text:         Text to dispay
/// input space:        Number of spaces until repeate
/// input direction:    Shift direction
/// input ms:           Shift interval in microseconsd
void ledmx_setup_shift_text(const char* text, uint space, enum ledmx_text_dir direction, uint ms);

/// Set shifting text direction on the fly
/// 
/// input direction:    New shifting direction
void ledmx_set_text_dir(enum ledmx_text_dir direction);

/// Rerender shifting text in internal buffer
void ledmx_shift_text(void);

/// Increase shift sepeed with SHIFT_MS_STEP
void ledmx_text_speed_up(void);

/// Decrease shift sepeed with SHIFT_MS_STEP
void ledmx_text_slow_down(void);

/// Display internal buffer on LED Matrixes
void ledmx_display(void);

/// Clear internal buffer
void ledmx_clear(void);
```


</details>


