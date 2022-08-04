#include <stdlib.h>
#include "epddriver.h"
#include "spi.h"
#include "utils.h"
#include "hal_board_cfg.h"

#define HAL_LCD_BUSY BNAME(HAL_LCD_BUSY_PORT, HAL_LCD_BUSY_PIN)

unsigned long epd_width = EPD_WIDTH;
unsigned long epd_height = EPD_HEIGHT;

static void waitUntilIdle(void);
static void delayMs(unsigned int delaytime);

void epdInit(void)
{
  SPIInit();
  epdReset();
  delayMs(100);
  waitUntilIdle();
  epdSendCommand(SW_RESET);
  waitUntilIdle();   
  
  epdSendCommand(DRIVER_OUTPUT_CONTROL);
  epdSendData(0x27);
  epdSendData(0x01);
  epdSendData(0x00);
        
  epdSendCommand(DATA_ENTRY_MODE_SETTING);
  epdSendData(0x03);
  
  epdSendCommand(DISPLAY_UPDATE_CONTROL_1);
  epdSendData(0x00);		
  epdSendData(0x80);					

  epdSetMemoryArea(0, 0, epd_width - 1, epd_height - 1);
  epdSetMemoryPointer(0, 0);
  waitUntilIdle();
}

void epdReInit(void)
{
  SPIInit();
  epdReset();
  delayMs(100);
  waitUntilIdle();
  epdSendCommand(SW_RESET);
  waitUntilIdle();   
  
  epdSendCommand(DRIVER_OUTPUT_CONTROL);
  epdSendData(0x27);
  epdSendData(0x01);
  epdSendData(0x00);
        
  epdSendCommand(DATA_ENTRY_MODE_SETTING);
  epdSendData(0x03);
  
  epdSendCommand(DISPLAY_UPDATE_CONTROL_1);
  epdSendData(0x00);		
  epdSendData(0x80);					

  epdSetLut(lut_partial);
  epdSendCommand(OTP_SELECTION_CONTROL_1); 
  epdSendData(0x00);  
  epdSendData(0x00);  
  epdSendData(0x00);  
  epdSendData(0x00); 
  epdSendData(0x00);  
  epdSendData(0x40);  
  epdSendData(0x00);  
  epdSendData(0x00);   
  epdSendData(0x00);  
  epdSendData(0x00);
 
  epdSendCommand(BORDER_WAVEFORM_CONTROL);
  epdSendData(0x80);	

  epdSendCommand(DISPLAY_UPDATE_CONTROL_2); 
  epdSendData(0xC0);   
	
  epdSendCommand(MASTER_ACTIVATION); 
  waitUntilIdle();  
	
  epdSendCommand(BORDER_WAVEFORM_CONTROL);
  epdSendData(0x80);	
  
  epdSetMemoryArea(0, 0, epd_width - 1, epd_height - 1);
  epdSetMemoryPointer(0, 0);
  waitUntilIdle();
}

void epdInitPartial(void) {
  SPIInit();
  epdReset();
  delayMs(100);
  waitUntilIdle();
  epdSetLut(lut_partial);
  epdSendCommand(OTP_SELECTION_CONTROL_1); 
  epdSendData(0x00);  
  epdSendData(0x00);  
  epdSendData(0x00);  
  epdSendData(0x00); 
  epdSendData(0x00);  
  epdSendData(0x40);  
  epdSendData(0x00);  
  epdSendData(0x00);   
  epdSendData(0x00);  
  epdSendData(0x00);
 
  epdSendCommand(BORDER_WAVEFORM_CONTROL);
  epdSendData(0x80);	

  epdSendCommand(DISPLAY_UPDATE_CONTROL_2); 
  epdSendData(0xC0);   
	
  epdSendCommand(MASTER_ACTIVATION); 
  waitUntilIdle();  
	
  epdSendCommand(BORDER_WAVEFORM_CONTROL);
  epdSendData(0x80);	
  
  epdSetMemoryArea(0, 0, epd_width - 1, epd_height - 1);
  epdSetMemoryPointer(0, 0);
  waitUntilIdle();
}

void epdSetLut(const unsigned char *lut) {       
  unsigned char count;
  epdSendCommand(WRITE_LUT_REGISTER);
  for(count=0; count<153; count++) 
    epdSendData(lut[count]); 
  waitUntilIdle();
}


void epdClearFrameMemory(void) {
    epdSetMemoryArea(0, 0, epd_width - 1, epd_height - 1);
    epdSetMemoryPointer(0, 0);
    
    epdSendCommand(WRITE_RAM);
    for (int i = 0; i < 4736; i++) {
        epdSendData(0XFF);
    }
}

