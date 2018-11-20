/*
 * ssd1306.c
 *
 *  Created on: 15 Kas 2018
 *      Author: melih
 */
#include "i2c_drv.h"
#include "main.h"
/**
  ******************************************************************************
  * @file    SSD1306.c
  * @author  Waveshare Team
  * @version
  * @date    13-October-2014
  * @brief   This file includes the OLED driver for SSD1306 display moudle
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WAVESHARE SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ssd1306.h"
#include "fonts.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SSD1306_CMD    0
#define SSD1306_DAT    1

#define SSD1306_WIDTH    128
#define SSD1306_HEIGHT   64


/* Private macro -------------------------------------------------------------*/

#if !defined(SH1106) && !defined(SSD1306)
	#warning Please select first the target OLED device(SH1106 or SSD1306) in your application!
	#define SSD1306  //define SSD1306 by default
#endif

#if defined(SSD1306)
#define __SET_COL_START_ADDR() 	do { \
									ssd1306_write_byte(0x00, SSD1306_CMD); \
									ssd1306_write_byte(0x10, SSD1306_CMD); \
								} while(false)
#endif
/* Private variables ---------------------------------------------------------*/
static uint8_t s_chDispalyBuffer[128][8];


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Writes an byte to the display data ram or the command register
  *
  * @param  chData: Data to be writen to the display data ram or the command register
  * @param chCmd:
  *                           0: Writes to the command register
  *                           1: Writes to the display data ram
  * @retval None
**/
static void ssd1306_write_byte(uint8_t chData, uint8_t chCmd)
{
	unsigned char data[2];

	if(chCmd){
		data[0] = 0x40;
	}
	else{
		data[0] = 0x00;
	}
	//
	data[1] = chData;
	i2c_master_write_slave(I2C_MASTER_NUM, data, 2);

}
static void ssd1306_write_bytes(uint8_t *chData, uint8_t size)
{
	//unsigned char data[2];

	i2c_master_write_slave(I2C_MASTER_NUM, chData, size);

}
/**
  * @brief  OLED turns on
  *
  * @param  None
  *
  * @retval None
**/
void ssd1306_display_on(void)
{
	ssd1306_write_byte(0x8D, SSD1306_CMD);
	ssd1306_write_byte(0x14, SSD1306_CMD);
	ssd1306_write_byte(0xAF, SSD1306_CMD);
}

/**
  * @brief  OLED turns off
  *
  * @param  None
  *
  * @retval  None
**/
void ssd1306_display_off(void)
{
	ssd1306_write_byte(0x8D, SSD1306_CMD);
	ssd1306_write_byte(0x10, SSD1306_CMD);
	ssd1306_write_byte(0xAE, SSD1306_CMD);
}

/**
  * @brief  Refreshs the graphic ram
  *
  * @param  None
  *
  * @retval  None
**/

void ssd1306_refresh_gram(void)
{
	uint8_t i, j;

	for (i = 0; i < 8; i ++) {
		ssd1306_write_byte(0xB0 + i, SSD1306_CMD);
		__SET_COL_START_ADDR();
		for (j = 0; j < 128; j ++) {
			ssd1306_write_byte(s_chDispalyBuffer[j][i], SSD1306_DAT);
		}
	}
}

/*
 * scrool text
 * */
void ssd1306_scrol_text(uint8_t start_pg, uint8_t stop_pg, uint8_t scrol_cmd)
{
	//uint8_t cmd[10] = {0x00, SSD1306_RIGHT_HORIZONTAL_SCROLL, 0x00, 0x06, 0x07, 0x07, 0x00, 0xFF};
	//ssd1306_write_bytes(cmd, SSD1306_CMD, 8);
	//if(scrol_type == SSD1306_LEFT_TO_RIGHT)
	//ssd1306_write_byte(SSD1306_RIGHT_HORIZONTAL_SCROLL, SSD1306_CMD);
	ssd1306_write_byte(scrol_cmd, SSD1306_CMD);
	ssd1306_write_byte(0x00, SSD1306_CMD);
	ssd1306_write_byte(start_pg, SSD1306_CMD);
	ssd1306_write_byte(0x07, SSD1306_CMD);
	ssd1306_write_byte(stop_pg, SSD1306_CMD);
	ssd1306_write_byte(0x00, SSD1306_CMD);
	ssd1306_write_byte(0xFF, SSD1306_CMD);

}

//stop text scrolling
void ssd1306_scrol_text_stop()
{
	ssd1306_write_byte(SSD1306_DEACTIVATE_SCROLL, SSD1306_CMD);
}

//start text scrolling
void ssd1306_scrol_text_start()
{
	ssd1306_write_byte(SSD1306_ACTIVATE_SCROLL, SSD1306_CMD);
}


