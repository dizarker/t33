#include "include.h"

//================================================================
inline void ssd1306_write_command(ssd1306_t* unit, uint8_t cmd)
{
  uni_i2c_start(&unit->i2c);
	uni_i2c_write_byte(&unit->i2c, unit->i2c_adr);
	uni_i2c_write_byte(&unit->i2c, 0x00);
	uni_i2c_write_byte(&unit->i2c, cmd);
	uni_i2c_stop(&unit->i2c);
}

//================================================================
inline void ssd1306_write_data(ssd1306_t* unit, uint8_t data)
{
	uni_i2c_start(&unit->i2c);
	uni_i2c_write_byte(&unit->i2c, unit->i2c_adr);
	uni_i2c_write_byte(&unit->i2c, 0x40);
	uni_i2c_write_byte(&unit->i2c, data);
	uni_i2c_stop(&unit->i2c);
}

//================================================================
inline void ssd1306_set_contrast(ssd1306_t* unit, uint8_t contrast)
{
	uni_i2c_start(&unit->i2c);
	uni_i2c_write_byte(&unit->i2c, unit->i2c_adr);
	uni_i2c_write_byte(&unit->i2c, 0x00);
	uni_i2c_write_byte(&unit->i2c, 0x81);
	uni_i2c_write_byte(&unit->i2c, contrast);
	uni_i2c_stop(&unit->i2c);
}

