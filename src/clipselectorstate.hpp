

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

struct ClipInfo {
  ClipInfo()
  {
    state = CLIP_STATE_EMPTY;
    bufferID = -1;
  }
  ClipState state;
  int bufferID;
};

struct ClipSelectorState
{
  int ID;
  
  std::vector<ClipInfo> clipInfo;
};

#endif
