#include "ledmatrix_driver.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

static const ASCII_TABLE_8X8[][8] = {
    { 0 },   // U+0000 (null)
    { 0 },   // U+0001
    { 0 },   // U+0002
    { 0 },   // U+0003
    { 0 },   // U+0004
    { 0 },   // U+0005
    { 0 },   // U+0006
    { 0 },   // U+0007
    { 0 },   // U+0008
    { 0 },   // U+0009
    { 0 },   // U+000A
    { 0 },   // U+000B
    { 0 },   // U+000C
    { 0 },   // U+000D
    { 0 },   // U+000E
    { 0 },   // U+000F
    { 0 },   // U+0010
    { 0 },   // U+0011
    { 0 },   // U+0012
    { 0 },   // U+0013
    { 0 },   // U+0014
    { 0 },   // U+0015
    { 0 },   // U+0016
    { 0 },   // U+0017
    { 0 },   // U+0018
    { 0 },   // U+0019
    { 0 },   // U+001A
    { 0 },   // U+001B
    { 0 },   // U+001C
    { 0 },   // U+001D
    { 0 },   // U+001E
    { 0 },   // U+001F
    { 0 },   // U+0020 (space)
    { 0b00110000, 0b01111000, 0b01111000, 0b00110000, 0b00110000, 0b00000000, 0b00110000, 0b00000000 },   // U+0021 (!)
    { 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0022 (")
    { 0b01101100, 0b01101100, 0b11111110, 0b01101100, 0b11111110, 0b01101100, 0b01101100, 0b00000000 },   // U+0023 (#)
    { 0b00110000, 0b01111100, 0b11000000, 0b01111000, 0b00001100, 0b11111000, 0b00110000, 0b00000000 },   // U+0024 ($)
    { 0b00000000, 0b11000110, 0b11001100, 0b00011000, 0b00110000, 0b01100110, 0b11000110, 0b00000000 },   // U+0025 (%)
    { 0b00111000, 0b01101100, 0b00111000, 0b01110110, 0b11011100, 0b11001100, 0b01110110, 0b00000000 },   // U+0026 (&)
    { 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0027 (')
    { 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00 },   // U+0028 (()
    { 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00 },   // U+0029 ())
    { 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00 },   // U+002A (*)
    { 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00 },   // U+002B (+)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06 },   // U+002C (,)
    { 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00 },   // U+002D (-)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00 },   // U+002E (.)
    { 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00 },   // U+002F (/)
    { 0b01111000, 0b11001100, 0b11011100, 0b11111100, 0b11101100, 0b11001100, 0b01111100, 0b00000000 },   // U+0030 (0)
    { 0b00110000, 0b01110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b11111100, 0b00000000 },   // U+0031 (1)
    { 0b01111000, 0b11001100, 0b00001100, 0b00111000, 0b01100000, 0b11001100, 0b11111100, 0b00000000 },   // U+0032 (2)
    { 0b01111000, 0b11001100, 0b00001100, 0b00111000, 0b00001100, 0b11001100, 0b01111000, 0b00000000 },   // U+0033 (3)
    { 0b00011100, 0b00111100, 0b01101100, 0b11001100, 0b11111110, 0b00001100, 0b00011110, 0b00000000 },   // U+0034 (4)
    { 0b11111100, 0b11000000, 0b11111000, 0b00001100, 0b00001100, 0b11001100, 0b01111000, 0b00000000 },   // U+0035 (5)
    { 0b00111000, 0b01100000, 0b11000000, 0b11111000, 0b11001100, 0b11001100, 0b01111000, 0b00000000 },   // U+0036 (6)
    { 0b11111100, 0b11001100, 0b00001100, 0b00011000, 0b00110000, 0b00110000, 0b00110000, 0b00000000 },   // U+0037 (7)
    { 0b01111000, 0b11001100, 0b11001100, 0b01111000, 0b11001100, 0b11001100, 0b01111000, 0b00000000 },   // U+0038 (8)
    { 0b01111000, 0b11001100, 0b11001100, 0b01111100, 0b00001100, 0b00011000, 0b01110000, 0b00000000 },   // U+0039 (9)
    { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00 },   // U+003A (:)
    { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06 },   // U+003B (//)
    { 0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00 },   // U+003C (<)
    { 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00 },   // U+003D (=)
    { 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00 },   // U+003E (>)
    { 0b01111000, 0b11001100, 0b00001100, 0b00011000, 0b00110000, 0b00000000, 0b00110000, 0b00000000 },   // U+003F (?)
    { 0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00 },   // U+0040 (@)
    { 0b00110000, 0b01111000, 0b11001100, 0b11001100, 0b11111100, 0b11001100, 0b11001100, 0b00000000 },   // U+0041 (A)
    { 0b11111100, 0b01100110, 0b01100110, 0b01111100, 0b01100110, 0b01100110, 0b11111100, 0b00000000 },   // U+0042 (B)
    { 0b00111100, 0b01100110, 0b11000000, 0b11000000, 0b11000000, 0b01100110, 0b00111100, 0b00000000 },   // U+0043 (C)
    { 0b11111000, 0b01101100, 0b01100110, 0b01100110, 0b01100110, 0b01101100, 0b11111000, 0b00000000 },   // U+0044 (D)
    { 0b11111110, 0b01100010, 0b01101000, 0b01111000, 0b01101000, 0b01100010, 0b11111110, 0b00000000 },   // U+0045 (E)
    { 0b11111110, 0b01100010, 0b01101000, 0b01111000, 0b01101000, 0b01100000, 0b11110000, 0b00000000 },   // U+0046 (F)
    { 0b00111100, 0b01100110, 0b11000000, 0b11000000, 0b11001110, 0b01100110, 0b00111110, 0b00000000 },   // U+0047 (G)
    { 0b11001100, 0b11001100, 0b11001100, 0b11111100, 0b11001100, 0b11001100, 0b11001100, 0b00000000 },   // U+0048 (H)
    { 0b01111000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b01111000, 0b00000000 },   // U+0049 (I)
    { 0b00011110, 0b00001100, 0b00001100, 0b00001100, 0b11001100, 0b11001100, 0b01111000, 0b00000000 },   // U+004A (J)
    { 0b11110110, 0b01100110, 0b01101100, 0b01111000, 0b01101100, 0b01100110, 0b11110110, 0b00000000 },   // U+004B (K)
    { 0b11110000, 0b01100000, 0b01100000, 0b01100000, 0b01100010, 0b01100110, 0b11111110, 0b00000000 },   // U+004C (L)
    { 0b11000110, 0b11101110, 0b11111110, 0b11111110, 0b11010110, 0b11000110, 0b11000110, 0b00000000 },   // U+004D (M)
    { 0b11000110, 0b11100110, 0b11110110, 0b11011110, 0b11001110, 0b11000110, 0b11000110, 0b00000000 },   // U+004E (N)
    { 0b00111000, 0b01101100, 0b11000110, 0b11000110, 0b11000110, 0b01101100, 0b00111000, 0b00000000 },   // U+004F (O)
    { 0b11111100, 0b01100110, 0b01100110, 0b01111100, 0b01100000, 0b01100000, 0b11110000, 0b00000000 },   // U+0050 (P)
    { 0b01111000, 0b11001100, 0b11001100, 0b11001100, 0b11011100, 0b01111000, 0b00011100, 0b00000000 },   // U+0051 (Q)
    { 0b11111100, 0b01100110, 0b01100110, 0b01111100, 0b01101100, 0b01100110, 0b11110110, 0b00000000 },   // U+0052 (R)
    { 0b01111000, 0b11001100, 0b11100000, 0b01110000, 0b00011100, 0b11001100, 0b01111000, 0b00000000 },   // U+0053 (S)
    { 0b11111100, 0b10110100, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b01111000, 0b00000000 },   // U+0054 (T)
    { 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11111100, 0b00000000 },   // U+0055 (U)
    { 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b01111000, 0b00110000, 0b00000000 },   // U+0056 (V)
    { 0b11000110, 0b11000110, 0b11000110, 0b11010110, 0b11111110, 0b11101110, 0b11000110, 0b00000000 },   // U+0057 (W)
    { 0b11000110, 0b11000110, 0b01101100, 0b00111000, 0b00111000, 0b01101100, 0b11000110, 0b00000000 },   // U+0058 (X)
    { 0b11001100, 0b11001100, 0b11001100, 0b01111000, 0b00110000, 0b00110000, 0b01111000, 0b00000000 },   // U+0059 (Y)
    { 0b11111110, 0b11000110, 0b10001100, 0b00011000, 0b00110010, 0b01100110, 0b11111110, 0b00000000 },   // U+005A (Z)
    { 0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00 },   // U+005B ([)
    { 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00 },   // U+005C (\)
    { 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00 },   // U+005D (])
    { 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00 },   // U+005E (^)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF },   // U+005F (_)
    { 0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0060 (`)
    { 0b00000000, 0b00000000, 0b01111000, 0b00001100, 0b01111100, 0b11001100, 0b01110110, 0b00000000 },   // U+0061 (a)
    { 0b11100000, 0b01100000, 0b01100000, 0b01111100, 0b01100110, 0b01100110, 0b11011100, 0b00000000 },   // U+0062 (b)
    { 0b00000000, 0b00000000, 0b01111000, 0b11001100, 0b11000000, 0b11001100, 0b01111000, 0b00000000 },   // U+0063 (c)
    { 0b00011100, 0b00001100, 0b00001100, 0b01111100, 0b11001100, 0b11001100, 0b01110110, 0b00000000 },   // U+0064 (d)
    { 0b00000000, 0b00000000, 0b01111000, 0b11001100, 0b11111100, 0b11000000, 0b01111000, 0b00000000 },   // U+0065 (e)
    { 0b00111000, 0b01101100, 0b01100000, 0b11110000, 0b01100000, 0b01100000, 0b11110000, 0b00000000 },   // U+0066 (f)
    { 0b00000000, 0b00000000, 0b01110110, 0b11001100, 0b11001100, 0b01111100, 0b00001100, 0b11111000 },   // U+0067 (g)
    { 0b11100000, 0b01100000, 0b01101100, 0b01110110, 0b01100110, 0b01100110, 0b11100110, 0b00000000 },   // U+0068 (h)
    { 0b00110000, 0b00000000, 0b01110000, 0b00110000, 0b00110000, 0b00110000, 0b01111000, 0b00000000 },   // U+0069 (i)
    { 0b00001100, 0b00000000, 0b00001100, 0b00001100, 0b00001100, 0b11001100, 0b11001100, 0b01111000 },   // U+006A (j)
    { 0b11100000, 0b01100000, 0b01100110, 0b01101100, 0b01111000, 0b01101100, 0b11100110, 0b00000000 },   // U+006B (k)
    { 0b01110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b00110000, 0b01111000, 0b00000000 },   // U+006C (l)
    { 0b00000000, 0b00000000, 0b11001100, 0b11111110, 0b11111110, 0b11010110, 0b11000110, 0b00000000 },   // U+006D (m)
    { 0b00000000, 0b00000000, 0b11111000, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b00000000 },   // U+006E (n)
    { 0b00000000, 0b00000000, 0b01111000, 0b11001100, 0b11001100, 0b11001100, 0b01111000, 0b00000000 },   // U+006F (o)
    { 0b00000000, 0b00000000, 0b11011100, 0b01100110, 0b01100110, 0b01111100, 0b01100000, 0b11110000 },   // U+0070 (p)
    { 0b00000000, 0b00000000, 0b01110110, 0b11001100, 0b11001100, 0b01111100, 0b00001100, 0b00011110 },   // U+0071 (q)
    { 0b00000000, 0b00000000, 0b10011100, 0b01110110, 0b01100110, 0b01100000, 0b11110000, 0b00000000 },   // U+0072 (r)
    { 0b00000000, 0b00000000, 0b01111100, 0b11000000, 0b01111000, 0b00001100, 0b11111000, 0b00000000 },   // U+0073 (s)
    { 0b00010000, 0b00110000, 0b01111100, 0b00110000, 0b00110000, 0b00110100, 0b00011000, 0b00000000 },   // U+0074 (t)
    { 0b00000000, 0b00000000, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b01110110, 0b00000000 },   // U+0075 (u)
    { 0b00000000, 0b00000000, 0b11001100, 0b11001100, 0b11001100, 0b01111000, 0b00110000, 0b00000000 },   // U+0076 (v)
    { 0b00000000, 0b00000000, 0b11000110, 0b11000110, 0b11010110, 0b11111110, 0b01101100, 0b00000000 },   // U+0077 (w)
    { 0b00000000, 0b00000000, 0b11000110, 0b01101100, 0b00111000, 0b01101100, 0b11000110, 0b00000000 },   // U+0078 (x)
    { 0b00000000, 0b00000000, 0b11001100, 0b11001100, 0b11001100, 0b01111100, 0b00001100, 0b11111000 },   // U+0079 (y)
    { 0b00000000, 0b00000000, 0b11111100, 0b10011000, 0b00110000, 0b01100100, 0b11111100, 0b00000000 },   // U+007A (z)
    { 0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00 },   // U+007B ({)
    { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00 },   // U+007C (|)
    { 0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00 },   // U+007D (})
    { 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+007E (~)
    { 0 }    // U+007F
};

static SPI_HandleTypeDef* hspi;
static uint segments;
static uint seg_width;
static uint seg_height;
static char odd_invert;

static uint8_t* PIXEL_ARRAY;

struct shift_text shifter = { 0 };

#define spi_select()	HAL_GPIO_WritePin(LEDMX_CS_GPIO_Port, LEDMX_CS_Pin, GPIO_PIN_RESET)
#define spi_deselect()	HAL_GPIO_WritePin(LEDMX_CS_GPIO_Port, LEDMX_CS_Pin, GPIO_PIN_SET);

void send_command(uint8_t cmd, uint8_t data) {
	uint8_t spi_data[2];

	spi_data[0] = cmd;
    spi_data[1] = data;

    spi_select();
    HAL_SPI_Transmit(hspi, spi_data, 2, 1000);
    spi_deselect();
}

void send_data(uint addr, uint8_t row, uint8_t data) {
    uint i;
    uint8_t spi_data[2];

    spi_select();
    for (i = 0; i < segments; i++) {
    	if (addr == i) {
			spi_data[0] = row;
			spi_data[1] = data;
    	}
    	else {
    		spi_data[0] = 0;
			spi_data[1] = 0;
    	}
        HAL_SPI_Transmit(hspi, spi_data, 2, 1000);
    }
    spi_deselect();
}

void send_data_rows(uint8_t row, uint8_t* data) {
    uint i;
    uint8_t spi_data[2];

    spi_select();
    for (i = 0; i < segments; i++) {
		spi_data[0] = row;
		spi_data[1] = data[i];
        HAL_SPI_Transmit(hspi, spi_data, 2, 1000);
    }
    spi_deselect();
}

void ledmx_init(SPI_HandleTypeDef* _hspi, uint s_width, uint s_height, char _odd_inverted) {
	uint i;

	hspi = _hspi;
	segments = s_width * s_height;
	seg_width = s_width;
	seg_height = s_height;
	odd_invert = _odd_inverted;

	PIXEL_ARRAY = malloc(segments * SEGMENT_RES);

	send_command(TEST, 0);
	send_command(DECODE, 0);
	send_command(SCAN_LIMIT, 7);
	send_command(INTENSITY, 4);
	send_command(ENABLE, 1);
    for (i = 0; i < segments; i++) {
        send_command(0, 0);
    }
}

void ledmx_text(const char* text) {
    char* c = text;
    uint seg, row;

    memset(PIXEL_ARRAY, 0, segments * SEGMENT_RES);
    for (seg = 0; *c != '\0' && seg < segments; c++, seg++) {
        for (row = 0; row < SEGMENT_RES; row++) {
            PIXEL_ARRAY[row * segments + seg] = ASCII_TABLE_8X8[*c][row];
        }
    }
}

void ledmx_setup_shift_text(const char* text, uint space, enum ledmx_text_dir direction, uint ms) {
    uint len, i;

    len = strlen(text);

    shifter.direction	= direction;
    shifter.position    = 0;
    shifter.length      = (len + space) * SEGMENT_RES;
    shifter.space       = space;

    if (shifter.text) {
        free(shifter.text);
    }
    shifter.text = malloc(len + space + 1);
    strcpy(shifter.text, text);
    for (i = 0; i < space; i++) {
        shifter.text[len + i] = ' ';
    }
    shifter.text[len + space] = '\0';

    shifter.ms        = ms;
    shifter.ms_change = HAL_GetTick() + ms;
    shifter.ms_i_min  = ms < SHIFT_MS_MIN ? ms : SHIFT_MS_MIN;
    shifter.ms_i_max  = ms > SHIFT_MS_MAX ? ms : SHIFT_MS_MAX;

    ledmx_text(text);
}

static void text_redraw(void) {
    uint8_t shift_origin, shift_next;
    char* c;
    uint seg, row;

    shift_origin = shifter.position % SEGMENT_RES;
    shift_next = 8 - shift_origin;
    c = shifter.text + (shifter.position / SEGMENT_RES);

    seg = 0;
    while (seg < segments) {
        if (seg > 0) {
            for (row = 0; row < SEGMENT_RES; row++) {
            	PIXEL_ARRAY[row * segments + seg - 1] = (ASCII_TABLE_8X8[c[0]][row] >> shift_next);
            }
        }

        for (; c[1] != '\0' && seg < segments; c++, seg++) {
            for (row = 0; row < SEGMENT_RES; row++) {
                PIXEL_ARRAY[row * segments + seg] =
                    (ASCII_TABLE_8X8[c[0]][row] << shift_origin) |
                    (ASCII_TABLE_8X8[c[1]][row] >> shift_next);
            }
        }

        if (seg < segments) {
        	for (row = 0; row < SEGMENT_RES; row++) {
        		PIXEL_ARRAY[row * segments + seg] = (ASCII_TABLE_8X8[*c][row] << shift_origin);
        	}
    		seg++;

			if ('\0' == c[1] && seg >= segments) {
				for (row = 0; row < SEGMENT_RES; row++) {
					PIXEL_ARRAY[row * segments + segments - 1] = (ASCII_TABLE_8X8[*shifter.text][row] >> shift_next);
				}
			}
        }
		c = shifter.text;
    }
}

void ledmx_shift_text(void) {
    uint64_t ms_now;

    ms_now = HAL_GetTick();
    if (ms_now < shifter.ms_change) {
        return;
    }
    shifter.ms_change += shifter.ms;

    if (left == shifter.direction) {
    	shifter.position++;
    }
    else {
    	shifter.position--;
    }


    if (shifter.position >= shifter.length) {
        shifter.position = left == shifter.direction ? 0 : shifter.length-1;
    }


    text_redraw();
}

void ledmx_set_text_dir(enum ledmx_text_dir direction) {
	shifter.direction = direction;
}

void ledmx_text_speed_up(void) {
	if (shifter.ms <= (shifter.ms_i_min + SHIFT_MS_STEP)) {
		shifter.ms = shifter.ms_i_min;
	}
	else {
		shifter.ms -= SHIFT_MS_STEP;
	}
}

void ledmx_text_slow_down(void) {
	if (shifter.ms >= (shifter.ms_i_max - SHIFT_MS_STEP)) {
		shifter.ms = shifter.ms_i_max;
	}
	else {
		shifter.ms += SHIFT_MS_STEP;
	}
}

void ledmx_display(void) {
	uint row, seg;

	//for (seg = 0; seg < segments; seg++) {
	//	for (row = 0; row < SEGMENT_RES; row++) {
    //        send_data(seg, row + 1, PIXEL_ARRAY[row * segments + seg]);
    //    }
	//}

	for (row = 0; row < SEGMENT_RES; row++) {
		send_data_rows(row+1, PIXEL_ARRAY + (row * segments));
	}
}

void ledmx_clear(void) {
	uint i, j;

	memset(PIXEL_ARRAY, 0, segments * SEGMENT_RES);
	ledmx_display();
}


