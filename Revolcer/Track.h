// Class for a single drum instrument track
#ifndef TRACK_H
#define TRACK_H

#include <Audio.h>

#define TITLE_LEN 30

class Track {
  public:
    Track(uint8_t* pattern, AudioPlayMemory* player, const unsigned int* sample, const char* title);

    void playStep(uint8_t step_num);
    void toggleStep(uint8_t step_num);
    uint8_t* _pattern;
    char _title[TITLE_LEN];

  private:
    AudioPlayMemory* _player;
    const unsigned int* _sample;
};

#endif
