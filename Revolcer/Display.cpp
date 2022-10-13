// Display class for encapsulating screen tasks

#include<U8g2lib.h>

#include "constants.h"

#include"Display.h"

Display::Display(Track** tracks)
:
  _tracks(tracks),
  _last_step_num(255),  // large value to not erase anything on the first run
  _last_selected_step(0)
{
  u8g2 = new U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);

  // setup display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  u8g2->begin();

  u8g2->clearBuffer();
  displayTrack(0);
}

void Display::displayTrack(uint8_t track_num) {
  u8g2->setDrawColor(1);
  u8g2->setFont(u8g2_font_profont12_mr);
  u8g2->drawGlyph(5, 8, 49 + track_num);  // ASCII 48 is 0, count from 1 like humans
  u8g2->drawStr(13, 8, _tracks[track_num]->_title);

  for (int i = 0; i < NUM_STEPS / 2; i++) {
    // draw the first row
    if (_tracks[track_num]->_pattern[i]) {
      u8g2->drawBox(i * 16 + 2, 11, 13, 8);
    } else {
      u8g2->setDrawColor(0); u8g2->drawBox(i * 16 + 2, 11, 13, 8);
      u8g2->setDrawColor(1); u8g2->drawFrame(i * 16 + 2, 11, 13, 8);
    }
    // draw the second row
    if (_tracks[track_num]->_pattern[i + NUM_STEPS / 2]) {
      u8g2->drawBox(i * 16 + 2, 22, 13, 8);
    } else {
      u8g2->setDrawColor(0); u8g2->drawBox(i * 16 + 2, 22, 13, 8);
      u8g2->setDrawColor(1); u8g2->drawFrame(i * 16 + 2, 22, 13, 8);
    }
  }
  u8g2->sendBuffer();
}

void Display::displayStep(uint8_t step_num) {
  // this is the indicator line for what the current step is
  u8g2->setDrawColor(0);  // blank out last steps lines
  if (_last_step_num < NUM_STEPS / 2) {
    u8g2->drawHLine(_last_step_num * 16 + 1, 9, 16);
  } else {
    u8g2->drawHLine((_last_step_num - NUM_STEPS / 2) * 16 + 1, 31, 16);
  }_last_step_num = step_num;
  
  u8g2->setDrawColor(1);
  if (step_num < NUM_STEPS / 2) {
    u8g2->drawHLine(step_num * 16 + 1, 9, 16);
  } else {
    u8g2->drawHLine((step_num - NUM_STEPS / 2) * 16 + 1, 31, 16);
  }
  u8g2->sendBuffer();
}

void Display::displaySelection(uint8_t selected_track, uint8_t selected_step) {
  // draw the selected track boxes
  displayTrack(selected_track);
  
  // this is the indicator frame for what the selected step is
  u8g2->setDrawColor(0);  // blank out last step's frame
  if (_last_selected_step < NUM_STEPS / 2) {
    u8g2->drawFrame(_last_selected_step * 16 + 1, 10, 15, 10);
  } else {
    u8g2->drawFrame((_last_selected_step - NUM_STEPS / 2) * 16 + 1, 21, 15, 10);  
  }
  _last_selected_step = selected_step;
  
  u8g2->setDrawColor(1);
  if (selected_step < NUM_STEPS / 2) {
    u8g2->drawFrame(selected_step * 16 + 1, 10, 15, 10);
  } else {
    u8g2->drawFrame((selected_step - NUM_STEPS / 2) * 16 + 1, 21, 15, 10);  
  }
  u8g2->sendBuffer();
}
