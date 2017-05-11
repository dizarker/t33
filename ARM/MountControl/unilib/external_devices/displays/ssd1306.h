
#ifndef _SSD1306_H_
#define _SSD1306_H_

#define SSD1306_WIDTH            128
#define SSD1306_HEIGHT           64
#define SSD1306_I2C_ADDR         0x78

//================================================================
typedef struct 
{
	uni_i2c_bitbang_t i2c;
	uint8_t i2c_adr;
	
	struct
	{
		uint8_t display_turn_fl;
		uint16_t current_x;
		uint16_t current_y;
		uint8_t inverted;
		uint8_t initialized;
		uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
	} data;
} ssd1306_t;

//================================================================
typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} ssd1306_color_t;

//================================================================
#include "include.h"

//================================================================
void ssd1306_write_command(ssd1306_t* unit, uint8_t cmd);
void ssd1306_write_data(ssd1306_t* unit, uint8_t data);
void ssd1306_set_contrast(ssd1306_t* unit, uint8_t contrast);
void ssd1306_init(ssd1306_t* unit);
void ssd1306_update_screen(ssd1306_t* unit);
void ssd1306_toggle_invert(ssd1306_t* unit);
void ssd1306_fill(ssd1306_t* unit, ssd1306_color_t color);
void ssd1306_draw_pixel(ssd1306_t* unit, uint16_t x, uint16_t y, ssd1306_color_t color);
void ssd1306_goto_xy(ssd1306_t* unit, uint16_t x, uint16_t y);
char ssd1306_putc(ssd1306_t* unit, char ch, TM_FontDef_t* Font, ssd1306_color_t color);
char ssd1306_puts(ssd1306_t* unit, char* str, TM_FontDef_t* Font, ssd1306_color_t color);
void ssd1306_put_block(ssd1306_t* unit, uint8_t* str, uint32_t size, TM_FontDef_t* Font, ssd1306_color_t color);
void ssd1306_draw_line(ssd1306_t* unit, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, ssd1306_color_t c);
void ssd1306_draw_rectangle(ssd1306_t* unit, uint16_t x, uint16_t y, uint16_t w, uint16_t h, ssd1306_color_t c);
void ssd1306_draw_filled_rectangle(ssd1306_t* unit, uint16_t x, uint16_t y, uint16_t w, uint16_t h, ssd1306_color_t c);
void ssd1306_draw_triangle(ssd1306_t* unit, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ssd1306_color_t color);
void ssd1306_draw_filled_triangle(ssd1306_t* unit, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ssd1306_color_t color);
void ssd1306_draw_circle(ssd1306_t* unit, int16_t x0, int16_t y0, int16_t r, ssd1306_color_t c);
void ssd1306_draw_filled_circle(ssd1306_t* unit, int16_t x0, int16_t y0, int16_t r, ssd1306_color_t c);
void ssd1306_on(ssd1306_t* unit);
void ssd1306_off(ssd1306_t* unit);

#endif
