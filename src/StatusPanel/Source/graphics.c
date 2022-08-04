#include "graphics.h"
#include "epddriver.h"
#include "characters.h"

void printString(int x, int y, const char* string, int isOld) {
  const char* p_string = string;
  int counter = 0;
  
  while (*p_string != 0) {
    printChar(x, y+counter*24, *p_string, isOld);
    counter++;
    p_string++;
  }
}

void printChar(int x, int y, char c, int isOld) {
  switch(c) {
    case '0':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(ZERO, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(ZERO, x, y, 48, 24);
      }
      break;
    case '1':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(ONE, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(ONE, x, y, 48, 24);
      }
      break;
    case '2':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(TWO, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(TWO, x, y, 48, 24);
      }
      break;
    case '3':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(THREE, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(THREE, x, y, 48, 24);
      }
      break;
    case '4':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(FOUR, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(FOUR, x, y, 48, 24);
      }
      break;
    case '5':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(FIVE, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(FIVE, x, y, 48, 24);
      }
      break;
    case '6':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(SIX, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(SIX, x, y, 48, 24);
      }
      break;
    case '7':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(SEVEN, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(SEVEN, x, y, 48, 24);
      }
      break;
    case '8':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(EIGHT, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(EIGHT, x, y, 48, 24);
      }
      break;
    case '9':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(NINE, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(NINE, x, y, 48, 24);
      }
      break;
    case ' ':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(SPACE, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(SPACE, x, y, 48, 24);
      }
      break;
    case 'D':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(DEGREE, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(DEGREE, x, y, 48, 24);
      }
      break;
    case 'C':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(C, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(C, x, y, 48, 24);
      }
      break;
    case '-':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(MINUS, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(MINUS, x, y, 48, 24);
      }
      break;
    case '.':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(DOT, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(DOT, x, y, 48, 24);
      }
      break;
    case 'E':
      if(isOld == 1) {
        epdSetFrameMemoryXYBase(E, x, y, 48, 24);
      } else {
        epdSetFrameMemoryXY(E, x, y, 48, 24);
      }
      break;
    }
}

/*void drawVerticalLine(int x, int y, int length) {
  for(int i=0; i< length; i++) {
    
  }
}*/