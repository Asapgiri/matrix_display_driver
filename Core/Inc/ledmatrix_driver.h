/*
 * ledmatrix_driver.h
 *
 *  Created on: Nov 4, 2023
 *      Author: Mika
 */

#ifndef INC_LEDMATRIX_DRIVER_H_
#define INC_LEDMATRIX_DRIVER_H_

#include "stm32l4xx_hal.h"

typedef unsigned int uint;

#define true	1
#define false 0

#define SEGMENT_RES	(8)

#define ENABLE 		0x0C
#define TEST 		0x0F
#define INTENSITY	0x0A
#define SCAN_LIMIT	0x0B
#define DECODE		0x09

#define SHIFT_MS_MAX 	1200
#define SHIFT_MS_MIN 	10
#define SHIFT_MS_STEP 	10

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
    uint                length;        // length on display in pixels
    uint                space;
};

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


#endif /* INC_LEDMATRIX_DRIVER_H_ */