void epdClearFrameMemoryBase(void) {
    epdSetMemoryArea(0, 0, epd_width - 1, epd_height - 1);
    epdSetMemoryPointer(0, 0);
    
    epdSendCommand(WRITE_RAM);
    for (int i = 0; i < 4736; i++) {
        epdSendData(0XFF);
    }
        
    epdSetMemoryArea(0, 0, epd_width - 1, epd_height - 1);
    epdSetMemoryPointer(0, 0);

    epdSendCommand(WRITE_RAM2);
    for (int i = 0; i < 4736; i++) {
        epdSendData(0XFF);
    }
}

void epdSetFrameMemory(const unsigned char* image_buffer) {
    epdSetMemoryArea(0, 0, epd_width - 1, epd_height - 1);
    epdSetMemoryPointer(0, 0);
    epdSendCommand(WRITE_RAM);
    for (int i = 0; i < 4736; i++) {
        epdSendData((uint8)(image_buffer[i]));
    }
}

void epdClearFrameMemoryXY(int x, int y, int image_width, int image_height) { 
    int x_end;
    int y_end;    

    if (
        x < 0 || image_width < 0 ||
        y < 0 || image_height < 0
    ) {
        return;
    }
    // x point must be the multiple of 8 or the last 3 bits will be ignored 
    x &= 0xF8;
    image_width &= 0xF8;
    if (x + image_width >= epd_width) {
        x_end = epd_width - 1;
    } else {
        x_end = x + image_width - 1;
    }
    if (y + image_height >= epd_height) {
        y_end = epd_height - 1;
    } else {
        y_end = y + image_height - 1;
    }
    
    epdSetMemoryArea(x, y, x_end, y_end);
    epdSetMemoryPointer(x, y);
    
    epdSendCommand(WRITE_RAM);
    for (int j = 0; j < y_end - y + 1; j++) {
        for (int i = 0; i < (x_end - x + 1) / 8; i++) {
          epdSendData(0XFF);
        }
    } 
}

void epdClearFrameMemoryXYBase(int x, int y, int image_width, int image_height) { 
    int x_end;
    int y_end;    

    if (
        x < 0 || image_width < 0 ||
        y < 0 || image_height < 0
    ) {
        return;
    }
    // x point must be the multiple of 8 or the last 3 bits will be ignored 
    x &= 0xF8;
    image_width &= 0xF8;
    if (x + image_width >= epd_width) {
        x_end = epd_width - 1;
    } else {
        x_end = x + image_width - 1;
    }
    if (y + image_height >= epd_height) {
        y_end = epd_height - 1;
    } else {
        y_end = y + image_height - 1;
    }
    
    epdSetMemoryArea(x, y, x_end, y_end);
    epdSetMemoryPointer(x, y);
    
    epdSendCommand(WRITE_RAM);
    for (int j = 0; j < y_end - y + 1; j++) {
        for (int i = 0; i < (x_end - x + 1) / 8; i++) {
          epdSendData(0XFF);
        }
    } 

    epdSetMemoryArea(x, y, x_end, y_end);
    epdSetMemoryPointer(x, y);

    epdSendCommand(WRITE_RAM2);
    for (int j = 0; j < y_end - y + 1; j++) {
        for (int i = 0; i < (x_end - x + 1) / 8; i++) {
          epdSendData(0XFF);
        }
    } 
}

void epdSetFrameMemoryXY(const unsigned char* image_buffer, int x, int y, int image_width, int image_height) { 
    int x_end;
    int y_end;    

    if (
        image_buffer == NULL ||
        x < 0 || image_width < 0 ||
        y < 0 || image_height < 0
    ) {
        return;
    }
    // x point must be the multiple of 8 or the last 3 bits will be ignored 
    x &= 0xF8;
    image_width &= 0xF8;
    if (x + image_width >= epd_width) {
        x_end = epd_width - 1;
    } else {
        x_end = x + image_width - 1;
    }
    if (y + image_height >= epd_height) {
        y_end = epd_height - 1;
    } else {
        y_end = y + image_height - 1;
    }
    
    epdSetMemoryArea(x, y, x_end, y_end);
    epdSetMemoryPointer(x, y);
     
    epdSendCommand(WRITE_RAM);
    for (int j = 0; j < y_end - y + 1; j++) {
        for (int i = 0; i < (x_end - x + 1) / 8; i++) {
            epdSendData(image_buffer[i + j * (image_width / 8)]);
        }
    }
}

