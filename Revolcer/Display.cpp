// Display class for encapsulating screen tasks

#include <Adafruit_NeoPixel.h>

#include "constants.h"

#include"Display.h"

Display::Display(Track** tracks)
:
  _tracks(tracks),
  _step_num(255),  // large value to not erase anything on the first run
  _selected_step(0),
  _selected_track(0)
{
  strip = new Adafruit_NeoPixel(LED_COUNT, LED_PIN,  NEO_GRB + NEO_KHZ800);

  // setup display
  strip->begin();
  strip->setBrightness(30);
  show();
}

void Display::setStep(uint8_t step_num) {
  _step_num = step_num;
}

void Display::setSelectedStep(uint8_t selected_step) {
  _selected_step = selected_step;
} 

void Display::setSelectedTrack(uint8_t selected_track) {
  _selected_track = selected_track;
}

void Display::show() {
  // start all black
  strip->clear();
  // Color the lights that are on for this track
  for (int i = 0; i < NUM_STEPS; i++) {
    // draw the first row
    if (_tracks[_selected_track]->_pattern[i]) {
      strip->setPixelColor(i, 128, 0, 0);
    } else {
      strip->setPixelColor(i, 0, 0, 0);
    }
  }
  // turn the current step white
  // TODO: pick a fancier color here
  strip->setPixelColor(_step_num, 255, 255, 255);

  // draw the cursor as light blue
  strip->setPixelColor(_selected_step, 164, 219, 232);
 
  strip->show();
}