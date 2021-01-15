// Class for a single drum instrument track

#include <Audio.h>
#include "Track.h"

Track::Track(uint8_t* pattern, AudioPlayMemory* player, const unsigned int* sample, const char *title) {
  _pattern = pattern;
  _player = player;
  _sample = sample;
  snprintf(_title, TITLE_LEN, "%-30s", title);
}

void Track::playStep(uint8_t step_num) {
  if (_pattern[step_num]) {
    _player->play(_sample);  
  }
}

void Track::toggleStep(uint8_t step_num) {
  // toggle the value of the pattern at one step
  if (_pattern[step_num]) {
    _pattern[step_num] = 0;
  } else {
    _pattern[step_num] = 1;
  }
}
