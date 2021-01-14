#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>
#include <Encoder.h>

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
AudioConnection c1(sound0, 0, mixer, 0);
AudioConnection c2(sound1, 0, mixer, 1);
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

const uint8_t BPM = 60;

// one button to start with on pin 14, 20ms debounce time
Bounce button = Bounce(14, 20);
unsigned long button_start_time = 0;
unsigned long button_held_time = 0;
#define LONG_PRESS_TIME 400
bool button_pressed = false;
bool ignore_rising_edge = true;

Encoder enc = Encoder(9, 12);
long previous_encoder_value = -999;
long encoder_value;
#define CLICKS 60
const float clicks_per_step = (float)CLICKS / (float)NUM_STEPS;

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
  mixer.gain(1, 0.5);
  mixer.gain(2, 0.5);

  // button setup
  pinMode(14, INPUT_PULLUP);

  display.displaySelection(selected_track, selected_step);
}

void loop() {
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

    // handle button presses
    if (button.update()) {
      if (button.fallingEdge()) {
        // the button was pressed down
        button_start_time = millis();
        button_pressed = true;
        ignore_rising_edge = false;
      } else {
        // the button was released
        button_pressed = false;
        button_held_time = millis() - button_start_time;
        if (button_held_time < LONG_PRESS_TIME) {
          // short press toggles the pattern at this step
          tracks[selected_track]->toggleStep(selected_step);
          display.displayTrack(selected_track);
        } else {
          // long press changes the selected track
          // we might already have acted on this button press
          if (!ignore_rising_edge) {
            selected_track = (selected_track + 1) % NUM_TRACKS;
            display.displaySelection(selected_track, selected_step);
          }
        }
      }
    }
    // even if the button didn't change, if it has been pressed more than LONG_PRESS_TIME
    // we should do the long press action now and mark to ignore the next rising edge.
    if (button_pressed) {
      if (millis() - button_start_time >= LONG_PRESS_TIME) {
        ignore_rising_edge = true;
        button_pressed = false;
        selected_track = (selected_track + 1) % NUM_TRACKS;
        display.displaySelection(selected_track, selected_step);
      }
    }

    // handle encoder
    encoder_value = enc.read();
    if (encoder_value != previous_encoder_value) {
      previous_encoder_value = encoder_value;
      // selected step is just the fraction of one rotation devoted to each step
      // 60 counts per rotation
      encoder_value = encoder_value % CLICKS;
      if (encoder_value < 0) {
        encoder_value += CLICKS;
      }
      selected_step = (int) (encoder_value / clicks_per_step);  // 3.75 clicks per step for 16 steps
      display.displaySelection(selected_track, selected_step);
    }
}
