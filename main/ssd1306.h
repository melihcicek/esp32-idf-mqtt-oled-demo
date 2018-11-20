/*
 * ssd1306.h
 *
 *  Created on: 15 Kas 2018
 *      Author: melih
 */

#ifndef MAIN_SSD1306_H_
#define MAIN_SSD1306_H_

//fundemantel commands
#define SSD1306_SET_CONTRAST			0x81
#define SSD1306_DISPLAY_ON_RESUME		0xA4
#define SSD1306_DISPLAY_ALL_ON			0xA5
#define SSD1306_DISPLAY_SET_NORMAL		0xA6
#define SSD1306_DISPLAY_SET_INVERSE		0xA7
#define SSD1306_DISPLAY_OFF				0xAE //sleep mode
#define SSD1306_DISPLAY_ON				0xAF // normal mode

//scrolling commands
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 				0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL  				0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL	0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  	0x2A
#define SSD1306_DEACTIVATE_SCROLL                    	0x2E
#define SSD1306_ACTIVATE_SCROLL                      	0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA             	0xA3

//adress settting commands
#define SSD1306_SET_LOW_COLUMN        	0x00 //lower nibble sets column start adress
#define SSD1306_SET_HIGH_COLUMN       	0x10 //lower nibble sets column start adress
#define SSD1306_SET_MEMORY_ADDR_MODE	0x20
#define SSD1306_SET_COLUMN_ADDR			0x21
#define SSD1306_SET_PAGE_START_ADDR		0x22
#define SSD1306_SET_PAGE_START_FOR_ADDR_MODE	0xB0

//hardware config commands
#define SSD1306_SET_DISPLAY_START_LINE	0x40
#define SSD1306_SET_SEGMENT_REMAP_0		0xA0
#define SSD1306_SET_SEGMENT_REMAP_127	0xA1
#define SSD1306_SET_MUX_RATIO			0xA8
#define SSD1306_SET_COM_SCAN_NORMA_MODE	0xC0
#define SSD1306_SET_COM_SCAN_REMAP_MODE	0xC8
#define SSD1306_SET_DISPLAY_OFFSET		0xD3
#define SSD1306_SET_COM_PIN_HW_CFG		0xDA
#define SSD1306_SET_DISPLAY_CLK_RATIO	0xD5
#define SSD1306_SET_PRECHARGE_RATIO		0xD9
#define SSD1306_SET_VCOM_DETECT			0xDB

//advance graphic commands
#define SSD1306_SET_FADEOUT_AND_BLINK	0x23
#define SSD1306_SET_ZOOM_IN				0xD6

//charge pump command
#define SSD1306_SET_CHARGE_PUMP			0x8D


extern void ssd1306_clear_screen(uint8_t chFill);
extern void ssd1306_refresh_gram(void);
extern void ssd1306_draw_point(uint8_t chXpos, uint8_t chYpos, uint8_t chPoint);
extern void ssd1306_fill_screen(uint8_t chXpos1, uint8_t chYpos1, uint8_t chXpos2, uint8_t chYpos2, uint8_t chDot);
extern void ssd1306_display_char(uint8_t chXpos, uint8_t chYpos, uint8_t chChr, uint8_t chSize, uint8_t chMode);
extern void ssd1306_display_num(uint8_t chXpos, uint8_t chYpos, uint32_t chNum, uint8_t chLen,uint8_t chSize);
extern void ssd1306_display_string(uint8_t chXpos, uint8_t chYpos, const uint8_t *pchString, uint8_t chSize, uint8_t chMode);
extern void ssd1306_draw_1616char(uint8_t chXpos, uint8_t chYpos, uint8_t chChar);
extern void ssd1306_draw_3216char(uint8_t chXpos, uint8_t chYpos, uint8_t chChar);
extern void ssd1306_draw_bitmap(uint8_t chXpos, uint8_t chYpos, const uint8_t *pchBmp, uint8_t chWidth, uint8_t chHeight);
extern void ssd1306_init(void);
extern void ssd1306_reset_init(void);
extern void ssd1306_scrol_text(uint8_t start_pg, uint8_t stop_pg, uint8_t scrol_cmd);
extern void ssd1306_scrol_text_stop(void);
extern void ssd1306_scrol_text_start(void);

#endif /* MAIN_SSD1306_H_ */
