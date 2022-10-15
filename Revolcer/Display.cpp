// Display class for encapsulating screen tasks

#include <Adafruit_NeoPixel.h>

#include "constants.h"

#include"Display.h"

Display::Display(Track** tracks)
:
  _tracks(tracks),
  _step_num(255),  // large value to not erase anything on the first run
  _selected_step(0),
  _selected_track(0),
  _show_cursor(true)
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
  // when step changes, show the cursor again
  _show_cursor = true;
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

  // draw the cursor as blue
  if (_show_cursor) {
    strip->setPixelColor(_selected_step, 0, 0, 255);
  }

  strip->show();
}

void Display::showTracks() {
  // start all black
  strip->clear();
  for (int i = 0; i < NUM_TRACKS; i++) {
    if (i == _selected_track) {
      // selected track is blue
      strip->setPixelColor(i, 0, 0, 255);
    } else {
      // other tracks are green?
      strip->setPixelColor(i, 0, 255, 0);
    }
  }
  strip->show();
}

void Display::showBpm(uint8_t bpm) {
  //start all black
  strip->clear();
  for (int i = 0; i < NUM_STEPS; i++) {
    // this fills pixels non-linearly by the time length of a beat rather than the BPM bnumber
    if (1.0 / i > 1.0 / floor((bpm / float(MAX_BPM)) * NUM_STEPS)) {
      strip->setPixelColor(i, 0, 255, 0);
    }
  }
  strip->show();
}

void Display::showVolume(float volume) {
  // start all black
  strip->clear();
  for (int i = 0; i < NUM_STEPS; i++) {
    // TODO: should we display this linearly?
    if (i < floor(volume / MAX_VOLUME * NUM_STEPS)) {
      strip->setPixelColor(i, 0, 255, 0);
    }   
  }
  strip->show();
}

void Display::hideCursor() {
  _show_cursor = false;
}