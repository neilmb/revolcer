#include <Audio.h>
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
AudioOutputMQS  mqs;     // play to on-chip DAC

// Create Audio connections between the components
//
AudioConnection c1(sound0, 0, mixer1, 0);
AudioConnection c2(sound1, 0, mixer1, 1);
AudioConnection c3(sound2, 0, mixer1, 2);
AudioConnection c4(sound3, 0, mixer1, 3);

AudioConnection c5(sound4, 0, mixer2, 0);
AudioConnection c6(mixer1, 0, mixer2, 1);

AudioConnection c7(mixer2, 0, mqs, 0);
AudioConnection c8(mixer2, 0, mqs, 1); // right and left channels of output are the same

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

// one button to start with on pin 40, 15ms debounce time
#define ENCODER_BUTTON_PIN 40
Bounce button = Bounce(40, 15);
unsigned long button_start_time = 0;
unsigned long button_held_time = 0;
#define LONG_PRESS_TIME 400
bool button_pressed = false;
bool ignore_rising_edge = true;

// encoder on pins 39 and 38 right now
Encoder enc = Encoder(39, 38);
long encoder_value;
uint8_t encoded_step;
#define CLICKS 96
const float clicks_per_step = float(CLICKS) / float(NUM_STEPS);

// UI variables
uint8_t selected_track = 0;
uint8_t selected_step = 0;

void setup() {

  Serial.begin(9600);
  Serial.println("Beginning Revolcer...");

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(10);

  // reduce gains for everyone
  mixer1.gain(0, 0.25);
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.25);
  mixer1.gain(3, 0.5);  // close hi-hat needs to be louder

  mixer2.gain(0, 0.25);

  // button setup
  pinMode(ENCODER_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  if (millis() > step_start_time) {
    // time to start the next step
    step_num = (step_num + 1) % NUM_STEPS;
    step_start_time += 60000 / BPM / 4.0f;  // 4 steps per beat, 60000 milliseconds per minute

    // draw for this step
    display.setStep(step_num);

    // play the notes
    for (int j = 0; j < NUM_TRACKS; j++) {
      tracks[j]->playStep(step_num);
    }
  } // END next step handler

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
      } else {
        // long press changes the selected track
        // we might already have acted on this button press
        if (!ignore_rising_edge) {
          selected_track = (selected_track + 1) % NUM_TRACKS;
          display.setSelectedTrack(selected_track);
        }
      }
    }
  } // END button presses
  
  // even if the button didn't change, if it has been pressed more than LONG_PRESS_TIME
  // we should do the long press action now and mark to ignore the next rising edge.
  if (button_pressed) {
    if (millis() - button_start_time >= LONG_PRESS_TIME) {
      ignore_rising_edge = true;
      button_pressed = false;
      selected_track = (selected_track + 1) % NUM_TRACKS;
    display.setSelectedTrack(selected_track);
    }
  }

  // handle encoder
  encoder_value = enc.read();
  // positive modulo of the encoder value (should be between 0 and CLICKS - 1)
  encoder_value = ((encoder_value % CLICKS) + CLICKS) % CLICKS;
  encoded_step = (int) encoder_value / clicks_per_step;
  if (encoded_step != selected_step) {
    Serial.print("step changed. encoder_value: "); Serial.println(encoder_value);
    Serial.print("clicks_per_step: "); Serial.println(clicks_per_step);
    Serial.print("encoded_step: "); Serial.print(encoded_step); Serial.println(selected_step);
    selected_step = encoded_step;
    display.setSelectedStep(selected_step);
  }
  display.show();
} // END loop
