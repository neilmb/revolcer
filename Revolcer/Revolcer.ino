#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>
#include <Encoder.h>

// WAV files converted to code by wav2sketch
#include "AudioSampleSnare.h"
#include "AudioSampleTom.h"
#include "AudioSampleClosedhat.h"
#include "AudioSampleOpenhat.h"
#include "AudioSampleKick.h"

#include "constants.h"
#include "Track.h"
#include "Display.h"

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioPlayMemory    sound0;
AudioPlayMemory    sound1;
AudioPlayMemory    sound2;
AudioPlayMemory    sound3;
AudioPlayMemory    sound4;
AudioMixer4        mixer1;
AudioMixer4        mixer2;
AudioOutputAnalog  dac;     // play to on-chip DAC

// Create Audio connections between the components
//
AudioConnection c1(sound0, 0, mixer1, 0);
AudioConnection c2(sound1, 0, mixer1, 1);
AudioConnection c3(sound2, 0, mixer1, 2);
AudioConnection c4(sound3, 0, mixer1, 3);

AudioConnection c5(sound4, 0, mixer2, 0);
AudioConnection c6(mixer1, 0, mixer2, 1);

AudioConnection c7(mixer2, 0, dac, 0);

uint8_t kick_pattern[NUM_STEPS] =
           {1, 1, 0, 0,
            1, 0, 0, 0,
            1, 0, 0, 0,
            1, 0, 0, 0};
Track kick(kick_pattern,
           &sound0,
           AudioSampleKick,
           "Kick");

uint8_t snare_pattern[NUM_STEPS] =
           {0, 0, 1, 0,
            0, 0, 1, 0,
            0, 0, 1, 0,
            0, 0, 1, 1};
Track snare(snare_pattern,
            &sound1,
            AudioSampleSnare,
            "Snare");

uint8_t tom_pattern[NUM_STEPS] =
           {0, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0};
Track tom(tom_pattern,
            &sound2,
            AudioSampleTom,
            "Tom");

uint8_t closedhat_pattern[NUM_STEPS] =
           {0, 1, 1, 0,
            0, 1, 1, 0,
            0, 1, 1, 0,
            0, 1, 1, 0};
Track closedhat(closedhat_pattern,
            &sound3,
            AudioSampleClosedhat,
            "ClosedHat");

uint8_t openhat_pattern[NUM_STEPS] =
           {0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0};
Track openhat(openhat_pattern,
            &sound3,
            AudioSampleOpenhat,
            "OpenHat");

Track *tracks[NUM_TRACKS] = { &kick, &snare, &tom, &closedhat, &openhat };

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
  mixer1.gain(0, 0.25);
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.25);
  mixer1.gain(3, 0.5);  // close hi-hat needs to be louder

  mixer2.gain(0, 0.25);

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
