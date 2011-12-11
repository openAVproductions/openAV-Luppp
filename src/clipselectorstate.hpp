

#ifndef LUPPP_CLIPSELECTORSTATE
#define LUPPP_CLIPSELECTORSTATE

#include <vector>

enum ClipState {
  CLIP_STATE_EMPTY = 0,
  CLIP_STATE_LOADED,
  CLIP_STATE_PLAYING,
  CLIP_STATE_QUEUED,
  CLIP_STATE_RECORDING,
};

struct ClipSelectorState
{
  int ID;
  
  std::vector<ClipState> clipStates;
};

#endif
