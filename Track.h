// Class for a single drum instrument track
#ifndef Track_h
#define Track_h

#include <Audio.h>

class Track {
  public:
    Track(uint8_t* pattern, AudioPlayMemory* player, const unsigned int* sample);

    void playStep(uint8_t step_num);
    void toggleStep(uint8_t step_num);
    uint8_t* _pattern;

  private:
    AudioPlayMemory* _player;
    const unsigned int* _sample;
};

#endif
