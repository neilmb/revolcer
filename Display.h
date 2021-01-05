// Display class for encapsulating screen tasks

#include <U8g2lib.h>

#include "Track.h"

#ifndef DISPLAY_H
#define DISPLAY_H

class Display {
  public:
    Display(Track **tracks);
    void displayTrack(uint8_t track_num);
    void displayStep(uint8_t step_num);
    void displaySelection(uint8_t selected_step, uint8_t selected_track);

  private:
    Track **_tracks;
    uint8_t _last_step_num, _last_selected_track, _last_selected_step;
    // OLED display
    U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C *u8g2;
};


#endif
