// Display class for encapsulating screen tasks

#include<U8g2lib.h>

#include "constants.h"

#include"Display.h"

Display::Display(Track** tracks)
:
  _tracks(tracks),
  _last_step_num(255),  // large value to not erase anything on the first run
  _last_selected_track(0),
  _last_selected_step(0)
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
        u8g2->drawBox(i * 8 + 2, 9 * j + 2, 6, 7);
      } else {
        u8g2->drawFrame(i * 8 + 2, 9 * j + 2, 6, 7);
      }
    }
  }
  u8g2->sendBuffer();
}

void Display::displayStep(uint8_t step_num) {
  // this is the indicator line for what the current step is
  u8g2->setDrawColor(0);  // blank out last steps lines
  u8g2->drawHLine(_last_step_num * 8 + 1, 0, 8);
  u8g2->drawHLine(_last_step_num * 8 + 1, 31, 8);
  _last_step_num = step_num;
  
  u8g2->setDrawColor(1);
  u8g2->drawHLine(step_num * 8 + 1, 0, 8);
  u8g2->drawHLine(step_num * 8 + 1, 31, 8);
  u8g2->sendBuffer();
}

void Display::displaySelection(uint8_t selected_track, uint8_t selected_step) {
  // this is the indicator line for what the current step is
  u8g2->setDrawColor(0);  // blank out last steps lines
  u8g2->drawFrame(_last_selected_step * 8 + 1, _last_selected_track * 9 + 1, 8, 9);
  _last_selected_step = selected_step;
  _last_selected_track = selected_track;
  
  u8g2->setDrawColor(1);
  u8g2->drawFrame(selected_step * 8 + 1, selected_track * 9 + 1, 8, 9);
  u8g2->sendBuffer();
}
