#include "graphics.h"
#include "epddriver.h"
#include "characters.h"

void printString(int x, int y, const char* string) {
  const char* p_string = string;
  int counter = 0;
  
  while (*p_string != 0) {
    printChar(x, y+counter*24, *p_string);
    counter++;
    p_string++;
  }
}

void printChar(int x, int y, char c) {
  switch(c) {
    case '0':
      epdSetFrameMemoryXY(ZERO, x, y, 48, 24);
      break;
    case '1':
      epdSetFrameMemoryXY(ONE, x, y, 48, 24);
      break;
    case '2':
      epdSetFrameMemoryXY(TWO, x, y, 48, 24);
      break;
    case '3':
      epdSetFrameMemoryXY(THREE, x, y, 48, 24);
      break;
    case '4':
      epdSetFrameMemoryXY(FOUR, x, y, 48, 24);
      break;
    case '5':
      epdSetFrameMemoryXY(FIVE, x, y, 48, 24);
      break;
    case '6':
      epdSetFrameMemoryXY(SIX, x, y, 48, 24);
      break;
    case '7':
      epdSetFrameMemoryXY(SEVEN, x, y, 48, 24);
      break;
    case '8':
      epdSetFrameMemoryXY(EIGHT, x, y, 48, 24);
      break;
    case '9':
      epdSetFrameMemoryXY(NINE, x, y, 48, 24);
      break;
    case ' ':
      epdSetFrameMemoryXY(SPACE, x, y, 48, 24);
      break;
    case 'D':
      epdSetFrameMemoryXY(DEGREE, x, y, 48, 24);
      break;
    case 'C':
      epdSetFrameMemoryXY(C, x, y, 48, 24);
      break;
    case '-':
      epdSetFrameMemoryXY(MINUS, x, y, 48, 24);
      break;
    case '.':
      epdSetFrameMemoryXY(DOT, x, y, 48, 24);
      break;
    case 'E':
      epdSetFrameMemoryXY(E, x, y, 48, 24);
      break;
    }
}

/*void drawVerticalLine(int x, int y, int length) {
  for(int i=0; i< length; i++) {
    
  }
}*/