void epdSetFrameMemoryXYBase(const unsigned char* image_buffer, int x, int y, int image_width, int image_height) { 
    int x_end;
    int y_end;    

    if (
        image_buffer == NULL ||
        x < 0 || image_width < 0 ||
        y < 0 || image_height < 0
    ) {
        return;
    }
    // x point must be the multiple of 8 or the last 3 bits will be ignored 
    x &= 0xF8;
    image_width &= 0xF8;
    if (x + image_width >= epd_width) {
        x_end = epd_width - 1;
    } else {
        x_end = x + image_width - 1;
    }
    if (y + image_height >= epd_height) {
        y_end = epd_height - 1;
    } else {
        y_end = y + image_height - 1;
    }
    
    epdSetMemoryArea(x, y, x_end, y_end);
    epdSetMemoryPointer(x, y);
     
    epdSendCommand(WRITE_RAM);
    for (int j = 0; j < y_end - y + 1; j++) {
        for (int i = 0; i < (x_end - x + 1) / 8; i++) {
            epdSendData(image_buffer[i + j * (image_width / 8)]);
        }
    }
    
    epdSetMemoryArea(x, y, x_end, y_end);
    epdSetMemoryPointer(x, y);
 
    epdSendCommand(WRITE_RAM2);
    for (int j = 0; j < y_end - y + 1; j++) {
        for (int i = 0; i < (x_end - x + 1) / 8; i++) {
            epdSendData(image_buffer[i + j * (image_width / 8)]);
        }
    }
}

void epdSetFrameMemoryBase(const unsigned char* image_buffer) {
    epdSetMemoryArea(0, 0, epd_width - 1, epd_height - 1);
    epdSetMemoryPointer(0, 0);
    epdSendCommand(WRITE_RAM);
    for (int i = 0; i < 4736; i++) {
        epdSendData((uint8)(image_buffer[i]));
    }
    
    epdSetMemoryArea(0, 0, epd_width - 1, epd_height - 1);
    epdSetMemoryPointer(0, 0);

    epdSendCommand(WRITE_RAM2);
    for (int i = 0; i < 4736; i++) {
        epdSendData((uint8)(image_buffer[i]));
    }
}

void epdDisplayFrame(void) {
  epdSendCommand(DISPLAY_UPDATE_CONTROL_2);
  epdSendData(0xF7);
  epdSendCommand(MASTER_ACTIVATION);
  waitUntilIdle();
}

void epdDisplayFramePartial(void) {
  epdSendCommand(DISPLAY_UPDATE_CONTROL_2);
  epdSendData(0x0F);
  epdSendCommand(MASTER_ACTIVATION);
  waitUntilIdle();
}

void epdSetMemoryPointer(int x, int y) {
    epdSendCommand(SET_RAM_X_ADDRESS_COUNTER);
    epdSendData((x >> 3) & 0xFF);
    
    epdSendCommand(SET_RAM_Y_ADDRESS_COUNTER);
    epdSendData(y & 0xFF);
    epdSendData((y >> 8) & 0xFF);
}

void epdSetMemoryArea(int x_start, int y_start, int x_end, int y_end) {
    epdSendCommand(SET_RAM_X_ADDRESS_START_END_POSITION);
    epdSendData((x_start >> 3) & 0xFF);
    epdSendData((x_end >> 3) & 0xFF);
    
    epdSendCommand(SET_RAM_Y_ADDRESS_START_END_POSITION);
    epdSendData(y_start & 0xFF);
    epdSendData((y_start >> 8) & 0xFF);
    epdSendData(y_end & 0xFF);
    epdSendData((y_end >> 8) & 0xFF);
}

void epdSendCommand(unsigned char command) {
    HalLcd_HW_Control(command);
}

void epdSendData(unsigned char data) {
    HalLcd_HW_Write(data);
}

void epdReset(void) {
  HalLcd_HW_Init();   
}

void delayMs(unsigned int delaytime) {
  while(delaytime--)
  {
    uint16 microSecs = 1000;
    while(microSecs--)
    {
      asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    }
  }
}

void waitUntilIdle(void) {
  uint8 error_time = 25; // over 2.5 sec return
     while(HAL_LCD_BUSY == 1) {      //LOW: idle, HIGH: busy
        delayMs(100);
        error_time = error_time - 1;
        if (error_time == 0){    
          epdReset();
          return;
        }
    } 
}

void epdSleep(void) {
    epdSendCommand(DEEP_SLEEP_MODE);
    epdSendData(0x01);
    delayMs(100);
}

const unsigned char lut_partial[159] =
{
0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x80,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x40,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0A,0x0,0x0,0x0,0x0,0x0,0x2,  
0x1,0x0,0x0,0x0,0x0,0x0,0x0,
0x1,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x22,0x22,0x22,0x22,0x22,0x22,0x0,0x0,0x0,
0x22,0x17,0x41,0xB0,0x32,0x36,
};

