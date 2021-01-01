#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// WAV files converted to code by wav2sketch
#include "AudioSampleSnare.h"        // http://www.freesound.org/people/KEVOY/sounds/82583/
#include "AudioSampleTomtom.h"       // http://www.freesound.org/people/zgump/sounds/86334/
#include "AudioSampleHihat.h"        // http://www.freesound.org/people/mhc/sounds/102790/
#include "AudioSampleKick.h"         // http://www.freesound.org/people/DWSD/sounds/171104/
#include "AudioSampleGong.h"         // http://www.freesound.org/people/juskiddink/sounds/86773/
#include "AudioSampleCashregister.h" // http://www.freesound.org/people/kiddpark/sounds/201159/

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioPlayMemory    sound0;
AudioPlayMemory    sound1;
AudioMixer4        mixer;
AudioOutputAnalog  dac;     // play to both I2S audio board and on-chip DAC

// Create Audio connections between the components
//
AudioConnection c1(sound0, 0, mixer, 1);
AudioConnection c2(sound1, 0, mixer, 0);
AudioConnection c3(mixer, 0, dac, 0);

// loop variables
uint8_t pattern0[16] = {1, 1, 0, 0,
                       1, 0, 0, 0,
                       1, 0, 0, 0,
                       1, 0, 0, 0};
uint8_t pattern1[16] = {0, 0, 1, 0,
                       0, 0, 1, 0,
                       0, 0, 1, 0,
                       0, 0, 1, 1};
unsigned long step_start_time = 0;
uint8_t step_num = 0;

const uint8_t BPM = 100;

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

}

void loop() {
    if (millis() > step_start_time) {
      // time to start the next step
      if (pattern0[step_num]) {
        sound0.play(AudioSampleKick);
      }
      if (pattern1[step_num]) {
        sound1.play(AudioSampleSnare);
      }

      // reset the step counters
      step_start_time += 60000 / BPM / 4.0f;
      step_num = (step_num + 1) % 16;
    }
}