/**
  * @brief   Clears the screen
  *
  * @param  None
  *
  * @retval  None
**/

void ssd1306_clear_screen(uint8_t chFill)
{
	uint8_t i, j;

	for (i = 0; i < 8; i ++) {
		ssd1306_write_byte(0xB0 + i, SSD1306_CMD);
		__SET_COL_START_ADDR();
		for (j = 0; j < 128; j ++) {
			s_chDispalyBuffer[j][i] = chFill;
		}
	}

	ssd1306_refresh_gram();
}

/**
  * @brief  Draws a piont on the screen
  *
  * @param  chXpos: Specifies the X position
  * @param  chYpos: Specifies the Y position
  * @param  chPoint: 0: the point turns off    1: the piont turns on
  *
  * @retval None
**/

void ssd1306_draw_point(uint8_t chXpos, uint8_t chYpos, uint8_t chPoint)
{
	uint8_t chPos, chBx, chTemp = 0;

	if (chXpos > 127 || chYpos > 63) {
		return;
	}
	chPos = 7 - chYpos / 8; //
	chBx = chYpos % 8;
	chTemp = 1 << (7 - chBx);

	if (chPoint) {
		s_chDispalyBuffer[chXpos][chPos] |= chTemp;

	} else {
		s_chDispalyBuffer[chXpos][chPos] &= ~chTemp;
	}
}

/**
  * @brief  Fills a rectangle
  *
  * @param  chXpos1: Specifies the X position 1 (X top left position)
  * @param  chYpos1: Specifies the Y position 1 (Y top left position)
  * @param  chXpos2: Specifies the X position 2 (X bottom right position)
  * @param  chYpos3: Specifies the Y position 2 (Y bottom right position)
  *
  * @retval
**/

void ssd1306_fill_screen(uint8_t chXpos1, uint8_t chYpos1, uint8_t chXpos2, uint8_t chYpos2, uint8_t chDot)
{
	uint8_t chXpos, chYpos;

	for (chXpos = chXpos1; chXpos <= chXpos2; chXpos ++) {
		for (chYpos = chYpos1; chYpos <= chYpos2; chYpos ++) {
			ssd1306_draw_point(chXpos, chYpos, chDot);
		}
	}

	ssd1306_refresh_gram();
}


/**
  * @brief Displays one character at the specified position
  *
  * @param  chXpos: Specifies the X position
  * @param  chYpos: Specifies the Y position
  * @param  chSize:
  * @param  chMode
  * @retval
**/
void ssd1306_display_char(uint8_t chXpos, uint8_t chYpos, uint8_t chChr, uint8_t chSize, uint8_t chMode)
{
	uint8_t i, j;
	uint8_t chTemp, chYpos0 = chYpos;

	chChr = chChr - ' ';
    for (i = 0; i < chSize; i ++) {
		if (chSize == 12) {
			if (chMode) {
				chTemp = c_chFont1206[chChr][i];
			} else {
				chTemp = ~c_chFont1206[chChr][i];
			}
		} else {
			if (chMode) {
				chTemp = c_chFont1608[chChr][i];
			} else {
				chTemp = ~c_chFont1608[chChr][i];
			}
		}

        for (j = 0; j < 8; j ++) {
			if (chTemp & 0x80) {
				ssd1306_draw_point(chXpos, chYpos, 1);
			} else {
				ssd1306_draw_point(chXpos, chYpos, 0);
			}
			chTemp <<= 1;
			chYpos ++;

			if ((chYpos - chYpos0) == chSize) {
				chYpos = chYpos0;
				chXpos ++;
				break;
			}
		}
    }
}
static uint32_t pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;
	while(n --) result *= m;
	return result;
}


void ssd1306_display_num(uint8_t chXpos, uint8_t chYpos, uint32_t chNum, uint8_t chLen, uint8_t chSize)
{
	uint8_t i;
	uint8_t chTemp, chShow = 0;

	for(i = 0; i < chLen; i ++) {
		chTemp = (chNum / pow(10, chLen - i - 1)) % 10;
		if(chShow == 0 && i < (chLen - 1)) {
			if(chTemp == 0) {
				ssd1306_display_char(chXpos + (chSize / 2) * i, chYpos, ' ', chSize, 1);
				continue;
			} else {
				chShow = 1;
			}
		}
	 	ssd1306_display_char(chXpos + (chSize / 2) * i, chYpos, chTemp + '0', chSize, 1);
	}
}