//================================================================
void ssd1306_init(ssd1306_t* unit)
{
	if (!unit->i2c_adr)
	{
		unit->i2c_adr = SSD1306_I2C_ADDR;
	}
	
	uni_i2c_init(&unit->i2c);
		
	/* Init LCD */
	ssd1306_write_command(unit, 0xAE); //display off
	ssd1306_write_command(unit, 0x20); //Set Memory Addressing Mode   
	ssd1306_write_command(unit, 0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	ssd1306_write_command(unit, 0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	ssd1306_write_command(unit, 0xC8); //Set COM Output Scan Direction
	ssd1306_write_command(unit, 0x00); //---set low column address
	ssd1306_write_command(unit, 0x10); //---set high column address
	ssd1306_write_command(unit, 0x40); //--set start line address
	ssd1306_write_command(unit, 0x81); //--set contrast control register
	ssd1306_write_command(unit, 0xFF);
	ssd1306_write_command(unit, 0xA1); //--set segment re-map 0 to 127
	ssd1306_write_command(unit, 0xA6); //--set normal display
	ssd1306_write_command(unit, 0xA8); //--set multiplex ratio(1 to 64)
	ssd1306_write_command(unit, 0x3F); //
	ssd1306_write_command(unit, 0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	ssd1306_write_command(unit, 0xD3); //-set display offset
	ssd1306_write_command(unit, 0x00); //-not offset
	ssd1306_write_command(unit, 0xD5); //--set display clock divide ratio/oscillator frequency
	ssd1306_write_command(unit, 0xF0); //--set divide ratio
	ssd1306_write_command(unit, 0xD9); //--set pre-charge period
	ssd1306_write_command(unit, 0x22); //
	ssd1306_write_command(unit, 0xDA); //--set com pins hardware configuration
	ssd1306_write_command(unit, 0x12);
	ssd1306_write_command(unit, 0xDB); //--set vcomh
	ssd1306_write_command(unit, 0x20); //0x20,0.77xVcc
	ssd1306_write_command(unit, 0x8D); //--set DC-DC enable
	ssd1306_write_command(unit, 0x14); //
	ssd1306_write_command(unit, 0xAF); //--turn on SSD1306 panel
	
	ssd1306_fill(unit, SSD1306_COLOR_BLACK);
	
	ssd1306_update_screen(unit);
	
	unit->data.current_x = 0;
	unit->data.current_y = 0;
	unit->data.initialized = 1;
}

//================================================================
void ssd1306_update_screen(ssd1306_t* unit) 
{
	uint8_t m, n;
	
	for (m = 0; m < 8; m++) 
	{
		ssd1306_write_command(unit, 0xB0 + m);
		ssd1306_write_command(unit, 0x00);
		ssd1306_write_command(unit, 0x10);
		
		for (n = 0; n < SSD1306_WIDTH; n++)
		{
			ssd1306_write_data(unit, unit->data.buffer[(SSD1306_WIDTH * m) + n]);
		}			
	}
}

//================================================================
void ssd1306_toggle_invert(ssd1306_t* unit) 
{
	uint16_t i;
	
	unit->data.inverted = !unit->data.inverted;
	
	for (i = 0; i < sizeof(unit->data.buffer); i++) 
	{
		unit->data.buffer[i] = ~unit->data.buffer[i];
	}
}

//================================================================
void ssd1306_fill(ssd1306_t* unit, ssd1306_color_t color)
{
	memset(unit->data.buffer, (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(unit->data.buffer));
}

//================================================================
void ssd1306_draw_pixel_real(ssd1306_t* unit, uint16_t x, uint16_t y, ssd1306_color_t color)
{
	if (x >= SSD1306_WIDTH ||	y >= SSD1306_HEIGHT)
	{
		return;
	}
	
	if (unit->data.inverted) {
		color = (ssd1306_color_t)!color;
	}
	
	if (color == SSD1306_COLOR_WHITE) {
		unit->data.buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} else {
		unit->data.buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

//================================================================
void ssd1306_draw_pixel(ssd1306_t* unit, uint16_t x, uint16_t y, ssd1306_color_t color)
{
//SSD1306_WIDTH
//SSD1306_HEIGHT
	if (!unit->data.display_turn_fl)
	{
		ssd1306_draw_pixel_real(unit, x, y, color);
	}
	else
	{
		ssd1306_draw_pixel_real(unit, SSD1306_WIDTH - y - 1, x, color);
	}
}

//================================================================
void ssd1306_goto_xy(ssd1306_t* unit, uint16_t x, uint16_t y) 
{
	unit->data.current_x = x;
	unit->data.current_y = y;
}

//================================================================
char ssd1306_putc(ssd1306_t* unit, char ch, TM_FontDef_t* Font, ssd1306_color_t color) 
{
	uint32_t i, b, j;
	
//	/* Check available space in LCD */
//	if (
//		SSD1306_WIDTH <= (unit->data.current_x + Font->FontWidth) ||
//		SSD1306_HEIGHT <= (unit->data.current_y + Font->FontHeight)
//	) {
//		/* Error */
//		return 0;
//	}
	
	/* Go through font */
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				ssd1306_draw_pixel(unit, unit->data.current_x + j, (unit->data.current_y + i), (ssd1306_color_t) color);
			} else {
				ssd1306_draw_pixel(unit, unit->data.current_x + j, (unit->data.current_y + i), (ssd1306_color_t)!color);
			}
		}
	}
	
	/* Increase pointer */
	unit->data.current_x += Font->FontWidth;
	
	/* Return character written */
	return ch;
}

//================================================================
char ssd1306_puts(ssd1306_t* unit, char* str, TM_FontDef_t* Font, ssd1306_color_t color) 
{
	/* Write characters */
	while (*str) {
		/* Write character by character */
		if (ssd1306_putc(unit, *str, Font, color) != *str) {
			/* Return error */
			return *str;
		}
		
		/* Increase string pointer */
		str++;
	}
	
	/* Everything OK, zero should be returned */
	return *str;
}

//================================================================
void ssd1306_put_block(ssd1306_t* unit, uint8_t* str, uint32_t size, TM_FontDef_t* Font, ssd1306_color_t color)
{
	/* Write characters */
	while (size--) {
		/* Write character by character */
		if (ssd1306_putc(unit, *str, Font, color) != *str) {
			/* Return error */
			return;
		}
		
		/* Increase string pointer */
		str++;
	}
}
 
//================================================================
void ssd1306_draw_line(ssd1306_t* unit, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, ssd1306_color_t c) 
{
	int16_t dx, dy, sx, sy, err, e2, i, tmp; 
	
	/* Check for overflow */
	if (x0 >= SSD1306_WIDTH) {
		x0 = SSD1306_WIDTH - 1;
	}
	if (x1 >= SSD1306_WIDTH) {
		x1 = SSD1306_WIDTH - 1;
	}
	if (y0 >= SSD1306_HEIGHT) {
		y0 = SSD1306_HEIGHT - 1;
	}
	if (y1 >= SSD1306_HEIGHT) {
		y1 = SSD1306_HEIGHT - 1;
	}
	
	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); 
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
	sx = (x0 < x1) ? 1 : -1; 
	sy = (y0 < y1) ? 1 : -1; 
	err = ((dx > dy) ? dx : -dy) / 2; 

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			ssd1306_draw_pixel(unit, x0, i, c);
		}
		
		/* Return from function */
		return;
	}
	
	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			ssd1306_draw_pixel(unit, i, y0, c);
		}
		
		/* Return from function */
		return;
	}
	
	while (1) {
		ssd1306_draw_pixel(unit, x0, y0, c); 
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err; 
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		} 
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		} 
	}
}

