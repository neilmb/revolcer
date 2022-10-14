// Display class for encapsulating screen tasks

#include <Adafruit_NeoPixel.h>

#include "Track.h"

#ifndef DISPLAY_H
#define DISPLAY_H

class Display {
  public:
    Display(Track **tracks);
    void setStep(uint8_t step_num);
    void setSelectedStep(uint8_t selected_step);
    void setSelectedTrack(uint8_t selected_track);
    void show();

  private:
    Track **_tracks;
    uint8_t _step_num, _selected_step, _selected_track;  
    Adafruit_NeoPixel *strip;
};


#endif
