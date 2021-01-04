// Display class for encapsulating screen tasks

#include<U8g2lib.h>

#include "constants.h"

#include"Display.h"

Display::Display(Track** tracks)
:
  _tracks(tracks),
  _last_step_num(255)  // large value to not erase anything on the first run
{
  u8g2 = new U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);

  // setup display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  u8g2->begin();

  u8g2->clearBuffer();
  // paint the track patterns
  for (int i = 0; i < NUM_STEPS; i++) {
    for (int j = 0; j < NUM_TRACKS; j++) {
      if (tracks[j]->_pattern[i]) {
        u8g2->drawBox(i * 8 + 2, 14 * j + 2, 6, 12);
      } else {
        u8g2->drawFrame(i * 8 + 2, 14 * j + 2, 6, 12);
      }
    }
  }
  u8g2->sendBuffer();
}

void Display::displayStep(uint8_t step_num) {
  // this is the indicator line for what the current step is
  u8g2->setDrawColor(0);  // blank out last steps lines
  u8g2->drawHLine(_last_step_num * 8 + 1, 0, 7);
  u8g2->drawHLine(_last_step_num * 8 + 1, 31, 7);
  _last_step_num = step_num;
  
  u8g2->setDrawColor(1);
  u8g2->drawHLine(step_num * 8 + 1, 0, 7);
  u8g2->drawHLine(step_num * 8 + 1, 31, 7);
  u8g2->sendBuffer();
}
