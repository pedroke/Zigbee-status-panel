#ifndef SPI_H
#define SPI_H
#include "hal_types.h"

/*
  LCD pins
  //control
  P0.0 - LCD_MODE (DC EPD)
  P1.1 - LCD_FLASH_RESET (RST EPD)
  P0.7 - LCD_CS (CS EPD)
  P0.4 - LCD_BUSY (BUSY EPD)
  //spi
  P1.5 - CLK
  P1.6 - MOSI
  P1.7 - MISO
*/

/* LCD Control lines */
#ifndef HAL_LCD_MODE_PORT
#define HAL_LCD_MODE_PORT 0
#endif
#ifndef HAL_LCD_MODE_PIN
#define HAL_LCD_MODE_PIN  0
#endif

#ifndef HAL_LCD_RESET_PORT
#define HAL_LCD_RESET_PORT 1
#endif
#ifndef HAL_LCD_RESET_PIN
#define HAL_LCD_RESET_PIN  1
#endif

#ifndef HAL_LCD_CS_PORT
#define HAL_LCD_CS_PORT 0
#endif
#ifndef HAL_LCD_CS_PIN
#define HAL_LCD_CS_PIN  7
#endif

#ifndef HAL_LCD_BUSY_PORT
#define HAL_LCD_BUSY_PORT 0
#endif
#ifndef HAL_LCD_BUSY_PIN
#define HAL_LCD_BUSY_PIN  4
#endif

/* LCD SPI lines */
#define HAL_LCD_CLK_PORT 1
#define HAL_LCD_CLK_PIN  5

#define HAL_LCD_MOSI_PORT 1
#define HAL_LCD_MOSI_PIN  6

#define HAL_LCD_MISO_PORT 1
#define HAL_LCD_MISO_PIN  7

void spi_ConfigIO(void);
void spi_ConfigSPI(void);
void spi_ConfigGP(void);
void spi_HW_WaitUs(uint16 i);

extern void SPIInit(void);
extern void HalLcd_HW_Init(void);
extern void HalLcd_HW_Control(uint8 cmd);
extern void HalLcd_HW_Write(uint8 data);

#endif