#include <Audio.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce2.h>
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

AudioAmplifier           amp1;
AudioConnection c7(mixer2, amp1);

AudioConnection c8(amp1, 0, mqs, 0);
AudioConnection c9(amp1, 0, mqs, 1); // right and left channels of output are the same


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

// encoder button to start with
#define ENCODER_BUTTON_PIN 40
Button encoder_button = Button();
// start/stop button
#define GO_BUTTON_PIN 24
Button go_button = Button();
// mute button
#define MUTE_BUTTON_PIN 25
Button mute_button = Button();
// track select button
#define TRACK_BUTTON_PIN 26
Button track_button = Button();
// tempo select button
#define TEMPO_BUTTON_PIN 27
Button tempo_button = Button();
// volume select button
#define VOLUME_BUTTON_PIN 28
Button volume_button = Button();

unsigned long last_input_time = 0;
#define CURSOR_HIDE_TIME 2000

// encoder on pins 39 and 38 right now
Encoder enc = Encoder(39, 38);
long encoder_value, starting_encoder_value;
uint8_t encoded_step, encoded_track;
#define CLICKS 96
const float clicks_per_step = float(CLICKS) / float(NUM_STEPS);
const float clicks_per_track = float(CLICKS) / float(NUM_TRACKS);


// UI variables
uint8_t selected_track = 0;
uint8_t starting_track;

uint8_t bpm = 60;
uint8_t starting_bpm;

float volume = 1.0;
float starting_volume;

uint8_t selected_step = 0;

bool is_playing = true;

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

  amp1.gain(volume);

  // button setup
  encoder_button.attach(ENCODER_BUTTON_PIN, INPUT_PULLUP);
  go_button.attach(GO_BUTTON_PIN, INPUT_PULLUP);
  go_button.setPressedState(LOW);
  mute_button.attach(MUTE_BUTTON_PIN, INPUT_PULLUP);
  mute_button.setPressedState(LOW);
  track_button.attach(TRACK_BUTTON_PIN, INPUT_PULLUP);
  track_button.setPressedState(LOW);
  tempo_button.attach(TEMPO_BUTTON_PIN, INPUT_PULLUP);
  tempo_button.setPressedState(LOW);
  volume_button.attach(VOLUME_BUTTON_PIN, INPUT_PULLUP);
  volume_button.setPressedState(LOW);
}

void loop() {
  if (is_playing) {
    if (millis() > step_start_time) {
      // time to start the next step
      step_num = (step_num + 1) % NUM_STEPS;
      step_start_time += 60000 / bpm / 4.0f;  // 4 steps per beat, 60000 milliseconds per minute

      // draw for this step
      display.setStep(step_num);

      // play the notes
      for (int j = 0; j < NUM_TRACKS; j++) {
        tracks[j]->playStep(step_num);
      }
    } // END next step handler
  } // END if playing
  else {
    step_start_time = millis();
  }

  // handle button presses
  encoder_button.update();
  go_button.update();
  mute_button.update();
  track_button.update();
  tempo_button.update();
  volume_button.update();

  // track button just got pressed, record things when that happened for use below
  if (track_button.pressed()) {
    // track button just got pressed, save the starting encoder position
    starting_encoder_value = enc.read();
    starting_track = selected_track;
  }

  // tempo button just got pressed, record things when that happened for use below
  if (tempo_button.pressed()) {
    starting_encoder_value = enc.read();
    starting_bpm = bpm;
    Serial.print("Changing bpm; bpm: "); Serial.println(bpm);
  }
  if (tempo_button.released()) {
    Serial.print("Changed bpm; bpm: "); Serial.println(bpm);
  }

  // volume button just got pressed, record things when that happened for use below
  if (volume_button.pressed()) {
    starting_encoder_value = enc.read();
    starting_volume = volume;
    Serial.print("Changing volume; volume: "); Serial.println(volume);
  }
  if (volume_button.released()) {
    Serial.print("Changed volume; volume: "); Serial.println(volume);
  }

  // go button just got pressed
  if (go_button.pressed()) {
    is_playing = !is_playing;    
  }


  if (track_button.isPressed()) {
    // in track select mode
    // encoder changes the selected track
    encoder_value = enc.read() - starting_encoder_value;
    // positive modulo of the encoder value (should be between 0 and CLICKS - 1)
    encoder_value = ((encoder_value % CLICKS) + CLICKS) % CLICKS;
    encoded_track = (int) encoder_value / clicks_per_step;
    // encoded_track needs to be relative to what track we started on
    encoded_track = (((encoded_track + starting_track) % NUM_TRACKS) + NUM_TRACKS) % NUM_TRACKS;
    if (encoded_track != selected_track) {
      selected_track = encoded_track;
      display.setSelectedTrack(selected_track);
      last_input_time = millis();
    }
    display.showTracks();
  } // END track select mode
  else if (tempo_button.isPressed()) {
    // in tempo select mode
    encoder_value = enc.read() - starting_encoder_value;
    // 1-bpm change per detent on the encoder
    // TODO: is this the right speed of adjustment?
    bpm = max(min(starting_bpm + (encoder_value * 0.25), MAX_BPM), MIN_BPM);
    display.showBpm(bpm);
  } // END tempo select mode
  else if (volume_button.isPressed()) {
    // volume select mode
    encoder_value = enc.read() - starting_encoder_value;
    // 8% volume change per detent on the encoder
    // TODO: is this the right speed of adjustment?
    volume = max(min(starting_volume * pow(1 + 0.08, encoder_value), MAX_VOLUME), 0);
    amp1.gain(volume);
    display.showVolume(volume);
  } else {
    // in play mode
    // handle encoder presses
    if (encoder_button.pressed()) {
      // encoder button was just pressed, toggle this step off
      tracks[selected_track]->toggleStep(selected_step);
      last_input_time = millis();
    }
    // handle encoder to select step
    encoder_value = enc.read();
    // positive modulo of the encoder value (should be between 0 and CLICKS - 1)
    encoder_value = ((encoder_value % CLICKS) + CLICKS) % CLICKS;
    encoded_step = (int) encoder_value / clicks_per_step;
    if (encoded_step != selected_step) {
      selected_step = encoded_step;
      display.setSelectedStep(selected_step);
      last_input_time = millis();
    }
    display.show();

    // hide cursor if it has been long enough
    if (millis() - last_input_time > CURSOR_HIDE_TIME) {
      display.hideCursor();
    }

  } // END play mode
} // END loop