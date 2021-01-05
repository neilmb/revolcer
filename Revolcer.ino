#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// WAV files converted to code by wav2sketch
#include "AudioSampleSnare.h"        // http://www.freesound.org/people/KEVOY/sounds/82583/
#include "AudioSampleTomtom.h"       // http://www.freesound.org/people/zgump/sounds/86334/
#include "AudioSampleHihat.h"        // http://www.freesound.org/people/mhc/sounds/102790/
#include "AudioSampleKick.h"         // http://www.freesound.org/people/DWSD/sounds/171104/
#include "AudioSampleGong.h"         // http://www.freesound.org/people/juskiddink/sounds/86773/
#include "AudioSampleCashregister.h" // http://www.freesound.org/people/kiddpark/sounds/201159/

#include "constants.h"
#include "Track.h"
#include "Display.h"

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioPlayMemory    sound0;
AudioPlayMemory    sound1;
AudioPlayMemory    sound2;
AudioMixer4        mixer;
AudioOutputAnalog  dac;     // play to on-chip DAC

// Create Audio connections between the components
//
AudioConnection c1(sound0, 0, mixer, 1);
AudioConnection c2(sound1, 0, mixer, 0);
AudioConnection c3(sound2, 0, mixer, 2);
AudioConnection c4(mixer, 0, dac, 0);

uint8_t kick_pattern[NUM_STEPS] =
           {1, 1, 0, 0,
            1, 0, 0, 0,
            1, 0, 0, 0,
            1, 0, 0, 0};
Track kick(kick_pattern,
           &sound0,
           AudioSampleKick);

uint8_t snare_pattern[NUM_STEPS] =
           {0, 0, 1, 0,
            0, 0, 1, 0,
            0, 0, 1, 0,
            0, 0, 1, 1};
Track snare(snare_pattern,
            &sound1,
            AudioSampleSnare);

uint8_t hat_pattern[NUM_STEPS] =
           {0, 1, 1, 0,
            0, 1, 1, 0,
            0, 1, 1, 0,
            0, 1, 1, 0};
Track hat(hat_pattern,
            &sound2,
            AudioSampleHihat);

Track *tracks[NUM_TRACKS] = { &kick, &snare, &hat };

Display display(tracks);

// loop variables
unsigned long step_start_time = 0;
uint8_t step_num = -1;
bool is_playing = 0;

const uint8_t BPM = 60;

// one button to start with on pin 14, 20ms debounce time
Bounce button = Bounce(14, 20);
unsigned long button_start_time = 0;
unsigned long button_held_time = 0;

// UI variables
uint8_t selected_track = 0;
uint8_t selected_step = 0;

void setup() {

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(10);

  // by default the Teensy 3.1 DAC uses 3.3Vp-p output
  // if your 3.3V power has noise, switching to the
  // internal 1.2V reference can give you a clean signal
  dac.analogReference(INTERNAL);

  // reduce gains for everyone
  mixer.gain(0, 0.5);
  mixer.gain(0, 0.5);

  // button setup
  pinMode(14, INPUT_PULLUP);

  display.displaySelection(selected_track, selected_step);
}

void loop() {
    if (is_playing) {
      if (millis() > step_start_time) {
        // time to start the next step
        step_num = (step_num + 1) % NUM_STEPS;
        step_start_time += 60000 / BPM / 4.0f;  // 4 steps per beat, 60000 milliseconds per minute

        // draw for this step
        display.displayStep(step_num);

        // play the notes
        for (int j = 0; j < NUM_TRACKS; j++) {
          tracks[j]->playStep(step_num);
        }  
      }
    }

    // handle button presses
    if (button.update()) {
      if (button.fallingEdge()) {
        // the button was pressed down
        button_start_time = millis();
      } else {
        // the button was released
        button_held_time = millis() - button_start_time;
        if (button_held_time > 500) {  // TODO: make the magic number into LONG_PRESS_TIME?
          // long press stops/starts playing
          if (is_playing) {
            is_playing = 0;
          } else {
            is_playing = 1;
            step_start_time = millis();
          }
        } else {
          // short press, move the selection
          selected_step = (selected_step + 1) % NUM_STEPS;
          if (selected_step == 0) {
            // wrapped off the end, increment track
            selected_track = (selected_track + 1) % NUM_TRACKS;
          }
          display.displaySelection(selected_track, selected_step);
        }
      }
    }
}
