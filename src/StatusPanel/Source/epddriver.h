#ifndef EPDDRIVER_H
#define EPDDRIVER_H

#include "hal_types.h"

// Display resolution
#define EPD_WIDTH       128
#define EPD_HEIGHT      296

// EPD2IN9 commands
#define DRIVER_OUTPUT_CONTROL                       0x01
#define BOOSTER_SOFT_START_CONTROL                  0x0C
#define GATE_SCAN_START_POSITION                    0x0F
#define DEEP_SLEEP_MODE                             0x10
#define DATA_ENTRY_MODE_SETTING                     0x11
#define SW_RESET                                    0x12
#define TEMPERATURE_SENSOR_CONTROL                  0x1A
#define MASTER_ACTIVATION                           0x20
#define DISPLAY_UPDATE_CONTROL_1                    0x21
#define DISPLAY_UPDATE_CONTROL_2                    0x22
#define WRITE_RAM                                   0x24
#define WRITE_RAM2                                  0x26
#define WRITE_VCOM_REGISTER                         0x2C
#define WRITE_LUT_REGISTER                          0x32
#define OTP_SELECTION_CONTROL_1                     0x37
#define SET_DUMMY_LINE_PERIOD                       0x3A
#define SET_GATE_TIME                               0x3B
#define BORDER_WAVEFORM_CONTROL                     0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION        0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION        0x45
#define SET_RAM_X_ADDRESS_COUNTER                   0x4E
#define SET_RAM_Y_ADDRESS_COUNTER                   0x4F
#define TERMINATE_FRAME_READ_WRITE                  0xFF

extern const unsigned char lut_partial[];

extern    unsigned long epd_width;
extern    unsigned long epd_height;

extern    void epdInit(void);
extern    void epdInitPartial(void);
extern    void epdReInit(void);

extern    void epdSendCommand(unsigned char command);
extern    void epdSendData(unsigned char data);
extern    void WaitUntilIdle(void);
extern    void epdReset(void);

extern    void epdSetFrameMemoryXY(const unsigned char* image_buffer,int x, int y, int image_width, int image_height);
extern    void epdSetFrameMemoryXYBase(const unsigned char* image_buffer, int x, int y, int image_width, int image_height);

extern    void epdSetFrameMemory(const unsigned char* image_buffer);
extern    void epdSetFrameMemoryBase(const unsigned char* image_buffer);

extern    void epdClearFrameMemory(void);
extern    void epdClearFrameMemoryBase(void);
extern    void epdClearFrameMemoryXY(int x, int y, int image_width, int image_height);
extern    void epdClearFrameMemoryXYBase(int x, int y, int image_width, int image_height);

extern    void epdDisplayFrame(void);
extern    void epdDisplayFramePartial(void);

extern    void epdSleep(void);

extern    void epdSetLut(const unsigned char* lut);
extern    void epdSetMemoryArea(int x_start, int y_start, int x_end, int y_end);
extern    void epdSetMemoryPointer(int x, int y);

#endif /* EPDDRIVER_H */