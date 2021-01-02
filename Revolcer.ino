#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <U8g2lib.h>

// WAV files converted to code by wav2sketch
#include "AudioSampleSnare.h"        // http://www.freesound.org/people/KEVOY/sounds/82583/
#include "AudioSampleTomtom.h"       // http://www.freesound.org/people/zgump/sounds/86334/
#include "AudioSampleHihat.h"        // http://www.freesound.org/people/mhc/sounds/102790/
#include "AudioSampleKick.h"         // http://www.freesound.org/people/DWSD/sounds/171104/
#include "AudioSampleGong.h"         // http://www.freesound.org/people/juskiddink/sounds/86773/
#include "AudioSampleCashregister.h" // http://www.freesound.org/people/kiddpark/sounds/201159/

#include "Track.h"

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioPlayMemory    sound0;
AudioPlayMemory    sound1;
AudioMixer4        mixer;
AudioOutputAnalog  dac;     // play to on-chip DAC

// Create Audio connections between the components
//
AudioConnection c1(sound0, 0, mixer, 1);
AudioConnection c2(sound1, 0, mixer, 0);
AudioConnection c3(mixer, 0, dac, 0);

uint8_t kick_pattern[16] = 
           {1, 1, 0, 0,
            1, 0, 0, 0,
            1, 0, 0, 0,
            1, 0, 0, 0};
Track kick(kick_pattern,
           &sound0,
           AudioSampleKick);

uint8_t snare_pattern[16] = 
           {0, 0, 1, 0,
            0, 0, 1, 0,
            0, 0, 1, 0,
            0, 0, 1, 1};
Track snare(snare_pattern,
            &sound1,
            AudioSampleSnare);


// loop variables
unsigned long step_start_time = 0;
uint8_t step_num = -1;
uint8_t last_step_num;

const uint8_t BPM = 60;

// OLED display
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);

void setup() {
  // setup display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  u8g2.begin();

  u8g2.clearBuffer();
  // paint the track patterns
  for (int i = 0; i < 16; i++) {
    if (kick._pattern[i]) {
      u8g2.drawBox(i * 8 + 2, 2, 6, 12);
    } else {
      u8g2.drawFrame(i * 8 + 2, 2, 6, 12);
    }
    if (snare._pattern[i]) {
      u8g2.drawBox(i * 8 + 2, 16, 6, 12);
    } else {
      u8g2.drawFrame(i * 8 + 2, 16, 6, 12);
    }

  }
  u8g2.sendBuffer();

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
      last_step_num =  step_num;
      step_num = (step_num + 1) % 16;
      step_start_time += 60000 / BPM / 4.0f;

      // current step display
      u8g2.setDrawColor(0);  // blank out last steps lines
      u8g2.drawHLine(last_step_num * 8 + 1, 0, 7);
      u8g2.drawHLine(last_step_num * 8 + 1, 31, 7);

      u8g2.setDrawColor(1);
      u8g2.drawHLine(step_num * 8 + 1, 0, 7);
      u8g2.drawHLine(step_num * 8 + 1, 31, 7);
      u8g2.sendBuffer();

      // play the notes
      kick.playStep(step_num);
      snare.playStep(step_num);
    }
}
