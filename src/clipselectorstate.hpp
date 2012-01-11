

#ifndef LUPPP_CLIPSELECTORSTATE
#define LUPPP_CLIPSELECTORSTATE

#include <vector>

enum ClipState {
  CLIP_STATE_EMPTY          = 1,
  CLIP_STATE_LOADED         = 2,
  CLIP_STATE_PLAYING        = 3,
  CLIP_STATE_PLAY_QUEUED    = 4,
  CLIP_STATE_RECORDING      = 5,
  CLIP_STATE_MASTER_TRACK   = 6,
};

// this struct should contain *all* information needed for buffer playback,
// as well as enough info for the APC to be updated.
struct ClipInfo {
  ClipInfo()
  {
    bufferID = -1;
    
    hasBuffer = false;
    
    numBeats = 8;
    speed = 1.f;
  }
  
  int bufferID;
  bool hasBuffer;
  
  int numBeats;
  float speed;
};

struct ClipSelectorState
{
  ClipSelectorState()
  {
    ID = -1;
    
    // initial values for playback block
    playing   = -1;
    queued    = -1;
    recording = -1;
    
    for(int i = 0; i < 10; i++)
      clipInfo.push_back( ClipInfo() );
  }
  
  int ID;
  
  int playing;
  int queued;
  int recording;
  
  std::list<ClipInfo> clipInfo;
};

#endif
