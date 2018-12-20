/*
lcd.h
*/

#ifndef _LCD_H
#define _LCD_H

#include "stm32l4xx_hal.h"
#include "stm32l476g_discovery.h"

enum {
	TRANSFER_WAIT,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
};

/* transfer state */
extern __IO uint32_t wTransferState;



//ENABLE PIN (включить управление экраном = 0)
#define EN_PIN                          GPIO_PIN_1
#define EN_GPIO_PORT                    GPIOA
#define EN_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define EN_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()

//RS PIN (D/C pin)
#define RS_PIN                          GPIO_PIN_10
#define RS_GPIO_PORT                    GPIOE
#define RS_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOE_CLK_ENABLE()
#define RS_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOE_CLK_DISABLE()

//CNTRL PIN (turn on power supply = 1, VDD VCC)
#define CNTRL_PIN                          GPIO_PIN_11
#define CNTRL_GPIO_PORT                    GPIOE
#define CNTRL_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOE_CLK_ENABLE()
#define CNTRL_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOE_CLK_DISABLE()

//RESET PIN
#define RESET_PIN                          GPIO_PIN_3
#define RESET_GPIO_PORT                    GPIOA
#define RESET_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define RESET_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOA_CLK_DISABLE()


#define LCD_WIDTH	128

#define LCD_SHEIGHT	32
#define LCD_BHEIGHT	64

#define LCD_PIXPERBYTE	8
/* Size of buffer */

extern uint32_t LCDBUFSIZE;

//command buffer size
#define BUFFERSIZE  50


extern uint8_t lcdbuf[LCD_WIDTH*LCD_BHEIGHT/LCD_PIXPERBYTE];

extern uint8_t lcd_height;


void lcd_clear();
void lcd_off(void);
void lcd_on(void);
void lcd_init();
void turnOnLCD();
void turnOffLCD();
void checkTransferState();
void lcd_fill(unsigned char fillbyte);
void lcd_show();
void lcdbuf_clear();
void lcdbuf_plot(unsigned int x, unsigned int y);
void lcd_changeHeight();

#endif
