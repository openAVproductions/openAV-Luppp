

#ifndef LUPPP_CLIPSELECTORSTATE
#define LUPPP_CLIPSELECTORSTATE

#include <vector>

enum ClipState {
  CLIP_STATE_EMPTY          = 1,
  CLIP_STATE_LOADED         = 2,
  CLIP_STATE_PLAYING        = 3,
  CLIP_STATE_QUEUED         = 4,
  CLIP_STATE_RECORDING      = 5,
};

struct ClipInfo {
  ClipInfo()
  {
    state = CLIP_STATE_EMPTY;
    bufferID = -1;
    numBeats = 8;
    speed = 1.f;
  }
  ClipState state;
  int bufferID;
  int numBeats;
  float speed;
};

struct ClipSelectorState
{
  ClipSelectorState()
  {
    ID = -1;
    playing = -1;
  }
  int ID;
  int playing;
  
  std::list<ClipInfo> clipInfo;
};

#endif