/**
  * @brief  Displays a string on the screen
  *
  * @param  chXpos: Specifies the X position
  * @param  chYpos: Specifies the Y position
  * @param  pchString: Pointer to a string to display on the screen
  *
  * @retval  None
**/
void ssd1306_display_string(uint8_t chXpos, uint8_t chYpos, const uint8_t *pchString, uint8_t chSize, uint8_t chMode)
{
    while (*pchString != '\0') {
        if (chXpos > (SSD1306_WIDTH - chSize / 2)) {
			chXpos = 0;
			chYpos += chSize;
			if (chYpos > (SSD1306_HEIGHT - chSize)) {
				chYpos = chXpos = 0;
				ssd1306_clear_screen(0x00);
			}
		}

        ssd1306_display_char(chXpos, chYpos, *pchString, chSize, chMode);
        chXpos += chSize / 2;
        pchString ++;
    }
}

void ssd1306_draw_1616char(uint8_t chXpos, uint8_t chYpos, uint8_t chChar)
{
	uint8_t i, j;
	uint8_t chTemp = 0, chYpos0 = chYpos, chMode = 0;

	for (i = 0; i < 32; i ++) {
		chTemp = c_chFont1612[chChar - 0x30][i];
		for (j = 0; j < 8; j ++) {
			chMode = chTemp & 0x80? 1 : 0;
			ssd1306_draw_point(chXpos, chYpos, chMode);
			chTemp <<= 1;
			chYpos ++;
			if ((chYpos - chYpos0) == 16) {
				chYpos = chYpos0;
				chXpos ++;
				break;
			}
		}
	}
}

void ssd1306_draw_3216char(uint8_t chXpos, uint8_t chYpos, uint8_t chChar)
{
	uint8_t i, j;
	uint8_t chTemp = 0, chYpos0 = chYpos, chMode = 0;

	for (i = 0; i < 64; i ++) {
		chTemp = c_chFont3216[chChar - 0x30][i];
		for (j = 0; j < 8; j ++) {
			chMode = chTemp & 0x80? 1 : 0;
			ssd1306_draw_point(chXpos, chYpos, chMode);
			chTemp <<= 1;
			chYpos ++;
			if ((chYpos - chYpos0) == 32) {
				chYpos = chYpos0;
				chXpos ++;
				break;
			}
		}
	}
}

void ssd1306_draw_bitmap(uint8_t chXpos, uint8_t chYpos, const uint8_t *pchBmp, uint8_t chWidth, uint8_t chHeight)
{
	uint16_t i, j, byteWidth = (chWidth + 7) / 8;

    for(j = 0; j < chHeight; j ++){
        for(i = 0; i < chWidth; i ++ ) {
            if(*(pchBmp + j * byteWidth + i / 8) & (128 >> (i & 7))) {
                ssd1306_draw_point(chXpos + i, chYpos + j, 1);
            }
        }
    }
}



/**
  * @brief  SSd1306 initialization
  *
  * @param  None
  *
  * @retval None
**/


void ssd1306_init(void)
{

	ssd1306_reset_init();

	ssd1306_write_byte(SSD1306_DISPLAY_OFF, SSD1306_CMD);//--turn off oled panel
	ssd1306_write_byte(SSD1306_SET_LOW_COLUMN, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_SET_HIGH_COLUMN, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_SET_MEMORY_ADDR_MODE, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_SET_PAGE_START_FOR_ADDR_MODE, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_SET_COM_SCAN_NORMA_MODE, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_SET_DISPLAY_START_LINE, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_SET_CONTRAST, SSD1306_CMD);
	ssd1306_write_byte(0x5F, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_SET_SEGMENT_REMAP_127, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_DISPLAY_SET_NORMAL, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_SET_MUX_RATIO, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_HEIGHT-1, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_SET_DISPLAY_OFFSET, SSD1306_CMD);
	ssd1306_write_byte(0x00, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_SET_DISPLAY_CLK_RATIO, SSD1306_CMD);
	ssd1306_write_byte(0x80, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_SET_PRECHARGE_RATIO, SSD1306_CMD);
	ssd1306_write_byte(0xF1, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_SET_VCOM_DETECT, SSD1306_CMD);
	ssd1306_write_byte(0x20, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_SET_CHARGE_PUMP, SSD1306_CMD);
	ssd1306_write_byte(0x14, SSD1306_CMD);
	ssd1306_write_byte(SSD1306_DISPLAY_ON, SSD1306_CMD);//--turn on oled panel

	ssd1306_clear_screen(0x00);
}


void ssd1306_reset_init()
{
	gpio_set_level(GPIO_OUTPUT_IO_0, 0);
	vTaskDelay(100/portTICK_PERIOD_MS);
	gpio_set_level(GPIO_OUTPUT_IO_0, 1);
	vTaskDelay(100/portTICK_PERIOD_MS);
}

/*-------------------------------END OF FILE-------------------------------*/