//================================================================
void ssd1306_draw_rectangle(ssd1306_t* unit, uint16_t x, uint16_t y, uint16_t w, uint16_t h, ssd1306_color_t c)
{
//	/* Check input parameters */
//	if (
//		x >= SSD1306_WIDTH ||
//		y >= SSD1306_HEIGHT
//	) {
//		/* Return error */
//		return;
//	}
	
	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}
	
	/* Draw 4 lines */
	ssd1306_draw_line(unit, x, y, x + w, y, c);         /* Top line */
	ssd1306_draw_line(unit, x, y + h, x + w, y + h, c); /* Bottom line */
	ssd1306_draw_line(unit, x, y, x, y + h, c);         /* Left line */
	ssd1306_draw_line(unit, x + w, y, x + w, y + h, c); /* Right line */
}

//================================================================
void ssd1306_draw_filled_rectangle(ssd1306_t* unit, uint16_t x, uint16_t y, uint16_t w, uint16_t h, ssd1306_color_t c)
{
	uint8_t i;
	
//	/* Check input parameters */
//	if (
//		x >= SSD1306_WIDTH ||
//		y >= SSD1306_HEIGHT
//	) {
//		/* Return error */
//		return;
//	}
	
	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}
	
	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		ssd1306_draw_line(unit, x, y + i, x + w, y + i, c);
	}
}

//================================================================
void ssd1306_draw_triangle(ssd1306_t* unit, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ssd1306_color_t color)
{
	/* Draw lines */
	ssd1306_draw_line(unit, x1, y1, x2, y2, color);
	ssd1306_draw_line(unit, x2, y2, x3, y3, color);
	ssd1306_draw_line(unit, x3, y3, x1, y1, color);
}

//================================================================
void ssd1306_draw_filled_triangle(ssd1306_t* unit, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ssd1306_color_t color)
{
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
	curpixel = 0;
	
	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		ssd1306_draw_line(unit, x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

//================================================================
void ssd1306_draw_circle(ssd1306_t* unit, int16_t x0, int16_t y0, int16_t r, ssd1306_color_t c)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    ssd1306_draw_pixel(unit, x0, y0 + r, c);
    ssd1306_draw_pixel(unit, x0, y0 - r, c);
    ssd1306_draw_pixel(unit, x0 + r, y0, c);
    ssd1306_draw_pixel(unit, x0 - r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ssd1306_draw_pixel(unit, x0 + x, y0 + y, c);
        ssd1306_draw_pixel(unit, x0 - x, y0 + y, c);
        ssd1306_draw_pixel(unit, x0 + x, y0 - y, c);
        ssd1306_draw_pixel(unit, x0 - x, y0 - y, c);

        ssd1306_draw_pixel(unit, x0 + y, y0 + x, c);
        ssd1306_draw_pixel(unit, x0 - y, y0 + x, c);
        ssd1306_draw_pixel(unit, x0 + y, y0 - x, c);
        ssd1306_draw_pixel(unit, x0 - y, y0 - x, c);
    }
}

//================================================================
void ssd1306_draw_filled_circle(ssd1306_t* unit, int16_t x0, int16_t y0, int16_t r, ssd1306_color_t c)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    ssd1306_draw_pixel(unit, x0, y0 + r, c);
    ssd1306_draw_pixel(unit, x0, y0 - r, c);
    ssd1306_draw_pixel(unit, x0 + r, y0, c);
    ssd1306_draw_pixel(unit, x0 - r, y0, c);
    ssd1306_draw_line(unit, x0 - r, y0, x0 + r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ssd1306_draw_line(unit, x0 - x, y0 + y, x0 + x, y0 + y, c);
        ssd1306_draw_line(unit, x0 + x, y0 - y, x0 - x, y0 - y, c);

        ssd1306_draw_line(unit, x0 + y, y0 + x, x0 - y, y0 + x, c);
        ssd1306_draw_line(unit, x0 + y, y0 - x, x0 - y, y0 - x, c);
    }
}

//================================================================
void ssd1306_on(ssd1306_t* unit)
{
	ssd1306_write_command(unit, 0x8D);  
	ssd1306_write_command(unit, 0x14);  
	ssd1306_write_command(unit, 0xAF);  
}

//================================================================
void ssd1306_off(ssd1306_t* unit)
{
	ssd1306_write_command(unit, 0x8D);  
	ssd1306_write_command(unit, 0x10);
	ssd1306_write_command(unit, 0xAE);  
}

