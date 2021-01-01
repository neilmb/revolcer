// Class for a single drum instrument track

#include <Audio.h>
#include "Track.h"

Track::Track(uint8_t* pattern, AudioPlayMemory* player, const unsigned int* sample) {
  _pattern = pattern;
  _player = player;
  _sample = sample;
}

void Track::playStep(uint8_t step_num) {
  if (_pattern[step_num]) {
    _player->play(_sample);  
  }
}
